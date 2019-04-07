
/*
 * (C) Copyright 2001
 * Denis Peter, MPL AG Switzerland
 *
 * Most of this source has been derived from the Linux USB
 * project.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <common.h>
#include <command.h>
#include <asm/byteorder.h>

#ifdef TPM_DIAG

//extern int tpm_write(char *buff, int count);
//extern int tpm_read(char *buff, int count);

#define DEVID_TPM    	   	0x29
#if 0
static struct tpm_dev_t {
    int currLocality;
    int opened;
    int delay;                  // in milliseconds 
    int tpm_major;
    int tpm_minor;
    unsigned int manuf;
    unsigned int vers;
    unsigned char iobuf[1024];
//  dev_t tpmdev;
//  wait_queue_head_t tpmTimeout;
} tpm;
#endif
extern int tpm_i2c_read_bb(int bus, int dev, unsigned char *buf, int count);
extern int tpm_i2c_write_bb(int bus, int dev, unsigned char *buf, int count);

int
tpm_write(unsigned char *buff, int count)
{
    unsigned long len = 0;
    int ret = 0, bus = 9;

    // Saved the best part of the Atmel driver: the comment below...
    // saddle up and send the whole buffer 

    len = (buff[2] << 24) + (buff[3] << 16) + (buff[4] << 8) + buff[5];

    // Always send the first 10 bytes as a seperate Xaction.
    if (len < 10) {
        printf("cmd length too short, len=%lu\n", len);
        ret = -1;
    }
    if (len > 0x20) {
        ret = tpm_i2c_write_bb(bus, DEVID_TPM, &buff[0], 0x20);
    } else {
        ret = tpm_i2c_write_bb(bus, DEVID_TPM, &buff[0], len);
        return len;
    }
    udelay(10 * 1000);
    if (len > 0x20)
        ret = tpm_i2c_write_bb(bus, DEVID_TPM, &buff[0x20], len - 0x20);
    return len;
}

int
tpm_read(unsigned char *buff, int count)
{
    unsigned long len, retlen;
    int ret = 0, bus = 9;

    memset(buff, 0, count);
    //printk("READ PREAMBLE\n");
//printf("%s:%u\n", __FUNCTION__, __LINE__);
    ret = tpm_i2c_read_bb(bus, DEVID_TPM, &buff[0], count);
    len = (buff[2] << 24) + (buff[3] << 16) + (buff[4] << 8) + buff[5];
#if 0
printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x\n", buff[0],
buff[1],
buff[2],
buff[3], buff[4], buff[5], buff[6], buff[7], buff[8], buff[9]);

printf("len %x\n", len);
#endif
    retlen = len;
    if (len == -1) {
        printf
            ("atpm_read: Len in the TPM resp hdr is -1, TPM still working...\n");
        ret = -1;
        return ret;
    }
    if (len > 2048) {
        ret = -1;
        printf("atpm_read: req length > TPM_KBUF_SZ (len=%d)\n", len);
        return ret;
    }
#if 0
    len -= 10;
    udelay(10 * 1000);
    if (len > 0) {
printf("%s:%u\n", __FUNCTION__, __LINE__);
        ret = tpm_i2c_read_bb(bus, DEVID_TPM, &buff[10], len);
    }
#endif
    return retlen;
}

static int
do_tpm_cmd(unsigned char *cmd_buf, unsigned char *resp_buf, int expectlen)
{
    int ret = 0, timeout = 0, len;

    len =
        (cmd_buf[2] << 24) + (cmd_buf[3] << 16) + (cmd_buf[4] << 8) +
        cmd_buf[5];
//    printf("write %d bytes\n",len);
    ret = tpm_write(cmd_buf, len);
    udelay(700 * 1000);
    if (ret < 0) {
        printf("do_tpm_cmd: write returned error\n");
    } else {
        timeout = 100;
//        do {
        ret = tpm_read(resp_buf, expectlen);
//            if(ret >= 0) break;
//            timeout--;
//        } while(timeout);
    }

    if (timeout == 0) {
        ret = -1;
    } else {
        ret =
            (resp_buf[2] << 24) + (resp_buf[3] << 16) + (resp_buf[4] << 8) +
            resp_buf[5];
        printf("resp len = %d \n", ret);
    }

    return ret;
}

#define TPM_STARTUP_RETLEN  (10)
#define TPM_RESET_RETLEN    (10)
#define TPM_OIAP_RETLEN     (34)
#define TPM_TERMINATE_RETLEN (10)
#define TPM_SELFTEST_RETLEN  (10)
#define TPM_VENDOR_RETLEN    (18)
#define TPM_VER_RETLEN       (18)
#define TPM_INFO_RETLEN      (30)

extern int in_diag_mode;

/*********************************************************************************
 * tpm command intepreter
 */
int
do_tpm(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
    unsigned char respbuf[256];
    unsigned char start_cmd[12] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x00, 0x00, 0x99,
        0x00, 0x01
    };
    unsigned char reset_cmd[10] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0xA,
        0x00, 0x00, 0x00, 0x5A
    };
    unsigned char getCapVer[22] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0x16,
        0x00, 0x00, 0x00, 0x65,
        0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x65
    };

    unsigned char getVendor[22] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0x16,
        0x00, 0x00, 0x00, 0x65,
        0x00, 0x00, 0x00, 0x05,
        0x00, 0x00, 0x00, 0x04,
        0x00, 0x00, 0x01, 0x03
    };

    unsigned char oiap[10] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0x0A,
        0x00, 0x00, 0x00, 0x0A
    };

    unsigned char tpm_term[14] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0x0E,
        0x00, 0x00, 0x00, 0x96,
        0x00, 0x00, 0x00, 0x00
    };

    unsigned char self_test[10] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0xA,
        0x00, 0x00, 0x00, 0x50
    };
    unsigned char tpm_info[18] = { 0x00, 0xC1,
        0x00, 0x00, 0x00, 0x12,
        0x00, 0x00, 0x00, 0x65,
        0x00, 0x00, 0x00, 0x1A,
        0x00, 0x00, 0x00, 0x00
    };
    int i, ret = 0;
    unsigned char start_cmd_resp[10] =
        { 0x00, 0xC4, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00 };
    unsigned char info_cmd_resp[29] =
        { 0x00, 0xC4, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0f, 0x00, 0x30, 0x01, 0x02, 0x23, 0x08,
        0x00, 0x02, 0x01, 0x41, 0x54, 0x4d, 0x4c, 0x00, 0x00
    };
    unsigned char vendor_cmd_resp[18] =
        { 0x00, 0xC4, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x04, 0x41, 0x54, 0x4d, 0x4c
    };
    unsigned char version_cmd_resp[18] =
        { 0x00, 0xC4, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x01
    };
    int failed = 0;

    if (argc < 2 || argc > 2) {
        printf
            ("Wrong no of Args\n Try: tpm < start / info / reset / vendor / self / term / version / oiap>\n");
        return 1;
    }

    if (strncmp(argv[1], "start", 5) == 0) {
        //usb_stop();
        memset(respbuf, 0, 256);
        printf("Send TPM_START\n");
        ret = do_tpm_cmd(start_cmd, respbuf, TPM_STARTUP_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > 20) ? 20 : ret); i++) {
                printf("%02x ", respbuf[i]);
                if (start_cmd_resp[i] != respbuf[i])
                    failed = 1;
            }
            printf("\n");
        }
        if (in_diag_mode)
            printf("Result: %s\n", failed ? "FAIL" : "PASS");
        return 0;
    }

    if (strncmp(argv[1], "reset", 5) == 0) {
        memset(respbuf, 0, 256);
        printf("Send TPM_RESET\n");
        ret = do_tpm_cmd(reset_cmd, respbuf, TPM_RESET_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > 20) ? 20 : ret); i++)
                printf("%02x ", respbuf[i]);
            printf("\n");
        }
        return 0;
    }

    if (strncmp(argv[1], "version", 7) == 0) {
        printf("Send TPM_VERSION\n");
        memset(respbuf, 0, 256);
        ret = do_tpm_cmd(getCapVer, respbuf, TPM_VER_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > 32) ? 32 : ret); i++) {
                printf("%02x ", respbuf[i]);
                if (version_cmd_resp[i] != respbuf[i])
                    failed = 1;
            }
            printf("\n");
        }
        if (in_diag_mode)
            printf("Result: %s\n", failed ? "FAIL" : "PASS");
        return 0;
    }

    if (strncmp(argv[1], "info", 4) == 0) {
        printf("Send TPM_INFO\n");
        memset(respbuf, 0, 256);
        ret = do_tpm_cmd(tpm_info, respbuf, TPM_INFO_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > 32) ? 32 : ret); i++) {
                printf("%02x ", respbuf[i]);
                if (info_cmd_resp[i] != respbuf[i])
                    failed = 1;
            }
            printf("\n");
        }
        if (in_diag_mode)
            printf("Result: %s\n", failed ? "FAIL" : "PASS");
        return 0;
    }

    if (strncmp(argv[1], "vendor", 6) == 0) {
        printf("Send TPM_VENDOR\n");
        memset(respbuf, 0, 256);
        ret = do_tpm_cmd(getVendor, respbuf, TPM_VENDOR_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > 32) ? 32 : ret); i++) {
                printf("%02x ", respbuf[i]);
                if (vendor_cmd_resp[i] != respbuf[i])
                    failed = 1;
            }
            printf("\n");
        }
        if (in_diag_mode)
            printf("Result: %s\n", failed ? "FAIL" : "PASS");
        return 0;
    }

    if (strncmp(argv[1], "oiap", 4) == 0) {
        printf("Send TPM_OIAP\n");
        memset(respbuf, 0, 256);
        ret = do_tpm_cmd(oiap, respbuf, TPM_OIAP_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > TPM_OIAP_RETLEN) ? TPM_OIAP_RETLEN : ret); i++)
                printf("%02x ", respbuf[i]);
            printf("\n");
        }
        return 0;
    }

    if (strncmp(argv[1], "term", 4) == 0) {
        printf("Send TPM_TERM\n");
        memset(respbuf, 0, 256);
        ret = do_tpm_cmd(tpm_term, respbuf, TPM_TERMINATE_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > 32) ? 32 : ret); i++)
                printf("%02x ", respbuf[i]);
            printf("\n");
        }
        return 0;
    }

    if (strncmp(argv[1], "self", 4) == 0) {
        printf("Send TPM_SELF\n");
        memset(respbuf, 0, 256);
        ret = do_tpm_cmd(self_test, respbuf, TPM_SELFTEST_RETLEN);
        if (ret > 0) {          // command didn't time out - display results
            printf("Bytes read back: %d = ", ret);
            for (i = 0; i < ((ret > 32) ? 32 : ret); i++)
                printf("%02x ", respbuf[i]);
            printf("\n");
        }
        return 0;
    }
    printf("Usage:\n%s\n", cmdtp->usage);
    return 1;
}

U_BOOT_CMD(tpm, 7, 1, do_tpm,
           "tpm    - send TPM test commands\n",
           " Usage: tpm < start / info / reset / vendor / self / term / version / oiap>\n");
#endif
