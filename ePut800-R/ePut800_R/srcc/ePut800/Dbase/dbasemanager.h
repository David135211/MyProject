/*
 * =====================================================================================
 *
 *       Filename:  dbasemanager.h
 *
 *    Description:  ���ݿ������ ����
 *
 *        Version:  1.0
 *        Created:  2015��03��18�� 14ʱ39��00��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  esdtek
 *
 *		  history:  Time							Author	 Version	Description
 *					2015��03��18�� 14ʱ41��05��	    mengqp   1.0		create
 *
 * =====================================================================================
 */

#ifndef  DBASEMANAGER_INC
#define  DBASEMANAGER_INC
/* #####   HEADER FILE INCLUDES   ################################################### */
#include "../Typedef.h"
#include "dbasedatatype.h"

#define DBASE_SOE_DATATYPE       1  // soe ��������
#define DBASE_YXALARM_DATATYPE       2  // YXALARM ��������

/* �˴����ӹ�����ģ��� */
#define	DBMANAGER_MOUDLE_SOE			1			/* ����SOE���ݿ����ģ�� */
/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */


/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */

/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/*
 * =====================================================================================
 *        Class:  CDBaseManager
 *  Description:
 * =====================================================================================
 */
class CDBaseManager
{
  public:
	/* ====================  LIFECYCLE     ======================================= */
	CDBaseManager ();                             /* constructor      */
	virtual ~CDBaseManager ();                            /* destructor       */
	/* ====================  METHODS       ======================================= */
	//��ʼ��
	CDBaseManager *Init( void );
	// ��ѯ��������
	virtual int SelectAllData( BYTE DataType);
	// ��ѯ�������ݵĵ�һҳ����
	virtual int SelectFirstPageOfAllData(DWORD datanum, BYTE DataType);
	// ��ʱ���ѯ����
	virtual int SelectDataByTime ( char *pszBeginTime, char *pszEndTime, BYTE DataType );
	// ��ʱ���ѯ���ݵĵ�һҳ����
	virtual int SelectFirstPageOfDataByTime ( DWORD datanum, char *pszBeginTime, char *pszEndTime, BYTE DataType );
	// �����ߺ͵�ַ��ѯ����
	virtual int SelectDataByBusLineAndAddr ( WORD wBusLine , WORD wAddr , BYTE DataType);
	// �����ߺ͵�ַ��ѯ���ݵ�һҳ����
	virtual int SelectFirstPageOfDataByBusLineAndAddr ( DWORD datanum,WORD wBusLine , WORD wAddr, BYTE DataType );
	// ��������
	virtual BOOL InsertData ( void *data );
	//������ʷ����
	virtual BOOL UpdateData( void *data );
	// ɾ������
	virtual BOOL DeleteData( void );
	//�ƻ�
	virtual void ClearData( void );

  public:
	// ��ȡ����
	virtual void* GetData ( DWORD pos );

  public:
	/* ====================  DATA MEMBERS  ======================================= */
	char m_szDBPathName[MAX_DBASE_NAME_LEN]; /* ���ݿ�·������ */
	char m_szDBTableName[MAX_DBASE_HISTORY_TABLE_NAME_LEN]; /* ��ʷ���ݱ����� */
	// BYTE m_bySaveDateLen;                   /* �����ʱ�䳤�� */
	BYTE m_byMoudle;  //ģ���  ���Ը���ģ��ţ�ѡ�����Ǹ����ݿ�
	/* ====================  METHODS       ======================================= */
	CDBaseManager *m_pMoudle;               /* ģ��ָ�� */
  protected:
	/* ====================  METHODS       ======================================= */

  private:
	/* ====================  METHODS       ======================================= */
	//��ȡ�����ļ�
	BOOL ReadCfgInfo( void );
	//�������ݿ�ģ��
	CDBaseManager *CreateDBMoudle( void );
	/* ====================  DATA MEMBERS  ======================================= */

}; /* -----  end of class CDBaseManager  ----- */


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */



#endif   /* ----- #ifndef DBASEMANAGER_INC  ----- */
