/*
 * =====================================================================================
 *
 *       Filename:  convert_cert.c
 *        Version:  1.0
 *        Created:  10/03/2013 10:20:53 PM
 *       Revision:  none
 *
 *        Company:  Aruba Networks
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __FAT_AP__
#include <fcntl.h>
#endif
#include <dirent.h>
#include <cyassl/ctaocrypt/settings.h>
#include <cyassl/ctaocrypt/asn_public.h>
#include <cyassl/ctaocrypt/coding.h>
#include <cyassl/internal.h>
#include <cyassl/ssl.h>
#include <cyassl/openssl/evp.h>
#ifdef __FAT_AP__
#include <ctaocrypt/error-crypt.h>
#endif

#ifndef __FAT_AP__
#include <ctaocrypt/error.h>

#define POLARSSL_MD_C
#define POLARSSL_MD5_C
#define POLARSSL_SHA1_C
#define POLARSSL_SHA2_C
#define POLARSSL_SHA4_C
#include "polarssl/config.h"
#include "polarssl/x509.h"
#include "polarssl/asn1.h"
#include "polarssl/base64.h"
#include "polarssl/pem.h"
#include "polarssl/des.h"
#include "polarssl/aes.h"
#include "polarssl/md5.h"
#include "polarssl/sha1.h"
#include "polarssl/sha2.h"
#include "polarssl/sha4.h"
#include "polarssl/dhm.h"
#include "polarssl/cipher.h"
#endif

#define SERVER_CERT   1
#define CA_CERT       2
#define CPSERVER_CERT   3
#define RADSEC_CLIENT_CERT   4
#define RADSEC_CA_CERT   5
#define AP1X_CLIENT_CERT        6
#define AP1X_CA_CERT        7
#define UISERVER_CERT 8
#define CUSTOM_AWC_CA_CERT 9       // pushed from activate
#define DATATUNNEL_CLIENT_CERT 10
#define DATATUNNEL_CA_CERT   11
#define CUSTOM_AWC_CA_CERT_2 12   //pushed from airwave
#define DEFAULT_CLEARPASS_CA_CERT 13   
#define CLEARPASS_CA_CERT 14

#define CLI_PEM_FORMAT   0
#define CLI_P12_FORMAT   1
#define CLI_DER_FORMAT   2

#define MAX_SERVER_CERT_FORMAT   2
#define MAX_CA_CERT_FORMAT       2
#define RADIUSD_MAX_PSK_LEN    32 
#define PEM_EXTRA    1024 
#ifndef __FAT_AP__
#define TMP_CERT_FILE "/tmp/tmp-cert.pem"
#endif
#ifdef __FAT_AP__
#define RADIUSD_PRV_KEY_FILE       "/tmp/prvkey.txt"
#define RADIUSD_CY_CERT_FILE       "/tmp/cy1xcert.txt"
#define CERTIFICATE_HEADER    "-----BEGIN CERTIFICATE-----"
#define CERTIFICATE_FOOTER    "-----END CERTIFICATE-----"
typedef enum {FALSE, TRUE} boolean;
#endif
char           cert_error[128];
#ifndef __FAT_AP__
int x509_parse_key( rsa_context *rsa, const unsigned char *key, size_t keylen,
                                     const unsigned char *pwd, size_t pwdlen, int copy2file);
int x509_parse_public_key( rsa_context *rsa, const unsigned char *key, 
                                   size_t keylen, int copy2file );
#endif
static int validate_x509_certbuf(const unsigned char *cert_buf, const size_t len, int *ca_cert);

#define DAYSPERNYEAR    365
#define DAYSPERLYEAR    366
#define MONSPERYEAR 12
#define SECSPERDAY  (SECSPERHOUR * HOURSPERDAY)
#define YEAR_BASE   1900
#define EPOCH_YEAR  1970
static const int year_lengths[2] = {
    DAYSPERNYEAR, DAYSPERLYEAR
};

static const int mon_lengths[2][MONSPERYEAR] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

/*
 * isleap()
 *  Check if a year is a leap year.
 *
 * Accurate only for the past couple of centuries; that will probably do.
 */
static int isleap(long y)
{
    /*
     * We can't handle anything before the start of the Gregorian calendar (we can't handle the Julian calendar).
     */
    //DEBUG_ASSERT(y > 1582, "Can't handle year: %u - dates must be in the Gregorian calendar", y)

    /*
     * If this isn't every 4th year it's not a leap year.
     */
    if (y & 3) {
        return 0;
    }

    /*
     * Things get really slow when we do all the modulos up ahead.  So we now have
     * code to check for the most common test years, the 20th and 21st century which
     * are all leap years.
     */
    if (y > 1900 && y < 2100) {
        return 1;
    }

    /*
     * At the end of a century it's normally not a leap year unless the year is divisible
     * by 400 in which case it is (e.g. 1700, 1800, 1900 and 2100 are not leap years but 2000 is).
     */
    if ((y % 100) == 0) {
            if ((y % 400) != 0) {
            return 0;
        }   
    }   
    
    return 1;
}   

/*
 * mktime
 *  Convert a tm structure into time_t, seconds after EPOCH
 */
static long long  lmktime(const struct tm *tmp)
{
   // DEBUG_ASSERT(tmp->tm_year >= (EPOCH_YEAR - YEAR_BASE), "Year (%u) before EPOCH", tmp->tm_year);

    long long yleap;
    long long days = 0;
    long long ybase = tmp->tm_year + YEAR_BASE;
    long long y = EPOCH_YEAR;
    int m = 0;
    for (; y < ybase; y++) {
        yleap = isleap(y);
        days += year_lengths[yleap];
    }

    yleap = isleap(ybase);
    for (; m < tmp->tm_mon; m++) {
        days += mon_lengths[yleap][m];
    }
    days += tmp->tm_mday - 1;

    return (long long)((days * 24 + tmp->tm_hour) * 60 + tmp->tm_min) * 60 + tmp->tm_sec;
}

unsigned int get_upper_approx(unsigned int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

#ifdef __FAT_AP__
/* Function help to split the chinaed PEM format X509 certificate and private key*/
int get_chained_cert(unsigned char *data, int len, unsigned char **begin, size_t *use_len, int ca_flag)
{
    unsigned char *s1 = NULL;
    unsigned char *s2 = NULL;
    unsigned char *start= data;
    int n = 0;
    int is_ca = 0;
    int is_server = 0;
    int ret = 0;

    if( ( *begin = (unsigned char *) malloc( len + 1 ) ) == NULL )
        return( -2 );
    
    memset(*begin, 0, len + 1);

    while (1){
        s1 = (unsigned char *) strstr( start, CERTIFICATE_HEADER );

        if (s1 == NULL){
            if (s2 != NULL)
                break;
            else 
               return -1;
        }

        s2 = (unsigned char *) strstr( s1, CERTIFICATE_FOOTER );
        if( s2 == NULL || s2 <= s1 )
            return -1;

        s2 += strlen( CERTIFICATE_FOOTER );
        if( *s2 == '\r' ){ 
            s2++;
        }
        if( *s2 == '\n' ){
            s2++;
        }

        if((ret = validate_x509_certbuf( s1, s2 - s1, &is_ca)) == -1){
            return -3;
        }
        else if (ret == -2)
            return -5;

        /*if one section doesn't include CA flag, we think it is server sertificate*/
        if (ca_flag){
            if (is_ca != 1)
                return -4;
        }
        else{
            if (is_ca == 0) 
                is_server = 1;
        }

        strlcpy(*begin + n, s1, s2 - s1 + 1);
        n += (s2 - s1);
        start = s2;
    }

    *use_len = n;

    if (!ca_flag && is_server != 1)
        return -4; /*certificate type error*/

    return 0;
}

/* Function help to split the PEM format X509 certificate and private key
 * Returns - 'begin' reference and 'len' of the PEM tagged certificate and private key
 *            w.r.t the provided header and footer */
int get_split_cert( char *header, char *footer, const unsigned char *data, unsigned char **begin, size_t *use_len)
{
    unsigned char *s1, *s2;

    s1 = (unsigned char *) strstr( (const char *) data, header );

    if( s1 == NULL )
        return( -1 );
    *begin = s1;
    s2 = (unsigned char *) strstr( (const char *) data, footer );
    if( s2 == NULL || s2 <= s1 )
        return( -1 );
    s2 += strlen( footer );
    if( *s2 == '\r' ) s2++;
    if( *s2 == '\n' ) s2++;

    *use_len = s2 - s1;
    return 0;
}

/*
 * Load all data from a file into a given buffer.
 */
int load_file( const char *path, unsigned char **buf, size_t *n )
{
    FILE *f;
    struct stat st;

    if (stat(path, &st))
        return(-1);
    *n = st.st_size;

    if( ( *buf = (unsigned char *) malloc( *n + 1 ) ) == NULL )
        return( -1 );

    if( ( f = fopen( path, "rb" ) ) == NULL ) {
        free( *buf );
        return(-1);
    }

    if( fread( *buf, 1, *n, f ) != *n )
    {
        fclose( f );
        free( *buf );
        return( -1 );
    }

    fclose( f );

    (*buf)[*n] = '\0';

    return( 0 );
}

static inline void write_pem_buf(int fd, const unsigned char *buf, const size_t len) {
    write(fd, buf, len);
    if (*(buf + len - 1) != '\n'){
        write(fd, "\n", 1);
    }
}

static int write_pem_to_file(const char *filename, const unsigned char *cert, const size_t len) {
    int fd;

    fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if (fd == -1) {
        snprintf(cert_error, sizeof(cert_error)-1, "open outfile failed ");
        return -1;
    }

    write_pem_buf(fd, cert, len);
    
    close(fd);
    return 0;
}

static int write_key_and_cert_to_file(const char *filename, 
                        const unsigned char *key, const size_t key_len, 
                        const unsigned char *cert, const size_t cert_len) {
    int fd;

    fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if (fd == -1) {
        snprintf(cert_error, sizeof(cert_error)-1, "open outfile failed ");
        return -1;
    }

    write_pem_buf(fd, key, key_len);
    write_pem_buf(fd, cert, cert_len);
    
    close(fd);
    return 0;
}

static inline unsigned int Xis_digit(const unsigned char b) {
    return ((b >= '0') && (b <= '9'));
}

static inline unsigned int btoi(const unsigned char b)
{
    return b - '0';
}

static inline int get_time(const unsigned char *date)
{   
    return btoi(date[0]) * 10 + btoi(date[1]);
}

static inline int asn1_time_sanity(const unsigned char *time, int len, int min_len)
{
    int i;
    if (len < min_len) {
        return -1;
    }

    for (i = 0; i < min_len; i++) {
        if (!Xis_digit(time[i])) {
            return -1;
        }
    }

    return 0;
}

int convert_date(const unsigned char *date_str, struct tm *date)
{
    int           length, min_len;
    unsigned char  format;
    const unsigned char  *p = date_str;

    if (date_str == NULL || date == NULL) {
        return -1;
    }

    format = *p; p++;
    length = *p; p++;

    if (length >= ASN_LONG_LENGTH) {
        return -1;
    }
    if (format == ASN_UTC_TIME) {
        min_len = 10;
        if (asn1_time_sanity(p, length, min_len) < 0) {
            return -1;
        }
        date->tm_year = get_time(p); p += 2;
        if (date->tm_year < 50) {
            date->tm_year += 100;
        }
        /*in struct tm, year is from 1900*/
        //date->tm_year += 1900;
    } else if (format == ASN_GENERALIZED_TIME) {
        min_len = 12;
        if (asn1_time_sanity(p, length, min_len) < 0) {
            return -1;
        }
        date->tm_year  = get_time(p) * 100; p += 2;
        date->tm_year += get_time(p)      ; p += 2;
        /*in struct tm, year is from 1900*/
        date->tm_year -= 1900;
    } else {
        return -1;
    }

    date->tm_mon = get_time(p); p += 2;
    if (date->tm_mon > 12 || date->tm_mon < 1) {
        return -1;
    }
    /*in struct tm, month is from 0*/
    date->tm_mon -= 1; 
    date->tm_mday = get_time(p); p += 2;
    date->tm_hour = get_time(p); p += 2;
    date->tm_min = get_time(p); p += 2;

    if (length >= min_len && Xis_digit(p[0]) && Xis_digit(p[1])) {
        date->tm_sec = get_time(p); p += 2;
    }

    date->tm_isdst = -1;
    return 0;
}

int certificate_date_validate(const unsigned char *date1, const unsigned char *date2)
{
    struct tm time1, time2;
    long long t, t1, t2;
    memset(&time1, 0, sizeof(struct tm));
    memset(&time2, 0, sizeof(struct tm));
    t = (long long)time(NULL);
    convert_date(date1, &time1);
    convert_date(date2, &time2);
    t1 = lmktime(&time1);
    t2 = lmktime(&time2);

    if (((t > t1) && (t > t2)) || ((t < t1) && (t < t2)))
        return 0;
    else
        return 1;
}

/*
 * validate x509 certificate
 */
static int validate_x509_certbuf( const unsigned char *cert_buf, const size_t len, int *ca_cert)
{
    int fd;
    CYASSL_X509 *cya_cert = NULL ;

    /* Write the cert in pem format in a temporary file, to pass to cyassl */
    fd = open(RADIUSD_CY_CERT_FILE, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if (fd == -1) {
        snprintf(cert_error, sizeof(cert_error)-1, "open cert file failed ");
        return -1;
    }
    write(fd, cert_buf, len);
    close(fd);

    /*  a. Validate the cert using CyaSSL calls */
    cya_cert = CyaSSL_X509_load_certificate_file( RADIUSD_CY_CERT_FILE, SSL_FILETYPE_PEM ) ;
    if (!cya_cert) {
        snprintf(cert_error, sizeof(cert_error)-1, "Error in Server Cert PEM validation");
        unlink(RADIUSD_CY_CERT_FILE);
        return -1;
    }

    *ca_cert = CyaSSL_X509_get_isCA(cya_cert);
    if (!certificate_date_validate(CyaSSL_X509_notBefore(cya_cert), CyaSSL_X509_notAfter(cya_cert)))
    {
        snprintf(cert_error, sizeof(cert_error)-1, "Error in certificate date");
        CyaSSL_X509_free(cya_cert);
        unlink(RADIUSD_CY_CERT_FILE);
        return -2;
    }

    CyaSSL_X509_free(cya_cert);
    unlink(RADIUSD_CY_CERT_FILE);

    return 0;
}

int priv_key_password_cb(char *buf, int num, int rwflag, void *userdata)
{
    strcpy(buf, (char *)userdata);
    return(strlen((char *)userdata));
}

static int validate_priv_key_and_psk(unsigned char *pem_buf, size_t len, char *psk)
{
    CYASSL_METHOD *m;
    CYASSL_CTX *ctx;
    int err, ret=0;

    m = CyaSSLv23_server_method();
    if (m == NULL) {
        snprintf(cert_error, sizeof(cert_error)-1, "Error validating private key: Failed to allocate method.");
        return -1;
    }
    ctx = CyaSSL_CTX_new(m);
    if (ctx == NULL) {
        snprintf(cert_error, sizeof(cert_error)-1, "Error validating private key: Failed to allocate CTX.");
        return -1;
    }

    if (psk) {
        CyaSSL_CTX_set_default_passwd_cb_userdata(ctx, psk);
        CyaSSL_CTX_set_default_passwd_cb(ctx, priv_key_password_cb);
    }

    err = write_pem_to_file(RADIUSD_PRV_KEY_FILE, pem_buf, len);
    if (err != 0) {
        return err;
    }

    err = CyaSSL_CTX_use_PrivateKey_file(ctx, RADIUSD_PRV_KEY_FILE, SSL_FILETYPE_PEM);
    if (err != SSL_SUCCESS) {
        snprintf(cert_error, sizeof(cert_error)-1, "Error validating private key: Invalid private key or passphrase.");
        ret = -1;
    }

    CyaSSL_CTX_free(ctx);
    unlink(RADIUSD_PRV_KEY_FILE);

    return ret;
} 

    int
convert_cert(unsigned int cert_type, char *cert, unsigned int format, char *psk, char *outfile)
{
    unsigned char    *pem_buf = NULL;
    unsigned char    *der_buf = NULL;
    unsigned char *begin = NULL;
    unsigned char *buf = NULL;
    FILE    *fp_tmp = NULL ;
    FILE    *fp_cert = NULL ;
    int ret, fd;
    int is_ca = 0;
    struct stat st;
    size_t len, pem_size, der_size, buf_size, use_len = 0;
    CYASSL_X509 *cya_cert = NULL ;

    if (format == CLI_P12_FORMAT) {
        snprintf(cert_error, sizeof(cert_error)-1, "PKCS#12 is not supported on IAP") ;
        return -1;
    }

    if (cert_type == CA_CERT || cert_type == RADSEC_CA_CERT || cert_type == AP1X_CA_CERT || cert_type == DATATUNNEL_CA_CERT
            || cert_type == CUSTOM_AWC_CA_CERT || cert_type == CUSTOM_AWC_CA_CERT_2 || 
            cert_type == DEFAULT_CLEARPASS_CA_CERT || cert_type == CLEARPASS_CA_CERT) {
        if (format == CLI_DER_FORMAT) {
            if ( (ret = load_file( cert, &der_buf, &der_size ) ) != 0 ) {
                snprintf(cert_error, sizeof(cert_error)-1, "Load private key file failed - %d", ret);
                return( ret );
            }
            buf_size = get_upper_approx(der_size);
            pem_buf = (unsigned char*)malloc(buf_size + PEM_EXTRA);
            if (pem_buf == NULL)
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to allocate PEM buf");
                free(der_buf);
                return -1;
            }

            /* Convert to PEM format */
            memset( pem_buf, 0, buf_size + PEM_EXTRA ) ;
            pem_size = DerToPem( der_buf, der_size, pem_buf, buf_size + PEM_EXTRA, CERT_TYPE ) ;

            if( pem_size < 0 ) 
            {
                char errstr[1024] ;
                CTaoCryptErrorString( pem_size, errstr ) ;
                snprintf(cert_error, sizeof(cert_error)-1, "Failed converting CA_Cert to PEM: %s", errstr) ;
                free(der_buf);
                free(pem_buf);
                return -1;
            }
            free(der_buf);

            /* Write the PEM cert to outfile */
            ret = write_pem_to_file(outfile, pem_buf, pem_size);
            if (ret != 0) {
                snprintf(cert_error, sizeof(cert_error)-1, "Error writing CA DER cert to outfile");
                free(pem_buf);
                return -1;
            }
            free(pem_buf);
        }

        /* Validate the PEM CA cert */
        if (format == CLI_PEM_FORMAT) {
            if ( (ret = load_file( cert, &buf, &len ) ) != 0 ) {
                snprintf(cert_error, sizeof(cert_error)-1, "Load private key file failed - %d", ret);
                return( ret );
            }

            ret = get_chained_cert(buf, len, &begin, &use_len, 1);
            if (ret == -1) {
                snprintf(cert_error, sizeof(cert_error)-1, "split certificate file failed ");
                free(buf);
                free(begin);
                return -1;
            }
            else if(ret == -2){
                snprintf(cert_error, sizeof(cert_error)-1, "split certificate file failed when malloc");
                free(buf);
                return -1;
            }
            else if(ret == -3){
                snprintf(cert_error, sizeof(cert_error)-1, "Validate certificate file failed");
                free(buf);
                free(begin);
                return -1;
            }
            else if(ret == -4){
                snprintf(cert_error, sizeof(cert_error)-1, "Validate certificate type failed");
                free(buf);
                free(begin);
                return -1;
            }
            else if(ret == -5){
                snprintf(cert_error, sizeof(cert_error)-1, "Validate certificate date failed");
                free(buf);
                free(begin);
                return -1;
            }
            ret = write_pem_to_file(outfile, begin, use_len);
            if (ret != 0) {
                snprintf(cert_error, sizeof(cert_error)-1, "Error writing CA PEM cert to outfile");
                free(buf);
                free(begin);
                return -1;
            }
            free(buf);
            free(begin);
        }

        return 0;
    }

    if  (((cert_type == SERVER_CERT) || (cert_type == CPSERVER_CERT) || (cert_type == UISERVER_CERT) ||
          (cert_type == RADSEC_CLIENT_CERT) || (cert_type == AP1X_CLIENT_CERT) || (cert_type == DATATUNNEL_CLIENT_CERT) 
          || (cert_type == CUSTOM_AWC_CA_CERT_2)) && (format == CLI_PEM_FORMAT)) {

        /*  
         *  Step for server certs
         *  1. Get the cert and private key separately
         *  2. Validate the cert using CyaSSL calls
         *  2a. If the cert is validated then copy to outfile
         *  3. Validate the private key using CyaSSL calls
         *  3a. if the private is validated then copy to outfile
         **/
        if ( (ret = load_file( cert, &buf, &len ) ) != 0 ) {
            snprintf(cert_error, sizeof(cert_error)-1, "Load pem cert file failed - %d", ret);
            return( ret );
        }

        ret = get_chained_cert(buf, len, &begin, &use_len, 0);
        if (ret == -1) {
            snprintf(cert_error, sizeof(cert_error)-1, "split certificate file failed ");
            free(buf);
            free(begin);
            return -1;
        }
        else if(ret == -2){
            snprintf(cert_error, sizeof(cert_error)-1, "split certificate file failed when malloc");
            free(buf);
            return -1;
        }
        else if(ret == -3){
            snprintf(cert_error, sizeof(cert_error)-1, "Validate certificate file failed");
            free(buf);
            free(begin);
            return -1;
        }
        else if(ret == -4 && (cert_type != CPSERVER_CERT || cert_type != UISERVER_CERT)){ /*Ignore checking cert type for CP cert*/
            snprintf(cert_error, sizeof(cert_error)-1, "Validate certificate type failed");
            free(buf);
            free(begin);
            return -1;
        }
        else if(ret == -5){
            snprintf(cert_error, sizeof(cert_error)-1, "Validate certificate date failed");
            free(buf);
            free(begin);
            return -1;
        }

        int pkcs8 = 0;
        /*  3a. Validate & Decrypt (only if encrypted) the private 
         *  key using CyaSSL calls */
        if (strstr(buf, "-----BEGIN RSA PRIVATE KEY-----") != NULL){
            if (get_split_cert("-----BEGIN RSA PRIVATE KEY-----",
                    "-----END RSA PRIVATE KEY-----",
                    buf, &pem_buf, &pem_size) == -1) {
                snprintf(cert_error, sizeof(cert_error)-1, "split private key file failed ");
                free(buf);
                free(begin);
                return -1;
            }
        }
        else if (strstr(buf, "-----BEGIN ENCRYPTED PRIVATE KEY-----") != NULL){
            if (get_split_cert("-----BEGIN ENCRYPTED PRIVATE KEY-----",
                    "-----END ENCRYPTED PRIVATE KEY-----",
                    buf, &pem_buf, &pem_size) == -1) {
                snprintf(cert_error, sizeof(cert_error)-1, "split private key file failed ");
                free(buf);
                free(begin);
                return -1;
            }
            /* If pem certificate delimiters have ENCRYPTED in them then it is pkcs8enc*/
            pkcs8 = 1;
        }
        else if (strstr(buf, "-----BEGIN PRIVATE KEY-----") != NULL){
            if (get_split_cert("-----BEGIN PRIVATE KEY-----",
                    "-----END PRIVATE KEY-----",
                    buf, &pem_buf, &pem_size) == -1) {
                snprintf(cert_error, sizeof(cert_error)-1, "split private key file failed ");
                free(buf);
                free(begin);
                return -1;
            }
            pkcs8 = 1;
        }
        else{
            snprintf(cert_error, sizeof(cert_error)-1, "Cannot find private key");
            free(buf);
            free(begin);
            return -1;
        }

        der_size = get_upper_approx(pem_size);
        der_buf = (unsigned char*)malloc(der_size);
        if (der_buf == NULL)
        {
            snprintf(cert_error, sizeof(cert_error)-1, "Unable to allocate DER buf");
            free(buf);
            free(begin);
            return -1;
        }

        // Do a pem to der conversion on the key.
        ret = CyaSSL_KeyPemToDer(pem_buf, pem_size, der_buf, der_size, psk);
        if (ret < 0) {
            snprintf(cert_error, sizeof(cert_error)-1, "Error in RSA key validation - %d", ret);
            free(buf);
            free(der_buf);
            free(begin);
            return -1;
        }

        /* 3b. If encrypted convert DER buffer to PEM
         * do the conversion only if pkcs8. Otherwise write the original private key in pem format as it is. */
        if (pkcs8) {
            /* pem_buf points to an offset in buf. buf is not used after this point and the conversion 
             * can not exceed pem_size so it is ok to do the conversion into buf.*/
            len = DerToPem(der_buf, ret, pem_buf, pem_size, PRIVATEKEY_TYPE);
            if (len < 0) {
                snprintf(cert_error, sizeof(cert_error)-1, "Error during private key DerToPem - %zu", len);
                free(buf);
                free(der_buf);
                free(begin);
                return -1;
            }
        } else {
            len = pem_size;
        }

        ret = validate_priv_key_and_psk(pem_buf, len, psk);
        if (ret != 0) {
            //TODO: rework to avoid duplication of cleanup.
            snprintf(cert_error, sizeof(cert_error)-1, "Error during validate priv key and psk - %zu", len);
            free(buf);
            free(begin);
            free(der_buf);
            return -1;
        }

        ret = write_key_and_cert_to_file(outfile, pem_buf, len, begin, use_len);
        if (ret != 0) {
            snprintf(cert_error, sizeof(cert_error)-1, "Error writing RSA key and cert to outfile - %d", ret);
            free(buf);
            free(begin);
            free(der_buf);
            return -1;
        }
        free(buf);
        free(begin);
        free(der_buf);
    }
    return 0;

}
/* Bug182947 shell command can be injected via psk, so encoded psk in cli,
   decoded it here.*/
static boolean
is_encoded(char *str)
{
    boolean encoded = TRUE;
    int i;
    for(i = 0; str[i] != '\0'; i++) {
        if (!isxdigit(str[i])) {
            encoded = FALSE;
            break;
        }
    }
    if (i % 2 != 0)
        encoded = FALSE;
    return encoded;
}
static int
hex_to_int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}
static int
hex_to_ascii(char first, char second)
{
    int high = hex_to_int(first) * 16;
    int low = hex_to_int(second);
    return high + low;
}
static void
decode_hex_to_str(char *dest, char *estr)
{
    int i, tmp;
    char buf = 0;
    char *p = dest;
    for(i = 0; estr[i] != '\0'; i++) {
        if (i%2 != 0) {
            tmp = hex_to_ascii(buf, estr[i]);
            sprintf(p++, "%c", tmp);
        } else {
            buf = estr[i];
        }
    }
    *p = 0;
}




int main(int argc, char **argv)
{
    FILE *fp_err = NULL;
    char *pass = NULL;
    char *certfile = NULL;
    char *outfile = NULL;
    char psk[512] = {0};
    unsigned int cert_type = 0, cert_format = 0;

    if (argc == 6) {
        pass = argv[5];
        if (is_encoded(pass)) {
            decode_hex_to_str(psk, pass);
            pass = psk;
        }
    } else if (argc != 5) {
        fprintf(stderr, "Usage: convert_cert certfile certtype certformat outfile password\n");
        fprintf(stderr, "certformat can be 0/1/2 i.e. CLI_PEM_FORMAT/CLI_P12_FORMAT/CLI_DER_FORMAT\n"); 
        fprintf(stderr, "certtype can be 1/2/3/4/5/6/7/8/9/12/13/14 i.e.  SERVER_CERT/CA_CERT/ \
                CPSERVER_CERT/RADSEC_CLIENT_CERT/RADSEC_CA_CERT/AP1X_CERT/AP1X_CA/ \
                DATATUNNEL_CLIENT_CERT/DATATUNNEL_CA_CERT/CUSTOM_AWC_CA_CERT_2/DEFAULT_CLEARPASS_CA_CERT/CLEARPASS_CA_CERT\n"); 
        exit (1);
    }
    unlink ("/tmp/convert.err");
    certfile = argv[1];
    cert_type = atoi(argv[2]);
    cert_format = atoi(argv[3]);
    outfile = argv[4];
    //certtype, cert_file, certformat, password, outfile
    if (convert_cert(cert_type, certfile, cert_format, pass, outfile)) {
        if((fp_err = fopen("/tmp/convert.err", "a")) != NULL) {
             fwrite(cert_error, sizeof(cert_error)-1, 1, fp_err);
             fclose (fp_err);
        }
        return 1;
    }
    unlink (RADIUSD_CY_CERT_FILE);
    unlink (RADIUSD_PRV_KEY_FILE);
    return 0;
}
#endif

#ifndef __FAT_AP__
/* Compare a given OID string with an OID x509_buf * */
#define OID_CMP(oid_str, oid_buf) \
        ( ( OID_SIZE(oid_str) == (oid_buf)->len ) && \
                memcmp( (oid_str), (oid_buf)->p, (oid_buf)->len) == 0)

/*
 * ASN.1 DER decoding routines
 */
int asn1_get_len( unsigned char **p,
                  const unsigned char *end,
                  size_t *len )
{
    if( ( end - *p ) < 1 )
        return( POLARSSL_ERR_ASN1_OUT_OF_DATA );

    if( ( **p & 0x80 ) == 0 )
        *len = *(*p)++;
    else
    {
        switch( **p & 0x7F )
        {
        case 1:
            if( ( end - *p ) < 2 )
                return( POLARSSL_ERR_ASN1_OUT_OF_DATA );

            *len = (*p)[1];
            (*p) += 2;
            break;

        case 2:
            if( ( end - *p ) < 3 )
                return( POLARSSL_ERR_ASN1_OUT_OF_DATA );

            *len = ( (*p)[1] << 8 ) | (*p)[2];
            (*p) += 3;
            break;

        case 3:
            if( ( end - *p ) < 4 )
                return( POLARSSL_ERR_ASN1_OUT_OF_DATA );

            *len = ( (*p)[1] << 16 ) | ( (*p)[2] << 8 ) | (*p)[3];
            (*p) += 4;
            break;

        case 4:
            if( ( end - *p ) < 5 )
                return( POLARSSL_ERR_ASN1_OUT_OF_DATA );

            *len = ( (*p)[1] << 24 ) | ( (*p)[2] << 16 ) | ( (*p)[3] << 8 ) | (*p)[4];
            (*p) += 5;
            break;

        default:
            return( POLARSSL_ERR_ASN1_INVALID_LENGTH );
        }
    }

    if( *len > (size_t) ( end - *p ) )
        return( POLARSSL_ERR_ASN1_OUT_OF_DATA );

    return( 0 );
}

int asn1_get_tag( unsigned char **p,
                  const unsigned char *end,
                  size_t *len, int tag )
{
    if( ( end - *p ) < 1 )
        return( POLARSSL_ERR_ASN1_OUT_OF_DATA );

    if( **p != tag )
        return( POLARSSL_ERR_ASN1_UNEXPECTED_TAG );

    (*p)++;

    return( asn1_get_len( p, end, len ) );
}

int asn1_get_int( unsigned char **p,
                  const unsigned char *end,
                  int *val )
{
    int ret;
    size_t len;

    if( ( ret = asn1_get_tag( p, end, &len, ASN1_INTEGER ) ) != 0 )
        return( ret );

    if( len > sizeof( int ) || ( **p & 0x80 ) != 0 )
        return( POLARSSL_ERR_ASN1_INVALID_LENGTH );

    *val = 0;

    while( len-- > 0 )
    {
        *val = ( *val << 8 ) | **p;
        (*p)++;
    }

    return( 0 );
}

int asn1_get_mpi( unsigned char **p,
                  const unsigned char *end,
                  mpi *X )
{
    int ret;
    size_t len;

    if( ( ret = asn1_get_tag( p, end, &len, ASN1_INTEGER ) ) != 0 )
        return( ret );

    ret = mpi_read_binary( X, *p, len );

    *p += len;

    return( ret );
}


void pem_init( pem_context *ctx )
{
    memset( ctx, 0, sizeof( pem_context ) );
}

/*
 * Read a 16-byte hex string and convert it to binary
 */
static int pem_get_iv( const unsigned char *s, unsigned char *iv, size_t iv_len )
{
    size_t i, j, k;

    memset( iv, 0, iv_len );

    for( i = 0; i < iv_len * 2; i++, s++ )
    {
        if( *s >= '0' && *s <= '9' ) j = *s - '0'; else
        if( *s >= 'A' && *s <= 'F' ) j = *s - '7'; else
        if( *s >= 'a' && *s <= 'f' ) j = *s - 'W'; else
            return( POLARSSL_ERR_PEM_INVALID_ENC_IV );

        k = ( ( i & 1 ) != 0 ) ? j : j << 4;

        iv[i >> 1] = (unsigned char)( iv[i >> 1] | k );
    }

    return( 0 );
}

static void pem_pbkdf1( unsigned char *key, size_t keylen,
                        unsigned char *iv,
                        const unsigned char *pwd, size_t pwdlen )
{
	md5_context md5_ctx;
    unsigned char md5sum[16];
    size_t use_len;

    /*
     * key[ 0..15] = MD5(pwd || IV)
     */
    md5_starts( &md5_ctx );
    md5_update( &md5_ctx, pwd, pwdlen );
    md5_update( &md5_ctx, iv,  8 );
    md5_finish( &md5_ctx , md5sum);

    if( keylen <= 16 )
    {
        memcpy( key, md5sum, keylen );

        memset( &md5_ctx, 0, sizeof(  md5_ctx ) );
        memset( md5sum, 0, 16 );
        return;
    }

    memcpy( key, md5sum, 16 );

    /*
     * key[16..23] = MD5(key[ 0..15] || pwd || IV])
     */
    md5_starts( &md5_ctx );
    md5_update( &md5_ctx, md5sum,  16 );
    md5_update( &md5_ctx, pwd, pwdlen );
    md5_update( &md5_ctx, iv,  8 );
    md5_finish( &md5_ctx, md5sum );

    use_len = 16;
    if( keylen < 32 )
        use_len = keylen - 16;

    memcpy( key + 16, md5sum, use_len );

    memset( &md5_ctx, 0, sizeof(  md5_ctx ) );
    memset( md5sum, 0, 16 );
}

/*
 * Decrypt with DES-CBC, using PBKDF1 for key derivation
 */
static void pem_des_decrypt( unsigned char des_iv[8],
                               unsigned char *buf, size_t buflen,
                               const unsigned char *pwd, size_t pwdlen )
{
    des_context des_ctx;
    unsigned char des_key[8];

    pem_pbkdf1( des_key, 8, des_iv, pwd, pwdlen );

    des_setkey_dec( &des_ctx, des_key );
    des_crypt_cbc( &des_ctx, DES_DECRYPT, buflen,
                     des_iv, buf, buf );

    memset( &des_ctx, 0, sizeof( des_ctx ) );
    memset( des_key, 0, 8 );
}

/*
 * Decrypt with 3DES-CBC, using PBKDF1 for key derivation
 */
static void pem_des3_decrypt( unsigned char des3_iv[8],
                               unsigned char *buf, size_t buflen,
                               const unsigned char *pwd, size_t pwdlen )
{
    des3_context des3_ctx;
    unsigned char des3_key[24];

    pem_pbkdf1( des3_key, 24, des3_iv, pwd, pwdlen );

    des3_set3key_dec( &des3_ctx, des3_key );
    des3_crypt_cbc( &des3_ctx, DES_DECRYPT, buflen,
                     des3_iv, buf, buf );

    memset( &des3_ctx, 0, sizeof( des3_ctx ) );
    memset( des3_key, 0, 24 );
}

/*
 * Decrypt with AES-XXX-CBC, using PBKDF1 for key derivation
 */
static void pem_aes_decrypt( unsigned char aes_iv[16], unsigned int keylen,
                               unsigned char *buf, size_t buflen,
                               const unsigned char *pwd, size_t pwdlen )
{
    aes_context aes_ctx;
    unsigned char aes_key[32];

    pem_pbkdf1( aes_key, keylen, aes_iv, pwd, pwdlen );

    aes_setkey_dec( &aes_ctx, aes_key, keylen * 8 );
    aes_crypt_cbc( &aes_ctx, AES_DECRYPT, buflen,
                     aes_iv, buf, buf );

    memset( &aes_ctx, 0, sizeof( aes_ctx ) );
    memset( aes_key, 0, keylen );
}


int pem_read_buffer( pem_context *ctx, char *header, char *footer, const unsigned char *data, const unsigned char *pwd, size_t pwdlen, size_t *use_len, int copy2file)
{
    int ret, enc;
    size_t len, slen;
    unsigned char *buf, *tmp_buf;
    const unsigned char *s1, *s2, *end, *pem_start, *pem_end;
    FILE *tmp_cert = NULL;
    unsigned char pem_iv[16];
    cipher_type_t enc_alg = POLARSSL_CIPHER_NONE;

    if( ctx == NULL )
        return( POLARSSL_ERR_PEM_BAD_INPUT_DATA );

    s1 = (unsigned char *) strstr( (const char *) data, header );

    if( s1 == NULL )
        return( POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT );

    s2 = (unsigned char *) strstr( (const char *) data, footer );

    if( s2 == NULL || s2 <= s1 )
        return( POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT );

  
    pem_start = s1;
    s1 += strlen( header );
    if( *s1 == '\r' ) s1++;
    if( *s1 == '\n' ) s1++;
    else return( POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT );

    end = s2;
    end += strlen( footer );
    if( *end == '\r' ) end++;
    if( *end == '\n' ) end++;
    pem_end = end;

    *use_len = end - data;

    enc = 0;

    if( memcmp( s1, "Proc-Type: 4,ENCRYPTED", 22 ) == 0 )
    {
        enc++;

        s1 += 22;
        if( *s1 == '\r' ) s1++;
        if( *s1 == '\n' ) s1++;
        else return( POLARSSL_ERR_PEM_INVALID_DATA );


        if( memcmp( s1, "DEK-Info: DES-EDE3-CBC,", 23 ) == 0 )
        {
            enc_alg = POLARSSL_CIPHER_DES_EDE3_CBC;

            s1 += 23;
            if( pem_get_iv( s1, pem_iv, 8 ) != 0 )
                return( POLARSSL_ERR_PEM_INVALID_ENC_IV );

            s1 += 16;
        }
        else if( memcmp( s1, "DEK-Info: DES-CBC,", 18 ) == 0 )
        {
            enc_alg = POLARSSL_CIPHER_DES_CBC;

            s1 += 18;
            if( pem_get_iv( s1, pem_iv, 8) != 0 )
                return( POLARSSL_ERR_PEM_INVALID_ENC_IV );

            s1 += 16;
        }

        if( memcmp( s1, "DEK-Info: AES-", 14 ) == 0 )
        {
            if( memcmp( s1, "DEK-Info: AES-128-CBC,", 22 ) == 0 )
                enc_alg = POLARSSL_CIPHER_AES_128_CBC;
            else if( memcmp( s1, "DEK-Info: AES-192-CBC,", 22 ) == 0 )
                enc_alg = POLARSSL_CIPHER_AES_192_CBC;
            else if( memcmp( s1, "DEK-Info: AES-256-CBC,", 22 ) == 0 )
                enc_alg = POLARSSL_CIPHER_AES_256_CBC;
            else
                return( POLARSSL_ERR_PEM_UNKNOWN_ENC_ALG );

            s1 += 22;
            if( pem_get_iv( s1, pem_iv, 16 ) != 0 )
                return( POLARSSL_ERR_PEM_INVALID_ENC_IV );

            s1 += 32;
        }
        
        if( enc_alg == POLARSSL_CIPHER_NONE )
            return( POLARSSL_ERR_PEM_UNKNOWN_ENC_ALG );

        if( *s1 == '\r' ) s1++;
        if( *s1 == '\n' ) s1++;
        else return( POLARSSL_ERR_PEM_INVALID_DATA );
    }

    if (!enc && copy2file) {
        /* Write the PEM cert/key back to file */
        tmp_cert = fopen( TMP_CERT_FILE, "a");
        if( !tmp_cert )
        {
            printf("Unable to open certificate file: %s",
                    TMP_CERT_FILE) ;
            return 0;
        }
        fwrite(pem_start, pem_end - pem_start, 1, tmp_cert);
        fclose(tmp_cert);
    }

    len = 0;
    ret = base64_decode( NULL, &len, s1, s2 - s1 );

    if( ret == POLARSSL_ERR_BASE64_INVALID_CHARACTER )
        return( POLARSSL_ERR_PEM_INVALID_DATA + ret );

    if( ( buf = (unsigned char *) malloc( len ) ) == NULL )
        return( POLARSSL_ERR_PEM_MALLOC_FAILED );

    if( ( ret = base64_decode( buf, &len, s1, s2 - s1 ) ) != 0 )
    {
        free( buf );
        return( POLARSSL_ERR_PEM_INVALID_DATA + ret );
    }
    
    if( enc != 0 )
    {
        if( pwd == NULL )
        {
            free( buf );
            return( POLARSSL_ERR_PEM_PASSWORD_REQUIRED );
        }

        if( enc_alg == POLARSSL_CIPHER_DES_EDE3_CBC )
            pem_des3_decrypt( pem_iv, buf, len, pwd, pwdlen );
        else if( enc_alg == POLARSSL_CIPHER_DES_CBC )
            pem_des_decrypt( pem_iv, buf, len, pwd, pwdlen );

        if( enc_alg == POLARSSL_CIPHER_AES_128_CBC )
            pem_aes_decrypt( pem_iv, 16, buf, len, pwd, pwdlen );
        else if( enc_alg == POLARSSL_CIPHER_AES_192_CBC )
            pem_aes_decrypt( pem_iv, 24, buf, len, pwd, pwdlen );
        else if( enc_alg == POLARSSL_CIPHER_AES_256_CBC )
            pem_aes_decrypt( pem_iv, 32, buf, len, pwd, pwdlen );

        if( buf[0] != 0x30 || buf[1] != 0x82 ||
            buf[4] != 0x02 || buf[5] != 0x01 )
        {
            free( buf );
            return( POLARSSL_ERR_PEM_PASSWORD_MISMATCH );
        }

        if (copy2file) {
            slen = len + PEM_EXTRA;
            if( ( tmp_buf = (unsigned char *) malloc( slen) ) == NULL )
            return( POLARSSL_ERR_PEM_MALLOC_FAILED );

            ret = Base64_Encode( buf, len, tmp_buf, &slen );
            if (!ret) {
                tmp_cert = fopen( TMP_CERT_FILE, "a");
                if( !tmp_cert )
                {
                    printf("Unable to open certificate file: %s",
                            TMP_CERT_FILE) ;
                    return 0;
                }
                fwrite(pem_start, strlen(header) + 1, 1, tmp_cert);
                fwrite(tmp_buf, slen, 1, tmp_cert);
                fwrite(s2, strlen(footer) + 1, 1, tmp_cert);
                fclose(tmp_cert);
            }
            free(tmp_buf);
        }
    }
    ctx->buf = buf;
    ctx->buflen = len;

    return( 0 );
}

void pem_free( pem_context *ctx )
{
    if( ctx->buf )
        free( ctx->buf );

    if( ctx->info )
        free( ctx->info );

    memset( ctx, 0, sizeof( pem_context ) );
}
/*
 *  AlgorithmIdentifier  ::=  SEQUENCE  {
 *       algorithm               OBJECT IDENTIFIER,
 *       parameters              ANY DEFINED BY algorithm OPTIONAL  }
 */
static int x509_get_alg( unsigned char **p,
                         const unsigned char *end,
                         x509_buf *alg )
{
    int ret;
    size_t len;

    if( ( ret = asn1_get_tag( p, end, &len,
            ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 )
        return( POLARSSL_ERR_X509_CERT_INVALID_ALG + ret );

    end = *p + len;
    alg->tag = **p;

    if( ( ret = asn1_get_tag( p, end, &alg->len, ASN1_OID ) ) != 0 )
        return( POLARSSL_ERR_X509_CERT_INVALID_ALG + ret );

    alg->p = *p;
    *p += alg->len;

    if( *p == end )
        return( 0 );

    /*
     * assume the algorithm parameters must be NULL
     */
    if( ( ret = asn1_get_tag( p, end, &len, ASN1_NULL ) ) != 0 )
        return( POLARSSL_ERR_X509_CERT_INVALID_ALG + ret );

    if( *p != end )
        return( POLARSSL_ERR_X509_CERT_INVALID_ALG +
                POLARSSL_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}


/*
 *  SubjectPublicKeyInfo  ::=  SEQUENCE  {
 *       algorithm            AlgorithmIdentifier,
 *       subjectPublicKey     BIT STRING }
 */
static int x509_get_pubkey( unsigned char **p,
                            const unsigned char *end,
                            x509_buf *pk_alg_oid,
                            mpi *N, mpi *E )
{
    int ret;
    size_t len;
    unsigned char *end2;

    if( ( ret = x509_get_alg( p, end, pk_alg_oid ) ) != 0 )
        return( ret );

    /*
     * only RSA public keys handled at this time
     */
    if( pk_alg_oid->len != 9 ||
        memcmp( pk_alg_oid->p, OID_PKCS1_RSA, 9 ) != 0 )
    {
        return( POLARSSL_ERR_X509_UNKNOWN_PK_ALG );
    }

    if( ( ret = asn1_get_tag( p, end, &len, ASN1_BIT_STRING ) ) != 0 )
        return( POLARSSL_ERR_X509_CERT_INVALID_PUBKEY + ret );

    if( ( end - *p ) < 1 )
        return( POLARSSL_ERR_X509_CERT_INVALID_PUBKEY +
                POLARSSL_ERR_ASN1_OUT_OF_DATA );

    end2 = *p + len;

    if( *(*p)++ != 0 )
        return( POLARSSL_ERR_X509_CERT_INVALID_PUBKEY );

    /*
     *  RSAPublicKey ::= SEQUENCE {
     *      modulus           INTEGER,  -- n
     *      publicExponent    INTEGER   -- e
     *  }
     */
    if( ( ret = asn1_get_tag( p, end2, &len,
            ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 )
        return( POLARSSL_ERR_X509_CERT_INVALID_PUBKEY + ret );

    if( *p + len != end2 )
        return( POLARSSL_ERR_X509_CERT_INVALID_PUBKEY +
                POLARSSL_ERR_ASN1_LENGTH_MISMATCH );

    if( ( ret = asn1_get_mpi( p, end2, N ) ) != 0 ||
        ( ret = asn1_get_mpi( p, end2, E ) ) != 0 )
        return( POLARSSL_ERR_X509_CERT_INVALID_PUBKEY + ret );

    if( *p != end )
        return( POLARSSL_ERR_X509_CERT_INVALID_PUBKEY +
                POLARSSL_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}

/*
 * Unallocate all certificate data
 */
void x509_free( x509_cert *crt )
{
    x509_cert *cert_cur = crt;
    x509_cert *cert_prv;
    x509_name *name_cur;
    x509_name *name_prv;
    x509_sequence *seq_cur;
    x509_sequence *seq_prv;

    if( crt == NULL )
        return;

    do
    {
        rsa_free( &cert_cur->rsa );

        name_cur = cert_cur->issuer.next;
        while( name_cur != NULL )
        {
            name_prv = name_cur;
            name_cur = name_cur->next;
            memset( name_prv, 0, sizeof( x509_name ) );
            free( name_prv );
        }

        name_cur = cert_cur->subject.next;
        while( name_cur != NULL )
        {
            name_prv = name_cur;
            name_cur = name_cur->next;
            memset( name_prv, 0, sizeof( x509_name ) );
            free( name_prv );
        }

        seq_cur = cert_cur->ext_key_usage.next;
        while( seq_cur != NULL )
        {
            seq_prv = seq_cur;
            seq_cur = seq_cur->next;
            memset( seq_prv, 0, sizeof( x509_sequence ) );
            free( seq_prv );
        }

        seq_cur = cert_cur->subject_alt_names.next;
        while( seq_cur != NULL )
        {
            seq_prv = seq_cur;
            seq_cur = seq_cur->next;
            memset( seq_prv, 0, sizeof( x509_sequence ) );
            free( seq_prv );
        }

        if( cert_cur->raw.p != NULL )
        {
            memset( cert_cur->raw.p, 0, cert_cur->raw.len );
            free( cert_cur->raw.p );
        }

        cert_cur = cert_cur->next;
    }
    while( cert_cur != NULL );

    cert_cur = crt;
    do
    {
        cert_prv = cert_cur;
        cert_cur = cert_cur->next;

        memset( cert_prv, 0, sizeof( x509_cert ) );
        if( cert_prv != crt )
            free( cert_prv );
    }
    while( cert_cur != NULL );
}

/*
 * Parse one or more PEM certificates from a buffer and add them to the chained list
 */
int x509_parse_pem_crt( x509_cert *chain, const unsigned char *buf, size_t buflen, int copy2file )
{
    int ret, success = 0, first_error = 0, total_failed = 0;
    int buf_format = X509_FORMAT_DER;

    /*
     * Check for valid input
     */
    if( chain == NULL || buf == NULL )
        return( POLARSSL_ERR_X509_INVALID_INPUT );

    /*
     * Determine buffer content. Buffer contains either one DER certificate or
     * one or more PEM certificates.
     */
    if( strstr( (const char *) buf, "-----BEGIN CERTIFICATE-----" ) != NULL )
        buf_format = X509_FORMAT_PEM;

    if( buf_format == X509_FORMAT_PEM )
    {
        pem_context pem;

        while( buflen > 0 )
        {
            size_t use_len;
            pem_init( &pem );

            ret = pem_read_buffer( &pem,
                           "-----BEGIN CERTIFICATE-----",
                           "-----END CERTIFICATE-----",
                           buf, NULL, 0, &use_len, copy2file );

            if( ret == 0 )
            {
                /*
                 * Was PEM encoded
                 */
                buflen -= use_len;
                buf += use_len;

            }
            else if( ret == POLARSSL_ERR_PEM_BAD_INPUT_DATA )
            {
                return( ret );
            }
            else if( ret == POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
            {
                pem_free( &pem );
                return( 0 );
            }
            else if( ret != POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
            {
                pem_free( &pem );

                /*
                 * PEM header and footer were found
                 */
                buflen -= use_len;
                buf += use_len;

                if( first_error == 0 )
                    first_error = ret;

                continue;
            }
            else
                break;

            pem_free( &pem );

            if( ret != 0 )
            {
                /*
                 * Quit parsing on a memory error
                 */
                if( ret == POLARSSL_ERR_X509_MALLOC_FAILED )
                    return( ret );

                if( first_error == 0 )
                    first_error = ret;

                total_failed++;
                continue;
            }

            success = 1;
        }
    }

    if( success )
        return( total_failed );
    else if( first_error )
        return( first_error );
    else
        return( POLARSSL_ERR_X509_CERT_UNKNOWN_FORMAT );
}

/*
 * Load all data from a file into a given buffer.
 */
int load_file( const char *path, unsigned char **buf, size_t *n )
{
    FILE *f;

    if( ( f = fopen( path, "rb" ) ) == NULL )
        return( POLARSSL_ERR_X509_FILE_IO_ERROR );

    fseek( f, 0, SEEK_END );
    *n = (size_t) ftell( f );
    fseek( f, 0, SEEK_SET );

    if( ( *buf = (unsigned char *) malloc( *n + 1 ) ) == NULL )
        return( POLARSSL_ERR_X509_MALLOC_FAILED );

    if( fread( *buf, 1, *n, f ) != *n )
    {
        fclose( f );
        free( *buf );
        return( POLARSSL_ERR_X509_FILE_IO_ERROR );
    }

    fclose( f );

    (*buf)[*n] = '\0';

    return( 0 );
}

/*
 * Load one or more certificates and add them to the chained list
 */
int validate_x509_pem_crtfile( x509_cert *chain, const char *path, int copy2file)
{
    int ret;
    size_t n;
    unsigned char *buf;

    if ( (ret = load_file( path, &buf, &n ) ) != 0 )
        return( ret );

    ret = x509_parse_pem_crt( chain, buf, n, copy2file);

    memset( buf, 0, n + 1 );
    free( buf );
    
    if ( ret == POLARSSL_ERR_X509_CERT_UNKNOWN_FORMAT )
        return( 0 );
    else 
        return( ret );
}

/*
 * Load and parse a private RSA key
 */
int validate_x509_keyfile( rsa_context *rsa, const char *path, const char *pwd, int copy2file )
{
    int ret;
    size_t n;
    unsigned char *buf;

    if ( (ret = load_file( path, &buf, &n ) ) != 0 )
        return( ret );

    if( pwd == NULL )
        ret = x509_parse_key( rsa, buf, n, NULL, 0, copy2file);
    else
        ret = x509_parse_key( rsa, buf, n,
                (unsigned char *) pwd, strlen( pwd ), copy2file);

    memset( buf, 0, n + 1 );
    free( buf );

    return( ret );
}

/*
 * Load and parse a public RSA key
 */
int validate_x509_public_keyfile( rsa_context *rsa, const char *path, int copy2file )
{
    int ret;
    size_t n;
    unsigned char *buf;

    if ( (ret = load_file( path, &buf, &n ) ) != 0 )
        return( ret );

    ret = x509_parse_public_key( rsa, buf, n, copy2file );

    memset( buf, 0, n + 1 );
    free( buf );

    return( ret );
}

/*
 * Parse a PKCS#1 encoded private RSA key
 */
static int x509parse_key_pkcs1_der( rsa_context *rsa,
                                    const unsigned char *key,
                                    size_t keylen )
{
    int ret;
    size_t len;
    unsigned char *p, *end;

    p = (unsigned char *) key;
    end = p + keylen;

    /*
     * This function parses the RSAPrivateKey (PKCS#1)
     *
     *  RSAPrivateKey ::= SEQUENCE {
     *      version           Version,
     *      modulus           INTEGER,  -- n
     *      publicExponent    INTEGER,  -- e
     *      privateExponent   INTEGER,  -- d
     *      prime1            INTEGER,  -- p
     *      prime2            INTEGER,  -- q
     *      exponent1         INTEGER,  -- d mod (p-1)
     *      exponent2         INTEGER,  -- d mod (q-1)
     *      coefficient       INTEGER,  -- (inverse of q) mod p
     *      otherPrimeInfos   OtherPrimeInfos OPTIONAL
     *  }
     */
    if( ( ret = asn1_get_tag( &p, end, &len,
            ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    end = p + len;

    if( ( ret = asn1_get_int( &p, end, &rsa->ver ) ) != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    if( rsa->ver != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_VERSION + ret );
    }

    if( ( ret = asn1_get_mpi( &p, end, &rsa->N  ) ) != 0 ||
        ( ret = asn1_get_mpi( &p, end, &rsa->E  ) ) != 0 ||
        ( ret = asn1_get_mpi( &p, end, &rsa->D  ) ) != 0 ||
        ( ret = asn1_get_mpi( &p, end, &rsa->P  ) ) != 0 ||
        ( ret = asn1_get_mpi( &p, end, &rsa->Q  ) ) != 0 ||
        ( ret = asn1_get_mpi( &p, end, &rsa->DP ) ) != 0 ||
        ( ret = asn1_get_mpi( &p, end, &rsa->DQ ) ) != 0 ||
        ( ret = asn1_get_mpi( &p, end, &rsa->QP ) ) != 0 )
    {
        rsa_free( rsa );
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    rsa->len = mpi_size( &rsa->N );

    if( p != end )
    {
        rsa_free( rsa );
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT +
                POLARSSL_ERR_ASN1_LENGTH_MISMATCH );
    }

    if( ( ret = rsa_check_privkey( rsa ) ) != 0 )
    {
        rsa_free( rsa );
        return( ret );
    }

    return( 0 );
}

/*
 * Parse an unencrypted PKCS#8 encoded private RSA key
 */
static int x509parse_key_pkcs8_unencrypted_der(
                                    rsa_context *rsa,
                                    const unsigned char *key,
                                    size_t keylen )
{
    int ret;
    size_t len;
    unsigned char *p, *end;
    x509_buf pk_alg_oid;

    p = (unsigned char *) key;
    end = p + keylen;

    /*
     * This function parses the PrivatKeyInfo object (PKCS#8)
     *
     *  PrivateKeyInfo ::= SEQUENCE {
     *    version           Version,
     *    algorithm       AlgorithmIdentifier,
     *    PrivateKey      BIT STRING
     *  }
     *
     *  AlgorithmIdentifier ::= SEQUENCE {
     *    algorithm       OBJECT IDENTIFIER,
     *    parameters      ANY DEFINED BY algorithm OPTIONAL
     *  }
     *
     *  The PrivateKey BIT STRING is a PKCS#1 RSAPrivateKey
     */
    if( ( ret = asn1_get_tag( &p, end, &len,
            ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    end = p + len;

    if( ( ret = asn1_get_int( &p, end, &rsa->ver ) ) != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    if( rsa->ver != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_VERSION + ret );
    }

    if( ( ret = x509_get_alg( &p, end, &pk_alg_oid ) ) != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    /*
     * only RSA keys handled at this time
     */
    if( pk_alg_oid.len != 9 ||
        memcmp( pk_alg_oid.p, OID_PKCS1_RSA, 9 ) != 0 )
    {
        return( POLARSSL_ERR_X509_UNKNOWN_PK_ALG );
    }

    /*
     * Get the OCTET STRING and parse the PKCS#1 format inside
     */
    if( ( ret = asn1_get_tag( &p, end, &len, ASN1_OCTET_STRING ) ) != 0 )
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );

    if( ( end - p ) < 1 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT +
                POLARSSL_ERR_ASN1_OUT_OF_DATA );
    }

    end = p + len;

    if( ( ret = x509parse_key_pkcs1_der( rsa, p, end - p ) ) != 0 )
        return( ret );

    return( 0 );
}

/*
 * Parse an encrypted PKCS#8 encoded private RSA key
 */
static int x509parse_key_pkcs8_encrypted_der(
                                    rsa_context *rsa,
                                    const unsigned char *key,
                                    size_t keylen,
                                    const unsigned char *pwd,
                                    size_t pwdlen )
{
    int ret;
    size_t len;
    unsigned char *p, *end, *end2;
    x509_buf pbe_alg_oid, pbe_params;
    unsigned char buf[2048];

    memset(buf, 0, 2048);

    p = (unsigned char *) key;
    end = p + keylen;

    if( pwdlen == 0 )
        return( POLARSSL_ERR_X509_PASSWORD_REQUIRED );

    /*
     * This function parses the EncryptedPrivatKeyInfo object (PKCS#8)
     *
     *  EncryptedPrivateKeyInfo ::= SEQUENCE {
     *    encryptionAlgorithm  EncryptionAlgorithmIdentifier,
     *    encryptedData        EncryptedData
     *  }
     *
     *  EncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
     *
     *  EncryptedData ::= OCTET STRING
     *
     *  The EncryptedData OCTET STRING is a PKCS#8 PrivateKeyInfo
     */
    if( ( ret = asn1_get_tag( &p, end, &len,
            ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    end = p + len;

    if( ( ret = asn1_get_tag( &p, end, &len,
            ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 )
    {
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    end2 = p + len;

    if( ( ret = asn1_get_tag( &p, end, &pbe_alg_oid.len, ASN1_OID ) ) != 0 )
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );

    pbe_alg_oid.p = p;
    p += pbe_alg_oid.len;

    /*
     * Store the algorithm parameters
     */
    pbe_params.p = p;
    pbe_params.len = end2 - p;
    p += pbe_params.len;

    if( ( ret = asn1_get_tag( &p, end, &len, ASN1_OCTET_STRING ) ) != 0 )
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );

    // buf has been sized to 2048 bytes
    if( len > 2048 )
        return( POLARSSL_ERR_X509_INVALID_INPUT );
#if 0 /* May be need in future */
    /*
     * Decrypt EncryptedData with appropriate PDE
     */
    if( OID_CMP( OID_PKCS12_PBE_SHA1_DES3_EDE_CBC, &pbe_alg_oid ) )
    {
        if( ( ret = pkcs12_pbe( &pbe_params, PKCS12_PBE_DECRYPT,
                                POLARSSL_CIPHER_DES_EDE3_CBC, POLARSSL_MD_SHA1,
                                pwd, pwdlen, p, len, buf ) ) != 0 )
        {
            if( ret == POLARSSL_ERR_PKCS12_PASSWORD_MISMATCH )
                return( POLARSSL_ERR_X509_PASSWORD_MISMATCH );

            return( ret );
        }
    }
    else if( OID_CMP( OID_PKCS12_PBE_SHA1_DES2_EDE_CBC, &pbe_alg_oid ) )
    {
        if( ( ret = pkcs12_pbe( &pbe_params, PKCS12_PBE_DECRYPT,
                                POLARSSL_CIPHER_DES_EDE_CBC, POLARSSL_MD_SHA1,
                                pwd, pwdlen, p, len, buf ) ) != 0 )
        {
            if( ret == POLARSSL_ERR_PKCS12_PASSWORD_MISMATCH )
                return( POLARSSL_ERR_X509_PASSWORD_MISMATCH );

            return( ret );
        }
    }
    else if( OID_CMP( OID_PKCS12_PBE_SHA1_RC4_128, &pbe_alg_oid ) )
    {
        if( ( ret = pkcs12_pbe_sha1_rc4_128( &pbe_params,
                                             PKCS12_PBE_DECRYPT,
                                             pwd, pwdlen,
                                             p, len, buf ) ) != 0 )
        {
            return( ret );
        }

        // Best guess for password mismatch when using RC4. If first tag is
        // not ASN1_CONSTRUCTED | ASN1_SEQUENCE
        //
        if( *buf != ( ASN1_CONSTRUCTED | ASN1_SEQUENCE ) )
            return( POLARSSL_ERR_X509_PASSWORD_MISMATCH );
    }
#endif
    else
        return( POLARSSL_ERR_X509_FEATURE_UNAVAILABLE );

    return x509parse_key_pkcs8_unencrypted_der( rsa, buf, len );
}

/*
 * Parse a private RSA key
 */
int x509_parse_key( rsa_context *rsa, const unsigned char *key, size_t keylen,
                                     const unsigned char *pwd, size_t pwdlen, int copy2file)
{
    int ret;

    size_t len;
    pem_context pem;

    pem_init( &pem );
    ret = pem_read_buffer( &pem,
                           "-----BEGIN RSA PRIVATE KEY-----",
                           "-----END RSA PRIVATE KEY-----",
                           key, pwd, pwdlen, &len, copy2file );
    if( ret == 0 )
    {
        if( ( ret = x509parse_key_pkcs1_der( rsa, pem.buf, pem.buflen ) ) != 0 )
        {
            rsa_free( rsa );
        }

        pem_free( &pem );
        return( ret );
    }
    else if( ret == POLARSSL_ERR_PEM_PASSWORD_MISMATCH )
        return( POLARSSL_ERR_X509_PASSWORD_MISMATCH );
    else if( ret == POLARSSL_ERR_PEM_PASSWORD_REQUIRED )
        return( POLARSSL_ERR_X509_PASSWORD_REQUIRED );
    else if( ret == POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
    {
        pem_free( &pem );
        return( 0 );
    }
    else if( ret != POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
        return( ret );

    ret = pem_read_buffer( &pem,
                           "-----BEGIN PRIVATE KEY-----",
                           "-----END PRIVATE KEY-----",
                           key, NULL, 0, &len, copy2file );
    if( ret == 0 )
    {
        if( ( ret = x509parse_key_pkcs8_unencrypted_der( rsa,
                                                pem.buf, pem.buflen ) ) != 0 )
        {
            rsa_free( rsa );
        }

        pem_free( &pem );
        return( ret );
    }
    else if( ret != POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
        return( ret );

    ret = pem_read_buffer( &pem,
                           "-----BEGIN ENCRYPTED PRIVATE KEY-----",
                           "-----END ENCRYPTED PRIVATE KEY-----",
                           key, NULL, 0, &len, copy2file );
    if( ret == 0 )
    {
        if( ( ret = x509parse_key_pkcs8_encrypted_der( rsa,
                                                pem.buf, pem.buflen,
                                                pwd, pwdlen ) ) != 0 )
        {
            rsa_free( rsa );
        }

        pem_free( &pem );
        return( ret );
    }
    else if( ret != POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
        return( ret );

    // At this point we only know it's not a PEM formatted key. Could be any
    // of the known DER encoded private key formats
    //
    // We try the different DER format parsers to see if one passes without
    // error
    //
    if( ( ret = x509parse_key_pkcs8_encrypted_der( rsa, key, keylen,
                                                   pwd, pwdlen ) ) == 0 )
    {
        return( 0 );
    }

    rsa_free( rsa );

    if( ret == POLARSSL_ERR_X509_PASSWORD_MISMATCH )
    {
        return( ret );
    }

    if( ( ret = x509parse_key_pkcs8_unencrypted_der( rsa, key, keylen ) ) == 0 )
        return( 0 );

    rsa_free( rsa );

    if( ( ret = x509parse_key_pkcs1_der( rsa, key, keylen ) ) == 0 )
        return( 0 );

    rsa_free( rsa );

    return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT );
}

/*
 * Parse a public RSA key
 */
int x509_parse_public_key( rsa_context *rsa, const unsigned char *key, size_t keylen, int copy2file )
{
    int ret;
    size_t len;
    unsigned char *p, *end;
    x509_buf alg_oid;
    pem_context pem;

    pem_init( &pem );
    ret = pem_read_buffer( &pem,
            "-----BEGIN PUBLIC KEY-----",
            "-----END PUBLIC KEY-----",
            key, NULL, 0, &len, copy2file );

    if( ret == 0 )
    {
        /*
         * Was PEM encoded
         */
        keylen = pem.buflen;
    }
    else if( ret == POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
    {
        pem_free( &pem );
        return( 0 );
    }
    else 
    {
        pem_free( &pem );
        return( ret );
    }

    p = ( ret == 0 ) ? pem.buf : (unsigned char *) key;
    end = p + keylen;

    /*
     *  PublicKeyInfo ::= SEQUENCE {
     *    algorithm       AlgorithmIdentifier,
     *    PublicKey       BIT STRING
     *  }
     *
     *  AlgorithmIdentifier ::= SEQUENCE {
     *    algorithm       OBJECT IDENTIFIER,
     *    parameters      ANY DEFINED BY algorithm OPTIONAL
     *  }
     *
     *  RSAPublicKey ::= SEQUENCE {
     *      modulus           INTEGER,  -- n
     *      publicExponent    INTEGER   -- e
     *  }
     */

    if( ( ret = asn1_get_tag( &p, end, &len,
                    ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 )
    {
        pem_free( &pem );
        rsa_free( rsa );
        return( POLARSSL_ERR_X509_CERT_INVALID_FORMAT + ret );
    }

    if( ( ret = x509_get_pubkey( &p, end, &alg_oid, &rsa->N, &rsa->E ) ) != 0 )
    {
        pem_free( &pem );
        rsa_free( rsa );
        return( POLARSSL_ERR_X509_KEY_INVALID_FORMAT + ret );
    }

    if( ( ret = rsa_check_pubkey( rsa ) ) != 0 )
    {
        pem_free( &pem );
        rsa_free( rsa );
        return( ret );
    }

    rsa->len = mpi_size( &rsa->N );

    pem_free( &pem );

    return( 0 );
}

int
convert_cert(unsigned int cert_type, char *cert, unsigned int format, char *psk, char *outfile)
{
    char    *pem_buf = NULL;
    char    *der_buf = NULL;
    FILE    *fp_tmp = NULL ;
    FILE    *fp_cert = NULL ;
    unsigned char *buf;
    x509_cert   c;
    rsa_context k;
    int pem_size = 0, der_size = 0, ret;
    unsigned int buf_size = 0;
    struct stat st;
    size_t len;
    pem_context pem;

    if (format == CLI_P12_FORMAT) {
        snprintf(cert_error, sizeof(cert_error)-1, "PKCS#12 is not supported on IAP") ;
        return -1;
    }

    if (cert_type == CA_CERT) {
        if (format == CLI_DER_FORMAT) {

            fp_cert = fopen( cert, "rb");
            if( !fp_cert )
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to open certificate file: %s",
                        cert) ;
                return -1;
            }

            stat(cert, &st);
            buf_size = st.st_size;
            buf_size = get_upper_approx(buf_size);
            der_buf = (char*)malloc(buf_size);
            if (der_buf == NULL)
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to allocate DER buf");
                return -1;
            }
            pem_buf = (char*)malloc(buf_size + PEM_EXTRA);
            if (pem_buf == NULL)
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to allocate PEM buf");
                free(der_buf);
                return -1;
            }

            der_size = fread(der_buf, 1, buf_size, fp_cert);
            fclose(fp_cert);

            /* Convert to PEM format */
            memset( pem_buf, 0, buf_size + PEM_EXTRA ) ;
            pem_size = DerToPem( der_buf, der_size, pem_buf, buf_size + PEM_EXTRA, CERT_TYPE ) ;

            if( pem_size < 0 ) 
            {
                char errstr[1024] ;
                CTaoCryptErrorString( pem_size, errstr ) ;
                snprintf(cert_error, sizeof(cert_error)-1, "Failed converting CA_Cert to PEM: %s", errstr) ;
                free(der_buf);
                free(pem_buf);
                return -1;
            }
            free(der_buf);

            /* Write the PEM cert to outfile */
            fp_cert = fopen( outfile, "w");
            if( !fp_cert )
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to open certificate file: %s", outfile) ;
                free(pem_buf);
                return -1;
            }
            ret = (int)fwrite(pem_buf, pem_size, 1, fp_cert);
            free(pem_buf);
            fclose(fp_cert);
        }

        /* Validate the PEM cert */
        if (format == CLI_PEM_FORMAT) {
            memset (&c, 0, sizeof (c));
            ret = validate_x509_pem_crtfile( &c, cert, 0);
            if (ret)
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Error in CA PEM cert validation - %d", ret);
                x509_free(&c);
                return -1;
            }
            x509_free(&c);
        }

        return 0;
    }

    if  (((cert_type == SERVER_CERT) || (cert_type == CPSERVER_CERT)) 
            && (format == CLI_PEM_FORMAT)) {

        /* Validate the PEM cert */
        memset (&c, 0, sizeof (c));
        ret = validate_x509_pem_crtfile( &c, cert, 0);
        if (ret)
        {
            snprintf(cert_error, sizeof(cert_error)-1, "Error in PEM validation - %d", ret);
            x509_free(&c);
            return -1;
        }
        x509_free(&c);

        /* Validate the RSA key so that we will not do it later */
        memset (&k, 0, sizeof (k));
        ret = validate_x509_keyfile (&k, cert, (strlen(psk) ? psk : NULL), 0);
        if (ret) {
            snprintf(cert_error, sizeof(cert_error)-1, "Error in RSA key validation - %d", ret);
            rsa_free(&k);
            return -1;
        }
        rsa_free(&k);

        /* Check for RSA private key tag */
        if ( (ret = load_file( cert, &buf, &len ) ) != 0 ) {
            snprintf(cert_error, sizeof(cert_error)-1, "Load private key file failed - %d", ret);
            return( ret );
        }

        len  = 0;
        pem_init( &pem );
        ret = pem_read_buffer( &pem,
                "-----BEGIN RSA PRIVATE KEY-----",
                "-----END RSA PRIVATE KEY-----",
                buf, psk, strlen(psk), &len, 0 );
        if( ret == 0 )
        {
            snprintf(cert_error, sizeof(cert_error)-1, "PEM read buffer failed - %d", ret);
            pem_free( &pem );
            return( ret );
        }
        else if( ret == POLARSSL_ERR_PEM_NO_HEADER_FOOTER_PRESENT )
        {
            /* If the tag - "-----BEGIN RSA PRIVATE KEY-----"
             * is missing then decode the cert as in convert_cert.sh */
            pem_free( &pem );
            /* Validate & copy the PEM cert */
            memset (&c, 0, sizeof (c));
            if (validate_x509_pem_crtfile( &c, cert, 1))
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Error in PEM validation & copy - %d", ret);
                x509_free(&c);
                return -1;
            }
            x509_free(&c);
            memset (&k, 0, sizeof (k));
            ret = validate_x509_public_keyfile( &k, cert, 1 );
            if (ret)
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Error in public validation & copy - %d", ret);
                rsa_free(&k);
                return -1;
            }

            /* Validate & copy the PEM private key */
            /* psk uplimit is 32 */
            if ((psk) && (strlen(psk) > RADIUSD_MAX_PSK_LEN)) {
                snprintf(cert_error, sizeof(cert_error)-1, "RSA Key Too long");
                return -1;
            }

            memset (&k, 0, sizeof (k));
            ret = validate_x509_keyfile (&k, cert, (strlen(psk) ? psk : NULL), 1);
            if (ret) {
                snprintf(cert_error, sizeof(cert_error)-1, "Error in RSA key validation & copy %d", ret);
                rsa_free(&k);
                return -1;
            }
            rsa_free(&k);

            stat(TMP_CERT_FILE, &st);
            pem_size = st.st_size;
            buf_size = (pem_size > buf_size) ? get_upper_approx(pem_size) : get_upper_approx(buf_size);
            pem_buf = (char*)malloc(buf_size);
            if (pem_buf == NULL)
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to allocate PEM buf");
                return -1;
            }
            /* Write the cert & unencrypted key to outfile */
            fp_tmp = fopen( TMP_CERT_FILE, "r");
            if( !fp_tmp )
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to open certificate file: %s", TMP_CERT_FILE) ;
                free(pem_buf);
                return -1;
            }
            memset( pem_buf, 0, buf_size ) ;
            pem_size = fread(pem_buf, 1, buf_size, fp_tmp);
            fclose(fp_tmp);
            fp_cert = fopen( outfile, "w");
            if( !fp_cert )
            {
                snprintf(cert_error, sizeof(cert_error)-1, "Unable to open certificate file: %s", outfile) ;
                free(pem_buf);
                return -1;
            }
            fwrite(pem_buf, pem_size, 1, fp_cert);
            free(pem_buf);
            fclose(fp_cert);
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *fp_err = NULL;
    unsigned int cert_type = 0, cert_format = 0;

	if (argc != 6) {
		fprintf(stderr, "Usage: convert_cert certfile certtype certformat password outfile\n");
		fprintf(stderr, "certformat can be 0/1/2 i.e. CLI_PEM_FORMAT/CLI_P12_FORMAT/CLI_DER_FORMAT\n"); 
		fprintf(stderr, "certtype can be 1/2/3 i.e. SERVER_CERT/CA_CERT/CPSERVER_CERT\n"); 
		exit (1);
	}
    unlink ("/tmp/convert.err");
    cert_type = atoi(argv[2]);
    cert_format = atoi(argv[3]);
    //certtype, cert_file, certformat, password, outfile
    if (convert_cert(cert_type, argv[1], cert_format, argv[4], argv[5])) {
        if((fp_err = fopen("/tmp/convert.err", "a")) != NULL) {
             fwrite(cert_error, sizeof(cert_error)-1, 1, fp_err);
             fclose (fp_err);
        }
        return 1;
    }
    unlink (TMP_CERT_FILE);
    return 0;
}


#endif
