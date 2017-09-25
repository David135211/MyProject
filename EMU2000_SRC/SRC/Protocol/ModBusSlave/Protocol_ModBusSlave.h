// Protocol_ModBusSlave.h: interface for the CProtocol_ModBusSlave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL_MODBUSSLAVE_H__74F71703_47F3_48C8_A555_4BB7B115F202__INCLUDED_)
#define AFX_PROTOCOL_MODBUSSLAVE_H__74F71703_47F3_48C8_A555_4BB7B115F202__INCLUDED_
#pragma pack( 1 )
#include "../../share/Rtu.h"
#include "../../share/CMethod.h"

class CProtocol_ModBusSlave  : public CRtuBase
{
public:
	CProtocol_ModBusSlave();
	virtual ~CProtocol_ModBusSlave();
	virtual BOOL Init( BYTE byLineNo ) ;
	BOOL GetDevData( ) ;
	BOOL ProcessFileData( CProfile &profile );
	BOOL CreateModule( int iModule , char * sMasterAddr , WORD iAddr , char * sName , char * stplatePath );
	//InitMS_Module为初始化ModBusSlave模块选项
	BOOL InitMS_Module( CProtocol_ModBusSlave * pProtocol , int iModule , char * sMasterAddr , WORD iAddr , char * sName , char * stplatePath ) ;
	WORD GetCRC(BYTE *pBuf,WORD nLen);
};
#pragma pack( )
#endif // !defined(AFX_PROTOCOL_MODBUSSLAVE_H__74F71703_47F3_48C8_A555_4BB7B115F202__INCLUDED_)
