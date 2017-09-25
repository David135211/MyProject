// PintTest.h: interface for the CPintTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PINTTEST_H__90ECF2C5_8928_41E7_A471_A56514CF8119__INCLUDED_)
#define AFX_PINTTEST_H__90ECF2C5_8928_41E7_A471_A56514CF8119__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "typedef.h"

#define IP_HSIZE sizeof(struct iphdr)   /*����IP_HSIZEΪipͷ������*/
#define IPVERSION  4   /*����IPVERSIONΪ4��ָ����ipv4*/
#define ICMP_HSIZE sizeof(struct icmphdr)
#define BUFSIZE 1500    /*���ͻ������ֵ*/
#define DEFAULT_LEN 56  /*ping��Ϣ����Ĭ�ϴ�С*/

//ICMP��Ϣͷ��
struct icmphdr {
    BYTE type;     /*������Ϣ����*/
    BYTE code;    /*������Ϣ����*/
    WORD checksum;   /*����У��*/

	WORD wRequestID ; //����ID��
	WORD wSequence ; //���к�
	DWORD timeStamp ; //ʱ��
	icmphdr( )
	{
		type = 0 ;
		code = 0 ;
		checksum = 0 ;
		wRequestID = 0 ;
		wSequence = 0 ;
		timeStamp = 0 ;
	}
};

struct iphdr {
    BYTE hlen:4, ver:4;   /*����4λ�ײ����ȣ���IP�汾��ΪIPV4*/
    BYTE tos;     /*8λ��������TOS*/
    WORD tot_len;    /*16λ�ܳ���*/
    WORD id;         /*16λ��־λ*/
    WORD frag_off;   /*3λ��־λ*/
    BYTE ttl;         /*8λ��������*/
    BYTE protocol;    /*8λЭ��*/
    WORD check;      /*16λIP�ײ�У���*/
    DWORD saddr;      /*32λԴIP��ַ*/
    WORD daddr;      /*32λĿ��IP��ַ*/
};

class CPintTest
{
public:
	CPintTest();
	virtual ~CPintTest();
	BOOL Ping( char * pDestIp ) ;

protected:
	void InitPing( ) ;
	BOOL PackICMP( char * pDestIP ) ;
	BOOL SendICMP( char * pDestIp );
	BOOL RecvICMP( char * pDestIp );
	WORD checksum( BYTE *buf,int len)  ;
	icmphdr *m_pICMP_hdr ;
	BYTE m_sbuf[ BUFSIZE ] ;
	WORD m_wRequestID ;
	HANDLE m_socket ;
	int m_dataLen ;
};

#endif // !defined(AFX_PINTTEST_H__90ECF2C5_8928_41E7_A471_A56514CF8119__INCLUDED_)
