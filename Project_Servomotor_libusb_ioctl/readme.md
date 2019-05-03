Servo Motor Control using ATmega8A

->Summary:
The main aim of this project is to control servo motor connected to ATmega8 using CLI. Servos are connected with three-wire-cables. A red and a black one for the power, and a yellow one for the signal. Power has to be between 4.8 and 6 volts, so the 5 volts from the USB-port is in the range. The angle of the servo is controlled with pulse width modulation (PWM).

Two implementations were tested. One using libusb and another using ioctl.

->Schematic:
![alt text](https://github.com/h20180142/G547/blob/master/Project_Servomotor_libusb_ioctl/circuit%20schematic.png)

->Prototype:
![alt text](https://github.com/h20180142/G547/blob/master/Project_Servomotor_libusb_ioctl/IMG_20190428_162544450_HDR1.jpg)

->Compiling & Uploading the firmware(firmware directory):

This project requires avr-gcc and avr-libc.

Steps to build:

1. Set the fuse-bits to use the external crystal:

`$ avrdude -p atmega8 -P /dev/parport0 -c stk200 -U hfuse:w:0xC9:m -U lfuse:w:0x9F:m`

2. Compile

`$ make`

3. Flash

`$ make program`

->Compiling & executing libusb userspace(comandline-libusb directory)

1. build

`$ make all`

2. Usage

`$ ./usb-servo status` Returns the current servo motor angle

`$ ./usb-servo set <angle>` Rotates the motor to the requested angle

->Compiling & Uploading the ioctl userspace and kernel module (comandline-ioctl directory)

1. Compiling

`$ make all`

2. Inserting

`$ make insert`

3. To compile user space code

`$ make user`

4. To execute the user space code: (Same as libusb)
