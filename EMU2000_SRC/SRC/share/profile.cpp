/**********************************************************************
  profile.cpp : implementation file for Linux
  Copyright (C): 2011 by houpeng
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <stdarg.h>
#include <iostream>
using namespace std;


#include "profile.h"
#include "global.h"

#define MAX_LINE_LEN  1024

//#define max(a,b)   (((a) > (b)) ? (a) : (b))
//#define min(a,b)   (((a) < (b)) ? (a) : (b))
/*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

//ɾ���ַ�����߿ո�\t
void ltrim(char *s);

//ɾ���ַ����ұ߿ո�'\t','\r','\n'
void rtrim(char *s);


int GetTextLine(FILE* fd, char* lpszBuf, int nSize)
{
    if( fd == NULL) return -1;
	if( fgets(lpszBuf, nSize, fd) == NULL ) return -2;
	return 1;
}

int stricmp(const char *s, const char *t)
{
    int a, b;

    while( *s && *t )
    {
        a = (int)*s; b = (int)*t;
        if( isupper(a) ) a = tolower(a);
        if( isupper(b) ) b = tolower(b);
        if( a != b ) return (a > b ? 1 : -1);
        s++; t++;
    }

    if( *s != *t )
        return (*s > *t ? 1 : -1);
    else
        return 0;
}

void LogPromptText(const char *fmt, ...);


#ifdef __cplusplus
}
#endif
/******************************************************************************
* CDataBuffer Object
*/
CDataBuffer::CDataBuffer()
{
    m_nLen = 0;
    m_pBuff = NULL;
    m_objPrev = NULL;
    m_objNext = NULL;
}

CDataBuffer::CDataBuffer(char *pBuf, int nLen)
{
    CDataBuffer();
    if( nLen <= 0 ) return;
    m_pBuff = new char[nLen];
	if( m_pBuff )
    {
        m_nLen = nLen;
        memcpy(m_pBuff, pBuf, m_nLen);
    }
}

CDataBuffer::~CDataBuffer()
{
    if(m_nLen>0)
    {
        if(m_pBuff)
        {
            delete m_pBuff;
            m_pBuff = NULL;
        }
        m_nLen = 0;
    }
    m_objPrev = NULL;
    m_objNext = NULL;
}

int CDataBuffer::GetBuffer(char *pBuf)
{
    if( m_pBuff ) memcpy(pBuf, m_pBuff, m_nLen);
    return m_nLen;
}

char* CDataBuffer::GetBuffer(void)
{
    return m_pBuff;
}

int CDataBuffer::GetBufLen(void)
{
    return m_nLen;
}
/******************************************************************************
* CMyList Object
*/
CMyList::CMyList()
{
    m_nSize = 0;
    m_objFirt = NULL;
    m_objLast = m_objFirt;
    m_objActual = m_objFirt;
}

CMyList::~CMyList()
{
    while( m_objFirt )
    {
        CDataBuffer* pTemp = m_objFirt;
        m_objFirt = m_objFirt->m_objNext;
        delete pTemp;
        pTemp = NULL;
    }
	CMyList();
}

int CMyList::GetSize()
{
    return m_nSize;
}

bool CMyList::AddObj(CDataBuffer* pObj)
{
    if( !pObj ) return false;
	return true;
}

CDataBuffer* CMyList::GetFirt()
{
    m_objActual = m_objFirt;
    return m_objFirt;
}

CDataBuffer* CMyList::GetLast()
{
    m_objFirt = m_objLast;
    return m_objLast;
}

CDataBuffer* CMyList::GetNext(CDataBuffer* pObj)
{
    if( !pObj ) return NULL;
	return pObj->m_objNext;
}

CDataBuffer* CMyList::GetPrev(CDataBuffer* pObj)
{
    if( !pObj ) return NULL;
	return pObj->m_objPrev;
}

/******************************************************************************
* CProfile Object
*/
CProfile::CProfile()
{
    m_fdFile = 0;
    sprintf(m_szFileName, "%s", "");
}

CProfile::CProfile(char* lpszName)
{
    sprintf(m_szFileName, "%s", lpszName);

    if( OpenFile(lpszName) )
    {
        char szLine[MAX_LINE_LEN];
    	while( GetTextLine(m_fdFile, szLine, sizeof(szLine)) > 0 )
        {
            ltrim(szLine);
            rtrim(szLine);
            if( szLine[0]==';' || szLine[0]=='#' ) continue;
            int nLen = strlen(szLine);
            if( nLen <= 0 ) continue;

			CDataBuffer *pObj = new CDataBuffer(szLine, nLen+1);
            m_LineArray.push_back(pObj);
        }
		CBufferArray(m_LineArray).swap(m_LineArray);
    }
}

CProfile::~CProfile()
{
    if(m_fdFile) fclose(m_fdFile);

    int nCount = m_LineArray.size();
    if( nCount > 0 )
    {
        while(nCount--) delete m_LineArray[nCount];
        m_LineArray.clear();
    }
}

bool CProfile::IsValid()
{
    return (m_fdFile != NULL);
}

bool CProfile::OpenFile(char* lpszName)
{
	m_fdFile = fopen(lpszName, "r");
    return IsValid();
}

int CProfile::GetProfileString(char* lpAppName,
                               char* lpKeyName,
                               char* lpDefault,
                               char* lpszBuff, int nSize,
                               FILE* fd)
{
    int  nLen;
    char szSect[64], *p;
    char szLine[MAX_LINE_LEN];

    if( fd > 0 )
    {
        sprintf(szSect, "[%s]", lpAppName);
    	fseek(fd, 0, SEEK_SET);
    	while( GetTextLine(fd, szLine, sizeof(szLine)) > 0 )
        {
            ltrim(szLine);
            rtrim(szLine);
            if( szLine[0]==';' || szLine[0]=='#' ) continue;
            /*if( strspn(szLine, szSect) == strlen(szSect) ) break;*/
            if( strcmp(szLine, szSect) == 0 ) break;
    	}
    	while( GetTextLine(fd, szLine, sizeof(szLine)) > 0 )
        {
            ltrim(szLine);
            rtrim(szLine);
            if( szLine[0]==';' || szLine[0]=='#' ) continue;
            //if( szLine[0]=='[' && strchr(szLine, ']') ) break;
            if( strchr(szLine, '[') < strchr(szLine, ']') ) break;
            if( !strpbrk(szLine, "=") ) continue;
            p = strtok(szLine, "=");
            if( !strcmp(p, lpKeyName) )
            {
                p = strtok(NULL, "= ");
                nLen = min((int)strlen(p), nSize-1);
                strncpy(lpszBuff, p, nLen);
                lpszBuff[nLen] = '\0';
                return nLen;
            }
    	}
    }
    nLen = strlen(lpDefault);
    strcpy(lpszBuff, lpDefault);
    return nLen;
}

int CProfile::GetProfileInt(char* lpAppName,
                               char* lpKeyName,
                               int   nDefault,
                               FILE* fd)
{
    int  nVal=nDefault;
    char szDefault[16], szBuff[32];

    sprintf(szDefault, "%d", nDefault);
    if(GetProfileString(lpAppName, lpKeyName, szDefault, szBuff, 32, fd)>0)
    {
        nVal = atoi(szBuff);
    }
    return nVal;
}

int CProfile::FindProfileString( char* lpAppName,
                          char* lpKeyName,
                          char* lpDefault,
                          char* lpszBuff, int nSize )
{
    int  i, nCount, nLen;
    char szSect[64], *p;
    char szLine[MAX_LINE_LEN];
    CDataBuffer *pObj;

    nCount = m_LineArray.size();
    if( nCount > 0 )
    {
        sprintf(szSect, "[%s]", lpAppName);
		for( i=0; i<nCount; i++ )
        {
            pObj = m_LineArray[i];
            if( pObj == NULL ) continue;
            pObj->GetBuffer(szLine);
            if( strcmp(szLine, szSect) == 0 ) break;
		}

    	for( i=i+1; i<nCount; i++ )
        {
            pObj = m_LineArray[i];
            if( pObj == NULL ) continue;
            pObj->GetBuffer(szLine);
            if( strchr(szLine, '[') < strchr(szLine, ']') ) break;
            if( !strpbrk(szLine, "=") ) continue;
            p = strtok(szLine, "=");
            if( strcmp(p, lpKeyName) == 0 )
            {
                p = strtok(NULL, "= "); //by zhg 2014.11.25
                if( p == NULL )
                continue ;

                nLen = min((int)strlen(p), nSize-1);
                strncpy(lpszBuff, p, nLen);
                lpszBuff[nLen] = '\0';
                return nLen;
            }
    	}
    }
    nLen = strlen(lpDefault);
    strcpy(lpszBuff, lpDefault);
    return nLen;
}

int CProfile::FindProfileInt( char* lpAppName,
                          char* lpKeyName,
                          int   nDefault )
{
    int  nVal=nDefault;
    char szDefault[16], szBuff[32];

    sprintf(szDefault, "%d", nDefault);
    if(FindProfileString(lpAppName, lpKeyName, szDefault, szBuff, 32)>0)
    {
        nVal = atoi(szBuff);
    }
    return nVal;
}

int CProfile::GetProfileString(char* lpAppName,
                               char* lpKeyName,
                               char* lpDefault,
                               char* lpszBuff,
                               int nSize)
{
    if( m_LineArray.size() > 0 )
        return FindProfileString(lpAppName, lpKeyName, lpDefault, lpszBuff, nSize);
    else
        return GetProfileString(lpAppName, lpKeyName, lpDefault, lpszBuff, nSize, m_fdFile);
}

int CProfile::GetProfileInt(char* lpAppName,
                               char* lpKeyName,
                               int   nDefault )
{
    if( m_LineArray.size() > 0 )
        return FindProfileInt(lpAppName, lpKeyName, nDefault);
    else
        return GetProfileInt(lpAppName, lpKeyName, nDefault, m_fdFile);
}
/*****************************************************************************/
