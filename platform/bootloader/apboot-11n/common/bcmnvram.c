/*
 * NVRAM variable manipulation (Linux kernel half)
 *
 * Copyright (C) 2012, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: nvram_linux.c,v 1.10 2010-09-17 04:51:19 $
 */

#include <common.h>
#ifdef CONFIG_BCM95301X_no
#include <linux/stddef.h>
#include <bcmnvram.h>
#include <asm/arch/bcmutils.h>
#include <asm/arch/bcmutils-nvram.h>
// #include "bcmutils.h"
#include <bcmendian.h>
#include <bcmnvram.h>
#include <sbsdram.h>
#include "configs/bcm95301x_svk.h"

extern ulong BCMROMFN(bcm_strtoul)(const char *cp, char **endp, uint base);
struct nvram_tuple *(_nvram_realloc)(struct nvram_tuple *t, const char *name, const char *value);
int _nvram_read(void *buf);
static void(nvram_free)(void);
char *nvram_get(const char *name);
int nvram_set(const char *name, const char *value);
int nvram_unset(const char *name);
int nvram_getall(char *buf, int count);
int _nvram_commit(struct nvram_header *header);
int _nvram_init(void);
void nvram_exit(void);
int nvram_commit(void);
extern int writeenv(size_t, u_char *);
extern int readenv(size_t, u_char *);
uint8 nvram_calc_crc(struct nvram_header *nvh);

static struct nvram_tuple *(nvram_hash)[257];
static struct nvram_tuple *nvram_dead;
static unsigned char nflash_nvh[CONFIG_NVRAM_SIZE];

static struct nvram_header *nvram_header = NULL;

struct nvram_tuple *_nvram_realloc(struct nvram_tuple *t, const char *name, const char *value)
{
	if (!(t = malloc(sizeof(struct nvram_tuple) + strlen(name) + 1 + strlen(value) + 1))) {
		printf("_nvram_realloc: our of memory\n");
		return NULL;
	}
    //printf("_nvram_realloc: %x size %x\n", t, sizeof(struct nvram_tuple) + strlen(name) + 1 + strlen(value) + 1);

	/* Copy name */
	t->name = (char *) &t[1];
	strcpy(t->name, name);

	/* Copy value */
	t->value = t->name + strlen(name) + 1;
	strcpy(t->value, value);

	return t;
}

/* Free all tuples. */
static void nvram_free(void)
{
	uint i;
	struct nvram_tuple *t, *next;

	/* Free hash table */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = next) {
			next = t->next;
            //printf("hash free %x\n", t);
            free(t);
		}
		nvram_hash[i] = NULL;
	}

	/* Free dead table */
	for (t = nvram_dead; t; t = next) {
		next = t->next;
        //printf("dead free %x", t);
        free(t);
	}
	nvram_dead = NULL;
}

/* String hash */
static inline uint hash(const char *s)
{
	uint hashval = 0;

	while (*s)
		hashval = 31 *hashval + *s++;

	return hashval;
}

/* (Re)initialize the hash table. Should be locked. */
static int nvram_rehash(struct nvram_header *header)
{
	char buf[] = "0xXXXXXXXX", *name, *value, *end, *eq;

	/* (Re)initialize hash table */
	nvram_free();

	/* Parse and set "name=value\0 ... \0\0" */
	name = (char *) &header[1];
	end = (char *) header + CONFIG_NVRAM_SIZE - 2;
	end[0] = end[1] = '\0';
	for (; *name; name = value + strlen(value) + 1) {
		if (!(eq = strchr(name, '=')))
			break;
		*eq = '\0';
		value = eq + 1;
		nvram_set(name, value);
		*eq = '=';
	}

	/* Set special SDRAM parameters */
	if (!nvram_get("sdram_init")) {
		sprintf(buf, "0x%04X", (uint16)(header->crc_ver_init >> 16));
		nvram_set("sdram_init", buf);
	}
	if (!nvram_get("sdram_config")) {
		sprintf(buf, "0x%04X", (uint16)(header->config_refresh & 0xffff));
		nvram_set("sdram_config", buf);
	}
	if (!nvram_get("sdram_refresh")) {
		sprintf(buf, "0x%04X", (uint16)((header->config_refresh >> 16) & 0xffff));
		nvram_set("sdram_refresh", buf);
	}
	if (!nvram_get("sdram_ncdl")) {
		sprintf(buf, "0x%08X", header->config_ncdl);
		nvram_set("sdram_ncdl", buf);
	}

	return 0;
}

int _nvram_read(void *buf)
{
	uint32 *src, *dst;
	uint i;

	if (!nvram_header) {
        printf("NVRAM header not found\n");
		return -1;
    }

	src = (uint32 *) nvram_header;
	dst = (uint32 *) buf;

	for (i = 0; i < sizeof(struct nvram_header); i += 4)
		*dst++ = *src++;

	for (; i < nvram_header->len && i < CONFIG_NVRAM_SIZE; i += 4)
		*dst++ = ltoh32(*src++);

	return 0;
}

/* Get the value of an NVRAM variable. Should be locked. */
char *nvram_get(const char *name)
{
	uint i;
	struct nvram_tuple *t;
	char *value;

	if (!name)
		return NULL;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (t = nvram_hash[i]; t && strcmp(t->name, name); t = t->next);

	value = t ? t->value : NULL;

	return value;
}

/* Set the value of an NVRAM variable. Should be locked. */
int nvram_set(const char *name, const char *value)
{
	uint i;
	struct nvram_tuple *t, *u, **prev;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (prev = &nvram_hash[i], t = *prev; t && strcmp(t->name, name);
	     prev = &t->next, t = *prev);

	/* (Re)allocate tuple */
	if (!(u = _nvram_realloc(t, name, value)))
		return -12; /* -ENOMEM */

	/* Value reallocated */
	if (t && t == u)
		return 0;

	/* Move old tuple to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	/* Add new tuple to the hash table */
	u->next = nvram_hash[i];
	nvram_hash[i] = u;

	return 0;
}

/* Unset the value of an NVRAM variable. Should be locked. */
int nvram_unset(const char *name)
{
	uint i;
	struct nvram_tuple *t, **prev;

	if (!name)
		return 0;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (prev = &nvram_hash[i], t = *prev; t && strcmp(t->name, name);
	     prev = &t->next, t = *prev);

	/* Move it to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	return 0;
}

/* Get all NVRAM variables. Should be locked. */
int nvram_getall(char *buf, int count)
{
	uint i;
	struct nvram_tuple *t;
	int len = 0;

	//bzero(buf, count);
        memset(buf,0,count);

	/* Write name=value\0 ... \0\0 */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = t->next) {
			if ((count - len) > (strlen(t->name) + 1 + strlen(t->value) + 1))
				len += sprintf(buf + len, "%s=%s", t->name, t->value) + 1;
			else
				break;
		}
	}

	return 0;
}

/* Regenerate NVRAM. Should be locked. */
int _nvram_commit(struct nvram_header *header)
{
	char *init, *config, *refresh, *ncdl;
	char *ptr, *end;
	int i;
	struct nvram_tuple *t;

	/* Regenerate header */
	header->magic = NVRAM_MAGIC;
	header->crc_ver_init = (NVRAM_VERSION << 8);
	if (!(init = nvram_get("sdram_init")) ||
	    !(config = nvram_get("sdram_config")) ||
	    !(refresh = nvram_get("sdram_refresh")) ||
	    !(ncdl = nvram_get("sdram_ncdl"))) {
		header->crc_ver_init |= SDRAM_INIT << 16;
		header->config_refresh = SDRAM_CONFIG;
		header->config_refresh |= SDRAM_REFRESH << 16;
		header->config_ncdl = 0;
	} else {
		header->crc_ver_init |= (bcm_strtoul(init, NULL, 0) & 0xffff) << 16;
		header->config_refresh = bcm_strtoul(config, NULL, 0) & 0xffff;
		header->config_refresh |= (bcm_strtoul(refresh, NULL, 0) & 0xffff) << 16;
		header->config_ncdl = bcm_strtoul(ncdl, NULL, 0);
	}

	/* Clear data area */
	ptr = (char *) header + sizeof(struct nvram_header);
	//bzero(ptr, nvram_space - sizeof(struct nvram_header));
        memset(ptr, 0,CONFIG_NVRAM_SIZE - sizeof(struct nvram_header));
	/* Leave space for a double NUL at the end */
	end = (char *) header + CONFIG_NVRAM_SIZE - 2;

	/* Write out all tuples */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = t->next) {
			if ((ptr + strlen(t->name) + 1 + strlen(t->value) + 1) > end)
				break;
			ptr += sprintf(ptr, "%s=%s", t->name, t->value) + 1;
		}
	}

	/* End with a double NUL */
	ptr += 2;

	/* Set new length */
	header->len = ROUNDUP(ptr - (char *) header, 4);

	/* Set new CRC8 */
	header->crc_ver_init |= nvram_calc_crc(header);

	/* Reinitialize hash table */
	return nvram_rehash(header);
}

/* Initialize hash table. Should be locked. */
int _nvram_init(void)
{
	struct nvram_header *header;
	int ret;

	if (!(header = (struct nvram_header *) malloc(CONFIG_NVRAM_SIZE))) {
		printf("nvram_init: out of memory\n");
		return -12; /* -ENOMEM */
	}

	if ((ret = _nvram_read(header)) == 0 && header->magic == NVRAM_MAGIC) {
		nvram_rehash(header);
    }

    free(header);
	return ret;
}

/* Free hash table. Should be locked. */
void nvram_exit(void)
{
	nvram_free();
}

/* returns the CRC8 of the nvram */
uint8 nvram_calc_crc(struct nvram_header *nvh)
{
	struct nvram_header tmp;
	uint8 crc;

	/* Little-endian CRC8 over the last 11 bytes of the header */
	tmp.crc_ver_init = htol32((nvh->crc_ver_init & NVRAM_CRC_VER_MASK));
	tmp.config_refresh = htol32(nvh->config_refresh);
	tmp.config_ncdl = htol32(nvh->config_ncdl);

	crc = hndcrc8((uint8 *) &tmp + NVRAM_CRC_START_POSITION,
		sizeof(struct nvram_header) - NVRAM_CRC_START_POSITION,
		CRC8_INIT_VALUE);

	/* Continue CRC8 over data bytes */
	crc = hndcrc8((uint8 *) &nvh[1], nvh->len - sizeof(struct nvram_header), crc);

	return crc;
}

extern unsigned char embedded_nvram[];

static struct nvram_header *find_nvram(uint32 start_addr, bool *isemb)
{
	struct nvram_header *nvh;
	uint32 off; 
	uint8 tmp_crc;
    
    *isemb = FALSE;
    for (off = 0; off < CONFIG_NVRAM_PARTITION_SIZE; off += CONFIG_NVRAM_SIZE) {

        /* Read into the nand_nvram */
        if (readenv(start_addr + off, (u_char *)nflash_nvh)) {
            printf("error reading env\n");
            continue;
        }

	nvh = (struct nvram_header *)nflash_nvh;
	if (nvh->magic != NVRAM_MAGIC)
		continue;
        printf("NVRAM_MAGIC found at offset %x\n", start_addr + off);

	tmp_crc = (uint8) (((struct nvram_header *)nflash_nvh)->crc_ver_init & ~(NVRAM_CRC_VER_MASK));

        printf("nflash_nvh magic: %x\n", ((struct nvram_header *)nflash_nvh)->magic);
        printf("nflash_nvh CRC: %x\n", tmp_crc);
        printf("nflash_nvh CRC calc: %x\n", nvram_calc_crc((struct nvram_header *)nflash_nvh));

        if (nvram_calc_crc((struct nvram_header *)nflash_nvh) == tmp_crc) 
            return (struct nvram_header *)nflash_nvh;
    }

    printf("find_nvram: nvram not found, trying embedded nvram next\n");

    nvh = (struct nvram_header *)embedded_nvram;

    if (nvh->magic != NVRAM_MAGIC) {
	printf("find_nvram: no embedded_nvram\n");
	goto no_found;
    }

    tmp_crc = (uint8) (nvh->crc_ver_init & ~(NVRAM_CRC_VER_MASK));
    printf("embedded nvram magic: %x\n", nvh->magic);
    printf("embedded nvram CRC: %x\n", tmp_crc);
    printf("embedded nvram CRC calc: %x\n", nvram_calc_crc(nvh));

    if (nvram_calc_crc(nvh) == tmp_crc) {
	*isemb = TRUE;
        return (nvh);
    }
 

no_found:
    printf("find_nvram: no nvram found\n");
	return (NULL);
}


int nvram_init(void)
{
	bool isemb;
	int ret;
	static int nvram_status = -1;

	/* Check for previous 'restore defaults' condition */
	if (nvram_status == 1)
		return 1;

	/* Check whether nvram already initilized */
	if (nvram_status == 0)
		return 0;

	/* Check for NVRAM in primary partition */
	nvram_header = find_nvram(CONFIG_NVRAM_OFFSET,&isemb);

    if (nvram_header == NULL) {
        printf("Restoring NVRAM Default Configuration\n");
    }

    ret = _nvram_init();

    if (isemb) nvram_commit();

    printf("_nvram_init: ret %x\n", ret);

	if (ret == 0) {
		/* Restore defaults if embedded NVRAM used */
		if (nvram_header) {
			ret = 1;
		}
	}
    nvram_status = ret;
    printf("nvram_init: ret %x\n", ret);
	return ret;
}

int nvram_find(void)
{
	int ret;
	bool isemb;
	struct nvram_header *nvh;

	/* Check for NVRAM in primary partition */
        nvh = find_nvram(CONFIG_NVRAM_OFFSET,&isemb);

    if (nvh == NULL) {
        printf("Cannot find NVRAM in Default Configuration\n");
	ret = 0;
    } else
	ret =1;

	return ret;
}


int nvram_commit(void)
{
	struct nvram_header *header;
	bool isemb;
    int ret;

    if (!(header = (struct nvram_header *) malloc(CONFIG_NVRAM_SIZE))) {
            printf("nvram_commit: out of memory\n");
            return 1;
    }

    /* Regenerate NVRAM */
    ret = _nvram_commit(header);
    if (ret) {
       printf("nvram commit failed\n");
       return 1;
    }

    printf("nvram_commit: Writing flash partitions\n");

    /* Write to both primary and redundant areas */
    writeenv(CONFIG_NVRAM_OFFSET, (u_char *)(header));

    free(header);

    nvram_header = find_nvram(CONFIG_NVRAM_OFFSET,&isemb);

    return 0;
}
#endif
