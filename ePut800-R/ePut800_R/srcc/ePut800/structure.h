#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "./Typedef.h"
#include "./ComPort/ccomport.h"
#include "./Proto/cprotocol.h"
#define TOTAL_BUS     15 //第15条总线配置软件中不可陪，当作ePUT800本身（已将主备电状态以遥信上传）

//工程路径
#define BUS_PATH	"./config/BusLine.ini"
#define PIC_PATH	"./config/Pic.ini"
#define PICNO_PATH	"./config/Pics/"
#define OBJECT_PATH	"./config/Objects.ini"
#define BINDDATANO_PATH	"./config/Pics/"
#define STATIONNO_PATH	"./config/Station/"
#define CABLE_PATH	"./config/Cable.ini"
#define CIRCUITNO_PATH	"./config/Cable/"
#define LOOPNO_PATH	"./config/Cable/"
#define ALARM_PATH "./config/Pics/"


#pragma pack( 1 )

class CMsg ;

//class CProtocol ;
//class CComPort ;
typedef struct sSYSPAGE
{
	BYTE byPageNo;
	QELEMENT_ARRAY m_EleArray ;
	char strPicTitle[ 50 ] ;
	//start
	char pWall_SysPage[64];
	//end
	/*lel*/
	BYTE pWareHome;
	/*end*/
	QPIC_NODE_TO_ID_MAP m_AllElementNodeToPicId_Map;//save all element Node to Id
	QPIC_NODE_TO_ID_ARRAY m_PowerElementNodeToPicId_Array;//save Power element Node to Id
	sSYSPAGE( ):m_EleArray( 0 )
	{
		memset( strPicTitle , 0, sizeof( strPicTitle ) );
		//start
		memset( pWall_SysPage , 0, sizeof( pWall_SysPage ) );
		//end
	}
}ST_SYSPAGE,*PST_SYSPAGE;

//start
class Alarm{						//报警图元信息
	public:
		BYTE pageid;				//alarmx.ini编号!
		BYTE objid;					//对象id!
		BYTE id;					//自身编号!
		BYTE busno;					//遥控总线号
		BYTE devaddr;				//地址号
		WORD point;					//点号
//		WORD relanum;				//无意义
		WORD reservation;			//无意义
		int toppole;				//报警上上限
		int top;					//报警上限
		int valuetop;				//值上限
		int valuebottom;			//值下限
		int bottom;					//报警下限
		int bottompole;				//报警下下限
		/*
		   BYTE picid;					//表征在哪一个picx.ini下，一个picx.ini就是一个page! 等于picx.ini中的picId.
		   BYTE ownid;					//表征自己(报警图元)在picx.ini中的序号.相当于picx.ini中的id!
		   BYTE related;				//和其关联的图元的序号,相当于picx.ini中的id。只有遥测图元才有用!

		   BYTE objid;					//对象ID!	相当于picx.ini中的LinkObjectId.
		   BYTE objrelated;			//和去关联的遥测在BindDatax.ini中的序号即datanum!
		   BYTE datanum;				//关联的遥控在BindDatax.ini中的序号		datanum和objid再加上数据类型2就可以确定BindDatax.ini中唯一的遥测

		   BYTE checkbox;				//关联checkbox!				完全可以使用id表征!
		   */
};
//end

//define file structure
typedef struct _ELE_FST
{
	WORD wObjectID ; //exclusive ID
	WORD wElementType ;  //element Type
	WORD wPageNo ; // index of page which owns pic element itself
	WORD wLinkObjectId;
	WORD wDirect ;
	WORD wLeft ;
	WORD wTop ;
	WORD wRight ;
	WORD wBottom ;
	char text[50];
	/*lel*/
	char picname[50];
	char picintroduce[50];
	WORD wPrimitiveType;
	/*end*/
	//start
	Alarm RelaAla;								//关联告警!
	WORD relanum;								//picx.ini中元素最后一个字段，表示alarmx.ini中元素序号!
	//end
	_ELE_FST( )
	{
		wObjectID = -1 ;
		wElementType = -1 ;
		wLeft = -1 ;
		wTop = -1 ;
		wRight = -1 ;
		wBottom = -1 ;
		wPageNo = -1 ;
		wLinkObjectId = -1 ;
		wDirect = -1 ;
		memset(text,0,sizeof(text));
		//start
	//	memset(&RelaAla, 0, sizeof(Alarm));
		//end
	}
}ELE_FST,*PELE_FST ;

typedef struct _PAGE_ST
{
	_PAGE_ST( )
	{
		memset( pPageTitle , 0 , sizeof( pPageTitle ) );
		//start
		memset(pWallName, 0, sizeof(pWallName));
		//end
		wEleNumber = -1 ;
		wPageNo = -1 ;
	}

	char pPageTitle[ 50 ] ;
	//start
	char pWallName[64];							//保存墙纸路径、文件名!
	//end
	/*lel*/
	BYTE pWareHome;
	/*end*/
	WORD wEleNumber ; // number of elements
	WORD wPageNo ;
	QELEMENT_FILEST_ARRAY ElementArray ;
}PAGE_FST , *PPAGE_FST;

//End define file structure

typedef struct _tagREALTIME {
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliSec;
	_tagREALTIME( )
	{
		wYear = 0 ;
		wMonth = 0 ;
		wDayOfWeek = 0 ;
		wDay = 0 ;
		wHour = 0 ;
		wMinute = 0 ;
		wSecond = 0 ;
		wMilliSec = 0 ;
	}
} REALTIME,*PREALTIME ;

typedef struct tagTHREAD_STRUCT
{
	THREAD_PROC ThreadProc ;
	pthread_t tid;
	CMsg *pMsg ;
	tagTHREAD_STRUCT( )
	{
		tid = -1 ;
		ThreadProc = NULL ;
		pMsg = NULL ;
	}
}STHREAD_PROC, *PSTHREAD_PROC;



typedef struct UsrPwd
{
	char Usr[32];
	char Pwd[32];
	UsrPwd()
	{
		memset(Usr,0,sizeof(Usr));
		memset(Pwd,0,sizeof(Pwd));
	}
}USRPWD,*PUSRPWD;

typedef struct NetParam
{
	char ip[16];
	char mask[16];
	char gateway[16];
	char dns[16];
	char name[16];
	NetParam()
	{
		memset(ip,0,sizeof(ip));
		memset(mask,0,sizeof(mask));
		memset(gateway,0,sizeof(gateway));
		memset(dns,0,sizeof(dns));
		memset(name,0,sizeof(name));
	}
}NETPARAM,*PNETPARAM;


typedef struct tagMsg
{
	tagMsg( )
	{
		msgType = 0 ;
		pVoid = NULL ;
		msgSize = sizeof( void * ) + sizeof( int ) ;
	}

	long msgType ; // use inside
	void * pVoid ;
	int msgSize ; //msgSize 为wMsgAutoType pVoid msgSize的大小总和

}LMSG, *PLMSG ;

typedef struct MSG_Struct
{
	MSG_Struct( )
	{
		msgCustomType = 0 ;
	}
	virtual ~MSG_Struct(){ }

	DWORD msgCustomType ;
	// define All msgType from here in order
	enum{ MSG_UPD_LOGINTIME = 0x9000 ,
		MSG_YK
	} ;
}SMSG , *PSMSG;

typedef struct _YK_Struct : public SMSG
{
	_YK_Struct( )
	{
		msgCustomType = MSG_YK ;
		byAction = YK_NULL ;
		wBusNoDest = -1 ;
		wAddrDest = -1 ;
		wPntNoDest = -1 ;

		wBusNoSrc = -1 ;
		wAddrSrc = -1 ;
		wPntNoSrc = -1 ;

		wVal = -1 ;
	}

	WORD wBusNoSrc;        //logical busNo
	WORD wAddrSrc;
	WORD wPntNoSrc;

	WORD wBusNoDest ;
	WORD wAddrDest ;
	WORD wPntNoDest ;

	BYTE byAction ;
	WORD wVal ;
}YKST_MSG,*PYKST_MSG;

typedef struct _UpDateLoginTime : public SMSG
{
	_UpDateLoginTime( )
	{
		msgCustomType = MSG_UPD_LOGINTIME ;
	}

	~_UpDateLoginTime( )
	{

		msgCustomType = 0 ;
	}

}UPD_LOGTime,*PUPD_LOGTime;

struct Property
{
	Property( )
	{
		m_ComType = COM_NULL ;
		wPortNo = -1 ;
		wBPortNo = -1 ;
		pComPort = NULL ;
	}
	virtual ~Property(){ }
	enum ComType{ COMRS485 = 0x11 , LAN_SERVER , LAN_CLIENT , PAUSETYPE , COM_NULL } ;
	ComType m_ComType ;
	WORD wPortNo ;
	WORD wBPortNo ; //backup port
	CComPort * pComPort ;    //association hardware device drive
};

struct LAN_SERVER_Property : public Property
{
	LAN_SERVER_Property( )
	{
		m_ComType = LAN_SERVER ;
		wPortNo = -1 ;
		wBPortNo = -1 ;
		memset( serverIp , 0 , sizeof( serverIp ) );
	}

	virtual ~LAN_SERVER_Property( )
	{
		if( pComPort )
		{
			delete pComPort ;
			pComPort = NULL ;
		}
	}

	char serverIp[32] ;
};

struct LAN_CLIENT_Property : public Property
{
	LAN_CLIENT_Property( )
	{
		m_ComType = LAN_CLIENT ;
		wPortNo = -1 ;
		wBPortNo = -1 ;
		memset( clientIp , 0 , sizeof( clientIp ) );
	}

	virtual ~LAN_CLIENT_Property( )
	{
		if( pComPort )
		{
			delete pComPort ;
			pComPort = NULL ;
		}
	}

	char clientIp[32] ;
};

struct COM_Property : public Property
{
	COM_Property( )
	{
		m_ComType = COMRS485 ;
		wPortNo = -1 ;
		wBPortNo = -1 ;
		wBaud = 9600 ;
		byDataBits = 8 ;
		byStopBits = 1 ;
		byParity = 'n' ;
	}
	~COM_Property()
	{

	}
	DWORD wBaud ;
	BYTE  byDataBits ;
	BYTE  byStopBits ;
	BYTE  byParity ;
};

typedef struct tagDataRoot
{
	BYTE byDataType ;
	WORD wBusNo;        //logical busNo
	WORD wAddr;
	WORD wPntNo;
	WORD wTransNo[ TOTAL_BUS ];
	char chName[ 50 ];
	enum DATA_TYPE{ YC_DTYPE=1 , YX_DTYPE , YM_DTYPE , YK_DTYPE};
}DataType,*PDataType ;

typedef  DataType::DATA_TYPE DATETYPE ;

/*遥信数据*/
typedef struct tagYX_STRUCT :public DataType
{
	tagYX_STRUCT()
	{
		byDataType = DataType::YX_DTYPE ;
		memset( chName , 0 , sizeof( chName ) ) ;
		byValue = 0 ;
		byReverse = 0 ;
		byWarn = 0 ;
		wBusNo = -1 ;
		wAddr = -1 ;
		wPntNo = -1 ;
		memset( wTransNo , 0 , sizeof( wTransNo ) ) ;
	}

	//char chName[ 40 ];
	BYTE byValue ;
	BYTE byReverse ;
	BYTE byWarn ; // warn or no warn

}YXST,*PYXST;

typedef struct tagYcOverThresd
{
	tagYcOverThresd()
	{
		fValue = 0 ;
		wBusNo = 0 ;
		wAddr = 0 ;
		wPntNo = 0 ;
		byYxval = 0 ;
	}

	float fValue; //Yc overflow val
	WORD wBusNo;
	WORD wAddr;
	WORD wPntNo;
	BYTE byYxval ;

}YCOVERTHRESD;

/*遥测数据*/
typedef struct tagYC_STRUCT:public DataType
{
	tagYC_STRUCT()
	{
		byDataType = DataType::YC_DTYPE ;
		memset( chName , 0 , sizeof( chName ) );
		wBusNo = -1 ;
		wAddr = -1 ;
		wPntNo = -1 ;
		fValue = 0 ;
		fBaseValue = 0 ;
		fCoefficient = 0.1 ;
		fSendBaseValue = 0 ;
		fSendCoefficient = 1 ;
		fThv = 0 ;
		byWarn = 0 ;
		memset( wTransNo , 0 , sizeof( wTransNo ) ) ;
		fVal_1Time = 0 ;
	}

	//char	chName[ 40];
	float	fValue;  //original value
	float	fBaseValue;
	float	fCoefficient;
	float	fSendBaseValue;   // transmit baseVal
	float	fSendCoefficient; // transmit coef use multiply by fValue
	float	fThv;             // threshold
	float   fVal_1Time;       // 1 times vals

	BYTE	byWarn;         // warn or not
	YCOVERTHRESD    fLowValue ;
	YCOVERTHRESD    fLowerValue ;
	YCOVERTHRESD    fHighValue ;
	YCOVERTHRESD    fHigherValue ;

}YCST,*PYCST;

typedef struct tagYM_STRUCT : public DataType
{
	tagYM_STRUCT( )
	{
		byDataType = DataType::YM_DTYPE ;
		memset( chName , 0 , sizeof( chName ) ) ;
		dwValue = 0 ;
		wBusNo = 0 ;
		wAddr = 0 ;
		wPntNo = 0 ;
		fBaseValue = 0.0 ;
		fCoefficient = 1 ;
		memset( wTransNo , 0 , sizeof( wTransNo ) ) ;
		dVal_1Time = 0 ;
	}


	DWORD dwValue; // original value
	double dVal_1Time  ;// 1Times Val
	float fBaseValue;
	float fCoefficient;
} YMST,*PYMST;

typedef struct tagYK_STRUCT : public DataType
{
	tagYK_STRUCT( )
	{
		byDataType = DataType::YK_DTYPE ;
		wBusNo = -1 ;
		wAddr = -1 ;
		wPntNo = -1 ;
		memset( wTransNo , 0 , sizeof( wTransNo ) ) ;
	}
}YKST , *PYKST;

typedef struct tagDevst
{
	tagDevst( )
	{
		wBusNo = -1 ;
		wAddr = -1 ;
		wDevState = COMM_ABNORMAL ;
		wYx = 0 ;
		wYc = 0 ;
		wYm = 0 ;
		wYK = 0 ;
		ycArray.clear() ;
		yxArray.clear();
		ymArray.clear() ;
		wProtoModuleNo = -1 ;
		memset(stemplate,0,sizeof(stemplate));
	}
	~tagDevst()
	{
		ycArray.clear();
		yxArray.clear();
		ymArray.clear();
	}

	WORD wBusNo ; //dev work in busline buffer
	WORD wAddr ; // dev addr
	WORD wProtoModuleNo ;
	WORD wDevState ;  //communication state
	char stemplate[64];
	WORD wYx;
	WORD wYc;
	WORD wYm;
	WORD wYK ;
	QDEV_DATA_ARRAY ycArray ;
	QDEV_DATA_ARRAY yxArray ;
	QDEV_DATA_ARRAY ymArray ;
	QDEV_DATA_ARRAY ykArray ;
}DEVST ,*PDEVST;

typedef struct _BUS_LINE
{
	_BUS_LINE()
	{
		wBusNo = -1 ;
		memset( szBusName , 20 , sizeof( szBusName ) ) ;
		pProto = NULL ;
		wInterval = 10 ;
		wTotalDev = 0 ;
		DevArray.clear() ;
		pProperty = NULL ;
		byBusType = BUS_PAUSE ;
		wBusCommState = COMM_ABNORMAL ;
		wPropertyType = Property::COM_NULL ;
		wProtoType = PROTO_NULL ;
		pThreadProc = NULL ;
		memset( ProtodllPath , 0 , sizeof( ProtodllPath ) ) ;
		dwSendNo = 0 ;
		dwRecvNo = 0 ;
	}

	WORD wBusNo ;    // logical portNo
	char szBusName[20] ;
	BYTE byBusType ; // gather or tansmit
	WORD wInterval ;  //circulation time
	WORD wTotalDev ;  //total dev
	WORD wBusCommState ; //communication state
	char m_BusString[ 200 ] ;
	PSTHREAD_PROC pThreadProc ; //store parameters about thread
	QDEV_ARRAY DevArray ;

	Property * pProperty ;
	WORD wPropertyType ;

	WORD wProtoType ;
	CProtocol * pProto ; //association protocol
	char ProtodllPath[200];

	//sendNo recvNo
	DWORD dwSendNo , dwRecvNo ;

}BUS_LINEST , *PBUS_LINEST;

typedef struct _CABINET
{
	_CABINET()
	{
		memset( sName , 0 , sizeof( sName ) );
		wSerialNo = -1 ;
	}
	~_CABINET( )
	{
		circuitArray.clear();
	}

	QSCIRCUIT_MAP circuitArray ;
	char sName[ 100 ] ;
	WORD wSerialNo ;    //exclusiveNo
}CABINET , *pCABINET;

typedef struct _SINGALCIRCUIT
{
	_SINGALCIRCUIT( )
	{
		ycArray.clear();
		yxArray.clear();
		ymArray.clear();
		memset( sName , 0 , sizeof( sName ) ) ;
		wSerialNo = -1 ;
	}
	~_SINGALCIRCUIT( )
	{
		ycArray.clear();
		yxArray.clear();
		ymArray.clear();
	}

	char sName[ 100 ] ;
	WORD wSerialNo ;    //exclusiveNo
	QDEV_DATA_ARRAY ycArray ;
	QDEV_DATA_ARRAY yxArray ;
	QDEV_DATA_ARRAY ymArray ;
}SINGLECIRCUIT,*PSINGLECIRCUIT;


typedef struct _Pic_Attribute_Data_ST
{
	_Pic_Attribute_Data_ST( )
	{
		wBusNo = -1;
		wAddr = -1;
		wPntNo = -1;
		byAttributeDataByte = -1;
		byDataType = -1;
		memset(name,0,sizeof(name));
		//start
		memset(&RelaYk, 0, sizeof(RelaYk));
		//end
	}

	WORD wBusNo;        //logical busNo
	WORD wAddr;
	WORD wPntNo;
	BYTE byAttributeDataByte;
	BYTE byDataType ;
	char name[50];
	//start
//	BYTE relanum;					//表征BindDatax.ini中和遥控关联的遥测的datanum;
	Alarm RelaYk;
	//end
	/*lel*/
	Alarm RelaYk1;					//为了将温湿度添加到一个对象上，不得已而为之。
	/*end*/
	enum ATTRIBUTE_DATA_TYPE{ YC_DTYPE = 1 , YX_DTYPE , YM_DTYPE , YK_DTYPE};
}PIC_ATTRIBUTE_DATA_FST , *PPIC_ATTRIBUTE_DATA_FST;

typedef struct _Pic_Attribute_Data_Element : public PIC_ATTRIBUTE_DATA_FST
{
	_Pic_Attribute_Data_Element( )
	{
		dbVal = 0.0;
	}
	double dbVal;
}PIC_ATTRIBUTE_DATA_ELEMENT , *PPIC_ATTRIBUTE_DATA_ELEMENT;

typedef struct _Pic_Attribute_ST
{
	_Pic_Attribute_ST( )
	{
		wObjectID = -1;
		DataArray.clear();
	}
	~_Pic_Attribute_ST( )
	{
		DataArray.clear();
	}

	WORD wObjectID ; //exclusive ID
	WORD wObjectNodeL ;
	WORD wObjectNodeR ;
	char ObjectName[32];
	QPIC_ATTRIBUTE_DATA_FST DataArray ;
}PIC_ATTRIBUTE_FST , *PPIC_ATTRIBUT_FST;

//总线结构
typedef struct tagBusData
{
	tagBusData( )
	{
		memset( m_BusString , 0 , sizeof( m_BusString ) ) ;
		m_BusType = Property::PAUSETYPE ;
		m_BusInterval = 0 ;
		// memset( m_szLocalGateWay , 0 , sizeof( m_szLocalGateWay ) );
		// memset( m_szLocalSubNetMask , 0 , sizeof( m_szLocalSubNetMask ) );
		// memset( m_szLocalDNS , 0 , sizeof( m_szLocalDNS ) ) ;

		memset( m_szPrintNetCard, 0, sizeof( m_szPrintNetCard ) );
		memset( m_szPrintRemoteIp, 0, sizeof( m_szPrintRemoteIp ) );
	}
	char m_BusString[ 200 ] ;
	int  m_BusType ;
	WORD  m_BusInterval ;
	// char    m_szLocalGateWay[24];
	// char    m_szLocalSubNetMask[24];
	// char    m_szLocalDNS[24];
	char   m_szIP[24];
	char m_NetCardName[ 30 ];
	char m_ProtocolDllPath[ 200 ] ;

	//端口打印报文相关  须在BusLine.ini 中配置
	char m_szPrintNetCard[5];    //网卡（eth0 eth1 eth2 eth3）
	char m_szPrintRemoteIp[16];  //打印的远程IP
	DWORD m_dwPrintStartPortNum;
}BUSDATA, *PBUSDATA;

//定义结构
typedef struct tagBusInfo
{
	public:
		tagBusInfo( )
		{
		}
		~tagBusInfo()
		{
			RemoveAll() ;
			//printf( "Bus Line Destruct OK . \n" ) ;
		}

		BOOL AddBusString( PBUSDATA pBusData )
		{
			if( pBusData == NULL )
				return FALSE ;

			m_busData.push_back( pBusData ) ;

			return TRUE ;
		}

		BOOL RemoveAll( )
		{
			int nCount = m_busData.size();
			if( nCount > 0 )
			{
				while(nCount--)
					delete m_busData[nCount];

				m_busData.clear();
			}
			return TRUE ;
		}

	public:
		QVector<PBUSDATA> m_busData ;
}INITBUS , *PINITBUS ;

typedef struct tagThreadPara
{
	int hThread;
	pthread_t ThreadID ;
}THREADPARA , *PTHREADPARA ;

typedef struct tagPicNode
{
	tagPicNode( )
	{
		PicNode = 0;
	}
	BOOL AddTheNodePicId( WORD PicId )
	{
		PicId_Array.push_back( PicId ) ;
		return TRUE ;
	}
	WORD PicNode;
	QVector<WORD> PicId_Array ;
}PICNODE , *PPICNODE ;

#pragma pack( )
#endif  //STRUCTURE_H
