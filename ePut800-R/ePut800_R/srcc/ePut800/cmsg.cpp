#include <stdio.h>
#include <sys/msg.h>
#include <unistd.h>
#include "cmsg.h"
#include "structure.h"

int CMsg::m_MsgID = -1 ;

CMsg::CMsg()
{
  m_MsgKey = 201607 ;// 非法值
  m_MsgType = -1 ;//非法值
  m_msgSize = sizeof( void * ) + sizeof( int ) ;
}

CMsg::~CMsg()
{
  //判断该消息队列是否有消息没处理
  //如果没有消息了就关闭消息通道
  //有消息等待消息处理完，在关闭该消息通道

    if( m_MsgID != -1 )
    {
       if( CloseMsgQueue() )
            printf( "\nCMsg Destructor OK.\n" ) ;
    }

}


int CMsg::CreateMsgQueue( MSG_TYPE msgType )
{
    long msgChannel = ( long )msgType ;
        if( msgChannel <= 0 )
                return -1 ;
        else
                m_MsgType = msgChannel ;

        if( m_MsgID != -1 )
                return m_MsgID ;

        int msgID = -1 ;
        msgID = msgget( m_MsgKey, IPC_CREAT|0666);
        if( msgID != -1 )
                m_MsgID = msgID ;

        return msgID ;
}

bool CMsg::IsMsgQueue(  )
{
        if( m_MsgID == -1 || m_MsgType == -1 )
                return false ;
        else
                return true ;
}

bool CMsg::CloseMsgQueue()
{
        int Return = msgctl( m_MsgID, IPC_RMID, NULL ) ;
        if( Return == 0 )
        {
                m_MsgID = -1;
                printf( "\nClose Msg Queue Success OK. \n" );
                return true ;
        }
        else
                return false ;
}

bool CMsg::SendMsg( void * pVoid )
{
        if( pVoid == NULL || m_MsgID == -1 || !IsMsgQueue() )
                return false ;

        //IPC_NOWAIT 立即返回 非阻塞
        //0 阻塞
        PLMSG  pMsg = ( PLMSG  )pVoid ;
        pMsg->msgType = m_MsgType ;

        int iReturn = msgsnd( m_MsgID, pVoid , m_msgSize , IPC_NOWAIT ) ;
        if( iReturn == 0 )
                return true ;
          else
                return false ;
}

bool CMsg::RecvMsg( void * pVoid )
{
        if( pVoid == NULL || m_MsgID == -1 || !IsMsgQueue() )
                return false ;

        int recv_byte = msgrcv(m_MsgID, pVoid, m_msgSize, m_MsgType , IPC_NOWAIT );
        if( recv_byte <= 0)
        {
            //perror("recvMsg" );
            return false;
        }
        else
                return true;
}
