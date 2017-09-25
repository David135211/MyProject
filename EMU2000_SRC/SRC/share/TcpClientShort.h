// TcpClientShort.h: interface for the CTcpClientShort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENTSHORT_H__1D95CAB6_9303_46D6_BE27_EDD7D0FCE928__INCLUDED_)
#define AFX_TCPCLIENTSHORT_H__1D95CAB6_9303_46D6_BE27_EDD7D0FCE928__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpClient.h"

//1. ��connet
//2. ��read or write
//3. ����ڹر�
//4. ���ڸ���ȥ����˭�����Ѿ����������ú���
class CTcpClientShort : public CTcpClient  
{
public:
	CTcpClientShort();
	virtual ~CTcpClientShort();
	virtual char* ClassName(){return (char *)"CTcpClientShort";}
};

#endif // !defined(AFX_TCPCLIENTSHORT_H__1D95CAB6_9303_46D6_BE27_EDD7D0FCE928__INCLUDED_)
