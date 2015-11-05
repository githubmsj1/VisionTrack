
#include"serial.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>  
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
//#include<opencv2/opencv.hpp>
//using namespace cv;
int Serial::set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)  
{	
	struct termios newtio, oldtio;  
	if (tcgetattr(fd, &oldtio) != 0)
    	{  
            perror("SetupSerial 1");  
	    return -1; 
	}
	bzero(&newtio, sizeof(newtio));  
	newtio.c_cflag |= CLOCAL;
	newtio.c_cflag |= CREAD;//read enable

	newtio.c_cflag |= CS8;  //8 bit data
	newtio.c_cflag &= ~PARENB;//odd check
	
	cfsetispeed(&newtio, B115200); //bundrate
	cfsetospeed(&newtio, B115200);  

	newtio.c_cflag &= ~CSTOPB;  //stop bit set

	newtio.c_cc[VTIME] = 0;//wait time
    	newtio.c_cc[VMIN] = 0; //minimun

	tcflush(fd, TCIFLUSH); //no recive deal

	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)  //activate
	{  
	            perror("com set error");  
		            return -1; 
	}  

	printf("set done!\n"); 
	return 0;
}

int read_datas_tty(int fd, char *rcv_buf, int TimeOut, int Len)  
{
	int retval;
	fd_set rfds;  
	struct timeval tv;  
	int ret, pos;  
	tv.tv_sec = TimeOut / 1000;  //set the rcv wait time  
	tv.tv_usec = TimeOut % 1000 * 1000;  //100000us = 0.1s 
	retval = select(fd + 1, &rfds, NULL, NULL, &tv);

	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		retval = select(fd + 1, &rfds, NULL, NULL, &tv);
		if (retval == -1)  
		{  
			perror("select()"); 
			break;  
		}  
		else if (retval)  
		{
			ret = read(fd, rcv_buf + pos, 1);	
			if (-1 == ret) 
			{
				break;
			}
			pos++;  
			if (Len <= pos)  
			{
				break;  
			}
		}
		else
		{
			break;
		}

	}
	
		return pos; 

	
}

int Serial::send_data_tty(unsigned char *send_buf, int Len) 
{
	ssize_t ret;
	ret = write(SerFd, send_buf, Len); 
	if (ret == -1) 
	{
		printf("write device error\n"); 
		return -1;
	}
	 return 0;  
}

int Serial::init()
{	
	SerFd = -1;
	SerFd = open("/dev/ttyUSB0", O_RDWR|O_NOCTTY|O_NDELAY); 
	if (0 < SerFd)  
	{
		set_opt(SerFd, BAUD_2400, DATA_BIT_8, PARITY_NONE, STOP_BIT_1);
	}
	else
	{
		 printf("open_port ERROR !\n");  
		 return -1;

	}
	return 0;
}

/*int main()
{

	int nTmp = 0;  
	char Buf[1024];
	Serial serial;
	serial.init();
	while(1)
	{
		serial.send_data_tty("hello series\n",sizeof("hello series\n")); 
		waitKey(100);
	}

	return 0;
}	
*/
