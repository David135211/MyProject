#include "CTcpPortServer.h"

BOOL CTcpPortServerDelegate::AcceptProc(int hSocket, unsigned short nPort,
		char* lpszRemote, char* lpszLocal)
{
	if( m_pServer )
		m_pServer->ProcessAcceptProc( hSocket , nPort , lpszRemote , lpszLocal ) ;
	else
		CDelegateBase::AcceptProc( hSocket , nPort , lpszRemote , lpszLocal ) ;
	return TRUE ;
}


CTcpPortServer::CTcpPortServer()
{
	//ctor
	m_delegate.m_pServer = this ;
}

CTcpPortServer::~CTcpPortServer()
{
	//dtor
	printf( "TcpPortServer Destruct OK.\n" );
}

BOOL CTcpPortServer::OpenPort( char* lpszError  )
{
	if( lpszError == NULL )
		return FALSE ;

	//给服务器赋值
	m_Listen.m_nThePort = m_uThePort ;
	strcpy( m_Listen.m_szLocalAddr , m_szAttrib ) ;

	//创建服务器
	char szError[ 200 ];
	memset( szError , 0 , sizeof( szError ) );
	m_Listen.CreateObj( szError ) ;
	m_Listen.StartRun( &m_delegate ) ;

	return TRUE ;
}

BOOL CTcpPortServer::ProcessAcceptProc(int hSocket, unsigned short nPort,
		char* lpszRemote, char* lpszLocal)
{
	char szStyle[32];

	sprintf(szStyle, "TCP%d:%s", nPort, lpszRemote);
	LogPromptText("\n***CCtrlLayer::ProcessAccept(%s:%d)***", lpszRemote, nPort);

	Attach( hSocket ) ;
	return TRUE ;
}

BOOL CTcpPortServer::InitPortOtherPara( char * path )
{
	return TRUE ;
}


