#include <linux/version.h>

#if KERNEL_VERSION(2,6,24) < LINUX_VERSION_CODE
#warning this driver requires kernel version 2.6.25 or later, compiling anyway
#endif

