#ifndef SERIAL_H
#define SERIAL_H
#define     BAUD_2400       2400  
#define     BAUD_4800       4800  
#define     BAUD_9600       9600  
#define     BAUD_115200     115200  
#define     BAUD_460800     460800  

#define     PARITY_ODD    'O' 
#define     PARITY_EVEN   'E' 
#define     PARITY_NONE   'N'

#define     STOP_BIT_1     1  
#define     STOP_BIT_2     2  

#define     DATA_BIT_7     7  
#define     DATA_BIT_8     8  

class Serial{
	public:
		int open_port(int fd,int comport);  
		int set_opt(int fd,int nSpeed, int nBits,char nEvent, int nStop);  
		int read_datas_tty(int fd,char *rcv_buf,int TimeOut,int Len);  
		int send_data_tty(unsigned char *send_buf,int Len);  
		int init();
	private:
		int SerFd;
};


#endif
