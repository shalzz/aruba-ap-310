#ifndef __CYASSL_X509_API_H__
#define __CYASSL_X509_API_H__

#include <cyassl/ssl.h>
#include <wolfcrypt/asn.h>
#include <wolfcrypt/rsa.h>
#include <stdint.h>

#define MSGH_DTLS_ARUBA_LA_POLICY_OID "1.3.6.1.4.1.14823.4.1.1"
#define MSGH_DTLS_ARUBA_POLICY_OID "1.3.6.1.4.1.311.509.3.1"

enum {
    ARUBA_DEVICE_TYPE_NOT_SET = 0,
    ARUBA_DEVICE_TYPE_AP = 1,
};
const char *cyassl_X509_get_signature_type_str(CYASSL_X509 *x509);
int cyassl_X509_get_serial_number_str(CYASSL_X509 *x509, char *str, int len);
int cyassl_X509_get_pubkey_size(CYASSL_X509 *x509);
int cyassl_X509_get_notbefore_str(CYASSL_X509 *x509, char *str, uint32_t len);
int cyassl_X509_get_notafter_str(CYASSL_X509 *x509, char *str, uint32_t len);

int wolfssl_get_rsa_pubkey(const char *name, int format, RsaKey *key, uint32_t *key_len);
int wolfssl_gen_rsa_key(RsaKey *rsa_key, int size);
int wolfssl_free_rsa_key(RsaKey *rsa_key);

int wolfssl_init_cert(DecodedCert *cert, byte *buf, word32 buf_len);
void wolfssl_free_cert(DecodedCert *cert);
int wolfssl_cert_policy_ext_set(DecodedCert *cert);
int wolfssl_cert_eku_ext_set(DecodedCert *cert);
int wolfssl_cert_policy_oid_set(DecodedCert *cert, char *oid);
int wolfssl_cert_la_oid_set(byte *buf, word32 buf_len);
int wolfssl_cert_get_aruba_device_type(DecodedCert *cert);

int wolfssl_cm_der_verify_load_ca(WOLFSSL_CERT_MANAGER *cm, const char *ca_name);

byte *wolfssl_read_file(const char *fname, size_t *len);
int wolfssl_write_to_file(const char *fname, uint8_t *buf, size_t buf_len);

#endif
