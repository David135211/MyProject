/*
 * =====================================================================================
 *
 *       Filename:  global.cpp
 *
 *    Description: ����ȫ�ֺ���
 *
 *        Version:  1.0
 *        Created:  2014��09��11�� 11ʱ18��41��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/reboot.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>


#include "global.h"

#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*******************************************************************************
 * ������:IsBigEndian
 * ��������:�жϴ�С�ˣ���˷���true С�˷���false
 * ����:void
 * ����ֵ:bool
 ******************************************************************************/
bool IsBigEndian(void)
{
    int i=1;

    char *p=(char *)&i;

    if ( 1 == *p)
    {
        return false;
    }
    else
    {
        return true;
    }

    return true;
}   /*-------- end IsBigEndian -------- */

/*******************************************************************************
 * ������:GlobalCopyByEndian
 * ��������:���ݴ�С��ģʽ����buf
 * ����: char *dest Ŀ��buf
 * ����: char *src Դbuf
 * ����: int num Ҫ����������
 * ����ֵ:bool
 ******************************************************************************/
bool GlobalCopyByEndian( unsigned char *dest, unsigned char *src, unsigned int num )
{
    if ( NULL == dest || NULL == src )
    {
        return false;
    }

    // ������dest��src�ڴ��غ�
    if ( IsBigEndian( ) )
    {
        src += num-1;
        while( 0 != num-- )
        {
            *( dest++ ) = *( src-- );
        }
    }
    else
    {
        while( 0 != num-- )
        {
            *( dest++ ) = *( src++ );
        }
    }

    return true;
}   /*-------- end GlobalCopyByEndian -------- */


/*  set advisory lock on file */
static int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;  /*  write lock */
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;  //lock the whole file
    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *filename)
{
    int fd;
    char buf[16];

    fd = open(filename, O_RDWR | O_CREAT, LOCKMODE);
    if (fd < 0)
    {
        printf( "can't open %s: %m\n", filename);
        exit(1);
    }
    /*  �Ȼ�ȡ�ļ��� */
    if (lockfile(fd) == -1)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            printf( "file: %s already locked", filename);
            close(fd);
            return 1;
        }

        printf("can't lock %s: %m\n", filename);
        exit(1);

    }
    /*  д������ʵ����pid */
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return 0;
}

void SetSerialConfigMode(  int byMode )
{
    BYTE byType = 0 ;
    switch( byMode )
    {
        case 1:
            {
                byType = CMD_SET_UT1_NON_RS485 ;
                printf( "Set RS01 = RS232 \n" );
            }
            break;
        case 2:
            {
                byType = CMD_SET_UT2_NON_RS485 ;
                printf( "Set RS02 = RS232 \n" );
            }
            break;
        case 8:
            {
                byType = CMD_SET_UT3_NON_RS485 ;
                printf( "Set RS03 = RS422 \n" );
            }
            break;
        case 4:
            {
                byType = CMD_SET_UT4_NON_RS485 ;
                printf( "Set RS04 = RS422 \n" );
            }
            break;
        default:
            return ;
    }

    int gpiofd;
    gpiofd = open(GPIO_DEV_NAME, O_RDWR);
    if( gpiofd < 1  ) return;
    ioctl(gpiofd, byType, 0);
    close(gpiofd);
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TransYxTimeToStructTm
 *  Description:  struct tm
 * =====================================================================================
 */
struct tm  SetStructTm ( TIMEDATA srcTime, struct tm &t, long &lTime )
{
    t.tm_year = srcTime.Year;
    t.tm_mon = srcTime.Month - 1;
    t.tm_mday = srcTime.Day;
    t.tm_hour = srcTime.Hour;
    t.tm_min = srcTime.Minute;
    t.tm_sec = srcTime.Second;

    lTime = mktime( &t ) ;
    return t;
}		/* -----  end of function TransYxTimeToStructTm  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  TransLongTimeToStructTm
 *  Description:
 * =====================================================================================
 */
void GetOwnStructTm ( long lTime, struct tm *t )
{
    localtime_r((time_t *)&lTime, t);
    t->tm_mon = t->tm_mon + 1;
}		/* -----  end of function TransLongTimeToStructTm  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *  Description: ����һЩ�ظ��� ���õĺ���
 * =====================================================================================
 */
#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */
    BOOL g_bDebugApp = TRUE; //����״̬
    BOOL g_bAppRun = TRUE;   //��������
    //ɾ���ַ�����߿ո�\t
    void ltrim(char *s)
    {
        char *p = s;
        if( *p==' ' || *p=='\t' )
        {
            while( *p==' ' || *p=='\t' ) p++;
            while( (*s++ = *p++) );
        }
    }

    //ɾ���ַ����ұ߿ո�'\t','\r', '\n'
    void rtrim(char *s)
    {
        char *p = s;
        while( *p ) p++;
        if( p != s )
        {
            p--;
            while( *p==' ' || *p=='\t' || *p=='\r' || *p=='\n' ) p--;
            *(++p) = 0;
        }
    }

    void OutPromptText(char *lpszText)
    {
        if( !g_bDebugApp ) return;
        printf("%s\n", lpszText);
    }

    void LogPromptText(const char *fmt, ...)
    {
        if( !g_bDebugApp ) return;
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }

    void OutMessageText(char *szSrc, unsigned char *pData, int nLen)
    {
        int i, k;
        char szBuff[96];

        if( !g_bDebugApp ) return;
        k = 0;
        sprintf(szBuff, "\n%s\n", szSrc);
        printf(szBuff);
        for( i=0; i<nLen; i++ )
        {
            k += sprintf(&szBuff[k], " %02X", pData[i]);
            if((i+1)%24==0)
            {
                printf(szBuff);
                szBuff[0] = '\n';
                k = 1;
            }
        }
        if( k > 1 ) printf(szBuff);
        /*
          if( nLen > 0 )
          printf("\n------------------------------------------------------------");
        */
    }


    int SignalHook(int iSigNo, LPSIGPROC func)
    {
        struct sigaction act, oact;

        act.sa_sigaction = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO|SA_RESTART;

        if(sigaction(iSigNo, &act, &oact) < 0)
            return -1;
        return 0;//(int)oact.sa_handler;
    }

    /*�����ػ�����*/
    void init_daemon(void)
    {
        int   i;
        pid_t child1, child2;
        /*������һ�ӽ���*/
        child1 = fork();
        if( child1 < 0 )
        {
            perror("Create first child process fail!\n");
            exit(1);
        }
        else if(child1>0) exit(0); /*����������*/
        /*��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤*/
        setsid();
        /*��һ�ӽ���������ն˷���*/
        child2 = fork();
        if( child2 < 0 )
        {
            perror("Create second child process fail!\n");
            exit(2);
        }
        else if(child2>0) exit(0); /*������һ�ӽ���*/
        /*�ڶ��ӽ��̼���, �ڶ��ӽ��̲����ǻỰ�鳤*/
        /*�رմ򿪵��ļ�������*/
        for(i=0; i<NOFILE; ++i)
            close(i);
        /*�ı乤��Ŀ¼��/tmp*/
        chdir("/tmp");
        /*�����ļ�������ģ*/
        umask(0);
        /*����SIGCHLD�ź�*/
        signal(SIGCHLD, SIG_IGN);
    }

#ifdef	__cplusplus
}
#endif	/* __cplusplus */

/*===========================================================================*/
