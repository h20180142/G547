#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include "servo_ioctl.h"
#include <sys/ioctl.h>
#include <string.h>


int main(int argc, char *argv[])
{ 
int ret, arg, fd, retval;	
uint16_t value;  

	if(argc==1)
	{
		printf("To set servo angle: $./usbservo set <0 to 255> \n");					
		exit(1);
	}

	if(strcmp(argv[1], "set") == 0)
	{
		arg = atoi(argv[2]);
		printf("arg=%d\n", arg);
		fd = open("/dev/servo",O_RDWR);
		if(fd==-1)
		{
			printf("Give necessary permissions to file /dev/myadc\n");
			return -1;
		}
		retval = ioctl(fd, IOCTL_SET, arg);
		printf("return value of IOCTL=%d\n", retval);
		close(fd);
	}

 return 0;
}
