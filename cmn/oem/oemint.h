/* vim: set ts=4 sw=4 expandtab: */
#ifndef __oemint_h__
#define __oemint_h__
/*
 * Copyright (C) 2002-2005 by Aruba Wireless Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */

typedef struct oem_t {
    char *oem_name;
    char *oem_shortname;
    char *oem_buildprefix;
    char *oem_legal;
    char *oem_website;
    char *oem_supportsite;
    char *oem_supportemail;
    char *oem_licensesite;
    char *oem_rapconsoledomain;
    char *oem_instantfactoryname;
    char *oem_master_switch;
    char *oem_trademark;
    int  oem_check_oui_on_assoc;
    char *oem_lcd_string;
} oem_t;

typedef struct os_t {
    char *os_name;
    char *os_longname;
} os_t;

typedef struct ap_t {
    int at_num;
    char *at_type;
    int reg_at_index;
} ap_t;

typedef struct buildmodel_t {
    char *bm_aimg;
    char *bm_timg;
    char *bm_cimg;
    char *bm_fimg;
} buildmodel_t;

typedef struct switchmodel_t {
    char *sm_name;
    char *sm_shortmodel;
    char *sm_oid;
    char *sm_internal_type;
    char *sm_build_image;
    char *sm_build_type;
} switchmodel_t;

typedef struct apmodel_t {
    char *am_name;
    char *am_internal_type;
    char *am_shortmodel;
    char *am_usermodel;
    char *am_oid;
} apmodel_t;

typedef struct platform_t {
    char *pl_name;
    char *pl_flashdir;
    char *pl_vardir;
    char *pl_confdir;
} platform_t;

typedef struct image_class_t {
    char     *ic_name;
    unsigned  ic_type;
} image_class_t;
#endif /* __oemint_h__ */
