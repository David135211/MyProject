/*
 * main.c -- Main program for the GoAhead WebServer (LINUX version)
 *
 * Copyright (c) GoAhead Software Inc., 1995-2010. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 */

/******************************** Description *********************************/

/*
 *	Main program for for the GoAhead WebServer.
 */

/********************************* Includes ***********************************/

#include "../uemf.h"
#include "../wsIntrn.h"
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "netcom.h"
#ifdef WEBS_SSL_SUPPORT
#include "../websSSL.h"
#endif

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifdef USER_MANAGEMENT_SUPPORT
#include	"../um.h"
void	formDefineUserMgmt(void);
#endif


int WebServer_SetConfig(WEB_CONFIG *pConfig);
void WebServer_GetConfig(WEB_CONFIG * pConfig);
int WebServer_SetDefaultConfig(void);
void WebServer_GetStatus(VCOM_STATUS *pStatus);
/*********************************** Locals ***********************************/
/*
 *	Change configuration here
 */

static char_t		*rootWeb = T("/myapp/www");			/* Root web directory */
static char_t		*demoWeb = T("wwwdemo");		/* Root web directory */
static char_t		*password = T("");				/* Security password */
static int			port = WEBS_DEFAULT_PORT;		/* Server port */
static int			retries = 5;					/* Server port retries */
static int			finished = 0;					/* Finished flag */

/****************************** Forward Declarations **************************/

static int 	initWebs(int demo);
static int	aspTest(int eid, webs_t wp, int argc, char_t **argv);
static void formTest(webs_t wp, char_t *path, char_t *query);
static int  websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
		int arg, char_t *url, char_t *path, char_t *query);
static void	sigintHandler(int);
#ifdef B_STATS
static void printMemStats(int handle, char_t *fmt, ...);
static void memLeaks();
#endif
static void getConfigData(void);
static void setConfigData(void);

void refreshwebconfig(void);
static void formtest(webs_t wp, char_t *path, char_t *query);
static int getPortAttributeList(int eid, webs_t wp, int argc, char_t **argv);
static int getPortNetworkList(int eid, webs_t wp, int argc, char_t **argv);
void printNetconf(webs_t wp,int ifnum);
static int getNetconfigList(int eid, webs_t wp, int argc, char_t **argv);
static int getPortStatus(int eid, webs_t wp, int argc, char_t **argv);
static int getPortNetworkStatus(int eid, webs_t wp, int argc, char_t **argv);
void printNetStatus(webs_t wp,int ifnum);
static int getNetconfigStatus(int eid, webs_t wp, int argc, char_t **argv);





/*added by zql for test 2015.3.8*/
#define	LOCAL_PORT	8000
#define	REMOTE_PORT	10000

struct sockaddr_in remote_addr;
struct sockaddr_in local_addr;
int initflag = 1;/*是否初始化标志*/
int saveFlag = 0;/*是否保存标志，0 不保存，1保存*/


/*获取接口名称*/
static int getIfaceName(char *iface_name, int len)
{
	int r = -1;
	int flgs, ref, use, metric, mtu, win, ir;
	unsigned long int d, g, m;
	char devname[20];
	FILE *fp = NULL;

	if((fp = fopen("/proc/net/route", "r")) == NULL) {
		perror("fopen error!\n");
		return -1;
	}

	if (fscanf(fp, "%*[^\n]\n") < 0) {
		fclose(fp);
		return -1;
	}

	while (1) {
		r = fscanf(fp, "%19s%lx%lx%X%d%d%d%lx%d%d%d\n",
				devname, &d, &g, &flgs, &ref, &use,
				&metric, &m, &mtu, &win, &ir);
		if (r != 11) {
			if ((r < 0) && feof(fp)) {
				break;
			}
			continue;
		}

		strncpy(iface_name, devname, len);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	return -1;
}

/*获取IP地址*/
static int getIpAddress(char *iface_name, char *ip_addr, int len)
{
	int sockfd = -1;
	struct ifreq ifr;
	struct sockaddr_in *addr = NULL;

	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_name, iface_name);
	addr = (struct sockaddr_in *)&ifr.ifr_addr;
	addr->sin_family = AF_INET;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket error!\n");
		return -1;
	}

	if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0) {
		strncpy(ip_addr, inet_ntoa(addr->sin_addr), len);
		close(sockfd);
		return 0;
	}

	close(sockfd);

	return -1;
}

/*初始化各种select*/
void initModeSelect(STIfSelect *modeSelect)
{
	modeSelect->first  = "0";
	modeSelect->second = "1";
}

void initIfSelect(STIfSelect *ifSelect)
{
	ifSelect->first  = "1";
	ifSelect->second = "2";
}

void initRateSelect(STBaudSelect *rateSelect)
{
	rateSelect->first    = "300";
	rateSelect->second   = "600";
	rateSelect->third    = "1200";
	rateSelect->forth    = "2400";
	rateSelect->fifth    = "4800";
	rateSelect->sixth    = "9600";
	rateSelect->seventh  = "14400";
	rateSelect->eightth  = "19200";
	rateSelect->nineth   = "38400";
	rateSelect->tenth    = "57600";
	rateSelect->eleventh = "115200";

}
void initDataSelect(STDataSelect *dataSelect)
{
	dataSelect->first  = "5";
	dataSelect->second = "6";
	dataSelect->third  = "7";
	dataSelect->forth  = "8";
}
void initParitySelect(STParitySelect *paritySelect)
{
	paritySelect->first  = "0";
	paritySelect->second = "1";
	paritySelect->third  = "2";
	paritySelect->forth  = "3";
}
void initStopSelect(STStopSelect *stopSelect)
{
	stopSelect->first  = "1";
	stopSelect->second = "15";
	stopSelect->third  = "2";
}

/*获取配置数据*/
static void getConfigData(void)
{
	printf("%s->Line %d, Enter.\n",__FUNCTION__,__LINE__);
	WebServer_GetConfig(&webconfig);
	return ;

}

/*设置配置数据*/
static void setConfigData(void)
{
	printf("%s->Line %d, Enter.\n",__FUNCTION__,__LINE__);
	WebServer_SetConfig(&webconfig);
	return ;
}

/*获取配置数据*/
static void getStatusData(void)
{
	printf("%s->Line %d, Enter.\n",__FUNCTION__,__LINE__);
	WebServer_GetStatus(&vcomStatus);
}


/*刷新当前各接口的配置参数，并把该参数转换为字符型*/
void refreshwebconfig(void)
{
	int num = 0;
	int index = 0;
	char string[25];
	num = webconfig.ValidSerial;

	for(index=0; index<num; index++)
	{

		sprintf(portAttr[index].portindex, "%d", index+1);
		sprintf(portAttr[index].rate, "%d", webconfig.Serial[index].rate);
		sprintf(portAttr[index].stopbit, "%d", webconfig.Serial[index].stopbit);
		sprintf(portAttr[index].parity, "%d", webconfig.Serial[index].parity);

		/*接口类型*/
		if(webconfig.Serial[index].Interface == RS485)
		{
			memcpy(portAttr[index].Interface, "RS485", 6);
		}
		else if(webconfig.Serial[index].Interface == RS232)
		{
			memcpy(portAttr[index].Interface, "RS232", 6);
		}
		else if(webconfig.Serial[index].Interface == RS422)
		{
			memcpy(portAttr[index].Interface, "RS422", 6);
		}



	}
}

static void resetDev(webs_t wp, char_t *path, char_t *query)   // 原型
{
	WebServer_SetDefaultConfig();
}

static void checkAndSaveConfig(webs_t wp, char_t *path, char_t *query)   // 原型
{
	char_t *tmprate, *tmpparity, *tmpdata,*tmpstop,*tmpif,*tmpport,*tmpmode,*tmpip;
	char_t rate[7];		/*波特率*/
	char_t databits[2];	/*数据位*/
	char_t parity[2]; 	/*奇偶校验*/
	char_t stopbit[3];	/*停止位*/
	char_t Interface[2]; /* 串口协议*/
	char_t netport[6]; /* 网络端口*/
	char_t name[12];	/*变量名字*/
	char_t mode[2]; /* 连接模式*/
	char_t ip[16]; /* ip地址*/
	int index = 0;
	int value=0;
	int tmp = 0;
	int portCount = webconfig.ValidSerial;
	int netCount = webconfig.ValidNet;
	/*获取当前最新的数据*/
	refreshwebconfig();

	for(index=0; index<portCount; index++)
	{
		/*获取波特率*/
		memset(rate,0,sizeof(rate));
		sprintf(name, "baudrate%d", index+1);
		tmprate = websGetVar(wp, T(name), T(""));
		memcpy(rate,tmprate,7);
		value=0;
		sscanf(rate,"%d",&value);
		/*检查数据是否有更新*/
		if(value != webconfig.Serial[index].rate)
		{
			//printf("index %d, rate %d  current rate %d\n",index+1, webconfig.Serial[index].rate, value);
			webconfig.Serial[index].rate = value;
			saveFlag = 1;

		}

		/*获取数据位*/
		memset(databits,0,sizeof(databits));
		sprintf(name, "databit%d", index+1);
		tmpdata = websGetVar(wp, T(name), T(""));
		memcpy(databits,tmpdata,2);
		value = 0;
		sscanf(databits,"%d",&value);
		/*检查数据是否有更新*/
		if(value != webconfig.Serial[index].databits)
		{
			//printf("index %d, databits %d  current databits %d\n",index+1, webconfig.Serial[index].databits, value);
			webconfig.Serial[index].databits = value;
			saveFlag = 1;
		}

		/*获取停止位*/
		memset(stopbit,0,sizeof(stopbit));
		sprintf(name, "stopbit%d", index+1);
		tmpstop = websGetVar(wp, T(name), T(""));
		memcpy(stopbit,tmpstop,3);
		value = 0;
		sscanf(stopbit,"%d",&value);
		/*检查数据是否有更新*/
		if(value != webconfig.Serial[index].stopbit)
		{
			//printf("index %d, stopbit %d  current stopbit %d\n",index+1, webconfig.Serial[index].stopbit, value);
			webconfig.Serial[index].stopbit = value;
			saveFlag = 1;
		}


		/*获取奇偶校验位*/
		memset(parity,0,sizeof(parity));
		sprintf(name, "parity%d", index+1);
		tmpparity = websGetVar(wp, T(name), T(""));
		memcpy(parity,tmpparity,2);
		value = 0;
		sscanf(parity,"%d",&value);
		/*检查数据是否有更新*/
		//printf("index %d value %d\n",index+1,value);

		switch(value)
		{
			case 0:
				value = 'N';
				break;
			case 1:
				value = 'E';
				break;
			case 2:
				value = 'O';
				break;
			case 3:
				value = 'M';
				break;
			default:
				value = 'N';
				break;

		}
		if(value != webconfig.Serial[index].parity)
		{
			webconfig.Serial[index].parity = value;
			saveFlag = 1;
		}

		/*获取接口类型*/
		memset(Interface,0,sizeof(Interface));
		sprintf(name, "ifport%d", index+1);
		tmpif = websGetVar(wp, T(name), T(""));
		memcpy(Interface,tmpif,2);
		value = 0;
		sscanf(Interface,"%d",&value);

		if(index+1<=2)
		{
			if(value == 1)
			{
				value = RS232;
			}
			else if(value == 2)
			{
				value = RS485;
			}
		}
		else if(2<(index+1)<4)
		{
			if(value == 1)
			{
				value = RS422;
			}
			else if(value == 2)
			{
				value = RS485;
			}
		}

		if(value != webconfig.Serial[index].Interface)
		{
			//printf("index %d current %d  config %d\n", index+1, value,webconfig.Serial[index].Interface);
			webconfig.Serial[index].Interface = value;
			saveFlag = 1;
		}

		/*获取端口*/
		memset(netport,0,sizeof(netport));
		sprintf(name, "netport%d", index+1);
		tmpport = websGetVar(wp, T(name), T(""));
		memcpy(netport,tmpport,6);
		value = 0;
		sscanf(netport,"%d",&value);
		/*检查数据是否有更新*/
		if(value != webconfig.Serial[index].net_port)
		{
			webconfig.Serial[index].net_port= value;
			saveFlag = 1;
		}

		/*获取连接方式*/
		memset(mode,0,sizeof(mode));
		sprintf(name, "sockmode%d", index+1);
		tmpmode = websGetVar(wp, T(name), T(""));
		memcpy(mode,tmpmode,6);
		value = 0;
		sscanf(mode,"%d",&value);
		/*检查数据是否有更新*/
		if(value != webconfig.Serial[index].ClientMode)
		{
			webconfig.Serial[index].ClientMode= value;
			saveFlag = 1;
		}

		/*获取服务器ip地址*/
		memset(ip,0,sizeof(ip));
		sprintf(name, "serverip%d", index+1);
		tmpip = websGetVar(wp, T(name), T(""));
		memcpy(ip,tmpip,16);
		value = inet_addr(ip);
		/*检查数据是否有更新且IP地址合法*/
		if((value != webconfig.Serial[index].ServerIP) && (0xffffffff != value))
		{
			webconfig.Serial[index].ServerIP= value;
			saveFlag = 1;
		}
	}

	for(index=0; index<netCount; index++)
	{
		/*获取网口ip地址*/
		memset(ip,0,sizeof(ip));
		sprintf(name, "netip%d", index+1);
		tmpip = websGetVar(wp, T(name), T(""));
		memcpy(ip,tmpip,16);
		value = inet_addr(ip);
		/*检查数据是否有更新且IP地址合法*/
		if((value != webconfig.Net[index].IPAddr) &&  (0xffffffff != value))
		{
			webconfig.Net[index].IPAddr = value;
			saveFlag = 1;
		}
	}


	if(saveFlag == 1)
	{
		setConfigData();
		sleep(1);
		getConfigData();
	}


	websHeader(wp);
	//websWrite(wp, T("<body><h2> Interface %s  </h2>\n"),Interface);
	websWrite(wp, T("<meta http-equiv=%s content=%s>"),"refresh","0;url=/form.asp");
	websFooter(wp);
	websDone(wp, 200);

}

/*获取当前串口的硬件配置，并显示在网页上*/

static int getPortAttributeList(int eid, webs_t wp, int argc, char_t **argv)
{
	int i=0;
	int SerialNum = 0;
	char index[3];
	STIfSelect ifSelect;
	STIfSelect ifSelect2;
	STBaudSelect rateSelect;
	STDataSelect dataSelect;
	STParitySelect paritySelect;
	STStopSelect stopbitSelect;

	/*初始化下拉菜单选项*/
	initIfSelect(&ifSelect);
	initRateSelect(&rateSelect);
	initDataSelect(&dataSelect);
	initParitySelect(&paritySelect);
	initStopSelect(&stopbitSelect);

	SerialNum = webconfig.ValidSerial;

	while(i<SerialNum)
	{

		sprintf(index, "%d", i+1);

		websWrite(wp, T("<tr>"));
		websWrite(wp, T("<td name=port%s>串口%s</td>"),index,index);

		/*接口协议配置*/
		websWrite(wp, T("<td>"));

		if(i+1 <=2)
		{	websWrite(wp, T("<select name=ifport%s>"),index);
			if(webconfig.Serial[i].Interface== RS232)
			{
				websWrite(wp, T("<option value=%s selected=true>RS232</option>"),ifSelect.first);
				websWrite(wp, T("<option value=%s>RS485</option>"),ifSelect.second);
			}
			else if(webconfig.Serial[i].Interface== RS485)
			{
				websWrite(wp, T("<option value =%s>RS232</option>"),ifSelect.first);
				websWrite(wp, T("<option value =%s selected=true>RS485</option>"),ifSelect.second);
			}

			websWrite(wp, T("</select>"));
		}
		else if(2<(i+1)<4)
		{
			websWrite(wp, T("<select name=ifport%s>"),index);
			if(webconfig.Serial[i].Interface== RS422)
			{
				websWrite(wp, T("<option value=%s selected=true>RS422</option>"),ifSelect.first);
				websWrite(wp, T("<option value=%s>RS485</option>"),ifSelect.second);
			}
			else if(webconfig.Serial[i].Interface== RS485)
			{
				websWrite(wp, T("<option value =%s>RS422</option>"),ifSelect.first);
				websWrite(wp, T("<option value =%s selected=true>RS485</option>"),ifSelect.second);
			}

			websWrite(wp, T("</select>"));
		}
		else
		{
			websWrite(wp, T("%s"),"RS485");
		}

		websWrite(wp, T("</td>"));

		/*波特率配置*/
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<select name=baudrate%s>"),index);
		if(webconfig.Serial[i].rate== 300)
		{
			websWrite(wp, T("<option value =%s selected=true>300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 600)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s selected=true>600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 1200)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s selected=true>1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 2400)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s selected=true>2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 4800)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s selected=true>4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 9600)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s selected=true>9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 14400)
		{
			websWrite(wp, T("<option value =%sselected=true>300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s selected=true>14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 19200)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s selected=true>19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 38400)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s selected=true>38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);
		}
		else if(webconfig.Serial[i].rate== 57600)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s selected=true>57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s >115200</option>"),rateSelect.eleventh);;
		}
		else if(webconfig.Serial[i].rate== 115200)
		{
			websWrite(wp, T("<option value =%s >300</option>"),rateSelect.first);
			websWrite(wp, T("<option value =%s >600</option>"),rateSelect.second);
			websWrite(wp, T("<option value =%s >1200</option>"),rateSelect.third);
			websWrite(wp, T("<option value =%s >2400</option>"),rateSelect.forth);
			websWrite(wp, T("<option value =%s >4800</option>"),rateSelect.fifth);
			websWrite(wp, T("<option value =%s >9600</option>"),rateSelect.sixth);
			websWrite(wp, T("<option value =%s >14400</option>"),rateSelect.seventh);
			websWrite(wp, T("<option value =%s >19200</option>"),rateSelect.eightth);
			websWrite(wp, T("<option value =%s >38400</option>"),rateSelect.nineth);
			websWrite(wp, T("<option value =%s >57600</option>"),rateSelect.tenth);
			websWrite(wp, T("<option value =%s selected=true>115200</option>"),rateSelect.eleventh);;
		}
		websWrite(wp, T("</select>"));
		websWrite(wp, T("</td>"));

		/*数据位配置*/
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<select name=databit%s>"),index);
		if(webconfig.Serial[i].databits== 5)
		{
			websWrite(wp, T("<option value =%s selected=true>5</option>"),dataSelect.first);
			websWrite(wp, T("<option value =%s >6</option>"),dataSelect.second);
			websWrite(wp, T("<option value =%s >7</option>"),dataSelect.third);
			websWrite(wp, T("<option value =%s >8</option>"),dataSelect.forth);
		}
		else if(webconfig.Serial[i].databits== 6)
		{
			websWrite(wp, T("<option value =%s >5</option>"),dataSelect.first);
			websWrite(wp, T("<option value =%s selected=true>6</option>"),dataSelect.second);
			websWrite(wp, T("<option value =%s >7</option>"),dataSelect.third);
			websWrite(wp, T("<option value =%s >8</option>"),dataSelect.forth);
		}
		else if(webconfig.Serial[i].databits== 7)
		{
			websWrite(wp, T("<option value =%s >5</option>"),dataSelect.first);
			websWrite(wp, T("<option value =%s >6</option>"),dataSelect.second);
			websWrite(wp, T("<option value =%s selected=true>7</option>"),dataSelect.third);
			websWrite(wp, T("<option value =%s >8</option>"),dataSelect.forth);
		}
		else if(webconfig.Serial[i].databits== 8)
		{
			websWrite(wp, T("<option value =%s >5</option>"),dataSelect.first);
			websWrite(wp, T("<option value =%s >6</option>"),dataSelect.second);
			websWrite(wp, T("<option value =%s >7</option>"),dataSelect.third);
			websWrite(wp, T("<option value =%s selected=true>8</option>"),dataSelect.forth);
		}
		websWrite(wp, T("</select>"));
		websWrite(wp, T("</td>"));

		/*停止位配置*/
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<select name=stopbit%s>"),index);
		if(webconfig.Serial[i].stopbit== 1)
		{
			websWrite(wp, T("<option value =%s selected=true>1</option>"),stopbitSelect.first);
			websWrite(wp, T("<option value =%s >1.5</option>"),stopbitSelect.second);
			websWrite(wp, T("<option value =%s >2</option>"),stopbitSelect.third);
		}
		else if(webconfig.Serial[i].stopbit== 15)
		{
			websWrite(wp, T("<option value =%s >1</option>"),stopbitSelect.first);
			websWrite(wp, T("<option value =%s selected=true>1.5</option>"),stopbitSelect.second);
			websWrite(wp, T("<option value =%s >2</option>"),stopbitSelect.third);
		}
		else if(webconfig.Serial[i].stopbit== 2)
		{
			websWrite(wp, T("<option value =%s >1</option>"),stopbitSelect.first);
			websWrite(wp, T("<option value =%s >1.5</option>"),stopbitSelect.second);
			websWrite(wp, T("<option value =%s selected=true>2</option>"),stopbitSelect.third);
		}
		websWrite(wp, T("</select>"));
		websWrite(wp, T("</td>"));


		/*奇偶校验位配置*/
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<select name=parity%s>"),index);
		if(webconfig.Serial[i].parity== 'N')
		{
			websWrite(wp, T("<option value =%s selected=true>无</option>"),paritySelect.first);
			websWrite(wp, T("<option value =%s >偶校验</option>"),paritySelect.second);
			websWrite(wp, T("<option value =%s >奇校验</option>"),paritySelect.third);
			websWrite(wp, T("<option value =%s >Mark校验</option>"),paritySelect.forth);
		}
		else if(webconfig.Serial[i].parity== 'E')
		{
			websWrite(wp, T("<option value =%s >无</option>"),paritySelect.first);
			websWrite(wp, T("<option value =%s selected=true>偶校验</option>"),paritySelect.second);
			websWrite(wp, T("<option value =%s >奇校验</option>"),paritySelect.third);
			websWrite(wp, T("<option value =%s >Mark校验</option>"),paritySelect.forth);
		}
		else if(webconfig.Serial[i].parity== 'O')
		{
			websWrite(wp, T("<option value =%s >无</option>"),paritySelect.first);
			websWrite(wp, T("<option value =%s >偶校验</option>"),paritySelect.second);
			websWrite(wp, T("<option value =%s selected=true>奇校验</option>"),paritySelect.third);
			websWrite(wp, T("<option value =%s >Mark校验</option>"),paritySelect.forth);
		}
		else if(webconfig.Serial[i].parity== 'M')
		{
			websWrite(wp, T("<option value =%s >无</option>"),paritySelect.first);
			websWrite(wp, T("<option value =%s >偶校验</option>"),paritySelect.second);
			websWrite(wp, T("<option value =%s >奇校验</option>"),paritySelect.third);
			websWrite(wp, T("<option value =%s selected=true>Mark校验</option>"),paritySelect.forth);
		}

		websWrite(wp, T("</select>"));
		websWrite(wp, T("</td>"));
		websWrite(wp, T("</tr>"));

		i++;
	}


	return 0;
}

/*获取串口与网口的对应关系*/
static int getPortNetworkList(int eid, webs_t wp, int argc, char_t **argv)
{
	int i=0;
	int SerialNum = 0;
	int test=0;
	char_t port[6];
	char_t index[3];

	SerialNum = webconfig.ValidSerial;
	STIfSelect modeSelect;
	struct in_addr ipAddress;

	initModeSelect(&modeSelect);

	while(i<SerialNum)
	{
		printf("%d",i);
		sprintf(index, "%d", i+1);

		/*打印当前串口序号*/
		websWrite(wp, T("<tr>"));
		websWrite(wp, T("<td name=portindex%s>串口%s</td>"),index,index);

		/*端口号打印*/
		websWrite(wp, T("<td>"));
		sprintf(port, "%d", webconfig.Serial[i].net_port);
		websWrite(wp, T("<input style=%s type=text name=netport%s value=%s>"),"border: #ffff00 ",index,port);
		websWrite(wp, T("</td>"));

		/*连接类型*/
		websWrite(wp, T("<td name=socktype%s>TCP</td>"),index);

		/*连接方式*/
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<select name=sockmode%s>"),index);
		if(webconfig.Serial[i].ClientMode== 0)
		{
			websWrite(wp, T("<option value =%s selected=true>服务器</option>"),modeSelect.first);
			websWrite(wp, T("<option value =%s >客户端</option>"),modeSelect.second);
		}
		else if(webconfig.Serial[i].ClientMode== 1)
		{
			websWrite(wp, T("<option value =%s >服务器</option>"),modeSelect.first);
			websWrite(wp, T("<option value =%s selected=true>客户端</option>"),modeSelect.second);
		}
		websWrite(wp, T("</select>"));
		websWrite(wp, T("</td>"));

		/*服务器IP地址*/
		ipAddress.s_addr = webconfig.Serial[i].ServerIP;
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<input type=text name=serverip%s value=%s>"),index,inet_ntoa(ipAddress));
		websWrite(wp, T("</td>"));
		websWrite(wp, T("</tr>"));

		websWrite(wp, T("</tr>"));
		i++;
	}



}


static int getConfigStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	/*当前没有错误*/
	websWrite(wp, T("<tr>"));
	if(webconfig.ConfigErr == 0)
	{
		websWrite(wp, T("<td name=configStat>无错误</td>"));
	}
	else
	{
		websWrite(wp, T("<td name=configStat><font color=red>%s</font></td>"),webconfig.ErrStatus);
	}
	websWrite(wp, T("</tr>"));
}

static int ShowSoftVersion(int eid, webs_t wp, int argc, char_t **argv)
{
	getConfigData();

	refreshwebconfig();
	websWrite(wp, T("<tr>"));

	websWrite(wp, T("<td ><font size=%s color=blue>%s</font></td>"),"4",webconfig.SoftVer);

	websWrite(wp, T("</tr>"));
}



/*打印单个网口配置*/
void printNetconf(webs_t wp,int ifnum)
{
	char_t index[3];
	struct in_addr ipAddress;

	sprintf(index, "%d", ifnum);
	websWrite(wp, T("<tr>"));
	websWrite(wp, T("<td name=netindex%s>网口%s</td>"),index,index);
	ipAddress.s_addr = webconfig.Net[ifnum-1].IPAddr;
	websWrite(wp, T("<td>"));
	websWrite(wp, T("<input type=text name=netip%s value=%s>"),index,inet_ntoa(ipAddress));
	websWrite(wp, T("</td>"));
}


/*网口配置*/
static int getNetconfigList(int eid, webs_t wp, int argc, char_t **argv)
{
	int netNum =webconfig.ValidNet;

	if(netNum == 1)
	{
		printNetconf(wp,1);
	}
	else if(netNum == 2)
	{
		printNetconf(wp,1);
		printNetconf(wp,2);
	}
	else if(netNum == 3)
	{
		printNetconf(wp,1);
		printNetconf(wp,2);
		printNetconf(wp,3);
	}
	else if(netNum == 4)
	{
		printNetconf(wp,1);
		printNetconf(wp,2);
		printNetconf(wp,3);
		printNetconf(wp,4);
	}


}

/***********获取状态数据*********/
/*获取当前串口的硬件配置状态，并显示在网页上*/

static int getPortStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int i=0;
	int SerialNum = 0;
	char index[3];
	char count[20];

	/*初始化配置数据*/
	getConfigData();
	getStatusData();


	SerialNum = webconfig.ValidSerial;

	while(i<SerialNum)
	{
		sprintf(index, "%d", i+1);

		websWrite(wp, T("<tr>"));
		websWrite(wp, T("<td name=port%s>串口%s</td>"),index,index);

		/* COM Status */
		websWrite(wp, T("<td>%s</td>"),vcomStatus[i].COMStatus);

		/*接口协议*/
		if(webconfig.Serial[i].Interface== RS232)
		{
			websWrite(wp, T("<td >RS232</td>"));
		}
		else if(webconfig.Serial[i].Interface ==  RS422)
		{
			websWrite(wp, T("<td >RS422</td>"));
		}
		else if(webconfig.Serial[i].Interface ==  RS485)
		{
			websWrite(wp, T("<td >RS485</td>"));
		}



		/*波特率配置*/

		if(webconfig.Serial[i].rate== 300)
		{
			websWrite(wp, T("<td >300</td>"));

		}
		else if(webconfig.Serial[i].rate== 600)
		{
			websWrite(wp, T("<td >600</td>"));
		}
		else if(webconfig.Serial[i].rate== 1200)
		{
			websWrite(wp, T("<td >1200</td>"));
		}
		else if(webconfig.Serial[i].rate== 2400)
		{
			websWrite(wp, T("<td >2400</td>"));
		}
		else if(webconfig.Serial[i].rate== 4800)
		{
			websWrite(wp, T("<td >4800</td>"));
		}
		else if(webconfig.Serial[i].rate== 9600)
		{
			websWrite(wp, T("<td >9600</td>"));
		}
		else if(webconfig.Serial[i].rate== 14400)
		{
			websWrite(wp, T("<td >14400</td>"));
		}
		else if(webconfig.Serial[i].rate== 19200)
		{
			websWrite(wp, T("<td >19200</td>"));
		}
		else if(webconfig.Serial[i].rate== 38400)
		{
			websWrite(wp, T("<td >38400</td>"));
		}
		else

			if(webconfig.Serial[i].rate== 57600)
			{
				websWrite(wp, T("<td>57600</td>"));
			}
			else if(webconfig.Serial[i].rate== 115200)
			{
				websWrite(wp, T("<td >115200</td>"));
			}


		/*数据位配置*/
		if(webconfig.Serial[i].databits== 5)
		{
			websWrite(wp, T("<td >5</td>"));
		}
		else if(webconfig.Serial[i].databits== 6)
		{
			websWrite(wp, T("<td >6</td>"));
		}
		else if(webconfig.Serial[i].databits== 7)
		{
			websWrite(wp, T("<td >7</td>"));
		}
		else if(webconfig.Serial[i].databits== 8)
		{
			websWrite(wp, T("<td >8</td>"));
		}

		/*停止位配置*/
		if(webconfig.Serial[i].stopbit== 1)
		{
			websWrite(wp, T("<td >1</td>"));
		}
		else if(webconfig.Serial[i].stopbit== 15)
		{
			websWrite(wp, T("<td >1.5</td>"));
		}
		else if(webconfig.Serial[i].stopbit== 2)
		{
			websWrite(wp, T("<td >2</td>"));
		}



		/*奇偶校验位配置*/
		if(webconfig.Serial[i].parity== 'N')
		{
			websWrite(wp, T("<td >无</td>"));
		}
		else if(webconfig.Serial[i].parity== 'E')
		{
			websWrite(wp, T("<td >偶校验</td>"));
		}
		else if(webconfig.Serial[i].parity== 'O')
		{
			websWrite(wp, T("<td >奇校验</td>"));
		}
		else if(webconfig.Serial[i].parity== 'M')
		{
			websWrite(wp, T("<td >Mark校验</td>"));
		}
		else
		{
			websWrite(wp, T("<td >配置错误</td>"));
		}


		/*网口发送*/
		memset(count,0,sizeof(count));
		sprintf(count, "%d", vcomStatus[i].ClientSendByte);
		websWrite(wp, T("<td>%s</td>"),count);

		/*网口接收*/
		memset(count,0,sizeof(count));
		sprintf(count, "%d", vcomStatus[i].ClientRecvByte);
		websWrite(wp, T("<td>%s</td>"),count);

		/*串口发送*/
		memset(count,0,sizeof(count));
		sprintf(count, "%d", vcomStatus[i].SerialSendByte);
		websWrite(wp, T("<td>%s</td>"),count);

		/*串口接收*/
		memset(count,0,sizeof(count));
		sprintf(count, "%d", vcomStatus[i].SerialRecvByte);
		websWrite(wp, T("<td>%s</td>"),count);

		i++;
	}


	return 0;
}

static int getPortNetworkStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int SerialNum = 0;
	char index[64][3];
	char port[64][6];
	char mode[64][8];
	struct in_addr ip[64];
	int i = 0;

	getConfigData();
	getStatusData();
	SerialNum = webconfig.ValidSerial;

	memset(index,0,sizeof(index));
	memset(port,0,sizeof(port));
	memset(ip,0,sizeof(ip));
	while(i<SerialNum)
	{
		/*串口序号*/
		sprintf(index[i], "%d", i+1);

		/*端口号*/
		sprintf(port[i], "%d", webconfig.Serial[i].net_port);

		/*连接方式*/
		if(webconfig.Serial[i].ClientMode== 0)
		{
			memcpy(mode[i],"服务器",8);
		}
		else if(webconfig.Serial[i].ClientMode== 1)
		{
			memcpy(mode[i],"客户端",8);
		}

		/*IP地址*/
		ip[i].s_addr = webconfig.Serial[i].ServerIP;

		i++;
	}

	i=0;
	while(i<SerialNum)
	{
		//memset(index,0,sizeof(index));
		//sprintf(index, "%d", i+1);


		/*打印当前串口序号*/
		websWrite(wp, T("<tr>"));
		websWrite(wp, T("<td>串口%s</td>"),index[i]);

		websWrite(wp, T("<td >%s</td>"),port[i]);
		websWrite(wp, T("<td >TCP</td>"));
		websWrite(wp, T("<td >%s</td>"),mode[i]);
		websWrite(wp, T("<td >%s</td>"),inet_ntoa(ip[i]));

		websWrite(wp, T("</tr>"));
		i++;
	}

}


/*打印单个网口配置*/
void printNetStatus(webs_t wp,int ifnum)
{
	char_t index[3];
	struct in_addr ipAddress;

	sprintf(index, "%d", ifnum);
	websWrite(wp, T("<tr>"));
	websWrite(wp, T("<td name=netindex%s>网口%s</td>"),index,index);
	ipAddress.s_addr = webconfig.Net[ifnum-1].IPAddr;
	websWrite(wp, T("<td>%s</td>"),inet_ntoa(ipAddress));
	websWrite(wp, T("</tr>"));
}


/*网口配置*/
static int getNetconfigStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int netNum =webconfig.ValidNet;

	if(netNum == 1)
	{
		printNetStatus(wp,1);
	}
	else if(netNum == 2)
	{
		printNetStatus(wp,1);
		printNetStatus(wp,2);
	}
	else if(netNum == 3)
	{
		printNetStatus(wp,1);
		printNetStatus(wp,2);
		printNetStatus(wp,3);
	}
	else if(netNum == 4)
	{
		printNetStatus(wp,1);
		printNetStatus(wp,2);
		printNetStatus(wp,3);
		printNetStatus(wp,4);
	}


}

#if 0
/*获取串口与网口的收发状态*/

static int getSendRcvStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	char count[20];

	websWrite(wp, T("<tr>"));
	websWrite(wp, T("<td>网口发送</td>"));
	sprintf(count, "%d", vcomStatus.ClientSendByte);
	websWrite(wp, T("<td>%s</td>"),count);
	websWrite(wp, T("</tr>"));

	websWrite(wp, T("<tr>"));
	websWrite(wp, T("<td>网口接收</td>"));
	sprintf(count, "%d", vcomStatus.ClientRecvByte);
	websWrite(wp, T("<td>%s</td>"),count);
	websWrite(wp, T("</tr>"));

	websWrite(wp, T("<tr>"));
	websWrite(wp, T("<td>串口发送</td>"));
	sprintf(count, "%d", vcomStatus.SerialSendByte);
	websWrite(wp, T("<td>%s</td>"),count);
	websWrite(wp, T("</tr>"));

	websWrite(wp, T("<tr>"));
	websWrite(wp, T("<td>串口接收</td>"));
	sprintf(count, "%d", vcomStatus.SerialRecvByte);
	websWrite(wp, T("<td>%s</td>"),count);
	websWrite(wp, T("</tr>"));
}


#endif




int main_VirtCOM(int argc, char **argv);
/*********************************** Code *************************************/
/*
 *	Main -- entry point from LINUX
 */

int main(int argc, char** argv)
{
	int i, demo = 0;
	pthread_t ThreadID_MainVirtCOM;
	int ret;
	int COMNum;

	COMNum = 8;
	if(argc >= 2){
		COMNum = atoi(argv[1]);
	}
	/*
	   for (i = 1; i < argc; i++) {
	   if (strcmp(argv[i], "-demo") == 0) {
	   demo++;
	   }
	   }

*/
	/*
	 *	Initialize the memory allocator. Allow use of malloc and start
	 *	with a 60K heap.  For each page request approx 8KB is allocated.
	 *	60KB allows for several concurrent page requests.  If more space
	 *	is required, malloc will be used for the overflow.
	 */
	bopen(NULL, (60 * 1024), B_USE_MALLOC);
	/*signal(SIGPIPE, SIG_IGN);
	  signal(SIGINT, sigintHandler);
	  signal(SIGTERM, sigintHandler);*/

	/*
	 *	Initialize the web server
	 */
	if (initWebs(demo) < 0) {
		return -1;
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLOpen();
	/*	websRequireSSL("/"); */	/* Require all files be served via https */
#endif


	ret = pthread_create(&ThreadID_MainVirtCOM, NULL, (void *)main_VirtCOM, &COMNum);
	if(ret != 0)
	{
		printf ("Create pthread main_VirtCOM error!return %d\n",ret);
	}
	usleep(500);
	/*
	 *	Basic event loop. SocketReady returns true when a socket is ready for
	 *	service. SocketSelect will block until an event occurs. SocketProcess
	 *	will actually do the servicing.
	 */
	finished = 0;
	while (!finished) {
		if (socketReady(-1) || socketSelect(-1, 1000)) {
			socketProcess(-1);
		}
		websCgiCleanup();
		emfSchedProcess();
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLClose();
#endif

#ifdef USER_MANAGEMENT_SUPPORT
	umClose();
#endif

	/*
	 *	Close the socket module, report memory leaks and close the memory allocator
	 */
	websCloseServer();
	socketClose();
#ifdef B_STATS
	memLeaks();
#endif
	bclose();
	return 0;
}

/*
 *	Exit cleanly on interrupt
 */
static void sigintHandler(int unused)
{
	finished = 1;
}

/******************************************************************************/
/*
 *	Initialize the web server.
 */

static int initWebs(int demo)
{
	struct hostent	*hp;
	struct in_addr	intaddr;
	char			host[128], dir[128], webdir[128];
	char			*cp;
	char_t			wbuf[128];
	char iface_name[20];

	if(getIfaceName(iface_name, sizeof(iface_name)) < 0) {
		printf("get interface name error!\n");
		return -1;
	}

	if(getIpAddress(iface_name, (char *) &intaddr, 15) < 0) {
		printf("get interface ip address error!\n");
		return -1;
	}

	printf("address:%s\n",(char *) &intaddr);

	/*
	 *	Initialize the socket subsystem
	 */
	socketOpen();

#ifdef USER_MANAGEMENT_SUPPORT
	/*
	 *	Initialize the User Management database
	 */
	umOpen();
	umRestore(T("umconfig.txt"));
#endif

	/*
	 *	Define the local Ip address, host name, default home page and the 
	 *	root web directory.
	 */
	/*if (gethostname(host, sizeof(host)) < 0) {
	  error(E_L, E_LOG, T("Can't get hostname"));
	  printf("gethostname failed!\n");
	  return -1;
	  }
	  if ((hp = gethostbyname(host)) == NULL) {
	  error(E_L, E_LOG, T("Can't get host address"));
	  printf("gethostbyname failed!\n");
	  return -1;
	  }
	  memcpy((char *) &intaddr, (char *) hp->h_addr_list[0],
	  (size_t) hp->h_length);*/

	//intaddr.s_addr = inet_addr("192.168.1.211");
	/*
	 *	Set ../web as the root web. Modify this to suit your needs
	 *	A "-demo" option to the command line will set a webdemo root
	 */
	getcwd(dir, sizeof(dir));
	if ((cp = strrchr(dir, '/'))) {
		*cp = '\0';
	}
	if (demo) {
		sprintf(webdir, "%s/%s", dir, demoWeb);
	} else {
		sprintf(webdir, "%s/%s", dir, rootWeb);
	}

	/*
	 *	Configure the web server options before opening the web server
	 */
	websSetDefaultDir(webdir);
	cp = inet_ntoa(intaddr);
	ascToUni(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
	websSetIpaddr(wbuf);
	ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
	websSetHost(wbuf);

	/*
	 *	Configure the web server options before opening the web server
	 */
	websSetDefaultPage(T("default.asp"));
	websSetPassword(password);

	/*
	 *	Open the web server on the given port. If that port is taken, try
	 *	the next sequential port for up to "retries" attempts.
	 */
	websOpenServer(port, retries);

	/*
	 * 	First create the URL handlers. Note: handlers are called in sorted order
	 *	with the longest path handler examined first. Here we define the security
	 *	handler, forms handler and the default web page handler.
	 */
	websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler,
			WEBS_HANDLER_FIRST);
	websUrlHandlerDefine(T("/goform"), NULL, 0, websFormHandler, 0);
	websUrlHandlerDefine(T("/cgi-bin"), NULL, 0, websCgiHandler, 0);
	websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler,
			WEBS_HANDLER_LAST);

	/*
	 *	Now define two test procedures. Replace these with your application
	 *	relevant ASP script procedures and form functions.
	 */
	websAspDefine(T("aspTest"), aspTest);
	//websFormDefine(T("formTest"), formTest);
	/*Added by zql 2015.3.8*/
	websAspDefine(T("MakeNetconfigList"),getNetconfigList);
	websAspDefine(T("MakePortAttributeList"),getPortAttributeList);
	websAspDefine(T("MakePortNetworkList"),getPortNetworkList);
	websAspDefine(T("MakeConfigStatus"),getConfigStatus);
	websAspDefine(T("ShowSoftVer"),ShowSoftVersion);
	websAspDefine(T("getPortStatus"),getPortStatus);
	websAspDefine(T("getPortNetworkStatus"),getPortNetworkStatus);
	websAspDefine(T("getNetconfigStatus"),getNetconfigStatus);
	//websAspDefine(T("getSendRcvStatus"),getSendRcvStatus);

	websFormDefine(T("formTest"), checkAndSaveConfig);	 // 注册

	websFormDefine(T("resetDev"), resetDev);


	/*
	 *	Create the Form handlers for the User Management pages
	 */
#ifdef USER_MANAGEMENT_SUPPORT
	formDefineUserMgmt();
#endif

	/*
	 *	Create a handler for the default home page
	 */
	websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0); 
	return 0;
}

/******************************************************************************/
/*
 *	Test Javascript binding for ASP. This will be invoked when "aspTest" is
 *	embedded in an ASP page. See web/asp.asp for usage. Set browser to 
 *	"localhost/asp.asp" to test.
 */

static int aspTest(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t	*name, *address;

	if (ejArgs(argc, argv, T("%s %s"), &name, &address) < 2) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	return websWrite(wp, T("Name: %s, Address %s"), name, address);
}

/******************************************************************************/
/*
 *	Test form for posted data (in-memory CGI). This will be called when the
 *	form in web/forms.asp is invoked. Set browser to "localhost/forms.asp" to test.
 */

static void formTest(webs_t wp, char_t *path, char_t *query)
{
	char_t	*name, *address;

	name = websGetVar(wp, T("name"), T("Joe Smith"));
	address = websGetVar(wp, T("address"), T("1212 Milky Way Ave."));

	websHeader(wp);
	websWrite(wp, T("<body><h2>Name: %s, Address: %s</h2>\n"), name, address);
	websFooter(wp);
	websDone(wp, 200);
}

/******************************************************************************/
/*
 *	Home page handler
 */

static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
		int arg, char_t *url, char_t *path, char_t *query)
{
	/*
	 *	If the empty or "/" URL is invoked, redirect default URLs to the home page
	 */
	if (*url == '\0' || gstrcmp(url, T("/")) == 0) {
		websRedirect(wp, WEBS_DEFAULT_HOME);
		return 1;
	}
	return 0;
}

/******************************************************************************/

#ifdef B_STATS
static void memLeaks() 
{
	int		fd;

	if ((fd = gopen(T("leak.txt"), O_CREAT | O_TRUNC | O_WRONLY, 0666)) >= 0) {
		bstats(fd, printMemStats);
		close(fd);
	}
}

/******************************************************************************/
/*
 *	Print memory usage / leaks
 */

static void printMemStats(int handle, char_t *fmt, ...)
{
	va_list		args;
	char_t		buf[256];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	write(handle, buf, strlen(buf));
}
#endif

/******************************************************************************/
