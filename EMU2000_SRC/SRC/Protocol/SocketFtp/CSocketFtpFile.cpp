/*
 * =====================================================================================
 *
 *       Filename:  CSocketFtpFile.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015��09��24�� 11ʱ56��23��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp 
 *   Organization:  
 *
 *		  history:
 * =====================================================================================
 */
#include <stdio.h>
#include "CSocketFtpFile.h"

static unsigned int gs_uiTotalSize = 0;	
/*
 *--------------------------------------------------------------------------------------
 *      Method:  CalDirSize
 * Description:  ����ÿ���ļ���С������ۼ�Ϊ�����ļ���С ��ftw����
 *       Input:  
 *		Return:
 *--------------------------------------------------------------------------------------
 */
static int CalDirSize ( const char *path, 
		const struct stat *s,
		int flagtype)
{
	// printf ( "�豸��:%llu �ڵ��:%lu ��С:%lu\n",
			// s->st_dev,  s->st_ino, s->st_size );
	//��ÿ���ļ��Ĵ�С���ӣ������˵�(.)�͵��(..)
	gs_uiTotalSize += s->st_size;
	return 0;
}		/* -----  end of method CSocketFtpFile::CalDirSize  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  CSocketFtpFile
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CSocketFtpFile::CSocketFtpFile ()
{
	//��ʼ������
	gs_uiTotalSize = 0;
}  /* -----  end of method CSocketFtpFile::CSocketFtpFile  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  ~CSocketFtpFile
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CSocketFtpFile::~CSocketFtpFile ()
{
}  /* -----  end of method CSocketFtpFile::~CSocketFtpFile  (destructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  WriteFile
 * Description:  ���ļ���д������   �ļ�����׷�ӵķ�ʽд�ģ���д�����ļ�ʱע��ɾ�����ļ�
 *       Input:  pchFileName:�ļ����� 
 *		Return:  д��ĳ��ȣ�д��ʧ�ܻ������ݣ�����0
 *--------------------------------------------------------------------------------------
 */
unsigned int CSocketFtpFile::WriteFile ( char *pchFileName, 
		unsigned char *puchBuf,
		int iLen)
{
	FILE *pFile = NULL;
	unsigned int uiWriteNum;

	// if( NULL == pchFileName ||
			// 0 == iLen)
	// {
		// return 0;
	// }
	//�鿴�ļ�Ŀ¼�Ƿ���ڣ�û���򴴽�
	if( !IsFileExist( pchFileName ) )
	{
		char pchDirPath[256];
		int len = strlen( pchFileName )-  strlen ( strrchr( pchFileName, '/' ));
		strncpy( pchDirPath, pchFileName, len ) ;
		pchDirPath[len] = 0;

		CreateDir( pchDirPath );
	}

	//�Զ�����׷�ӵ���ʽд�ļ�
	pFile = fopen( pchFileName, "ab+" );
	if( NULL == pFile )
	{
		printf ( "pchFileName=%s\n", pchFileName );
		perror( "pszFileName" );
		return 0;
	}

	//����ʵ��д��ķ��ӣ�ûд�뽫����0
	uiWriteNum = fwrite( puchBuf, 1, iLen, pFile  );
	if( 0 == uiWriteNum )
	{
	}

	fclose( pFile );

	return uiWriteNum;
}		/* -----  end of method CSocketFtpFile::WriteFile  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  ReadFile
 * Description:  ���ļ�
 *       Input:  �ļ�����
 *				 ������
 *				 ����
 *				 �ļ�λ��
 *		Return:  �����ĳ��� 
 *--------------------------------------------------------------------------------------
 */
unsigned int CSocketFtpFile::ReadFile ( char *pchFileName,
		unsigned char *puchBuf, 
		int iLen,
		unsigned int &uiReadpos) 
{
	FILE *pFile = NULL;
	int uiReadNum=0;

	if( 0 == puchBuf )
	{
		return 0;
	}

	pFile = fopen( pchFileName, "rb" );
	if( NULL == pFile )
	{
		printf ( "pchFileName=%s\n", pchFileName );
		perror( "pszFileName" );
		return 0;
	}

	if( -1 == fseek( pFile, uiReadpos, SEEK_SET ) )
	{
		fclose( pFile );
		return 0;
	}

	//����ʵ����Ŀ
	uiReadNum = fread( puchBuf, 1, iLen, pFile  );

	uiReadpos += uiReadNum;

	fclose( pFile );

	return uiReadNum;
}		/* -----  end of method CSocketFtpFile::ReadFile  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  CreateDir
 * Description:  �����ļ�Ŀ¼ 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
int CSocketFtpFile::CreateDir ( const char *pchDirPath )
{
	char   DirName[256];  
	strcpy(DirName,   pchDirPath);  
	int   i,len   =   strlen(DirName);  
	if(DirName[len-1]!='/')  
		strcat(DirName,   "/");  

	len   =   strlen(DirName);  

	for(i=1;   i<len;   i++)  
	{  
		if(DirName[i]=='/')  
		{  
			DirName[i]   =   0;  
			if(  access(DirName,  F_OK)!=0    )  
			{  
				if(mkdir(DirName,   0755)==-1)  
				{   
					perror("mkdir   error");   
					return   -1;   
				}  
			}  
			DirName[i]   =   '/';  
		}  
	}  

	return   0;  
}		/* -----  end of method CSocketFtpFile::CreateDir  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  GetFileSize
 * Description:  ��ȡ�ļ���С
 *       Input:  pchFileName:�ļ���
 *		Return:  ��С 0Ϊʧ��
 *--------------------------------------------------------------------------------------
 */
unsigned long CSocketFtpFile::GetFileSize (  const char *cpchFileName)
{
	struct stat statbuff;


	if(stat(cpchFileName, &statbuff) < 0)
	{
		printf ( "%s %lu\n", cpchFileName, statbuff.st_size );
		return 0xffffffff; 
	}

	return statbuff.st_size;
}		/* -----  end of method CSocketFtpFile::GetFileSize  ----- */



/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  GetDirSize
 * Description:  ��ȡ�ļ����������ļ���С ����. ��.. �����ļ��� 
 *       Input:  pchDirPath :Ŀ¼
 *		Return:  Ŀ¼���ļ���С
 *--------------------------------------------------------------------------------------
 */
unsigned int CSocketFtpFile::GetDirSize ( const char *pchDirPath )
{
	int iRtn;
	gs_uiTotalSize = 0;

    if (NULL == pchDirPath || access(pchDirPath, R_OK))  
	{
		printf ( "GetDirSize:is not a dirpath\n" );
		return 0;	
	}

	//�ݹ���ü����ļ���С  ftw()�ɹ�����0 ������ֹ-1
	iRtn = ftw( pchDirPath, CalDirSize, 20 );
	if( -1 ==  iRtn)
	{
		printf ( "GetDirSize error %s\n", pchDirPath );
	}

	return gs_uiTotalSize;
}		/* -----  end of method CSocketFtpFile::GetDirSize  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  isFileExist
 * Description:  �鿴�ļ��Ƿ����
 *       Input:  �ļ���
 *		Return:  bool
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::IsFileExist ( const char *cpchFileName )
{
    struct stat statbuf;
    if(lstat(cpchFileName, &statbuf) ==0)
	{
        return S_ISREG(statbuf.st_mode) != 0;//�ж��ļ��Ƿ�Ϊ�����ļ�
	}

	return false;
}		/* -----  end of method CSocketFtpFile::isFileExist  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  isDirExist
 * Description:  �鿴�ļ����Ƿ����
 *       Input:  �ļ���·��
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::IsDirExist ( const char *cpchDirPath )
{
    struct stat statbuf;
    if(lstat(cpchDirPath, &statbuf) ==0)//lstat�����ļ�����Ϣ���ļ���Ϣ�����stat�ṹ��
    {
        return S_ISDIR(statbuf.st_mode) != 0;//S_ISDIR�꣬�ж��ļ������Ƿ�ΪĿ¼
    }

    return false;
}		/* -----  end of method CSocketFtpFile::isDirExist  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  isSpecialDir
 * Description:  �鿴�Ƿ�������Ŀ¼ ��ָ. ..
 *       Input:  �ļ�Ŀ¼·��
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::IsSpecialDir ( const char *cpchDirPath )
{
    return strcmp(cpchDirPath, ".") == 0 || strcmp(cpchDirPath, "..") == 0;
}		/* -----  end of method CSocketFtpFile::isSpecialDir  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  GetTotalFilePath
 * Description:  ��Ŀ¼���ļ����������·�� ��../ �� main.c ����../main.c
 *       Input:  cpchPath Ŀ¼ "../"
 *				 cpchFileName �ļ��� "main.c"
 *				 cpchFilePath Ҫ��ɵ��ļ����ɹ�����"../main.c"
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CSocketFtpFile::GetTotalFilePath ( const char *cpchPath, 
		const char *cpchFileName,
		char *cpchFilePath)
{
    strcpy(cpchFilePath, cpchPath);
    if(cpchFilePath[strlen(cpchPath) - 1] != '/')
        strcat(cpchFilePath, "/");
    strcat(cpchFilePath, cpchFileName);
}		/* -----  end of method CSocketFtpFile::GetTotalFilePath  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  ChangeFileMode
 * Description:  �޸��ļ���Ȩ��  �޸������ļ���Ȩ��ʹ��
 *       Input:  pszFileName:�ļ��� 
 *               imode��Ȩ����stat�к궨��
 *               S_ISGID 02000 �ļ��� (set group-id on execution)λ
 *               S_ISVTX 01000 �ļ���sticky λ
 *               S_IRUSR (S_IREAD) 00400 �ļ������߾߿ɶ�ȡȨ��
 *               S_IWUSR (S_IWRITE)00200 �ļ������߾߿�д��Ȩ��
 *               S_IXUSR (S_IEXEC) 00100 �ļ������߾߿�ִ��Ȩ��
 *               S_IRGRP 00040 �û���߿ɶ�ȡȨ��
 *               S_IWGRP 00020 �û���߿�д��Ȩ��
 *               S_IXGRP 00010 �û���߿�ִ��Ȩ��
 *               S_IROTH 00004 �����û��߿ɶ�ȡȨ��
 *               S_IWOTH 00002 �����û��߿�д��Ȩ��
 *               S_IXOTH 00001 �����û��߿�ִ��Ȩ��
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::ChangeFileMode ( char *pszFileName, int imode )
{
	if( NULL == pszFileName )
	{
		return false;
	}

	if( 0 == chmod ( pszFileName, imode ))
	{
		return true;
	}

	return false;
}		/* -----  end of method CSocketFtpFile::ChangeFileMode  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  ChangeDirFilesMode
 * Description:  �޸��ļ����������ļ���Ȩ�� 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::ChangeDirFilesMode ( char *pchDirPath, int imode )
{
	DIR *dir;
	struct dirent *ptr;
	struct stat ;
	char base[1000];

	if ((dir=opendir(pchDirPath)) == NULL)
	{
		perror("Open dir error...");
		return false;

	}

	while ((ptr=readdir(dir)) != NULL)
	{
		if(IsSpecialDir( ptr->d_name ))    ///current dir OR parrent dir
			continue;
		else if(ptr->d_type == 8 ||     ///file
				ptr->d_type == 10)    ///link file
		{
			char chFileName[256];
			sprintf( chFileName, "%s/%s", pchDirPath, ptr->d_name );
			if(  !ChangeFileMode( chFileName, imode ) )	
			{
				closedir( dir );
				return false;
			}
		}
		else if(ptr->d_type == 4)    ///dir
		{
			memset(base,'\0',sizeof(base));
			strcpy(base,pchDirPath);
			strcat(base,"/");
			strcat(base,ptr->d_name);
			if( !ChangeDirFilesMode(base, imode))
			{
				closedir( dir );
				return false;
			}
		}
	}

	closedir(dir);
	return true ;
}		/* -----  end of method CSocketFtpFile::ChangeDirFilesMode  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  DeleteDirFiles
 * Description:  ɾ���ļ����������ļ�
 *       Input:  Ŀ¼
 *		Return:  
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::DeleteDirFiles ( const char *pchDirPath )
{
    DIR *dir = NULL;
    dirent *dir_info;
    char file_path[PATH_MAX];
    if(IsFileExist(pchDirPath))
    {
        remove(pchDirPath);
        return false;
    }

    if(IsDirExist(pchDirPath))
    {
        if((dir = opendir(pchDirPath)) == NULL)
            return false;
        while((dir_info = readdir(dir)) != NULL)
        {
            GetTotalFilePath(pchDirPath, dir_info->d_name, file_path);
            if(IsSpecialDir(dir_info->d_name))
                continue;
            DeleteDirFiles(file_path);
            rmdir(file_path);
        }
    }

	closedir( dir );

	return true;
}		/* -----  end of method CSocketFtpFile::DeleteDirFiles  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  MoveDirFiles
 * Description:  �ƶ��ļ���Ŀ¼�� ����renameʵ��
 *       Input:  Դ�ļ��л�Ŀ¼
 *				 Ŀ���ļ��л�Ŀ¼
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::MoveDirFiles ( char *pchSrcPath, char *pchDirPath )
{
	if( NULL == pchSrcPath
		|| NULL == pchDirPath )
	{
		return false;	
	}

	if( 0 == rename( pchSrcPath, pchDirPath ) )
	{
		return true;	
	}
	perror( "rename" );
	
	return false;
}		/* -----  end of method CSocketFtpFile::MoveDirFiles  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  WriteFileListFile
 * Description:  ���ļ����µ�����д��ĳ�ļ��� 
 *       Input:  pchDirPath ·��
 *				 pchFileName �ļ���
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::WriteFileListFile (  char *pchDirPath, char *pchFileName )
{
	DIR *dir;
	struct dirent *ptr;
	struct stat ;
	char base[1000];

	if( NULL == pchFileName )
	{
		return false;
	}

	if ((dir=opendir(pchDirPath)) == NULL)
	{
		perror("Open dir error...");
		return false;

	}

	while ((ptr=readdir(dir)) != NULL)
	{
		if(IsSpecialDir( ptr->d_name ))    ///current dir OR parrent dir
			continue;
		else if(ptr->d_type == 8 ||     ///file
				ptr->d_type == 10)    ///link file
		{
			char chFileName[256];
			sprintf( chFileName, "%s/%s\n", pchDirPath, ptr->d_name );
			WriteFile( pchFileName, (unsigned char *)chFileName , strlen( chFileName ));
		}
		else if(ptr->d_type == 4)    ///dir
		{
			memset(base,'\0',sizeof(base));
			strcpy(base,pchDirPath);
			strcat(base,"/");
			strcat(base,ptr->d_name);
			if( !WriteFileListFile(base, pchFileName))
			{
				closedir( dir );
				return false;
			}
		}
	}

	closedir(dir);
	return true ;
}		/* -----  end of method CSocketFtpFile::WriteFileListFile  ----- */


/* 
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ����Ŀ��Թ��ã�����Ľ�����������Э��ʹ�õ�-.-2015��09��25�� 14ʱ20��41��
 * ---------------------------------------------------------------------------
 *  */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  CheckFileList
 * Description:  �������ļ��б��Ƿ���ȷ 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::CheckFileList (  )
{
	const int iCheckNum = 5;
	const int iMaxFileNum = 50;
	bool bPrgm[iCheckNum] = {false, false, false, false, false};
	int iother = 0;
	char szCheckType[iCheckNum][64] = {"/mynand/bin/pman", "/mynand/bin/gather", "/mynand/bin/rtdbserver", ".so", "/myapp/user.sh"};
	char chLineBuf[256];

	for ( int i=1; NULL != GetFileLineBuf( FTP_FILE_LIST, i, chLineBuf ) && i<iMaxFileNum; i++)
	{
		if( '\n' == chLineBuf[0] )
		{
			continue;
		}

		int j = 0;
		for( j=0; j<iCheckNum; j++ )
		{
			if( NULL != strstr( chLineBuf, szCheckType[j] ) )
			{
				bPrgm[j] = true;
				break;
			}
		}

		if( iother )
		if( j >= iCheckNum )
		{
			return false;
		}
	}


	for ( int k=0; k<iCheckNum-1; k++)
	{
		if( !bPrgm[k] )
		{
			printf ( "CheckFileList none %s\n", szCheckType[k] );
			return false;	
		}
	}

	return true;
}		/* -----  end of method CSocketFtpFile::CheckFileList  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  GetDownType
 * Description:  ��filelist�л���ļ����ͣ�������һ���Ĵ洢
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
FTP_FILE_TYPE CSocketFtpFile::GetDownType ( char *pchFileName )
{
	FILE *fp = NULL;
	char chLineBuf[256];

	fp = fopen( pchFileName, "rb" );
	if( NULL == fp )
	{
		perror( "ftp_list" );
		return FTP_NONE_TYPE;
	}

	while( fgets( chLineBuf, sizeof( chLineBuf ), fp ) )
	{
		if( NULL != strstr( chLineBuf, "/mynand/config/" ) )
		{
			fclose( fp );
			return FTP_CFG_TYPE;
		}
		
		if( NULL != strstr( chLineBuf, "/mynand/bin" ) ||
				NULL != strstr( chLineBuf, "/mynand/lib" ) ||
				NULL != strstr( chLineBuf, "/myapp/user.sh" ))
		{
			fclose( fp );
			return FTP_PRGM_TYPE;
		}

		continue;
	}

	fclose( fp );
	return FTP_NONE_TYPE;
}		/* -----  end of method CSocketFtpFile::GetDownType  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  GetDownFileName
 * Description:  ��駴��������ļ����޸�Ϊ��Ҫ���ļ���
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
char * CSocketFtpFile::GetDownFileName ( FTP_FILE_TYPE fType,
		char *pchFileName )
{
	char chTmpBuf[256];
	memset( chTmpBuf, 0, 256 );

	if( NULL == pchFileName ||
			FTP_NONE_TYPE == fType)
	{
		return NULL;
	}

	if( FTP_CFG_TYPE == fType )
	{
		if( 0 == strncmp( pchFileName, "/mynand/", 8 ) )		
		{
			strcpy( chTmpBuf, FTP_DOWN_CFG_FILE );	
			strcat( chTmpBuf, pchFileName+8 );
			strcpy( pchFileName, chTmpBuf );
			return pchFileName;
		}
	}

	if( FTP_PRGM_TYPE == fType )
	{
		if( 0 == strncmp( pchFileName, "/mynand/", 8 ) )		
		{
			strcpy( chTmpBuf, FTP_DOWN_PRGM_FILE );	
			strcat( chTmpBuf, pchFileName+8 );
			strcpy( pchFileName, chTmpBuf );
			return pchFileName;
		}
		else if( 0 == strncmp( pchFileName, "/myapp/user.sh" , 14) )
		{
			strcpy( pchFileName, FTP_DOWN_USER_FILE );
			return pchFileName;
		}
	}

	return NULL;
}		/* -----  end of method CSocketFtpFile::GetDownFileName  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  GetFileLineBuf
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
char * CSocketFtpFile::GetFileLineBuf ( char *pchFileName, 
		int iLine,
		char *pchLineBuf )
{
	FILE *fp = NULL;
	char chLineBuf[256];
	int iCount = 0;

	if( NULL == pchFileName ||
			NULL == pchLineBuf)
	{
		return NULL;
	}

	fp = fopen( pchFileName, "rb" );
	if( NULL == fp )
	{
		perror( "ftp_list" );
		return NULL;
	}

	while( fgets( chLineBuf, sizeof( chLineBuf ), fp ) )
	{
		if( NULL == strstr( chLineBuf, "/" ) )
		{
			continue;
		}

		iCount ++ ;
		if( iCount == iLine )
		{
			strcpy( pchLineBuf, chLineBuf );	
			fclose( fp );
			return pchLineBuf;
		}
	}

	fclose( fp );
	return NULL;
}		/* -----  end of method CSocketFtpFile::GetFileLineBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  BakDirFiles
 * Description:  �����ļ�
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::BakDirFiles ( char* pchSrcPath,
		char *pchDirPath )
{
	if( NULL == pchSrcPath ||
			NULL == pchDirPath)
	{
		return false;
	}

	//����config
	DeleteDirFiles( pchDirPath ) ;
	if( !MoveDirFiles( pchSrcPath, pchDirPath ) )
	{
		printf ( "BakDirFiles::%s bak %s fail\n", pchSrcPath, pchDirPath );
		return false;
	}

	return true;
}		/* -----  end of method CSocketFtpFile::BakDirFiles  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  UpdateCfg
 * Description:  ���������ļ�
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::UpdateCfg ( void )
{
	char chCfgFile[256] = "/mynand/config/";
	char chBakCfgFile[256] = "/mynand/bak/config/";
	char chTmp[256] = "";

	sprintf( chTmp, "%s%s", FTP_DOWN_CFG_FILE, (char *)"config/" );
	if( !IsDirExist( chTmp ) )
	{
		printf ( "can't find %s\n", chTmp );
		return false;
	}
	printf ( "/mynand/config begin to update\n" );

	//����config
	if( !BakDirFiles( chCfgFile, chBakCfgFile ) )
	{
		return false;
	}

	//�ƶ�downcfg
	if( !BakDirFiles( chTmp, chCfgFile ) )
	{
		if( !BakDirFiles( chBakCfgFile, chCfgFile ) )
		{
			return false;	
		}

		return false;
	}
	else
	{
		// DeleteDirFiles( chBakCfgFile );
	}

	//Ȩ��
	if( !ChangeDirFilesMode( chCfgFile, 
				S_IRUSR | S_IRGRP ) )
	{
		return false;
	}

	DeleteDirFiles( FTP_DOWN_CFG_FILE );
	rmdir( FTP_DOWN_CFG_FILE );
	printf ( "/mynand/config update success\n" );
	return true;
}		/* -----  end of method CSocketFtpFile::UpdateCfg  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CSocketFtpFile
 *      Method:  UpdatePrgm
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
bool CSocketFtpFile::UpdatePrgm ( void )
{
	char chPrgmBinFile[256] = "/mynand/bin/";
	char chBakPrgmBinFile[256] = "/mynand/bak/bin/";
	char chPrgmLibFile[256] = "/mynand/lib/";
	char chBakPrgmLibFile[256] = "/mynand/bak/lib/";
	char chPrgmUserFile[256] = "/myapp/user.sh";
	char chBakPrgmUserFile[256] = "/myapp/bak/user.sh";
	char chTmp[256] = "";

	if( !IsDirExist( FTP_DOWN_PRGM_FILE ) )
	{
		return false;
	}

	sprintf( chTmp, "%s%s", FTP_DOWN_PRGM_FILE, "bin/" );
	if( IsDirExist( chTmp ) )
	{
		printf ( "/mynand/bin begin to update\n" );
		//����bin
		if( !BakDirFiles( chPrgmBinFile, chBakPrgmBinFile ) )
		{
			return false;
		}

		//�ƶ�bin
		if( !BakDirFiles( chTmp, chPrgmBinFile ) )
		{
			if( !BakDirFiles( chBakPrgmBinFile, chPrgmBinFile ) )
			{
				return false;	
			}

			return false;
		}

		// Ȩ��
		if( !ChangeDirFilesMode( chPrgmBinFile, 
					S_IRUSR | S_IWUSR | S_IXUSR	| 
					S_IRGRP | S_IWGRP | S_IXGRP |
					S_IROTH | S_IWOTH) )
		{
			return false;	
		}
		printf ( "/mynand/bin update success\n" );
		DeleteDirFiles( chTmp );
		rmdir( chTmp );
	}
	else
	{
		printf ( "can't find %s\n", chTmp );
	}


	sprintf( chTmp, "%s%s", FTP_DOWN_PRGM_FILE, "lib/" );
	if( IsDirExist( chTmp ) )
	{
		printf ( "/mynand/lib begin to update\n" );
		//����lib
		if( !BakDirFiles( chPrgmLibFile, chBakPrgmLibFile ) )
		{
			return false;
		}

		//�ƶ�bin
		if( !BakDirFiles( chTmp, chPrgmLibFile ) )
		{
			if( !BakDirFiles( chBakPrgmLibFile, chPrgmLibFile ) )
			{
				return false;	
			}

			return false;
		}

		// Ȩ��
		if( !ChangeDirFilesMode( chPrgmLibFile, 
					S_IRUSR | S_IWUSR |  
					S_IRGRP | S_IWGRP | 
					S_IROTH | S_IWOTH) )
		{
			return false;	
		}

		DeleteDirFiles( chTmp );
		rmdir( chTmp );
		printf ( "/mynand/lib update success\n" );
	}
	else
	{
		printf ( "can't find %s\n", chTmp );
	}

	if( IsFileExist( FTP_DOWN_USER_FILE ) )
	{
		printf ( "/myapp/user.sh begin to update\n" );
		//����user.sh
		if( !BakDirFiles( chPrgmUserFile, chBakPrgmUserFile ) )
		{
			return false;
		}

		//�ƶ�user.sh
		// sprintf( chTmp, "%s%s", FTP_DOWN_PRGM_FILE, "user.sh" );
		if( !BakDirFiles(FTP_DOWN_USER_FILE , chPrgmUserFile ) )
		{
			if( !BakDirFiles( chBakPrgmUserFile, chPrgmUserFile ) )
			{
				return false;	
			}

			return false;
		}

		// Ȩ��
		if( !ChangeFileMode( chPrgmUserFile, 
					S_IRUSR | S_IWUSR | S_IXUSR	| 
					S_IRGRP | S_IWGRP | S_IXGRP |
					S_IROTH | S_IWOTH) )
		{
			return false;	
		}

		DeleteDirFiles( FTP_DOWN_USER_FILE );
		rmdir( FTP_DOWN_USER_FILE );
		printf ( "/myapp/usr.sh update success\n" );
	}
	else
	{
		printf ( "can't find %s\n", FTP_DOWN_USER_FILE );
	}

	return true	;
}		/* -----  end of method CSocketFtpFile::UpdatePrgm  ----- */



