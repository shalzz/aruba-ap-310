
/* * Copyright (c) 2012-2013 The Linux Foundation. All rights reserved.* */

/*
 * Adapted from lib/asm-offsets.c
 */

#include <common.h>
#include <linux/kbuild.h>
#include "generated/generic-asm-offsets.h"

//#define GENERATED_GBL_DATA_SIZE (128)

int main(void)
{
#if !defined(DO_DEPS_ONLY) || defined(DO_SOC_DEPS_ONLY)
	DEFINE(GENERATED_IPQ_RESERVE_SIZE, sizeof(ipq_mem_reserve_t));
#endif

	return 0;
}
