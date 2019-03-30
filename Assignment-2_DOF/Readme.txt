-------------------------------------------------------------------------------------
Instructions to build and load driver module:
-------------------------------------------------------------------------------------
$ make all
$ sudo insmod BD1.ko

-------------------------------------------------------------------------------------
Intructions to test
-------------------------------------------------------------------------------------
1. dd:
$ dd if=/dev/zero of=/dev/dor1 count=500 (this writes zero to a sector)
2. writing:
$ cat > /dev/dor1 (writes to the block device)
3. viewing info and data:
$ sudo fdisk -l /dev/dor1
$ sudo xxd /dev/dor1 | less (view actual data and pipe to less for readability)
4. removing driver:
$ sudo rmmod BD1.ko
