
#include <common.h>
#include <config.h>
#include <command.h>

#ifdef CONFIG_PSE_CONTROL

extern void poeEnable(void);
extern void poeDisable(void);

int do_poe_on (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
        poeEnable();
        printf("POE enable\n");
        return 0;
}

int do_poe_off (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
        poeDisable();
        printf("POE disable\n");
        return 0;
}

U_BOOT_CMD(
          poeon, 1, 1, do_poe_on,
          "poe     - Poweron POE\n",
          "  "
);

U_BOOT_CMD(
          poeoff, 1, 1, do_poe_off,
          "poe     - Poweroff POE\n",
          "  "
);

#endif /* CONFIG_PSE_CONTROL */

