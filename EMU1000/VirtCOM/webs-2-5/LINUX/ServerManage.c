#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <netdb.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/reboot.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <signal.h>
#include <dirent.h>
#include <sys/mman.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/watchdog.h>
#ifdef SOLARIS
#include <sys/sockio.h>
#endif

#include "VirtCOM.h"

#define WDT "/dev/watchdog"
#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0

#define	BACKLOG	10
extern VIRT_COM_DEVSTATE VirtCOM_DevState;
static unsigned int  BaudTable[16];


int g_iAppRun = 1;

static void init_daemon(void)
{
	int   i;
	pid_t child1, child2;

	/*捕捉的信号*/
	/*创建第一子进程*/
	child1 = fork();

	if( child1 < 0 )
	{
		perror("Create first child process fail!\n");
		exit(1);
	}
	else if(child1 > 0) exit(0); /*结束父进程*/
	/*第一子进程成为新的会话组长和进程组长*/
	setsid();
	/*第一子进程与控制终端分离*/
	child2 = fork();
	if( child2 < 0 )
	{
		perror("Create second child process fail!\n");
		exit(2);
	}
	else if(child2 > 0) exit(0); /*结束第一子进程*/
	/*第二子进程继续, 第二子进程不再是会话组长*/
	/*关闭打开的文件描述符*/
	for(i = 0; i < NOFILE; ++i)
		close(i);
	/*改变工作目录到/tmp*/
	chdir("/tmp");
	/*重设文件创建掩模*/
	umask(0);
	/*处理SIGCHLD信号*/
	signal(SIGCHLD, SIG_IGN);
}

static int is_proc_exist( char *name )
{
	FILE *pstr; char cmd[128],buff[512],*p;
	pid_t pID;
	int pidnum;
	int ret=3;
	memset(cmd,0,sizeof(cmd));

	sprintf(cmd, "ps -ef|grep %s ",name);
	pstr=popen(cmd, "r");//

	if(pstr==NULL)
	{
		return -1;
	}
	memset(buff,0,sizeof(buff));
	fgets(buff,512,pstr);
	printf ( "buff=%s\n", buff );
	if( NULL != strstr( buff, cmd ) )
	{
		return -1;
	}
	p=strtok(buff, " ");
	/* p=strtok(NULL, " "); //这句是否去掉，取决于当前系统中ps后，进程ID号是否是第一个字段 pclose(pstr); */
	if(p==NULL)
	{
		return -1;
	}
	if(strlen(p)==0)
	{
		return -1;
	}
	if((pidnum=atoi(p))==0)
	{
		return -1;
	}
	printf("pidnum: %d\n",pidnum);
	pID=(pid_t)pidnum;
	ret=kill(pID,0);//这里不是要杀死进程，而是验证一下进程是否真的存在，返回0表示真的存在
	printf("ret= %d \n",ret);
	if(0==ret)
	{
		printf("process: %s exist!\n",name);
	}
	else
	{
		printf("process: %s not exist!\n",name);
		return -1;
	}

	return 0;
}
//是不是数字
static int IsNumeric(const char* ccharptr_CharacterList)
{
	for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
		if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
			return 0; // false
	return 1; // true
}

//intCaseSensitive=0大小写不敏感
static int strcmp_Wrapper(const char *s1, const char *s2, int intCaseSensitive)
{
	if (intCaseSensitive)
		return !strcmp(s1, s2);
	else
		return !strcasecmp(s1, s2);
}

//intCaseSensitive=0大小写不敏感
static int strstr_Wrapper(const char* haystack, const char* needle, int intCaseSensitive)
{
	if (intCaseSensitive)
		return (int) strstr(haystack, needle);
	else
		return (int) strcasestr(haystack, needle);
}

static pid_t GetPIDbyName_implements(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
{
	char chrarry_CommandLinePath[100]  ;
	char chrarry_NameOfProcess[300]  ;
	char* chrptr_StringToCompare = NULL ;
	pid_t pid_ProcessIdentifier = (pid_t) -1 ;
	struct dirent* de_DirEntity = NULL ;
	DIR* dir_proc = NULL ;

	int (*CompareFunction) (const char*, const char*, int) ;

	if (intExactMatch)
		CompareFunction = &strcmp_Wrapper;
	else
		CompareFunction = &strstr_Wrapper;


	dir_proc = opendir(PROC_DIRECTORY) ;
	if (dir_proc == NULL)
	{
		perror("Couldn't open the " PROC_DIRECTORY " directory") ;
		return (pid_t) -2 ;
	}

	// Loop while not NULL
	while ( (de_DirEntity = readdir(dir_proc)) )
	{
		if (de_DirEntity->d_type == DT_DIR)
		{
			if (IsNumeric(de_DirEntity->d_name))
			{
				strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
				strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
				strcat(chrarry_CommandLinePath, "/cmdline") ;
				FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  //open the file for reading text
				if (fd_CmdLineFile)
				{
					fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; //read from /proc/<NR>/cmdline
					fclose(fd_CmdLineFile);  //close the file prior to exiting the routine

					if (strrchr(chrarry_NameOfProcess, '/'))
						chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ;
					else
						chrptr_StringToCompare = chrarry_NameOfProcess ;

					/* printf("Process name: %s\n", chrarry_NameOfProcess); */
					//这个是全路径，比如/bin/ls
					/* printf("Pure Process name: %s\n", chrptr_StringToCompare ); */
					//这个是纯进程名，比如ls

					//这里可以比较全路径名，设置为chrarry_NameOfProcess即可
					//	printf("chrptr_StringToCompare = %s cchrptr_ProcessName = %s\n", chrptr_StringToCompare, cchrptr_ProcessName);
					if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, intCaseSensitiveness) )
					{
						pid_ProcessIdentifier = (pid_t) atoi(de_DirEntity->d_name) ;
						closedir(dir_proc) ;
						return pid_ProcessIdentifier ;
					}
				}
			}
		}
	}
	closedir(dir_proc) ;
	return pid_ProcessIdentifier ;
}

//简单实现
static pid_t GetPIDbyName_Wrapper(const char* cchrptr_ProcessName)
{
	return GetPIDbyName_implements(cchrptr_ProcessName, 0, 0);//大小写不敏感
}

// int main()
// {
// pid_t pid = GetPIDbyName_Wrapper("em761-b") ; // If -1 = not found, if -2 = proc fs access error
// printf("PID %d\n", pid);
// return EXIT_SUCCESS ;
// }

static void SignHandler(int signum, siginfo_t *pInfo, void *pReserved)
{
	g_iAppRun = 0;
}

typedef void (*LPSIGPROC)(int, siginfo_t*, void*);

static int SignalHook(int iSigNo, LPSIGPROC func)
{
	struct sigaction act, oact;

	act.sa_sigaction = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO|SA_RESTART;

	if(sigaction(iSigNo, &act, &oact) < 0)
		return -1;
	return 0;
}

int  Manage_ParseRemoteConfigSerial( unsigned char *pSrcChar,
		NETCOM_COM_PARAM_STATE *pNewCOMParam,unsigned int *pPort);


void NetManage_PrintFrame(void *pParam)
{

	MUSTER_TELE *pManageFrame;
	int ClientIP,ClientIP3,ClientIP2,ClientIP1,ClientIP0;


	pManageFrame = (MUSTER_TELE *)pParam;

	printf("========== Net Manage Frame Print ================\n");
	printf("xid = %d\n",pManageFrame->xid);
	printf("msg_type = %d\n",pManageFrame->msg_type);
	printf("muster_ver = %d\n",pManageFrame->muster_ver);
	pManageFrame->net_hostname[11] = 0;
	printf("net_hostname = %s\n",pManageFrame->net_hostname);

	printf("net_mac = %02x:%02x:%02x:%02x:%02x:%02x\n",
			pManageFrame->net_mac[0],
			pManageFrame->net_mac[1],
			pManageFrame->net_mac[2],
			pManageFrame->net_mac[3],
			pManageFrame->net_mac[4],
			pManageFrame->net_mac[5]
		  );

	ClientIP = pManageFrame->net_ip_addr;
	ClientIP3 = (ClientIP>>24)&0xff;
	ClientIP2 = (ClientIP>>16)&0xff;
	ClientIP1 = (ClientIP>>8)&0xff;
	ClientIP0 = (ClientIP)&0xff;
	printf("net_ip_addr = %d.%d.%d.%d\n",
			ClientIP3,ClientIP2,ClientIP1,ClientIP0	);

	ClientIP = pManageFrame->net_ip_mask;
	ClientIP3 = (ClientIP>>24)&0xff;
	ClientIP2 = (ClientIP>>16)&0xff;
	ClientIP1 = (ClientIP>>8)&0xff;
	ClientIP0 = (ClientIP)&0xff;
	printf("net_ip_mask = %d.%d.%d.%d\n",
			ClientIP3,ClientIP2,ClientIP1,ClientIP0	);

	ClientIP = pManageFrame->net_gateway;
	ClientIP3 = (ClientIP>>24)&0xff;
	ClientIP2 = (ClientIP>>16)&0xff;
	ClientIP1 = (ClientIP>>8)&0xff;
	ClientIP0 = (ClientIP)&0xff;
	printf("net_gateway = %d.%d.%d.%d\n",
			ClientIP3,ClientIP2,ClientIP1,ClientIP0	);


	printf("devmodel = %d\n",pManageFrame->devmodel);
	printf("workmodel = %d\n",pManageFrame->workmodel);
	printf("portnum = %d\n",pManageFrame->portnum);
	printf("firstport = %d\n",pManageFrame->firstport);
	pManageFrame->firmware[23] = 0;
	printf("firmware = %s\n",pManageFrame->firmware);


	pManageFrame->cfgpwd[23] = 0;
	printf("cfgpwd = %s\n",pManageFrame->cfgpwd);


	return ;
}

int main_NetManage_send(char *buf)
{
	/*
	   ip_hdr *iphdr;
	   udp_hdr *udphdr;
	   psd_header psd;

	   iphdr = (ip_hdr*)buf;
	   udphdr = (udp_hdr*)(buf+20);

	   iphdr->ip_length = 5;
	   iphdr->ip_version= 4;
	   iphdr->ip_tos = 0;
	   iphdr->ip_total_length = htons(sizeof(buf));
	   iphdr->ip_id = 0;
	   iphdr->ip_flags = htons(0x4000);
	   iphdr->ip_ttl = 0x40;
	   iphdr->ip_protocol = 0x11;
	   iphdr->ip_cksum = 0;
	   iphdr->ip_source = inet_addr("192.168.1.211");
	   iphdr->ip_dest = inet_addr(BROAD_IP);
	   iphdr->ip_cksum = checksum((unsigned short*)buf, 20);

	   udphdr->s_port = htons(PORT);
	   udphdr->d_port = htons(PORT);
	   udphdr->length = htons(sizeof(buf)-20);
	   udphdr->cksum = 0;

	   psd.s_ip = iphdr->ip_source;
	   psd.d_ip = iphdr->ip_dest;
	   psd.mbz = 0;
	   psd.proto = 0x11;
	   psd.plen = (udphdr->length);
	   char tmp[sizeof(psd)+ntohs(udphdr->length)];
	   memcpy(tmp, &psd, sizeof(psd));
	   memcpy(buf+20+sizeof(udphdr)+4,macaddr,ETH_ALEN);
	   memcpy(buf+20+sizeof(udphdr)+4+ETH_ALEN,&(inet_pton("192.168.1.211")),4);
	   memcpy(tmp+sizeof(psd), buf+20, sizeof(buf)-20);
	   udphdr->cksum = checksum((unsigned short*)tmp, sizeof(tmp));
	   return 0;
	   */
}


int	main_NetManage(void)
{
	int	sin_size,yes=1,new_fd;
	struct	sockaddr_in	server_addr;
	struct	sockaddr_in	ClientAddr;
	struct	sigaction	sa;
	NETCOM_TABLE_ITEM * pNewItem;
	NETCOM_COM_PARAM_STATE *pCOMParam;
	int ret,i;
	unsigned char NetRecvBuffer[2048];
	int RecSize;
	int NextIndex;

	printf("NEWw : %s->Start to listen Port %d\n",__FUNCTION__,VirtCOM_DevState.NetManagePort);

	//return 0;
	if((VirtCOM_DevState.NetManageFD =socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		perror("main_NetManage  socket");
		return -1;
	}
	if(setsockopt(VirtCOM_DevState.NetManageFD,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
	{
		perror("main_NetManage  setsocopt");
		return -1;
	}

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(VirtCOM_DevState.NetManagePort);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	memset(&(server_addr.sin_zero),'\0',8);

	if(bind(VirtCOM_DevState.NetManageFD,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))==-1)
	{
		perror("main_NetManage  bind");
		return -1;
	}	
	if(listen(VirtCOM_DevState.NetManageFD,BACKLOG)==-1)
	{
		perror("main_NetManage  listen");
		return -1;
	}

	while(1){
		NextIndex = 0xff;
		do{
			for(i=0;i<MAX_CONN_PER_REMOTEMANAGE_PORT;i++){
				//printf("index %d, ClientPort:%d\n",i,   pCOMParam->ClientPort[i]);
				if( 0 == VirtCOM_DevState.NetManageClientFD[i]){ // find a valid index
					NextIndex = i;
					break;
				}
			}
			if(0xff == NextIndex) sleep(1);	
			else printf("Find Next Index = %d\n",NextIndex);
		}while(NextIndex == 0xff);


		sin_size=sizeof(struct sockaddr_in);
		if((VirtCOM_DevState.NetManageClientFD[i]=accept(VirtCOM_DevState.NetManageFD,(struct sockaddr *)&ClientAddr,&sin_size))==-1)
		{
			perror("main_NetManage  accept");
			continue;
		}

		printf("main_NetManage  accept OK, Server Get TCP Connection From %s, Port:%d\n",
				inet_ntoa(ClientAddr.sin_addr),
				ntohs(ClientAddr.sin_port)
			  );
		VirtCOM_DevState.NetManageClientIP[NextIndex] = ClientAddr.sin_addr.s_addr;
		VirtCOM_DevState.NetManageClientPort[NextIndex] = ClientAddr.sin_port;
		ret = pthread_create(&VirtCOM_DevState.ThreadID_NetManageClient[NextIndex],NULL,(void *)Manage_NewClient_TCP,&NextIndex);
		if(ret!=0)
		{
			printf ("Create pthread Net New Client  Index %d error!\n",NextIndex);
			return ;
		}
		usleep(100000);

	}

	return 0;
}



void Manage_NewClient_TCP(unsigned int  *pIndex)
{
	unsigned char NetRecvBuffer[2048];
	unsigned int Index;
	int RecSize;
	int i;
	int fd;
	int ret;
	unsigned int DoPort;
	NETCOM_COM_PARAM_STATE *pCOMParam;
	NETCOM_COM_PARAM_STATE NewCOMParam;

	Index = *pIndex;

	fd = VirtCOM_DevState.NetManageClientFD[Index];

	while(1){
		if((RecSize=recv(fd,NetRecvBuffer,sizeof(NetRecvBuffer)-1,0))<=0)
		{
			perror("net manage  close socket");
			close(fd);
			VirtCOM_DevState.NetManageClientFD[Index] = 0;
			return ;
		}
#if 1
		printf("net manage %d rec %d:\n",Index,RecSize);

		VirtCOM_PrintBufInASCII(NetRecvBuffer,RecSize);
		//VirtCOM_PrintBuf(NetRecvBuffer,RecSize);
		printf("\n");
#endif

		ret = Manage_ParseRemoteConfigSerial(NetRecvBuffer,
				&NewCOMParam,&DoPort);
		if(ret) continue;

		for(i=0;i<MAX_SERIAL_PORTS_NUM;i++){
			/*
			   printf("%s->com %d, name:%s,ServerPort:%d,valid: %d\n",__FUNCTION__,i,
			   VirtCOM_DevState.COMState[i].COMDevName,
			   VirtCOM_DevState.COMState[i].ServerPort,
			   VirtCOM_DevState.COMState[i].COMValid);
			   */
			if( 0 == VirtCOM_DevState.COMState[i].COMValid) continue;
			if( DoPort == VirtCOM_DevState.COMState[i].ServerPort){ // find a valid index
				pCOMParam = &VirtCOM_DevState.COMState[i];
				break;
			}
		}
		if(i >= MAX_SERIAL_PORTS_NUM) {
			printf("%s->not find serial for port %d\n",__FUNCTION__,DoPort);
			continue;
		}
		pCOMParam->Baud = NewCOMParam.Baud;
		pCOMParam->DataBit = NewCOMParam.DataBit;
		pCOMParam->StopBit = NewCOMParam.StopBit;
		pCOMParam->Parity= NewCOMParam.Parity;

		VirtCOM_DevState.NetManageLastPort[Index] = DoPort;

		ret = Serial_ChangeParam(pCOMParam);
		if(ret) {
			printf("%s->set serial param failed for port %d,serial dev:%s\n",__FUNCTION__,DoPort,pCOMParam->COMDevName);
			continue;
		}
	}
}
/*
   find next ";"
   slim the space head or rail
   save it in pDstChar
   return the new dst point in pChar
   end at 0x0a or 0x0d
   */
unsigned char *  Manage_FindDotAndSlim(unsigned char *pSrcChar,unsigned char *pDstChar)
{
	unsigned char * pChar;
	int RetFlag;
	int Flag_FindHead;

	pChar = pSrcChar;
	RetFlag = 0;
	Flag_FindHead = 0;
	do{
		if((pChar[0] == 0x0a) ||(pChar[0] == 0x0d)||(pChar[0] == 0x0)){
			RetFlag = 1;
			break;
		}
		if((pChar[0] == ';')){
			pChar ++;
			RetFlag = 1;
			break;
		}
		if((pChar[0] == ' ') && (0 == Flag_FindHead)){ //do not find the first valid ASCII
			pChar ++;
			continue;
		}

		if((pChar[0] == ' ') && (1 == Flag_FindHead)){//have find the first valid ASCII,then end scan
			pChar ++;
			RetFlag = 1;
			break;
		}
		pDstChar[0] = pChar[0];
		Flag_FindHead = 1;
		pDstChar ++;
		pChar ++;

		pDstChar[0] = 0; //pre add a 0x00 end pad

	}while(RetFlag == 0 );

	return pChar;

}
int  Manage_ParseRemoteConfigSerial( unsigned char *pSrcChar,
		NETCOM_COM_PARAM_STATE *pNewCOMParam,unsigned int *pPort)
{
	REMOTE_SET_SERIAL_PARAM MyTag,*pTag;
	unsigned char *pChar;
	unsigned char ProcBuf[1024];

	pChar = pSrcChar;
	pTag = &MyTag;

	pChar = Manage_FindDotAndSlim(pChar,ProcBuf);
	if((pChar[0] == 0x0a) ||(pChar[0] == 0x0d)||(pChar[0] == 0x0)){	return -1;}
	pTag->UserTag = atoi(ProcBuf);
	//printf("Set UserTag = %d\n",pTag->UserTag); 

	pChar = Manage_FindDotAndSlim(pChar,ProcBuf);
	if((pChar[0] == 0x0a) ||(pChar[0] == 0x0d)||(pChar[0] == 0x0)){	return -1;}

	pTag->Cmd = atoi(ProcBuf);
	//printf("Set CMD = %d\n",pTag->Cmd); 
	if(pTag->Cmd != 10){
		printf("Wrong cmd:%d\n",pTag->Cmd); 
		return -1;
	}

	pChar = Manage_FindDotAndSlim(pChar,ProcBuf);
	if((pChar[0] == 0x0a) ||(pChar[0] == 0x0d)||(pChar[0] == 0x0)){	return -1;}
	pTag->NetPort = atoi(ProcBuf);
	//*pPort = SwapChar32(pTag->NetPort);
	*pPort = (pTag->NetPort);
	//printf("Set NetPort = %d\n",pTag->NetPort); 

	pChar = Manage_FindDotAndSlim(pChar,ProcBuf);
	if((pChar[0] == 0x0a) ||(pChar[0] == 0x0d)||(pChar[0] == 0x0)){	return -1;}
	pTag->Baudrate = atoi(ProcBuf);
	//printf("Set Baudrate = %d\n",pTag->Baudrate); 
	switch(pTag->Baudrate){
		case 0: pNewCOMParam->Baud = 9600;break;
		case 1: pNewCOMParam->Baud = 4800;break;
		case 2: pNewCOMParam->Baud = 19200;break;
		case 3: pNewCOMParam->Baud = 38400;break;
		case 4: pNewCOMParam->Baud = 57600;break;
		case 5: pNewCOMParam->Baud = 115200;break;
		case 6: pNewCOMParam->Baud = 300;break;
		case 7: pNewCOMParam->Baud = 1200;break;
		case 8: pNewCOMParam->Baud = 2400;break;
		default: {printf("Wrong Value:%d\n",pTag->Baudrate); return -1;}
	}

	pChar = Manage_FindDotAndSlim(pChar,ProcBuf);
	if((pChar[0] == 0x0a) ||(pChar[0] == 0x0d)||(pChar[0] == 0x0)){	return -1;}
	pTag->Pariority = atoi(ProcBuf);
	//printf("Set Pariority = %d\n",pTag->Pariority); 
	switch(pTag->Pariority){
		case 0: pNewCOMParam->Parity = 'N';break;
		case 1: pNewCOMParam->Parity = 'O';break;
		case 2: pNewCOMParam->Parity = 'E';break;
		case 3: pNewCOMParam->Parity = 'S';break; //space
		case 4: pNewCOMParam->Parity = 'M';break; //mark
		default: {printf("Wrong Value:%d\n",pTag->Pariority); return -1;}
	}

	pChar = Manage_FindDotAndSlim(pChar,ProcBuf);
	if((pChar[0] == 0x0a) ||(pChar[0] == 0x0d)||(pChar[0] == 0x0)){	return -1;}
	pTag->DataBit = atoi(ProcBuf);
	//printf("Set DataBit = %d\n",pTag->DataBit); 
	switch(pTag->DataBit){
		case 0: pNewCOMParam->DataBit = 8;break;
		case 1: pNewCOMParam->DataBit = 7;break;
		case 2: pNewCOMParam->DataBit = 6;break;
		case 3: pNewCOMParam->DataBit = 5;break;
		default:{ printf("Wrong Value:%d\n",pTag->DataBit);  return -1;}
	}

	pChar = Manage_FindDotAndSlim(pChar,ProcBuf);
	pTag->StopBit = atoi(ProcBuf);
	//printf("Set StopBit = %d\n",pTag->StopBit); 
	switch(pTag->StopBit){
		case 0: pNewCOMParam->StopBit = 1;break;
		case 1: pNewCOMParam->StopBit = 2;break;
		default: {printf("Wrong Value:%d\n",pTag->StopBit);return -1;}
	}	

	return 0;

}


void Manage_PrintRemoteConfigSerial( REMOTE_SET_SERIAL_PARAM *pTag)
{

	printf("Tag:0x%04x(%d)\n",SwapChar32(pTag->UserTag),SwapChar32(pTag->UserTag));
	printf("Cmd:0x%04x(%d)\n",pTag->Cmd,pTag->Cmd);
	printf("NetPort:0x%04x(%d)\n",SwapChar32(pTag->NetPort),SwapChar32(pTag->NetPort));
	printf("Baudrate:");
	switch(pTag->Baudrate){
		case 0: printf("9600\n");break;
		case 1: printf("4800\n");break;
		case 2: printf("19200\n");break;
		case 3: printf("38400\n");break;
		case 4: printf("57600\n");break;
		case 5: printf("115200\n");break;
		case 6: printf("300\n");break;
		case 7: printf("1200\n");break;
		case 8: printf("2400\n");break;
		default: printf("Wrong Value:%d\n",pTag->Baudrate);break;
	}

	printf("Pariority:");
	switch(pTag->Pariority){
		case 0: printf("None\n");break;
		case 1: printf("Odd(Ji)\n");break;
		case 2: printf("Even(Ou)\n");break;
		case 3: printf("Space\n");break;
		case 4: printf("Mark\n");break;
		default: printf("Wrong Value:%d\n",pTag->Pariority);break;
	}

	printf("DataBit:");
	switch(pTag->DataBit){
		case 0: printf("8Bit\n");break;
		case 1: printf("7Bit\n");break;
		case 2: printf("6Bit\n");break;
		case 3: printf("5Bit\n");break;
		default: printf("Wrong Value:%d\n",pTag->DataBit);break;
	}

	printf("StopBit:");
	switch(pTag->StopBit){
		case 0: printf("0-1Bit\n");break;
		case 1: printf("2-2Bit\n");break;
		default: printf("Wrong Value:%d\n",pTag->StopBit);break;
	}

	printf("Pad:0x%04x(%d)\n",pTag->Pad,pTag->Pad);

	return ;

}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
/*
   receive and process zhaohuan packet
   */
int	main_ZhaoHuan(void)
{
	int	sin_size,yes=1,new_fd;
	struct	sockaddr_in	server_addr;
	struct	sockaddr_in	ClientAddr;
	struct	sigaction	sa;
	int ret;
	int addr_len;
	unsigned char NetRecvBuffer[2048];
	int RecSize;
	VIRT_COM_DEVSTATE *pCOMParam = &VirtCOM_DevState;


	return 0;
	memset(BaudTable,0,sizeof(BaudTable));
	BaudTable[0] = B9600;
	BaudTable[1] = B4800;
	BaudTable[2] = B19200;
	BaudTable[3] = B38400;
	BaudTable[4] = B57600;
	BaudTable[5] = B115200;
	BaudTable[6] = B300;
	BaudTable[7] = B1200;
	BaudTable[8] = B2400;


	printf("%s->Start to listen Port %d\n",__FUNCTION__,VirtCOM_DevState.NetZhaoHuanPort);
	printf("%s->sizeof  MUSTER_TELE = %d\n",__FUNCTION__,sizeof(MUSTER_TELE));
	//printf("%s->sizeof  MUSTER_TELE_TEST = %d\n",__FUNCTION__,sizeof(MUSTER_TELE_TEST));
	addr_len = sizeof(struct	sockaddr_in);
	if((VirtCOM_DevState.NetMZhaoHuanFD=socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		perror("zhaohuan socket");
		return -1;
	}
	yes = 1;
	if(setsockopt(VirtCOM_DevState.NetMZhaoHuanFD,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
	{
		perror(" zhaohuan setsocopt");
		return -1;
	}

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(VirtCOM_DevState.NetZhaoHuanPort);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	memset(&(server_addr.sin_zero),'\0',8);

	if(bind(VirtCOM_DevState.NetMZhaoHuanFD,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))==-1)
	{
		perror(" zhaohuan  bind");
		return -1;
	}	

	while(1){
		if((RecSize=recvfrom(VirtCOM_DevState.NetMZhaoHuanFD,NetRecvBuffer,sizeof(NetRecvBuffer)-1,0,
						(struct sockaddr*)&ClientAddr,&addr_len))<=0)
		{
			perror("recv");
			close(VirtCOM_DevState.NetManageFD);
			return ;
		}
		printf(" zhaohuan  recv OK, Server Get  zhaohuan  Connection From %s, Port:%d\n",
				inet_ntoa(ClientAddr.sin_addr),
				ntohs(ClientAddr.sin_port)
			  );

		printf("recv_size = %d\n", RecSize);
		VirtCOM_PrintBuf(NetRecvBuffer,RecSize);

		if(RecSize <sizeof(MUSTER_TELE)) continue;

		RecSize = ZhaoHuan_Process(NetRecvBuffer);

		//RecSize = sendto(pCOMParam->NetManageFD,NetRecvBuffer,strlen(NetRecvBuffer)+1,0,
		//	(struct sockaddr*)&ClientAddr,sizeof(ClientAddr));
		//printf("send  , send_size is : %d\n", RecSize);
		//VirtCOM_PrintBuf(NetRecvBuffer,RecSize);
		sendto(pCOMParam->NetMZhaoHuanFD,NetRecvBuffer,RecSize,0,
				(struct sockaddr*)&ClientAddr,sizeof(ClientAddr));
	}
	return 0;
}
int ZhaoHuan_GetIPMAC(unsigned int *pIP,unsigned char *pMAC)
{
	register int fd, intrface, retn = 0; 
	struct ifreq buf; 
	struct arpreq arp; 
	struct ifconf ifc; 

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
	{ 
		ifc.ifc_len = sizeof buf; 
		ifc.ifc_buf = (caddr_t) &buf; 
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 
		{ 
			//获取接口信息
			intrface = ifc.ifc_len / sizeof (struct ifreq); 
			printf("interface num is intrface=%d\n\n\n",intrface); 
			//根据借口信息循环获取设备IP和MAC地址
			while (intrface-- > 0) 
			{ 
				//获取设备名称
				printf ("net device %s\n", buf.ifr_name); 

				//获取当前网卡的IP地址 
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf))) 
				{ 
					puts ("IP address is:"); 
					puts(inet_ntoa(((struct sockaddr_in*)(&buf.ifr_addr))->sin_addr)); 
					puts(""); 
					*pIP = ((struct sockaddr_in*)(&buf.ifr_addr))->sin_addr.s_addr;

					//puts (buf[intrface].ifr_addr.sa_data); 
				} 
				else 
				{ 
					char str[256]; 
					sprintf (str, "ZhaoHuan_GetIPMAC: ioctl ip device %s", buf.ifr_name); 
					perror (str); 
				} 
				/* this section can't get Hardware Address,I don't know whether the reason is module driver*/ 

				if (!(ioctl (fd, SIOCGIFHWADDR, (char *) &buf))) 
				{ 
					puts ("HW address is:"); 
					printf("%02x:%02x:%02x:%02x:%02x:%02x\n", 
							(unsigned char)buf.ifr_hwaddr.sa_data[0], 
							(unsigned char)buf.ifr_hwaddr.sa_data[1], 
							(unsigned char)buf.ifr_hwaddr.sa_data[2], 
							(unsigned char)buf.ifr_hwaddr.sa_data[3], 
							(unsigned char)buf.ifr_hwaddr.sa_data[4], 
							(unsigned char)buf.ifr_hwaddr.sa_data[5]); 
					puts(""); 
					puts(""); 
					pMAC[0] = 	 (unsigned char)buf.ifr_hwaddr.sa_data[0];
					pMAC[1] = 	 (unsigned char)buf.ifr_hwaddr.sa_data[1];
					pMAC[2] = 	 (unsigned char)buf.ifr_hwaddr.sa_data[2];
					pMAC[3] = 	 (unsigned char)buf.ifr_hwaddr.sa_data[3];
					pMAC[4] = 	 (unsigned char)buf.ifr_hwaddr.sa_data[4];
					pMAC[5] = 	 (unsigned char)buf.ifr_hwaddr.sa_data[5];
				} 
				else 
				{ 
					char str[256]; 
					sprintf (str, "ZhaoHuan_GetIPMAC: ioctl mac device %s", buf.ifr_name); 
					perror (str); 
				} 
			} //while
		} else 
			perror ("ZhaoHuan_GetIPMAC: ioctl"); 
	} else 
		perror ("ZhaoHuan_GetIPMAC: socket"); 
	close (fd); 
	return retn; 
} 

int ZhaoHuan_GeneTestPacket(MUSTER_TELE *pZhaoHuan)
{
	MUSTER_TELE  recbuf;

	memset(pZhaoHuan,0,sizeof(MUSTER_TELE));

	memset(&recbuf,0,sizeof(MUSTER_TELE));
	recbuf.msg_type=1;
	recbuf.muster_ver = 1;
	sprintf(recbuf.net_hostname ,"ServerID");
	sprintf(recbuf.net_mac ,"010203040526");
	recbuf.net_ip_addr = 1690347712; //设备IP
	recbuf.net_ip_mask =16777215 ;//设备子网掩码
	recbuf.net_gateway = 0;
	recbuf.workmodel=0 ;//工作模式，1-Server 模式,2-Client 模式
	recbuf.portnum=3 ;//设备硬件端口数
	recbuf.mapip[0]=1690347712 ;//设备第1 个监听IP
	recbuf.mapport[0]=6020; //设备第1 个监听端口，对应第1 个硬件端口。
	recbuf.mapip[1]=1690347712;
	recbuf.mapport[1]=6021;
	recbuf.mapip[2]=1690347712;
	recbuf.mapport[2]=6030;
	memcpy(pZhaoHuan,&recbuf,sizeof(MUSTER_TELE));
	return 0;

}

int ZhaoHuan_Process(unsigned char *pChar)
{
	MUSTER_TELE *pZhaoHuan;
	unsigned int IPAddr;
	int i,j;

	pZhaoHuan = (MUSTER_TELE *)pChar;

	memset(pZhaoHuan,0,sizeof(MUSTER_TELE));

	pZhaoHuan->msg_type = 1;
	pZhaoHuan->muster_ver = 1;

	memset(pZhaoHuan->net_hostname,0,sizeof(pZhaoHuan->net_hostname));
	sprintf(pZhaoHuan->net_hostname,"MPC8313GW");
	ZhaoHuan_GetIPMAC(&pZhaoHuan->net_ip_addr,pZhaoHuan->net_mac);

	pZhaoHuan->net_ip_mask = 0xffffff00;
	pZhaoHuan->net_gateway = 0;
	pZhaoHuan->devmodel = 0;  // not use
	pZhaoHuan->workmodel = 1; // 1= server mode, 2=client mode
	pZhaoHuan->portnum = VirtCOM_DevState.COMNum;
	pZhaoHuan->firstport = VirtCOM_DevState.COMState[0].ServerPort;

	memset(pZhaoHuan->firmware,0,sizeof(pZhaoHuan->firmware));
	sprintf(pZhaoHuan->firmware,"aaaaaaaa");

	memset(pZhaoHuan->cfgpwd,0,sizeof(pZhaoHuan->cfgpwd));
	sprintf(pZhaoHuan->cfgpwd,"66666666");

	//pZhaoHuan->cfgpwd = 1;
	memset(pZhaoHuan->mapip,0,sizeof(pZhaoHuan->mapip));

	memset(pZhaoHuan->mapport,0,sizeof(pZhaoHuan->mapport));
	i = sizeof(pZhaoHuan->mapport)/sizeof(pZhaoHuan->mapport[0]);
	j= VirtCOM_DevState.COMNum;
	j= (j>i)?i:j;
	for(i=0;i<j;i++)
		pZhaoHuan->mapport[i] = VirtCOM_DevState.COMState[i].ServerPort;

	ZhaoHuan_GeneTestPacket((MUSTER_TELE *)pChar);
	return sizeof(MUSTER_TELE);
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
int PrintFile_InsertLine(int fp,unsigned char *pChar)
{
	//fwrite(pChar,strlen(pChar),1,fp);
	write(fp,pChar,strlen(pChar));
}

#define CMD_SET_LEDRUN_ON  21
#define CMD_SET_LEDRUN_OFF  22
#define CMD_SET_LEDALARM_ON  23
#define CMD_SET_LEDALARM_OFF  24

void  SetLED_Alarm(int Light)
{
	int i;
	int fd_GPIO,Data32=0,cmd;

	fd_GPIO = open(FILE_NAME_GPIO, O_RDWR);
	if (fd_GPIO < 0)
	{
		printf("open device gpio %s failed\n",FILE_NAME_GPIO);
		perror("error :\n");
		return ;
	}

	if(Light){
		cmd = CMD_SET_LEDALARM_ON;
	}else{
		cmd = CMD_SET_LEDALARM_OFF;
	}

	ioctl(fd_GPIO,cmd,&Data32);

	close(fd_GPIO);

	return ;
}


void  SetLED_RUN(int Light)
{
	int i;
	int fd_GPIO,Data32=0,cmd;

	fd_GPIO = open(FILE_NAME_GPIO, O_RDWR);
	if (fd_GPIO < 0)
	{
		printf("open device gpio %s failed\n",FILE_NAME_GPIO);
		perror("error :\n");
		return ;
	}

	if(Light){
		cmd = CMD_SET_LEDRUN_ON;
	}else{
		cmd = CMD_SET_LEDRUN_OFF;
	}
	ioctl(fd_GPIO,cmd,&Data32);

	close(fd_GPIO);
	return ;
}

int	main_PrintFile(void)
{
	/*lel*/
#if 1
	int wdt_fd = -1;
	int timeout;

	g_iAppRun = 1;

	// SignalHook(SIGINT,  SignHandler); /*CTRL-C*/
	// SignalHook(SIGQUIT, SignHandler); /*CTRL-\*/
	// SignalHook(SIGSTOP, SignHandler); /*CTRL-Z*/

	//创建守护进程
	//	init_daemon();

	wdt_fd = open(WDT, O_WRONLY);
	if (wdt_fd == -1)
	{
		printf("fail to open WDT !\n");
	}
	printf("WDT is opened!\n");

	timeout = 20;
	ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);

	ioctl(wdt_fd, WDIOC_GETTIMEOUT, &timeout);
	printf("The timeout was is %d seconds\n", timeout);

	while(g_iAppRun)
	{
		// if( -1 != is_proc_exist( ( char * )"./em761-b" ) )
		//	if( GetPIDbyName_Wrapper("em761-b") > 0 )
		if( GetPIDbyName_Wrapper("VirtCOM") > 0 )
		{
			write(wdt_fd, "\0", 1);
		//	printf("feed dog\n");
		}
		else
		{
			//	printf ( "not find em761-b\n" );
			printf ( "not find VirtCom\n" );
		}
		sleep(3);
	}

	// #if 1
	write(wdt_fd, "V", 1);
	close(wdt_fd);
	printf( WDT " is closeed!\n");
	// #endif

//	return 0;
#else
	int fp;
	unsigned char WriteBuf[2048];
	int i,j,index,SubIndex;
	NETCOM_COM_PARAM_STATE *pCOMParam;
	struct sockaddr_in MyIP;
	int RunLEDIsOn = 0;
	int AlarmLEDIsOn = 0;
	int fd_WDG;
	int cmd,arg;
	int count = 0;

	//return 0;

	fd_WDG = open("/dev/watchdog", O_RDWR);
	if (fd_WDG < 0)
	{
		printf("Open WDT Dev Error!\n");
		return -1;
	}


	cmd = WDIOC_SETTIMEOUT;
	arg = 30;
	printf("<--- Set WDT timeout to %d seconds--->\r\n",arg);
	if (ioctl(fd_WDG, cmd, &arg) < 0)
	{
		/*lel*/
#if 0
		printf("Call cmd Settimerout fail\n");
		return -1;
#endif
		/*end*/
	}
	cmd = WDIOC_SETOPTIONS;
	if (ioctl(fd_WDG, cmd) < 0)
	{
		printf("Call cmd Start WDT fail\n");
		return -1;
	}
	//return 0;
	while(1){
		if(RunLEDIsOn){
			SetLED_RUN(0);
			//SetLED_Alarm(0);
			RunLEDIsOn = 0;
		}else{
			SetLED_RUN(1);
			//SetLED_Alarm(1);
			RunLEDIsOn = 1;
		}
		if(VirtCOM_DevState.LEDStatus_Alarm == 1){
			if(AlarmLEDIsOn){
				SetLED_Alarm(0);
				AlarmLEDIsOn= 0;
			}else{
				SetLED_Alarm(1);
				AlarmLEDIsOn= 1;
			}
		}else{
			SetLED_Alarm(0);
			AlarmLEDIsOn= 0;
		}

		//sleep(1); continue;

		//------- Feed Dog ---------
		//printf("<--- Feed Dog %d --->\r\n",i);
		cmd = WDIOC_KEEPALIVE;
		if (ioctl(fd_WDG, cmd) < 0)
		{
			printf("Call cmd Feedwatchdog fail\n");
			;
		}

		sleep(1);
		count++;
		//	printf("count = %d\n", count);

	}
#endif
	/*end*/
}


