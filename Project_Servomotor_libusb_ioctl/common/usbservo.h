#ifndef __usbservo_h_included__
#define __usbservo_h_included__

/**
 * \file usbservo.h
 * \brief Global definitions and datatypes, used by the firmware and the commandline-client. Also contains the main doxygen-documentation.
 * \author Ronald Schaten
 * \version $Id: usbservo.h,v 1.1 2006/10/28 12:40:42 rschaten Exp $
 *
 * License: See documentation.
 */

/**
 * \mainpage USB-Servo
 *
 * \section sec_intro Introduction
 *
 * The USB-Servo is a device to control a servo via USB. A servo is a motorized
 * device that is commonly used in remote controlled cars and planes. I built
 * this device to activate a toy puppet. The puppet has a button on its bottom,
 * if you press the button the puppet collapses. When the computer is able to
 * press the button, I can use the puppet to signal information like someone's
 * online-state in the Jabber-network: when my friend goes online, the puppet
 * stands up, when he logs off it collapses.
 *
 * Servos are connected with three-wire-cables. A red and a black one for the
 * power, and a yellow one for the signal. Power has to be between 4.8 and 6
 * volts, so the 5 volts from the USB-port is in the range. The signal doesn't
 * take much current, so you can connect it directly to the controller. The
 * angle of the servo is controlled with pulse width modulation (PWM). It gets
 * a signal of about 50Hz (one pulse every 20ms), the length of the pulse tells
 * the servo the angle to adjust.
 *
 * A problem that I didn't really solve is the power consumption: I don't know
 * the current that runs through the motor. It seems to be low enough not to
 * cause any problems, but I don't know how high it will get when the servo is
 * blocked. <b>YOU HAVE BEEN WARNED</b>, I don't feel responsible for USB-ports
 * catching fire... :-/
 *
 * There are three parts included in the distribution: The firmware for an
 * ATmega8 microcontroller, a commandline-client that can be run under Linux,
 * and the circuits needed to build the device.
 *
 * This project is based on my USB-LED-Fader, which itself is based on the
 * PowerSwitch example application by Objective Development. Like those, it
 * uses Objective Development's firmware-only USB driver for Atmel's AVR
 * microcontrollers.
 *
 * Objective Development's USB driver is a firmware-only implementation of the
 * USB 1.1 standard (low speed device) on cheap single chip microcomputers of
 * Atmel's AVR series, such as the ATtiny2313 or even some of the small 8 pin
 * devices. It implements the standard to the point where useful applications
 * can be implemented. See the file "firmware/usbdrv/usbdrv.h" for features and
 * limitations.
 *
 * \section sec_install Building and installing
 *
 * Both, the firmware and Unix command line tool are built with "make". You may
 * need to customize both makefiles.
 *
 * \subsection sec_fw Firmware
 *
 * The firmware for this project requires avr-gcc and avr-libc (a C-library for
 * the AVR controller). Please read the instructions at
 * http://www.nongnu.org/avr-libc/user-manual/install_tools.html for how to
 * install the GNU toolchain (avr-gcc, assembler, linker etc.) and avr-libc.
 *
 * Once you have the GNU toolchain for AVR microcontrollers installed, you can
 * run "make" in the subdirectory "firmware". You may have to edit the Makefile
 * to use your preferred downloader with "make program". The current version is
 * built for avrdude with a parallel connection to an stk200-compatible
 * programmer.
 *
 * If working with a brand-new controller, you may have to set the fuse-bits to
 * use the external crystal:
 *
 * \code
 * avrdude -p atmega8 -P /dev/parport0 -c stk200 -U hfuse:w:0xC9:m -U lfuse:w:0x9F:m
 * \endcode
 *
 * Afterwards, you can compile and flash to the device:
 *
 * \code
 * make program
 * \endcode
 *
 * \subsection sec_client Commandline client and demo application
 *
 * The command line tool and the demo application require libusb. Please take
 * the packages from your system's distribution or download libusb from
 * http://libusb.sourceforge.net/ and install it before you compile. Change to
 * directory "commandline", check the Makefile and edit the settings if
 * required and type
 *
 * \code
 * make
 * \endcode
 *
 * This will build the unix executable "usb-servo" which can be used to control
 * the device, and the demo application "xservopointer".
 *
 * \section sec_usage Usage
 *
 * Connect the device to the USB-port. If it isn't already, the servo will move
 * to the 0-position.
 *
 * \subsection sec_commandline Commandline client
 *
 * Use the commandline-client as follows:
 *
 * \code
 * usb-servo status
 * usb-servo set <angle>
 * usb-servo test
 * \endcode
 *
 * \subsubsection sec_params Parameters
 *
 * - \e angle: The angle you want to set the servo to. 0 is full left, 255 is
 *   full right.
 *
 * \subsubsection sec_examples Examples
 *
 * <b>Get the status of the servo:</b>
 * \code
 * usb-servo status
 * \endcode
 * This will tell you the angle the servo is currently put to.
 * \code
 * Current servo angle: 42
 * \endcode
 *
 * <b>Set a new angle:</b>
 * \code
 * usb-servo set 23
 * \endcode
 * This sets the servo to the angle 23. 0 is full left, 255 is full right, so
 * with 23 the servo will be almost on the left side.
 *
 * <b>Test the device:</b>
 * \code
 * usb-led-fader test
 * \endcode
 * This function sends many random numbers to the device. The device returns
 * the packages, and the client looks for differences in the sent and the
 * received numbers.
 *
 * \subsection sec_xservopointer Demo application xservopointer
 *
 * This is a pure fun thing, nobody will need it. That was reason enough to
 * write it...
 *
 * To use it, you have to position the servo centered above the screen (with a
 * little tweaking in the source, you can change that position). Then, you
 * attach a pointer to the servo and start the application.
 *
 * You'll never ever have to search for your mouse cursor in the future. The
 * pointer on the servo will always show you where to search.
 *
 * \section sec_drawbacks Drawbacks
 *
 * The main drawback is the mentioned power consumption. I tested it with my
 * servo on my notebook, it is not sure to work on other systems. <b>THIS MAY
 * BE HARMFUL FOR YOUR COMPUTER</b>, and nobody but yourself will be
 * responsible for any damages.
 *
 * Another, not so big problem is the crude implementation of the PWM. I got
 * the timing-values by trial and error, and they might not fit on your servo.
 * On the other hand, I think that servos should be interchangeable. But this
 * is my first and only one, so I can't say anything about that.
 *
 * \section sec_files Files in the distribution
 *
 * - \e Readme.txt: Documentation, created from the htmldoc-directory.
 * - \e firmware: Source code of the controller firmware.
 * - \e firmware/usbdrv: USB driver -- See Readme.txt in this directory for
 *   info
 * - \e commandline: Source code of the host software (needs libusb). Here, you
 *   find the pure commandline client (usb-servo) and the fun demo application
 *   (xservopointer).
 * - \e common: Files needed by the firmware and the commandline-client.
 * - \e circuit: Circuit diagrams in PDF and EAGLE 4 format. A free version of
 *   EAGLE is available for Linux, Mac OS X and Windows from
 *   http://www.cadsoft.de/.
 * - \e License.txt: Public license for all contents of this project, except
 *   for the USB driver. Look in firmware/usbdrv/License.txt for further info.
 * - \e Changelog.txt: Logfile documenting changes in soft-, firm- and
 *   hardware.
 *
 * \section sec_thanks Thanks!
 *
 * I'd like to thank <b>Objective Development</b> for the possibility to use
 * their driver for my project. In fact, this project wouldn't exist without
 * the driver.
 *
 * \section sec_license About the license
 *
 * My work - all contents except for the USB driver - are licensed under the
 * GNU General Public License (GPL). A copy of the GPL is included in
 * License.txt. The driver itself is licensed under a special license by
 * Objective Development. See firmware/usbdrv/License.txt for further info.
 *
 * <b>(c) 2006 by Ronald Schaten - http://www.schatenseite.de</b>
 */

#include <stdint.h>

/* return codes for USB-communication */
#define msgOK 0     /**< Return code for OK. */
#define msgErr 1    /**< Return code for Error. */

/* These are the vendor specific SETUP commands implemented by our USB device */
#define CMD_ECHO  0 /**< Command to echo the sent data */
#define CMD_GET   1 /**< Command to fetch values */
#define CMD_SET   2 /**< Command to send values */

#endif
