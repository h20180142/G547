#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() 
{
	char axis;
	int buff;
	int value;
	int fd;
	do
	{
		printf("3 axis Acceleromete\n");
		printf("press x for X-axis\npress y for Y-axis\npress z for Z-axis\nPress e to exit\n\n");
		scanf(" %c", &axis);
		if (axis == 'x')
		{
			fd = open("/dev/adxl_x", O_RDWR);
			if(fd < 0) 
			{
				printf("Cannot open device file...\n");
                		return 0;
        		}
			value=read(fd, &buff, 10);
			printf("x axis: %d\n\n\n",buff);
		}
		else if (axis == 'y')
		{
			fd = open("/dev/adxl_y", O_RDWR);
			if(fd < 0) 
			{
				printf("Cannot open device file...\n");
                		return 0;
        		}
			value=read(fd, &buff, 10);
			printf("y axis: %d\n\n\n",buff);
		}
		else if (axis == 'z')
		{
			fd = open("/dev/adxl_z", O_RDWR);
			if(fd < 0) 
			{
				printf("Cannot open device file...\n");
                		return 0;
        		}
			value=read(fd, &buff, 10);
			printf("z axis: %d\n\n\n",buff);
		}
		else
		{
			printf("Please enter a valid key\n\n\n");
		}
	}while(axis!='e');
	close(fd);
	return 0;
}
