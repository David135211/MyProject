#include     <stdio.h>
#include     <stdlib.h>
#include     <termios.h>
#include     <unistd.h>
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>
#include     <pthread.h>
#include     <string.h>
#include     <errno.h>

#include	"VirtCOM.h"

#define CMSPAR	  010000000000

#define FALSE  -1
#define TRUE   0
int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, B110};
int baudrate_arr[] = {115200,57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 110};
//int fd;
pthread_mutex_t mutex;
struct timespec delay;

extern VIRT_COM_DEVSTATE VirtCOM_DevState;
extern DEV_CONFIG gDevConfig;

void set_baudrate(int fd, int baudrate)
{
	int   i;
	int   status;
	struct termios   Opt;
	tcgetattr(fd, &Opt);
	for(i= 0;  i < sizeof(speed_arr) / sizeof(int); i++)
	{
		if(baudrate == baudrate_arr[i])
		{
			printf("Set Baudrate to %d\n", baudrate);
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if(status != 0)
			{
				perror("tcsetattr fd err");
				return;
			}
			tcflush(fd, TCIOFLUSH);
		}
	}

	tcgetattr(fd, &Opt);
	Opt.c_cflag |= CS8;
	Opt.c_cflag &= ~CSTOPB;
	Opt.c_cflag &= ~PARENB;
	Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    Opt.c_cflag |= (CLOCAL | CREAD);
	Opt.c_iflag &= ~(INLCR | ICRNL | IGNCR);
	Opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	Opt.c_oflag &= ~(ONLCR | OCRNL);

	tcflush(fd, TCIOFLUSH);
	tcsetattr(fd, TCSANOW, &Opt);

	printf("set_baudrate ok \n");
}

int set_Parity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	if(tcgetattr( fd,&options) != 0) {
		return FALSE;
	}
	options.c_cflag &= ~CSIZE;
	printf("Set Databit to %d\n", databits);
	switch (databits)
	{
	case 5:
		options.c_cflag |= CS5;
	case 6:
		options.c_cflag |= CS6;
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr, "Unsupported data size\n");
		return FALSE;
	}

	switch (parity)
	{
		case 'n':
		case 'N':
			printf("Set parity to NONE\n");
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			printf("Set parity to ODD(Ji)\n");
			options.c_cflag |= (PARODD | PARENB);
			options.c_iflag |= INPCK;             	/* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			printf("Set parity to EVEN(ou)\n");
			options.c_cflag |= PARENB;    		 /* Enable parity */
			options.c_cflag &= ~PARODD;
			options.c_iflag |= INPCK;       	 /* Disnable parity checking */
			break;
		case 'M':
		case 'm':
			printf("Set parity to MARK\n");
			options.c_cflag |= (PARENB | CMSPAR | PARODD);
			options.c_iflag |= INPCK;       	 /* Disnable parity checking */
			break;
		case 'S':
		case 's':
			printf("Set parity to SPACE\n");
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;
		default:
			fprintf(stderr, "Unsupported parity\n");
			return FALSE;
	}

	printf("Set stopbits to %d\n", stopbits);
	switch (stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return FALSE;
	}

	if ((parity != 'n') && (parity != 'N'))
		options.c_iflag |= INPCK;
	tcflush(fd, TCIFLUSH);
	options.c_cc[VTIME] = 150;
	options.c_cc[VMIN]  = 0; /* Update the options and do it NOW */

	if (tcsetattr(fd, TCSANOW, &options) != 0)
		return FALSE;
	printf("set_parity ok \n");
	return TRUE;
}


int Serial_ChangeParam(NETCOM_COM_PARAM_STATE *pCOMParam)
{
	int ret;

	printf("change %s Param\n",pCOMParam->COMDevName);
	if(pCOMParam->COMFd <= 0){ //com is invalid
		printf("Device %s is invalid \n",pCOMParam->COMDevName);
		return -1;
	}

	//if((time(NULL) -pCOMParam->COMLastCfgTime) > SERIAL_CFG_INTERVAL_IN_SECOND){
	if(1){
		pthread_mutex_lock(&(pCOMParam->COMParamLock));
		set_baudrate(pCOMParam->COMFd,pCOMParam->Baud);

		ret = set_Parity(pCOMParam->COMFd,
		pCOMParam->DataBit,
		pCOMParam->StopBit,
		pCOMParam->Parity);
		pCOMParam->COMLastCfgTime = time(NULL);
		pthread_mutex_unlock(&(pCOMParam->COMParamLock));
	}
	else{
		printf("Device %s config is too high,so this time is not config, interval is %d second \n",
			pCOMParam->COMDevName,
			SERIAL_CFG_INTERVAL_IN_SECOND);
		return -1;
	}
	return ret;
}

int main_SerialInit(NETCOM_COM_PARAM_STATE *pCOMParam)
{
	int ComNum; /* 1=COM1,... */
	int ServerPort;
	int *pCOMFD;
	int i, ret;
	int port_num, baudrate;
	int DataBit, StopBit, Parity;
	int ReadNum;
	char dev_name[128];
	unsigned char ComReadBuffer[256];

	sprintf(dev_name, "%s", pCOMParam->COMDevName);
	printf("%s %d dev_name = %s\n", __func__, __LINE__, dev_name);
	printf("%s->COMDevName %s\n", __FUNCTION__, pCOMParam->COMDevName);

	baudrate = pCOMParam->Baud;
	printf("%s->baudrate =  %d\n", __FUNCTION__, pCOMParam->Baud);

	pCOMParam->COMFd = open(dev_name, O_RDWR);//|O_NDELAY);
	if (-1 == pCOMParam->COMFd)
	{
		perror("Can't Open Serial Port\r\n");
		sprintf(pCOMParam->COMStatus, "打开失败");
		VirtCOM_DevState.LEDStatus_Alarm = 1;
		return -1;
	}
	sprintf(pCOMParam->COMStatus, "打开成功");

	ret = pthread_mutex_init(&(pCOMParam->COMParamLock), NULL);
	if (-1 == ret)
	{
		perror("Can't Open Serial Port\r\n");
		printf("Init Lock Failed for %s\r\n", pCOMParam->COMDevName);
		return -1;
	}

	printf("%s, FD = %d\n", pCOMParam->COMDevName, pCOMParam->COMFd);
	Serial_ChangeParam(pCOMParam);

#if 0
	i=0;
	while(1){
		printf("send again.%d\n",i);
		Serial_Send(fd,dev_name,strlen(dev_name));
		i++;
		sleep(1);
	}
#endif

	return TRUE;
}

int Serial_Send(NETCOM_COM_PARAM_STATE *pCOMParam, unsigned char *pData, int Len)
{
	int nwrite;

	if(pCOMParam->COMFd <= 0) return -1;

		nwrite = write(pCOMParam->COMFd, pData, Len);
		if(nwrite <= 0)
			printf("write failed. return\n");
		//tcflush(pCOMParam->COMFd,TCIFLUSH);;
		//printf("%s-> send OK.\n",pCOMParam->COMDevName);
		pCOMParam->SerialSendByte += nwrite;
		//printf("%s->Send %d bytes\n",pCOMParam->COMDevName,pSerialSendBuf[SendIndex].ValidLen);
		//VirtCOM_PrintBuf(pSerialSendBuf[SendIndex].Buffer,pSerialSendBuf[SendIndex].ValidLen);

	return 0;
}
