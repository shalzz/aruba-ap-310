#ifndef __AT97SC3203_H_
#define __AT97SC3203_H_

#define MAX_TPM 1

struct tpm_func {
    unsigned long     val;
};

#define TPM_IS_3204	_IOW('p', 0x0a, struct tpm_func) /* Write AT97SC3203  params  */

#endif
