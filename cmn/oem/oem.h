/* vim: set ts=4 sw=4 expandtab: */
#ifndef __oem_h__
#define __oem_h__
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

extern char *oem_get_name(void);
extern char *oem_get_support_email(void);
extern char *oem_get_master_switch(void);
extern char *oem_get_licensesite(void);
extern char *oem_get_domainname(void);
#ifdef __FAT_AP__
extern char *oem_get_instantfactoryname(void);
#endif
extern char *oem_get_website(void);
extern char *oem_get_legal(void);
extern char *oem_get_osname(void);
extern char *oem_get_oslongname(void);
extern char *oem_get_trademark(void);

extern char *oem_get_shortname(void);

extern char *oem_get_user_visible_switch_model(char *, int);
extern char *oem_get_cdevice_switch_model(char *, int );
extern char *oem_get_internal_model(char *);
extern char *oem_get_model_internal(int);
extern char *oem_ap_user_visible_model(char *);
extern char *oem_get_ap_internal_model(char *);

extern char *oem_get_oid(char *);
extern char *oem_get_user_visible_sc_model(char *);
extern char *oem_get_ap_oid(char *);

extern char *oem_get_build_number(void);
extern char *oem_get_build_info(char *, char *);
extern int oem_has_internal_ap(void);
extern int oem_check_oui_on_assoc(void);

extern int getSCType(void);
extern int getSCTypeOrig(void);

#define AP_UNKNOWN  0

#endif /* __oem_h__ */
