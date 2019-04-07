// vim:set ts=4 sw=4:
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 */

/*
 * Boot support
 */
#include <common.h>
#include <command.h>
#ifdef CONFIG_APBOOT
#include <aruba_image.h>
#include <aruba_apboot.h>
#endif
#include <net.h>
#include <watchdog.h>
#if defined(CONFIG_GRENACHE) || defined(CONFIG_NEBBIOLO)
#include "../../../../cmn/util/aruba_image.h"
#endif

#if (CONFIG_COMMANDS & CFG_CMD_NET)

int tftpload = 1;

#ifdef CONFIG_APBOOT
extern int boot_aruba_image(unsigned);
#endif	// apboot

static int netboot_common (proto_t, cmd_tbl_t *, int , char *[]);

#if defined(CONFIG_MSR_SUBTYPE)
/* Used by msr_boot_image */
#define MSR_PRI_IMG		0
extern int msr_image_type(void *load_addr);
extern int msr_boot_image(void *addr, int type); 
#endif

#ifndef CONFIG_APBOOT
int do_bootp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return netboot_common (BOOTP, cmdtp, argc, argv);
}

U_BOOT_CMD(
	bootp,	3,	1,	do_bootp,
	"bootp - boot image via network using BootP/TFTP protocol\n",
	"[loadAddress] [bootfilename]\n"
);
#endif

int do_tftpb (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    tftpload = 1;

	NetOurIP = getenv_IPaddr("ipaddr");
	NetServerIP = getenv_IPaddr("serverip");
	if (!NetOurIP) {
		return netboot_common (DHCP, cmdtp, argc, argv);
	} else if (!NetServerIP) {
#ifdef CONFIG_APBOOT
		return netboot_common (ADP, cmdtp, argc, argv);
#else
		;
		// fall through
#endif
	}
	return netboot_common (TFTP, cmdtp, argc, argv);
}

#ifdef CONFIG_APBOOT
int do_tftpboot (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int r = 1;
	char *av[5];
	char abuf[32];
	int ac;
	unsigned ramaddr;
	int tries = 0;
#ifndef CFG_TFTP_STAGING_AREA
    DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	ramaddr = bd->bi_uboot_ram_addr + bd->bi_uboot_ram_used_size;
	ramaddr += (1024 * 1024);
#else
	ramaddr = CFG_TFTP_STAGING_AREA;
#endif

restart:
	if (tries++ > 60) {
		extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

		printf("Too many time outs, resetting...\n");
		do_reset(NULL, 0, 0, NULL);
	}
    tftpload = 1;

	NetOurIP = getenv_IPaddr("ipaddr");
	NetServerIP = getenv_IPaddr("serverip");
	NetOurGatewayIP = getenv_IPaddr("gatewayip");
	NetOurSubnetMask = getenv_IPaddr("netmask");

	av[0] = "tftp";
	av[1] = abuf;
	if (argc == 2) {
		av[2] = argv[1];
		av[3] = 0;
		ac = 3;
	} else {
		av[2] = 0;
		ac = 2;
	}

	sprintf(abuf, "%x", ramaddr);

	if (!NetOurIP || !NetOurGatewayIP || !NetOurSubnetMask) {
		r = netboot_common (DHCP, cmdtp, ac, av);
	} else if (!NetServerIP) {
		r = netboot_common (ADP, cmdtp, ac, av);
	} else {
		r = netboot_common (TFTP, cmdtp, ac, av);
	}
	if (r == 0) {
#if defined(CONFIG_MSR_SUBTYPE)
        if (msr_image_type((void *)load_addr)) {
            printf("Start to boot MeshOS image\n");
            if (msr_boot_image((void *)load_addr, MSR_PRI_IMG)) {
                printf("Boot MeshOS failed.\n");
            }
        }
#endif /* CONFIG_MSR_SUBTYPE */
#ifdef CONFIG_APBOOT
		r = aruba_basic_image_verify((void *)load_addr, 
			__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0);
		if (r) {
			goto retry_download;
		}
#if defined(__SIGNED_IMAGES__)
        if ((r = image_verify((aruba_image_t*) ramaddr))) {
			r = 2;
            goto retry_download;
		}
#endif
#endif	// apboot
		(void)boot_aruba_image(ramaddr + ARUBA_HEADER_SIZE);
	} else if (r == 2) {	// 2 == image verification failure
		uint64_t t;
retry_download:
		t = get_timer(0);
		printf("tftpboot failed: retrying in one second\n");
		while (get_timer(t) < CFG_HZ) {
			WATCHDOG_RESET();
		}
		goto restart;
	}
	return r;
}

U_BOOT_CMD(
	netget,	3,	1,	do_tftpb,
	"netget- load image via network using TFTP protocol\n",
	"[loadAddress] [bootfilename]\n"
);

U_BOOT_CMD(
	tftpboot,	2,	0,	do_tftpboot,
	"tftpboot- boot image via network using TFTP protocol\n",
	"[bootfilename]\n"
);
#else
U_BOOT_CMD(
	tftpboot,	3,	0,	do_tftpb,
	"tftpboot- boot image via network using TFTP protocol\n",
	"[loadAddress] [bootfilename]\n"
);
#endif

#ifndef CONFIG_APBOOT
int do_rarpb (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
      tftpload = 1;
	return netboot_common (RARP, cmdtp, argc, argv);
}

U_BOOT_CMD(
	rarpboot,	3,	1,	do_rarpb,
	"rarpboot- boot image via network using RARP/TFTP protocol\n",
	"[loadAddress] [bootfilename]\n"
);
#endif

#if (CONFIG_COMMANDS & CFG_CMD_DHCP)
int do_dhcp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
      tftpload = 0;
	return netboot_common(DHCP, cmdtp, argc, argv);
}

U_BOOT_CMD(
	dhcp,	3,	0,	do_dhcp,
	"dhcp - invoke DHCP client to obtain IP/boot params\n",
	"\n"
);
#endif	/* CFG_CMD_DHCP */

#if (CONFIG_COMMANDS & CFG_CMD_NFS)
int do_nfs (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return netboot_common(NFS, cmdtp, argc, argv);
}

U_BOOT_CMD(
	nfs,	3,	1,	do_nfs,
	"nfs - boot image via network using NFS protocol\n",
	"[loadAddress] [host ip addr:bootfilename]\n"
);
#endif	/* CFG_CMD_NFS */

static void netboot_update_env (void)
{
	char tmp[22];

	if (NetOurGatewayIP) {
		ip_to_string (NetOurGatewayIP, tmp);
		setenv ("gatewayip", tmp);
	}

	if (NetOurSubnetMask) {
		ip_to_string (NetOurSubnetMask, tmp);
		setenv ("netmask", tmp);
	}

	if (NetOurHostName[0])
		setenv ("hostname", NetOurHostName);

	if (NetOurRootPath[0])
		setenv ("rootpath", NetOurRootPath);

	if (NetOurIP) {
		ip_to_string (NetOurIP, tmp);
		setenv ("ipaddr", tmp);
	}

	if (NetServerIP) {
		ip_to_string (NetServerIP, tmp);
		setenv ("serverip", tmp);
	}

	if (NetOurDNSIP) {
		ip_to_string (NetOurDNSIP, tmp);
		setenv ("dnsip", tmp);
	}
#if (CONFIG_BOOTP_MASK & CONFIG_BOOTP_DNS2)
	if (NetOurDNS2IP) {
		ip_to_string (NetOurDNS2IP, tmp);
		setenv ("dnsip2", tmp);
	}
#endif
	if (NetOurNISDomain[0])
		setenv ("domain", NetOurNISDomain);

#if (CONFIG_COMMANDS & CFG_CMD_SNTP) && (CONFIG_BOOTP_MASK & CONFIG_BOOTP_TIMEOFFSET)
	if (NetTimeOffset) {
		sprintf (tmp, "%d", NetTimeOffset);
		setenv ("timeoffset", tmp);
	}
#endif
#if (CONFIG_COMMANDS & CFG_CMD_SNTP) && (CONFIG_BOOTP_MASK & CONFIG_BOOTP_NTPSERVER)
	if (NetNtpServerIP) {
		ip_to_string (NetNtpServerIP, tmp);
		setenv ("ntpserverip", tmp);
	}
#endif
}

unsigned net_copy_size = 0;

static int
netboot_common (proto_t proto, cmd_tbl_t *cmdtp, int argc, char *argv[])
{
	char *s;
	int   rcode = 0;
	int   size;

	net_copy_size = 0;

	/* pre-set load_addr */
	if ((s = getenv("loadaddr")) != NULL) {
		load_addr = simple_strtoul(s, NULL, 16);
	}
	/* pre-set BootFile */
	if ((s = getenv("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof(BootFile));
	}

	switch (argc) {
	case 1:        
		break;

	case 2:	/* only one arg - accept two forms:
		 * just load address, or just boot file name.
		 * The latter form must be written "filename" here.
		 */
#ifndef CONFIG_APBOOT
        if ((argv[1][0] == '0')&&(argv[1][1] == 'x')) {
			load_addr = simple_strtoul(argv[1], NULL, 16);
        } else {
			copy_filename (BootFile, argv[1], sizeof(BootFile)-2);
		}
#else
		if (argv[1][0] == '"') {	/* just boot filename */
			copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {			/* load address	*/
			load_addr = simple_strtoul(argv[1], NULL, 16);
		}
#endif
		break;

	case 3:	load_addr = simple_strtoul(argv[1], NULL, 16);
		copy_filename (BootFile, argv[2], sizeof(BootFile));

		break;

	default: printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

    /* This is where all the magic happens */
	size = NetLoop(proto);
	if (size < 0)
		return 1;

	net_copy_size = size;

	/* NetLoop ok, update environment */
	netboot_update_env();

	/* done if no file was loaded (no errors though) */
	if (size == 0)
		return 0;

	/* flush cache */
	flush_cache(load_addr, size);

    /* Null BootFile str - setting it once and reusing it forever doesn't make
       sense. If it's pre-set in the env vars, then we'll still pick it up. */
    BootFile[0] = '\0';

	/* Loading ok, check if we should attempt an auto-start */
	if (((s = getenv("autostart")) != NULL) && (strcmp(s,"yes") == 0)) {
		char *local_args[3];

		local_args[0] = argv[0];
		local_args[1] = NULL;

#ifdef CONFIG_APBOOT
#if defined(CONFIG_MSR_SUBTYPE)
        if (msr_image_type((void *)load_addr)) {
            printf("Start to boot MeshOS image\n");
            if (msr_boot_image((void *)load_addr, MSR_PRI_IMG)) {
                printf("Boot MeshOS failed.\n");
            }
        }
#endif /* CONFIG_MSR_SUBTYPE */
		rcode = aruba_basic_image_verify((void *)load_addr, 
			__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0);
		if (rcode) {
			goto out;
		}
#if defined(__SIGNED_IMAGES__)
        if ((rcode = image_verify((aruba_image_t*) load_addr))) {
			rcode = 2;
            goto out;
		}
#endif
#endif
		printf ("Automatic boot of image at addr 0x%08lX ...\n",
			load_addr);
#ifdef CONFIG_APBOOT
        load_addr += ARUBA_HEADER_SIZE;
		rcode = boot_aruba_image(load_addr);
#else
		rcode = do_bootm (cmdtp, 0, 1, local_args);
#endif
	}

#ifdef CONFIG_AUTOSCRIPT
	if (((s = getenv("autoscript")) != NULL) && (strcmp(s,"yes") == 0)) {
		printf("Running autoscript at addr 0x%08lX ...\n", load_addr);
		rcode = autoscript (load_addr);
	}
#endif
#if defined(CONFIG_APBOOT)
out:
#endif
	return rcode;
}

#if (CONFIG_COMMANDS & CFG_CMD_PING)
int do_ping (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	NetPingIP = string_to_ip(argv[1]);
	if (NetPingIP == 0) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	if (NetLoop(PING) < 0) {
		printf("ping failed; host %s is not alive\n", argv[1]);
		return 1;
	}

	printf("host %s is alive\n", argv[1]);

	return 0;
}

U_BOOT_CMD(
	ping,	2,	0,	do_ping,
	"ping - send ICMP ECHO_REQUEST to network host\n",
	"pingAddress\n"
);
#endif	/* CFG_CMD_PING */

#if (CONFIG_COMMANDS & CFG_CMD_CDP)

static void cdp_update_env(void)
{
	char tmp[16];

	if (CDPApplianceVLAN != htons(-1)) {
		printf("CDP offered appliance VLAN %d\n", ntohs(CDPApplianceVLAN));
		VLAN_to_string(CDPApplianceVLAN, tmp);
		setenv("vlan", tmp);
		NetOurVLAN = CDPApplianceVLAN;
	}

	if (CDPNativeVLAN != htons(-1)) {
		printf("CDP offered native VLAN %d\n", ntohs(CDPNativeVLAN));
		VLAN_to_string(CDPNativeVLAN, tmp);
		setenv("nvlan", tmp);
		NetOurNativeVLAN = CDPNativeVLAN;
	}

}

int do_cdp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int r;

	r = NetLoop(CDP);
	if (r < 0) {
		printf("cdp failed; perhaps not a CISCO switch?\n");
		return 1;
	}

	cdp_update_env();

	return 0;
}

U_BOOT_CMD(
	cdp,	1,	1,	do_cdp,
	"cdp - Perform CDP network configuration\n",
);
#endif	/* CFG_CMD_CDP */

#if (CONFIG_COMMANDS & CFG_CMD_SNTP)
int do_sntp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *toff;

	if (argc < 2) {
		NetNtpServerIP = getenv_IPaddr ("ntpserverip");
		if (NetNtpServerIP == 0) {
			printf ("ntpserverip not set\n");
			return (1);
		}
	} else {
		NetNtpServerIP = string_to_ip(argv[1]);
		if (NetNtpServerIP == 0) {
			printf ("Bad NTP server IP address\n");
			return (1);
		}
	}

	toff = getenv ("timeoffset");
	if (toff == NULL) NetTimeOffset = 0;
	else NetTimeOffset = simple_strtol (toff, NULL, 10);

	if (NetLoop(SNTP) < 0) {
		printf("SNTP failed: host %s not responding\n", argv[1]);
		return 1;
	}

	return 0;
}

U_BOOT_CMD(
	sntp,	2,	1,	do_sntp,
	"sntp - synchronize RTC via network\n",
	"[NTP server IP]\n"
);
#endif	/* CFG_CMD_SNTP */

#endif	/* CFG_CMD_NET */
