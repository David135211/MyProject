/*
 * =====================================================================================
 *
 *       Filename:  CSocketFtp.h
 *
 *    Description:  ����socket ģ��ftp���д�������
 *
 *        Version:  1.0
 *        Created:  2015��09��24�� 11ʱ53��44��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  
 *
 *		  history:
 *
 * =====================================================================================
 */

#ifndef  CSOCKETFTP_INC
#define  CSOCKETFTP_INC


/* #####   HEADER FILE INCLUDES   ################################################### */
#include "../../share/CTcpPortServer.h"
#include "CGroupBroadProto.h"
#include "CSocketFtpProto.h"



/* #####   LOCAL CLASS DEFINITIONS   ################################################ */

/*
 * =====================================================================================
 *        Class:  CSocketFtp
 *  Description:  ����socket ftp����
 * =====================================================================================
 */
class CSocketFtp
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CSocketFtp ();                             /* constructor      */
		~CSocketFtp ();                            /* destructor       */

	public:
		//��ʼ��Э��
		BOOL Init ( void );
		//�߳�
		BOOL CreateThread ( void );

	private:
		//����tcp������
		BOOL CreateTcpServer ( void );
		//����Э��
		BOOL CreateProto ( void );

		//�����鲥Э�鼰�߳�
		BOOL CreateGroupBroad ( void );

	public:
		/* ====================  DATA MEMBERS  ======================================= */
		CTcpPortServer *m_pPort;
		CSocketFtpProto *m_pProto;
		CGroupBroadProto *m_pGroupProto;        /* �M��� */

		pthread_t  m_pthread_id;
		pthread_t  m_groupbroad_id;

	protected:
		/* ====================  DATA MEMBERS  ======================================= */

	private:
		/* ====================  DATA MEMBERS  ======================================= */
		

	private:
		/* ====================  DATA MEMBERS  ======================================= */

}; /* -----  end of class CSocketFtp  ----- */





#endif   /* ----- #ifndef CSOCKETFTP_INC  ----- */
