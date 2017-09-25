/*
 * =====================================================================================
 *
 *       Filename:  CMethod.h
 *
 *    Description:   通讯进程与内存共享空间之间提供公共方法
 *
 *        Version:  1.0
 *        Created:  2014年07月22日 08时27分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (),
 *        Company:  esdtek
 *
 * =====================================================================================
 */

#ifndef  CMETHOD_H_INC
#define  CMETHOD_H_INC


#include	<stdio.h>
#include	<stdlib.h>

#include	"rdbDef.h"
#include	"typedef.h"
#include	"gDataType.h"

class CProtocol  ;
class CRTDBObj ;
class CBusManger;
class  CBasePort ;
struct tagBusManager ;
typedef tagBusManager * PBUSMANAGER ;

/*
 * =====================================================================================
 *        Class:  CMethod
 *  Description:  父类 提供方法
 * =====================================================================================
 */
class CMethod
{
	public:

		/* ====================  LIFECYCLE     ======================================= */
		CMethod ()
		{
			m_pRdbObj = NULL;
			m_pRtuObj = NULL;
			m_pBusManager = NULL;
			m_pPort=NULL;
			return;
		}                             /* constructor      */
		virtual ~CMethod (){return; } 			        /* destructor       */

		/* ====================  ACCESSORS     ======================================= */

		/* ====================  MUTATORS      ======================================= */
		/* ====================  public  method      ======================================= */
		// 通过wSerialNo 获取到总线号和相应地址
		virtual BOOL GetBusLineAndAddr ( WORD wSerialNo, BYTE &byBusNo, WORD &wDevAddr ){return FALSE;}
		// 通过总线号和装置地址号获取 连续号
		virtual int GetSerialNo ( BYTE byBusNo, WORD wDevAddr ){ return -1; }

		/*==================================遥测处理=========================================*/
		virtual	void YcUpdate ( WORD SerialNo, YC_DATA YcData[], UINT YcNum ){return ;}
		//设置处理一个遥测
		virtual void SetYcData ( WORD wSerialNo, WORD wPnt, float fVal ){return ;}
		//设置处理一个带时间的遥测
		virtual	void SetYcDataWithTime ( WORD wSerialNo, WORD wPnt, float fVal, char *pTime  ){return ;}
		// 读取所有的遥测值
		virtual void ReadAllYcData ( float *pData ){return ;}
		//读取共享内存中的一个遥测源码值
		virtual DWORD ReadYcData ( WORD wSerialNO, WORD wPnt ){return 0;}
		/*==================================遥信处理=========================================*/
		virtual void YxUpdate (  WORD SerialNo, YX_DATA YxData[], UINT YxNum){return ;}
		//设置处理一个遥信
		virtual	void SetYxData ( WORD wSerialNo, WORD wPnt, BYTE byVal ){return ;}
		//设置处理一个带时间的遥信
		virtual	void SetYxDataWithTime ( WORD wSerialNo, WORD wPnt, BYTE byVal, TIMEDATA *pTime ){return ;}
		//读取所有的遥信值
		virtual void ReadAllYxData ( BYTE *pData ){return ;}
		//读取共享内存中的一个遥信源码值
		virtual int ReadYxData ( WORD wSerialNO, WORD wPnt, WORD *pwVal){return -1 ;}
		/*==================================遥脉处理=========================================*/
		virtual void YmUpDate ( WORD SerialNo, YM_DATA YmData[], UINT YmNum ){return ;}
		//设置处理一个遥脉
		virtual	void SetYmData ( WORD wSerialNO, WORD wPnt, float fVal ){return ;}
		virtual BOOL GetYmData( WORD wSerialNo, WORD wPnt, DWORD &dwVal ){ return FALSE ;}
		//读取所有电度值
		virtual void ReadAllYmData ( DWORD *pdwData ){return ;}
		//读取共享内存中的一个遥脉源码值
		virtual int ReadYmData ( WORD wSerialNO, WORD wPnt, DWORD *pdwVal ){return -1;}
		/*==================================遥控处理=========================================*/
		//遥控选择返回
		virtual void SetYkSelRtn (const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, WORD wPnt, BYTE byVal ){return ;}
		//遥控执行返回
		virtual void SetYkExeRtn (const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, WORD wPnt, BYTE byVal ){return ;}
		//遥控取消返回
		virtual void SetYkCancelRtn (const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, WORD wPnt, BYTE byVal ){return ;}

		//遥控选择
		virtual void SetYkSel (const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, WORD wPnt, BYTE byVal ){};
		//遥控执行
		virtual void SetYkExe (const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, WORD wPnt, BYTE byVal ){};
		//遥控取消
		virtual void SetYkCancel (const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, WORD wPnt, BYTE byVal ){};
		/*==================================定值处理=========================================*/
		//召唤定值区
		virtual void SetDzZoneCall(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
		//召唤定值区返回
		virtual void SetDzZoneCallRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
		//定值区切换预置
		virtual void SetDzZoneSwitchPreset(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
	 	//定值区切换预置返回
		virtual void SetDzZoneSwitchPresetRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
		//定值区切换执行
		virtual void SetDzZoneSwitchExct(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
		//定值区切换执行返回
		virtual void SetDzZoneSwitchExctRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
		//定值区切换取消
		virtual void SetDzZoneSwitchCancel(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
		//定值区切换取消返回
		virtual void SetDzZoneSwitchCancelRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr, BYTE byDzZoneNo ){};
		//定值区错
		virtual void SetDzZoneError(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo ){};
		//召唤定值
		virtual void SetDzCall(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//召唤定值返回
		virtual void SetDzCallRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//定值写预置
		virtual void SetDzWritePreset(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//定值写预置返回
		virtual void SetDzWritePresetRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//定值写执行
		virtual void SetDzWriteExct(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//定值写执行返回
		virtual void SetDzWriteExctRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//定值写取消;
		virtual void SetDzWriteCancel(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//定值写取消返回
		virtual void SetDzWriteCancelRtn(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};
		//定值错
		virtual void SetDzError(const CProtocol *pProtocol, BYTE byBusNo, WORD wDevAddr,BYTE byDzZoneNo, DZ_DATA DzData[], int iDzDataNum){};

		/* ====================  OPERATORS     ======================================= */
		virtual void CloseSocket( BYTE byBusLine ) { return ; }
		//打开闭网络套接字
		virtual void OpenSocket ( BYTE byBusLine ){;}
		virtual BOOL IsPortValid( ) { return FALSE; } //判断端口是否可用
		virtual BOOL GetDevCommState( BYTE byLineNo , WORD wDevNo ){ return FALSE ; }
		virtual BOOL GetDevCommState( WORD wSerialNo ) { return FALSE ; }
		virtual BOOL GetCommState( BYTE byLineNo ){ return FALSE ; }
		virtual BYTE GetToTalBusNum( ){ return 0 ;}
		virtual BYTE GetDevNum( BYTE byBusNo ){ return 0 ;}
		//通过总线号和模块号得到装置地址
		virtual WORD GetAddrByLineNoAndModuleNo ( BYTE byLineNo, WORD wModuleNo ){return 0;}
		virtual BYTE GetBusLineProtocolType( BYTE byLineNo ){ return 0 ;}
		virtual BOOL IsSoeTime ( UINT uiMilSec, UINT uiSec, UINT uiMin, UINT uiHour, UINT uiDay, UINT uiMonth, UINT uiYear) const{return FALSE;}

		//获取所有采集总线的装置总数（不包括转发总线下所连的装置）
		virtual WORD GetGatherDevCount( ) { return 0 ; }

		//设置所有采集总线的装置总数
		virtual BOOL SetGatherDevCount( WORD wCount ) { return FALSE ;}

		//获取单条采集总线的装置总数
		virtual BYTE GetSingleGatherDevCount( BYTE byBusNo , BYTE byDevIndex = 0 , WORD * pAddr = NULL ) { return 0 ; }
		//获取总线
		virtual PBUSMANAGER GetBus( BYTE byIndex ) { return NULL ;}
	public:

		/* ====================  DATA MEMBERS  ======================================= */
		CRTDBObj *m_pRdbObj;
		CProtocol *m_pRtuObj;
		CBusManger *m_pBusManager;
		CBasePort * m_pPort ;
};


#endif   /* ----- #ifndef CMETHOD_H_INC  ----- */

