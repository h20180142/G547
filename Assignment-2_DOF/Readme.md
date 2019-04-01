-------------------------------------------------------------------------------------

Instructions to build and load driver module:

-------------------------------------------------------------------------------------

   $ make
 all
   $ sudo insmod main.ko

-------------------------------------------------------------------------------------

Intructions to test

-------------------------------------------------------------------------------------

1. dd:

   $ sudo -s

   $ dd if=/dev/zero of=/dev/dof1 count=200 (this writes zero to a sector)


2. writing:

   $ cat > /dev/dof1 (write some text here)


3. viewing info and data:

   $ fdisk -l

   $ xxd /dev/dof1 | less (view actual data and pipe to less for readability)


4. removing driver:

   $ sudo rmmod main.ko