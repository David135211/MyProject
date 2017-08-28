#ifndef _CMSG_H__
#define _CMSG_H__
#include "structure.h"
#pragma pack( 1 )
class CMsg
{
public:
        CMsg() ;
        ~CMsg()	;

protected:
        static int m_MsgID ; //通信标示符，唯一代表消息队列
        int m_MsgKey ;//创建消息通道键值
        int m_msgSize ;
public:
        enum MSG_TYPE{ MSG_COM_CHANNEL = 0x1702 ,
                       MSG_COM_END = MSG_COM_CHANNEL + TOTAL_BUS ,
                       MSG_REAL_CHANNEL = 0x1601
                     };
        int  CreateMsgQueue( MSG_TYPE msgType );
        bool SendMsg( void * pVoid ) ;
        bool RecvMsg( void * pVoid ) ;
        bool CloseMsgQueue() ;
        bool IsMsgQueue(  ); //消息队列是否可用
    long m_MsgType ;//消息通道
};
#pragma pack(  )
#endif
