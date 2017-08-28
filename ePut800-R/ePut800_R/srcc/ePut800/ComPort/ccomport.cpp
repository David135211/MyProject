#include "ccomport.h"
#include "structure.h"

CComPort::CComPort()
{
}

CComPort::~CComPort()
{
}

BOOL CComPort::IsPortValid ( void )
{
	return FALSE;
}

BOOL CComPort::OpenPort ( char* lpszError )
{
	return FALSE;
}

void CComPort::ClosePort ( void )
{
	return ;
}


int CComPort::ReadPort ( BYTE *pBuf , int nRead )
{
	return 0;
}

int CComPort::WritePort ( BYTE *pBuf  , int nWrite )
{
	return 0;
}


int CComPort::AsyReadData ( BYTE *pBuf  , int nRead )
{
	return 0;
}


int CComPort::AsySendData ( BYTE *pBuf , int nWrite )
{
	return 0;
}

BOOL CComPort::SetPreOpenPara( Property * pProperty )
{
    return FALSE ;
}

