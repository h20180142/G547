
#ifndef _SERVO_IOCTL_H
#define _SERVO_IOCTL_H

#define EP_OUT 0x00

#define MAGIC_NUMBER 'T'
#define SET 0x02
#define STATUS 0x01

#define SEND_THRESHOLD 1

#define IOCTL_SET _IOW(MAGIC_NUMBER, SET, char)
#define IOCTL_STATUS _IOR(MAGIC_NUMBER, STATUS, char)

#endif
