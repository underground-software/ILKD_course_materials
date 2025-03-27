#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <err.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mount_point>\n", argv[0]);
        exit(1);
    }

    const char *mount_point = argv[1];
    int fd = open(mount_point, O_RDONLY);
    if (fd == -1)
        err(1, "Error opening mount point");

    printf("Freezing filesystem mounted at %s\n", mount_point);
    if (ioctl(fd, FIFREEZE, 0) == -1) {
        err(1, "Error freezing filesystem");
        close(fd);
    }

    printf("Filesystem frozen. Press Enter to thaw...");
    getchar();

    printf("Thawing filesystem\n");
    if (ioctl(fd, FITHAW, 0) == -1) {
        err(1, "Error thawing filesystem");
        close(fd);
    }

    printf("Filesystem thawed\n");

    close(fd);
    return 0;
}
