/**
 * \file xservopointer.c
 * \brief Tool that uses a servo to point to the mouse cursor under X.
 * \author Ronald Schaten
 * \version $Id: xservopointer.c,v 1.1 2006/10/28 12:40:42 rschaten Exp $
 *
 * License: See documentation.
 */

#include <math.h>
#include <time.h>

#include <X11/Xlib.h>

#include <usb.h>                /* this is libusb, see http://libusb.sourceforge.net/ */

#include "usbdrv.h"

Display *dpy; /** The display to use */
Window root;  /** The root-window of the display */

int rootwidth, rootheight; /** Measurements of the desktop */
int servoposx, servoposy;  /** Position of the mouse cursor */

usb_dev_handle *handle = NULL; /** Handle to the device */

/**
 * Determines the current cursor position and sets the servo angle.
 */
void update() {
    Window wroot, wchild;
    int absx, absy, relx, rely, angle;
    static int oldabsx = -1000;
    static int oldabsy = -1000;
    unsigned int modmask;
    unsigned char buffer[8];

    // get cursor position
    XQueryPointer(dpy, root, &wroot, &wchild, &absx, &absy, &relx, &rely, &modmask);

    if (oldabsx == absx && oldabsy == absy) {
        // position unchanged
    } else {
        // calculate new angle
        angle = (int)(atan((float)(absx - servoposx) / (absy - servoposy)) * (255 / M_PI));
        angle = 128 - angle;
        printf("absx/absy %d/%d -> angle %d\n", absx, absy, angle);
        int nBytes = usb_control_msg(handle,
                                     USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, CMD_SET, angle, 0,
                                     (char *)buffer, sizeof(buffer), 5000);

        if (nBytes < 0) {
            fprintf(stderr, "USB error: %s\n", usb_strerror());
            exit(1);
        }
    }
    oldabsx = absx;
    oldabsy = absy;
}

/**
 * Main function. Initializes the X-settings and the USB-device, starts the
 * timer and calls the update-function.
 * \param argc Number of arguments.
 * \param argv Arguments.
 * \return Error code.
 */
int main(int argc, char *argv[]) {
    // values for timer
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100 * 1000 * 1000; // every 100ms

    // open display
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "xservopointer: Unable to open display\n");
        exit(1);
    }

    root = DefaultRootWindow(dpy);

    // determine screen width and height
    XWindowAttributes Attributes;
    XGetWindowAttributes(dpy, root, &Attributes);
    rootwidth = Attributes.width;
    rootheight = Attributes.height;

    // set servo position
    servoposx = rootwidth / 2;  // middle of the screen
    servoposy = -150;           // above the screen

    // initialize USB-device
    usb_init();
    if (usbOpenDevice(&handle, USBDEV_SHARED_VENDOR, "www.schatenseite.de", USBDEV_SHARED_PRODUCT, "USB-Servo") != 0) {
        fprintf(stderr, "Could not find USB device \"USB-Servo\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
        exit(1);
    }

    // main loop
    while (1) {
        update();
        nanosleep(&ts, NULL);
    }
    usb_close(handle);
    return 0;
}
