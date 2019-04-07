#include <common.h>
#if CONFIG_POST_SYSMON
#include <command.h>

extern int sysmon_init_f (void);

extern void sysmon_reloc (void);

extern void sysmon_post_test(void);

int do_sysmon (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    sysmon_reloc();
    sysmon_init_f();
    sysmon_post_test();
    return 0;

}

U_BOOT_CMD(
	sysmon, 2,      1,      do_sysmon,
	"sysmon  - print system information\n",
	"chip    - monitor chip w83792d\n"
	" \n"
	);

#endif
