/* 
 **** NOTE: This implementation is fully changed for IAP (__FAT_AP__) 
 *          It can't be used as traditional dhcp_helper.
 *          Open source code design expect eth0:x interface for each 
 *          vlan, which is not there in ARUBA IAP design. ARUBA IAP design works
 *          based on opcode.
 *          Config is no more from command line option, instead 
 *          /tmp/dhcprelay.conf config file is expected.
 */

/* Author's email: simon@thekelleys.org.uk */



#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <limits.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <netdb.h>
#include <linux/types.h>
#include <linux/capability.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <assert.h>

#include "dnsmasq.h"
/* There doesn't seem to be a universally-available 
   userpace header for this. */
extern int capset(cap_user_header_t header, cap_user_data_t data);
extern int capget(cap_user_header_t header, cap_user_data_t data);
extern int
raw_udp_send(char *header, int plen, 
             struct in_addr src, struct in_addr dst,
             u_int16_t sport, u_int16_t dport,
             char *dmac);
void get_dhcp_relay_config(void);
#define OPTION_PAD               0
#define OPTION_OVERLOAD          52
#define RELAY_BAD_VLAN              ((unsigned short)(-1)) 


#define LINUX_CAPABILITY_VERSION_1  0x19980330
#define LINUX_CAPABILITY_VERSION_2  0x20071026
#define LINUX_CAPABILITY_VERSION_3  0x20080522

#ifndef FORWARD_RAW
#define FORWARD_RAW
#endif
#define RAW_DEV_NAME "br0"
#define RAW_PPP_DEV_NAME "ppp0"

#include <sys/prctl.h>
#include <net/if_arp.h>

#ifdef FORWARD_RAW
#include <signal.h>
static int is_master = 2;
char vc_ip[256] = {0};
char tmp_serv_buf[256] = {0};
int         g_br0_index = -1;
#endif

#define PIDFILE "/var/run/dhcp-helper.pid"
#define USER "nobody"

#define DHCP_CHADDR_MAX  16
#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_ALTPORT 1067
#define DHCP_CLIENT_ALTPORT 1068
#define BOOTREQUEST      1
#define BOOTREPLY        2

#define MAX_DHCP_RELAY_SERVER 3 //this has been defined in cli_config.h also

struct namelist {
    char name[IF_NAMESIZE];
    struct in_addr addr;
    struct namelist *next;
};

struct interface {
    int index;
    struct in_addr addr;
    struct interface *next;
};

#define DL_MAX_CFG_ENTRY    7
typedef struct relay_cfg_s {
    __u8            rc_enabled;
    __u16           rc_vlan;
    struct in_addr  rc_dhcp_server[MAX_DHCP_RELAY_SERVER];
    struct in_addr  rc_l_addr;
    int             rc_send_fd;
} relay_cfg_t;
relay_cfg_t g_cfg_db[DL_MAX_CFG_ENTRY];
int g_cfg_db_entry = 0;

#ifdef FORWARD_RAW
  unsigned char raw_mac_addr[ETH_ALEN];
  int raw_sock;
  int fd = -1;
#endif
static void sig_handler(int sig)
{
    /* master process */
    if (sig == SIGTERM){
        get_dhcp_relay_config();
        printf("receive a term sig\n");
#ifdef __FAT_AP__
        exit(0xaa);
#else
        exit(0);
#endif
    }
    else	
        return;
}

#define DHCP_RELAY_BUF_SIZE 256
#define DHCP_RELAY_CONF_FILE "/tmp/dhcprelay.conf"
#define DHCP_RELAY_DEBUG_FILE "/tmp/debug_dhcprelay"
#define DEBUG_DHCP_RELAY(fmt, args...) \
{\
    FILE *fp_dbg=fopen(DHCP_RELAY_DEBUG_FILE, "a+");\
    fprintf(fp_dbg,"%s:%d "fmt, __FUNCTION__,__LINE__, ##args);\
    fclose(fp_dbg);\
}
void get_dhcp_relay_config(void)
{
    FILE *fp;
    char buf[DHCP_RELAY_BUF_SIZE];
    char *str, *tmp, *gets_rs;
    int count = 0;
    int i = 0, dhcp_server_count = 0;
    char zero_ip[] = "0.0.0.0";

    //role:1 ipaddr#10.17.91.222 
    fp = fopen(DHCP_RELAY_CONF_FILE, "r");
    if (fp == NULL) {
        DEBUG_DHCP_RELAY("can't read the config file %s\n", 
                DHCP_RELAY_CONF_FILE);
        exit(1);
    }
    gets_rs = fgets(buf, DHCP_RELAY_BUF_SIZE, fp);
    if (gets_rs == NULL) {
        DEBUG_DHCP_RELAY("No entry in %s, exiting ...\n", 
                DHCP_RELAY_CONF_FILE);
        exit(1);
    }
    str = strchr(buf, ':');
    if (!str ) {
        DEBUG_DHCP_RELAY("expecting : in %s\n", buf);
        exit(1);
    }
    is_master = atoi(++str);
    DEBUG_DHCP_RELAY("The role(%d) is %s\n", 
            is_master, is_master ? "master" : "slave");

    /* make magic vlan entry for slave */
    g_cfg_db[0].rc_enabled = is_master ? 0 : 1; 
    g_cfg_db[0].rc_vlan = SWARM_MAGIC_VLAN_ID_FOR_DNSMASQ; 

    str = strchr(buf, '#'); //this is VC ip for MAGIC_VLAN
    if (!str ) {
        DEBUG_DHCP_RELAY("expecting # in %s\n", buf);
        exit(1);
    }
    str++;
    tmp = str;
    count = 0;
    while(tmp && (*tmp != 0) && (*tmp != ' ') && (*tmp != '\n')) {count++; tmp++;};
    memcpy(vc_ip, str, count);
    vc_ip[count] = 0;
    DEBUG_DHCP_RELAY("vc_ip is:%s\n", vc_ip);
    inet_aton(vc_ip, &(g_cfg_db[0].rc_dhcp_server[0]));
    inet_aton(zero_ip, &(g_cfg_db[0].rc_dhcp_server[1]));
    inet_aton(zero_ip, &(g_cfg_db[0].rc_dhcp_server[2]));
    /* Get br0 address of slave and set, done later */

    /* next line */
    //vlan#12 dhcprelay:1 dhcpserver#10.10.10.10 #10.13.11.102 #0.0.0.0 giaddr#10.17.91.222
    i++;
    if (!is_master) goto out; /* Dont parse the file if it is slave. */
second_line:
    gets_rs = fgets(buf, DHCP_RELAY_BUF_SIZE, fp);
    if (!gets_rs) {
        DEBUG_DHCP_RELAY("End of the file\n");
        g_cfg_db_entry = i;
        DEBUG_DHCP_RELAY("end of file reached, returning with entries %u\n", g_cfg_db_entry);
        return;
    }
    str = strchr(buf, '#'); //this is for vlan
    if (!str ) {
        DEBUG_DHCP_RELAY("expecting # in %s\n", buf);
        exit(1);
    }
    g_cfg_db[i].rc_vlan = atoi(++str);
    DEBUG_DHCP_RELAY("Entry[%d] vlan:%d\n", i, g_cfg_db[i].rc_vlan);

    str = strchr(str, ':'); //this is for dhcprelay
    if (!str ) {
        DEBUG_DHCP_RELAY("expecting : in %s\n", str);
        exit(1);
    }
    g_cfg_db[i].rc_enabled = atoi(++str);
    DEBUG_DHCP_RELAY("Entry[%d] dhcprelay is enabled ? %d\n", 
            i, g_cfg_db[i].rc_enabled);

    //we have 3 dhcp-relay-server
    for( dhcp_server_count = 0; dhcp_server_count < MAX_DHCP_RELAY_SERVER; dhcp_server_count++ ) {
        str = strchr(str, '#'); //this is for dhcpserver
        if (!str ) {
            DEBUG_DHCP_RELAY("expecting # in %s\n", str);
            exit(1);
        }
        str++;
        tmp = str;
        count = 0;
        while(tmp && (*tmp != 0) && (*tmp != ' ') && (*tmp != '\n')) {count++; tmp++;};
        memcpy(tmp_serv_buf, str, count);
        tmp_serv_buf[count] = 0;
        DEBUG_DHCP_RELAY("Entry[%d] DHCP server is:%s\n", i, tmp_serv_buf);
        inet_aton(tmp_serv_buf, &(g_cfg_db[i].rc_dhcp_server[dhcp_server_count]));
        str += (count + 1);
        memset(tmp_serv_buf, 0, sizeof(tmp_serv_buf));
    }

    str = strchr(str, '#'); //this is for giaddr
    if (!str ) {
        DEBUG_DHCP_RELAY("expecting # in %s\n", str);
        exit(1);
    }
    str++;
    tmp = str;
    count = 0;
    while(tmp && (*tmp != 0) && (*tmp != ' ') && (*tmp != '\n')) {count++; tmp++;};
    memcpy(tmp_serv_buf, str, count);
    DEBUG_DHCP_RELAY("Entry[%d] giaddr is %s\n", i, tmp_serv_buf);
    inet_aton(tmp_serv_buf, &(g_cfg_db[i].rc_l_addr));
    tmp_serv_buf[count] = 0;
    str += (count + 1);

    if (++i < DL_MAX_CFG_ENTRY) {
        goto second_line;
    } else {
        DEBUG_DHCP_RELAY("Max config entries(%d) are read\n", i);
    }
out:
    g_cfg_db_entry = i;
    fclose(fp);
    return;
}

static unsigned short get_vlan_id(struct dhcp_packet* mess)
{
    u8 len;
    unsigned short vlan;
    u8* opt = mess->options;
    u8* end = opt + sizeof(mess->options);

#if 0
    {
        int i;

        for(i=0; i<16; i++)
            my_syslog(MS_DHCP | LOG_WARNING, "%x  %x  %x  %x  %x  %x  %x  %x\n", opt[8*i], opt[8*i+1], opt[8*i+2], opt[8*i+3], opt[8*i+4], opt[8*i+5], opt[8*i+6], opt[8*i+7]);
    }
#endif

    opt += 4; //Ignore magic cookie, DHCP.
    while (*opt != OPT_END && opt < end)
    {
        if (*opt == OPTION_PAD) {
            opt++;
            continue;
        }
        if (*opt == OPT_ARUBA_VLAN_ID)
        {
            opt += 2;
            vlan = *((unsigned short*)(opt));
            printf("Vlan id: %d\n", vlan);
            return vlan;
        }
        len = opt[1];
        opt += (len + 2); //1 byte option and 1 byte len.
    }

    return RELAY_BAD_VLAN;
}

static unsigned char *dhcp_skip_opts(unsigned char *start, unsigned char *end)
{
  while (*start != 0xFF) {
    start += start[1] + 2;
    if(start > end) {
        return NULL;
    } 
  }
  return start;
}

/* only for use when building packet: doesn't check for bad data. */
static unsigned char *find_overload(struct dhcp_packet *mess)
{
  unsigned char *p = &mess->options[0] + sizeof(u32);

  while (*p != 0)
    {
      if (*p == OPTION_OVERLOAD)
    return p;
      p += p[1] + 2;
    }
  return NULL;
}

static unsigned char *
free_space(struct dhcp_packet *mess, unsigned char *end, int opt, int len)
{
    unsigned char *p = dhcp_skip_opts(&mess->options[0] + sizeof(u32), end);
    if(p == NULL)
        return p;

    if (p + len + 3 >= end)
        /* not enough space in options area, try and use overload, if poss */
    {
        unsigned char *overload;

        if (!(overload = find_overload(mess)) &&
                (mess->file[0] == 0 || mess->sname[0] == 0))
        {
            /* attempt to overload fname and sname areas, we've reserved space for the
               overflow option previuously. */
            overload = p;
            *(p++) = OPTION_OVERLOAD;
            *(p++) = 1;
        }

        p = NULL;

        /* using filename field ? */
        if (overload)
        {
            if (mess->file[0] == 0)
                overload[2] |= 1;

            if (overload[2] & 1)
            {
                p = dhcp_skip_opts(mess->file, end);
                if (p + len + 3 >= mess->file + sizeof(mess->file))
                    p = NULL;
            }

            if (!p)
            {
                /* try to bring sname into play (it may be already) */
                if (mess->sname[0] == 0)
                    overload[2] |= 2;

                if (overload[2] & 2)
                {
                    p = dhcp_skip_opts(mess->sname, end);
                    if (p + len + 3 >= mess->sname + sizeof(mess->file))
                        p = NULL;
                }
            }
        }

        if (!p)
            fprintf(stderr , 
                    "cannot send DHCP/BOOTP option %d: no space left in packet",
                    opt);
    }

    if (p)
    {

        *p = opt;
        p++;
        *p = len;
    }

    return p;
}

static unsigned char 
option_put(struct dhcp_packet *mess, unsigned char *end, 
        int opt, int len, unsigned int val)
{
    int i;
    unsigned char *p = free_space(mess, end, opt, len);

    if (p) {
        for (i = 0; i < len; i++) {
            p++;
            *p = val >> (8 * (len - (i + 1)));
        }
    } else {
        return 0; //FALSE
    }
    p++;
    *p = 0xFF;

    return 1; //TRUE
}


static void
get_uplink_type (char *s)
{
    FILE *uplink = fopen("/tmp/uplink", "r");

    if(uplink) {
        fgets(s, 16, uplink);
        fclose(uplink);
    }

    return;
}

int main(int argc, char **argv)
{
    int  oneopt = 1, mtuopt = IP_PMTUDISC_DONT;
    struct sockaddr_in saddr;
    size_t buf_size = sizeof(struct dhcp_packet) + 2000;
    struct dhcp_packet *packet = NULL;
    char *runfile = PIDFILE;
    char *allocated_buf = 0;
    char *user = USER;
    int debug = 0;
    unlink(DHCP_RELAY_DEBUG_FILE);
    sleep(2);
    get_dhcp_relay_config();
    struct sigaction sigact;
    int    udp_fd = -1;
    int    send_fd = -1;
    int    raw_rcv_sock = -1;
    int     i = 0, dhcp_server_count = 0, dhcp_server_present = 0;
    int     rc = 0;
    int    ppp_pkt = 0;
    int j;
    char   uplink_type[16]; 
    memset(&sigact, 0, sizeof(struct sigaction));
    sigact.sa_handler = sig_handler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGTERM, &sigact, NULL);

    while (1)
    {
        /*
           "-u <user>        Change to user <user> (defaults to %s)\n"
           "-r <file>        Write daemon PID to this file (default %s)\n"
           "-p               Use alternative ports (1067/1068)\n"
           "-d               Debug mode\n"
           "-v               Give version and copyright info and then exit\n",
           */
        int option = getopt(argc, argv, "u:r:dv");

        if (option == -1)
            break;

        switch (option) 
        {
            case 'u':
                /* "-u <user>        Change to user <user> (defaults to %s)\n" */
                if ((user = malloc(strlen(optarg) + 1)))
                    strcpy(user, optarg);
                break;

            case 'r':
                /* "-r <file>        Write daemon PID to this file (default %s)\n" */
                if ((runfile = malloc(strlen(optarg) + 1)))
                    strcpy(runfile, optarg);
                break;

            case 'd':
                debug = 1;
                break;

            case 'v':
                DEBUG_DHCP_RELAY("dhcp-helper version %s, %s\n", VERSION, COPYRIGHT);
                exit(0);

            default:
                DEBUG_DHCP_RELAY(
                        "Usage: dhcp-helper [OPTIONS]\n"
                        "Options are:\n"
                        "-u <user>        Change to user <user> (defaults to %s)\n"
                        "-r <file>        Write daemon PID to this file (default %s)\n"
                        "-d               Debug mode\n"
                        "-v               Give version and copyright info and then exit\n",
                        USER, PIDFILE);
                exit(1);

        }
    }

    if (!(allocated_buf = malloc(buf_size)))
    {
        perror("dhcp-helper: cannot allocate buffer");
        exit(1);
    }
    /* save allocate buffer ptr */
    packet = (struct dhcp_packet *)allocated_buf;

    if (udp_fd == -1 && (udp_fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("dhcp-helper: cannot create UDP socket");
        exit(1);
    }

    if (setsockopt(udp_fd, SOL_IP, IP_PKTINFO, &oneopt, sizeof(oneopt)) == -1 ||
            setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &oneopt, sizeof(oneopt)) == -1 ||
            setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &oneopt, sizeof(oneopt)) == -1 ||
            setsockopt(udp_fd, SOL_IP, IP_MTU_DISCOVER, &mtuopt, sizeof(mtuopt)) == -1)  
    {
        perror("dhcp-helper: cannot set options on DHCP socket");
        exit(1);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(DHCP_SERVER_ALTPORT);
    saddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udp_fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in))) {
        perror("dhcp-helper: cannot bind DHCP server socket");
        exit(1);
    }

    rc = fcntl(udp_fd, F_SETFL, O_NONBLOCK);
    if (rc < 0 ) {
        perror("Can't make udp_fd non-blocking\n");
        exit (1);
    }

    /* User ALTPORT for Magic VLAN relay */
    g_cfg_db[0].rc_send_fd = udp_fd;

    /* Create UDP fd for all relay servers so that it binds to the giaddr address and sends the packet out with giaddr address as sip an on SPORT 1067 */
    for (dhcp_server_count = 1; dhcp_server_count < DL_MAX_CFG_ENTRY; dhcp_server_count++)
    {
        if (g_cfg_db[dhcp_server_count].rc_vlan == 0 &&
            g_cfg_db[dhcp_server_count].rc_l_addr.s_addr == 0) {
            continue;
        }
        if ((send_fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
        {
            perror("dhcp-helper: cannot create UDP socket");
            exit(1);
        }

        if (setsockopt(send_fd, SOL_IP, IP_PKTINFO, &oneopt, sizeof(oneopt)) == -1 ||
                setsockopt(send_fd, SOL_SOCKET, SO_BROADCAST, &oneopt, sizeof(oneopt)) == -1 ||
                setsockopt(send_fd, SOL_SOCKET, SO_REUSEADDR, &oneopt, sizeof(oneopt)) == -1 ||
                setsockopt(send_fd, SOL_IP, IP_MTU_DISCOVER, &mtuopt, sizeof(mtuopt)) == -1)  
        {
            perror("dhcp-helper: cannot set options on DHCP socket");
            exit(1);
        }

        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(DHCP_SERVER_ALTPORT);
        saddr.sin_addr.s_addr = g_cfg_db[dhcp_server_count].rc_l_addr.s_addr;
        if (bind(send_fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
            DEBUG_DHCP_RELAY("bind failed for IP %x port %u", saddr.sin_addr.s_addr, saddr.sin_port);
            perror("dhcp-helper: cannot bind DHCP server socket");
            exit(1);
        }
        rc = fcntl(send_fd, F_SETFL, O_NONBLOCK);
        if (rc < 0 ) {
            perror("Can't make send_fd non-blocking\n");
            exit (1);
        }
        g_cfg_db[dhcp_server_count].rc_send_fd = send_fd;
    }
    DEBUG_DHCP_RELAY("Total db entry present %u\n", g_cfg_db_entry);
    for (j=0;j<g_cfg_db_entry;j++) {
        DEBUG_DHCP_RELAY("%u %u %x %x %u\n",g_cfg_db[j].rc_enabled,
                         g_cfg_db[j].rc_vlan, ntohl(g_cfg_db[j].rc_dhcp_server[0].s_addr),
                         ntohl(g_cfg_db[j].rc_l_addr.s_addr), g_cfg_db[j].rc_send_fd);
    }

#ifdef FORWARD_RAW
    struct sockaddr_ll my_ll;
    struct ifreq raw_ifr;
    rc = 0;

#if 1 
    // This code is setting g_br0_index ONLY; raw_rcv_sock is defined below
    // raw_sock is useless, code should be cleaned up.
    // we can check the uplink device and keep this in future
    raw_sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ASAP_TO_DHCP_HELPER)); 
    if (raw_sock < 0) {
        perror("Can't create raw socket for ETH_P_ASAP_TO_DHCP_HELPER");
        exit(1);
    }
    /* get ethernet index */
    memset(&raw_ifr, 0, sizeof (raw_ifr));
    snprintf(raw_ifr.ifr_name, sizeof (raw_ifr.ifr_name), "%s", RAW_DEV_NAME);
    rc = ioctl(raw_sock, SIOCGIFINDEX, &raw_ifr);
    if (rc < 0) {
        perror("Can't get ifindex for " RAW_DEV_NAME);
        exit(1);
    }
    memset((char*)&my_ll,0,sizeof(my_ll));
    my_ll.sll_family	 = AF_PACKET;
    my_ll.sll_protocol = htons(ETH_P_ASAP_TO_DHCP_HELPER);
    my_ll.sll_ifindex  = g_br0_index = raw_ifr.ifr_ifindex;
    printf("br0 ifindex = %d\n", g_br0_index);
    my_ll.sll_hatype	 = ARPHRD_ETHER;
    my_ll.sll_pkttype  = PACKET_HOST;
    my_ll.sll_halen	 = ETH_ALEN;

    if (bind(raw_sock,(struct sockaddr*)&my_ll,sizeof(my_ll))) {
        perror("Can't bind raw socket for " RAW_DEV_NAME);
        exit(1);
    }

    rc = fcntl(raw_sock, F_SETFL, O_NONBLOCK);
    if (rc < 0 ) {
        perror("Can't make raw_sock non-blocking\n");
        exit (1);
    }
#endif

    raw_rcv_sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ASAP_TO_DHCP_HELPER)); 
    if (raw_rcv_sock < 0) {
        perror("Can't create raw socket for ETH_P_ASAP_TO_DHCP_HELPER");
        exit(1);
    }

    memset((char*)&my_ll,0,sizeof(my_ll));
    my_ll.sll_family	 = AF_PACKET;
    my_ll.sll_protocol = htons(ETH_P_ASAP_TO_DHCP_HELPER);
    my_ll.sll_hatype	 = ARPHRD_ETHER;
    my_ll.sll_pkttype  = PACKET_HOST;
    my_ll.sll_halen	 = ETH_ALEN;

    if (bind(raw_rcv_sock,(struct sockaddr*)&my_ll,sizeof(my_ll))) {
        perror("Can't bind raw socket for " RAW_DEV_NAME);
        exit(1);
    }

    rc = fcntl(raw_rcv_sock, F_SETFL, O_NONBLOCK);
    if (rc < 0 ) {
        perror("Can't make raw_sock non-blocking\n");
        exit (1);
    }
    {
        int s;
        struct ifreq tmp_ifr;

        strcpy (tmp_ifr.ifr_name, RAW_DEV_NAME);

        if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            exit(1);
        }

        if (ioctl(s, SIOCGIFHWADDR, &tmp_ifr) >= 0) {
            memcpy(raw_mac_addr, tmp_ifr.ifr_hwaddr.sa_data, 6);
        } else {
            DEBUG_DHCP_RELAY("Can't get raw-mac of '%s'\n", tmp_ifr.ifr_name);
        }

        get_uplink_type(uplink_type);

        if(uplink_type[0] == 'p')
            strcpy (tmp_ifr.ifr_name, RAW_PPP_DEV_NAME);

        tmp_ifr.ifr_addr.sa_family = AF_INET;
        while (ioctl(s, SIOCGIFADDR, &tmp_ifr) == -1) {
            printf("Can't get IPV4 address of '%s', will retry after 60 seconds...\n", tmp_ifr.ifr_name);
            sleep(60);
        } 
        /* set magic giaddr entry for slave */
        g_cfg_db[0].rc_l_addr = ((struct sockaddr_in *) &tmp_ifr.ifr_addr)->sin_addr;
        printf("br0 ip address is(g_cfg_db[0].rc_l_addr) = %s\n",
                            inet_ntoa(g_cfg_db[0].rc_l_addr));
        close(s);
    }
#endif
    if (!debug)
    {
        FILE *pidfile;
        int i;
        struct passwd *ent_pw = getpwnam(user);
        gid_t dummy;
        struct group *gp;
        cap_user_header_t hdr = malloc(sizeof(*hdr));
        cap_user_data_t data = NULL;

        if (getuid() == 0)
        {
            if (hdr)
            {
                int capsize = 1;

                /* find version supported by kernel */
                memset(hdr, 0, sizeof(*hdr));
                capget(hdr, NULL);

                if (hdr->version != LINUX_CAPABILITY_VERSION_1)
                {
                    /* if unknown version, use largest supported version (3) */
                    if (hdr->version != LINUX_CAPABILITY_VERSION_2)
                        hdr->version = LINUX_CAPABILITY_VERSION_3;
                    capsize = 2;
                }

                if ((data = malloc(sizeof(*data) * capsize)))
                    memset(data, 0, sizeof(*data) * capsize);
            }

            if (!hdr || !data)
            {
                perror("dhcp-helper: cannot allocate memory");
                exit(1);
            }

            hdr->pid = 0; /* this process */
            data->effective = data->permitted = data->inheritable =
                (1 << CAP_NET_ADMIN) | (1 << CAP_SETGID) | (1 << CAP_SETUID);

            /* Tell kernel to not clear capabilities when dropping root */
            if (capset(hdr, data) == -1 || prctl(PR_SET_KEEPCAPS, 1) == -1)
            {
                perror("dhcp-helper: cannot set kernel capabilities");
                exit(1);
            }

            if (!ent_pw)
            {
                DEBUG_DHCP_RELAY("dhcp-helper: cannot find user %s\n", user);
                exit(1);
            };
        }	  

        /* The following code "daemonizes" the process. 
           See Stevens section 12.4 */

        if (fork() != 0 )
            _exit(0);

        setsid();

        if (fork() != 0)
            _exit(0);

        chdir("/");
        umask(022); /* make pidfile 0644 */

        /* write pidfile _after_ forking ! */
        if ((pidfile = fopen(runfile, "w")))
        {
            fprintf(pidfile, "%d\n", (int) getpid());
            fclose(pidfile);
        }

        umask(0);

        for (i=0; i<64; i++) { 
            if ((i != udp_fd) && (i != raw_sock) && (i != raw_rcv_sock)) { 
                close(i);
            }
        }

        if (getuid() == 0)
        {
            setgroups(0, &dummy);

            if ((gp = getgrgid(ent_pw->pw_gid)))
                setgid(gp->gr_gid);
            setuid(ent_pw->pw_uid); 

            data->effective = data->permitted = 1 << CAP_NET_ADMIN;
            data->inheritable = 0;

            /* lose the setuid and setgid capbilities */
            capset(hdr, data);	
        }
    }

    while (1) {
        ssize_t sz = 0;
        struct msghdr msg;
        struct iovec iov;
        struct cmsghdr *cmptr;
        struct in_pktinfo *pkt;
        unsigned short     pkt_vlan = 0;
        int    entry_hit = 0;
        static fd_set   all_read_fds;
        packet = (struct dhcp_packet *)allocated_buf;
        int     max_fd_num = 0;

        typedef struct {
            struct ether_header ethernet;
            struct ip ip __attribute__ ((__packed__));
            struct udphdr udp __attribute__ ((__packed__));
        } frame_t;
        frame_t frame;

        union {
            struct cmsghdr align; /* this ensures alignment */
            char control[CMSG_SPACE(sizeof(struct in_pktinfo))];
        } control_u;

        rc = 0;
        FD_ZERO(&all_read_fds);
        FD_SET(udp_fd, &all_read_fds);
        FD_SET(raw_rcv_sock, &all_read_fds);

        memset(&msg, 0, sizeof(msg));

        msg.msg_control = control_u.control;
        msg.msg_controllen = sizeof(control_u);
        msg.msg_name = &saddr;
        msg.msg_namelen = sizeof(saddr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_flags = 0;
        iov.iov_base = packet;
        iov.iov_len = buf_size;

        //Sandip: make raw_rcv_sock and udp_fd both non blocing, take care
        //of write eagain
        max_fd_num = (raw_rcv_sock > udp_fd) ? raw_rcv_sock : udp_fd;
        rc = select(max_fd_num + 1, &all_read_fds, NULL, NULL, NULL);
        if (rc < 1) {
            perror( "Select Failed" );
            continue;
        }

        if (FD_ISSET(raw_rcv_sock, &all_read_fds)) {
            int         got_vlan_tag = 0;
            FD_CLR(raw_rcv_sock, &all_read_fds);

            while ((sz = recvmsg(raw_rcv_sock, &msg, 0)) == -1 && errno == EINTR);
            if (sz == -1 && errno == EAGAIN) {
                continue;
            }
            if (msg.msg_flags & MSG_TRUNC) {
                DEBUG_DHCP_RELAY("Message turncated\n");
                continue;
            }

            if ((size_t)sz < sizeof(frame) ) {
                DEBUG_DHCP_RELAY("Packet is too short(%d)\n", sz); 
                continue;
            }
            
            if (sz < 0) {
                DEBUG_DHCP_RELAY("Race condition while reading the packet. sz is (%d)\n", sz); 
                continue;
            }

            ppp_pkt = 0;
            memcpy(&frame, packet, sizeof(frame));
            if (frame.ethernet.ether_type == htons(ETH_P_8021Q)) {
                memcpy(&frame.ip, (((char*)packet) + sizeof(struct ether_header) + 4), 
                                    sizeof(struct ip) + sizeof(struct udphdr));
                got_vlan_tag = 1;

            } else if (frame.ethernet.ether_type != htons(ETHERTYPE_IP)) {
                DEBUG_DHCP_RELAY("Non-IP packet(0x%x) ignoring \n",
                        frame.ethernet.ether_type); 
                ppp_pkt = 1;
                memcpy(&(frame.ip), packet, sizeof(frame) - sizeof(struct ether_header));
            } 
            if (frame.ip.ip_p != IPPROTO_UDP) {
                DEBUG_DHCP_RELAY("Non-UDP packet(0x%x) ignoring \n", frame.ip.ip_p);
                continue;
            }
            if ((frame.udp.dest != htons(DHCP_SERVER_PORT)) && 
                    (frame.udp.dest != htons(DHCP_CLIENT_PORT))) {
                DEBUG_DHCP_RELAY("Non-DHCP packet( udp dest port:%d, src port:%d) ignoring \n",
                        frame.udp.dest, frame.udp.source);
                continue;
            }
            if (ppp_pkt) {
                packet = (struct dhcp_packet *)(((char*)packet) + sizeof(frame) - sizeof(struct ether_header) 
                                                + (got_vlan_tag ? 4 : 0));
            } else {
                packet = (struct dhcp_packet *)(((char*)packet) + sizeof(frame) + (got_vlan_tag ? 4 : 0));
            }
            sz -= (sizeof(frame) + (got_vlan_tag ? 4 : 0));
            /* Check, if is configured vlan or magic vlan*/
            pkt_vlan = get_vlan_id(packet);
            pkt_vlan = htons(pkt_vlan);
            if (pkt_vlan != RELAY_BAD_VLAN) {
                for (i = 0; i < g_cfg_db_entry; i++) {
                    if (g_cfg_db[i].rc_vlan == pkt_vlan) {
                        break;
                    }
                }
            } else {
                for (i = 0; i < g_cfg_db_entry; i++) {
                    if (g_cfg_db[i].rc_l_addr.s_addr == packet->giaddr.s_addr) {
                        break;
                    }
                }
            }
            if (i == g_cfg_db_entry) {
                DEBUG_DHCP_RELAY("Got pkt from unknown vlan(%d) and with unknown giaddr (%x), ignoring\n",
                        pkt_vlan, packet->giaddr.s_addr);
                continue;
            }
            entry_hit = i;
        } else if (FD_ISSET(udp_fd, &all_read_fds)) {
            FD_CLR(udp_fd, &all_read_fds);
            while ((sz = read(udp_fd, packet, buf_size)) == -1) {
                if (errno != EINTR)
                        break;
            }
            if (sz == -1 && errno == EAGAIN) {
                continue;
            }
            for (i = (is_master ? 1 : 0); i < g_cfg_db_entry; i++) {
                if ((void *)&g_cfg_db[i].rc_l_addr != NULL && g_cfg_db[i].rc_l_addr.s_addr == packet->giaddr.s_addr) {
                    break;
                }
            }
            if (i == g_cfg_db_entry) {
                DEBUG_DHCP_RELAY("Got unwanted DHCP pkt (giaddr:%s), ignoring\n", 
                        inet_ntoa(packet->giaddr));
                continue;
            }
            entry_hit = i;
        }

        /* last ditch loop squashing. */
        if ((packet->hops++) > 20)
            continue;

        if (packet->hlen > DHCP_CHADDR_MAX)
            continue;

        if (packet->op == BOOTREQUEST)
        {
            /* message from client */
            /* already gatewayed ? */
            if (packet->giaddr.s_addr)
            {
                /* if so check if by us, to stomp on loops. */
                for (i = 0; i < g_cfg_db_entry; i++) {
                    if (g_cfg_db[i].rc_l_addr.s_addr == packet->giaddr.s_addr) {
                        break;
                    }
                }
                if (i == g_cfg_db_entry) {
                    DEBUG_DHCP_RELAY("Got our own pkt from giaddr(%s), ignoring\n",
                            inet_ntoa(packet->giaddr));
                    continue;
                }
            } else {
                /* plug in our address */
                packet->giaddr.s_addr = g_cfg_db[entry_hit].rc_l_addr.s_addr;
            }
            /* send to configured server. */
            dhcp_server_present = 0;
            for (dhcp_server_count = 0; dhcp_server_count < MAX_DHCP_RELAY_SERVER; dhcp_server_count++)
            {
                if (g_cfg_db[entry_hit].rc_dhcp_server[dhcp_server_count].s_addr) {
                    dhcp_server_present = 1; //at least one dhcp server present
                    memset(&saddr, 0, sizeof(saddr));
                    saddr.sin_family = AF_INET;
                    saddr.sin_addr.s_addr = g_cfg_db[entry_hit].rc_dhcp_server[dhcp_server_count].s_addr;
                    saddr.sin_port = htons(DHCP_SERVER_PORT);
                    if (ppp_pkt) {
                        sz += sizeof(struct ether_header);
                    } 
                    while((sendto(g_cfg_db[entry_hit].rc_send_fd, packet, sz, 0, 
                            (struct sockaddr *)&saddr, sizeof(saddr)) == -1) && 
                            ((errno == EINTR) || (errno == EAGAIN))){
                        if (errno == EAGAIN) {
                            usleep(1000); /* take it easy */
                        }
                    };
                } else {
                    //assert(0);
                }
            }
            if( dhcp_server_present == 0)
                DEBUG_DHCP_RELAY("!!! NO dhcp server configured in relay !!!\n");
        }
        else if (packet->op == BOOTREPLY)
        { 
            /* packet from server send back to client */	
            /* Sandip: need to add ARUBA_VLAN option */
            if(!option_put(packet, ((char*)packet + buf_size),
                    OPT_ARUBA_VLAN_ID, 2, g_cfg_db[entry_hit].rc_vlan)) {
                /* bug #82142 */ 
                DEBUG_DHCP_RELAY("!!! malicious packet found, dropping !!!\n");
                continue;
            }

            sz += 4;

            saddr.sin_port = htons(DHCP_CLIENT_PORT);
            msg.msg_control = NULL;
            msg.msg_controllen = 0;
            msg.msg_namelen = sizeof(saddr);
            iov.iov_len = sz;

            if (packet->ciaddr.s_addr != 0 )
            {
                saddr.sin_addr = packet->ciaddr;
#ifdef FORWARD_RAW
                raw_udp_send(iov.iov_base + sizeof(frame_t), iov.iov_len, g_cfg_db[entry_hit].rc_l_addr,
                        saddr.sin_addr,
                        htons(DHCP_SERVER_PORT), saddr.sin_port, packet->chaddr);
#else
                assert(0); /* not tested */
                while (sendmsg(fd, &msg, 0) == -1 && errno == EINTR);
#endif
            }
            else if (ntohs(packet->flags) & 0x8000 || packet->hlen > 14)
            {
                /* broadcast to 255.255.255.255 */
                msg.msg_controllen = sizeof(control_u);
                msg.msg_control = control_u.control;
                cmptr = CMSG_FIRSTHDR(&msg);
                saddr.sin_addr.s_addr = INADDR_BROADCAST;
                pkt = (struct in_pktinfo *)CMSG_DATA(cmptr);
                pkt->ipi_ifindex = g_br0_index;
                pkt->ipi_spec_dst.s_addr = 0;
                msg.msg_controllen = cmptr->cmsg_len = CMSG_LEN(sizeof(struct in_pktinfo));
                cmptr->cmsg_level = SOL_IP;
                cmptr->cmsg_type = IP_PKTINFO;
#ifdef FORWARD_RAW
                raw_udp_send(iov.iov_base + sizeof(frame_t), iov.iov_len, g_cfg_db[entry_hit].rc_l_addr,
                        saddr.sin_addr, htons(DHCP_SERVER_PORT), saddr.sin_port, "\xff\xff\xff\xff\xff\xff");
#else
                while (sendmsg(fd, &msg, 0) == -1 && errno == EINTR);
#endif
            }
#ifdef FORWARD_RAW
            else
            {
                saddr.sin_addr = packet->yiaddr;
                raw_udp_send(iov.iov_base + sizeof(frame_t), iov.iov_len, g_cfg_db[entry_hit].rc_l_addr,
                        saddr.sin_addr,
                        htons(DHCP_SERVER_PORT), saddr.sin_port, packet->chaddr);
            }
#else
            else
            {
                /* client not configured and cannot reply to ARP. 
                   Insert arp entry direct.*/
                saddr.sin_addr = packet->yiaddr;
                ifr.ifr_ifindex = g_br0_index;
                if (ioctl(fd, SIOCGIFNAME, &ifr) != -1)
                {
                    struct arpreq req;
                    *((struct sockaddr_in *)&req.arp_pa) = saddr;
                    req.arp_ha.sa_family = packet->htype;
                    memcpy(req.arp_ha.sa_data, packet->chaddr, packet->hlen);
                    strncpy(req.arp_dev, ifr.ifr_name, 16);
                    req.arp_flags = ATF_COM;
                    ioctl(fd, SIOCSARP, &req);
                }
            }
#endif
        }
    }
}
