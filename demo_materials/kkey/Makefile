obj-m += kkey.o

.PHONY: all build clean load unload play reset

all: unload clean build load

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(PWD)
	gcc reset.c -o reset

clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)
	-rm reset
	-rm kkey.mid
	-rm examples/race

load: build
	sudo insmod kkey.ko

unload:
	-sudo rmmod kkey

play:
	cat /dev/kkey000 > kkey.mid
	timidity kkey.mid

reset:
	gcc reset.c -o reset
	./reset

race:
	gcc -D_GNU_SOURCE examples/race.c -o examples/race
	./runrace.sh
	echo 100 > /sys/class/kkey/kkey_slowdown
	make play
