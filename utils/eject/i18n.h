/*
    i18nized by:  KUN-CHUNG, HSIEH <linuxer@coventive.com>
		  Taiwan

    Homepage: http://www.geocities.com/linux4tw/

    程式國際化設計:  謝崑中
*/

#include <features.h>
#ifndef __i18n__
# ifdef USE_NLS
   #define PKG "eject"
   #define __i18n__
   #define LOCALEDIR "/usr/share/locale"
   #include <locale.h>
   #include <libintl.h>
   #define _(str) gettext (str)
   #define N_(str) (str)
   #define I18NCODE setlocale(LC_ALL,""); textdomain(PKG); bindtextdomain(PKG,LOCALEDIR);
# else
   #define _(str) (str)
   #define N_(str) (str)
   #define I18NCODE
# endif
#endif

