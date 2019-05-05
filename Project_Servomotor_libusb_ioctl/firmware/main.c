/**
 * \file main.c
 * \brief Firmware for the USB-Servo
 * \author Ronald Schaten
 * \version $Id: main.c,v 1.1 2006/10/28 12:40:42 rschaten Exp $
 *
 * License: See documentation.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "oddebug.h"
#include "usbservo.h"

/** Global variable, contains the angle of the servo. */
static uint8_t angle;

/**
 * Handler for the timer-interrupt. This function is called every 20
 * milliseconds (50Hz).
 */
static void timerInterrupt(void) {
    int i;
    PORTC = 0xff;
    // wait 7200us (value by trial & error)
    for (i = 0; i < 72; i++) {
        _delay_us(100);
    }
    // still have to wait up to 17850us (value by trial & error)
    for (i = 0; i < angle; i++) {
        _delay_us(70);          // 17850 / 255 = 70
    }
    PORTC = 0x00;
}

/**
 * USB-Setup-Handler. Handles setup-calls that are received from the
 * USB-Interface.
 * \param data Eight bytes of data.
 * \return The number of returned bytes (in replyBuffer[]).
 */
uchar usbFunctionSetup(uchar data[8]) {
    static uchar replyBuffer[8];
    uchar replyLength;

    replyBuffer[0] = msgOK;
    switch (data[1]) {
    case CMD_ECHO:             /* echo */
        replyBuffer[0] = data[2];
        replyBuffer[1] = data[3];
        replyLength = 2;
        break;
    case CMD_GET:              /* read status */
        replyBuffer[0] = angle;
        replyLength = 1;
        break;
    case CMD_SET:              /* set status */
        angle = data[2];
        replyLength = 0;
        break;
    default:                   /* WTF? */
        replyBuffer[0] = msgErr;
        replyLength = 1;
        break;
    }
    usbMsgPtr = replyBuffer;
    return replyLength;
}

/**
 * Main-function. Initializes the hardware and starts the main loop of the
 * application.
 * \return An integer. Whatever... :-)
 */
int main(void) {
    uchar i, j;
    odDebugInit();
    DDRD = ~0;                  /* output SE0 for USB reset */
    PORTD = 0x00;               /* no pullups on USB pins */
    DDRC = 0xff;                /* all outputs */
    PORTC = 0x00;

    j = 0;
    while (--j) {               /* USB Reset by device only required on
                                   Watchdog Reset */
        i = 0;
        while (--i);            /* delay >10ms for USB reset */
    }
    DDRD = ~USBMASK;            /* all outputs except USB data */
    TCCR0 = 5;                  /* set prescaler to 1/1024 */
    usbInit();
    sei();

    for (i = 0; i < 10; i++) {  /* wait one second to prevent strange
                                   effects when the USB-bus isn't
                                   initialized (e. g. when the host system
                                   is on standby. */
        _delay_ms(100);
    }

    angle = 0;

    while (1) {                 /* main event loop */
        usbPoll();
        if (TIFR & (1 << TOV0)) {
            TIFR |= 1 << TOV0;  /* clear pending flag */
            timerInterrupt();
        }
    }
    return 0;
}
