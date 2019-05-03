# $Id: Makefile,v 1.1 2006/10/28 12:40:42 rschaten Exp $

AVRDUDE = avrdude -p atmega8 -c usbasp
F_CPU = 16000000

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) -Iusbdrv -I../common -I. -mmcu=atmega8 #-DDEBUG_LEVEL=2
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o
# Note that we link usbdrv.o first! This is required for correct alignment of
# driver-internal global variables!


# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

program:	all
	$(AVRDUDE) -U flash:w:main.hex

clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s

# file targets:
main.bin:	$(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS)

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c
