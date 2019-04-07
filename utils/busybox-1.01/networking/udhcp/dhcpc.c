/* dhcpc.c
 *
 * udhcp DHCP client
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <sys/time.h>
#include <sys/file.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <ctype.h>

#include "dhcpd.h"
#include "dhcpc.h"
#include "options.h"
#include "clientpacket.h"
#include "clientsocket.h"
#include "script.h"
#include "socket.h"
#include "common.h"
#include "signalpipe.h"
#include "arpping.h"
#if defined(AP_PLATFORM)
#include "cmn/include/rap_defs.h"
#ifdef __FAT_AP__
#include "cmn/include/udhcp_defs.h"
#endif
#endif

static int state;
/* Something is definitely wrong here. IPv4 addresses
 * in variables of type long?? BTW, we use inet_ntoa()
 * in the code. Manpage says that struct in_addr has a member of type long (!)
 * which holds IPv4 address, and the struct is passed by value (!!)
 */
static unsigned long requested_ip; /* = 0 */
static unsigned long server_addr;
static unsigned long timeout;

#ifdef __FAT_AP__
int packet_num;
#define DHCP_CONFIG_DOWNLOADED_FILE "/tmp/tftp_downloaded_cfg"
#include "cmn/mgmt/swarm_prov_logs.h"
#include "cmn/mgmt/swarm_prov_logs.c"
#else
static int packet_num; /* = 0 */
#endif
static int fd = -1;

#define LISTEN_NONE 0
#define LISTEN_KERNEL 1
#define LISTEN_RAW 2
static int listen_mode;

struct client_config_t client_config = {
	/* Default options. */
	abort_if_no_lease: 0,
	foreground: 0,
	quit_after_lease: 0,
	background_if_no_lease: 0,
	interface: "eth0",
	pidfile: NULL,
	script: DEFAULT_SCRIPT,
	clientid: NULL,
	hostname: NULL,
	ifindex: 0,
	arp: "\0\0\0\0\0\0",		/* appease gcc-3.0 */
};

#if defined(AP_PLATFORM)
extern void reboot_ap(char *fmt, ...)
    __attribute__ ((format (printf, 1, 2)));
#endif

#ifndef IN_BUSYBOX
static void __attribute__ ((noreturn)) show_usage(void)
{
	printf(
"Usage: udhcpc [OPTIONS]\n\n"
"  -c, --clientid=CLIENTID         Set client identifier\n"
"  -C, --clientid-none             Suppress default client identifier\n"
"  -H, --hostname=HOSTNAME         Client hostname\n"
"  -h                              Alias for -H\n"
"  -f, --foreground                Do not fork after getting lease\n"
"  -b, --background                Fork to background if lease cannot be\n"
"                                  immediately negotiated.\n"
"  -i, --interface=INTERFACE       Interface to use (default: eth0)\n"
"  -n, --now                       Exit with failure if lease cannot be\n"
"                                  immediately negotiated.\n"
"  -p, --pidfile=file              Store process ID of daemon in file\n"
"  -q, --quit                      Quit after obtaining lease\n"
"  -r, --request=IP                IP address to request (default: none)\n"
"  -s, --script=file               Run file at dhcp events (default:\n"
"                                  " DEFAULT_SCRIPT ")\n"
"  -v, --version                   Display version\n"
	);
	exit(0);
}
#else
#define show_usage bb_show_usage
extern void show_usage(void) __attribute__ ((noreturn));
#endif

#define AP_MASTER_DISCOVERY_TYPE_FILE "/tmp/discover_type"

/* just a little helper */
static void change_mode(int new_mode)
{
	DEBUG(LOG_INFO, "entering %s listen mode",
		new_mode ? (new_mode == 1 ? "kernel" : "raw") : "none");
	if (fd >= 0) close(fd);
	fd = -1;
	listen_mode = new_mode;
}


/* perform a renew */
static void perform_renew(void)
{
	LOG(LOG_INFO, "Performing a DHCP renew");
	switch (state) {
	case BOUND:
		change_mode(LISTEN_KERNEL);
	case RENEWING:
	case REBINDING:
		state = RENEW_REQUESTED;
		break;
	case RENEW_REQUESTED: /* impatient are we? fine, square 1 */
		run_script(NULL, "deconfig");
	case REQUESTING:
	case RELEASED:
		change_mode(LISTEN_RAW);
		state = INIT_SELECTING;
		client_config.first_secs = 0;
		break;
	case INIT_SELECTING:
		break;
	}

	/* start things over */
	packet_num = 0;

	/* Kill any timeouts because the user wants this to hurry along */
	timeout = 0;
}


/* perform a release */
static void perform_release(void)
{
	char buffer[16];
	struct in_addr temp_addr;

	/* send release packet */
	if (state == BOUND || state == RENEWING || state == REBINDING) {
		temp_addr.s_addr = server_addr;
		sprintf(buffer, "%s", inet_ntoa(temp_addr));
		temp_addr.s_addr = requested_ip;
		LOG(LOG_INFO, "Unicasting a release of %s to %s",
				inet_ntoa(temp_addr), buffer);
		send_release(server_addr, requested_ip); /* unicast */
		run_script(NULL, "deconfig");
	}
	LOG(LOG_INFO, "Entering released state");

	change_mode(LISTEN_NONE);
	state = RELEASED;
	timeout = 0x7fffffff;
}


static void client_background(void)
{
	background(client_config.pidfile);
	client_config.foreground = 1; /* Do not fork again. */
	client_config.background_if_no_lease = 0;
}

#if defined(AP_PLATFORM)
static char hex_tab[] = {'0','1','2','3','4','5','6','7','8','9',
			 'A','B','C','D','E','F'} ;
char *
mac2str(unsigned char *addr, char *str)
{
  int i ;
  for (i = 0 ; i < 6 ; ++i) {
    str[3*i] = (hex_tab[(addr[i] & 0xf0)>>4]) ;
    str[3*i+1] = (hex_tab[(addr[i] & 0x0f)]) ;
    str[3*i+2] = ':' ;
  }
  str[17] = '\0' ;
  return str ;
}
static int aruba_reboot = 0 ;
static char aruba_reboot_reason[256];
static unsigned long saved_ip=0;
static unsigned long saved_router=0;

static void remove_pid_file (const char *file_name)
{
    char rm_cmd[51];

    snprintf(rm_cmd, 50, "rm -f %s", file_name);
    system(rm_cmd);
}

static void kill_rapper (unsigned int rapper_pid) 
{
    char kill_cmd[51];

    LOG(LOG_INFO," Ethernet lease expired. restarting rapper pid %d", rapper_pid);
    snprintf(kill_cmd, 50, "kill -9 %d", rapper_pid);
    system(kill_cmd);
}

static void kill_rapper_processes (void)
{
    FILE *fp = NULL;
    unsigned int rapper_pid = 0;
#ifdef __FAT_AP__
    char file_name[64];
    int i;
#endif

    fp = fopen(TEMP_RAPPER_PID_FILE, "r");
    if (fp != NULL) {
        /* 
         * Read the rapper_pid from file.
         * Call kill_rapper by passing rapper_pid.
         * Close the FILE pointer.
         * Remove the pid file.
         */
         fscanf(fp, "%d", &rapper_pid);
         if (rapper_pid) {
            kill_rapper(rapper_pid);
         }
         rapper_pid = 0;
         fclose(fp);
         fp = NULL;
         remove_pid_file(TEMP_RAPPER_PID_FILE);
    }

    fp = fopen(RAPPER_PID_FILE_1, "r");
    if (fp != NULL) {
        /* 
         * Read the rapper_pid from file.
         * Call kill_rapper by passing rapper_pid.
         * Close the FILE pointer.
         * Remove the pid file.
         */
         fscanf(fp, "%d", &rapper_pid);
         if (rapper_pid) {
            kill_rapper(rapper_pid);
         }
         rapper_pid = 0;
         fclose(fp);
         fp = NULL;
         remove_pid_file(RAPPER_PID_FILE_1);
    }

    fp = fopen(RAPPER_PID_FILE_2, "r");
    if (fp != NULL) {
        /* 
         * Read the rapper_pid from file.
         * Call kill_rapper by passing rapper_pid.
         * Close the FILE pointer.
         * Remove the pid file.
         */
         fscanf(fp, "%d", &rapper_pid);
         if (rapper_pid) {
            kill_rapper(rapper_pid);
         }
         rapper_pid = 0;
         fclose(fp);
         fp = NULL;
         remove_pid_file(RAPPER_PID_FILE_2);
    }
#ifdef __FAT_AP__
    for (i=0;i <RAPPER_MAX_DATA_TUNNELS; i++) {
        snprintf(file_name, sizeof(file_name), "%s_%d",RAPPER_DC_PID_FILE, (i + 1));
        fp = fopen(file_name, "r");
        if (fp != NULL) {
            /* 
             * Read the rapper_pid from file.
             * Call kill_rapper by passing rapper_pid.
             * Close the FILE pointer.
             * Remove the pid file.
             */
            fscanf(fp, "%d", &rapper_pid);
            if (rapper_pid) {
                kill_rapper(rapper_pid);
            }
            rapper_pid = 0;
            fclose(fp);
            fp = NULL;
            remove_pid_file(file_name);
        } else {
            break;
        }
    }
#endif
}

#define SAPD_REBOOTSTRAP_REASON_FILE "/tmp/rebootstrap_reason"
#define SAPD_REBOOTSTRAP_LMS_FILE "/tmp/rebootstrap_lms"
#define SAPD_REBOOTSTRAP_REASON_HISTORY_FILE "/tmp/rebootstrap_reason_history"
#define RBS_REASON_MAX_FILESIZE 4096
#define RBS_REASON_MAX_LOGLINE  128

static void
sapd_truncate_log_file(char *fn, unsigned max_size)
{
    struct stat sb;
    int r;
    int len;
    char fbuf[RBS_REASON_MAX_FILESIZE];
    int fd_log;
    off_t l;
    char *p;
    char *ep;
    int new_size;

    r = stat(fn, &sb);
    if (r == -1) {
        return;
    }

    if (sb.st_size < RBS_REASON_MAX_FILESIZE) {
        return;
    }

    fd_log = open(fn, O_RDONLY, 0666);
    if (fd_log == -1) {
        return;
    }

    new_size = RBS_REASON_MAX_FILESIZE / 2;

    /* seek to the end and back up the amount we want */
    l = lseek(fd_log, (off_t)-new_size, SEEK_END);
#if 0
    if (l == -1) {
        /* it's not clear there's much that can be done here */
    }
#endif

    len = read(fd_log, fbuf, new_size);
    close(fd_log);

    if (len == -1) {
        len = 0;
    }

    p = fbuf;
    ep = fbuf + len;

    /* skip until we find the beginning of a line */
    while (*p != '\n' && p != ep) {
        p++;
        len--;
    }
    if (p != ep) {
        p++;
        len--;
    }

    fd_log = open(fn, O_WRONLY|O_TRUNC|O_CREAT, 0666);
    if (fd_log == -1) {
        return;
    }
    r = write(fd_log, p, len);
    close(fd_log);
}

/* sapd_rbs_log() should be identical to the one in sap/nsapd/sapd_cfg.c */
static void
sapd_rbs_log(char* filename, char* fmt, va_list ap)
{
    FILE *fp;
    char buf1[32], buf2[96], buf3[RBS_REASON_MAX_LOGLINE];
    time_t t = time(NULL);
    int n;

    /* Form the string to be written to the log file - include timestamp and
     * loginfo */
    strftime(buf1, sizeof(buf1), "%F %T", localtime(&t));
    vsnprintf(buf2, sizeof(buf2), fmt, ap);
    snprintf(buf3, sizeof(buf3), "%s %s\n", buf1, buf2);

    /* If LMS file, just overwrite the previous content. Also log it in the
     * REASON file */
    if(!strncmp(filename, SAPD_REBOOTSTRAP_LMS_FILE, 
                strlen(SAPD_REBOOTSTRAP_LMS_FILE)))
    {
        /* Open the file */
        if((fp = fopen(filename, "w")) == NULL)
        {
            LOG(LOG_INFO,"Unable to open %s: %s", filename, 
                                   strerror(errno));
            return;
        }
        n = fputs(buf3, fp);
        fclose(fp);
    }

    /* Write the log to the RBS_REASON file */
    if((fp = fopen(SAPD_REBOOTSTRAP_REASON_FILE, "a")) == NULL)
    {
        LOG(LOG_INFO,"Unable to open %s: %s", SAPD_REBOOTSTRAP_REASON_FILE, 
                               strerror(errno));
        return;
    }
    n = fputs(buf3, fp);
    fclose(fp);

    sapd_truncate_log_file(SAPD_REBOOTSTRAP_REASON_FILE, RBS_REASON_MAX_FILESIZE);
}

/*aruba_rebootstrap() should be identical to the one in sapd_cfg.c*/
void
aruba_rebootstrap(char *fmt, ...)
{
    va_list ap;

    if (fmt) 
    {
        va_start(ap, fmt);
        sapd_rbs_log(SAPD_REBOOTSTRAP_REASON_FILE, fmt, ap);
        va_end(ap);
    }
    return;
}

#define DHCPC_MAKE_IP_ADDR_STR(buf, addr, iaddr)                        \
    do {                                                                \
        (iaddr).s_addr = (addr);                                        \
        strcpy((buf), inet_ntoa(iaddr));                                \
    } while (0);

static void write_provision_ipv4_file(struct dhcpc_ipv4_info *ipinfo)
{
	FILE *fp = NULL;
	uint8_t buf1[16], buf2[16], buf3[16], buf4[16], buf5[16], buf6[16];
	struct in_addr tmp;

	if (!(fp = fopen(DYNAMIC_IPV4_FILE, "w"))) {
		return;
	}

	DHCPC_MAKE_IP_ADDR_STR(buf1, ipinfo->addr.s_addr, tmp);
	DHCPC_MAKE_IP_ADDR_STR(buf2, ipinfo->subnet.s_addr, tmp);
	DHCPC_MAKE_IP_ADDR_STR(buf3, ipinfo->gatewayip.s_addr, tmp);
	DHCPC_MAKE_IP_ADDR_STR(buf4, ipinfo->dhcp_server.s_addr, tmp);
	DHCPC_MAKE_IP_ADDR_STR(buf5, ipinfo->dns_servers[0].s_addr, tmp);
	DHCPC_MAKE_IP_ADDR_STR(buf6, ipinfo->dns_servers[1].s_addr, tmp);

	fprintf(fp, "ipv4 address: %s\n" \
			"ipv4 netmask: %s\n" \
			"ipv4 gateway: %s\n" \
			"ipv4 lease: %d\n" \
			"ipv4 dhcp server: %s\n" \
			"ipv4 dns server: %s, %s\n", \
			buf1, buf2, buf3, \
			ipinfo->lease_time, \
			buf4, buf5, buf6
		   );

	fclose(fp);
}

#endif

#if defined(AP_PLATFORM)
//format:
//"organization,ip,key"
void get_airwave_info(char *data, int opt60_present)
{
    int len;
    unsigned int ampip = 0;
    unsigned int bakip = 0;
    char org[32 + 1] = { 0 };
    char ip[32] = { 0 };
    char key[32 + 1] = { 0 };
    char amp_domain[128] = { 0 };
    char ampip_str[16] = { 0 };
    char bakip_str[16] = { 0 };
    char buf[256] = { 0 };
    FILE * fp;
    char *tmp;
    char *tmp2;
    struct hostent *host;
    struct in_addr in;
    char opt_len_str[256];

    fp = fopen ("/tmp/airwave", "w");
    if (!fp) {
        LOG(LOG_ERR, "DHCP OPT 43 fopen error\n");
        return;
    }

    if (!data) {
        LOG(LOG_INFO, "DHCP OPT 43 deleted airwave config\n");
        fprintf(fp, "deleted");
        goto out;
    }

    len = strlen(data);
    if (len <= 0 || len >= 255) {
        LOG(LOG_ERR, "DHCP OPT 43 Invalid, len: %d\n", len) ;
        goto out;
    }

    memset(buf, 0, 256);
    strncpy(buf, data, len);
    LOG(LOG_INFO, "DHCP OPT 43, len: %d, buf: %s\n", len, buf);
    snprintf(opt_len_str, sizeof(opt_len_str), "%d", len);
#ifdef __FAT_AP__
    //append_provision_log(&prov_dhcp_option_obtained,  opt_len_str, buf);
#endif
    if ( ! opt60_present) {
        if ((tmp = strstr (buf, "ArubaInstantAP")) == NULL) {
            LOG(LOG_INFO, "DHCP OPT 60 does not match ArubaInstantAP, delete airwave config\n");
#ifdef __FAT_AP__
            append_provision_log(&prov_dhcp_option_error,  opt_len_str, buf);
#endif
            fprintf(fp, "deleted");
            goto out;
        }
        
        // skip first ArubaInstantAP
        tmp = strstr(tmp, ",");
        if (!tmp) {
            LOG(LOG_ERR, "DHCP OPT 43 format Invalid\n");
#ifdef __FAT_AP__
            append_provision_log(&prov_dhcp_option_error,  opt_len_str, buf);
#endif
            goto out;
        }
        tmp[0] = 0;
        tmp++;
    }
    else
        tmp = buf;

    tmp2 = tmp;
    {
        FILE *new_fp = fopen("/tmp/opt43", "w");
        if (new_fp) {
            fprintf(new_fp, "%s", buf);
            fclose(new_fp);
        }
    }

    /* orgnization */
    tmp = strstr(tmp, ",");
    if (!tmp) {
        LOG(LOG_ERR, "DHCP OPT 43 format Invalid\n");
#ifdef __FAT_AP__
        append_provision_log(&prov_dhcp_option_error,  opt_len_str, buf);
#endif
        goto out;
    }
    tmp[0] = '\0';
    strncpy(org, tmp2, sizeof(org));

    /* ip & key  / amp_domain */
    tmp2 = strstr(++tmp, ",");
    if (!tmp2) {
	/* amp_domain */
    	strncpy(amp_domain, tmp, 127);
	LOG(LOG_ERR, "amp-domain: %s\n", amp_domain);
	if (access("/etc/resolv.conf", R_OK) != -1) {
	    FILE *tmp_fp;
    	    char tbuf[256] = {0};
 	    LOG(LOG_ERR, "nameserver is ready\n");
    	    tmp_fp = fopen("/etc/resolv.conf", "r");
    	    if (tmp_fp) {
    		fread(tbuf, 1, 255, tmp_fp);
	    	fclose(tmp_fp);
	    	LOG(LOG_ERR, tbuf);
	    }
	}

	host = gethostbyname(amp_domain);
	if (host) {	
	    ampip = *((unsigned int *) host->h_addr_list[0]); 
	    in.s_addr = ampip;
	    strncpy(ampip_str, inet_ntoa(in), sizeof(ampip_str));
        ampip_str[sizeof(ampip_str)-1] = 0;
	    LOG(LOG_INFO, "AMP ip parsed from dhcp option: %s\n", ampip_str);
	    if (host->h_addr_list[1]) {
		bakip = *((unsigned int *) host->h_addr_list[1]); 
		in.s_addr = bakip;
       	strncpy(bakip_str, inet_ntoa(in), sizeof(bakip_str));
        bakip_str[sizeof(bakip_str)-1] = 0;
		LOG(LOG_INFO, "AMP backup ip parsed from dhcp option: %s\n", bakip_str);
	    }
	} else {
	    LOG(LOG_ERR, "AMP domain from dhcp option(%s) resolution error\n", amp_domain);
#ifdef __FAT_AP__
            append_provision_log(&prov_dhcp_option_error,  opt_len_str, buf);
#endif
	    goto out;
	} 
    } else {
    	tmp2[0] = '\0';
	/* ip & key or ip:port & key*/
	strncpy(ip, tmp, sizeof(ip)-1);
	strncpy(key, ++tmp2, sizeof(key)-1);
	LOG(LOG_INFO, "ams-ip: %s, length of ams-key: %d\n", ip, strlen(key));
    }

    if (ip[0]) { 
        fprintf(fp, "organization='%s' ams-ip='%s' ams-key='%s'", org, ip, key);
#ifdef __FAT_AP__
        append_provision_log(&prov_dhcp_option_parsed_simple, ip, org);
#endif
    } else if (amp_domain[0] && ampip_str[0]) {
        if (!bakip_str[0]) {
            fprintf(fp, "organization='%s' amp-domain='%s' amp-dns-ip='%s'", org, amp_domain, ampip_str);
#ifdef __FAT_AP__
            append_provision_log(&prov_dhcp_option_parsed_advanced, amp_domain, ampip_str, "N/A", org);
#endif
        } else {
            fprintf(fp, "organization='%s' amp-domain='%s' amp-dns-ip='%s' amp-dns-bakip='%s'", org, amp_domain, ampip_str, bakip_str);
#ifdef __FAT_AP__
            append_provision_log(&prov_dhcp_option_parsed_advanced, amp_domain, ampip_str, bakip_str, org);
#endif
        }
    } else {
        LOG(LOG_ERR, "DHCP OPT 43 format Invalid\n");
    }

out:
    fclose(fp);
}

void
get_activate_disable(unsigned char *data)
{
    FILE * fp;
    fp = fopen ("/tmp/opt43_activate_disable", "w");
    if (!fp) {
        LOG(LOG_ERR, "DHCP OPT 43 fopen error\n");
        return;
    }

    fprintf(fp, data);
#ifdef __FAT_AP__
    append_provision_log(&prov_dhcp_activate_disable, data);
#endif
    fclose(fp);
}

void
remove_quotes (unsigned char * str)
{
    int len;

    len = strlen (str);
    if (((str[0] == '"') || (str[0] == '\''))
            && ((str[len - 1] == '"') || (str[len - 1] == '\'')))
    {
        /* remove trailing " */
        str[len - 1] = '\0';

        /* remove leading " */
        memmove(str, str + 1, strlen(str));  
    }
}

void
remove_square_brackets (unsigned char * str)
{
    int len;

    len = strlen (str);
    if ((str[0] == '[') && (str[len - 1] == ']'))
    {
        /* remove trailing " */
        str[len - 1] = '\0';

        /* remove leading " */
        memmove(str, str + 1, strlen(str));  
    }
}
unsigned char *
find_next_parameter(unsigned char *str)
{
    unsigned char * p = str;
    int square_brackets = FALSE;
    
    while(*p != '\0')
    {
        if (!square_brackets && (*p == '['))
            square_brackets = TRUE;
        if ( square_brackets && (*p == ']'))
            square_brackets = FALSE;
        if (!square_brackets && (*p == ','))
        {
            *p = '\0';
            return (p+1);
        }
        p ++;
    }
    return NULL;
}

#define OPT43_AIRWAVE "airwave"
#define OPT43_ACTIVATE_DISABLE "activate-disable"
//option43 format example
//airwave="organization,ip,key",activate-disable=True
//and also support the old way "organization,ip,key".
void parse_option43(unsigned char *data, int opt60_present)
{
    char option43[256];
    char *option43ptr = option43;
    char *key = NULL;
    char *value = NULL;
    unsigned int len;
    unsigned int if_airwave_present = FALSE;

    len = *(data-1);
    if (len <= 0 || len >= 255) {
        LOG(LOG_ERR, "DHCP OPT 43 Invalid, len: %d\n", len) ;
        return;
    }

    memset(option43, 0, 256);
    strncpy(option43, data, len);
    LOG(LOG_INFO, "DHCP OPT 43, len: %d, option43: %s\n", len, option43);
    if (strstr(option43, "="))
    {
        while ((key = strsep(&option43ptr, "=")))
        {
            value = option43ptr;
            option43ptr = find_next_parameter(value);
            trim(value);
            remove_quotes(value);
            remove_square_brackets(value);
            trim(key);

            if (!strcasecmp(key, OPT43_AIRWAVE))
            {
                LOG(LOG_INFO, "DHCP OPT 43, airwave provisioning, value: %s\n", value);
                get_airwave_info(value, opt60_present);
                if_airwave_present = TRUE;
            }
            else if (!strcasecmp(key, OPT43_ACTIVATE_DISABLE))
            {
                if (opt60_present)
                {
                    LOG(LOG_INFO, "DHCP OPT 43, activate disable, value: %s\n", value);
                    get_activate_disable(value);
                }
                else
                {
                    LOG(LOG_INFO, "activate disable option is now only apply to Aruba Instant product.");
                }
            }
        }
    }
    else
    {
        LOG(LOG_INFO, "DHCP OPT 43 for airwave provisioning\n");
        get_airwave_info(option43, opt60_present);
    }
}


#endif

#ifdef __FAT_AP__
#define MAX_IPv4_ADDR_LEN 15
void ntp_server_changed(void)
{
    int change_fd;

    change_fd = open(DHCP_NTP_CFG_CHANGE_FILE, O_WRONLY|O_CREAT, 0600);
    if (change_fd == -1) {
        LOG(LOG_ERR, "NTP server changed. Error touching file.");
        return;
    }
    close(change_fd);
}

void parse_ntp_servers(struct dhcpMessage *dhcp_packet)
{
    const uint8_t *ntp_opt;
    uint8_t ntp_opt_len;
    char ip[MAX_IPv4_ADDR_LEN + 1], old_ip[MAX_IPv4_ADDR_LEN + 1];
    int ip_len = 0, old_ip_len = 0;
    int ntp_fd;

    ntp_fd = open(DHCP_NTP_SERVER_CFG_FILE, O_RDONLY);
    if (ntp_fd != -1) {
        ssize_t sz;

        sz = read(ntp_fd, old_ip, MAX_IPv4_ADDR_LEN);
        if (sz > 0) {
            if (old_ip[sz - 1] == '\n') {
                sz --;
            }
            old_ip[sz] = '\0';
            old_ip_len = sz;
        }
        close(ntp_fd);
    }

    ntp_opt = get_option(dhcp_packet, DHCP_NTP_SERVER);
    if (!ntp_opt) {
        // try to delete anyway
        unlink(DHCP_NTP_SERVER_CFG_FILE); 
        if (old_ip_len) {
            ntp_server_changed();
        }
        return;
    }
    ntp_opt_len = *(ntp_opt - 1);
    if (ntp_opt_len & 0x3) {
        LOG(LOG_ERR, "Invalid NTP server option length %u.", ntp_opt_len);
        return;
    }

    // One IPv4 address is supported. Just read the first IP address.
    ip_len = sprintf(ip, "%u.%u.%u.%u", ntp_opt[0], ntp_opt[1], ntp_opt[2], ntp_opt[3]);
    if (ip_len < 0) {
        LOG(LOG_ERR, "Failed to write ntp server to buffer.");
        return;
    }

    LOG(LOG_INFO, "DHCP OPT %u, len: %u, ip: %s", DHCP_NTP_SERVER, ntp_opt_len, ip);

    if (ip_len == old_ip_len && !strcmp(ip, old_ip)) {
        return;
    }

    ntp_fd = open(DHCP_NTP_SERVER_CFG_FILE, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if (ntp_fd == -1) {
        LOG(LOG_ERR, "Failed to open ntp configuration file.");
        return;
    }
    write(ntp_fd, ip, ip_len);
    close(ntp_fd);
    ntp_server_changed();
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  This function checks for venerability like executable command 
 * which can be passed to the shell script later on.
 *
 * @Param string : the string which needs to be checked against the venrability
 *
 * @Returns
 * returns 0 if its a valid string
 */
/* ----------------------------------------------------------------------------*/
int
check_string_for_venerability (char *string)
{
    char    *p = NULL;

    if (string == NULL) {
        return -1;
    }

    if (((p = strchr(string, '`')) != NULL) && (*(p+1) != '\0') && ((p = strchr((p+1), '`')) != NULL)) {
        LOG(LOG_WARNING,"DHCP OPT 66/67: Bootstrap parameters contains backticks");
        return -1;
    }

    if ((p = strstr(string, "$(")) != NULL && (*(p+2) != '\0') && (strchr(p+2, ')') != NULL)) {
        LOG(LOG_WARNING,"DHCP OPT 66/67: Bootstrap parameters has $()");
        return -1;
    }

    return 0;
}

void parse_config_server_file_opt(char *serv_opt, char *file_opt)
{
    uint8_t server_opt_len = *(serv_opt -1);
    uint8_t file_opt_len   = *(file_opt -1);
    char boot_file[256];
    char *bootf = &boot_file[0];
    if ((server_opt_len < 4) || (server_opt_len > 256) ||
        (file_opt_len < 1) || (file_opt_len > 256)) {
        /* Invalid options. dont parse further. */
        LOG(LOG_WARNING,"DHCP OPT 66/67: Invalid length for option 66/67");
        return;
    }

    memset (boot_file, 0, sizeof (boot_file));
    FILE *fp = fopen(DHCP_BOOTSTRAP_CFG_PRESENT, "w");
    if (fp) { 
        char dn_server_val[256];
        char *dn_method = "tftp";
        char *dn_server = NULL;
        char *ptr = NULL;
        char *encrypted_switch = "encrypted";
        memset (dn_server_val, 0, sizeof (dn_server_val));
        
        strncpy(dn_server_val, serv_opt, server_opt_len); 
        dn_server_val[server_opt_len] = 0;
        if (strchr(dn_server_val, ':')) {
            /* if the server name is in tftp://server format */
            dn_method = strtok(dn_server_val, "://"); 
            dn_server = strtok(NULL, "://");
           if (strcmp(dn_method,"tftp")) {
               LOG(LOG_WARNING,"DHCP OPT 66/67: not in format tftp://server");
           }
        } else {
            /* we would like to support only the above case */
            /* consider the whole name to the server name */
            dn_server = dn_server_val;
        }

        if ( !dn_method || !dn_server || check_string_for_venerability(dn_method) || check_string_for_venerability(dn_server)) {
            LOG(LOG_WARNING,"DHCP OPT 66/67: Bootstrap parameters not in the correct format");
            fclose(fp);
            return ;
        }

        strncpy(boot_file, file_opt, file_opt_len);
        boot_file[file_opt_len] = 0;
        if (check_string_for_venerability(boot_file)) {
            LOG(LOG_WARNING,"DHCP OPT 66/67: Bootstrap file parameters not in the correct format");
            fclose(fp);
            return;
        }
        /* check for encryption switch
         * OPT67: no_encrypt;filename
         */

        if ((ptr = strstr(boot_file, ";")) != NULL) {
            *ptr = '\0';
            ptr++;
            if (strcmp(boot_file, "no_encrypt") == 0) {
                encrypted_switch = "cleartext";
            } else {
                LOG(LOG_INFO,"DHCP OPT 66/67: not in the correct format, discarding");
                LOG(LOG_INFO,"DHCP OPT 66/67: %s %s %s %s",dn_method, dn_server, bootf, encrypted_switch);
                fclose(fp);
                return ;
            }
            bootf = ptr;
        }

        /* all fine; writing to the file now */
        fprintf(fp,"%s %s %s %s",dn_method, dn_server, bootf, encrypted_switch);
        LOG(LOG_INFO,"DHCP OPT 66/67: %s %s %s %s",dn_method, dn_server, bootf, encrypted_switch);
        fclose(fp);
        unlink(DHCP_CONFIG_DOWNLOADED_FILE);
    }
}

void parse_http_proxy_opt(char *proxy_opt)
{
    FILE * fp;
    int len;
    char * http_proxy_file = "/tmp/dhcp_http_proxy";
    char protocol[16] = {0};
    char proxy_server[512] = {0};
    char server[512] = {0};
    char port[16] = {0};
    char server_tmp[512] = {0};
    char port_tmp[16] = {0};
    char server_key[16] = {0};
    char port_key[16] = {0};

    len = *(proxy_opt-1);
    fp = fopen(http_proxy_file, "w");
    if (fp != NULL){
        strncpy(proxy_server, proxy_opt, len);
    } else {
        LOG(LOG_ERR, "dhcp_http_proxy file open failed");
        return;
    }   
    LOG(LOG_INFO, "DHCP http proxy, buf: %s", proxy_server);
    
    if (!(strstr(proxy_server, ","))) {
        LOG(LOG_ERR, "DHCP proxy format error, should be 'server=xxx,port=xxx'");
        fclose(fp);
        return;
    }

    sscanf(proxy_server, "%[^,],%s", server_tmp, port_tmp);

    if ( (!(strstr(server_tmp, "server="))) || (!(strstr(port_tmp, "port="))) ) {
        LOG(LOG_ERR, "DHCP proxy server format error. should be 'server=xxx,port=xxx'");
        fclose(fp);
        return;
    }
    sscanf(server_tmp, "%[^=]=%s", server_key, server);
    //check the server is domain. eg: http://xxxxx
    if ((strstr(server, "://"))) {
        sscanf(server, "%[^://]://%s", protocol, server_tmp);
        strncpy(server, server_tmp, sizeof(server));
        if (strlen(server) > 256) {
            LOG(LOG_ERR, "DHCP proxy server too long, cannot be more than 256 characters");
            fclose(fp);
            return;
        }
    }
    
    sscanf(port_tmp, "%[^=]=%s", port_key, port);
    if ((atoi(port) > 65535) || (atoi(port) < 1)) {
        LOG(LOG_ERR, "DHCP proxy port should be 1-65535");
        fclose(fp);
        return;
    }
    LOG(LOG_INFO, "dhcp_http_proxy server:%s, port:%s", server, port);
    
    fprintf(fp, "%s,%s", server, port);
    
     append_provision_log(&prov_dhcp_proxy_server_parsed, server, port);

    fclose(fp);
}

#endif
#ifdef COMBINED_BINARY
int udhcpc_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	uint8_t *temp, *message;
    char *opt60 = NULL;
#ifdef __FAT_AP__
    char *opt66 = NULL;
    char *opt67 = NULL;
    char *opt148 = NULL;
#endif
	unsigned long t1 = 0, t2 = 0, xid = 0;
	unsigned long start = 0, lease = 0;
	fd_set rfds;
	int retval;
	struct timeval tv;
	int c, len;
	struct dhcpMessage packet;
	struct in_addr temp_addr;
	long now;
	int max_fd;
	int sig;
	int no_clientid = 0;
#if defined(AP_PLATFORM)
	u_int32_t new_router=0 ;
	struct dhcpc_ipv4_info ipinfo;
#endif

	static const struct option arg_options[] = {
		{"clientid",	required_argument,	0, 'c'},
		{"clientid-none", no_argument,		0, 'C'},
		{"foreground",	no_argument,		0, 'f'},
		{"background",	no_argument,		0, 'b'},
		{"hostname",	required_argument,	0, 'H'},
		{"hostname",    required_argument,      0, 'h'},
		{"interface",	required_argument,	0, 'i'},
		{"now", 	no_argument,		0, 'n'},
		{"pidfile",	required_argument,	0, 'p'},
		{"quit",	no_argument,		0, 'q'},
		{"request",	required_argument,	0, 'r'},
		{"script",	required_argument,	0, 's'},
		{"version",	no_argument,		0, 'v'},
		{0, 0, 0, 0}
	};
	/* get options */
	while (1) {
		int option_index = 0;
		c = getopt_long(argc, argv, "c:CfbH:h:i:np:qr:s:v", arg_options, &option_index);
		if (c == -1) break;

		switch (c) {
		case 'c':
			if (no_clientid) show_usage();
			len = strlen(optarg) > 255 ? 255 : strlen(optarg);
			if (client_config.clientid) free(client_config.clientid);
			client_config.clientid = xmalloc(len + 2);
			client_config.clientid[OPT_CODE] = DHCP_CLIENT_ID;
			client_config.clientid[OPT_LEN] = len;
			client_config.clientid[OPT_DATA] = '\0';
			strncpy(client_config.clientid + OPT_DATA, optarg, len);
			break;
		case 'C':
			if (client_config.clientid) show_usage();
			no_clientid = 1;
			break;
		case 'f':
			client_config.foreground = 1;
			break;
		case 'b':
			client_config.background_if_no_lease = 1;
			break;
		case 'h':
		case 'H':
			len = strlen(optarg) > 255 ? 255 : strlen(optarg);
			if (client_config.hostname) free(client_config.hostname);
			client_config.hostname = xmalloc(len + 2);
			client_config.hostname[OPT_CODE] = DHCP_HOST_NAME;
			client_config.hostname[OPT_LEN] = len;
			strncpy(client_config.hostname + 2, optarg, len);
			break;
		case 'i':
			client_config.interface =  optarg;
			break;
		case 'n':
			client_config.abort_if_no_lease = 1;
			break;
		case 'p':
			client_config.pidfile = optarg;
			break;
		case 'q':
			client_config.quit_after_lease = 1;
			break;
		case 'r':
			requested_ip = inet_addr(optarg);
			break;
		case 's':
			client_config.script = optarg;
			break;
		case 'v':
			printf("udhcpcd, version %s\n\n", VERSION);
			return 0;
			break;
		default:
			show_usage();
		}
	}

	/* Start the log, sanitize fd's, and write a pid file */
	start_log_and_pid("udhcpc", client_config.pidfile);

	if (read_interface(client_config.interface, &client_config.ifindex,
			   NULL, client_config.arp) < 0)
		return 1;

	/* if not set, and not suppressed, setup the default client ID */
	if (!client_config.clientid && !no_clientid) {
		client_config.clientid = xmalloc(6 + 3);
		client_config.clientid[OPT_CODE] = DHCP_CLIENT_ID;
		client_config.clientid[OPT_LEN] = 7;
		client_config.clientid[OPT_DATA] = 1;
		memcpy(client_config.clientid + 3, client_config.arp, 6);
	}

	/* setup the signal pipe */
	udhcp_sp_setup();

	state = INIT_SELECTING;
	client_config.first_secs = 0;
	run_script(NULL, "deconfig");
	change_mode(LISTEN_RAW);

	for (;;) {

		tv.tv_sec = timeout - uptime();
		tv.tv_usec = 0;

		if (listen_mode != LISTEN_NONE && fd < 0) {
			if (listen_mode == LISTEN_KERNEL)
				fd = listen_socket(INADDR_ANY, CLIENT_PORT, client_config.interface);
			else
				fd = raw_socket(client_config.ifindex);
			if (fd < 0) {
				LOG(LOG_ERR, "FATAL: couldn't listen on socket, %m");
				return 0;
			}
		}
		max_fd = udhcp_sp_fd_set(&rfds, fd);

		if (tv.tv_sec > 0) {
			DEBUG(LOG_INFO, "Waiting on select...");
			retval = select(max_fd + 1, &rfds, NULL, NULL, &tv);
		} else retval = 0; /* If we already timed out, fall through */

		now = uptime();
		if (retval == 0) {
			/* timeout dropped to zero */
			switch (state) {
			case INIT_SELECTING:
                        {
                                char cmd[64];
                                snprintf(cmd, sizeof(cmd)-1, "/sbin/ifconfig %s 0.0.0.0",
                                         client_config.interface);
#ifdef __FAT_AP__
                                if (access("/tmp/default_gotip", R_OK) != 0)
                                    system(cmd);
#else
                                system(cmd);
#endif
				if (packet_num < 3) {
					if (packet_num == 0) 
						xid = random_xid();


					/* send discover packet */
#ifdef __FAT_AP__
                    append_provision_log(&prov_dhcp_discover_sent, NULL);
#endif
					send_discover(xid, requested_ip); /* broadcast */

					timeout = now + 2;
					packet_num++;
				} else {
#ifndef __FAT_AP__
					run_script(NULL, "leasefail");
#endif
					if (client_config.background_if_no_lease) {
						LOG(LOG_INFO, "No lease, forking to background.");
						client_background();
					} else if (client_config.abort_if_no_lease) {
						LOG(LOG_INFO, "No lease, failing.");
						return 1;
				  	}
					/* wait to try again */
					packet_num = 0;
					timeout = now + 20;
				}
				break;
                        }
			case RENEW_REQUESTED:
			case REQUESTING:
				if (packet_num < 3) {
					/* send request packet */
					if (state == RENEW_REQUESTED)
						send_renew(xid, server_addr, requested_ip); /* unicast */
					else send_selecting(xid, server_addr, requested_ip); /* broadcast */

					timeout = now + ((packet_num == 2) ? 10 : 2);
					packet_num++;
				} else {
					/* timed out, go back to init state */
					if (state == RENEW_REQUESTED) run_script(NULL, "deconfig");
					state = INIT_SELECTING;
					client_config.first_secs = 0;
					timeout = now;
					packet_num = 0;
					change_mode(LISTEN_RAW);
				}
				break;
			case BOUND:
				/* Lease is starting to run out, time to enter renewing state */
				state = RENEWING;
				client_config.first_secs = 0;
				change_mode(LISTEN_KERNEL);
				DEBUG(LOG_INFO, "Entering renew state");
				/* fall right through */
			case RENEWING:
				/* Either set a new T1, or enter REBINDING state */
				if ((t2 - t1) <= (lease / 14400 + 1)) {
					/* timed out, enter rebinding state */
					state = REBINDING;
#if defined(AP_PLATFORM)
					aruba_reboot = 1;
                                        snprintf(aruba_reboot_reason, sizeof(aruba_reboot_reason),
                                                "Entering REBINDING state; t1 %lu t2 %lu lease %lu",
                                                t1, t2, lease);
#endif
					timeout = now + (t2 - t1);
					DEBUG(LOG_INFO, "Entering rebinding state");
				} else {
#if defined(AP_PLATFORM)
					aruba_reboot = 1;
                                        snprintf(aruba_reboot_reason, sizeof(aruba_reboot_reason),
                                                "RENEWING state; t1 %lu t2 %lu lease %lu",
                                                t1, t2, lease);
#endif
					/* send a request packet */
					send_renew(xid, server_addr, requested_ip); /* unicast */

					t1 = (t2 - t1) / 2 + t1;
					timeout = t1 + start;
				}
				break;
			case REBINDING:
				/* Either set a new T2, or enter INIT state */
				if ((lease - t2) <= (lease / 14400 + 1)) {
#if defined(AP_PLATFORM)
					/* kill rapper process to kickin reboostrap */
					kill_rapper_processes();
					aruba_rebootstrap("DHCP Lease expired");
#endif
					/* timed out, enter init state */
					state = INIT_SELECTING;
					client_config.first_secs = 0;
					LOG(LOG_INFO, "Lease lost, entering init state");
					run_script(NULL, "deconfig");
					timeout = now;
					packet_num = 0;
					change_mode(LISTEN_RAW);
				} else {
					/* send a request packet */
					send_renew(xid, 0, requested_ip); /* broadcast */

					t2 = (lease - t2) / 2 + t2;
					timeout = t2 + start;
				}
				break;
			case RELEASED:
				/* yah, I know, *you* say it would never happen */
				timeout = 0x7fffffff;
				break;
			}
		} else if (retval > 0 && listen_mode != LISTEN_NONE && FD_ISSET(fd, &rfds)) {
			/* a packet is ready, read it */

			if (listen_mode == LISTEN_KERNEL)
				len = get_packet(&packet, fd);
			else len = get_raw_packet(&packet, fd);

			if (len == -1 && errno != EINTR) {
				DEBUG(LOG_INFO, "error on read, %m, reopening socket");
				change_mode(listen_mode); /* just close and reopen */
			}
			if (len < 0) continue;

			if (packet.xid != xid) {
				DEBUG(LOG_INFO, "Ignoring XID %lx (our xid is %lx)",
					(unsigned long) packet.xid, xid);
				continue;
			}
			/* Ignore packets that aren't for us */
			if (memcmp(packet.chaddr, client_config.arp, 6)) {
				DEBUG(LOG_INFO, "packet does not have our chaddr -- ignoring");
				continue;
			}

			if ((message = get_option(&packet, DHCP_MESSAGE_TYPE)) == NULL) {
				DEBUG(LOG_ERR, "couldnt get option from packet -- ignoring");
				continue;
			}

			switch (state) {
			case INIT_SELECTING:
				/* Must be a DHCPOFFER to one of our xid's */
				if (*message == DHCPOFFER) {
                    temp = get_option(&packet, DHCP_SERVER_ID);
                    if (temp) {
                        server_addr = *(uint32_t*)temp;
						xid = packet.xid;
						requested_ip = packet.yiaddr;

						/* enter requesting state */
						state = REQUESTING;
						timeout = now;
						packet_num = 0;
					} else {
						DEBUG(LOG_ERR, "No server ID in message");
					}
				}
				break;
			case RENEW_REQUESTED:
			case REQUESTING:
			case RENEWING:
			case REBINDING:
				if (*message == DHCPACK) {
#if defined(AP_PLATFORM)
					memset(&ipinfo, 0, sizeof(ipinfo));
                    temp = get_option(&packet, DHCP_SERVER_ID);
                    if (temp) {
                        unsigned long tmp_server_addr = 0;
                        tmp_server_addr = *(uint32_t*)temp; 
                        if (server_addr != tmp_server_addr) {
                            /* in customer network, sometime PXE-ACK packet disturb our normal process,
                            * so those PXE-ACK will be ignored (see bug95893)*/
                            if ((opt60 = get_option(&packet, DHCP_VENDOR))
                                && (strncmp(opt60, "PXEClient", sizeof("PXEClient")-1) == 0)) {
                                LOG(LOG_INFO,"Received DHCP ACK with vendor 'PXEClinet' "
                                    "for other server, ignoring");
                                break;
                            }
                            /* update DHCP server IP for bug 110095 */
                            server_addr = *(uint32_t*)temp;
                            DEBUG(LOG_ERR, "in ack, server IP updated.");
                        }

						ipinfo.dhcp_server.s_addr = server_addr;
                    }
#endif
                    
                    temp = get_option(&packet, DHCP_LEASE_TIME);
                    if (!temp) {
						LOG(LOG_ERR, "No lease time with ACK, using 1 hour lease");
						lease = 60 * 60;
					} else {
                        lease = ntohl(*(uint32_t*)temp);
					}
#if defined(AP_PLATFORM) && !defined(__FAT_AP__)
/* RFC 2131 3.1 paragraph 5:
 * "The client receives the DHCPACK message with configuration
 * parameters. The client SHOULD perform a final check on the
 * parameters (e.g., ARP for allocated network address), and notes
 * the duration of the lease specified in the DHCPACK message. At this
 * point, the client is configured. If the client detects that the
 * address is already in use (e.g., through the use of ARP),
 * the client MUST send a DHCPDECLINE message to the server and restarts
 * the configuration process..." */
					{
#define PROBE_NUM 3
						int i = 0;
						int conflict = 0;
						for (i = 0; i < PROBE_NUM || conflict; i++) {
							if (!arpping(packet.yiaddr, 0,
									client_config.arp, client_config.interface)) {
								DEBUG(LOG_ERR, "offered address is in use "
									"(got ARP reply), declining");
								send_decline(xid, server_addr, packet.yiaddr);
				
								change_mode(LISTEN_RAW);
								state = INIT_SELECTING;
								client_config.first_secs = 0; /* make secs field count from 0 */
								requested_ip = 0;
								timeout = 20;
								packet_num = 0;
								conflict = 1;
								break; /* back to main loop */
							}
						}
						if (conflict) continue;
					}
#endif // if defined(AP_PLATFORM) && defined(__IAP__)


#if defined(AP_PLATFORM)
					ipinfo.lease_time = lease;
#endif

					/* enter bound state */
					t1 = lease / 2;

					/* little fixed point for n * .875 */
					t2 = (lease * 0x7) >> 3;
					temp_addr.s_addr = packet.yiaddr;
					LOG(LOG_INFO, "Lease of %s obtained, lease time %ld",
						inet_ntoa(temp_addr), lease);
#ifdef __FAT_AP__
                    {
                        char ip_addr_str[256];
                        char lease_time_str[256];
                        strncpy(ip_addr_str, inet_ntoa(temp_addr), sizeof(ip_addr_str));
                        ip_addr_str[sizeof(ip_addr_str)-1] = 0;
                        snprintf(lease_time_str, sizeof(lease_time_str), "%ld", lease);
                        append_provision_log(&prov_dhcp_lease_obtained,  ip_addr_str, lease_time_str);
                    }
#endif
					start = now;
					timeout = t1 + start;
#if defined(AP_PLATFORM)
					if ((temp = get_option(&packet, DHCP_ROUTER)))
						new_router = *(u_int32_t *)temp ;
					if (aruba_reboot) {
						if ((saved_ip && 
						     (saved_ip != packet.yiaddr)) ||
						    (saved_router &&
						     (saved_router != new_router))) {
#ifdef __FAT_AP__
                                                    system ("touch /tmp/default_gw_changed");
#else
                                                    char buf1[16], buf2[16], buf3[16], buf4[16];
                                                    struct in_addr aa;
                                                    DHCPC_MAKE_IP_ADDR_STR(buf1, saved_ip, aa);
                                                    DHCPC_MAKE_IP_ADDR_STR(buf2, packet.yiaddr, aa);
                                                    DHCPC_MAKE_IP_ADDR_STR(buf3, saved_router, aa);
                                                    DHCPC_MAKE_IP_ADDR_STR(buf4, new_router, aa);
							reboot_ap("%s (saved_ip %s yiaddr %s saved_router %s new_router %s)",
                                                                aruba_reboot_reason, buf1, buf2, buf3, buf4);
#endif
                                                }
					}
					saved_ip = packet.yiaddr ;
					saved_router = new_router ;
                    {
                        /* Write save_router into "/tmp/dhcp_nexthop" file */
                        FILE *fp;
                        char nh_ip[32];
                        struct in_addr addr;

                        addr.s_addr = saved_router;
                        memset(nh_ip, '\0', 32);
                        strcpy(nh_ip, inet_ntoa(addr));
                        fp = fopen("/tmp/dhcp_nexthop", "w");
                        if (fp != NULL)
                        {
                            fprintf (fp, "%s\n", nh_ip);
                            fclose(fp);
                        }
                    }

					ipinfo.addr.s_addr = saved_ip;
					ipinfo.gatewayip.s_addr = saved_router;
#endif
					requested_ip = packet.yiaddr;
					run_script(&packet,
						   ((state == RENEWING || state == REBINDING) ? "renew" : "bound"));
#if defined(AP_PLATFORM)
                    if (!(temp = get_option(&packet, DHCP_VSA))) {
					  // master information not found...
					} else {
					  FILE * fp;
					  FILE * cfg_fp;
					  char   ip[32];
					  unsigned long switch_ip;
					  struct in_addr addr;
                      FILE* dis_type_fp = NULL;
					  char dis_type_str[16] = {0};
					  int need_cover = 0;

					  len = *(temp - 1);
					  if (!(opt60 = get_option(&packet, DHCP_VENDOR))) {
                          LOG(LOG_INFO,"DHCP OPT 60 NULL\n");
                      } else if (strncmp(opt60, "ArubaAP", sizeof("ArubaAP")-1) == 0) {
                        LOG(LOG_DEBUG, "DHCP OPT 60 is ArubaAP\n") ;
					    ip[0] = 0;
					    if (len == 4) {
					      memcpy (&switch_ip, temp, len);
					      addr.s_addr = switch_ip;
					      strcpy (ip, inet_ntoa (addr));
					    }
					    else {
					      int i1, i2, i3, i4;
					      memcpy (ip, temp, len);
					      ip[len] = 0;
					      
					      if (sscanf (ip, "%d.%d.%d.%d", 
							  &i1, &i2, &i3, &i4) != 4) {
						ip[0] = 0;
					      }
					      else {
						if ((i1 >= 0) && (i1 <= 255) &&
						    (i2 >= 0) && (i2 <= 255) &&
						    (i3 >= 0) && (i3 <= 255) &&
						    (i4 >= 0) && (i4 <= 255)) {
						  // valid IP
						}
						else
						  ip[0] = 0;
					      }
					    }
					    if (ip[0] != 0) {
					      cfg_fp = fopen ("/tmp/dhcp_cfg", "w");
					      fp = fopen ("/tmp/master", "r");
					      if (fp == NULL) {
						    fp = fopen ("/tmp/master", "w");
						    fprintf (fp, "%s\n", ip);
						    fclose (fp);
						
						    fprintf (cfg_fp, "master=%s\n", ip);

                            dis_type_fp = fopen(AP_MASTER_DISCOVERY_TYPE_FILE, "w");
                            if (dis_type_fp) {
                                fprintf(dis_type_fp, "DHCP");
                                fclose(dis_type_fp);
                            }
					      } else {
						    fclose(fp);
                            dis_type_fp = fopen(AP_MASTER_DISCOVERY_TYPE_FILE, "r");
                            if (dis_type_fp) {
								if(!feof(dis_type_fp)){
								    fgets(dis_type_str, 16, dis_type_fp);
								    if (NULL != strstr(dis_type_str, "ADP")) {
									    need_cover = 1;
										LOG(LOG_INFO, "Master has been discovered by ADP,need to cover.\n") ;
									} else if (NULL != strstr(dis_type_str, "DNS")) {
									    need_cover = 1;
										LOG(LOG_INFO, "Master has been discovered by DNS,need to cover.\n") ;
									}
								}
                                fclose(dis_type_fp);
                            }
						    if (need_cover) {
								fprintf (cfg_fp, "master=%s\n", ip);
                                dis_type_fp = fopen(AP_MASTER_DISCOVERY_TYPE_FILE, "w");
							    if (dis_type_fp) {
								    fprintf(dis_type_fp, "DHCP");
                                    fclose(dis_type_fp);
								}
								fp = fopen ("/tmp/master", "w");
								fprintf (fp, "%s\n", ip);
								fclose (fp);
							}
					      }
					      fp = fopen ("/tmp/serverip", "r");
					      if (fp == NULL) {
						    fp = fopen ("/tmp/serverip", "w");
						    fprintf (fp, "%s\n", ip);
						    fclose (fp);
						
						    fprintf (cfg_fp, "serverip=%s\n", ip);
					      } else {
						    fclose(fp);
					      }
					      
					      fclose (cfg_fp);
					      
					    }
					  }else if (strncmp(opt60, "ArubaInstantAP", sizeof("ArubaInstantAP")-1) == 0) {
                        LOG(LOG_DEBUG, "DHCP OPT 60 is ArubaInstantAP\n") ;
                        parse_option43(temp, 1);
                      } else {
                          /* make the  logic be consistent with previous ones */ 
                          parse_option43(temp, 0);
                      }
                    }
#ifndef __FAT_AP__
/* This part merging from AOS6.5 makes IAP dhcp option43 not work, should be ignored in instant code */
					temp = get_option(&packet, DHCP_SUBNET);
					if (temp) {
						ipinfo.subnet.s_addr = *(uint32_t *)temp;
					}

					temp = get_option(&packet, DHCP_DNS_SERVER);
					if (temp) {

						len = *(temp - 1);

						ipinfo.dns_servers[0].s_addr = *(uint32_t *)temp;
						if (len >= MAX_DNS_NUM * sizeof(struct in_addr)) {
							ipinfo.dns_servers[1].s_addr = *((uint32_t *)temp + 1);
						}
					}

#endif
					write_provision_ipv4_file(&ipinfo);

#ifdef __FAT_AP__
					if ((opt66 = get_option(&packet, DHCP_TFTP_SERVER)) &&
                        (opt67 = get_option(&packet, DHCP_BOOTFILE))) {
                        /* This confirms we have TFTP Server and Boot file presnet in TFTP. */
                        parse_config_server_file_opt(opt66, opt67);
                    }
                    parse_ntp_servers(&packet);

                    if ((opt148 = get_option(&packet, DHCP_HTTP_PROXY))) {
                        opt60 = get_option(&packet, DHCP_VENDOR);
					    if (opt60)
                            if (strncmp(opt60, "ArubaInstantAP", sizeof("ArubaInstantAP")-1) == 0) {
                                parse_http_proxy_opt(opt148);
                            }
                    }
#endif
                    // Touch this file to announce that dhcp got IP
                    system("touch /tmp/dhcp_gotip");
#endif

					state = BOUND;
					change_mode(LISTEN_NONE);
					if (client_config.quit_after_lease)
						return 0;
					if (!client_config.foreground)
						client_background();

				} else if (*message == DHCPNAK) {
#if defined(AP_PLATFORM)
                    if ((temp = get_option(&packet, DHCP_SERVER_ID)) &&
                        (memcmp (&server_addr, temp, 4) != 0)) {
                        LOG(LOG_INFO, 
                            "Received DHCP NAK for other server, ignoring");
                        break;
                    }
#endif
					/* return to init state */
					LOG(LOG_INFO, "Received DHCP NAK");
					run_script(&packet, "nak");
					if (state != REQUESTING)
						run_script(NULL, "deconfig");
					state = INIT_SELECTING;
					client_config.first_secs = 0;
					timeout = now;
					requested_ip = 0;
					packet_num = 0;
					change_mode(LISTEN_RAW);
					sleep(3); /* avoid excessive network traffic */
				}
				break;
			/* case BOUND, RELEASED: - ignore all packets */
			}
		} else if (retval > 0 && (sig = udhcp_sp_read(&rfds))) {
			switch (sig) {
			case SIGUSR1:
				client_config.first_secs = 0;
                                perform_renew();
				break;
			case SIGUSR2:
                                perform_release();
				break;
			case SIGTERM:
				LOG(LOG_INFO, "Received SIGTERM");
				return 0;
			}
		} else if (retval == -1 && errno == EINTR) {
			/* a signal was caught */
		} else {
			/* An error occured */
			DEBUG(LOG_ERR, "Error on select");
		}

	}
	return 0;
}

#ifdef __FAT_AP__
int
prov_log_disabled(provision_state_t st)
{
    FILE *fp = fopen(VALID_SSID_DURING_BOOT_FILE, "r");
    if (fp) {
        fclose(fp);
        return TRUE;
    }

    return FALSE;
}
#endif
