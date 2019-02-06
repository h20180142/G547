-->To build and insert module into kernel

$make all
$sudo insmod adxl.ko


-->To compile and execute user application

$gcc useradxl.c
$gcc -o useradxl useradxl.c
$./useradxl


-->To get the value of acceleration in X, Y or Z axis enter x, y or z.


-->To remove module from kernel
$sudo rmmod adxl.ko
