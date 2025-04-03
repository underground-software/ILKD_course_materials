//auto-generated C/MIDI IR

#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>



int main(void){

    int kkey[128];

    char buf[64];

    for(int i=0; i<128; ++i){

        sprintf(buf, "/dev/kkey%03d", i);

        kkey[i] = open(buf, O_NONBLOCK | O_RDWR);

    }
    write(kkey[60], "1", 1);
    usleep(24000);
    write(kkey[60], "0", 1);
    usleep(24000);
    write(kkey[60], "1", 1);
    usleep(24000);
    write(kkey[60], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[69], "1", 1);
    usleep(24000);
    write(kkey[69], "0", 1);
    usleep(24000);
    write(kkey[69], "1", 1);
    usleep(24000);
    write(kkey[69], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(72000);
    write(kkey[65], "1", 1);
    usleep(24000);
    write(kkey[65], "0", 1);
    usleep(24000);
    write(kkey[65], "1", 1);
    usleep(24000);
    write(kkey[65], "0", 1);
    usleep(24000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[62], "1", 1);
    usleep(24000);
    write(kkey[62], "0", 1);
    usleep(24000);
    write(kkey[62], "1", 1);
    usleep(24000);
    write(kkey[62], "0", 1);
    usleep(24000);
    write(kkey[60], "1", 1);
    usleep(24000);
    write(kkey[60], "0", 1);
    usleep(72000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[65], "1", 1);
    usleep(24000);
    write(kkey[65], "0", 1);
    usleep(72000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[62], "1", 1);
    usleep(24000);
    write(kkey[62], "0", 1);
    usleep(72000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[65], "1", 1);
    usleep(24000);
    write(kkey[65], "0", 1);
    usleep(24000);
    write(kkey[65], "1", 1);
    usleep(24000);
    write(kkey[65], "0", 1);
    usleep(24000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[62], "1", 1);
    usleep(24000);
    write(kkey[62], "0", 1);
    usleep(72000);
    write(kkey[60], "1", 1);
    usleep(24000);
    write(kkey[60], "0", 1);
    usleep(24000);
    write(kkey[60], "1", 1);
    usleep(24000);
    write(kkey[60], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[69], "1", 1);
    usleep(24000);
    write(kkey[69], "0", 1);
    usleep(24000);
    write(kkey[69], "1", 1);
    usleep(24000);
    write(kkey[69], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(72000);
    write(kkey[65], "1", 1);
    usleep(24000);
    write(kkey[65], "0", 1);
    usleep(24000);
    write(kkey[65], "1", 1);
    usleep(24000);
    write(kkey[65], "0", 1);
    usleep(24000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[64], "1", 1);
    usleep(24000);
    write(kkey[64], "0", 1);
    usleep(24000);
    write(kkey[62], "1", 1);
    usleep(24000);
    write(kkey[62], "0", 1);
    usleep(24000);
    write(kkey[67], "1", 1);
    usleep(24000);
    write(kkey[67], "0", 1);
    usleep(24000);
    write(kkey[60], "1", 1);
    usleep(24000);
    write(kkey[60], "0", 1);

    for(int i=0; i<128; ++i){

        close(kkey[i]);

    }

    return 0;
}

