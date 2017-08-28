#define NONE 0
#define EVEN 1
#define ODD 2
#define TCP 1
#define UDP 2
#define RS485 0
#define RS232 1
#define RS422 2
#define GET_CONFIG	0x01
#define SET_CONFIG	0x02
#define SEND_CONFIG	0x03
#define	GET_STATUS	0x04
#define	SET_DEFAULTCONFIG	0x05



typedef struct  {
		char_t portindex[3];		/*����*/
		char_t rate[7];		/*������*/
		char_t databits[2];	/*����λ*/
		char_t parity[2]; 	/*��żУ��*/
		char_t stopbit[3];	/*ֹͣλ*/
		
		char_t net_port[6];		/*�˿ں�*/
		char_t socket_type[4];	/*����Э������*/
		char_t ClientMode[7]; 	/*����ģʽ*/
		char_t ServerIP[16]; 		/* ������IP��ַ*/
		char_t Interface[6]; 		/* ����Э��*/
} PORT_ATTRIBUTE; 

typedef struct {
	int rate;		/*115200,...*/
	int databits;		/*8,7,6,5*/
	int parity;		/* 0 = NONE,1= Even,2 = Odd */
	int stopbit;		/*1,15, 2*/
	
	int net_port;		/*4000,4001,...*/
	int socket_type;	/*1=tcp;2=udp*/
	int ClientMode; /* 0= work in server mode., 1= work in client mode */
	int ServerIP; /* remote server ip when work in client mode */
	int Interface; /* 0=RS485, 1= RS232, 2= RS422*/
}DEV_COMCONFIG;

typedef struct {
	int IPAddr;
	int NetMask;
	int MACAddr1;
	int MACAddr2;
	int Gateway;
	int DNS;
	int SecondDNS;	
}DEV_NETCONFIG;

typedef struct {
	char_t *first;
	char_t *second;
}STIfSelect;
typedef struct {
	char_t *first;
	char_t *second;
	char_t *third;
	char_t *forth;
	char_t *fifth;
	char_t *sixth;
	char_t *seventh;
	char_t *eightth;
	char_t *nineth;
	char_t *tenth;
	char_t *eleventh;

}STBaudSelect;
typedef struct {
	char_t *first;
	char_t *second;
	char_t *third;
	char_t *forth;
}STDataSelect;
typedef struct {
	char_t *first;
	char_t *second;
	char_t *third;
	char_t *forth;
}STParitySelect;

typedef struct {
	char_t *first;
	char_t *second;
	char_t *third;
}STStopSelect;


typedef struct {
unsigned char SoftVer[64];
int ValidSerial; 
int ValidNet;
DEV_COMCONFIG Serial[64];
DEV_NETCONFIG Net[64];
unsigned char CRC;
int ConfigErr;
unsigned char ErrStatus[512];
}WEB_CONFIG;

typedef struct {
unsigned char COMStatus[16];
unsigned int ClientSendByte;
unsigned int ClientRecvByte;
int SerialRecvByte;
int SerialSendByte;
}VCOM_STATUS;


WEB_CONFIG webconfig;	/*webconfigѡ��*/
VCOM_STATUS vcomStatus[64];
PORT_ATTRIBUTE portAttr[64];/*��������*/
char dataBuf[10000];/*1�ֽڵ����2761�ֽڵ�WEB_CONFIG���ݣ�����1λ  |cmd| |config data....| */
	/*���ǻ�ȡ״̬��1�ֽڵ�����,����1λ��2777�ֽڵ�״̬���� |cmd| |status data....| */
