#ifdef CONFIG_OCTEON
#include "pci_octeon.h"
#elif defined(ARUBA_FREESCALE) || defined(CONFIG_IPROC) || defined(CONFIG_MACH_IPQ806x)
#include "pci_fsl.h"
#else
#include "pci_atheros.h"
#endif
