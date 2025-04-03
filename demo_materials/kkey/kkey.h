#ifndef KKEY_H
#define KKEY_H

#ifdef __KERNEL__
#include <linux/ioctl.h>
#else /* userspace */
#include <sys/ioctl.h>
#endif

#define KKEY_IOC_RESET _IO(0x11, 0)

#endif
