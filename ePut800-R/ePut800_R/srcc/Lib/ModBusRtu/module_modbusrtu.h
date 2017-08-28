#ifndef MODULE_MODBUSRTU_H
#define MODULE_MODBUSRTU_H

#include "modbusrtu.h"
#include <vector>
#include <sys/time.h>
using namespace std;

#define MSGERROR (1)
#define MSGTRUE (0)

class CModule_ModBusRtu : public CModBusRtu
{
public:
    CModule_ModBusRtu( CMethod * m_pMethod , WORD wBusNo , WORD wAddr);
    virtual BOOL GetReadBuf( BYTE *pBuf , int &len , BYTE &byType, LPVOID pVoid  );
    virtual BOOL ProcessBuf( BYTE *pBuf , int len , BYTE byType , LPVOID pVoid ) ;
    virtual int GetErrorMsg( BYTE *pBuf , int len );
    virtual WORD GetProtoAddr( ){ return m_wAddr ;}
    virtual BOOL Init( );
    WORD m_wBusNO ;

private:
    //char m_sTemplatePath[ 64 ] ;//模板名字
    vector < INFO > yk_info;
    vector < MODBUSCONF > modbus_conf;

    char ErrorInformation[300];
    char FirstErrorInformation[300];
    BOOL MessageChangeState;
    BYTE MessageChangeByte;

    int pos_flag;
    int line;
    int yk_flag;
    int readval_flag;
    int writeval_flag;
    int val_flag;
    int pos;
    int settime_pos;
    int yk_pos_num;
    int readval_pos_num;
    int writeval_pos_num;
    long last_settime;
    int m_wErrorTimer;
    int m_byPortStatus;
    int timeflag;
    WORD wSrcBusNo;
    WORD wSrcDevAddr;
    WORD wSrcPnt;
    int YkNo;
    BYTE YkVal;
    BYTE YkBuf[256];
    int YkLen;
    BYTE MsgErrorFlag;
    BYTE MsgRegisteAndData[4];

    BYTE ESL411SOEFlag;
    BOOL DevCirFlag;

    //读取配置文件
    int ReadConf(char *filename);
    DWORD Atoh(char *buf);
    void DefaultValConfig(MODBUSCONF * mc);
    void SendBuf( MODBUSCONF modbusconf, BYTE * buf ,int *len );
    int SysLocalTime(MODBUSCONF modbusconf,unsigned char *buffer,int i);
    int TimePackMsecBigEdian(MODBUSCONF modbusconf,unsigned char *buffer,int i,struct tm *p,WORD msec);
    int TimePackMsecLittleEdian(MODBUSCONF modbusconf,unsigned char *buffer,int i,struct tm *p,WORD msec);
    int TimePackEM310(MODBUSCONF modbusconf,unsigned char *buffer,int i,struct tm *p,WORD msec);

    //求值  数据格式、有符号
    float ModBusValue(unsigned char *buffer, int a, MODBUSCONF modbusconf);
    short TwoByteValue(unsigned char *buffer, int a, MODBUSCONF modbusconf);
    int FourByteValue(unsigned char *buffer, int a, MODBUSCONF modbusconf);
    float FloatValue(unsigned char *buffer, int a, MODBUSCONF modbusconf);

    //遥控信号处理
    BOOL GetYKBuffer( BYTE * buf , int &len , PYKST_MSG pBusMsg );
    //发送遥控报文
    void YkSendBuf( MODBUSCONF modbusconf , PYKST_MSG pBusMsg , BYTE * buf ,int *len );


    //遥信处理
    void ModBusYxDeal(unsigned char *buffer,MODBUSCONF modbusconf);
    //遥信安位处理
    void ModBusYxBitDeal(unsigned char *buffer,MODBUSCONF modbusconf);
    //要信一次处理的值
    DWORD ModBusYXTempValue(unsigned char *buffer, int pos, MODBUSCONF modbusconf);
    //遥测处理
    void ModBusYcDeal(unsigned char *buffer,MODBUSCONF modbusconf);
    //遥控处理
    void ModBusYkDeal(unsigned char *buffer,MODBUSCONF modbusconf );
    void ModBusRtuYkDeal(unsigned char *buffer,MODBUSCONF modbusconf );
    //遥脉处理
    void ModBusYmDeal(unsigned char *buffer,MODBUSCONF modbusconf);
    //对时处理
    void ModBusSetTime(unsigned char *buffer,MODBUSCONF modbusconf);

    //SOE处理
    void ModBusSoeDeal(unsigned char *buffer,MODBUSCONF modbusconf);
    void ModBusYZ202SoeDeal(unsigned char *buffer,MODBUSCONF modbusconf);
    void ModBusEM310SoeDeal(unsigned char *buffer,MODBUSCONF modbusconf);
    void ModBusRtuSoeDeal(unsigned char *buffer,MODBUSCONF modbusconf);
};

#endif // MODULE_MODBUSRTU_H
