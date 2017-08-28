#include "widget.h"
#include "ui_widget.h"
#include "./Pages/cpage.h"
#include "./Pages/cpage_home.h"
#include "./Pages/cpage_syspic.h"
#include "./Pages/cpage_dataquery.h"
#include "./Pages/cpage_diagnostic.h"
#include "./Pages/cpage_sysparameter.h"
#include <QPainter>
#include <QImage>
#include "cglobal.h"
#include <QMouseEvent>
#include "structure.h"
#include "./Proto/cprotocol.h"
#include "./ComPort/SerialPort.h"
#include "./ComPort/CTcpPortServer.h"
#include "./ComPort/TcpClient.h"
#include "Typedef.h"
#include "Proto/publicmethod.h"
#include "Element/ceelment.h"
#include "BasePort.h"

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{/*{{{*/
	ui->setupUi(this);
	m_wDevNum = 0;
#if 1
	//start
	timerid = startTimer(1000);
//	timerid = startTimer(10000);
	//end
#endif
}/*}}}*/

Widget::~Widget()
{/*{{{*/
	if(LoginDlg != NULL)
		delete LoginDlg;
	if(YkDlg != NULL)
		delete YkDlg;
	if(Numkeyboard != NULL)
		delete Numkeyboard;
	clearData();
	delete ui;
	printf("delete ui OK.\n");
}/*}}}*/

BOOL Widget::GetDevData(PBUS_LINEST pBus)
{/*{{{*/
	char m_sDevPath[128]="";
	char m_ProtoFile[32]="";
	memset(m_sDevPath, 0, sizeof(m_sDevPath));
	switch(pBus->wProtoType)
	{
		case PROTO_MODBUS:
			strcpy(m_ProtoFile, "ModBus");
			break;
		case PROTO_MODBUSTCP:
			strcpy(m_ProtoFile, "ModBusTcp");
			break;
	}
	sprintf(m_sDevPath, "%s/%s/%s%02d.ini", SYSDATAPATH, m_ProtoFile, DEVNAME, pBus->wBusNo + 1);
	CProfile profile(m_sDevPath);

	return ProcessFileData(profile, pBus);
}/*}}}*/

BOOL Widget::ProcessFileData(CProfile &profile, PBUS_LINEST pBus)
{/*{{{*/
	//BOOL bRtn = FALSE;
	if(!profile.IsValid())
	{
		printf("Open file %s Failed ! \n ", profile.m_szFileName);
		return FALSE;
	}

	char sSect[ 200 ] = "DEVNUM";
	char sKey[ 20 ][ 100 ]={ "module", "serialno", "addr", "name", "template",
		"yxnum", "ycnum", "ymnum", "yknum" };

	WORD wModule = 0;
	int serialno=1;
	WORD addr =3;
	char sName[ 50 ] = { 0 };
	char stemplate[ 200 ] = { 0 };
	int iNum = 0;
	int yxnum = 0;
	int ycnum = 0;
	int ymnum = 0;
	int yknum = 0;

	iNum = profile.GetProfileInt(sSect, (char *)"NUM", 0);
	if(iNum == 0)
	{
		printf("Get DEVNUM Failed ! \n ");
		return FALSE;
	}

	for(int i = 0; i < iNum; i++)
	{
		sprintf(sSect, "%s%03d", "DEV", i + 1);

		wModule = profile.GetProfileInt(sSect, sKey[ 0 ], 0);
		serialno = profile.GetProfileInt(sSect, sKey[ 1 ], 0);
		addr = profile.GetProfileInt(sSect, sKey[ 2 ], 0);
		profile.GetProfileString(sSect, sKey[ 3 ], (char *)"NULL", sName, sizeof(sName));
		profile.GetProfileString(sSect, sKey[ 4 ], (char *)"NULL", stemplate, sizeof(stemplate));
		yxnum = profile.GetProfileInt(sSect, sKey[ 5 ], 0);
		ycnum = profile.GetProfileInt(sSect, sKey[ 6 ], 0);
		ymnum = profile.GetProfileInt(sSect, sKey[ 7 ], 0);
		yknum = profile.GetProfileInt(sSect, sKey[ 8 ], 0);

		//must check up pDev addr
		//each pDev addr exclusive
		PDEVST pDev = new DEVST;
		pDev->wProtoModuleNo = wModule;
		pDev->wBusNo = pBus->wBusNo;
		pDev->wAddr = addr;
		pDev->wDevState = COMM_ABNORMAL;
		pDev->wYc = ycnum;
		pDev->wYx = yxnum;
		pDev->wYm = ymnum;
		pDev->wYK = yknum;
		strcpy(pDev->stemplate, stemplate);

		//set bus transmit interface
		//transmit all kinds of data yc yx ym and so on
		//all dev in gather bus set transmit serialNo .
		if(pBus->wProtoType == PROTO_MODBUS)
		{
			m_wDevNum++;
			char sStn[ 64 ] = "";
			sprintf(sStn, "%sstn%03d.conf",STATIONNO_PATH,m_wDevNum);
			CProfile Profile(sStn);
			char sTemp[ 200 ];
			memset(sTemp, 0, sizeof(sTemp));
			char sbuffer[ 200 ] = { 0 };
			int sizebuff = sizeof(sbuffer);
			memset(sbuffer, 0, sizebuff);

			Profile.GetProfileString((char *)"DEV", (char *)"dev", (char *)"null", sbuffer, sizebuff);
			if(strcmp("null", sbuffer) == 0)
			{
				printf("%s %s %s Config Error OK. \n", sStn,"DEV","dev");
				return FALSE;
			}
			char *tmpBusNo = strtok(sbuffer, ",");
			char *tmpDevAddr = strtok(NULL, ",");
			if(atoi(tmpBusNo)!=pDev->wBusNo || atoi(tmpDevAddr)!=pDev->wAddr)
			{
				printf("%s %d %d Config Error OK. \n", 
						sStn, atoi(tmpBusNo),atoi(tmpDevAddr));
				return FALSE;
			}

			AddDataToDev(Profile, pDev, &pDev->ycArray, DataType::YC_DTYPE);
			AddDataToDev(Profile, pDev, &pDev->yxArray, DataType::YX_DTYPE);
			AddDataToDev(Profile, pDev, &pDev->ymArray, DataType::YM_DTYPE);
			AddDataToDev(Profile, pDev, &pDev->ykArray, DataType::YK_DTYPE);
		}
		else if(pBus->wProtoType == PROTO_MODBUSTCP)
		{

		}
		pBus->DevArray[ pDev->wAddr ] = pDev;

	}
	return TRUE;
}/*}}}*/

BOOL Widget::GetBusProfileString(CProfile &Profile, char * sSect, char * sKey, char * sTemp, int &size)
{/*{{{*/
	if(!Profile.IsValid())
		return FALSE;

	Profile.GetProfileString(sSect, sKey, (char *)"null", sTemp, size);
	if(strcmp("null", sTemp) == 0)
		return FALSE;

	return TRUE;
}/*}}}*/

BOOL Widget::AddNetPara(PBUSDATA pBusData, CProfile &profile, BYTE byNo)
{/*{{{*/
	if(pBusData == NULL || !profile.IsValid())
		return FALSE;

	char sNetCard[ ] = "NetCard";
	char sSect[ ] = "PORT";
	char sTemp[ 200 ] = { 0 };
	char sKey[ 200 ] = { 0 };
	int sizebuff = sizeof(sTemp);
	//获取网卡名字
	sprintf(sKey, "%s%02d", sNetCard, byNo);
	memset(sTemp, 0, sizeof(sTemp));
	if(!GetBusProfileString(profile, sSect, sKey, sTemp, sizebuff))
		return FALSE;

	strcpy(pBusData->m_NetCardName, sTemp);

	return TRUE;
}/*}}}*/

BOOL Widget::AddPortOtherPara(PBUSDATA pBusData, CProfile &profile, char * busString, BYTE byNo)
{/*{{{*/
	char sAttrib[ 100 ];
	UINT nPort = 0;
	BYTE byType = CBasePort::GetCommAttrib(busString, sAttrib, nPort);
	BOOL bflag = TRUE;

	switch(byType)
	{
		case Property::COMRS485://case COMRS422:case COMRS485:
			break;
		case Property::LAN_SERVER:
			//读取子网掩码，网关，DNS
			bflag = AddNetPara(pBusData, profile, byNo);
			//strcpy(pBusData->m_NetCardName, sTemp);
			break;
		case Property::LAN_CLIENT:
			strcpy(pBusData->m_szIP, sAttrib);
			break;
		default:
			return 0xFF;
	}

	pBusData->m_BusType = byType;
	return bflag;
}/*}}}*/

int Widget::GetBusProtoInterval(CProfile &Profile, char * sSect, char * sKey)
{/*{{{*/
	int wVal = 0;

	wVal = Profile.GetProfileInt(sSect, sKey, -1);
	if(wVal == -1)
	{
		printf("Get %s Error OK. \n", sKey);
		return wVal;
	}
	return wVal;
}/*}}}*/


BOOL Widget::ReadPortPara(INITBUS & bus)
{/*{{{*/
	char sBusLine[] = BUS_PATH;
	CProfile Profile(sBusLine);
	BYTE byLineNum = 0;


	Profile.GetProfileString((char *)"PROJECT", (char *)"name", (char *)"null", m_strProjectName, sizeof(m_strProjectName));
	if(strcmp("null", m_strProjectName) == 0)
	{
		printf("%s name read fail \n ", sBusLine);
		return FALSE;
	}
//	printf("$$$ %s %d sBusLine = %s m_strProjectName = %s\n", __func__, __LINE__, sBusLine, m_strProjectName);


	byLineNum = (BYTE)Profile.GetProfileInt((char *)"LINE-NUM", (char *)"NUM", 0);
	if(byLineNum == 0 || byLineNum > TOTAL_BUS)
	{
		printf("%s Line Number exceed MaxLine : Current Line Number = %d \n ", sBusLine, byLineNum);
		return FALSE;
	}

	for(BYTE i = 0; i < byLineNum; i++)
	{
		char sPort[ ] = "port";
		char sPara[ ] = "para";
		char sInterval[ ] = "internal";
		char sSect[ ] = "PORT";
		int iInterval = 0;
		int sizebuff = 0;
		char sbuffer[ 200 ] = { 0 };
		sizebuff = sizeof(sbuffer);
		char sDllPath[ 200 ];
		BOOL bPause = FALSE;
		memset(sDllPath, 0, sizeof(sDllPath));
		char sTemp[ 200 ];
		memset(sTemp, 0, sizeof(sTemp));

		memset(sbuffer, 0, sizebuff);

		//获取通讯口类型
		sprintf(sTemp, "%s%02d", sPort, i + 1);
		Profile.GetProfileString(sSect, sTemp, (char *)"null", sbuffer, sizebuff);
		if(strcmp("null", sbuffer) == 0)
		{
			printf("Bus Config Error OK.\n");
			return FALSE;
		}
		else if(strcmp(PASUE, sbuffer) == 0)
		{
			printf("Bus%d is PASUE \n ", i + 1);
			bPause = TRUE;
		}

		if(!bPause)
		{
			//获取协议类型
			sprintf(sTemp, "%s%02d", sPara, i + 1);
			Profile.GetProfileString(sSect, sTemp, (char *)"null", sDllPath, sizeof(sDllPath));
			if(strcmp("null", sDllPath) == 0)
			{
				printf("Bus Config Error OK. \n");
				return FALSE;
			}

			//获取通讯间隔
			sprintf(sTemp, "%s%02d", sInterval, i + 1);
			iInterval = GetBusProtoInterval(Profile, sSect, sTemp);
			if(iInterval == -1)
				return FALSE;
		}

		PBUSDATA pBusData = new BUSDATA;
		strcpy(pBusData->m_BusString, sbuffer);
		pBusData->m_BusInterval = iInterval;
		strcpy(pBusData->m_ProtocolDllPath, sDllPath);

		if(!bPause)
		{
			//获取通讯口其它通讯参数
			if(!AddPortOtherPara(pBusData, Profile, sbuffer, i + 1))
			{
				delete pBusData;
				return FALSE;
			}

			//添加总线协议打印信息
			//strcpy(pBusData->m_szPrintNetCard, NetCard);
			//strcpy(pBusData->m_szPrintRemoteIp, RemoteIp);
			//pBusData->m_dwPrintStartPortNum = StartPortNum;
		}

		bus.AddBusString(pBusData);
	}
	return TRUE;
}/*}}}*/
/*
   BOOL Widget::SetLocalNetPara(CProfile &profile)
   {
   char sNetCard[ ] = "NetCard";
   char sSect[ ] = "NetCard";
   char sDNS[ ] = "DNS";
   char sGateWay[ ] = "GateWay";
   char sSubNetMask[ ] = "SubNetMask";
   char sIP[ ] = "IP";

   char sTemp[ 200 ] = { 0 };
   int sizebuff = sizeof(sTemp);

   char sSysDNS[ ] = "SYSTEM-DNS";
   char sSysDNS_Key[ ] = "DNS";
   char sTemp_Dns[ 20 ] = { 0 };
   int sizebuff_Dns = sizeof(sTemp_Dns);

   char sKey[ 200 ] = { 0 };

   BYTE byNo = 1;
   memset(sKey, 0, sizeof(sKey));
   sprintf(sKey, "%s%02d", sDNS, byNo);
   NETPARAM netParam;

//获取DNS
//if(!GetBusProfileString(profile, sSect, sKey, sTemp, sizebuff))
//return FALSE;

strcpy(netParam.dns, sTemp);

//获取网关
sprintf(sKey, "%s%02d", sGateWay, byNo);
memset(sTemp, 0, sizeof(sTemp));
if(!GetBusProfileString(profile, sSect, sKey, sTemp, sizebuff))
return FALSE;

strcpy(netParam.gateway, sTemp);

//获取子网掩码
sprintf(sKey, "%s%02d", sSubNetMask, byNo);
memset(sTemp, 0, sizeof(sTemp));
if(!GetBusProfileString(profile, sSect, sKey, sTemp, sizebuff))
return FALSE;

strcpy(netParam.mask, sTemp);

//获取网卡名字
sprintf(sKey, "%s%02d", sNetCard, byNo);
memset(sTemp, 0, sizeof(sTemp));
if(!GetBusProfileString(profile, sSect, sKey, sTemp, sizebuff))
return FALSE;

strcpy(netParam.name, sTemp);

//获取网卡IP
sprintf(sKey, "%s%02d", sIP, byNo);
memset(sTemp, 0, sizeof(sTemp));
if(!GetBusProfileString(profile, sSect, sKey, sTemp, sizebuff))
return FALSE;
strcpy(netParam.ip, sTemp);

if(!GetBusProfileString(profile, sSysDNS, sSysDNS_Key, sTemp_Dns, sizebuff_Dns))
{
return FALSE;
}
else
{
strcpy(netParam.dns, sTemp_Dns);
}

return TRUE;
}
	*/
BOOL Widget::LoadBusData()
{/*{{{*/
	//get Bus dev data from project file
	INITBUS bus;
	/*读取总线参数*/
	if(ReadPortPara(bus))
		printf("ReadPortPara OK.\n");
	else
	{
		printf("ReadPortPara Failed. \n ");
		return FALSE;
	}
	int BusNum = bus.m_busData.size();
	if(BusNum == 0)
	{
		printf("CreateBusLine In ReadProtPara Failed . \n ");
		return FALSE;
	}
	//use temporary memory data
	for(BYTE i = 0; i < BusNum; i++)
	{
		PBUS_LINEST pBus = new BUS_LINEST;
		pBus->wBusNo = i;
		pBus->wInterval = 200;
		sprintf(pBus->szBusName, "%d", i + 1);
		pBus->byBusType = BUS_PAUSE;
		pBus->wBusCommState = COMM_ABNORMAL;
		pBus->wTotalDev = 0;
		pBus->pProperty = NULL;
		pBus->pProto = NULL;
		if(i == BusNum - 1)
		{
			PDataType pData = NULL;
			PDEVST pDev = new DEVST;
			pDev->wBusNo = pBus->wBusNo;
			pDev->wAddr = 1;
			pDev->wYc = 0;
			pDev->wYx = 2;
			pDev->wYm = 0;
			pDev->wYK = 0;

			for(BYTE j = 0; j < pDev->wYx; j++)
			{
				pData = new YXST;
				//SetYxParam(pData, sbuffer);

				pData->wBusNo =pDev->wBusNo;
				pData->wAddr = pDev->wAddr;
				pData->wPntNo = i;
				pDev->yxArray.push_back(pData);
			}
			pBus->DevArray[ pDev->wAddr ] = pDev;

		}
		if(bus.m_busData[i]->m_BusType != Property::PAUSETYPE)
		{
			pBus->wPropertyType = bus.m_busData[i]->m_BusType;
			strcpy(pBus->ProtodllPath, bus.m_busData[i]->m_ProtocolDllPath);
			pBus->wInterval = bus.m_busData[i]->m_BusInterval;
			strcpy(pBus->m_BusString, bus.m_busData[i]->m_BusString);
			if(0 == strcmp(bus.m_busData[i]->m_ProtocolDllPath, "./lib/libModBusRtu.so"))
			{
				pBus->byBusType = BUS_GATHER;
				pBus->wProtoType = PROTO_MODBUS;
				GetDevData(pBus);
			}

			else if(0 == strcmp(bus.m_busData[i]->m_ProtocolDllPath, "./lib/libModBusTcp.so"))
			{
				pBus->byBusType = BUS_TRANS;
				pBus->wProtoType = PROTO_MODBUSTCP;
				GetDevData(pBus);

			}
		}
		//set bus property
		CreateBusProperty(pBus);

		//set bus protocol
		CreateBusProtocol(pBus);

		//Initialize Protocol
		AddProtoModule(pBus);

		m_busArray[ i ] = pBus;
	}

	//cabinet associate with voltage current
	//default: make 3 circuits
	//CreateLogicalCircuit();

	//default:create 2 cabinets, 2 circuits in one cabinet and 1 in another cabinet
	CreateCabinet();
	//扫描Client端是否掉线
	ScanServerOnLine();

	return TRUE;
}/*}}}*/

BOOL Widget::CreateLogicalCircuit()
{/*{{{*/
	WORD wCircuitNum = 3;
	for(WORD i = 0; i < wCircuitNum; i++)
	{
		PSINGALCIRCUIT pCircuit = new SINGLECIRCUIT;
		char buf[ 10 ];
		sprintf(buf, "test%02d", i + 1);
		strcpy(pCircuit->sName, buf);
		pCircuit->wSerialNo = i;
		for(BYTE m = 0; m < 2; m++)
		{
			PDataType pData = AfxGetYc(i, 1, m);
			if(!pData)
			{
				Q_ASSERT(FALSE);
				return FALSE;
			}
			pCircuit->ycArray.push_back(pData);
		}

		for(BYTE m = 0; m < 2; m++)
		{
			PDataType pData = AfxGetYx(i, 1, m);
			if(!pData)
			{
				Q_ASSERT(FALSE);
				return FALSE;
			}
			pCircuit->yxArray.push_back(pData);
		}

		for(BYTE m = 0; m < 2; m++)
		{
			PDataType pData = AfxGetYm(i, 1, m);
			if(!pData)
			{
				Q_ASSERT(FALSE);
				return FALSE;
			}
			pCircuit->ymArray.push_back(pData);
		}

		m_sCircuitArray.push_back(pCircuit);
	}
	return TRUE;
}/*}}}*/

BOOL Widget::CreateCabinet()
{/*{{{*/
	char sCabinetIni[ ] = CABLE_PATH;
	CProfile Profile(sCabinetIni);
	BYTE byCabinetNum = 0;

	byCabinetNum = (BYTE)Profile.GetProfileInt((char *)"CABLENUM", (char *)"num", 0);
	if(byCabinetNum == 0)
	{
		printf("%s Number exceed MaxLine : Current Line Number = %d \n ", sCabinetIni, byCabinetNum);
		return FALSE;
	}
	for(BYTE byCabinetNo = 0; byCabinetNo < byCabinetNum; byCabinetNo++)
	{
		PCABINET pCab = new CABINET;
		char sTemp[ 200 ];
		memset(sTemp, 0, sizeof(sTemp));
		char sbuffer[ 200 ] = { 0 };
		int sizebuff = sizeof(sbuffer);
		memset(sbuffer, 0, sizebuff);
		char sSect[ ] = "CABLEINFO";
		char sCabinetName[ ] = "cablename";

		//Pic Page namespace
		sprintf(sTemp, "%s%02d", sCabinetName, byCabinetNo + 1);
		Profile.GetProfileString(sSect, sTemp, (char *)"null", sbuffer, sizebuff);
		if(strcmp("null", sbuffer) == 0)
		{
			printf("%s %s Config Error OK. \n", sCabinetIni,sSect);
			return FALSE;
		}
		pCab->wSerialNo = byCabinetNo;
		strcpy(pCab->sName, sbuffer);
		ReadCircuit(pCab);
		m_cabArray.push_back(pCab);
	}
	/*
	   BYTE cabinetNum = 2;
	   for(BYTE i = 0; i < cabinetNum; i++)
	   {
	   PSINGALCIRCUIT pCircuit = NULL;
	   PCABINET pCab = NULL;
	   pCab = new CABINET;
	   sprintf(pCab->sName, "Cab%02d", i + 1);
	   pCab->wSerialNo = i;

	   if(i == 0)
	   {
	   for(BYTE m = 0; m < 2; m++)
	   {
	   pCircuit = m_sCircuitArray[ m ];
	   if(pCircuit)
	   pCab->circuitArray[ pCircuit->wSerialNo ] = pCircuit;
	   }
	   }
	   else if(i == 1)
	   {
	   pCircuit = m_sCircuitArray[ 2 ];
	   if(pCircuit)
	   pCab->circuitArray[ pCircuit->wSerialNo ] = pCircuit;
	   }

	   m_cabArray.push_back(pCab);
	   }*/
	return TRUE;
}/*}}}*/

BOOL Widget::ReadCircuit(PCABINET pCab)
{/*{{{*/
	char sCircuitNoIni[ 64 ] = "";
	sprintf(sCircuitNoIni, "%sCable%02d.ini", CIRCUITNO_PATH, pCab->wSerialNo+1);
	CProfile Profile(sCircuitNoIni);
	char sSect[] = "LOOPINFO";
	WORD wLoopNum = 0;

	wLoopNum = (WORD)Profile.GetProfileInt((char *)"LOOPNUM", (char *)"num", 0);
	if(wLoopNum == 0)
	{
		printf("%s Num %d \n ", sCircuitNoIni,wLoopNum);
		return TRUE;
	}

	for(BYTE byLoopNo = 0; byLoopNo < wLoopNum; byLoopNo++)
	{
		PSINGALCIRCUIT pCircuit = new SINGLECIRCUIT;
		int iNum;
		char sbuffer[ 200 ] = { 0 };
		int sizebuff = sizeof(sbuffer);
		memset(sbuffer, 0, sizebuff);
		char sTemp[ 200 ];
		memset(sTemp, 0, sizeof(sTemp));

		char sLoopName[ ] = "loopname";
		char sLoopId[ ] = "loopid";

		memset(sTemp, 0, sizeof(sTemp));
		sprintf(sTemp, "%s%02d", sLoopName, byLoopNo + 1);
		Profile.GetProfileString(sSect, sTemp, (char *)"null", sbuffer, sizebuff);
		if(strcmp("null", sbuffer) == 0)
		{
			printf("Get %s %s %s Failed. \n", sCircuitNoIni,sSect,sTemp);
			return FALSE;
		}
		strcpy(pCircuit->sName, sbuffer);

		memset(sTemp, 0, sizeof(sTemp));
		sprintf(sTemp, "%s%02d", sLoopId, byLoopNo + 1);
		iNum = Profile.GetProfileInt(sSect, sTemp, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sCircuitNoIni,sSect,sTemp);
			return FALSE;
		}
		pCircuit->wSerialNo = iNum;

		ReadLoop(pCircuit);
		pCab->circuitArray[ pCircuit->wSerialNo ] = pCircuit;
		m_sCircuitArray.push_back(pCircuit);
	}
	return TRUE;
}/*}}}*/

BOOL Widget::ReadLoop(PSINGALCIRCUIT pCircuit)
{/*{{{*/
	char sLoopNoIni[ 64 ] = "";
	sprintf(sLoopNoIni, "%sLoop%03d.ini", LOOPNO_PATH, pCircuit->wSerialNo+1);
	CProfile Profile(sLoopNoIni);
	char sSect[16] = "";
	WORD wDataNum = 0;

	wDataNum = (WORD)Profile.GetProfileInt((char *)"DATANUM", (char *)"num", 0);
	if(wDataNum == 0)
	{
		printf("%s Num %d \n ", sLoopNoIni,wDataNum);
		return TRUE;
	}

	for(BYTE byDataNo = 0; byDataNo < wDataNum; byDataNo++)
	{
		int iDatatype, iBusNo,iAddr,iPoint;
		char sbuffer[ 200 ] = { 0 };
		int sizebuff = sizeof(sbuffer);
		memset(sbuffer, 0, sizebuff);
		char sTemp[ 200 ];
		memset(sTemp, 0, sizeof(sTemp));

		char sType[ ] = "type";
		char sBusNo[ ] = "bus";
		char sAddr[ ] = "addr";
		char sPoint[ ] = "point";

		memset(sSect, 0, sizeof(sSect));
		sprintf(sSect, "%s%04d", "DATA", byDataNo + 1);

		iDatatype = Profile.GetProfileInt(sSect, sType, -1);
		if(iDatatype == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sLoopNoIni,sSect,sType);
			return FALSE;
		}
		iBusNo = Profile.GetProfileInt(sSect, sBusNo, -1);
		if(iBusNo == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sLoopNoIni,sSect,sBusNo);
			return FALSE;
		}
		iAddr = Profile.GetProfileInt(sSect, sAddr, -1);
		if(iAddr == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sLoopNoIni,sSect,sAddr);
			return FALSE;
		}
		iPoint = Profile.GetProfileInt(sSect, sPoint, -1);
		if(iPoint == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sLoopNoIni,sSect,sPoint);
			return FALSE;
		}

		switch(iDatatype)
		{
			case DataType::YC_DTYPE:
				{
					PDataType pData = AfxGetYc(iBusNo, iAddr, iPoint);
					if(!pData)
					{
						Q_ASSERT(FALSE);
						return FALSE;
					}
					pCircuit->ycArray.push_back(pData);
				}
				break;
			case DataType::YX_DTYPE:
				{
					PDataType pData = AfxGetYx(iBusNo, iAddr, iPoint);
					if(!pData)
					{
						Q_ASSERT(FALSE);
						return FALSE;
					}
					pCircuit->yxArray.push_back(pData);
				}
				break;
			case DataType::YM_DTYPE:
				{
					PDataType pData = AfxGetYm(iBusNo, iAddr, iPoint);
					if(!pData)
					{
						Q_ASSERT(FALSE);
						return FALSE;
					}
					pCircuit->ymArray.push_back(pData);
				}
				break;
			default:
				printf("Get %s %s %s Failed ! \n ", sLoopNoIni,sSect,sType);
				return FALSE;
		}
	}
	return TRUE;
}/*}}}*/

BOOL Widget::AddProtoModule(PBUS_LINEST pBus)
{/*{{{*/
	if(!pBus || pBus->byBusType == BUS_PAUSE)
		return FALSE;

	if(!pBus->pProto || !pBus->pProto->IsProtoValid())
		return FALSE;

	BYTE size = pBus->DevArray.size();
	if(!size)
		return FALSE;

	QDEV_ARRAY_ITOR Begin_itor = pBus->DevArray.begin();
	QDEV_ARRAY_ITOR End_itor = pBus->DevArray.end();
	CProtocol * pProto = pBus->pProto;
	for(;Begin_itor != End_itor; Begin_itor++)
	{
		PDEVST pDev = Begin_itor.value();
		if(!pDev || pDev->wAddr != Begin_itor.key())
			return FALSE;

		pProto->AddModule(pDev->wBusNo, pDev->wAddr, pDev->wProtoModuleNo, pDev->stemplate);
	}

	return TRUE;
}/*}}}*/

BOOL Widget::CreateBusProtocol(PBUS_LINEST pBus)
{/*{{{*/
	if(!pBus || pBus->wProtoType == PROTO_NULL ||
			pBus->pProto)
		return FALSE;

	//Loadlibrary
	CMethod * pMethod = m_pMethod;
	void * pHandle = m_loadLibrary.GetProtoObj(pBus->ProtodllPath, pMethod);
	if(!pHandle)
	{
		//Q_ASSERT(FALSE);
		return FALSE;
	}

	CProtocol * pProto = (CProtocol *)pHandle;
	if(!pProto->IsProtoValid())
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}

	pBus->pProto = pProto;
	return TRUE;
}/*}}}*/


BOOL Widget::CreateBusProperty(PBUS_LINEST pBus)
{/*{{{*/
	if(!pBus)
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}
	char sComType[ 100 ]="";
	UINT nPort = 0;
	CBasePort::GetCommAttrib(pBus->m_BusString, sComType, nPort);
//	printf("##### m_BusString = %s\n", pBus->m_BusString);


	if(pBus->wPropertyType == Property::LAN_SERVER)
	{
		NETPARAM netPara;
		AfxGetCurNetPara(&netPara);

		LAN_SERVER_Property * pProperty = new LAN_SERVER_Property;
		pProperty->wPortNo = nPort;

		if(strlen(netPara.ip) != 0)
			strcpy(pProperty->serverIp, netPara.ip);

		CComPort * pPort = new CTcpPortServer;
		pPort->SetPreOpenPara(pProperty);

		pBus->pProperty = pProperty;
		pBus->pProperty->pComPort = pPort;
	//	printf("%s %d IsPortValid() = %d \n", __func__, __LINE__, pBus->pProperty->pComPort->IsPortValid());
	}
	else if(pBus->wPropertyType == Property::LAN_CLIENT)
	{
		//NETPARAM netPara;
		//AfxGetCurNetPara(&netPara);

		LAN_CLIENT_Property * pProperty = new LAN_CLIENT_Property;
		pProperty->wPortNo = nPort;

		if(strlen(sComType) != 0)
			strcpy(pProperty->clientIp, sComType);

		CComPort * pPort = new CTcpClient;
		pPort->SetPreOpenPara(pProperty);

		pBus->pProperty = pProperty;
		pBus->pProperty->pComPort = pPort;
	//	printf("%s %d IsPortValid() = %d \n", __func__, __LINE__, pBus->pProperty->pComPort->IsPortValid());
		m_VectorTcpClientPush_back(pPort);
	}
	else if(pBus->wPropertyType == Property::COMRS485)
	{
		COM_Property * pProperty = new COM_Property;
		pProperty->wPortNo = nPort;

		char szName[32];
		const char* p = sComType;
	//	printf("####### sComType = %s\n", sComType);
		int i, k, nLen;
		nLen = strlen(sComType);
		for(i = 0, k = 0; i < nLen; i++, p++)
		{
			if(*p != ',') { szName[k++] = *p; }
			else { szName[k] = '\0';p++; break; }
		}

		pProperty->wBaud = atoi(szName);
		pProperty->byDataBits = atoi(p+2);
		pProperty->byStopBits = atoi(p+4);
		pProperty->byParity = p[0];
	//	printf("#### wBaud = %d byDataBits = %d byStopBits = %d byParity = %c\n", pProperty->wBaud, pProperty->byDataBits, pProperty->byStopBits, pProperty->byParity);

		CComPort * pPort = new CSerialPort;
		pPort->SetPreOpenPara(pProperty);

		pBus->pProperty = pProperty;
		pBus->pProperty->pComPort = pPort;
	//	printf("%s %d IsPortValid() = %d \n", __func__, __LINE__, pBus->pProperty->pComPort->IsPortValid());
	}
	else if(pBus->wPropertyType == Property::COM_NULL)
		return TRUE;
	else
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}/*}}}*/

BOOL Widget::AddDataToDev(CProfile &profile, PDEVST pDev, QDEV_DATA_ARRAY * pArray, BYTE byType)
{/*{{{*/
	if(!pDev || !pArray)
		return FALSE;

	if(pArray->size() != 0)
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}


	static WORD wYcTransNo = 1;
	static WORD wYxTransNo = 1;
	static WORD wYmTransNo = 1;
	static WORD wYkTransNo = 1;

	static WORD wsAddr = -1;
	if(wsAddr != pDev->wAddr)
	{
		//wYcTransNo = 1;
		//wYxTransNo = 1;
		//wYmTransNo = 1;
		wsAddr = pDev->wAddr;
	}
	char sSect[16]="";
	char sTemp[16]="";
	char sbuffer[ 200 ] = "";
	int sizebuff = sizeof(sbuffer);
	WORD wNum = 0;
	if(byType == DataType::YC_DTYPE)
	{
		strcpy(sSect, "AI");
		wNum = pDev->wYc;
	}
	else if(byType == DataType::YX_DTYPE)
	{
		strcpy(sSect, "DI");
		wNum = pDev->wYx;
	}
	else if(byType == DataType::YM_DTYPE)
	{
		strcpy(sSect, "PI");
		wNum = pDev->wYm;
	}
	else if(byType == DataType::YK_DTYPE)
	{
		strcpy(sSect, "DO");
		wNum = pDev->wYK;
	}
	else
	{

		Q_ASSERT(FALSE);
		return FALSE;
	}

	WORD wBusNo = pDev->wBusNo;
	WORD wAddr = pDev->wAddr;


	for(BYTE i = 0; i < wNum; i++)
	{
		PDataType pData = NULL;

		sprintf(sTemp, "%s%03d", sSect, i + 1);
		profile.GetProfileString(sSect, sTemp, (char *)"null", sbuffer, sizebuff);
		if(strcmp("null", sbuffer) == 0)
		{
			printf("%s %s Config Error OK. \n", profile.m_szFileName,sTemp);
			return FALSE;
		}

		if(byType == DataType::YC_DTYPE)
		{
			pData = new YCST;
			SetYcParam(pData, sbuffer);
		}
		else if(byType == DataType::YX_DTYPE)
		{
			pData = new YXST;
			SetYxParam(pData, sbuffer);
		}
		else if(byType == DataType::YM_DTYPE)
		{
			pData = new YMST;
			SetYmParam(pData, sbuffer);
		}
		else if(byType == DataType::YK_DTYPE)
		{
			pData = new YKST;
			SetYkParam(pData, sbuffer);
		}
		else
		{

			Q_ASSERT(FALSE);
			return FALSE;
		}
		pData->wBusNo =wBusNo;
		pData->wAddr = wAddr;
		pData->wPntNo = i;
		/*
		//set transmitNo from memory now
		//set transmitno from file in future
		if(byType == DataType::YC_DTYPE)
		pData->wTransNo[ 8 ] = wYcTransNo++;
		else if(byType == DataType::YX_DTYPE)
		pData->wTransNo[ 8 ] = wYxTransNo++;
		else if(byType == DataType::YM_DTYPE)
		pData->wTransNo[ 8 ] = wYmTransNo++;
		else if(byType == DataType::YK_DTYPE)
		pData->wTransNo[ 8 ] = wYkTransNo++;
		*/
		pArray->push_back(pData);
	}

	return TRUE;
}/*}}}*/

/*==============================模拟量=======================================*/
/*=名称, 类型,单位,系数,零点,控制字*/
void Widget::SetYcParam(PDataType PData, char *szParam)
{/*{{{*/
	PYCST PYcData = (PYCST)PData;
	int i = 0;
	int nLen = strlen(szParam);
	if(nLen <= 0) return;
	char *p = strtok(szParam, ",");
	while(p)
	{
		switch(i)
		{
			case 0: //名称
				sprintf(PYcData->chName, "%s", p);
				break;
			case 1: //类型
				//pObj->byType = (BYTE)atoi(p);
				break;
			case 2: //单位
				// pObj->byUnit = (BYTE)atoi(p);
				break;
			case 3: //系数
				PYcData->fCoefficient = (float)atof(p);
				break;
			case 4: //偏移
				PYcData->fBaseValue = (float)atof(p);
				break;
			case 5: //点控制字
				//pObj->wPntCtrl = (WORD)atoi(p);
				break;
			case 6: //变化阈值
				PYcData->fThv = (WORD)atoi(p);
				break;
			default:
				break;
		}
		p = strtok(NULL, ",");
		i++;
	}
}/*}}}*/

/*=名称, 取反*/
void Widget::SetYxParam(PDataType PData, char *szParam)
{/*{{{*/
	PYXST PYxData = (PYXST)PData;
	int i = 0;
	int nLen = strlen(szParam);
	if(nLen <= 0) return;
	char *p = strtok(szParam, ",");
	while(p)
	{
		switch(i)
		{
			case 0: //名称
				sprintf(PYxData->chName, "%s", p);
				break;
			case 1: //类型
				PYxData->byReverse = (BYTE)atoi(p);
				break;
			case 2: //类型
				PYxData->byWarn = (BYTE)atoi(p);
				break;
			default:
				break;
		}
		p = strtok(NULL, ",");
		i++;
	}
}/*}}}*/

/*=名称, 类型,单位*/
void Widget::SetYmParam(PDataType PData, char *szParam)
{/*{{{*/
	PYMST PYmData = (PYMST)PData;
	int i = 0;
	int nLen = strlen(szParam);
	if(nLen <= 0) return;
	char *p = strtok(szParam, ",");
	while(p)
	{
		switch(i)
		{
			case 0: //名称
				sprintf(PYmData->chName, "%s", p);
				break;
			case 1: //类型
				//pObj->byType = (BYTE)atoi(p);
				break;
			case 2: //单位
				// pObj->byUnit = (BYTE)atoi(p);
				break;
			case 3: //系数
				PYmData->fCoefficient = (float)atof(p);
				break;
			default:
				break;
		}
		p = strtok(NULL, ",");
		i++;
	}
}/*}}}*/

/*=名称, 类型,单位,系数,零点,控制字*/
void Widget::SetYkParam(PDataType PData, char *szParam)
{/*{{{*/
	PYKST PYkData = (PYKST)PData;
	int i = 0;
	int nLen = strlen(szParam);
	if(nLen <= 0) return;
	char *p = strtok(szParam, ",");
	while(p)
	{
		switch(i)
		{
			case 0: //名称
				sprintf(PYkData->chName, "%s", p);
				break;
			case 1: //类型
				//pObj->byType = (BYTE)atoi(p);
				break;
			default:
				break;
		}
		p = strtok(NULL, ",");
		i++;
	}
}/*}}}*/

void Widget::clearData()
{/*{{{*/
	AfxGetGlobal()->SetThreadRunFlag(FALSE);

	WORD size = m_ThreadArray.size();
	DWORD dwkey = CMsg::MSG_REAL_CHANNEL;
	for(WORD i = 0; i < size; i++)
	{
		PSTHREAD_PROC pProc = m_ThreadArray[ dwkey ];
		if(pProc)
		{
			pthread_cancel(pProc->tid);
			pthread_join(pProc->tid, NULL);
			pProc->tid = 0;
			delete pProc;
			pProc = NULL;
		}

		m_ThreadArray.remove(dwkey);
		dwkey++;
	}

	m_loadLibrary.ReleaseHandle();
}/*}}}*/

BOOL Widget::InitPics()
{/*{{{*/
	//Create All kinds of display page
	if(!CreatePageFactory())
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}/*}}}*/

BOOL Widget::LoadPageData()
{/*{{{*/
	//初始化每页数据
	WORD wIndex = PAGE_HOME;
	for(WORD i = wIndex; i < PAGE_MAX; i++)
	{
		CPage * pPage = m_PageMag[ i ];
		if(pPage)
		{
			if(i == PAGE_SYS_PIC)
				pPage->AddData(&m_objPageElement);
			else
				pPage->AddData(NULL);
		}
	}

	return TRUE;
}/*}}}*/

BOOL Widget::CreateThread()
{/*{{{*/
	//create real thread
	m_ThreadRealProc = ThreadRealProc;
	BOOL bFlag = CreateThread(m_ThreadRealProc, CMsg::MSG_REAL_CHANNEL);

	//create each port thread
	bFlag = CreateComThread();
	return bFlag;
}/*}}}*/

BOOL Widget::CreateComThread()
{/*{{{*/
	int size = m_busArray.size();
	if(!size)
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}

	QBUS_ARRAY_ITOR itor = m_busArray.begin();
	QBUS_ARRAY_ITOR itor_end = m_busArray.end();
	BYTE byIndex = 0;
	for(;itor != itor_end; itor++)
	{
		PBUS_LINEST pBus = itor.value();
		if(!pBus)
		{
			Q_ASSERT(FALSE);
			return FALSE;
		}

		if(pBus->byBusType == BUS_PAUSE)
			continue;

		if(!pBus->pProperty || !pBus->pProperty->pComPort)
			continue;

		//if(!pBus->pProperty->pComPort->IsPortValid())
		// continue;

		CMsg::MSG_TYPE type = (CMsg::MSG_TYPE)(CMsg::MSG_COM_CHANNEL + byIndex);
		if(!CreateThread(ThreadComProc, type, pBus))
		{
			Q_ASSERT(FALSE);
			return FALSE;
		}

		byIndex++;
	}

	return TRUE;
}/*}}}*/

BOOL Widget::CreateThread(THREAD_PROC pProc, CMsg::MSG_TYPE dwType,
		PBUS_LINEST pBus)
{/*{{{*/
	if(!pProc || dwType >= CMsg::MSG_COM_END || !pBus)
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}

	int iRtn = 0;
	pthread_t tid = 0;
	iRtn = pthread_create(&tid, 
			NULL, 
			pProc, 
			(void *)pBus);
	if(0 == iRtn)
	{
		if(pBus->pThreadProc != NULL)
		{
			Q_ASSERT(FALSE);
			return FALSE;
		}

		PSTHREAD_PROC pThreadProc = new STHREAD_PROC;
		pThreadProc->pMsg = new CMsg;
		pThreadProc->ThreadProc = pProc;
		pThreadProc->tid = tid;
		pThreadProc->pMsg->CreateMsgQueue(dwType);
		m_ThreadArray[ dwType ] = pThreadProc;
		pBus->pThreadProc = pThreadProc;
		return TRUE;
	}

	return FALSE;
}/*}}}*/

void *handler(void *arg)
{
	sleep(5);								//休息5s是为了避免启动时下发遥控给系统带来的不稳定!
	Widget *pointer = (Widget *)arg;
	do{
		for(QVector<CElement *>::iterator beg = pointer->yc_ykvec.begin(), en = pointer->yc_ykvec.end(); beg != en; beg++)
			(*beg)->CompareAttributeData();
		for(QVector<CElement *>::iterator beg = pointer->yx_ykvec.begin(), en = pointer->yx_ykvec.end(); beg != en; beg++)
			(*beg)->CompareAttributeData();
		sleep(1);
	}while(1);
}

BOOL Widget::CreateThread(THREAD_PROC pProc, CMsg::MSG_TYPE dwType)
{/*{{{*/
	if(!pProc || dwType >= CMsg::MSG_COM_END)
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}

	int iRtn = 0;
	pthread_t tid;

	PSTHREAD_PROC pThreadProc = new STHREAD_PROC;
	iRtn = pthread_create(&tid, 
			NULL, 
			pProc, 
			(void *)pThreadProc);

	//start
	pthread_t thread;
	pthread_create(&thread, NULL, handler, this);
	//end
	if(0 == iRtn)
	{
		if(pThreadProc->pMsg != NULL)
		{
			Q_ASSERT(FALSE);
			return FALSE;
		}

		pThreadProc->pMsg = new CMsg;
		pThreadProc->ThreadProc = pProc;
		pThreadProc->tid = tid;
		pThreadProc->pMsg->CreateMsgQueue(dwType);
		m_ThreadArray[ dwType ] = pThreadProc;
		return TRUE;
	}
	else
	{
		if(pThreadProc)
		{
			delete pThreadProc;
			pThreadProc = NULL;
		}

		Q_ASSERT(FALSE);
		return FALSE;
	}

	return FALSE;
}/*}}}*/

CPage * Widget::CreatePage(BYTE byType)
{/*{{{*/
	CPage * pPage = NULL;
	//	printf("%d %s byType = %d\n", __LINE__, __func__, byType);
	switch(byType)
	{
		case PAGE_HOME:
			pPage = new CPage_Home(byType);
			break;
		case PAGE_SYS_PIC:
			pPage = new CPage_SysPic(byType);
			break;
		case PAGE_DATA_QUERY:
			pPage = new CPage_DataQuery(byType);
			break;
		case PAGE_DIOGNOSTIC:
			pPage = new CPage_Diagnostic(byType);
			break;
		case PAGE_SYS_PARA:
			pPage = new CPage_SysParameter(byType);
			break;
		default:
			{
				Q_ASSERT(FALSE);
				return NULL;
			}
	}

	m_PageMag[ byType ] = pPage;
	return pPage;
}/*}}}*/

BOOL Widget::CreatePageFactory()
{/*{{{*/
	CreatePage(PAGE_HOME);
	CreatePage(PAGE_SYS_PIC);
	CreatePage(PAGE_DATA_QUERY);
	CreatePage(PAGE_DIOGNOSTIC);
	CreatePage(PAGE_SYS_PARA);

	return TRUE;
}/*}}}*/

void Widget::paintEvent(QPaintEvent * pEvent)
{/*{{{*/

	int cxSize = Widget::HOR_SIZE;
	int cySize = Widget::VER_SIZE;
	QImage image(cxSize, cySize, QImage::Format_ARGB32);
	QPainter imagePainter(&image);
	imagePainter.setRenderHint(QPainter::Antialiasing, TRUE);
	imagePainter.eraseRect(rect());

	BYTE byIndex = AfxGetCurPageIndex();
	CPage* pPage = AfxGetSpecifyPage(byIndex);
	if(pPage){
#if 0
		//start
		pixmap.load("/usr/share/ePut800/Pics/timg.jpg");
		painter.drawPixmap(0, 0, 800, 600, pixmap);				//为什么这一个不显示而上一个显示!
		pixmap.detach();
		//end
#endif
		//start
#if 0
		BYTE count = yc_ykvec.size();
		for(BYTE i = 0; i < count; i++)
			yc_ykvec[i]->CompareAttributeData();
		count = yx_ykvec.size();
		for(BYTE i = 0; i < count; i++)
			yx_ykvec[i]->CompareAttributeData();
#endif
		//end
		pPage->DrawPage(&imagePainter);
	}
	imagePainter.end();

	QPainter wPainter(this);
	wPainter.drawImage(0, 0, image);

	return;
}/*}}}*/

void Widget::timerEvent(QTimerEvent *)
{
	//	time_t timeflag = 0;
	//	timeflag = time(NULL);
	//	if((timeflag % 10) == 0)
	repaint();
}

void Widget::mousePressEvent(QMouseEvent * pEvent)
{/*{{{*/
	if(!pEvent)
	{
		Q_ASSERT(FALSE);
		return;
	}

	BYTE byIndex = AfxGetCurPageIndex();
	CPage* pPage = AfxGetSpecifyPage(byIndex);
	if(pPage)
		pPage->mousePressEvent(pEvent);
}/*}}}*/

BOOL Widget::OpenPort()
{/*{{{*/
	int size = m_busArray.size();
	if(!size)
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}

	QBUS_ARRAY_ITOR Bitor = m_busArray.begin();
	QBUS_ARRAY_ITOR Eitor = m_busArray.end();
	for(; Bitor != Eitor; Bitor++)
	{
		WORD wBusNo = Bitor.key();
		if(wBusNo >= TOTAL_BUS)
		{
			Q_ASSERT(FALSE);
			return FALSE;
		}

		PBUS_LINEST pBus = Bitor.value();
		if(pBus->byBusType == BUS_PAUSE)
			continue;

		if(pBus->pProperty && pBus->pProperty->m_ComType != Property::COM_NULL
				&& pBus->pProperty->pComPort)
		{
			char buf[ 200 ];
			memset(buf, 0,sizeof(buf));
			BOOL bflag = pBus->pProperty->pComPort->OpenPort(buf);

			if(!bflag)
			{
				perror(buf);
				// Q_ASSERT(FALSE);
				// return FALSE;
				continue;
			}
			pBus->wBusCommState = COMM_NOMAL;
		}

	}

	return TRUE;
}/*}}}*/

BOOL Widget::OnInitialize()
{/*{{{*/
	m_pMethod = new CPublicMethod;
	LoginDlg = new CLoginDlg(this);
	YkDlg = new CYkDlg(this);
	AlarmDlg = new CAlarmDlg(this);
	Numkeyboard = new Keyboard(this);
	NumkeyboardFocusWindow = BOARD_WIDGET;
	AfxReadPwd();
	AfxReadNetParamFile();

	// 初始化数据库
	InitDBase();

	//Get all kinds of datas from files
	//but now just from memory data
	LoadAllKindsOfData();

	//Init each pics
	InitPics();

	//load busData
	LoadBusData();

	//Load page Data
	LoadPageData();

	//open ports of all devices
	OpenPort();

	//create thread
	if(!CreateThread())
	{
		printf("create thread error!\n");
		return FALSE;
	}
	return TRUE;
}/*}}}*/

BOOL Widget::LoadAllKindsOfData()
{/*{{{*/
	// Get bus or devices data from file

	//Get pics from files include location and type
	InitPicsDataFromMemory();
	InitPicsAttributeFromMemory();
	return TRUE;
}/*}}}*/

//
BOOL Widget::InitPicsDataFromMemory()
{/*{{{*/
	char sPicIni[ ] = PIC_PATH;
	CProfile Profile(sPicIni);
	BYTE byPicPageNum = 0;

	byPicPageNum = (BYTE)Profile.GetProfileInt((char *)"PIC_NUM", (char *)"NUM", 0);
	if(byPicPageNum == 0)
	{
		printf("%s Number exceed MaxLine : Current Line Number = %d \n ", sPicIni, byPicPageNum);
		return FALSE;
	}
	for(BYTE byPage = 0; byPage < byPicPageNum; byPage++)
	{
		PPAGE_FST pPageFst = new PAGE_FST;
		//start
		//	QVector<Alarm*> pAlarms = new QVector<Alarm*>;
		QVector<Alarm> vec_alarm;
		//end
		char sTemp[ 200 ];
		memset(sTemp, 0, sizeof(sTemp));
		char sbuffer[ 200 ] = { 0 };
		int sizebuff = sizeof(sbuffer);
		memset(sbuffer, 0, sizebuff);
		char sSect[ ] = "PIC_INFO";
		char sPicPageName[ ] = "PICNAME";

		//start
		char wallbuffer[64];		//大小没有和上面的200统一，但这样更合理
		//end

		//Pic Page namespace
		sprintf(sTemp, "%s%02d", sPicPageName, byPage + 1);
		Profile.GetProfileString(sSect, sTemp, (char *)"null", sbuffer, sizebuff);

		//start
		memset(sSect, 0, sizeof(sSect));
		memset(sPicPageName, 0, sizeof(sPicPageName));
		memset(sTemp, 0, sizeof(sTemp));
		strcpy(sSect, "WALL_PAPER");
		strcpy(sPicPageName, "WALLNAME");
		sprintf(sTemp, "%s%02d", sPicPageName, byPage + 1);
		Profile.GetProfileString(sSect, sTemp, (char *)"null", wallbuffer, sizeof(wallbuffer));
		//	printf("%s %d wallbuffer = %s\n", __func__, __LINE__, wallbuffer);
		if(strcmp("null", wallbuffer) == 0){
			printf("----------------	%s:%d:ERROR!	----------------\n", __FILE__, __LINE__);
			return FALSE;
		}
		//end
		/*lel*/
#if 1
		char sPicWareHome[] = "PIC_WAREHOME";
		char sPicWareHomeNum[] = "PICWAREHOMENUM";
		BYTE PicWareHomeNum = 0;

		memset(sTemp, 0, sizeof(sTemp));
		sprintf(sTemp, "%s%02d", sPicWareHomeNum, byPage + 1);
		PicWareHomeNum = (BYTE)Profile.GetProfileInt(sPicWareHome, sTemp, 0);
		pPageFst->pWareHome = PicWareHomeNum;
		printf("%s %d %s = %d\n", __func__, __LINE__, sTemp, PicWareHomeNum);
#endif
		/*end*/

		if(strcmp("null", sbuffer) == 0)
		{
			printf("%s %d %s %s Config Error OK. \n", __func__, __LINE__, sPicIni, sSect);
			return FALSE;
		}
		pPageFst->wPageNo = byPage;
		strcpy(pPageFst->pPageTitle, sbuffer);
		strcpy(pPageFst->pWallName, wallbuffer);

		if(FALSE == ReadPicData(pPageFst))
		{
			printf("%s %d %s%02d Config Error OK. \n", __func__, __LINE__, sPicIni, pPageFst->wPageNo);
			return FALSE;
		}
		//start
		if(ReadAlarmFile(byPage, vec_alarm) == FALSE){
			printf("%s	%d Config Error Ko.\n", __FILE__, __LINE__);
			//	return FALSE;
		}
		alarmobj[byPage] = vec_alarm;
		//	if(ReadAlarmFile(byPage) == FALSE)
		//	printf("%s	%d Config Error Ko.\n", __FILE__, __LINE__);
		//end

		m_objPageElement.push_back(pPageFst);

	}
#if 0
	//start
	BYTE Size=alarmobj.size();
	for(BYTE i = 0; i < Size; i++){
		if(alarmobj[i].size() != 0){
			for(auto beg = alarmobj[i].begin(), en = alarmobj[i].end(); beg != en; beg++){
				printf("\n\n\n\n********	%d	********\n", (*beg).pageid);
				printf("********	%d	********\n", (*beg).objid);
				printf("********	%d	********\n", (*beg).id);
				printf("********	%d	********\n", (*beg).toppole);
				printf("********	%d	********\n", (*beg).top);
				printf("********	%d	********\n", (*beg).valuetop);
				printf("********	%d	********\n", (*beg).valuebottom);
				printf("********	%d	********\n", (*beg).bottom);
				printf("********	%d	********\n", (*beg).bottompole);
				printf("********	%d	********\n", (*beg).busno);
				printf("********	%d	********\n", (*beg).devaddr);
				printf("********	%d	********\n", (*beg).point);
				printf("********	%d	********\n\n\n\n", (*beg).relanum);
			}
		}
	}
	//end
#endif

	return TRUE;
}/*}}}*/

BOOL Widget::ReadPicData(PPAGE_FST pPageFst)
{/*{{{*/
	char sPicNoIni[ 64 ] = "";
	sprintf(sPicNoIni, "%spic%02d.ini", PICNO_PATH, pPageFst->wPageNo+1);
	//	printf("%s %d sPicNoIni = %s\n", __func__, __LINE__, sPicNoIni);
	CProfile Profile(sPicNoIni);
	WORD wSignNum = 0;

	wSignNum = (WORD)Profile.GetProfileInt((char *)"SIGNNUM", (char *)"NUM", 0);
	if(wSignNum == 0)
	{
		printf("%s Num %d \n ", sPicNoIni,wSignNum);
		return TRUE;
	}
	pPageFst->wEleNumber = wSignNum;
	for(BYTE bySignNo = 0; bySignNo < wSignNum; bySignNo++)
	{
		PELE_FST pEleFst = new ELE_FST;
		int iNum;
		char sTemp[ 200 ];
		memset(sTemp, 0, sizeof(sTemp));
		char sbuffer[ 200 ] = { 0 };
		int sizebuff = sizeof(sbuffer);
		memset(sbuffer, 0, sizebuff);
		char sId[ ] = "id";

		char sPicType[ ] = "signid";
		//start
		//	char sAlarm[] = "alarmid";
		//end
		char sDirect[ ] = "direct";
		char sLinkObjectId[ ] = "LinkObjectId";
		char sTop[ ] = "top";
		char sBottom[ ] = "bottom";
		char sLeft[ ] = "left";
		char sRight[ ] = "right";
		char sText[ ] = "text";

		//start
		char sRelanum[] = "relanum";
		//end

		char sSect[16] = "";
		sprintf(sSect, "SIGN%03d",bySignNo+1);

		pEleFst->wPageNo = pPageFst->wPageNo;

		iNum = Profile.GetProfileInt(sSect, sId, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sId);
			return FALSE;
		}
		pEleFst->wObjectID = iNum;

		iNum = Profile.GetProfileInt(sSect, sPicType, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sPicType);
			return FALSE;
		}
		pEleFst->wElementType = (UINT)iNum;

		iNum = Profile.GetProfileInt(sSect, sDirect, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sDirect);
			return FALSE;
		}
		pEleFst->wDirect = iNum;

		iNum = Profile.GetProfileInt(sSect, sLinkObjectId, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sLinkObjectId);
			return FALSE;
		}
		pEleFst->wLinkObjectId = iNum;

		iNum = Profile.GetProfileInt(sSect, sTop, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sTop);
			return FALSE;
		}
		pEleFst->wTop = iNum;

		iNum = Profile.GetProfileInt(sSect, sBottom, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sBottom);
			return FALSE;
		}
		pEleFst->wBottom = iNum;

		iNum = Profile.GetProfileInt(sSect, sLeft, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sLeft);
			return FALSE;
		}
		pEleFst->wLeft = iNum;

		iNum = Profile.GetProfileInt(sSect, sRight, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect,sRight);
			return FALSE;
		}
		pEleFst->wRight = iNum;
		//start
		iNum = Profile.GetProfileInt(sSect, sRelanum, -1);					//alarmx.ini中的元素id!
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni,sSect, sRelanum);
			return FALSE;
		}
		pEleFst->relanum = iNum;
		//end
		Profile.GetProfileString(sSect, sText, (char *)"null", sbuffer, sizebuff);
		/* if(strcmp("null", sbuffer) == 0)
		   {
		   printf("Get %s %s %s Failed ! \n", sPicNoIni,sSect,sText);
		   return TRUE;
		   }*/
		strcpy(pEleFst->text, sbuffer);
		/*lel*/
#if 1
		char sPicName[] = "picname";
		memset(sbuffer, 0, sizebuff);
		Profile.GetProfileString(sSect, sPicName, (char *)"null", sbuffer, sizebuff);
		if(strcmp("null", sbuffer) == 0){
			printf("Get %s %s %s Failed ! \n", sPicNoIni, sSect, sPicName);
			return TRUE;
		}
		strcpy(pEleFst->picname, sbuffer);

		char sPicIntroduce[] = "picintroduce";
		memset(sbuffer, 0, sizebuff);
		Profile.GetProfileString(sSect, sPicIntroduce, (char *)"null", sbuffer, sizebuff);
		if(strcmp("null", sbuffer) == 0){
			printf("Get %s %s %s Failed ! \n", sPicNoIni, sSect, sPicName);
			return TRUE;
		}
		strcpy(pEleFst->picintroduce, sbuffer);

		char sPrimitiveType[] = "primitivetype";
		iNum = Profile.GetProfileInt(sSect, sPrimitiveType, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sPicNoIni, sSect, sPrimitiveType);
			return FALSE;
		}
		pEleFst->wPrimitiveType = iNum;
#endif
		/*end*/
		//start
		//	if(pEleFst->wElementType == CElement::ALARM_FOR_AI)
		//	ReadAlarmFile(pEleFst, pPageFst);
		//end
		pPageFst->ElementArray.push_back(pEleFst);
	}
	return TRUE;
}/*}}}*/
#if 0
//start
/*
 * --------------------------------------------------------------------------------
 * class:			Widget
 * function name:	ReadAlarmFile
 * function:		读取alarmx.ini文件
 * parameter:		PELE_FST
 * return:			BOOL
 * --------------------------------------------------------------------------------
 */
BOOL Widget::ReadAlarmFile(QVector<Alarm*> &pAlarms, BYTE pagenum)
{/*{{{*/
	char sAlarm[64] = "";
	sprintf(sAlarm, "%salarm%02d.ini", ALARM_PATH, pagenum + 1);
	CProfile Profile(sAlarm);
	WORD unitnum;

	unitnum = (WORD)Profile.GetProfileInt((char *)"UNITNUM", (char *)"NUM", 0);
	if(unitnum == 0){
		printf("-----------	%s	%d	------------\n", __FILE__, __LINE__);
		printf("%d\n", unitnum);
		return FALSE;
	}

	for(BYTE unitno = 0; unitno < unitnum; unitno++){
		Alarm *punit = new Alarm;
		WORD iNum;
		char sPageid[] = "pageid";
		char sObjid[] = "objid";
		char sId[] = "id";
		char sToppole[ ] = "toppole";
		char sTop[ ] = "top";					//图元类别
		char sValuetop[ ] = "valuetop";						//关联对象中的数据顺序，这个顺序中yc.yx, ym是分开排列的!
		char sValuebottom[ ] = "valuebottom";			//关联对象id, LinkObjectId和direct共同确定唯一的关联数据!
		char sBottom[ ] = "bottom";							//top & bottom & left & right图元坐标
		char sBottompole[ ] = "bottompole";
		char sBusno[ ] = "busno";
		char sDevaddr[ ] = "devaddr";
		char sPoint[ ] = "point";							//文本图元的内容!				这个可以用来保存遥测定制的图片路径!
		char sRelanum[] = "relanum";						//暂时还没有使用!

		char sSect[16] = "";
		sprintf(sSect, "UNIT%03d",unitno + 1);
		/*{{{*/
		iNum = Profile.GetProfileInt(sSect, sPageid, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sPageid);
			return FALSE;
		}
		punit->pageid = iNum;

		iNum = Profile.GetProfileInt(sSect, sObjid, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sObjid);
			return FALSE;
		}
		punit->objid = iNum;

		iNum = Profile.GetProfileInt(sSect, sId, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sId);
			return FALSE;
		}
		punit->id = iNum;

		iNum = Profile.GetProfileInt(sSect, sToppole, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sToppole);
			return FALSE;
		}
		punit->toppole = iNum;

		iNum = Profile.GetProfileInt(sSect, sTop, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sTop);
			return FALSE;
		}
		punit->top = iNum;

		iNum = Profile.GetProfileInt(sSect, sValuetop, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sValuetop);
			return FALSE;
		}
		punit->valuetop = iNum;

		iNum = Profile.GetProfileInt(sSect, sValuebottom, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sValuebottom);
			return FALSE;
		}
		punit->valuebottom = iNum;

		iNum = Profile.GetProfileInt(sSect, sBottom, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sBottom);
			return FALSE;
		}
		punit->bottom = iNum;

		iNum = Profile.GetProfileInt(sSect, sBottompole, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sBottompole);
			return FALSE;
		}
		punit->bottompole = iNum;

		iNum = Profile.GetProfileInt(sSect, sBusno, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sBusno);
			return FALSE;
		}
		punit->busno = iNum;

		iNum = Profile.GetProfileInt(sSect, sDevaddr, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sDevaddr);
			return FALSE;
		}
		punit->devaddr = iNum;

		iNum = Profile.GetProfileInt(sSect, sPoint, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sPoint);
			return FALSE;
		}
		punit->point = iNum;

		iNum = Profile.GetProfileInt(sSect, sRelanum, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sRelanum);
			return FALSE;
		}
		punit->relanum = iNum;
		/*}}}*/
		pAlarms->push_back(punit);
#if 0
		printf("****	%d	****\n", (*beg)->pageid);
		printf("****	%d	****\n", (*beg)->objid);
		printf("****	%d	****\n", (*beg)->id);
		printf("****	%d	****\n", (*beg)->toppole);
		printf("****	%d	****\n", (*beg)->top);
		printf("****	%d	****\n", (*beg)->valuetop);
		printf("****	%d	****\n", (*beg)->valuebottom);
		printf("****	%d	****\n", (*beg)->busno);
		printf("****	%d	****\n", (*beg)->devaddr);
		printf("****	%d	****\n", (*beg)->point);
#endif
		//		return punit;
		return TRUE;
	}
}/*}}}*/
#else
BOOL Widget::ReadAlarmFile(BYTE byPage, QVector<Alarm> &vec_alarm)
{/*{{{*/
	char sAlarm[64] = "";
	sprintf(sAlarm, "%salarm%02d.ini", ALARM_PATH, byPage + 1);
	CProfile Profile(sAlarm);
	WORD unitnum;

	unitnum = (WORD)Profile.GetProfileInt((char *)"UNITNUM", (char *)"NUM", 0);
	if(unitnum == 0){
		printf("-----------	%s	%d	------------\n", __FILE__, __LINE__);
		printf("%d\n", unitnum);
		return FALSE;
	}

	printf("%s %d unitnum = %d\n", __func__, __LINE__, unitnum);
	for(BYTE unitno = 0; unitno < unitnum; unitno++){
		//		Alarm *punit = new Alarm;
		Alarm unitobj;
		memset(&unitobj, 0, sizeof(Alarm));
		short iNum;
		char sPageid[] = "pageid";
		char sObjid[] = "objid";
		char sId[] = "id";
		char sToppole[ ] = "toppole";
		char sTop[ ] = "top";              // 图元类别
		char sValuetop[ ] = "valuetop";    // 关联对象中的数据顺序，这个顺序中yc.yx, ym是分开排列的!
		char sValuebottom[ ] = "valuebottom"; // 关联对象id, LinkObjectId和direct共同确定唯一的关联数据!
		char sBottom[ ] = "bottom";        // top & bottom & left & right图元坐标
		char sBottompole[ ] = "bottompole";
		char sBusno[ ] = "busno";
		char sDevaddr[ ] = "devaddr";
		char sPoint[ ] = "point";         // 文本图元的内容!				这个可以用来保存遥测定制的图片路径!
	//	char sReservation[] = "reservation"; // 暂时还没有使用!

		char sSect[16] = "";
		sprintf(sSect, "UNIT%03d",unitno + 1);
		//	printf("%s %d sSect = %s\n", __func__, __LINE__, sSect);
		/*{{{*/
		iNum = Profile.GetProfileInt(sSect, sPageid, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sPageid);
			return FALSE;
		}
		unitobj.pageid = iNum;

		iNum = Profile.GetProfileInt(sSect, sObjid, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sObjid);
			return FALSE;
		}
		unitobj.objid = iNum;

		iNum = Profile.GetProfileInt(sSect, sId, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sId);
			return FALSE;
		}
		unitobj.id = iNum;

		iNum = Profile.GetProfileInt(sSect, sToppole, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sToppole);
			return FALSE;
		}
		unitobj.toppole = iNum;

		iNum = Profile.GetProfileInt(sSect, sTop, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sTop);
			return FALSE;
		}
		unitobj.top = iNum;

		iNum = Profile.GetProfileInt(sSect, sValuetop, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sValuetop);
			return FALSE;
		}
		unitobj.valuetop = iNum;
		//	printf("%s %d iNum = %d", __func__, __LINE__, iNum);

		iNum = Profile.GetProfileInt(sSect, sValuebottom, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sValuebottom);
			return FALSE;
		}
		unitobj.valuebottom = iNum;

		iNum = Profile.GetProfileInt(sSect, sBottom, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sBottom);
			return FALSE;
		}
		unitobj.bottom = iNum;

		iNum = Profile.GetProfileInt(sSect, sBottompole, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sBottompole);
			return FALSE;
		}
		unitobj.bottompole = iNum;

		iNum = Profile.GetProfileInt(sSect, sBusno, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sBusno);
			return FALSE;
		}
		unitobj.busno = iNum;

		iNum = Profile.GetProfileInt(sSect, sDevaddr, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sDevaddr);
			return FALSE;
		}
		unitobj.devaddr = iNum;

		iNum = Profile.GetProfileInt(sSect, sPoint, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sPoint);
			return FALSE;
		}
		unitobj.point = iNum;
#if 0
		iNum = Profile.GetProfileInt(sSect, sReservation, -1);
		if(iNum == -1){
			printf("Get %s %s %s Failed ! \n ", sAlarm, sSect, sReservation);
			return FALSE;
		}
		unitobj.reservation = iNum;
	//	printf("%s %d reservation = %d unitno = %d\n", __func__, __LINE__, unitobj.reservation, unitno);
#endif
		/*}}}*/
		vec_alarm.push_back(unitobj);
		/*
		   printf("****	%d	****\n", unitobj.pageid);
		   printf("****	%d	****\n", unitobj.objid);
		   printf("****	%d	****\n", unitobj.id);
		   printf("****	%d	****\n", unitobj.toppole);
		   printf("****	%d	****\n", unitobj.top);
		   printf("****	%d	****\n", unitobj.valuetop);
		   printf("****	%d	****\n", unitobj.valuebottom);
		   printf("****	%d	****\n", unitobj.busno);
		   printf("****	%d	****\n", unitobj.devaddr);
		   printf("****	%d	****\n", unitobj.point);
		   */
		//		return punit;
	}
	return TRUE;
}/*}}}*/
//end
#endif

BOOL Widget::InitPicsAttributeFromMemory()
{/*{{{*/
	char sObjIni[ ] = OBJECT_PATH;
	CProfile Profile(sObjIni);
	BYTE byObjNum = 0;
	char sTemp[ 200 ];
	memset(sTemp, 0, sizeof(sTemp));
	char sbuffer[ 200 ] = { 0 };
	int sizebuff = sizeof(sbuffer);
	memset(sbuffer, 0, sizebuff);
	char sSect[16] = "";
	char sObjId[ ] = "id";
	//char sObjType[ ] = "type";
	char sObjName[ ] = "name";
	char sObjNodeL[ ] = "nodenum1";
	char sObjNodeR[ ] = "nodenum2";
	int iRtn=0;

	byObjNum = (BYTE)Profile.GetProfileInt((char *)"OBJNUM", (char *)"NUM", 0);
	if(byObjNum == 0)
	{
		printf("%s Number exceed MaxLine : Current Line Number = %d \n ", sObjIni, byObjNum);
		return FALSE;
	}
	for(BYTE byObjNo = 0; byObjNo < byObjNum; byObjNo++)
	{
		PPIC_ATTRIBUT_FST pPicAttFst = new PIC_ATTRIBUTE_FST;


		memset(sSect, 0,sizeof(sSect));
		sprintf(sSect, "%s%03d","OBJ", byObjNo + 1);
		iRtn = Profile.GetProfileInt(sSect, sObjId, 0);
		if(iRtn == 0)
		{
			printf("%s %s %s Config Error OK. \n", sObjIni,sSect,sObjId);
			return FALSE;
		}
		pPicAttFst->wObjectID = iRtn;

		iRtn = Profile.GetProfileInt(sSect, sObjNodeL, -1);
		if(iRtn == -1)
		{
			printf("%s %s %s Config Error OK. \n", sObjIni,sSect,sObjId);
			return FALSE;
		}
		pPicAttFst->wObjectNodeL = iRtn;

		iRtn = Profile.GetProfileInt(sSect, sObjNodeR, -1);
		if(iRtn == -1)
		{
			printf("%s %s %s Config Error OK. \n", sObjIni,sSect,sObjId);
			return FALSE;
		}
		pPicAttFst->wObjectNodeR = iRtn;

		Profile.GetProfileString(sSect, sObjName, (char *)"null", sbuffer, sizebuff);
		if(strcmp("null", sbuffer) == 0)
		{
			printf("%s %s %s Config Error OK. \n", sObjIni,sSect,sObjName);
			return FALSE;
		}
		strcpy(pPicAttFst->ObjectName, sbuffer);



		ReadBindData(pPicAttFst);
		m_objAllElementAttribute_Map[ pPicAttFst->wObjectID ] = pPicAttFst;
		//m_objAllElementAttribute.push_back(pPicAttFst);
	}
	/*
	   WORD wObjectID = 0x1000;

	   for(BYTE page = 0; page < 1; page++)
	   {

	   for(BYTE byindex = 0; byindex < m_objPageElement.at(page)->wEleNumber; byindex++)
	   {
	   PPIC_ATTRIBUT_FST pPicAttFst = new PIC_ATTRIBUTE_FST;
	   pPicAttFst->wObjectID = wObjectID++;
	// set element position
	if(byindex == 0) //generatrix pos
	{
	continue;
	}
	else if(byindex > 36 && byindex < 36+16+1)
	{
	PPIC_ATTRIBUTE_DATA_FST Attribute_data = new PIC_ATTRIBUTE_DATA_FST;
	Attribute_data->wBusNo = 2;
	Attribute_data->wAddr = 1;
	Attribute_data->wPntNo = byindex - 37;
	Attribute_data->byAttributeDataByte = 1;
	Attribute_data->byDataType = PIC_ATTRIBUTE_DATA_FST::YC_DTYPE;

	pPicAttFst->DataArray.push_back(Attribute_data);
	}
	else if(byindex % 4 == 1)//linker pos
	{
	continue;
	}
	else if(byindex % 4 == 2) //breaker pos
	{
	for(BYTE i = 0; i < 2; i++)
	{
	PPIC_ATTRIBUTE_DATA_FST Attribute_data = new PIC_ATTRIBUTE_DATA_FST;


	Attribute_data->wPntNo = byindex/4+page*9;
	if(Attribute_data->wPntNo == 0 || Attribute_data->wPntNo == 1)
	{
	Attribute_data->wBusNo = 0;
	}
	else
	{
	Attribute_data->wPntNo = Attribute_data->wPntNo-2;

	Attribute_data->wBusNo = 1;
	}

	if(i==0)
	{
	Attribute_data->wAddr = 1;
	Attribute_data->byAttributeDataByte = 1;
	Attribute_data->byDataType = PIC_ATTRIBUTE_DATA_FST::YX_DTYPE;
	}
	else
	{
	Attribute_data->wAddr = 2;
	Attribute_data->byAttributeDataByte = 3;
	Attribute_data->byDataType = PIC_ATTRIBUTE_DATA_FST::YK_DTYPE;
	}

	sprintf(Attribute_data->name, " name %d ",byindex/4+page*9);
	pPicAttFst->DataArray.push_back(Attribute_data);
	}
	}
	else if(byindex % 4 == 3)//TEXT_SIGN
	{
	continue;
	}
	else if(byindex % 4 == 0)//RECT_INDICATOR_SIGN
	{
		PPIC_ATTRIBUTE_DATA_FST Attribute_data = new PIC_ATTRIBUTE_DATA_FST;

		Attribute_data->wPntNo = (byindex/4)-1+page*9;
		if(Attribute_data->wPntNo == 0 || Attribute_data->wPntNo == 1)
		{
			Attribute_data->wBusNo = 0;
			Attribute_data->wAddr = 1;
		}
		else
		{
			Attribute_data->wPntNo = Attribute_data->wPntNo - 2;
			Attribute_data->wBusNo = 1;
			Attribute_data->wAddr = 1;
		}
		Attribute_data->byAttributeDataByte = 1;
		Attribute_data->byDataType = PIC_ATTRIBUTE_DATA_FST::YX_DTYPE;

		pPicAttFst->DataArray.push_back(Attribute_data);
	}
	else
	{
		Q_ASSERT(FALSE);
		return FALSE;
	}
	m_objAllElementAttribute.push_back(pPicAttFst);
}

}
*/
return TRUE;
}/*}}}*/

BOOL Widget::ReadBindData(PPIC_ATTRIBUT_FST pPicAttFst)
{/*{{{*/
	char sBingDataNoIni[ 64 ] = "";
	sprintf(sBingDataNoIni, "%sBindData%03d.ini", BINDDATANO_PATH, pPicAttFst->wObjectID);
	CProfile Profile(sBingDataNoIni);
	WORD wObjBindDataNum = 0;

	wObjBindDataNum = (WORD)Profile.GetProfileInt((char *)"BINDNUM", (char *)"NUM", 0);
//	printf("%s %d #### wObjBindDataNum = %d\n", __func__, __LINE__, wObjBindDataNum);
	if(wObjBindDataNum == 0)
	{
		printf("%s DataNum is %d \n ", sBingDataNoIni,wObjBindDataNum);
		return TRUE;
	}

	for(BYTE byBindDataNo = 0; byBindDataNo < wObjBindDataNum; byBindDataNo++)
	{
		int iNum;
		char sbuffer[ 200 ] = { 0 };
		int sizebuff = sizeof(sbuffer);
		memset(sbuffer, 0, sizebuff);

		char sBindId[ ] = "bindid";
		char sDataType[ ] = "datatype";
		char sDataNum[ ] = "datanum";
		char sBusNum[ ] = "busnum";
		char sAddr[ ] = "addr";
		char sPoint[ ] = "point";
		//start
		//	char sRelanum[] = "relanum";
		//end


		char sSect[16] = "";
		sprintf(sSect, "BIND%03d",byBindDataNo+1);

		iNum = Profile.GetProfileInt(sSect, sBindId, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sBingDataNoIni,sSect,sBindId);
			return FALSE;
		}
		if(iNum != pPicAttFst->wObjectID)
		{
			printf("Get %s %s bindid:%d!=BindData%d sPicId Failed ! \n ", 
					sBingDataNoIni, sSect,iNum,pPicAttFst->wObjectID);
			return FALSE;
		}

		PPIC_ATTRIBUTE_DATA_FST Attribute_data = new PIC_ATTRIBUTE_DATA_FST;

		iNum = Profile.GetProfileInt(sSect, sDataType, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sBingDataNoIni,sSect,sDataType);
			return FALSE;
		}
		Attribute_data->byDataType = iNum;

		iNum = Profile.GetProfileInt(sSect, sDataNum, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sBingDataNoIni,sSect,sDataNum);
			return FALSE;
		}
		Attribute_data->byAttributeDataByte = iNum;

		iNum = Profile.GetProfileInt(sSect, sBusNum, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sBingDataNoIni,sSect,sBusNum);
			return FALSE;
		}
		Attribute_data->wBusNo = iNum;

		iNum = Profile.GetProfileInt(sSect, sAddr, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sBingDataNoIni,sSect,sAddr);
			return FALSE;
		}
		Attribute_data->wAddr = iNum;

		iNum = Profile.GetProfileInt(sSect, sPoint, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sBingDataNoIni,sSect,sPoint);
			return FALSE;
		}
		Attribute_data->wPntNo = iNum;
#if 0
		//start				不应该在对象里添加!
		iNum = Profile.GetProfileInt(sSect, sRelanum, -1);
		if(iNum == -1)
		{
			printf("Get %s %s %s Failed ! \n ", sBingDataNoIni,sSect,sPoint);
			return FALSE;
		}
		Attribute_data->relanum = iNum;
		//end
#endif
		pPicAttFst->DataArray.push_back(Attribute_data);
	}
	//start
	//	RelateYcYk(pPicAttFst, wObjBindDataNum);
	//end
	return TRUE;
}/*}}}*/

#if 0
//start
BOOL Widget::RelateYcYk(PPIC_ATTRIBUT_FST pPicAttFst, WORD wObjBindDataNum)
{/*{{{*/
	//如果在BindDatax.ini中添加一个字段，表示遥测相关的遥控的datanum似乎就简单了!
	WORD count = pPicAttFst->DataArray.size();
	for(WORD i = count - wObjBindDataNum; i < count; i++){
		if(pPicAttFst->DataArray.at(i)->byDataType == 4){
			for(WORD j = count - wObjBindDataNum; j < count; j++){
				if(pPicAttFst->DataArray.at(i)->relanum == pPicAttFst->DataArray.at(j)->datanum){
					//读alramx.ini文件，以前认为alarmx.ini中只能存一个遥控信息，但是如果我一页下存在多个遥测图元且每一个都关联了则无法通过alarmx.ini的编号x来区分所以必须允许存储多个元素!
					//找到alarmx.ini中对用元素是现阶段面临的主要矛盾!
					for(BYTE m = 0; m < m_objPageElement.size(); m++){
						if(m_objPageElement.at(m).wPageNo)
					}
					break;
				}
			}
		}
	}
}/*}}}*/
//end
#endif

/*******************************************************************************
 * 类:Widget
 * 函数名:InitDBase
 * 功能描述:初始化数据库
 * 参数:void
 * 返回值:void
 ******************************************************************************/
BOOL Widget::InitDBase(void)
{/*{{{*/
	CDBaseManager *pModel = NULL;
	m_pDBase = new CDBaseManager;
	if (NULL == m_pDBase)
	{
		printf("dbase new error\n");
		return FALSE;
	}

	pModel = m_pDBase->Init();
	if(NULL == pModel)
	{
		printf("dbase model init error\n");
		return FALSE;
	}

	m_pDBase->m_pMoudle = pModel;

	return TRUE;
} /*-------- end class Widget method InitDBase -------- *//*}}}*/

/*******************************************************************************
 * 类:Widget
 * 函数名:DeleteDBase
 * 功能描述:关闭数据库
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void Widget::DeleteDBase(void)
{/*{{{*/
	if(NULL != m_pDBase)
	{
		delete m_pDBase;
		m_pDBase		= NULL;
	}
} /*-------- end class Widget method DeleteDBase -------- *//*}}}*/
