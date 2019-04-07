#ifndef _DES_ENC_H_
#define _DES_ENC_H_

#define MAX_SECRET_SZ   64
#define MAX_ENCODED_SZ  2*MAX_SECRET_SZ
int ap_prov_encrypt(__u8 * clear, int ilen, __u8 * cipher, int *olen);
int ap_prov_decrypt(__u8 * cipher, int ilen, __u8 * clear, int *olen);
int ap_prov_encrypt_cert(__u8 * clear, int ilen, __u8 * cipher, int *olen);
int ap_prov_decrypt_cert(__u8 * cipher, int ilen, __u8 * clear, int *olen);

void bytes_to_asc(__u8 * in, int ilen, __u8 * out);
void asc_to_bytes(__u8 * in, int ilen, __u8 * out);

#endif
