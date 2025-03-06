#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define FILE_NAME "example.txt"

int main() {
    int fd;
    struct flock fl = {0};
    char *message = "Hello, POSIX locks!\n";

    // Open the file
    fd = open(FILE_NAME, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    printf("Attempting to lock the file...\n");

    // Prepare the lock structure
    fl.l_type = F_WRLCK;    // Write lock
    fl.l_whence = SEEK_SET; // Lock starts at beginning of file
    fl.l_start = 0;         // Starting offset
    fl.l_len = 0;           // Lock entire file (0 means to EOF)

    // Try to set the lock
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl error");
        close(fd);
        exit(1);
    }

    printf("File locked successfully. Writing data...\n");

    // Write to the file
    if (write(fd, message, strlen(message)) == -1) {
        perror("Write error");
        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);
        close(fd);
        exit(1);
    }

    printf("Data written. Press Enter to unlock...");
    getchar();

    // Unlock the file
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl unlock error");
        close(fd);
        exit(1);
    }

    printf("File unlocked.\n");

    close(fd);
    return 0;
}
