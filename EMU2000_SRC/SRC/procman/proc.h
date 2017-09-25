/******************************************************************************
  proc.h : header file
  Copyright (C): 2011 by houpeng
******************************************************************************/
#ifndef _PROCDEF_H__
#define _PROCDEF_H__

#include "../share/typedef.h"

#define	PROC_MAX_SUM   16
#define	PROC_NAME_LEN  24
#define	PROC_PATH_LEN  64

#define	PARA_MAX_SUM   4
#define	PROC_PARA_LEN  96

#if defined(__unix)
#define	END_PROC_SIGNAL		SIGKILL
#elif defined(WIN32)
#define END_PROC_SIGNAL		WM_CLOSE
#endif
/*******************************************************************/
//����������ʽ
enum AUTOSTARTMODE
{
	NOTSTART = 0,	//������
	RESPAWN,		//�����ػ�
	STARTAT,		//��ʱ����
	STARTCYCLE		//��������
};
//����״̬
enum ProcState
{
	PROC_NOEXIST = 0,	/*������*/
	PROC_START,			/*����*/
	PROC_RUN,			/*����*/
	PROC_ERROR,			/*�쳣�˳�*/
	PROC_STOP,			/*�����˳�*/
//	PROC_DEAD			/*����Ӧ*/
};

#pragma pack(1)

//���̹������ݽṹ
typedef struct
{
	WORD wCount;  //��������(0-PROC_MAX_SUM)
	WORD wflag;   //״̬��־ 0=���� 1=���� 2=����
	struct _tagProc
	{
		WORD  wProcType;  //�������� 0=��ͨ���� 1=��Ҫ���� 2=�ؼ�����
		WORD  wStartMode; //����ģʽ 0=������   1=�����ػ�
		LONG  lStartTime; //����ʱ��
		char  szDescribe[PROC_NAME_LEN];	//��������
		char  szExecPath[PROC_PATH_LEN];	//ִ��·��
		char  szProcName[PROC_NAME_LEN];	//��������
		char  szParam[PROC_PARA_LEN];	//��������
		//argv[0]�������֣�argv[1]���̲����ļ�ȫ·��,argv[2~6]�ý�����������
		char  *argv[PARA_MAX_SUM+2];

		pid_t hProcess;		//���̱�ʶ(pid)
		short nRunState;	//����״̬ 0=��Ч 1=���� 2=���� 3=�쳣 4=�˳�
		short nErrTimer;    //�����ʱ(��)
		short nRestoreNum;  //�ָ�����
	}proc[PROC_MAX_SUM];
}PROC_DB;

int GetRealSysMemory( ) ;
#pragma pack()

/*****************************************************************************/
#endif /* #ifndef _PROCDEF_H__ */
