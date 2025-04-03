#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>

#include "kkey.h"

int main(int argc, char * argv[])
{
	int fd;

	if ((fd = open("/dev/kkey000", O_RDWR)) < 0)
		err(1, "open kkey000");

	if (ioctl(fd, KKEY_IOC_RESET, 0))
		err(1, "ioctl kkey000");

	if (close(fd))
		err(1, "close kkey000");

	return 0;
}
