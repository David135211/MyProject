#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/reboot.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()

#define WDT "/dev/watchdog"
#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0

int g_iAppRun = 1;

static void init_daemon(void)
{
	int   i;
    pid_t child1, child2;

	/*捕捉的信号*/
	/*创建第一子进程*/
	child1 = fork();

	if( child1 < 0 )
	{
		perror("Create first child process fail!\n");
		exit(1);
	}
	else if(child1 > 0) exit(0); /*结束父进程*/
	/*第一子进程成为新的会话组长和进程组长*/
	setsid();
	/*第一子进程与控制终端分离*/
	child2 = fork();
	if( child2 < 0 )
	{
		perror("Create second child process fail!\n");
		exit(2);
	}
	else if(child2 > 0) exit(0); /*结束第一子进程*/
	/*第二子进程继续, 第二子进程不再是会话组长*/
	/*关闭打开的文件描述符*/
	for(i = 0; i < NOFILE; ++i)
		close(i);
	/*改变工作目录到/tmp*/
	chdir("/tmp");
	/*重设文件创建掩模*/
	umask(0);
	/*处理SIGCHLD信号*/
	signal(SIGCHLD, SIG_IGN);
}

static int is_proc_exist( char *name )
{
    FILE *pstr; char cmd[128],buff[512],*p;
    pid_t pID;
    int pidnum;
    int ret=3;
    memset(cmd,0,sizeof(cmd));

    sprintf(cmd, "ps -ef|grep %s ",name);
    pstr=popen(cmd, "r");//

    if(pstr==NULL)
    {
		return -1;
	}
    memset(buff,0,sizeof(buff));
    fgets(buff,512,pstr);
	printf ( "buff=%s\n", buff );
	if( NULL != strstr( buff, cmd ) )
	{
		return -1;
	}
    p=strtok(buff, " ");
    /* p=strtok(NULL, " "); //这句是否去掉，取决于当前系统中ps后，进程ID号是否是第一个字段 pclose(pstr); */
    if(p==NULL)
    {
		return -1;
	}
    if(strlen(p)==0)
    {
		return -1;
	}
    if((pidnum=atoi(p))==0)
    {
		return -1;
	}
    printf("pidnum: %d\n",pidnum);
    pID=(pid_t)pidnum;
    ret=kill(pID,0);//这里不是要杀死进程，而是验证一下进程是否真的存在，返回0表示真的存在
    printf("ret= %d \n",ret);
    if(0==ret)
	{
        printf("process: %s exist!\n",name);
	}
    else
	{
		printf("process: %s not exist!\n",name);
		return -1;
	}

    return 0;
}
//是不是数字
static int IsNumeric(const char* ccharptr_CharacterList)
{
	for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
		if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
			return 0; // false
	return 1; // true
}

//intCaseSensitive=0大小写不敏感
static int strcmp_Wrapper(const char *s1, const char *s2, int intCaseSensitive)
{
	if (intCaseSensitive)
		return !strcmp(s1, s2);
	else
		return !strcasecmp(s1, s2);
}

//intCaseSensitive=0大小写不敏感
static int strstr_Wrapper(const char* haystack, const char* needle, int intCaseSensitive)
{
	if (intCaseSensitive)
		return (int) strstr(haystack, needle);
	else
		return (int) strcasestr(haystack, needle);
}

static pid_t GetPIDbyName_implements(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
{
	char chrarry_CommandLinePath[100]  ;
	char chrarry_NameOfProcess[300]  ;
	char* chrptr_StringToCompare = NULL ;
	pid_t pid_ProcessIdentifier = (pid_t) -1 ;
	struct dirent* de_DirEntity = NULL ;
	DIR* dir_proc = NULL ;

	int (*CompareFunction) (const char*, const char*, int) ;

	if (intExactMatch)
		CompareFunction = &strcmp_Wrapper;
	else
		CompareFunction = &strstr_Wrapper;


	dir_proc = opendir(PROC_DIRECTORY) ;
	if (dir_proc == NULL)
	{
		perror("Couldn't open the " PROC_DIRECTORY " directory") ;
		return (pid_t) -2 ;
	}

	// Loop while not NULL
	while ( (de_DirEntity = readdir(dir_proc)) )
	{
		if (de_DirEntity->d_type == DT_DIR)
		{
			if (IsNumeric(de_DirEntity->d_name))
			{
				strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
				strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
				strcat(chrarry_CommandLinePath, "/cmdline") ;
				FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  //open the file for reading text
				if (fd_CmdLineFile)
				{
					fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; //read from /proc/<NR>/cmdline
					fclose(fd_CmdLineFile);  //close the file prior to exiting the routine

					if (strrchr(chrarry_NameOfProcess, '/'))
						chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ;
					else
						chrptr_StringToCompare = chrarry_NameOfProcess ;

					/* printf("Process name: %s\n", chrarry_NameOfProcess); */
					//这个是全路径，比如/bin/ls
					/* printf("Pure Process name: %s\n", chrptr_StringToCompare ); */
					//这个是纯进程名，比如ls

					//这里可以比较全路径名，设置为chrarry_NameOfProcess即可
				//	printf("chrptr_StringToCompare = %s cchrptr_ProcessName = %s\n", chrptr_StringToCompare, cchrptr_ProcessName);
					if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, intCaseSensitiveness) )
					{
						pid_ProcessIdentifier = (pid_t) atoi(de_DirEntity->d_name) ;
						closedir(dir_proc) ;
						return pid_ProcessIdentifier ;
					}
				}
			}
		}
	}
	closedir(dir_proc) ;
	return pid_ProcessIdentifier ;
}

//简单实现
static pid_t GetPIDbyName_Wrapper(const char* cchrptr_ProcessName)
{
	return GetPIDbyName_implements(cchrptr_ProcessName, 0, 0);//大小写不敏感
}

// int main()
// {
	// pid_t pid = GetPIDbyName_Wrapper("em761-b") ; // If -1 = not found, if -2 = proc fs access error
	// printf("PID %d\n", pid);
	// return EXIT_SUCCESS ;
// }

static void SignHandler(int signum, siginfo_t *pInfo, void *pReserved)
{
	g_iAppRun = 0;
}

typedef void (*LPSIGPROC)(int, siginfo_t*, void*);

static int SignalHook(int iSigNo, LPSIGPROC func)
{
	struct sigaction act, oact;

	act.sa_sigaction = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO|SA_RESTART;

	if(sigaction(iSigNo, &act, &oact) < 0)
		return -1;
	return 0;
}

int main(void)
{
    int wdt_fd = -1;
    int timeout;

	g_iAppRun = 1;

	// SignalHook(SIGINT,  SignHandler); /*CTRL-C*/
	// SignalHook(SIGQUIT, SignHandler); /*CTRL-\*/
	// SignalHook(SIGSTOP, SignHandler); /*CTRL-Z*/

	//创建守护进程
//	init_daemon();

    wdt_fd = open(WDT, O_WRONLY);
    if (wdt_fd == -1)
    {
        printf("fail to open WDT !\n");
    }
    printf("WDT is opened!\n");

    timeout = 20;
    ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);

    ioctl(wdt_fd, WDIOC_GETTIMEOUT, &timeout);
    printf("The timeout was is %d seconds\n", timeout);

    while(g_iAppRun)
    {
		// if( -1 != is_proc_exist( ( char * )"./em761-b" ) )
	//	if( GetPIDbyName_Wrapper("em761-b") > 0 )
		if( GetPIDbyName_Wrapper("VirtCOM") > 0 )
		{
			write(wdt_fd, "\0", 1);
		}
		else
		{
		//	printf ( "not find em761-b\n" );
			printf ( "not find VirtCom\n" );
		}
		sleep(3);
    }

// #if 1
    write(wdt_fd, "V", 1);
    close(wdt_fd);
    printf( WDT " is closeed!\n");
// #endif

    return 0;
}
