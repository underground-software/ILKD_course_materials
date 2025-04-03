obj-m += kdlpdev.o

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)

load: build
	sudo insmod kdlpdev.ko

unload:
	sudo rmmod kdlpdev
