obj-m:=servo_ioctl.o
 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insert: servo_ioctl.ko
	sudo insmod servo_ioctl.ko

remove: servo_ioctl.ko
	sudo rmmod servo_ioctl

clean:
	rm -rf *.o *.order *.ko *.mod *.symvers *.mod.c

user:
	gcc -o usbservo userapp.c
