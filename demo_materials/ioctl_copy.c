#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <err.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <dest_file>\n", argv[0]);
        exit(1);
    }

    const char *src_path = argv[1];
    const char *dst_path = argv[2];

    // Open source file
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1)
        err(1, "Error opening source file");

    // Open destination file (create if it doesn't exist)
    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (dst_fd == -1) {
        close(src_fd);
        err(1, "Error opening destination file");
    }

    // Perform the file clone operation
    if (ioctl(dst_fd, FICLONE, src_fd) == -1) {
        close(src_fd);
        close(dst_fd);
        err(1, "Error cloning file");
    }

    printf("File cloned successfully.\n");

    close(src_fd);
    close(dst_fd);
    return 0;
}
