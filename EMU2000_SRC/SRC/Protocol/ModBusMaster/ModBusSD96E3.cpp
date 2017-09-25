/*
 * =====================================================================================
 *
 *       Filename:  ModBusSD96E3.cpp
 *
 *    Description: SD 系列仪表的测量值可以用 03H 命令读出。
	SD 系列参量地址表：
	以下为电量信息参量地址： 03H 功能码读， 10H 功能码写
	地址 参数 读写
	属性 说明 数据类型
	0000H
	相电 UA
	(三相四线)
	相电压 UAB
	(三相三线)
	R word
	0001H
	相电压 UB
	(三相四线)
	相电压 UBC
	(三相三线)
	R word
	0002H
	相电压 UC
	(三相四线)
	相电压 UAC
	(三相三线)
	R word
	0003H 电流 IA R word
	0004H 电流 IB R word
	0005H 电流 IC R word
	0006H 有功功率 P R word
	0007H 无功功率 Q R word
	0008H 功率因数 λ R word
	0009H 频率 F R
	数据格式描述
	二 次 侧 电 能 数 据
	（不带变比）采用定点
	数表示： 高字在前，低
	字在后。 （详细情况见表
	格下方举例说明）
	一 次 侧 电 能 数 据
	（带变比）采用标准的
	IEEE754 浮点数（单精
	度）格式表示。
	注：电能数据的最
	小刻度为 0.1Wh，即如
	果 读 到 电 能 数 据 为
	10000(2710H)时， 代表
	此 时 电 能 为 10000 ×
	0.1Wh＝1KWh。
	word
	000AH
	电压电流
	辅助信息 R/W word
	000BH
	功率
	辅助信息 R/W
	详细情况见表格下方辅
	助信息说明
	word
	000CH-0
	00DH
	二次侧
	总有功电能 R/W Dword
	000EH-0
	00FH
	二次侧
	总无功电能 R/W
	定点数表示：
	高字在前，低字在后
	Dword
	0010H-0
	011H
	一次侧
	总有功电能 R Dword
	0012H-0
	013H
	一次侧
	总无功电能 R
	IEEE754 浮点数
	（单精度）格式
	Dword
	SD 系列多功能电力监测仪表用户手册 V1.4 版
	17
	举例说明：（ 16 进制表示）
	上位机发送命令： 01 03 00 00 00 14 45 C5
	其中 01H 为仪表地址， 03H 为命令， 0000H 为要查询
	的数据起始地址， 0014H 指要查询的数据长度为 20 个字， 45C5H
	为 CRC 校验码。
	仪表响应： 01 03 28 XX（ 20 个字） CRC 码
	如果收到电压数据为： 03 E8H，则转换为十进制数据 1000
	如果收到二次电能数据为： 00 00 2710，则读作： 2710H，
	转换为十进制数据为 10000。
	注：电能数据的最小刻度为 0.1Wh，即如果读到电能数据为
	10000(2710H)时，代表此时电能为 10000×0.1Wh＝1KWh。
	辅助信息说明：（ 16 进制表示）
	高字节 低字节
	电压电流辅助信息 电流指数部分 电压指数部分
	功率辅助信息 功率指数部分 功率符号
	电压电流功率的指数部分含义为：
	例如电压电流辅助信息寄存器的返回数据为 02 03H，则电流指数
	部分读数为 02H，电压指数部分读数为 03H。若 A 相电流（ 0003H）读
	数为 1388H（十进制数 5000），则 A 相电流实际值为（ 5000/10000）×
	102=50A，电压、功率读数也与之类似。
	功率符号部分含义为：
	功率符号读数 00H 01H 10H 11H
	有功功率符号 正 负 正 负
	无功功率符号 正 正 负 负
 *
 *        Version:  1.0
 *        Created:  2017年1月9日 11时40分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ykk (), 

 *   Organization:  
 *
 *		  history:
 * =====================================================================================
 */

#include <math.h>
#include "ModBusSD96E3.h"
extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  CModBusSD96E3
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CModBusSD96E3::CModBusSD96E3 ()
{
	m_bLinkStatus = FALSE;
	m_bySendCount = 0;
	m_byRecvCount = 0;
}  /* -----  end of method CModBusSD96E3::ModBusSD96E3  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  ~CModBusSD96E3
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CModBusSD96E3::~CModBusSD96E3 ()
{
}  /* -----  end of method CModBusSD96E3::~CModBusSD96E3  (destructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  WhetherBufValue
 * Description:  查看接收报文有效性 
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusSD96E3::WhetherBufValue ( BYTE *buf, int &len )
{
	BYTE *pointer = buf;
	WORD wCrc;
	int pos = 0;

	while( len >= 4 )
	{
		//判断地址
		if ( *pointer != m_wDevAddr )
		{
			goto DEFAULT;
		}

		//判断功能码
		if ( *( pointer + 1 ) != 0x03 )
		{
			goto DEFAULT;
		}

		//判断校验
		wCrc = GetCrc( pointer, ( *( pointer + 2 ) + 3 ) );
		if( *( pointer + ( *( pointer + 2 ) + 3 ) ) !=  HIBYTE(wCrc)
		 || *( pointer + ( *( pointer + 2 ) + 4 ) ) !=  LOBYTE(wCrc))
		{
			goto DEFAULT;
		}
			
		buf = buf + pos;
		len = *(pointer + 2) + 5;
		return TRUE;
DEFAULT:
		pointer ++;
		len --;
		pos ++;
	}
	return FALSE ;
}		/* -----  end of method CModBusSD96E3::WhetherBufValue  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  ProcessRecvBuf
 * Description:  处理报文  更新数据
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusSD96E3::ProcessRecvBuf ( BYTE *buf, int len )
{
	if( len < 15 )
	{
		return FALSE;
	}
	BYTE IPower=0 , UPower=0 , FPower=0 , FSignBit=0;
	IPower = buf[23];
	UPower = buf[24];
	FSignBit = buf[25];
	FPower = buf[26];
	//printf("iuff %d %d %d %d \n ",IPower,UPower,FPower,FSignBit);
	for( int i=0; i<10; i++ )
	{	//printf("yc i =%d\n",i);
		float fYcValue = 0.0;	
		fYcValue = (float)( MAKEWORD( buf[4 + 2 * i], buf[ 3 + 2 * i ] ) );
		if( i < 3 )
			fYcValue = (fYcValue/10000)*pow(10,UPower);
		else if( i > 2 && i < 6 )
			fYcValue = (fYcValue/10000)*pow(10,IPower);
		else if( i > 5 && i < 8 )
		{
			fYcValue = (fYcValue/10000)*pow(10,FPower)/1000;
			switch( FSignBit )
			{
				case 0x00:
				break;
				case 0x01:
					if( i == 6 )
						fYcValue = -fYcValue; 
				break;
				case 0x10:
					if( i == 7 )
						fYcValue = -fYcValue; 
				break;
				case 0x11:
					fYcValue = -fYcValue; 
				break;
			}
		}
		else if( i == 8 )
			fYcValue = fYcValue*0.001;
		else if( i == 9 )
			fYcValue = fYcValue*0.01;
		//printf("yc m_SerialNo=%d fYcValue =%f\n",m_SerialNo,fYcValue);
		m_pMethod->SetYcData( m_SerialNo, i, fYcValue );
		//printf("SetYcData m_SerialNo=%d fYcValue =%f\n",m_SerialNo,fYcValue);
		// printf ( "yc pnt=%d val=%f\n", i, fYcValue );
	}
	for( int i=0; i<2; i++ )
	{//printf("ym i =%d\n",i);
		float fYmValue = 0.0;	
		fYmValue = *(float *)(buf+35 + 4*i );
		fYmValue = fYmValue/10000;
		//printf("ym m_SerialNo=%d fYmValue =%f\n",m_SerialNo,fYmValue);
		m_pMethod->SetYmData( m_SerialNo, i, fYmValue );
		//printf("SetYmData m_SerialNo=%d fYmValue =%f\n",m_SerialNo,fYmValue);
	}

	return TRUE;
}		/* -----  end of method CModBusSD96E3::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  GetProtocolBuf
 * Description:  获取发送报文  
 *       Input:  缓冲区 长度 消息（无遥控等消息内容 始终为NULL）
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusSD96E3::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	if( pBusMsg != NULL )
	{
		return FALSE;	
	}

	len = 0;

	buf[len++] = m_wDevAddr;
	buf[len++] = 0x03;

	buf[len++] = 0x00;
	buf[len++] = 0x00;
	buf[len++] = 0x00;
	buf[len++] = 0x14;
	
	WORD wCRC = GetCrc( buf, len );
    buf[ len++ ] = HIBYTE(wCRC);
    buf[ len++ ] = LOBYTE(wCRC);

	m_bySendCount ++;

	return TRUE;
}		/* -----  end of method CModBusSD96E3::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  ProcessProtocolBuf
 * Description:  处理接收报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusSD96E3::ProcessProtocolBuf ( BYTE *buf, int len )
{
	if ( !WhetherBufValue( buf, len ) )
	{
		char szBuf[256] = "";
		sprintf( szBuf, "%s",  "CModBusSD96E3 recv buf err !!!\n" );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen( szBuf ), 2 );

		m_byRecvCount ++;
		return FALSE;	
	}
	ProcessRecvBuf( buf, len );

	m_bLinkStatus = TRUE;
	m_bySendCount = 0;
	m_byRecvCount = 0;

	return TRUE;
}		/* -----  end of method CModBusSD96E3::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  Init
 * Description:  初始化协议
 *       Input:  总线号
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusSD96E3::Init ( BYTE byLineNo )
{
	return TRUE;
}		/* -----  end of method CModBusSD96E3::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  TimerProc
 * Description:  时钟处理
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CModBusSD96E3::TimerProc ( void )
{
	if( m_bySendCount > 3 || m_byRecvCount > 3)
	{
		m_bySendCount = 0;
		m_byRecvCount = 0;
		if( m_bLinkStatus  )
		{
			m_bLinkStatus = FALSE;
			OutBusDebug( m_byLineNo, (BYTE *)"CModBusSD96E3:unlink\n", 30, 2 );
		}
	}
}		/* -----  end of method CModBusSD96E3::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusSD96E3
 *      Method:  GetDevCommState
 * Description:  获取装置状态
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusSD96E3::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
	{
		return COM_DEV_NORMAL;
	}
	else
	{
		return COM_DEV_ABNORMAL;
	}
}		/* -----  end of method CModBusSD96E3::GetDevCommState  ----- */

