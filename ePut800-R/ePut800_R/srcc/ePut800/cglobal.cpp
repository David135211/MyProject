#include "cglobal.h"
#include "time.h"
#include <sys/time.h>
#include <stdio.h>
#include "qpainter.h"
#include "Pages/cpage.h"
#include "Pages/cpage_home.h"
#include "Pages/cpage_sysparameter.h"
#include "Pages/cpage_diagnostic.h"
#include "Pages/cpage_dataquery.h"
#include "Pages/cpage_syspic.h"
#include "Element/ceelment.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>



CGlobal g_global ;
CGlobal::CGlobal()
{/*{{{*/
	m_pWidget = NULL ;
	m_byCurPage = Widget::PAGE_HOME ;
	m_strRealTime = "2016-06-05 11:10:22" ;
	m_bThreadRun = TRUE ;
	m_byLogState = CPage::LOGOUT ;
	m_wCountdownTime = 0 ;
	memset( UsrPwdPath , 0 , sizeof(UsrPwdPath) );
	strcpy( UsrPwdPath , "./config/UsrPwd" );
	memset( NetParamPath , 0 , sizeof(NetParamPath) );
	strcpy( NetParamPath , "./config/NetParam" );
	memset( SysDnsPath , 0 , sizeof(SysDnsPath) );
	strcpy( SysDnsPath , "/etc/resolv.conf" );

	memset( &DefaultUsrPwd , 0 , sizeof(USRPWD) );
	memset( &CurrentUsrPwd , 0 , sizeof(USRPWD) );
	strcpy( DefaultUsrPwd.Usr , "administrator" );
	strcpy( DefaultUsrPwd.Pwd , "123456" );
	strcpy( CurrentUsrPwd.Usr , "administrator" );
	strcpy( CurrentUsrPwd.Pwd , "123456" );

	memset( &DefaultNetParam , 0 , sizeof(NETPARAM) );
	memcpy(DefaultNetParam.ip , "192.168.1.136" , sizeof("192.168.1.136"));
	memcpy(DefaultNetParam.mask , "255.255.255.0" , sizeof("255.255.255.0"));
	memcpy(DefaultNetParam.gateway , "192.168.1.1" , sizeof("192.168.1.1"));
	memcpy(DefaultNetParam.dns , "192.168.1.1" , sizeof("192.168.1.1"));

	m_GpinFd = open(GPIN, O_WRONLY);;
	if (m_GpinFd == -1)
	{
		printf("fail to open %s!\n",GPIN);

	}
	m_GpoutFd = open(GPOUT, O_WRONLY);;
	if (m_GpoutFd == -1)
	{
		printf("fail to open %s!\n",GPOUT);

	}

	GpinCmd[0]=GPIO_GETMU;
	GpinCmd[1]=GPIO_GETOS;
	GpinCmd[2]=GPIO_GETBF;
	GpinCmd[3]=GPIO_GETBU;
	GpinCmd[4]=GPIO_GETMW;
	GpinCmd[5]=GPIO_GETBW;

	memset(GpioState,GPIOOFF,sizeof(GpioState));

	s_bstate = FALSE;
	gs_uisum_usec = 10 * 60 * 1000 * 1000;  // 等待的最长时间
	gs_uiadd_usec = 0;                 // 每次增加时间在此变量上
	gs_uisleep_usec = 1000 * 1000;      // 睡眠时间
	keys_fd = open ("/dev/input/event0", O_RDONLY | O_NONBLOCK);
	if (keys_fd <= 0)
	{
		printf ("open /dev/input/event0 device error!\n");
	}
	else
	{
		printf ("open /dev/input/event0 device OK!\n");
	}

	YkBeginTime = 0;
}/*}}}*/

BYTE CGlobal::GetLoginState( )
{/*{{{*/
	return m_byLogState ;
}/*}}}*/

void CGlobal::SetLoginState( BYTE byState )
{/*{{{*/
	if( byState != CPage::LOGIN && byState != CPage::LOGOUT )
	{
		Q_ASSERT( FALSE ) ;
		return ;
	}

	m_byLogState = byState ;
	emit LoginStateChanged(m_byLogState);
}/*}}}*/


QString CGlobal::GetRealTimeStr( )
{/*{{{*/
	return m_strRealTime ;
}/*}}}*/

void CGlobal::SetRealTimeStr( QString str )
{/*{{{*/
	if( str.size() <= 0 )
	{
		Q_ASSERT( FALSE ) ;
		return ;
	}

	m_strRealTime = str ;
}/*}}}*/

BOOL CGlobal::GetThreadRunFlag( )
{/*{{{*/
	return m_bThreadRun ;
}/*}}}*/

BOOL CGlobal::SetThreadRunFlag( BOOL bFlag )
{/*{{{*/
	m_bThreadRun = bFlag ;
	return TRUE ;
}/*}}}*/

time_t CGlobal::GetCountdownTime( )
{/*{{{*/
	return m_wCountdownTime ;
}/*}}}*/

void CGlobal::SetCountdownTime( time_t timeCount )
{/*{{{*/
	m_wCountdownTime = timeCount ;
}/*}}}*/

void CGlobal::RefreshGpioPowerState()
{/*{{{*/
	int level,BackPowerLevel;
	BYTE tmpGpiostate;
	BYTE YxState = 0;
	//主电北电
	for( int i = 0 ; i < 4 ; i++ )
	{   YxState = 0;
		if( i == 1 )//不判断备电故障时的短/断信号
			continue;
		if(ioctl(m_GpinFd, GpinCmd[i], &level) >= 0)
		{
			//printf("ioctl gpioinfo %d level %d \n", i, level);
			BYTE tmpIndex = GPIO_ERROR;
			switch(i)
			{
			case 0:
				tmpIndex = MAIN_POWER;
				if(level == 1)
					tmpGpiostate = GPIOON;
				else
					tmpGpiostate = GPIOOFF;
				break;
			case 1:
			case 2:
				BackPowerLevel = level;
				tmpIndex = GPIO_ERROR;
				break;
			case 3:
				tmpIndex = BACKUP_POWER;
				if( (BackPowerLevel == 1) && (level == 1) )
					tmpGpiostate = GPIOON;
				else
					tmpGpiostate = GPIOOFF;
				break;
			default:
				break;
			}
			if( tmpIndex == GPIO_ERROR )
				continue;
			if( GpioState[tmpIndex] == tmpGpiostate )
				continue;

			AfxSetGpioState( tmpIndex , tmpGpiostate );
			if( tmpGpiostate ==  GPIOON )
				YxState = 1;
			else if( tmpGpiostate ==  GPIOOFF )
				YxState = 0;
			if( tmpIndex == MAIN_POWER )
				AfxSetYx(TOTAL_BUS-1,1,0,YxState);
			else if( tmpIndex == BACKUP_POWER )
				AfxSetYx(TOTAL_BUS-1,1,1,YxState);
		}
	}
}/*}}}*/

void CGlobal::RefreshGpioFalutState()
{/*{{{*/
	if( (GpioState[MAIN_POWER] == GPIOON)
			&& (GpioState[BACKUP_POWER] == GPIOON) )
	{
		//printf("GpioState[FAULT] %d off %d\n",GpioState[FAULT],GPIOOFF);
		if(GpioState[FAULT] == GPIOON)
			AfxSetGpioState( FAULT , GPIOOFF );
	}
	else
	{
		if(GpioState[FAULT] == GPIOOFF)
			AfxSetGpioState( FAULT , GPIOON );
	}
}/*}}}*/

void CGlobal::RefreshGpioWorkingState()
{/*{{{*/
	if(GpioState[WORKING] == GPIOON)
		AfxSetGpioState( WORKING , GPIOOFF );
	else if(GpioState[WORKING] == GPIOOFF)
		AfxSetGpioState( WORKING , GPIOON );
}/*}}}*/

void CGlobal::RefreshGpioWarnState()
{/*{{{*/

}/*}}}*/

//Set指示灯状态,state 0-On 1-Off
void CGlobal::AfxSetGpioState( BYTE index , BYTE State)
{/*{{{*/
	CGlobal * pGlobal = AfxGetGlobal() ;
	if( !pGlobal )
	{
		Q_ASSERT( FALSE );
		return ;
	}
	char data[2]="";
	data[0] = index;
	data[1] = State;
	if(write(pGlobal->m_GpoutFd, data, sizeof(data)) <= 0)
	{
		printf("Write Fail :set gpioinfo %d level %d \n", data[0], data[1]);
		return ;
	}
	pGlobal->GpioState[ index ] = State;
	//printf("set gpioinfo %d level %d \n", data[0], data[1]);
}/*}}}*/

void CGlobal::SetStandby( BOOL bstate )
{/*{{{*/
	// 如果现在的状态和输入的状态相同，则不进行重新设置;
	if (bstate == s_bstate )
	{
		return;
	}

	if (bstate) //
	{
		/* 设置待机 */
		system( "echo \"4\" > /sys/class/graphics/fb0/blank");
	}
	else
	{
		/* 设置不待机 */
		system( "echo \"0\" > /sys/class/graphics/fb0/blank" );
	}

	s_bstate = bstate;
}/*}}}*/

void CGlobal::RefreshScreenState()
{/*{{{*/
	while(1)
	{
		// 如果是0xffffffff 则为不待机状态
		if (0xffffffff == gs_uisum_usec )
		{
			SetStandby( FALSE );
			usleep( 2 * gs_uisleep_usec );
			break;
		}

		if (read (keys_fd, &t, sizeof (t)) == sizeof (t))
		{

			if (t.type == EV_ABS )
			{
				gs_uiadd_usec = 0;
				SetStandby( FALSE );
			}
			// 这里使用continue是使点击事件快速结束
			//printf(" t= %d %d %d\n", t.type, t.code, t.value );
			continue;
		}//perror("read:");
		// 大于等于最大时间就待机
		//printf("%d>=%d\n",gs_uiadd_usec,gs_uisum_usec);
		if (gs_uiadd_usec >= gs_uisum_usec )
		{
			SetStandby( TRUE );
			gs_uiadd_usec = gs_uisum_usec + gs_uisleep_usec;
		}

		// printf("usec=%d %d\n", gs_uiadd_usec, gs_uisum_usec);

		gs_uiadd_usec += gs_uisleep_usec;
		//usleep( gs_uisleep_usec );
		break;
	}
}/*}}}*/

void CGlobal::EmitSignal_YkRtnInRealThread(WORD wBusNoDest, WORD wAddrDest, WORD nPntDest, WORD wBusNoSrc, WORD wAddrSrc, WORD nPntSrc, BYTE byAction, WORD wVal)
{/*{{{*/
	/*printf("EmitSignal_YkRtnInRealThread %d %d %d %d %d %d %d %d\n",wBusNoDest,wAddrDest,nPntDest,wBusNoSrc,wAddrSrc,nPntSrc,byAction,wVal);*/
	emit Signal_YkRtnInRealThread(wBusNoDest,wAddrDest,nPntDest,
			wBusNoSrc,wAddrSrc,nPntSrc,
			byAction,wVal);
}/*}}}*/

void CGlobal::EmitSignal_YkRtnTimeOut()
{/*{{{*/
	emit Signal_YkRtnTimeOut();
}/*}}}*/

void CGlobal::EmitSignal_YxAlarm(char *chName, char byValue)
{/*{{{*/
	emit Signal_YxAlarm(chName,byValue);
}/*}}}*/

BYTE AfxGetLoginState( )
{/*{{{*/
	return AfxGetGlobal()->GetLoginState() ;
}/*}}}*/

void AfxSetLoginState( BYTE byLoginState )
{/*{{{*/
	AfxGetGlobal()->SetLoginState( byLoginState );

	if( byLoginState == CPage::LOGOUT )
	{
		Widget * pWidget = AfxGetWidget() ;
		CPage * pPage = AfxGetCurPagePointer();
		if( !pWidget || !pPage )
		{
			Q_ASSERT( FALSE );
			return ;
		}

		QRect rc( pPage->m_rcLoginInfo ) ;
		rc = rc.united( pPage->m_rcLoginPic ) ;
		pWidget->update( rc );
	}

}/*}}}*/

CGlobal * AfxGetGlobal( )
{/*{{{*/
	return &g_global ;
}/*}}}*/

CPage * AfxGetSpecifyPage( BYTE byPageIndex )
{/*{{{*/
	Widget * pWidget = AfxGetWidget() ;
	if( !pWidget )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	CPage * pPage = NULL ;
	BOOL bFlag = pWidget->m_PageMag.contains( byPageIndex );
	if( bFlag )
		pPage = pWidget->m_PageMag[ byPageIndex ] ;

	return pPage ;
}/*}}}*/

Widget * AfxGetWidget( )
{/*{{{*/
	return g_global.m_pWidget ;
}/*}}}*/

BOOL AfxSetMainWidet( Widget * pWidget )
{/*{{{*/
	if( !pWidget )
		return FALSE ;

	g_global.m_pWidget = pWidget ;

	return TRUE ;
}/*}}}*/

BOOL AfxSetCurPageIndex( BYTE byIndex )
{/*{{{*/
	Widget * pWidget = AfxGetWidget() ;
	if( !pWidget )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	BOOL bFlag = pWidget->m_PageMag.contains( byIndex ) ;
	if( !bFlag )
		return FALSE ;

	g_global.m_byCurPage = byIndex ;

	return TRUE ;
}/*}}}*/

BYTE AfxGetCurPageIndex ( )
{/*{{{*/
	return g_global.m_byCurPage ;
}/*}}}*/

void AfxRefreshScreen( )
{
	int cx = Widget::HOR_SIZE ;
	int cy = Widget::VER_SIZE ;
	AfxGetWidget()->update( QRect( 0 , 0 ,cx , cy ) );
}

//获得系统时间
BOOL AfxGetSysTime( PREALTIME pTime )
{
	if( pTime == NULL )
		return FALSE ;

	time_t     lSecond;
	struct tm  currTime;
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	lSecond = (time_t)(tv.tv_sec);
	localtime_r( &lSecond, &currTime );
	pTime->wMilliSec = tv.tv_usec/1000;
	pTime->wSecond   = currTime.tm_sec;
	pTime->wMinute   = currTime.tm_min;
	pTime->wHour     = currTime.tm_hour;
	pTime->wDay      = currTime.tm_mday;
	pTime->wMonth    = 1+currTime.tm_mon;
	pTime->wYear     = 1900+currTime.tm_year;
	pTime->wDayOfWeek = currTime.tm_wday ;
	return TRUE ;
}

BOOL AfxSetSysTime(REALTIME *pRealTime)
{
	if( pRealTime == NULL )
		return FALSE;
	struct timeval  tv;

	struct tm SetTime_Now;
	SetTime_Now.tm_year = pRealTime->wYear-1900;
	SetTime_Now.tm_mon = pRealTime->wMonth-1;
	SetTime_Now.tm_mday = pRealTime->wDay;
	SetTime_Now.tm_hour = pRealTime->wHour;
	SetTime_Now.tm_min = pRealTime->wMinute;
	SetTime_Now.tm_sec = pRealTime->wSecond;

	tv.tv_sec  = mktime( &SetTime_Now );
	tv.tv_usec  = pRealTime->wMilliSec*1000;

	int rtn = settimeofday(&tv, NULL);
	if( rtn == 0 )
	{
		system("hwclock -w");
		return TRUE;
	}
	return FALSE;
}


QString AfxGetSysTimeStr( )
{
	REALTIME tm ;
	AfxGetSysTime( &tm ) ;

	QString strTime = AfxFormatStr("%04d-%02d-%02d %02d:%02d:%02d" ,
			tm.wYear , tm.wMonth , tm.wDay , tm.wHour , tm.wMinute , tm.wSecond );

	return strTime ;
}

QString AfxFormatStr( const char * pString , ... )
{
	if( pString == NULL )
		return QString( "" ) ;

	int len = strlen( pString ) ;
	if( len >= 2048 )
		return QString( "" ) ;

	char buf[ 2048 ] ;
	memset( buf , 0 , sizeof( buf ) ) ;

	va_list vaList ;
	va_start ( vaList , pString ) ;
	vsprintf( buf , pString , vaList ) ;
	va_end( vaList ) ;

	QString str ;
	str = UTF8( buf ) ;
	return str ;
}

void AfxAsleep( DWORD dwMilliSecd )
{
	struct timeval tv;
	tv.tv_sec = 0 ;
	tv.tv_usec = dwMilliSecd * 1000 ;
	select(0, NULL, NULL, NULL, &tv);
}

BOOL AfxRefreshHomePageTime( )
{
	BYTE byIndex = AfxGetCurPageIndex() ;
	if( byIndex != Widget::PAGE_HOME )
		return FALSE ;

	CPage * pPage = AfxGetSpecifyPage( byIndex ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	REALTIME tm ;
	AfxGetSysTime( &tm ) ;
	static WORD wSec = 0xFFFF ;
	if( wSec != tm.wSecond )
		wSec = tm.wSecond ;
	else
		return FALSE ;

	QString strTime = AfxGetSysTimeStr() ;
	AfxGetGlobal()->SetRealTimeStr( strTime );

	QRect rcDate = (( CPage_Home * )pPage)->m_rcDate ;

	AfxGetWidget()->update( rcDate );
	return TRUE ;
}

BOOL AfxRefreshDiagnosticPageBusAndDevState( )
{
	BYTE byIndex = AfxGetCurPageIndex() ;
	if( byIndex != Widget::PAGE_DIOGNOSTIC )
		return FALSE ;

	CPage * pPage = AfxGetSpecifyPage( byIndex ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	CPage_Diagnostic *tmpDiagnostic = ( CPage_Diagnostic * )pPage;

	if( tmpDiagnostic->DiagnosticOption == tmpDiagnostic->DATAGRAM_PAGE )
		AfxGetWidget()->update( tmpDiagnostic->RxTxRect );
	else
		AfxGetWidget()->update( tmpDiagnostic->DevStateRect );



	return TRUE;
}

//刷新动态着色
BOOL AfxRefreshPicColor(PST_SYSPAGE CurPagest ,CElement * FElement)
{
	Widget *tmpWidget = AfxGetWidget();

	if( FElement->IsChangeColor() )
	{
		PPICNODE tmpNodeArray = CurPagest->m_AllElementNodeToPicId_Map[FElement->m_wObjectNodeNext];
		if( tmpNodeArray == NULL )
		{
			printf("FElement->m_wObjectNodeNext %d tmpNodeArray == NULL\n",FElement->m_wObjectNodeNext);
			return FALSE;
		}
		int CurNodeElementSize = tmpNodeArray->PicId_Array.size();
		WORD ElementId = 0;
		CElement * tmpElement = NULL;
		for( int j = 0; j < CurNodeElementSize; j++ )
		{
			ElementId = tmpNodeArray->PicId_Array.at(j);
			tmpElement = AfxGetWidget()->m_objAllElement[ ElementId ];
			if( tmpElement == NULL )
			{
				printf("AfxGetWidget()->m_objAllElement[ %d ]; == NULL\n",ElementId);
				continue;
			}
			else if( tmpElement == FElement )
			{
				continue;
			}
			//printf("type %d id %d\n",tmpElement->GetObjectType(),tmpElement->m_wObjectID);
			tmpElement->SetChangeNodeColor(FElement->m_wObjectNodeNext,FElement->m_wObjectNodeNextState);
			tmpWidget->update(tmpElement->GetQRect());
			switch(tmpElement->GetObjectType())
			{
			case CElement::BREAKER:
			case CElement::CONTACTOR:
			case CElement::LOAD_BREAKER_2POS:
			case CElement::LOAD_BREAKER_3POS:
			case CElement::SWITCH_2POS:
			case CElement::SWITCH_3POS:
			case CElement::SWITCH_CONNECT_EARTH:
				AfxRefreshPicColor(CurPagest,tmpElement);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		printf(" FElement->IsChangeColor() is FALSE!\n");
	}

	return TRUE;
}

//刷新图元状态
BOOL AfxRefreshPicState()
{
	BYTE byIndex = AfxGetCurPageIndex() ;
	if( byIndex != Widget::PAGE_SYS_PIC )
		return FALSE ;

	CPage * pPage = AfxGetSpecifyPage( byIndex ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	Widget *tmpWidget = AfxGetWidget();
	CPage_SysPic *tmpSysPic = ( CPage_SysPic* )pPage;

	PST_SYSPAGE CurPagest = tmpSysPic->GetSysPage_Strcut(tmpSysPic->GetCurPage());
	if( !CurPagest )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	int CurPagElementSize = CurPagest->m_EleArray.size();
	CElement * FElement = NULL;
	for( int i = 0; i < CurPagElementSize; i++ )
	{
		FElement = CurPagest->m_EleArray.at(i);
		if( FElement == NULL )
		{
			Q_ASSERT( FALSE ) ;
			return FALSE;
		}
#if 0
		/*对于eput800_r着色不需要!*/
		if( FElement->CompareAttributeData() )
		{
			tmpWidget->update(FElement->GetQRect());
			switch(FElement->GetObjectType())
			{
			case CElement::BREAKER:
			case CElement::CONTACTOR:
			case CElement::LOAD_BREAKER_2POS:
			case CElement::LOAD_BREAKER_3POS:
			case CElement::SWITCH_2POS:
			case CElement::SWITCH_3POS:
			case CElement::SWITCH_CONNECT_EARTH:
//			case CElement::IMAGE_FOR_AI:
//			case CElement::ALARM_FOR_AI:
				AfxRefreshPicColor(CurPagest,FElement);
				break;
			default:
				break;
			}

		}
#endif
	}

	return TRUE;
}


//刷新指示灯状态
void AfxRefreshGpioState()
{
	CGlobal * pGlobal = AfxGetGlobal() ;
	if( !pGlobal )
	{
		Q_ASSERT( FALSE );
		return ;
	}
	pGlobal->RefreshGpioWarnState();
	pGlobal->RefreshGpioWorkingState();
	pGlobal->RefreshGpioPowerState();
	pGlobal->RefreshGpioFalutState();

}

//数据查询刷新实时状态
BOOL AfxRefreshDataQueryState()
{
	BYTE byIndex = AfxGetCurPageIndex() ;
	if( byIndex != Widget::PAGE_DATA_QUERY )
		return FALSE ;

	CPage * pPage = AfxGetSpecifyPage( byIndex ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	AfxGetWidget()->update( QRect( 546,165,150,350 ) );

	return TRUE;
}

BOOL AfxRefreshSysParamPageSysTime( )
{
	BYTE byIndex = AfxGetCurPageIndex() ;
	if( byIndex != Widget::PAGE_SYS_PARA )
		return FALSE ;

	CPage * pPage = AfxGetSpecifyPage( byIndex ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}
	CPage_SysParameter * tmpSysParam = ( CPage_SysParameter * )pPage;

	if( tmpSysParam->SysParamOption == CPage_SysParameter::SYS_TIME )
	{
		if( tmpSysParam->Is_Refresh_SysParamTime == TRUE )
		{
			REALTIME tm ;
			AfxGetSysTime( &tm ) ;
			if( tmpSysParam->SysParamPageRectText[CPage_SysParameter::YEAR_INPUT_RECT].toShort() != tm.wYear )
			{
				tmpSysParam->SysParamPageRectText[CPage_SysParameter::YEAR_INPUT_RECT] = QString::number(tm.wYear);
				AfxGetWidget()->update( tmpSysParam->SysParamPageRect[CPage_SysParameter::YEAR_INPUT_RECT] );
			}
			if( tmpSysParam->SysParamPageRectText[CPage_SysParameter::MONTH_INPUT_RECT].toShort() != tm.wMonth )
			{
				tmpSysParam->SysParamPageRectText[CPage_SysParameter::MONTH_INPUT_RECT] = QString::number(tm.wMonth);
				AfxGetWidget()->update( tmpSysParam->SysParamPageRect[CPage_SysParameter::MONTH_INPUT_RECT] );
			}
			if( tmpSysParam->SysParamPageRectText[CPage_SysParameter::DAY_INPUT_RECT].toShort() != tm.wDay )
			{
				tmpSysParam->SysParamPageRectText[CPage_SysParameter::DAY_INPUT_RECT] = QString::number(tm.wDay);
				AfxGetWidget()->update( tmpSysParam->SysParamPageRect[CPage_SysParameter::DAY_INPUT_RECT] );
			}
			if( tmpSysParam->SysParamPageRectText[CPage_SysParameter::HOUR_INPUT_RECT].toShort() != tm.wHour )
			{
				tmpSysParam->SysParamPageRectText[CPage_SysParameter::HOUR_INPUT_RECT] = QString::number(tm.wHour);
				AfxGetWidget()->update( tmpSysParam->SysParamPageRect[CPage_SysParameter::HOUR_INPUT_RECT] );
			}
			if( tmpSysParam->SysParamPageRectText[CPage_SysParameter::MINUTE_INPUT_RECT].toShort() != tm.wMinute )
			{
				tmpSysParam->SysParamPageRectText[CPage_SysParameter::MINUTE_INPUT_RECT] = QString::number(tm.wMinute);
				AfxGetWidget()->update( tmpSysParam->SysParamPageRect[CPage_SysParameter::MINUTE_INPUT_RECT] );
			}
			if( tmpSysParam->SysParamPageRectText[CPage_SysParameter::SECOND_INPUT_RECT].toShort() != tm.wSecond )
			{
				tmpSysParam->SysParamPageRectText[CPage_SysParameter::SECOND_INPUT_RECT] = QString::number(tm.wSecond);
				AfxGetWidget()->update( tmpSysParam->SysParamPageRect[CPage_SysParameter::SECOND_INPUT_RECT] );
			}
		}
	}
	return TRUE ;
}


void AfxRefreshLoginState( )
{
	BYTE byLoginState = AfxGetLoginState() ;
	if( byLoginState == CPage::LOGOUT )
	{
		AfxGetGlobal()->SetCountdownTime( 0 );
		return ;
	}

	REALTIME tm ;
	AfxGetSysTime( &tm ) ;
	static WORD wSec = 0xFFFF ;
	if( wSec != tm.wSecond )
		wSec = tm.wSecond ;
	else
		return ;

	CPage * pPage = AfxGetCurPagePointer() ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return  ;
	}

	Widget * pWidget = AfxGetWidget();
	if( !pWidget )
	{
		Q_ASSERT( FALSE );
		return  ;
	}

	time_t timeBegin = AfxGetGlobal()->GetCountdownTime();
	if( !timeBegin )
	{
		time_t tBegin = 0 ;
		time( &tBegin ) ;
		AfxGetGlobal()->SetCountdownTime( tBegin );
	}

	QRect rc( pPage->m_rcLoginInfo ) ;
	rc = rc.united( pPage->m_rcLoginPic ) ;
	pWidget->update( rc );
}

BOOL AfxRefreshScreenState()
{
	AfxGetGlobal()->RefreshScreenState();
	return TRUE;
}

void AfxRefreshYkTime()
{
	time_t tmpCurTime=0;
	if( AfxGetGlobal()->YkBeginTime != 0 )
	{
		time(&tmpCurTime);
		if( abs(tmpCurTime-AfxGetGlobal()->YkBeginTime) > 30 )
		{
			AfxGetGlobal()->YkBeginTime = 0;
			AfxGetGlobal()->EmitSignal_YkRtnTimeOut();
		}
	}

}

CPage * AfxGetCurPagePointer( )
{
	BYTE byIndex = AfxGetCurPageIndex() ;
	CPage * pPage = AfxGetSpecifyPage( byIndex ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return pPage ;
}

PSTHREAD_PROC AfxGetThreadInfo( pthread_t tid )
{
	Widget * pWidget = AfxGetWidget() ;
	if( !pWidget )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	QTHREAD_ARRAY * pMap = &pWidget->m_ThreadArray ;

	int size = pMap->size() ;
	if( size == 0 )
		return NULL ;

	QTHREAD_ARRAY::const_iterator itor = pMap->find( tid );
	if( itor == pMap->end() )
		return NULL ;
	else
		return itor.value( );
}

void AfxUpdateTimeoutTime( )
{
	BYTE byState = AfxGetLoginState() ;
	if( byState == CPage::LOGOUT )
		return ;

	DWORD dwMsgtype = CMsg::MSG_REAL_CHANNEL ;
	PSTHREAD_PROC pThread = AfxGetThreadInfo( dwMsgtype ) ;
	if( !pThread )
		return ;

	PUPD_LOGTime pTime = new UPD_LOGTime ;
	AfxSendMsg( pThread->pMsg , pTime ) ;
}

BOOL AfxSendMsg( CMsg * pMsg ,LPVOID pVoid )
{
	if( !pMsg || !pMsg->IsMsgQueue() )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}


	LMSG msg ;
	msg.pVoid = ( LPVOID )pVoid ;
	pMsg->SendMsg( &msg );

	return TRUE ;
}


void * ThreadRealProc( void * pParam )
{
	//pthread_cancel( ThreadId );
	//pthread_join( ThreadId, NULL  );
	//pthread_exit(0);
	if( !pParam )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	//use sleep wait for app has completed
	//we can use flag that app has completed instead of sleep
	AfxAsleep( 1000 );

	PSTHREAD_PROC pTData = ( PSTHREAD_PROC )pParam ;
	CGlobal * pGlobal = AfxGetGlobal() ;
	if( !pGlobal || !pTData )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	CMsg * pMsg = pTData->pMsg ;
	if( !pMsg || !pMsg->IsMsgQueue() )
	{
		pthread_exit( 0 );
		return NULL ;
	}

	int CirNo=0;
	while( pGlobal->GetThreadRunFlag() )
	{

		if( !pMsg->IsMsgQueue() )
			break;

		LMSG RMsg ;
		if( pMsg->RecvMsg( &RMsg ) )
		{
			PSMSG pMsg = ( PSMSG )RMsg.pVoid ;
			if( !pMsg )
			{
				sleep( 1 ) ;
				continue ;
			}
			if( pMsg->msgCustomType == SMSG::MSG_YK )
			{
				PYKST_MSG pYkmsg = ( PYKST_MSG )pMsg ;
				AfxGetGlobal()->EmitSignal_YkRtnInRealThread(pYkmsg->wBusNoDest,pYkmsg->wAddrDest,pYkmsg->wPntNoDest,
						pYkmsg->wBusNoSrc,pYkmsg->wAddrSrc,pYkmsg->wPntNoSrc,
						pYkmsg->byAction,pYkmsg->wVal);
				//                emit AfxGetGlobal()->Signal_YkRtnInRealThread(pYkmsg->wBusNoDest,pYkmsg->wAddrDest,pYkmsg->wPntNoDest,
				//                                                              pYkmsg->wBusNoSrc,pYkmsg->wAddrSrc,pYkmsg->wPntNoSrc,
				//                                                              pYkmsg->byAction,pYkmsg->wVal);
			}
			else if( pMsg->msgCustomType == SMSG::MSG_UPD_LOGINTIME )
				AfxGetGlobal()->SetCountdownTime( 0 );
			if( pMsg )
			{
				delete pMsg ;
				pMsg = NULL ;
			}
		}

		//刷新yk操作时间，30s超时
		AfxRefreshYkTime();

		//刷新主页时间
		AfxRefreshHomePageTime() ;

		//elapse time
		AfxRefreshLoginState( ) ;

		//刷新sysparam时间
		AfxRefreshSysParamPageSysTime();


		//刷新图元状态
		AfxRefreshPicState();

		if( CirNo == 0 )
		{
			//刷新指示灯状态
			AfxRefreshGpioState();

			//数据查询刷新实时状态
			AfxRefreshDataQueryState();

			//刷新屏幕待机
			AfxRefreshScreenState();

			//刷新装置状态
			AfxRefreshDiagnosticPageBusAndDevState();
		}

		AfxAsleep( 100 );
		CirNo++;
		if( CirNo > 10 )
			CirNo = 0;
	}

	pthread_exit( 0 );
	return NULL ;
}

void * ThreadComProc( void * pParam )
{
	//pthread_cancel( ThreadId );
	//pthread_join( ThreadId, NULL  );
	//pthread_exit(0);
	if( !pParam )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	//use sleep wait for app has completed
	//we can use flag that app has completed instead of sleep
	AfxAsleep( 1000 );

	PBUS_LINEST pBus = ( PBUS_LINEST )pParam ;
	CGlobal * pGlobal = AfxGetGlobal() ;
	if( !pGlobal || !pBus || !pBus->pThreadProc
			|| !pBus->pThreadProc->pMsg )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	CMsg * pMsg = pBus->pThreadProc->pMsg ;
	CProtocol * pBusProto = pBus->pProto ;
	CComPort * pComPort = pBus->pProperty->pComPort ;
	if( !pBusProto ||!pMsg || !pMsg->IsMsgQueue() )
	{
		pthread_exit( 0 );
		return NULL ;
	}

	if( !pComPort )
	{
		pthread_exit( 0 );
		return NULL ;
	}

	while( pGlobal->GetThreadRunFlag() )
	{

		if( !pMsg->IsMsgQueue() )
			break;

		LMSG RMsg ;
		if( pMsg->RecvMsg( &RMsg ) )
		{
			PSMSG pMsg = ( PSMSG )RMsg.pVoid ;
			if( !pMsg )
			{
				sleep( 1 ) ;
				continue ;
			}

			AfxComPortSpecify( pMsg ) ;

			if( pMsg )
			{
				delete pMsg ;
				pMsg = NULL ;
			}
		}

		AfxComPortProcess( pBus ) ;
		AfxAsleep( 10 );
	}

	pthread_exit( 0 );
	return NULL ;
}

BOOL AfxComPortSpecify( PSMSG pMsg )
{
	if( !pMsg )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	if( pMsg->msgCustomType == SMSG::MSG_YK)
	{
		AfxProcessYk( ( PYKST_MSG )pMsg );
	}
	return TRUE ;
}

BOOL AfxProcessYk( PYKST_MSG pYk_msg )
{
	PYKST_MSG pYkmsg = pYk_msg ;
	if( !pYkmsg )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	PBUS_LINEST pBus = AfxGetBus( pYkmsg->wBusNoDest ) ;
	if( !pBus )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	AfxComPortProcess( pBus , pYkmsg ) ;
	return TRUE ;
}

//check
//send recv msg counts
//communacation msg
BOOL AfxComPortProcess( PBUS_LINEST pBus , PSMSG pMsg)
{
	if( !pBus )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}
	AfxSetBusTransClientState( pBus );
	CProtocol * pBusProto = pBus->pProto ;
	CComPort * pComPort = pBus->pProperty->pComPort ;
	if( !pBusProto || !pComPort || !pComPort->IsPortValid() )
		return FALSE ;

	WORD wInterval = pBus->wInterval ;
	CProtocol * pProto = NULL ;
	if( !pMsg )
		pProto = AfxGetDevProto( pBusProto ) ;
	else
		pProto = AfxGetSpecifyProto( pMsg ) ;

	if( !pProto )
		return FALSE ;
	BYTE buf[ CGlobal::BUFFERSIZE ];
	memset( buf , 0 ,sizeof( buf ) );
	int len = 0 ;
	BYTE byReserved = 0 ;
	BOOL bFlag = pProto->GetReadBuf( buf , len , byReserved , pMsg ) ;
	AfxDisplayMsg( pBus->wBusNo ,pProto->m_wAddr , CGlobal::SEND_MSG , buf , len ) ;
//	printf( "bFlag=%d -------------\n", bFlag);
	if( bFlag )
	{
		if( pComPort->WritePort( buf , len ) )
		{
			if( pBus->dwSendNo >= 100000 )
				pBus->dwSendNo = 0 ;
			else
				pBus->dwSendNo++ ;
		}
	}

	AfxAsleep( wInterval );
	memset(buf,0,sizeof(buf));
	int iRecvLen = pComPort->AsyReadData( buf , sizeof( buf ) ) ;
	if( !iRecvLen )
	{

		return FALSE ;
	}

	if( pBus->dwRecvNo >= 100000 )
		pBus->dwRecvNo = 0 ;
	else
		pBus->dwRecvNo++ ;

	bFlag = pProto->ProcessBuf( buf , iRecvLen , byReserved , pMsg ) ;
	AfxDisplayMsg( pBus->wBusNo ,pProto->m_wAddr , CGlobal::RECV_MSG , buf , iRecvLen ) ;

	if( !bFlag )
	{
		int iLen = pProto->GetErrorMsg( buf , sizeof( buf ) ) ;
		if( iLen )
			AfxDisplayMsg( pBus->wBusNo ,pProto->m_wAddr , CGlobal::ERROR_MSG , buf , iLen );
	}

	return TRUE ;
}

CProtocol * AfxGetSpecifyProto( PSMSG _pMsg )
{
	PSMSG pMsg = _pMsg ;
	if( !pMsg )
	{
		Q_ASSERT( FALSE ) ;
		return NULL;
	}

	PYKST_MSG pYkmsg = NULL ;
	if( pMsg->msgCustomType == SMSG::MSG_YK)
		pYkmsg = ( PYKST_MSG )pMsg ;

	if( !pYkmsg )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	PBUS_LINEST pBus = AfxGetBus( pYkmsg->wBusNoDest ) ;
	if( !pBus )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	if( !pBus->pProto )
		return NULL ;

	int size = pBus->pProto->m_ProtoModule.size() ;
	for( int i = 0 ; i < size ; i++ )
	{
		CProtocol * pProto = pBus->pProto->m_ProtoModule[ i ] ;
		if( !pProto )
			return NULL ;

		if( pProto->GetProtoAddr() == pYkmsg->wAddrDest )
			return pProto ;
	}

	return NULL ;
}


void AfxDisplayMsg( WORD wBusNo , WORD wAddr , BYTE byType , BYTE * pBuf , int len )
{
	BYTE byIndex = AfxGetCurPageIndex();
	if( byIndex != Widget::PAGE_DIOGNOSTIC )
		return ;

	CPage * pPage = AfxGetCurPagePointer() ;
	if( !pPage )
	{
		Q_ASSERT( FALSE );
		return ;
	}

	CPage_Diagnostic *pPageDiagnostic = ( CPage_Diagnostic * )pPage ;
	/*BOOL bFlag = */
	pPageDiagnostic->DisplayMsg(  wBusNo ,  wAddr , byType , pBuf , len ) ;

	return ;
}

CProtocol * AfxGetDevProto( CProtocol *pBusProto )
{
	if( !pBusProto )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	BYTE byProtoType = pBusProto->GetProtoType();
	if( byProtoType == BUS_PAUSE )
		return NULL ;

	//transProto mustn't have one more devices
	int size = pBusProto->m_ProtoModule.size() ;
	if( ( byProtoType == BUS_TRANS && size > 1 ) || !size )
		return NULL ;

	WORD wCirculateCounter = pBusProto->m_circulateCounter ;
	if( wCirculateCounter >= size )
		wCirculateCounter = 0 ;

	CProtocol * pProto = NULL ;
	pProto = pBusProto->m_ProtoModule[ wCirculateCounter ] ;

	wCirculateCounter++ ;
	pBusProto->m_circulateCounter = wCirculateCounter ;
	return pProto ;
}

BOOL AfxReadPwd( )
{
	int fd = 0;
	int bytes_read = 0 ;
	char buffer[ 64 ] = "";

	CGlobal * tmpGlobal = AfxGetGlobal();
	/* 创建目的文件 */
	if ( (fd = open(tmpGlobal->UsrPwdPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1 )
	{
		fprintf(stderr, "AfxReadPwd Open %s Error：%s\n", tmpGlobal->UsrPwdPath, strerror(errno));
		return FALSE;
	}
	if( (bytes_read = read(fd, buffer, sizeof(buffer))) == -1 )
	{
		fprintf(stderr, "AfxReadPwd Read %s Error：%s\n", tmpGlobal->UsrPwdPath, strerror(errno));
		close(fd);
		return FALSE;
	}
	else
	{
		memset( &tmpGlobal->CurrentUsrPwd , 0 , sizeof(USRPWD) );
		if( bytes_read == 0 )
		{
			memcpy( &tmpGlobal->CurrentUsrPwd , &tmpGlobal->DefaultUsrPwd ,sizeof(USRPWD) );
			if( 0 < write( fd , &tmpGlobal->DefaultUsrPwd , sizeof(USRPWD)) )
			{
				fprintf(stderr, "AfxReadPwd Write %s Error：%s\n", tmpGlobal->UsrPwdPath, strerror(errno));
				close(fd);
				return FALSE;
			}
		}
		else
		{
			memcpy( &tmpGlobal->CurrentUsrPwd , buffer , sizeof( USRPWD )  );
		}
	}
	close(fd);
	return TRUE;
}

BOOL AfxWritePwd( PUSRPWD VevisePwd )
{
	int fd = 0;
	int bytes_write = 0 ;
	//char buffer[ 64 ] = "";

	CGlobal * tmpGlobal = AfxGetGlobal();

	/* 创建目的文件 */
	if ( (fd = open(tmpGlobal->UsrPwdPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1 )
	{
		fprintf(stderr, "AfxWritePwd Open %s Error：%s\n", tmpGlobal->UsrPwdPath, strerror(errno));
		return FALSE;
	}
	if( (bytes_write = write(fd, VevisePwd, sizeof(USRPWD))) == -1 )
	{
		fprintf(stderr, "AfxWritePwd write %s Error：%s\n", tmpGlobal->UsrPwdPath, strerror(errno));
		close(fd);
		return FALSE;
	}
	else if( bytes_write > 0 )
	{
		fprintf(stderr, "AfxWritePwd write %s sucess\n", tmpGlobal->UsrPwdPath);
	}
	memset( &tmpGlobal->CurrentUsrPwd , 0 , sizeof(USRPWD) );
	memcpy( &tmpGlobal->CurrentUsrPwd , VevisePwd ,sizeof(USRPWD) );
	close(fd);
	return TRUE;
}

BOOL AfxResetPwd( )
{
	USRPWD ResetUsrPwd;
	CGlobal * tmpGlobal = AfxGetGlobal();
	memset( &ResetUsrPwd , 0 , sizeof(USRPWD) );
	memcpy( &ResetUsrPwd , &tmpGlobal->DefaultUsrPwd , sizeof(USRPWD) );
	AfxWritePwd( &ResetUsrPwd );
	return TRUE;
}

BOOL AfxReadNetParamFile( )
{
	int fd = 0;
	int bytes_read = 0 ;
	char buffer[ 128 ] = "";
	//NETPARAM tmpNetParam;

	CGlobal * tmpGlobal = AfxGetGlobal();
	/* 创建目的文件 */
	if ( (fd = open(tmpGlobal->NetParamPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1 )
	{
		fprintf(stderr, "AfxReadNetParamFile Open %s Error：%s\n", tmpGlobal->NetParamPath, strerror(errno));
		memset(&tmpGlobal->CurrentNetParam, 0 , sizeof(NETPARAM));
		memcpy(&tmpGlobal->CurrentNetParam , &tmpGlobal->DefaultNetParam , sizeof(NETPARAM));
		close(fd);
		if(AfxWriteNetParamFile(&tmpGlobal->DefaultNetParam))
			AfxSetNetParam(&tmpGlobal->CurrentNetParam);
		return FALSE;
	}
	if( (bytes_read = read(fd, buffer, sizeof(buffer))) == -1 )
	{
		fprintf(stderr, "AfxReadNetParamFile Read %s Error：%s\n", tmpGlobal->NetParamPath, strerror(errno));
		memset(&tmpGlobal->CurrentNetParam, 0 , sizeof(NETPARAM));
		memcpy(&tmpGlobal->CurrentNetParam , &tmpGlobal->DefaultNetParam , sizeof(NETPARAM));
		close(fd);
		if(AfxWriteNetParamFile(&tmpGlobal->DefaultNetParam))
			AfxSetNetParam(&tmpGlobal->CurrentNetParam);
		return FALSE;
	}
	else
	{
		int len = sizeof(NETPARAM);
		if( bytes_read == 0  || bytes_read != len )
		{
			fprintf(stderr, "AfxReadNetParamFile Read len %s Error：%s\n", tmpGlobal->NetParamPath, strerror(errno));
			memset(&tmpGlobal->CurrentNetParam, 0 , sizeof(NETPARAM));
			memcpy(&tmpGlobal->CurrentNetParam , &tmpGlobal->DefaultNetParam , sizeof(NETPARAM));
			close(fd);
			if(AfxWriteNetParamFile(&tmpGlobal->DefaultNetParam))
				AfxSetNetParam(&tmpGlobal->CurrentNetParam);
			return FALSE;
		}
		else
		{
			fprintf(stderr, "AfxReadNetParamFile  %s sucess%s\n", tmpGlobal->NetParamPath, strerror(errno));
			memset( &tmpGlobal->CurrentNetParam , 0 , sizeof(NETPARAM) );
			memcpy( &tmpGlobal->CurrentNetParam , buffer , sizeof( NETPARAM )  );
			AfxSetNetParam(&tmpGlobal->CurrentNetParam);
		}
	}
	close(fd);
	return TRUE;
}

BOOL AfxWriteNetParamFile( PNETPARAM VeviseNetParam )
{
	int fd = 0;
	int bytes_write = 0 ;

	CGlobal * tmpGlobal = AfxGetGlobal();

	/* 创建目的文件 */
	if ( (fd = open(tmpGlobal->NetParamPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1 )
	{
		fprintf(stderr, "AfxWriteNetParamFile Open %s Error：%s\n", tmpGlobal->NetParamPath, strerror(errno));
		return FALSE;
	}
	if( (bytes_write = write(fd, VeviseNetParam, sizeof(NETPARAM))) == -1 )
	{
		fprintf(stderr, "AfxWriteNetParamFile write %s Error：%s\n", tmpGlobal->NetParamPath, strerror(errno));
		close(fd);
		return FALSE;
	}
	else if( bytes_write > 0 )
	{
		fprintf(stderr, "AfxWriteNetParamFile write %s sucess\n", tmpGlobal->NetParamPath);
	}
	memset( &tmpGlobal->CurrentNetParam , 0 , sizeof(NETPARAM) );
	memcpy( &tmpGlobal->CurrentNetParam , VeviseNetParam ,sizeof(NETPARAM) );
	close(fd);
	return TRUE;
}

BOOL AfxWriteLocalDns ( char *pDns )
{
	FILE *fp = NULL;
	int Rtn = 0;

	fp = fopen( AfxGetGlobal()->SysDnsPath,  "w");
	if(fp == NULL)
	{
		return FALSE;
	}

	Rtn = fwrite(pDns, strlen(pDns), 1, fp);

	fclose(fp);
	return Rtn;
}

BOOL AfxSetNetParam ( PNETPARAM VeviseNetParam )
{
	//设置IP,网关,子网掩码,DNS
	char buf[ 300 ] = { 0 } ;
	sprintf( buf , "ifconfig eth0 %s netmask %s",
			VeviseNetParam->ip,
			VeviseNetParam->mask);
	system(buf);

	memset(buf , 0 , sizeof(buf));

	sprintf(buf,  "route del default");

	system(buf);
	fprintf(stderr, "AfxSetNetParam del route ：errno %s\n", strerror(errno));
	printf("buf %s\n",buf);
	memset(buf , 0 , sizeof(buf));

	sprintf(buf,  "route add default gw %s",
			VeviseNetParam->gateway);
	system(buf);
	printf("buf %s\n",buf);

	if(FALSE == AfxWriteLocalDns( VeviseNetParam->dns ) )
	{
		printf(" WriteDns false \n");
	}
	return TRUE;
}

QBUS_ARRAY * AfxGetBusArray()
{
	Widget * pWidget = AfxGetWidget() ;
	if( !pWidget )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return &pWidget->m_busArray ;
}

PBUS_LINEST AfxGetBus( WORD wBusNo )
{
	QBUS_ARRAY * pBusArray = AfxGetBusArray() ;
	if( !pBusArray )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	int size = pBusArray->size() ;
	if( !size )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	QBUS_ARRAY::iterator itor =  pBusArray->find( wBusNo ) ;
	if( itor == pBusArray->end() )
		return NULL ;

	return itor.value() ;
}

PDEVST AfxGetDev( WORD wBusNo , WORD wAddr )
{
	PBUS_LINEST pBus = AfxGetBus( wBusNo ) ;
	if( !pBus )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	int size = pBus->DevArray.size() ;
	if( !size )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	QDEV_ARRAY::iterator itor = pBus->DevArray.find( wAddr ) ;
	if( itor == pBus->DevArray.end() )
		return NULL ;

	return itor.value() ;
}

PDataType AfxGetData( BYTE byDataType , WORD wBusNo , WORD wAddr , WORD wPnt )
{
	if( byDataType != DataType::YC_DTYPE &&
			byDataType != DataType::YX_DTYPE &&
			byDataType != DataType::YM_DTYPE &&
			byDataType != DataType::YK_DTYPE
	  )
		return NULL ;

	PDEVST pDev = AfxGetDev( wBusNo , wAddr ) ;
	if( !pDev )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	PDataType pData = NULL ;
	if( byDataType == DataType::YC_DTYPE )
		pData = AfxGetData< YCST >( &pDev->ycArray , wPnt ) ;
	else if( byDataType == DataType::YX_DTYPE )
		pData = AfxGetData< YXST >( &pDev->yxArray , wPnt ) ;
	else if( byDataType == DataType::YM_DTYPE )
		pData = AfxGetData< YMST >( &pDev->ymArray , wPnt ) ;
	else if( byDataType == DataType::YK_DTYPE)
		pData = AfxGetData< YKST >( &pDev->ykArray , wPnt ) ;
	else
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return pData ;
}

	template< class DTYPE >
DTYPE * AfxGetData( QDEV_DATA_ARRAY * pArray , WORD wPnt )
{
	if( !pArray )
		return NULL ;

	int size = pArray->size() ;
	if( !size || wPnt >= ( WORD )size )
	{
		printf("size=%d,wPnt=%d\n",size,wPnt);
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	DTYPE * pEle = ( DTYPE *)(*pArray)[ wPnt ] ;
	if( !pEle )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}
	return pEle ;
}

PYCST AfxGetYc( WORD wBusNo , WORD wAddr , WORD wPnt )
{
	return ( PYCST )AfxGetData( DataType::YC_DTYPE , wBusNo , wAddr , wPnt ) ;
}

PYXST AfxGetYx( WORD wBusNo , WORD wAddr , WORD wPnt )
{
	return ( PYXST )AfxGetData( DataType::YX_DTYPE , wBusNo , wAddr , wPnt ) ;
}

PYMST AfxGetYm( WORD wBusNo , WORD wAddr , WORD wPnt )
{
	return ( PYMST )AfxGetData( DataType::YM_DTYPE , wBusNo , wAddr , wPnt ) ;
}

//Warning transmitProto doesn't have real yk structure
PYKST AfxGetYk( WORD wBusNo , WORD wAddr , WORD wPnt )
{
	return ( PYKST )AfxGetData( DataType::YK_DTYPE , wBusNo  ,wAddr , wPnt ) ;
}

BOOL AfxGetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float &fVal )
{
	PYCST pYcSt = AfxGetYc( wBusNo , wAddr , wPnt );
	if( !pYcSt )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	fVal = pYcSt->fVal_1Time ;
	return TRUE ;
}

BOOL AfxGetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE &byVal )
{
	PYXST pYxSt = AfxGetYx( wBusNo , wAddr , wPnt );
	if( !pYxSt )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	byVal = pYxSt->byValue ;
	return TRUE ;
}

BOOL AfxGetYm( WORD wBusNo , WORD wAddr , WORD wPnt , double &dVal )
{
	PYMST pYmSt = AfxGetYm( wBusNo , wAddr , wPnt );
	if( !pYmSt )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	dVal = pYmSt->dVal_1Time ;
	return TRUE ;
}

BOOL AfxSetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float fVal )
{
	PYCST pYc = AfxGetYc( wBusNo , wAddr , wPnt ) ;
	if( pYc )
	{
		if( fabs( pYc->fValue - fVal ) > pYc->fThv )
		{
			pYc->fValue = fVal ;
			pYc->fVal_1Time = pYc->fBaseValue + pYc->fValue * pYc->fCoefficient ;
		}
	}
	else
		return FALSE ;

	return TRUE ;
}

BOOL AfxSetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE byVal )
{
	PYXST pYx = AfxGetYx( wBusNo , wAddr , wPnt ) ;
	if( pYx )
	{
		if( pYx->byReverse )
		{
			switch( byVal )
			{
			case 0:
				if( pYx->byValue != 1 )
				{
					pYx->byValue = 1 ;
					if( pYx->byWarn )
					{
						if( wBusNo != 14 )
						{
							InsertYxAlarmDbase(pYx);
							AfxGetGlobal()->EmitSignal_YxAlarm(pYx->chName,pYx->byValue);
						}
					}
				}
				break;
			case 1:
				if( pYx->byValue != 0 )
				{
					pYx->byValue = 0 ;
					if( pYx->byWarn )
					{
						if( wBusNo != 14 )
						{
							InsertYxAlarmDbase(pYx);
							AfxGetGlobal()->EmitSignal_YxAlarm(pYx->chName,pYx->byValue);
						}
					}
				}
				break;
			default:
				return FALSE;
			}
		}
		else
		{
			if( pYx->byValue != byVal )
			{
				pYx->byValue = byVal ;
				if( pYx->byWarn )
				{
					if( wBusNo != 14 )
					{
						InsertYxAlarmDbase(pYx);
						AfxGetGlobal()->EmitSignal_YxAlarm(pYx->chName,pYx->byValue);
					}
				}
			}
		}
	}
	else
		return FALSE ;
	return TRUE ;
}

BOOL AfxSetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD dwVal )
{
	PYMST pYm = AfxGetYm( wBusNo , wAddr , wPnt ) ;
	if( pYm )
	{
		pYm->dVal_1Time = pYm->fBaseValue + pYm->fCoefficient * dwVal ;
		pYm->dwValue = dwVal ;
	}
	else
		return FALSE ;

	return TRUE ;
}

void AfxGetCurNetPara( NETPARAM * pNetParam )
{
	if( !pNetParam )
	{
		Q_ASSERT( FALSE ) ;
		return ;
	}

	memcpy( pNetParam , &AfxGetGlobal()->CurrentNetParam ,sizeof( NETPARAM ) );
}

const QCAB_ARRAY &AfxGetCabinetArray( )
{
	return AfxGetWidget()->m_cabArray ;
}

const QSCIRCUIT_ARRAY &AfxGetSCircuitArray( )
{
	return AfxGetWidget()->m_sCircuitArray ;
}

PCABINET AfxGetCabinet( WORD wCabNo )
{
	const QCAB_ARRAY &pCabArray = AfxGetCabinetArray() ;
	int size = pCabArray.size() ;
	if( wCabNo >= size )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	return pCabArray[ wCabNo ] ;
}

PSINGLECIRCUIT AfxGetSingleCircuit( WORD wCircuitNo )
{
	const QSCIRCUIT_ARRAY pArray = AfxGetSCircuitArray();
	int size = pArray.size() ;
	if( wCircuitNo >= size )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return pArray[ wCircuitNo ] ;
}

PSINGLECIRCUIT AfxGetCabSingleCircuit( WORD wCab , WORD wCircuitNo )
{
	const PCABINET pCab = AfxGetCabinet( wCab ) ;
	if( !pCab )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	const QSCIRCUIT_MAP_ITOR itor = pCab->circuitArray.find( wCircuitNo ) ;
	if( itor == pCab->circuitArray.end() )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	if( itor.key( ) != wCircuitNo )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return itor.value( ) ;
}

const QDEV_DATA_ARRAY* AfxGetCabCircuitData( WORD wCab , WORD wCircuitNo , DATETYPE byType )
{
	if( byType != DataType::YC_DTYPE &&
			byType != DataType::YX_DTYPE &&
			byType != DataType::YM_DTYPE )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	const PSINGLECIRCUIT pCircuit = AfxGetCabSingleCircuit( wCab ,wCircuitNo ) ;
	if( !pCircuit )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	if( byType == DataType::YC_DTYPE )
		return &pCircuit->ycArray ;
	else if( byType == DataType::YX_DTYPE )
		return &pCircuit->yxArray ;
	else if( byType == DataType::YM_DTYPE )
		return &pCircuit->ymArray ;
	else
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return NULL  ;
}


const QDEV_DATA_ARRAY* AfxGetCircuitData( WORD wCircuitNo , DATETYPE byType )
{
	if( byType != DataType::YC_DTYPE &&
			byType != DataType::YX_DTYPE &&
			byType != DataType::YM_DTYPE )
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	const PSINGLECIRCUIT pCircuit = AfxGetSingleCircuit( wCircuitNo ) ;
	if( !pCircuit )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	if( byType == DataType::YC_DTYPE )
		return &pCircuit->ycArray ;
	else if( byType == DataType::YX_DTYPE )
		return &pCircuit->yxArray ;
	else if( byType == DataType::YM_DTYPE )
		return &pCircuit->ymArray ;
	else
	{
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return NULL  ;
}


//BYTE AfxSetBus

BOOL AfxSetBusCommState( WORD wBusNo , WORD wState )
{
	if( wState != COMM_ABNORMAL && wState != COMM_NOMAL )
	{
		Q_ASSERT( FALSE );
		return FALSE;
	}

	QBUS_ARRAY * pBusArray = AfxGetBusArray() ;
	if( !pBusArray )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	int size = pBusArray->size() ;
	if( wBusNo >= size )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	PBUS_LINEST pBus = AfxGetBus( wBusNo ) ;
	if( !pBus )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	pBus->wBusCommState = wState ;
	return TRUE ;
}

BOOL AfxGetBusCommState( WORD wBusNo , WORD &wState )
{
	QBUS_ARRAY * pBusArray = AfxGetBusArray() ;
	if( !pBusArray )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	int size = pBusArray->size() ;
	if( wBusNo >= size )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	PBUS_LINEST pBus = AfxGetBus( wBusNo ) ;
	if( !pBus )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	wState = pBus->wBusCommState ;

	return TRUE ;
}


BOOL AfxSetDevCommState( WORD wBusNo , WORD wAddr , WORD wState )
{
	if( wState != COMM_ABNORMAL && wState != COMM_NOMAL )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	PDEVST pDev = AfxGetDev( wBusNo , wAddr ) ;
	if( !pDev )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	pDev->wDevState = wState ;
	return TRUE ;
}

BOOL AfxGetDevCommState( WORD wBusNo , WORD wAddr , WORD &wState )
{
	PDEVST pDev = AfxGetDev( wBusNo , wAddr ) ;
	if( !pDev )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	wState = pDev->wDevState ;
	return TRUE ;
}

//for  protocol
BOOL AfxSengYK( WORD wBusNo , WORD wAddr , WORD nPnt , BYTE byAction , WORD wVal )
{
	if( byAction != YK_PREPARE && byAction != YK_PREPARE_RTN &&
			byAction != YK_EXEC && byAction != YK_EXEC_RTN )
		return FALSE ;

	//if dev does't exist return directly
	//check nPnt is valid or not
	PYKST pYkst = AfxGetYk( wBusNo , wAddr , nPnt ) ;
	if( !pYkst )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	PBUS_LINEST pBus = AfxGetBus( wBusNo ) ;

	if( !pBus->pThreadProc || !pBus->pThreadProc->pMsg )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	CMsg * pMsg = pBus->pThreadProc->pMsg ;

	PYKST_MSG  pYK = new YKST_MSG ;
	pYK->byAction = byAction ;

	pYK->wBusNoDest = wBusNo ;
	pYK->wAddrDest = wAddr ;
	pYK->wPntNoDest = nPnt ;
	pYK->wVal = wVal ;

	AfxSendMsg( pMsg , pYK ) ;
	return TRUE ;
}

//for transmit protocol
BOOL AfxSengYK( WORD wBusNoDest , WORD wAddrDest , WORD nPntDest ,
		WORD wBusNoSrc , WORD wAddrSrc , WORD nPntSrc ,
		BYTE byAction , WORD wVal )
{
	if( byAction != YK_PREPARE && byAction != YK_PREPARE_RTN &&
			byAction != YK_EXEC && byAction != YK_EXEC_RTN &&
			byAction != YK_CANCEL && byAction != YK_CANCEL_RTN)
		return FALSE ;

	//    PYKST pYkst = AfxGetYk( wBusNoDest , wAddrDest , nPntDest ) ;
	//    if( !pYkst )
	//    {
	//        Q_ASSERT( FALSE ) ;
	//        return FALSE ;
	//    }

	CMsg * pMsg = NULL;
	if( wBusNoDest == 14 )
	{
		DWORD dwMsgtype = CMsg::MSG_REAL_CHANNEL ;
		PSTHREAD_PROC pThread = AfxGetThreadInfo( dwMsgtype ) ;
		if( !pThread )
		{
			Q_ASSERT( FALSE ) ;
			return FALSE ;
		}
		pMsg = pThread->pMsg;
	}
	else
	{
		PBUS_LINEST pBus = AfxGetBus( wBusNoDest ) ;
		if( !pBus->pThreadProc || !pBus->pThreadProc->pMsg )
		{
			Q_ASSERT( FALSE ) ;
			return FALSE ;
		}

		pMsg = pBus->pThreadProc->pMsg ;
	}
	PYKST_MSG  pYK = new YKST_MSG ;
	pYK->byAction = byAction ;

	pYK->wBusNoDest = wBusNoDest ;
	pYK->wAddrDest = wAddrDest ;
	pYK->wPntNoDest = nPntDest ;

	pYK->wBusNoSrc = wBusNoSrc ;
	pYK->wAddrSrc = wAddrSrc ;
	pYK->wPntNoSrc = nPntSrc ;

	pYK->wVal = wVal ;
	AfxSendMsg( pMsg , pYK ) ;
	return TRUE ;
}

BOOL AfxSetSoe( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE byVal ,REALTIME * pRealTime)
{
	if( NULL == pRealTime )
	{
		return FALSE;
	}


	SOEDATA data;
	// 值
	data.BusNo = wBusNo;
	data.DevNo = wAddr;
	data.DataType = 1;

	// 组织内容
	PYXST pYx = AfxGetYx( wBusNo, wAddr, wPnt );
	if( NULL == pYx )
	{
		return FALSE;
	}

	if ( 0 == byVal )
	{
		sprintf( data.Content,"%s:合->分",pYx->chName);
	}
	else if( 1 == byVal )
	{
		sprintf( data.Content,"%s:分->合",pYx->chName);
	}
	else
	{
		return FALSE;
	}
	// 组织时间
	sprintf( data.DateTime,"%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
			pRealTime->wYear,
			pRealTime->wMonth,
			pRealTime->wDay,
			pRealTime->wHour,
			pRealTime->wMinute,
			pRealTime->wSecond
		   );

	// 组织位置
	sprintf( data.Location,"P%d,%d,%d", wBusNo+1, wAddr, wPnt+1);

	CDBaseManager *pDBase = AfxGetDBase();
	if ( NULL == pDBase )
	{
		printf("NULL == pDBase\n");
		return FALSE;
	}
	pDBase->InsertData((SOEDATA *)&data);

	return TRUE;
}

BOOL AfxSetBusTransClientState( PBUS_LINEST pBus )
{
	if( pBus->byBusType != BUS_TRANS )
		return FALSE;
	if( pBus->pProperty->m_ComType == pBus->pProperty->LAN_SERVER )
	{
		int DevSize = pBus->DevArray.size() ;
		if( DevSize != 1 )
		{
			Q_ASSERT( FALSE ) ;
			return FALSE ;
		}

		QDEV_ARRAY::iterator Begin_itor =  pBus->DevArray.begin() ;
		PDEVST tmpDev = Begin_itor.value() ;

		if( pBus->pProperty->pComPort->IsPortValid() ){
		//	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			tmpDev->wDevState =COMM_NOMAL;
		}
		else{
		//	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			tmpDev->wDevState =COMM_ABNORMAL;
		}
	}
	return TRUE;
}

void AfxGetProjectName( QString & str )
{
	str.clear();
	str = QString("%1").arg(AfxGetWidget()->m_strProjectName);
}

//删除字符串左边空格及\t
void ltrim(char *s)
{
	char *p = s;
	if( *p==' ' || *p=='\t' )
	{
		while( *p==' ' || *p=='\t' ) p++;
		while( (*s++ = *p++) );
	}
}

//删除字符串右边空格及'\t','\r', '\n'
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

/*******************************************************************************
 * 函数名:AfxGetDBase
 * 功能描述:获取数据库
 * 参数:void
 * 返回值:CDBaseManager
 ******************************************************************************/
CDBaseManager *AfxGetDBase(void)
{
	Widget *pWiget = AfxGetWidget();
	if( NULL == pWiget)
	{
		return NULL;
	}

	CDBaseManager* pDBase = pWiget->m_pDBase;
	if( NULL == pDBase)
	{
		return NULL;
	}

	return pDBase->m_pMoudle;
}   /*-------- end AfxGetDBase -------- */


void ScanServerOnLine( )
{
	int size = AfxGetGlobal()->m_VectorTcpClient.size() ;
	if( size == 0 )
		return ;

	THREADPARA ThreadPara ;
	ThreadPara.hThread = pthread_create(&ThreadPara.ThreadID, NULL, ThreadScanSever , (void*)&AfxGetGlobal()->m_VectorTcpClient );
	if(ThreadPara.ThreadID<0)
	{
		;//OutPromptText((char *)" ****ScanServerOnLine____CreateThread Fail!****");
	}

	return ;
}

void* ThreadScanSever(void *pProtObj )
{
	QVector<CComPort*> *pVectorScan = ( QVector<CComPort *>* )pProtObj ;
	if( pVectorScan == NULL )
		return NULL ;

	int size = pVectorScan->size() ;
	if( !size )
		return NULL ;

	CGlobal * pGlobal = AfxGetGlobal() ;
	if( !pGlobal )
	{
		Q_ASSERT( FALSE );
		return NULL ;
	}

	while( pGlobal->GetThreadRunFlag() )
	{
		QVector<CComPort *>::iterator ibegin = pVectorScan->begin() ;
		QVector<CComPort *>::iterator iend = pVectorScan->end();

		for (  ; ibegin != iend ; ibegin++ )
		{
			if( ( *ibegin )->IsPortValid() )
				continue ;

			( *ibegin )->ClosePort() ;
			( *ibegin )->OpenPort() ;
		}

		AfxAsleep( 1000 ) ;
	}
	printf( "Scan Client thread has exited!\n" ) ;
	return NULL ;
}

void m_VectorTcpClientPush_back( CComPort * pPort )
{
	AfxGetGlobal()->m_VectorTcpClient.push_back(pPort);
}

/*******************************************************************************
 * 函数名:set_standby
 * 功能描述: 设置待机
 * 参数: bool bstate  ture 为待机，false为点亮
 * 返回值:void
 ******************************************************************************/
void AfxSetStandby( BOOL bstate )
{
	AfxGetGlobal()->SetStandby( bstate );
}   /*-------- end set_standby -------- */

BOOL InsertYxAlarmDbase( PYXST pYxData )
{
	// printf(" InsertYxAlarmDbase\n");
	if ( NULL == pYxData )
	{
		return FALSE;
	}

	SOEDATA data;
	// 值
	data.BusNo = pYxData->wBusNo;
	data.DevNo = pYxData->wAddr;
	data.DataType = DBASE_YXALARM_DATATYPE;

	// 组织内容
	PYXST pYx = pYxData;
	// if( NULL == pYx )
	// {
	//     return FALSE;
	// }

	if ( 0 == pYxData->byValue )
	{
		sprintf( data.Content,"%s:合->分",pYx->chName);
	}
	else if( 1 == pYxData->byValue )
	{
		sprintf( data.Content,"%s:分->合",pYx->chName);
	}
	else
	{
		return FALSE;
	}

	REALTIME tm ;
	AfxGetSysTime( &tm ) ;

	REALTIME *pRealTime = &tm;
	// 组织时间
	sprintf( data.DateTime,"%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
			pRealTime->wYear,
			pRealTime->wMonth,
			pRealTime->wDay,
			pRealTime->wHour,
			pRealTime->wMinute,
			pRealTime->wSecond
		   );

	// 组织位置
	sprintf( data.Location,"P%d,%d,%d", pYxData->wBusNo+1, pYxData->wAddr, pYxData->wPntNo+1);

	CDBaseManager *pDBase = AfxGetDBase();
	if ( NULL == pDBase )
	{
		printf("NULL == pDBase\n");
		return FALSE;
	}
	pDBase->InsertData((SOEDATA *)&data);

	return TRUE;
}
