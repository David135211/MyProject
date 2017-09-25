/*
 * =====================================================================================
 *
 *       Filename:  CSocketFtpFile.h
 *
 *    Description:  ����socket ftp �ļ��Ĵ����� 
 *
 *        Version:  1.0
 *        Created:  2015��09��24�� 11ʱ56��26��
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

#ifndef  CSOCKETFTPFILE_INC
#define  CSOCKETFTPFILE_INC

/* #####   HEADER FILE INCLUDES   ################################################### */
#include "../../share/md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h> 
#include <sys/stat.h> 
#include <stdlib.h>
#include <ftw.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */

#define			FTP_FILE_LIST				(char *)"/mynand/filelist.tmp"			/* ��ʱ���ļ��б� */
#define			FTP_DOWN_CFG_FILE			(char *)"/mynand/downprgm/"				/* �����ļ� */
#define			FTP_DOWN_PRGM_FILE			(char *)"/mynand/downprgm/"				/* �����ļ� */
#define			FTP_DOWN_USER_FILE			(char *)"/myapp/downuser/user.sh"						/* user.sh */


typedef enum _FTP_FILE_TYPE 
{
	FTP_NONE_TYPE,
	FTP_CFG_TYPE,
	FTP_PRGM_TYPE
}FTP_FILE_TYPE;				/* ----------  end of enum _FTP_FILE_TYPE  ---------- */

/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */

/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/*
 * =====================================================================================
 *        Class:  CSocketFtpFile
 *  Description:  �����ļ��Ĵ���
 * =====================================================================================
 */
class CSocketFtpFile
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CSocketFtpFile ();                             /* constructor      */
		~CSocketFtpFile ();                            /* destructor       */

	public:
		/* ====================  Method		   ======================================= */
		//д����
		unsigned int WriteFile ( char *pchFileName, 
				unsigned char *puchBuf,
				int iLen);
		//���ļ�
		unsigned int ReadFile ( char *pchFileName,
				unsigned char *puchBuf, 
				int iLen,
				unsigned int &uiReadpos) ;

		// �����ļ�Ŀ¼
		int CreateDir ( const char *pchDirPath );
		//��ȡ�ļ���С
		unsigned long GetFileSize (  const char *cpchFileName);
		//Ŀ¼�������ļ��������ļ��Ĵ�С
		unsigned int GetDirSize ( const char *pchDirPath );
		//�鿴�ļ��Ƿ����
		bool IsFileExist ( const char *cpchFileName );
		//�鿴�ļ����Ƿ����
		bool IsDirExist ( const char *cpchDirPath );
		//�鿴�Ƿ�������Ŀ¼
		bool IsSpecialDir ( const char *cpchDirPath );
		//��ȡ����·��
		void GetTotalFilePath ( const char *cpchPath, 
				const char *cpchFileName,
				char *cpchFilePath);
		// �޸��ļ�Ȩ��
		bool ChangeFileMode ( char *pszFileName,
				int imode );
		// �޸�Ŀ¼�������ļ�Ȩ��
		bool ChangeDirFilesMode ( char *pchDirPath, 
				int imode );
		//ɾ��Ŀ¼�������ļ�
		bool DeleteDirFiles ( const char *pchDirPath );
		//�ƶ��ļ���Ŀ¼
		bool MoveDirFiles ( char *pchSrcPath, char *pchDirPath );
		//д�ļ����������ļ�
		bool WriteFileListFile (  char *pchDirPath, char *pchFileName );
		//��õ�n������
		char * GetFileLineBuf ( char *pchFileName, 
				int iLine,
				char *pchLineBuf );

	public:
		//����ļ��б� 
		bool CheckFileList (void  );
		//������ص��ļ�����
		FTP_FILE_TYPE GetDownType ( char *pchFileName );
		//������ص��ļ�����
		char * GetDownFileName ( FTP_FILE_TYPE fType,
				char *pchFileName );

		//��������
		bool UpdateCfg ( void );
		//���³���
		bool UpdatePrgm ( void );

	protected:
		/* ====================  DATA MEMBERS  ======================================= */
	private:
		//�����ļ�
		bool BakDirFiles ( char* pchSrcPath,
				char *pchDirPath );
	private:
		/* ====================  DATA MEMBERS  ======================================= */

}; /* -----  end of class CSocketFtpFile  ----- */


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */







#endif   /* ----- #ifndef CSOCKETFTPFILE_INC  ----- */
