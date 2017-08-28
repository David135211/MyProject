#ifndef MODBUSTCP_H
#define MODBUSTCP_H

#include "../../ePut800/Proto/cprotocol.h"
#include "../../ePut800/Proto/cmethod.h"
#include "../../ePut800/structure.h"
#include "ModBusTcp_global.h"
#include <stdio.h>
#define	MODBUSTCPPREFIXFILENAME			"./config/ModBusTcp/"
typedef struct _tagMAPITEM
{
    WORD   wBusNo;
    WORD   wAddr;
    WORD   wPntNum;
} MAPITEM;

class MODBUSTCPSHARED_EXPORT CModBusTcp  : public CProtocol
{
public:
    CModBusTcp( CMethod * pMethod );
    virtual ~CModBusTcp();

    virtual WORD GetCRC(BYTE *pBuf,WORD nLen){ return 0 ;};

    virtual BOOL Init( );
    virtual BOOL AddModule(WORD wBusNo , WORD wAddr , WORD wModule, char * stemplate);
    virtual BOOL IsProtoValid() { return TRUE ;}
    virtual WORD GetProtoNo () { return PROTO_MODBUS ; } // proto serialno
    virtual BYTE GetProtoType(){ return BUS_GATHER ; }  //proto type gather or transmit
    virtual WORD GetProtoAddr( ){ return m_wAddr ;}
    void ReadMapConfig(char* lpszFile);

    enum{ERROR_FUN=1,ERROR_REGISTER,ERROR_DATA,ERROR_DEVSTATE};
    enum{YC_NUM=1,YX_NUM,YM_NUM,YK_NUM,DZ_NUM,YC_DEAD,YC_PROPTY,TIMING
         ,YC_PROPERTY,YX_PROPERTY,YM_PROPERTY,YK_PROPERTY,DZ_PROPERTY};

    /*数据发送配置参数*/
    WORD  m_wAISum;
    WORD  m_wDISum;
    WORD  m_wPISum;
    WORD  m_wDOSum;
    WORD  m_wAOSum;
    WORD  m_wSignSum;
    WORD m_wDZSum ;
    WORD*    m_pwCITrans;
    MAPITEM* m_pAIMapTab; WORD* m_pwAITrans;
    MAPITEM* m_pDIMapTab; WORD* m_pwDITrans;
    MAPITEM* m_pPIMapTab; WORD* m_pwPITrans;
    MAPITEM* m_pDOMapTab; WORD* m_pwDOTrans;
    MAPITEM* m_pAOMapTab; WORD* m_pwAOTrans;
    MAPITEM* m_pDZMapTab; WORD * m_pwDZTrans ;
    WORD    m_wDeadVal;      /*遥测死区值*/
    WORD    m_wRipeVal;      /*遥测发送类型: 0=二次值  1=一次值*/
    WORD    m_wRecvClock;    /*接受对时标志*/

    int GetMapItem(char *strItem, WORD *pwNum);
    void GetMapParam(char* strParam,
                  WORD *pwVal1, WORD *pwVal2, WORD *pwVal3, WORD *pwVal4, WORD *pwVal5);
    int ParseMapLine(char* strLine, WORD *pwNum,
                              WORD *pwVal1, WORD *pwVal2, WORD *pwVal3, WORD *pwVal4, WORD *pwVal5);
    void CreateTransTab();
};

#endif // MODBUSTCP_H
