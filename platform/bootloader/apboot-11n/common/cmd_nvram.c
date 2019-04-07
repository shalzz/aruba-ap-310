/*
 * Commands for accessing nvram flash.
 *
 */

#include <common.h>
#ifdef CONFIG_BCM95301X_no
#include <bcmnvram.h>
#include <spi.h>
#include <spi_flash.h>
#include <asm/io.h>
#include <bcmendian.h>
#ifndef CONFIG_ENV_SPI_BUS
#define CONFIG_ENV_SPI_BUS	0
#endif
#ifndef CONFIG_ENV_SPI_CS
#define CONFIG_ENV_SPI_CS		0
#endif
#ifndef CONFIG_ENV_SPI_MAX_HZ
#define CONFIG_ENV_SPI_MAX_HZ	1000000
#endif
#ifndef CONFIG_ENV_SPI_MODE
#define CONFIG_ENV_SPI_MODE	SPI_MODE_3
#endif

extern int nvram_init(void);
extern int nvram_commit(void);
extern int nvram_find(void);

struct spi_flash* spi_flash_probe(unsigned int bus, unsigned int cs,
                                  unsigned int max_hz, unsigned int spi_mode);

static int do_nvram_init(int argc, char *const argv[]) {
   int ret;
   ret = nvram_init();
   return ret;

#if 0
usage:
   puts("Usage: nvram init\n");
   return 1;
#endif
}

static int do_nvram_find(int argc, char *const argv[]) {
   int ret;
   ret = nvram_find();
   return ret;
#if 0
usage:
   puts("Usage: nvram find\n");
   return 1;
#endif
}


static int do_nvram_show(int argc, char *const argv[]) {
   char *buf, *name;
   size_t size;

   if (argc < 1) goto usage;
   buf = malloc(MAX_NVRAM_SPACE);
   if (buf == NULL) {
      return -1;
   }
   nvram_getall(buf, MAX_NVRAM_SPACE);
   for (name = buf; *name; name += strlen(name) + 1) printf("%s\n", name);
   size = sizeof(struct nvram_header) + ((uintptr)name - (uintptr)buf);
   printf("size: %d bytes (%d left)\n", size, MAX_NVRAM_SPACE - size);
   free(buf);
   return 0;

usage:
   puts("Usage: nvram show\n");
   return 1;
}

static int do_nvram_set(int argc, char *const argv[]) {
   char *name, *value;

   if (argc < 2) goto usage;
   name = argv[1];
   if ((value = strchr(name, '='))) *value++ = '\0';
   else if ((value = argv[2])) {
      if (*value == '=') value = argv[3];
   }
   if (value) nvram_set(name, value);

   return 0;

usage:
   printf("Usage: nvram set VARIABLE=VALUE\n", argv[0]);
   return 1;
}

static int do_nvram_unset(int argc, char *const argv[]) {
   char *name;

   if (argc < 2) goto usage;
   name = argv[1];
   nvram_unset(name);
   return 0;

usage:
   printf("Usage: nvram unset VARIABLE\n", argv[0]);
   return 1;
}

static int do_nvram_commit(int argc, char *const argv[]) {
//   char *buf, *name;
//   size_t size;
//   struct spi_flash *flash;
//   unsigned long offset, e_offset;
   int ret;

//   struct nvram_header *header;
//   uint32 *src, *dst;
//   uint i;

   if (argc < 1) goto usage;

   /* Regenerate NVRAM */
   ret = nvram_commit();
   return 0;

usage:
   printf("Usage: nvram commit\n", argv[0]);
   return 1;
}

static int do_nvram_get(int argc, char *const argv[]) {
   char *name, *value;

   if (argc < 2) goto usage;
   name = argv[1];
   if ((value = nvram_get(name))) printf("%s\n", value);
   return 0;

usage:
   puts("Usage: nvram get VARIABLE\n");
   return 1;
}

static int do_nvram_erase(int argc, char *const argv[]) {
    #if 0
   struct spi_flash *flash;
   unsigned long offset;
   int ret;

   offset = 0xff0000;
   flash = spi_flash_probe(CONFIG_ENV_SPI_BUS,
                           CONFIG_ENV_SPI_CS,
                           CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
   if (!flash) {
      set_default_env("!spi_flash_probe() failed");
      return 1;
   }
   ret = spi_flash_erase(flash, offset, MAX_NVRAM_SPACE);
   if (ret) {
      printf("SPI flash erase failed\n");
      return 1;
   }


   return 0;
usage:
#endif
   puts("Usage: nvram erase\n");
   return 1;
}

static int do_nvram(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]) {
   const char *cmd;

   /* need at least two arguments */
   if (argc < 2) goto usage;

   cmd = argv[1];

   if (strcmp(cmd, "init") == 0) return do_nvram_init(argc - 1, argv + 1);
   if (strcmp(cmd, "find") == 0) return do_nvram_find(argc - 1, argv + 1);
   if (strcmp(cmd, "set") == 0) return do_nvram_set(argc - 1, argv + 1);
   if (strcmp(cmd, "unset") == 0) return do_nvram_unset(argc - 1, argv + 1);
   if (strcmp(cmd, "get") == 0) return do_nvram_get(argc - 1, argv + 1);
   if (strcmp(cmd, "show") == 0) return do_nvram_show(argc - 1, argv + 1);
   if (strcmp(cmd, "erase") == 0) return do_nvram_erase(argc - 1, argv + 1);
   if (strcmp(cmd, "commit") == 0) return do_nvram_commit(argc - 1, argv + 1);

usage:
   return cmd_usage(cmdtp);
}

U_BOOT_CMD(
   nvram,	5,	1,	do_nvram,
   "nvram subsystem",
   "init	            - initialize the nvram subsystem\n"
   "nvram find	            - find the nvram subsystem\n"
   "nvram show	            - dump all variable/value pairs in the nvram\n"
   "nvram get VARIABLE 	    - retrieve and display the VALUE of VARIABLE\n"
   "nvram set VARIABLE=VALUE   - set the VARIABLE to VALUE\n"
   "nvram unset VARIABLE       - clear the VARIABLE from nvram\n"
   "nvram erase		    - erase the nvram\n"
   "nvram commit               - write nvram to flash\n"
   );
#endif
