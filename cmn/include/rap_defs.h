#ifndef RAP_DEFS_H
#define RAP_DEFS_H

/* Files where rapper PID are stored and are being accessed by other 
 * modules for killing the rapper
 */
/* File for storing DHCP started rapper pid*/
#define RAPPER_PID_FILE_1   "/tmp/rapper_pid_1"
#define RAPPER_PID_FILE_2   "/tmp/rapper_pid_2"
#ifdef __FAT_AP__
#define RAPPER_DC_PID_FILE  "/tmp/rapper_pid_dc"
#endif

/* File for storing ppp started rapper pid */
#define RAPPER_OVER_PPP_PID_FILE_1   "/tmp/rapper_ppp_pid_1"
#define RAPPER_OVER_PPP_PID_FILE_2   "/tmp/rapper_ppp_pid_2"
#ifdef __FAT_AP__
#define RAPPER_DC_OVER_PPP_PID_FILE  "/tmp/rapper_ppp_pid_dc"
#endif

/* File for storing rapper pid during preemptive failback */
#define TEMP_RAPPER_PID_FILE "/tmp/temp_rapper_pid"
#ifdef __FAT_AP__
#define RAPPER_MAX_DATA_TUNNELS     6
#endif

#endif


