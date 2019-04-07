#include <ctaocrypt/asn.h>
#include <ctaocrypt/rsa.h>
#include <ctaocrypt/error-crypt.h>
#include <ctaocrypt/logging.h>
#include <openssl/ssl.h>
#include "cyassl_wrap/x509_wrap.h"

byte *wolfssl_read_file(const char *fname, size_t *len);
static const char *mon[12]=
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const char *cyassl_X509_get_signature_type_str(CYASSL_X509 *x509)
{
    const char *sig_type;

    switch (CyaSSL_X509_get_signature_type(x509))
    {
        case CTC_MD5wRSA:
            sig_type = "MD5-RSA";
            break;
        case CTC_SHAwRSA:
            sig_type = "SHA1-RSA";
            break;
        case CTC_SHAwECDSA:
            sig_type = "SHA1-ECDSA";
            break;
        case CTC_SHA256wRSA:
            sig_type = "SHA256-RSA";
            break;
        case CTC_SHA256wECDSA:
            sig_type = "SHA256-ECDSA";
            break;
        case CTC_SHA384wRSA:
            sig_type = "SHA384-RSA";
            break;
        case CTC_SHA384wECDSA:
            sig_type = "SHA384-ECDSA";
            break;
        case CTC_SHA512wRSA:
            sig_type = "SHA512-RSA";
            break;
        case CTC_SHA512wECDSA:
            sig_type = "SHA512-ECDSA";
            break;
        default:
            sig_type = "unknown";
            break;
    }

    return sig_type;
}

int cyassl_X509_get_serial_number_str(CYASSL_X509 *x509, char *str, int len)
{
    byte            serial[32];
    char            *ptr;
    int             sz, i, n, status;

    if (str == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = sizeof(serial);
    status = CyaSSL_X509_get_serial_number(x509, serial, &sz);
    if (status == SSL_SUCCESS) {
        if (len < (sz * 3)) {
            return BUFFER_E;
        }
        ptr = str; n = 0;
        for (i = 0; i < sz; i++) {
            n = sprintf(ptr, "%02X%s", serial[i], (i == (sz - 1)) ? "" : ":" );
            if (n < 2) {
                status = BUFFER_E;
                break;
            }
            ptr += n; len -= n;
        }
    }
    return status;
}

int cyassl_X509_get_pubkey_size(CYASSL_X509 *x509)
{
    CYASSL_EVP_PKEY *pkey;
    uint32_t        idx = 0;
    RsaKey          key;
    int32_t         ret;

    pkey = CyaSSL_X509_get_pubkey(x509);
    if (pkey) {
        switch (pkey->type) 
        {
            case RSAk:
                {
                    ret = InitRsaKey(&key, NULL);
                    if (ret != 0) {
                        CyaSSL_EVP_PKEY_free(pkey);
                        return -1;
                    }
                    if ((ret = RsaPublicKeyDecode(pkey->pkey.ptr, &idx, &key, pkey->pkey_sz)) >= 0) { 
                        ret = key.n.used  * DIGIT_BIT;
                    }
                    FreeRsaKey(&key);
                }
                break;
            default:
                ret = -1;
                break;
        }

        CyaSSL_EVP_PKEY_free(pkey);
        return ret;
    }
    return -1;
}


static inline uint32_t Xis_digit(const uint8_t b) {
    return ((b >= '0') && (b <= '9'));
}

static inline uint32_t btoi(const uint8_t b) 
{
    return b - '0';
}

static inline int get_time(const uint8_t *date)
{
    return btoi(date[0]) * 10 + btoi(date[1]); 
}

static inline int asn1_time_sanity(const uint8_t *time, int len, int min_len)
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

static int cyassl_X509_date_to_str(const uint8_t *date, char *str, uint32_t len) 
{
    int           length, min_len;
    uint8_t       format;
    const uint8_t *p = date;
    const char    *tz;
    int           y, M, d, h, m, s = 0;

    if (date == NULL || str == NULL) {
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
        y = get_time(p); p += 2;
        if (y < 50) {
            y += 100;
        }
        y += 1900;
    } else if (format == ASN_GENERALIZED_TIME) {
        min_len = 12;
        if (asn1_time_sanity(p, length, min_len) < 0) {
            return -1;
        }
        y  = get_time(p) * 100; p += 2;
        y += get_time(p)      ; p += 2;
    } else {
        return -1;
    }

    M = get_time(p); p += 2;
    if (M > 12 || M < 1) {
        return -1;
    }
    d = get_time(p); p += 2;
    h = get_time(p); p += 2;
    m = get_time(p); p += 2;

    if (length >= min_len && Xis_digit(p[0]) && Xis_digit(p[1])) {
        s = get_time(p); p += 2;
    }

    tz = (date[2 + length - 1] == 'Z') ? "GMT" : "";

    snprintf(str, len, "%s %2d %02d:%02d:%02d %d %s", mon[M - 1], d, h, m, s, y, tz);
    return 0;
}

int cyassl_X509_get_notbefore_str(CYASSL_X509 *x509, char *str, uint32_t len)
{
    const uint8_t *date;

    if (x509 == NULL) {
        return BAD_FUNC_ARG;
    }
    date = CyaSSL_X509_notBefore(x509);
    return cyassl_X509_date_to_str(date, str, len);
}

int cyassl_X509_get_notafter_str(CYASSL_X509 *x509, char *str, uint32_t len)
{
    const uint8_t *date;

    if (x509 == NULL) {
        return BAD_FUNC_ARG;
    }
    date = CyaSSL_X509_notAfter(x509);
    return cyassl_X509_date_to_str(date, str, len);
}

static
int wolfssl_X509_get_rsa_pubkey(WOLFSSL_X509 *x509, RsaKey *key, uint32_t *key_len)
{
    WOLFSSL_EVP_PKEY     *pkey;
    uint32_t             idx = 0;
    int32_t              ret;

    pkey = wolfSSL_X509_get_pubkey(x509);
    if (pkey) {
        switch (pkey->type) 
        {
            case RSAk:
                {
                    ret = wc_InitRsaKey(key, NULL);
                    if (ret != 0) {
                        wolfSSL_EVP_PKEY_free(pkey);
                        return -1;
                    }
                    ret = wc_RsaPublicKeyDecode(pkey->pkey.ptr, &idx, key, pkey->pkey_sz);
                    if (ret >= 0) {
                        *key_len = (key->n.used * DIGIT_BIT) / 8;
                    }
                }
                break;
            default:
                ret = -1;
                break;
        }

        wolfSSL_EVP_PKEY_free(pkey);
        return ret;
    }
    return -1;
}

int wolfssl_get_rsa_pubkey(const char *name, int format, RsaKey *key, uint32_t *key_len)
{
    WOLFSSL_X509     *x509;
    int32_t          ret;

    if (name == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    x509 = wolfSSL_X509_load_certificate_file(name, format);
    if (x509 == NULL) {
        return -1;
    }

    ret = wolfssl_X509_get_rsa_pubkey(x509, key, key_len);
    wolfSSL_X509_free(x509);

    return ret;
}

/* Generates an rsa key of the given key size in bits. e=0x10001 */
int wolfssl_gen_rsa_key(RsaKey *rsa_key, int size)
{
    WC_RNG              rng;
    int                 ret;

    ret = wc_InitRsaKey(rsa_key, 0);
    if (ret != 0) {
        return ret;
    }
    ret = wc_InitRng(&rng);
    if (ret != 0) {
        return ret;
    }
    ret = wc_MakeRsaKey(rsa_key, size, 0x10001, &rng);
    wc_FreeRng(&rng);

    return ret;
}

int wolfssl_free_rsa_key(RsaKey *rsa_key)
{
    return wc_FreeRsaKey(rsa_key);
}

int wolfssl_cm_der_verify_load_ca(WOLFSSL_CERT_MANAGER *cm, const char *ca_name)
{
    int                 ret;
    size_t              len;
    uint8_t             *der;
    SSL_CTX             *ctx;
    SSL_METHOD          *method;

    if (cm == NULL || ca_name == NULL) {
        return BAD_FUNC_ARG;
    }
    method = TLSv1_2_client_method();
    ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        return MEMORY_E;
    }
    der = wolfssl_read_file(ca_name, &len);
    if (der == NULL) {
        SSL_CTX_free(ctx);
        return SSL_BAD_FILE;
    }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    ret = wolfSSL_CTX_load_verify_buffer(ctx, der, len, SSL_FILETYPE_ASN1);
    SSL_CTX_free(ctx);
    if (ret != SSL_SUCCESS) {
        free(der);
        return ret;
    }

    ret = wolfSSL_CertManagerLoadCABuffer(cm, der, len, SSL_FILETYPE_ASN1);

    free(der);
    return ret;
}

int wolfssl_init_cert(DecodedCert *cert, byte *buf, word32 buf_len)
{
    int ret;

    if (buf == NULL || buf_len == 0 || cert == NULL) {
        return BAD_FUNC_ARG;
    }

    InitDecodedCert(cert, buf, buf_len, 0);

    ret = ParseCert(cert, CERT_TYPE, NO_VERIFY, 0);
    if (ret != 0) {
        return SSL_FATAL_ERROR;
    }

    return SSL_SUCCESS;
}

void wolfssl_free_cert(DecodedCert *cert)
{
    if (cert) {
        FreeDecodedCert(cert);
    }
}

int wolfssl_cert_policy_ext_set(DecodedCert *cert)
{
    if (cert) {
        return cert->extCertPoliciesNb;
    }
    return 0;
}

int wolfssl_cert_eku_ext_set(DecodedCert *cert)
{
    if (cert) {
        return cert->extExtKeyUsageSet;
    }
    return 0;
}

int wolfssl_cert_policy_oid_set(DecodedCert *cert, char *oid)
{
    int i;
    if (!cert) {
        return 0;
    }
    for (i = 0; i < cert->extCertPoliciesNb; i++) {
        if (strcmp(oid, cert->extCertPolicies[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

#if 0
/* Checks if low assurance oid is set in the EKU extension of cert 
 * 0 - not set
 * 1 - set 
 * < 0 - error */
int wolfssl_cert_la_oid_set(byte *buf, word32 buf_len) 
{
    DecodedCert cert;   
    int ret;
    int i = 0;
    uint8_t *p;
    uint32_t sz;
    uint32_t len;

    /* DER encoded hex of "1.3.6.1.4.1.14823.4.2.1" */
    uint8_t la_oid[] = {0x2b, 0x06, 0x01, 0x04, 0x01, 0xf3, 0x67, 0x04, 0x02, 0x01};

    if (buf == NULL || buf_len == 0) {
        return BAD_FUNC_ARG;
    }

    InitDecodedCert(&cert, buf, buf_len, 0);

    ret = ParseCert(&cert, CERT_TYPE, NO_VERIFY, 0);
    if (ret != 0) {
        return SSL_FATAL_ERROR;
    }

    ret = 0;
    if (!cert.extExtKeyUsageSet) {
        return ret;
    }

    p = cert.extExtKeyUsageSrc;
    sz = cert.extExtKeyUsageSz;
    while (i < sz) {
        if ((i + 2) >= sz) {
            ret = BUFFER_E;
            break;
        }
        if (p[i++] != ASN_OBJECT_ID) {
            ret = ASN_OBJECT_ID_E;
            break;
        }
        len = p[i++];
        /* len >= 128 is not expected, so treat it like an error */
        if (len >= 0x80 || (i + len) > sz) {
            ret = BUFFER_E;
            break;
        }
        if (len == sizeof(la_oid) && memcmp(p + i, la_oid, sizeof(la_oid)) == 0) {
            printf("la eku set\n");
            ret = 1;
            break;
        }
        i += len;
    }

    FreeDecodedCert(&cert);

    return ret;
}
#endif
int wolfssl_cert_get_aruba_device_type(DecodedCert *cert) 
{
    int ret;
    int i = 0;
    uint8_t *p;
    uint32_t sz;
    uint32_t len;

    /* DER encoded hex of "1.3.6.1.4.1.14823.4.2.1" */
    uint8_t dev_type_oid[] = {0x2b, 0x06, 0x01, 0x04, 0x01, 0xf3, 0x67, 0x04, 0x02, 0x01};

    if (cert == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = ARUBA_DEVICE_TYPE_NOT_SET;
    if (!cert->extExtKeyUsageSet) {
        return ret;
    }

    p = cert->extExtKeyUsageSrc;
    sz = cert->extExtKeyUsageSz;
    while (i < sz) {
        if ((i + 2) >= sz) {
            ret = BUFFER_E;
            break;
        }
        if (p[i++] != ASN_OBJECT_ID) {
            ret = ASN_OBJECT_ID_E;
            break;
        }
        len = p[i++];
        /* len >= 128 is not expected, so treat it like an error */
        if (len >= 0x80 || (i + len) > sz) {
            ret = BUFFER_E;
            break;
        }
        if (len == sizeof(dev_type_oid) && memcmp(p + i, dev_type_oid, sizeof(dev_type_oid) - 1) == 0) {
            //printf("Aruba device type eku set\n");
            ret = p[i + len - 1];
            break;
        }
        i += len;
    }

    return ret;
}

/* Reads content of file to a buffer and returns the buffers
 * The buffer is malloced and it is upto the caller to free it*/
byte *wolfssl_read_file(const char *fname, size_t *len)
{
    FILE *file;
    byte *buf;
    long sz;

    if (fname == NULL) {
        return NULL;
    }
    file = fopen(fname, "rb");
    if (file == NULL) {
        return NULL;
    } 
    if (fseek(file, 0, SEEK_END) < 0) {
        fclose(file);
        return NULL;
    }
    sz = ftell(file);
    if (sz < 0) {
        fclose(file);
        return NULL;
    }
    rewind(file);

    buf = malloc(sz + 1);
    if (buf == NULL) {
        fclose(file);
        return NULL;
    }
    if (fread(buf, 1, sz, file) != sz) {
        fclose(file);
        free(buf);
        return NULL;
    }

    fclose(file);
    buf[sz] = '\0';
    *len = (size_t)sz;

    return buf;
}

int wolfssl_write_to_file(const char *fname, uint8_t *buf, size_t buf_len)
{
    FILE                *fp;
    size_t              ret;

    fp = fopen(fname, "w");
    if (fp == NULL) {
        return SSL_BAD_FILE;
    }
    ret = fwrite(buf, buf_len, 1, fp);
    fclose(fp);
    return ret;
}

#if 0 

/* Work in progress APIs */


static byte *cyassl_X509_get_PEM_from_buf(byte *buf, const char *header, const char *footer, size_t *len)
{
    byte *b, *e;

    b = strstr(buf, header);
    if (b == NULL) {
        CYASSL_MSG("Failed to find PEM header in input buffer");
        return NULL;
    }

    e = strstr(buf, footer);
    if (e == NULL || e <= b) {
        CYASSL_MSG("Failed to find PEM footer in input buffer");
        return NULL;
    }

    e += strlen(footer);
    if (*e == '\r' || *e == '\n') e++;

    *len = e - b;
    return b;
}

/* Parses the first PEM cert in given file and returns it in CYASSL_X509.
 *
 * Donot call CyaSSL_X509_load_certificate_file to parse the cert into x509. if there is an encrypted private key in
 * the file before the cert then the API will fail resulting in the caller considering it as an invalid cert present 
 * int file */
CYASSL_X509 *cyassl_X509_PEM_parse_cert_file(const char *fname)
{
    CYASSL_X509 *x509;
    byte *pem_buf;
    byte *der_buf;
    byte *file_buf;
    byte *der;
    const char *header = "-----BEGIN CERTIFICATE-----";
    const char *footer = "-----END CERTIFICATE-----";
    size_t sz;
    int ret;

#define MAX_DER_BUF_LEN   8192

    CYASSL_ENTER("cyassl_X509_PEM_parse_cert_file");

    file_buf = cyassl_read_file(fname, &sz);
    if (file_buf == NULL) {
        CYASSL_MSG("Failed to get PEM cert from file");
        return NULL;
    }
    pem_buf = cyassl_X509_get_PEM_from_buf(file_buf, header, footer, &sz);
    if (pem_buf == NULL) {
        CYASSL_MSG("Failed to find PEM cert in buffer");
        free(file_buf);
        return NULL;
    }
    der_buf = malloc(MAX_DER_BUF_LEN);
    if (der_buf == NULL) {
        CYASSL_MSG("Failed to allocate DER buffer");
        free(file_buf);
        return NULL;
    }
    ret = CyaSSL_CertPemToDer(pem_buf, sz, der_buf, MAX_DER_BUF_LEN, CERT_TYPE);
    if (ret < 0) {
        CYASSL_MSG("Failed to convert PEM buffer to DER");
        free(file_buf);
        free(der_buf);
        return NULL;
    }
    free(file_buf);

    x509 = CyaSSL_X509_d2i(NULL, der_buf, ret);

    free(der_buf);
    return x509;
}

const char *cyassl_X509_get_pubkey_type(CYASSL_X509 *x509)
{
    CYASSL_EVP_PKEY *pkey;
    const char *key_type = "Unknown";

    pkey = CyaSSL_X509_get_pubkey(x509);
    if(pkey) {
        switch (pkey->type) 
        {
            case DSAk:
                key_type = "DSA"
                break;
            case RSAk:
                key_type = "RSA"
                break;
            case ECDSAk:
                key_type = "ECDSA"
                break;
            default:
                key_type = "Unknown"
                break;
        }
        add_data (cli_amapi_buf, "Signed Using  ", "RSA", TRUE);
        CyaSSL_EVP_PKEY_free(pkey);
    }
    return 
}

static int cyassl_X509_get_notAfterSz(CYASSL_X509 *x509)
{
    CYASSL_ENTER("cyassl_X509_get_notAfterSz");

    if (x509 == NULL)
        return 0;

    return x509->notAfterSz;
}

static int cyassl_X509_get_notBeforeSz(CYASSL_X509* x509)
{
    CYASSL_ENTER("cyassl_X509_get_notBeforeSz");

    if (x509 == NULL)
        return 0;

    return x509->notBeforeSz;
}

const char *cyassl_X509_get_signature_type_str(CYASSL_X509 *x509)
{
    const char *sig_type;

    switch (CyaSSL_X509_get_signature_type(x509))
    {
        case CTC_MD5wRSA:
            sig_type = "MD5-RSA";
            break;
        case CTC_SHAwRSA:
            sig_type = "SHA1-RSA";
            break;
        case CTC_SHAwECDSA:
            sig_type = "SHA1-ECDSA";
            break;
        case CTC_SHA256wRSA:
            sig_type = "SHA256-RSA";
            break;
        case CTC_SHA256wECDSA:
            sig_type = "SHA256-ECDSA";
            break;
        case CTC_SHA384wRSA:
            sig_type = "SHA384-RSA";
            break;
        case CTC_SHA384wECDSA:
            sig_type = "SHA384-ECDSA";
            break;
        case CTC_SHA512wRSA:
            sig_type = "SHA512-RSA";
            break;
        case CTC_SHA512wECDSA:
            sig_type = "SHA512-ECDSA";
            break;
        default:
            sig_type = "unknown";
            break;
    }

    return sig_type;
}


int cyassl_X509_get_serial_number_str(CYASSL_X509 *x509, char *str, int len)
{
    byte            serial[32];
    char            *ptr;
    int             sz, i, n, status;

    if (str == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = sizeof(serial);
    status = CyaSSL_X509_get_serial_number(x509, serial, &sz);
    if (status == SSL_SUCCESS) {
        if (len < (sz * 3)) {
            return BUFFER_E;
        }
        ptr = str; n = 0;
        for (i = 0; i < sz; i++) {
            n = sprintf(ptr, "%02X%s", serial[i], (i == (sz - 1)) ? "" : ":" );
            if (n < 2) {
                status = BUFFER_E;
                break;
            }
            ptr += n; len -= n;
        }
    }
    return status;
}
#endif

