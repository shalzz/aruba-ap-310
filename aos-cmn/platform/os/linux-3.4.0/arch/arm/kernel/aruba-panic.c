// vim:set ts=4 sw=4 expandtab:
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/notifier.h>
#include <linux/mtd/mtd.h>
#include <linux/interrupt.h>
#include <sap/flash/ap60_flash_common.h>
#include <sap/flash/fl_cache.h>
#include <linux/aruba-ap.h>
#include <cmn/oem/aphw.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#include <linux/export.h>
#endif

#ifdef CONFIG_OCTOMORE
extern int aruba_spi_flash_read(char *, loff_t, loff_t, size_t);
#endif
/*
 * NOTE: This is a new version of this file, which is only tied
 * to the ARM kernel at present.
 */

apflash_t *aruba_get_flash_info_by_name(char *section);

static void
apfc_compute_digest(__u8 *digest, __u8 *buf, int size)
{
    __u32 sum;
    __u32 *p;
    int i;
    int nwords = size / sizeof(__u32);

    p = (__u32 *) buf;
    sum = 0;
    for (i = 0; i < nwords; i++) {
        sum += *p++;
    }
    memset(digest, 0, APFC_DIGEST_SIZE);
    memcpy(digest, &sum, sizeof(sum));
}

void
aruba_save_crash_reason(char *buf)
{
    static int entries = 0;
    loff_t cache_addr;
    void *cache_base;
    __u32 cache_size;
    int ret;
    apfc_hdr_t *hdr;
    static __u8 cache_buf[131072];
    __u8 odigest[APFC_DIGEST_SIZE];
    __u8 ndigest[APFC_DIGEST_SIZE];
    apfc_entry_hdr_t *entry;
    int msglen = strlen(buf);
    struct timeval tv;
    int spi = 0;
    unsigned erase_size;
    apflash_t *fl, *flall;
    apfc_entry_hdr_t *eh = NULL;
    apfc_entry_hdr_t *endp = NULL; 
#ifdef powerpc
    extern void *aruba_panic_flash_virt;
#endif

    if (!ap_panic_dump/*ap_panic_dump_write || !ap_panic_dump_erase*/) {
        /* in case this isn't set up yet */
        return;
    }

    /*
     * Prevent repeated calls in case of additional panics/page faults.
     */
    if (++entries > 1) {
        return;
    }

    fl = aruba_get_flash_info_by_name("cache");
    if (!fl) {
        return;
    }

    flall = aruba_get_flash_info_by_name("all");
    if (!flall) {
        return;
    }

    erase_size = fl->af_size;
    cache_size = fl->af_size;
    cache_addr = fl->af_offset;
#ifndef powerpc
    cache_base = (void *)flall->af_start;
#else
    cache_base = aruba_panic_flash_virt;
#endif

#ifdef CONFIG_OCTOMORE
    if (aruba_spi_flash_read(cache_buf, cache_addr, cache_addr, cache_size)) {
        printk("%s: read failed \n", __FUNCTION__);
        return;
    }
#else
    memcpy(cache_buf, cache_addr + cache_base, cache_size);
#endif
    hdr = (apfc_hdr_t *) cache_buf;

    entry = NULL;

    if (strncmp(hdr->magic, APFC_MAGIC_STRING, APFC_MAGIC_SIZE) == 0) {
        memcpy(odigest, hdr->digest, APFC_DIGEST_SIZE);
        memset(hdr->digest, 0, APFC_DIGEST_SIZE);
        apfc_compute_digest(ndigest, cache_buf, cache_size);
        if (memcmp(odigest, ndigest, APFC_DIGEST_SIZE) == 0) {
            /*
             * Remove existing reboot entry if present, and find end.
             */
            eh = (apfc_entry_hdr_t *) (hdr + 1);
            endp = (apfc_entry_hdr_t *) (cache_buf +
                    cache_size - sizeof(apfc_entry_hdr_t));
            while (eh < endp) {
                if (eh->type[0] == '\0') {
                    entry = eh;
                    break;
                }
                if (strncmp(eh->type, "reboot", APFC_ENTRY_TYPE_SIZE) == 0) {
                    __u32 todelete = APFC_TOTAL_ENTRY_SIZE(eh);
                    apfc_entry_hdr_t *neh = APFC_NEXT_ENTRY(eh);
                    if (neh > endp) {
                        /* error */
                        break;
                    }
                    memmove(eh, neh, (cache_buf + cache_size) - ((__u8 *) neh));
                    memset(cache_buf + cache_size - todelete, 0, todelete);
                }
                else {
                    eh = APFC_NEXT_ENTRY(eh);
                }
            }
        }
        else {
            printk("%s: digest doesn't match", __FUNCTION__);
        }
    }

    if (eh && endp && eh >= endp) {
        pr_crit("%s: No room to store reboot reason", __FUNCTION__);
        return; 
    }

    if (!entry) {
        memset(cache_buf, 0, cache_size);
        memcpy(hdr->magic, APFC_MAGIC_STRING, APFC_MAGIC_SIZE);
        entry = (apfc_entry_hdr_t *) (hdr + 1);
    }

    memset(entry, 0, sizeof(apfc_entry_hdr_t));
    strncpy(entry->type, "reboot", APFC_ENTRY_TYPE_SIZE);
    entry->size = msglen;
    if (APFC_NEXT_ENTRY(entry) >
            ((apfc_entry_hdr_t *) (cache_buf + cache_size))) {
        printk("%s: No room to store reboot reason", __FUNCTION__);
        return;
    }
    memcpy((entry + 1), buf, msglen);

    do_gettimeofday(&tv);
    hdr->last_write_time = tv.tv_sec;
    hdr->write_count++;

    apfc_compute_digest(ndigest, cache_buf, cache_size);
    memcpy(hdr->digest, ndigest, APFC_DIGEST_SIZE);

    ret = ap_panic_dump_erase(cache_base, cache_addr, erase_size, spi);
    if (ret != 0) {
        printk("%s: erase failed (%d)\n", __FUNCTION__, ret);
        return;
    }

    ret = ap_panic_dump_write(cache_base, cache_addr, cache_size, cache_buf, spi);

    if (ret != 0) {
        printk("%s: write failed (%d)\n", __FUNCTION__, ret);
    }
}
EXPORT_SYMBOL(aruba_save_crash_reason);

void aruba_save_kernel_kat_reboot_reason(const char *reason)
{
    char msgbuf[256];
    printk("\n\nFIPS Kernal KAT failure rebooting: %s\n\n", reason);
    snprintf(msgbuf, sizeof(msgbuf), "Reboot caused by Kernel KAT failure : %s", reason);
    aruba_save_crash_reason(msgbuf);
}

void aruba_save_hw_kat_reboot_reason(const char *reason)
{
    char msgbuf[256];
    printk("\n\nFIPS HW KAT failure rebooting: %s\n\n", reason);
    snprintf(msgbuf, sizeof(msgbuf), "Reboot caused by HW KAT failure : %s", reason);
    aruba_save_crash_reason(msgbuf);
}

EXPORT_SYMBOL(aruba_save_kernel_kat_reboot_reason);
EXPORT_SYMBOL(aruba_save_hw_kat_reboot_reason);

static int
aruba_notify_panic(struct notifier_block *self, unsigned long unused1, void *ptr)
{
    char *buf = ptr;
    char msgbuf[256];

    snprintf(msgbuf, sizeof(msgbuf), "Reboot caused by kernel panic: %s", buf);
    aruba_save_crash_reason(msgbuf);

    return 0;
}

static struct notifier_block aruba_panic_notifier = {
    .notifier_call = aruba_notify_panic,
};

void
aruba_panic_init(void)
{
    atomic_notifier_chain_register(&panic_notifier_list, &aruba_panic_notifier);
}

apflash_t *
aruba_get_flash_info_by_name(char *section)
{
    int i;

    for (i = 0; i < __ap_hw_info[xml_ap_model].flash_variants[ap_flash_variant].afv_num_flash_segments; i++) {
        if (!strcmp(__ap_hw_info[xml_ap_model].flash_variants[ap_flash_variant].afv_flash_segments[i].af_name, section)) {
            return &__ap_hw_info[xml_ap_model].flash_variants[ap_flash_variant].afv_flash_segments[i];
        }
    }
    return 0;
}
EXPORT_SYMBOL(aruba_get_flash_info_by_name);
