/* Values that cmd_id can take */
#define AP1X_SUPPLICANT_READY		1
#define AP1X_SUPPLICANT_SUCCESS		2
#define AP1X_SUPPLICANT_FAIL		3
#define AP1X_SUPPLICANT_TIMEOUT		4

typedef struct ap1x_to_sapd_msg {
    u_int32_t cmd_id;
    u_int8_t data[0];
} __attribute__ ((packed)) ap1x_to_sapd_msg_t;

#define SAPD_UNIX_SOCK_NAME_FOR_AP1X		"/tmp/sapd_1x"

#define AP1XCERT_FACTORY_PRIVATE    "tpmdev"
#define AP1XCERT_FACTORY_CERT       "/tmp/deviceCerts/certifiedKeyCert.der"

/* private key encrypt types */
typedef enum {
  AP1X_CERT_ENCRYPT_NONE,      /* none */
  AP1X_CERT_ENCRYPT_PROV,      /* ap_prov_decrypt_cert */
} ap1x_cert_encrypt_type_t;


