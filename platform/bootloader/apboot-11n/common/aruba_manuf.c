// vim:set ts=4 sw=4 expandtab:
#include <config.h>
#if defined(CONFIG_APBOOT) || defined(CONFIG_GRENACHE)

#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include "aruba_apboot.h"
#include "aruba_manuf.h"
#if defined(CONFIG_CMD_NAND)
#include <nand.h>

int nand_mfg_device = 0;
#endif

extern int do_flerase(cmd_tbl_t *, int, int, char **);
extern int do_mem_cp(cmd_tbl_t *, int, int, char **);
extern int do_mem_cmp(cmd_tbl_t *, int, int, char **);

#ifdef __TLV_INVENTORY__
#include "aruba-inventory.h"
void *inventory_handle = 0;

int manuf_is_fips_ap(void);

int
ap_invent_read(__u8 *buf, unsigned l)
{
#ifdef CFG_MANUF_BASE
#ifdef USE_FLASH_READ_SUPPORT
#ifdef CONFIG_CMD_NAND
    nand_read_skip_bad(&nand_info[nand_mfg_device], CFG_MANUF_BASE, &l, buf);
#else
    aruba_flash_read((void *)CFG_MANUF_BASE, buf, l);
#endif
#else
    // XXX assumes all sectors same size
    memcpy(buf, (void *)CFG_MANUF_BASE, l);
#endif
#endif
    return 0;
}

int
ap_invent_write(__u8 *buf, unsigned l)
{
#ifdef CFG_MANUF_BASE
    aruba_copy_and_verify(0, 0, (ulong)buf, CFG_MANUF_BASE, CFG_MANUF_BASE + CFG_MFG_SIZE - 1,
        l, 1, 1);
#endif
    return 0;
}

__u8 invent_buf[__TLV_INVENTORY_BUF_SIZE__];

void
aruba_manuf_init_tlv(void)
{
    if (inventory_init == 0) {
        invent_setup(invent_buf, sizeof(invent_buf), 
            ap_invent_read, ap_invent_write, &inventory_handle);
        invent_init(inventory_handle);
        inventory_init = 1;
    }
}

void
manuf_recover_ethaddr(int do_save)
{
#ifdef CFG_MANUF_BASE
    int need_save = 0;
    char *env_fips;

    if (!getenv("ethaddr")) {
        int r;
        __u8 e[6];
        unsigned l = sizeof(e);

        r = invent_get_card_tlv(inventory_handle, 0, INVENT_TLV_WIRED_MAC, e, &l);
        if (r == 0 && l == 6) {
            char mac[18];
            sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
                e[0], e[1], e[2], e[3], e[4], e[5]);
            setenv("ethaddr", mac);
#ifdef CONFIG_CARDHU_notused
            setenv("eth1addr", mac);
#endif
            need_save = 1;
        }
    }

    env_fips = getenv("fips_certified");
    /* if FIPS AP, save FIPS flag to env */
    if (manuf_is_fips_ap()) {
        if (!env_fips || strcmp(env_fips, "1")) {
            setenv("fips_certified", "1");
            need_save = 1;
        }
    } else if (env_fips) {
        setenv("fips_certified", NULL);
        need_save = 1;
    }

    if (do_save && need_save) {
        saveenv();
    }
#endif
}

int
do_mfginfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef CFG_MANUF_BASE
    invent_mfginfo(inventory_handle);
#endif
    return 0;
}

static int cpu_prom_mod = 0;
extern uint8_t *ether_aton(char *, uint8_t *);

int 
cli_prog_inv(cmd_tbl_t *cmdtp, int flag, int argc, char* argv[])
{
    int cpu = 0, chassis = 0;  //ap = 0, base = 0, 
    int type = INVENT_CARD_TYPE_CPU;
    int antenna = 0;
    int cm_board = 0;
    int power_board = 0;
    int           parent = 0, card = 0, r;
    int           item_len;
    uint8_t       mac[6];

    // not enough args? bounce
    if (argc != 4) {
        printf ("Wrong number of arguments\n");
        return 1;
    }
    if (inventory_init == 0) {
        r = invent_setup(invent_buf, sizeof(invent_buf), 
            ap_invent_read, ap_invent_write, &inventory_handle);
        invent_init(inventory_handle);
        inventory_init = 1;
    }

    // main card prom
    if (!strcmp(argv[1], "cpu")) {
        cpu = 1;
        card = 1;
        parent = 0;
    } else if (!strcmp(argv[1], "system")) {
        // chassis prom info
        chassis = 1;
        card = 0;
        parent = INVENT_NO_PARENT;
    } else if (!strcmp(argv[1], "antenna")) {
        // chassis prom info
        antenna = 1;
        card = 2;
        parent = 1;
    } else if (!strcmp(argv[1], "base")) {
        // base board
        cpu = 1;
        card = 1;
        parent = 0;
        type = INVENT_CARD_TYPE_BASE;
    } else if (!strcmp(argv[1], "cm")) {
        // cm board
        cm_board = 1;
        card = 2;
        parent = 0;
        type = INVENT_CARD_TYPE_CM;
    } else if (!strcmp(argv[1], "power")) {
        // power board
        power_board = 1;
        card = 2;
        parent = 1;
        type = INVENT_CARD_TYPE_POWER_BOARD;
    } else {
        printf("must specify  'cpu' or 'antenna', 'base', 'system' or 'power' \n");
        return 0;
    }

    // assure the presence of the base card before accessing the EEPROM
//    if (!strcmp(argv[1], "base") || !strcmp(argv[1], "chassis")) {
//        if (!base_card_present()) {
//            printf("Base card not detected - can't access Base Card SEEPROM\n");
//            return 1;
//        }
//    }
    if (cpu) {
        r = invent_set_card(inventory_handle, card, type, parent);
        if (r < 0) {
            printf("set card #1 (cpu/base info) failed: %s\n", invent_error(r));
        }
    }
    
    if (chassis) {
        r = invent_set_card(inventory_handle, card, INVENT_CARD_TYPE_SYSTEM, INVENT_NO_PARENT);
        if (r < 0) {
            printf("set card #0 (system info) failed: %s\n", invent_error(r));
        }
    }

    if (antenna) {
        r = invent_set_card(inventory_handle, card, INVENT_CARD_TYPE_ANTENNA, parent);
        if (r < 0) {
            printf("set card #2 (antenna info) failed: %s\n", invent_error(r));
        }
    }

    if (cm_board) {
        r = invent_set_card(inventory_handle, card, INVENT_CARD_TYPE_CM, parent);
        if (r < 0) {
            printf("set card #2 (CM board info) failed: %s\n", invent_error(r));
        }
    }

    if (power_board) {
        r = invent_set_card(inventory_handle, card, INVENT_CARD_TYPE_POWER_BOARD, parent);
        if (r < 0) {
            printf("set card #2 (power board info) failed: %s\n", invent_error(r));
        }
    }

    // magic time!
    // don't store more than 32 bytes for any item
    item_len = strlen(argv[argc - 1]);
    item_len = (item_len > INVENT_MAX_DATA) ? INVENT_MAX_DATA : item_len;
    if (!strcmp(argv[2], "assem")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_ASSY, (__u8 *)argv[argc - 1], item_len);        
    } else if (!strcmp(argv[2], "date")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_DATE, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "mac") && chassis) {
        ether_aton(argv[3], mac);
        printf("MAC addr=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);

        r = invent_add_tlv(inventory_handle, card, INVENT_TLV_WIRED_MAC, mac, 6);
    } else if (!strcmp(argv[2], "wmac") && chassis) {
        ether_aton(argv[3], mac);
        printf("WMAC addr=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);

        r = invent_add_tlv(inventory_handle, card, INVENT_TLV_WIRELESS_MAC, mac, 6);
    } else if (!strcmp(argv[2], "numwmacs") && chassis) {
        __u16 zmac;
        zmac = simple_strtoul(argv[argc - 1],0,0);
        mac[1] = zmac & 0xff;
        mac[0] = (zmac >> 8) & 0xff;
        r = invent_add_tlv(inventory_handle, card, INVENT_TLV_WIRELESS_MAC_COUNT, mac, 2);
    } else if (!strcmp(argv[2], "nummacs") && chassis) {
        __u16 zmac;
        zmac = simple_strtoul(argv[argc - 1],0,0);
        mac[1] = zmac & 0xff;
        mac[0] = (zmac >> 8) & 0xff;
        r = invent_add_tlv(inventory_handle, card, INVENT_TLV_WIRED_MAC_COUNT, mac, 2);
    } else if (!strcmp(argv[2], "majrev")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_MAJOR_REV, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "variant")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_MINOR_REV, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "sernum")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_SN, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "sertag")) {
        if (strcmp(argv[argc - 1], "-d")) {
            invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_ST, (__u8 *)argv[argc - 1], item_len);
        } else {
            printf("Deleting service tag\n");
            invent_remove_tlv(inventory_handle, card, INVENT_TLV_CARD_ST);
        }
    } else if (!strcmp(argv[2], "5gsn")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_RADIO0_SN, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "2gsn")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_RADIO1_SN, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "ccode")) {
        if (strcmp(argv[argc - 1], "-d")) {
            invent_add_tlv(inventory_handle, card, INVENT_TLV_COUNTRY_CODE, (__u8 *)argv[argc - 1], item_len);
        } else {
            printf("Deleting country code\n");
            invent_remove_tlv(inventory_handle, card, INVENT_TLV_COUNTRY_CODE);
        }
    } else if (!strcmp(argv[2], "teststa")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_TEST_STATION, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "model")) {
        invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_MODEL, (__u8 *)argv[argc - 1], item_len);
    } else if (!strcmp(argv[2], "imac")) {
        ether_aton(argv[3], mac);
        printf("MAC addr=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);
        r = invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_INTERNAL_MAC, mac, 6);
    } else if (!strcmp(argv[2], "mac") && cm_board) {
        ether_aton(argv[3], mac);
        printf("MAC addr=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);
        r = invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_MAC, mac, 6);
    } else if (!strcmp(argv[2], "fips")) {
        if (strcmp(argv[argc - 1], "-d")) {
            invent_add_tlv(inventory_handle, card, INVENT_TLV_FIPS, (__u8 *)argv[argc - 1], item_len);
        } else {
            printf("Deleting fips flag\n");
            invent_remove_tlv(inventory_handle, card, INVENT_TLV_FIPS);
        }
    } else if (!strcmp(argv[2], "snum") && chassis) {
        if (strcmp(argv[argc - 1], "-d")) {
            invent_add_tlv(inventory_handle, card, INVENT_TLV_OEM_SN, (__u8 *)argv[argc - 1], item_len);
        } else {
            printf("Deleting scalance number\n");
            invent_remove_tlv(inventory_handle, card, INVENT_TLV_OEM_SN);
        }
    } else if (!strcmp(argv[2], "eirp")) {
        if (strcmp(argv[argc - 1], "-d")) {
            invent_add_tlv(inventory_handle, card, INVENT_TLV_CARD_MAX_EIRP, (__u8 *)argv[argc - 1], item_len);
        } else {
            printf("Deleting max EIRP\n");
            invent_remove_tlv(inventory_handle, card, INVENT_TLV_CARD_MAX_EIRP);
        }
    } else {
        printf("Sorry! I don't recognize '%s' as a valid %s inventory entry\n", argv[2], argv[1]);
        return 1;
    }
    
        
    if (cpu || chassis || antenna || power_board || cm_board) {
        cpu_prom_mod = 1;
    }
       
    return 0;
}

int 
cli_init_invent(cmd_tbl_t *cmdtp, int flag, int argc, char* argv[])
{
    int             c, init, print, r; // bc_present, ap = 0;
    
    optind = 1;
    optreset = 1;
    init = print = 0; //bc_present = 0;
    while ((c = getopt (argc, argv, "wpz")) != -1)
    {
        switch (c)
        {
            case 'w':
                init = 1;
                break;
            
            case 'p':
                print = 1;
                break;
            
            case '?':
                if (isprint (optopt))
                  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                  fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;

            default:
                break;
        }
    }
    if (inventory_init == 0) {
        r = invent_setup(invent_buf, sizeof(invent_buf), ap_invent_read, 
            ap_invent_write, &inventory_handle);
        invent_init(inventory_handle);
        inventory_init = 1;
    }
    
    if (init) {      // option -w
        if (cpu_prom_mod) {
            r = invent_save(inventory_handle);
            cpu_prom_mod = 0;
        }
    }
   
    if (print) {     // option -p
        invent_mfginfo(inventory_handle);
    }
    return 0;
}

int
manuf_is_iap(void)
{
#if defined(CFG_MANUF_BASE)
#define __CCODE_LEN__ 6
    int r;
    __u8 p[__CCODE_LEN__];
    unsigned l = sizeof(p);

    r = invent_get_card_tlv(inventory_handle, 0, INVENT_TLV_COUNTRY_CODE, p, &l);
    if (r == 0 && l == __CCODE_LEN__) {
        if (!strncmp((char *)p, "CCODE-", __CCODE_LEN__)) {
            return 1;
        }
    }
#undef __CCODE_LEN__
#endif
    return 0;
}

int
manuf_is_fips_ap(void)
{
#if defined(CFG_MANUF_BASE)
#define __FIPS_FLAG_LEN__ 3
    int r;
    __u8 p[4];
    unsigned l = sizeof(p);

    r = invent_get_card_tlv(inventory_handle, 0, INVENT_TLV_FIPS, p, &l);
    if (r == 0 && l == __FIPS_FLAG_LEN__) {
        if (!strncmp((char *)p, "yes", __FIPS_FLAG_LEN__)) {
            return 1;
        }
    }
#undef __FIPS_FLAG_LEN__
#endif
    return 0;
}
#else   /* __TLV_INVENTORY__ */

unsigned char
manuf_compute_checksum(nvram_manuf_t *manuf)
{
    unsigned char checksum = 0;
    unsigned char *ptr = (unsigned char *) manuf;
    int i;

    for (i = 0; i < sizeof(*manuf) - 1; i++) {
        checksum += *ptr++;
    }
    return checksum;
}

int
do_mfgsum(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    nvram_manuf_t *manuf = 0;
    unsigned char checksum = 0;

    if (argc == 2) {
        manuf = (nvram_manuf_t *)simple_strtoul(argv[1], NULL, 16);
    } else {
        printf("Memory address must be supplied.\n");
        return 1;
    }

    checksum = manuf_compute_checksum(manuf);
    manuf->checksum = checksum;

    return 0;
}

void
manuf_recover_ethaddr(int do_save)
{
#ifdef CFG_MANUF_BASE
    if (!getenv("ethaddr")) {
        nvram_manuf_t *manuf = (nvram_manuf_t *)CFG_MANUF_BASE;
        unsigned char checksum = 0;

        checksum = manuf_compute_checksum(manuf);

        if (checksum == manuf->checksum) {
            char mac[18];
            sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
                manuf->network_address[0],
                manuf->network_address[1],
                manuf->network_address[2],
                manuf->network_address[3],
                manuf->network_address[4],
                manuf->network_address[5]);
            setenv("ethaddr", mac);
#ifdef CONFIG_CARDHU_notused
            setenv("eth1addr", mac);
#endif
            if (do_save) {
                saveenv();
            }
        }
    }
#endif
}

int
do_mfginfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef CFG_MANUF_BASE
    nvram_manuf_t *manuf = (nvram_manuf_t *)CFG_MANUF_BASE;
    unsigned char checksum = 0;
#ifdef CFG_CCODE_OFFSET
    char *p = (char *)(CFG_MANUF_BASE + CFG_CCODE_OFFSET);
    int i;
#endif

    checksum = manuf_compute_checksum(manuf);

//printf("%x %u %x %x\n", manuf, sizeof(*manuf), checksum, manuf->checksum);

    if (checksum == manuf->checksum) {
        printf("Manufacturing info:\n");
        printf("  System S/N      : %.*s\n", sizeof(manuf->system_serial_number), manuf->system_serial_number);
        printf("  Date Code       : %.*s\n", sizeof(manuf->date), manuf->date);
        printf("  Base MAC        : %02x:%02x:%02x:%02x:%02x:%02x\n", 
            manuf->network_address[0], manuf->network_address[1], manuf->network_address[2], manuf->network_address[3],
            manuf->network_address[4], manuf->network_address[5]);
        printf("  MAC Count       : %u\n", *((ushort *)(&manuf->network_address_count[0])));
        printf("  CPU Assembly    : %.*s\n", sizeof(manuf->part_number), manuf->part_number);
        printf("  CPU Major Rev   : %.*s\n", sizeof(manuf->major_revision), manuf->major_revision);
        printf("  CPU Brd Variant : %.*s\n", sizeof(manuf->minor_revision), manuf->minor_revision);
        printf("  CPU Board S/N   : %.*s\n", sizeof(manuf->box_serial_number), manuf->box_serial_number);
#ifdef AP_HAS_TWO_BOARDS
        printf("  Radio Assembly  : %.*s\n", sizeof(manuf->radio_card_assembly_number), manuf->radio_card_assembly_number);
        printf("  Radio Major Rev : %.*s\n", sizeof(manuf->radio_card_major_revision), manuf->radio_card_major_revision);
        printf("  Radio Variant   : %.*s\n", sizeof(manuf->radio_card_variant), manuf->radio_card_variant);
        printf("  Radio S/N       : %.*s\n", sizeof(manuf->radio_card_serial_number), manuf->radio_card_serial_number);
#endif
#ifdef AP_HAS_SEPARATE_WIRELESS_MAC
        printf("  Radio MAC       : %02x:%02x:%02x:%02x:%02x:%02x\n", 
            manuf->wireless_address[0], manuf->wireless_address[1], manuf->wireless_address[2], manuf->wireless_address[3],
            manuf->wireless_address[4], manuf->wireless_address[5]);
        printf("  Radio MAC Count : %u\n", (manuf->wireless_address_count[0] << 8)
                                            + manuf->wireless_address_count[1]);
#endif
#ifdef CFG_CCODE_OFFSET
#define DOMAIN_STR_LEN 6  /* defined in cmn/oem/countryCode.h */
        if (!strncmp(p, "CCODE-", strlen("CCODE-")))  {
            printf("  Country Code    : ");
            p += strlen("CCODE-");
            for (i = 0; ((i < DOMAIN_STR_LEN) && (*p != '-')); i++)
                printf("%c", *p++);
            printf("\n");
        }
#endif
        printf("  PROM Format     : %c\n", manuf->format);
        printf("  Checksum        : %x\n", manuf->checksum);
    } else {
        printf("Manufacturing checksum is incorrect.\n");
    }
#endif
    return 0;
}

int
manuf_is_iap(void)
{
#if defined(CFG_MANUF_BASE) && defined(CFG_CCODE_OFFSET)
    char *p = (char *)(CFG_MANUF_BASE + CFG_CCODE_OFFSET);
    if (!strncmp(p, "CCODE-", strlen("CCODE-")))
        return 1;
#endif
    return 0;
}

#ifdef CONFIG_TALISKER
static unsigned char tmpmanuf[CFG_FLASH_SECTOR_SIZE];

int
do_fixmfg(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    nvram_manuf_t *manuf = 0;
    unsigned char checksum = 0;
    char *av[8];
    char sbuf[32], ebuf[32], mbuf[32];
    char *sn, *mac;
    int r = 0, i, l;
    char tmpe[18], *tmp, *end;

    if (argc == 3) {
        sn = argv[1];
        mac = argv[2];
    } else {
        printf("Serial number and MAC address must be supplied.\n");
        return 1;
    }

    memcpy(tmpmanuf, (void *)CFG_MANUF_BASE, sizeof(tmpmanuf));
    manuf = (nvram_manuf_t *)tmpmanuf;

    if (strlen(sn) != sizeof(manuf->system_serial_number)) {
        printf("Serial number is not %u characters\n", sizeof(manuf->system_serial_number));
        return 1;
    }

    l = strlen(mac);

    if (l != 17 && l != 12) {
        printf("MAC address is not 17 (with colons) or 12 (without) characters\n");
        return 1;
    }

    av[0] = "mfginfo";
    av[1] = 0;

    printf("Old ");
    do_mfginfo(cmdtp, flag, 1, av);

    memcpy(manuf->system_serial_number, sn, sizeof(manuf->system_serial_number));

	strcpy(tmpe, mac);
    tmp = &tmpe[0];

    if (l == 17) {
        for (i = 0; i < 6 ; i++) {
            manuf->network_address[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
            if (tmp)
                tmp = (*end) ? end + 1 : end;
        }
    } else {
        char *p, pbuf[3];
        int j = 0;

        pbuf[2] = '\0';
        p = mac;
        i = 0;
        j = 0;

        while (i < 12) {
            pbuf[0] = p[i];
            pbuf[1] = p[i + 1];
            manuf->network_address[j] = simple_strtoul(pbuf, 0, 16);
            i += 2;
            j++;
        }
    }
#if 0
printf("New info: %.*s %02x:%02x:%02x:%02x:%02x:%02x\n", 9, 
    manuf->system_serial_number,
    manuf->network_address[0],
    manuf->network_address[1],
    manuf->network_address[2],
    manuf->network_address[3],
    manuf->network_address[4],
    manuf->network_address[5]);
#endif

    checksum = manuf_compute_checksum(manuf);
    manuf->checksum = checksum;

    snprintf(sbuf, sizeof(sbuf), "0x%x", CFG_MANUF_BASE);
    snprintf(ebuf, sizeof(ebuf), "0x%x", CFG_MANUF_BASE + CFG_FLASH_SECTOR_SIZE - 1);

//printf("%s %s\n", sbuf, ebuf);
    av[0] = "erase";
    av[1] = sbuf;
    av[2] = ebuf;
    av[3] = 0;

    printf("Erasing flash\n");
    r = do_flerase(cmdtp, flag, 3, av);

    snprintf(sbuf, sizeof(sbuf), "0x%x", CFG_MANUF_BASE);
    snprintf(ebuf, sizeof(ebuf), "0x%x", CFG_FLASH_SECTOR_SIZE);
    snprintf(mbuf, sizeof(mbuf), "0x%x", &tmpmanuf[0]);
    av[0] = "cp.b";
    av[1] = mbuf;
    av[2] = sbuf;
    av[3] = ebuf;
    av[4] = 0;
//printf("%s %s %s\n", av[1], av[2], av[3]);

    r |= do_mem_cp(cmdtp, flag, 4, av);

    av[0] = "cmp.b";
    av[1] = mbuf;
    av[2] = sbuf;
    av[3] = ebuf;
    av[4] = 0;
    r |= do_mem_cmp(cmdtp, flag, 4, av);

    printf("New ");
    do_mfginfo(cmdtp, flag, 1, av);

    return r;
}
#endif  /* TALISKER */
#endif  /* __TLV_INVENTORY__ */

U_BOOT_CMD(
	mfginfo,     1,     0,      do_mfginfo,
	"mfginfo - show manufacturing info\n",
	"Show manufacturing info\n"
);


#ifdef __TLV_INVENTORY__
U_BOOT_CMD(
	invent,    3,    1,     cli_init_invent,
	"invent   - display/write Manufacturing inventory contents\n",
	"   -w write modified inventory contents to flash\n"
	"   -p print contents of inventory\n"
);

U_BOOT_CMD(
	proginv,    5,    1,     cli_prog_inv,
    "proginv    - program a given entry in the inventory\n",
	"<board> <prom_entry> <value1>\n"
    "Where\n"
    "  board = 'cpu', 'antenna', 'system' or 'power'\n"
    "  prom_entry = one of 'assem', 'date', 'mac', 'nummacs'\n"
    "               'sernum', 'majrev', 'variant'\n"
);
#else
U_BOOT_CMD(
	mfgsum,     2,     1,      do_mfgsum,
	"mfgsum addr - checksum manufacturing info\n",
	"Checksum manufacturing info \n"
);

#ifdef CONFIG_TALISKER
U_BOOT_CMD(
	fixmfg,     3,     1,      do_fixmfg,
	"fixmfg serno MAC - Update serial number and MAC address\n",
	"Update serial number and MAC address\n"
);
#endif  /* TALISKER */
#endif  /* TLV_INVENTORY */

#endif
