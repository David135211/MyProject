/*
 * =====================================================================================
 *
 *       Filename:  ModBusLiuLiangJi.cpp
 *
 *    Description:   苏州宜家家具商场项目（合同号：SM-16-XM-QD007）的涡街流量计
				
				4、485协议操作说明：
							modbus协议
					波特率:9600
					校验位:无
					数据位:8
					停止位:1
					寻址方式：（表号）H +（读命令） 01H+（数据长度）11H
					通讯方式：异步串口
					数据长度：共11H位
					1位：表号H（仪表编号为参数设置中b项，范围0~255，看补充）
					2位：读命令01H
					3位: 数据长度11H
					4位：流量单位和介质（看说明）
					5，6，7位：流量值
					 格式：16位浮点数ACCBHI（bit15为符号位），ACCBLO，EXPB，下同
					8，9，10位：温度值，单位℃(格式同上)
					11，12，13位：压力值，单位MPa(格式同上) 
					14，15，16位：密度值，单位Kg/ m3(格式同上)
					17，18，19，20位：累计流量
					格式：由高到低为17位（X8 ,X7）,18位（X6 ,X5）,19位（X4, X3）,
					20位（X2, X1），小数点始终默认在X2，X1之间。
					21位：CRC低字节 
					22位：CRC高字节
 *        Version:  1.0
 *        Created:  2016年05月20日 09时45分00秒
 *       Revision:  none
 *       Compiler:  
 *
 *         Author:  ykk
 *   Organization:  
 *
 *		  history:
 * =====================================================================================
 */


#include "ModBusLiuLiangJi.h"
#include <math.h>

extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  ModBusLiuLiangJi
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
ModBusLiuLiangJi::ModBusLiuLiangJi ()
{
	m_bLinkStatus = FALSE;
	m_bySendCount = 0;
	m_byRecvCount = 0;
}  /* -----  end of method CModBusXHMCU::CModBusXHMCU  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  ~ModBusLiuLiangJi
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
ModBusLiuLiangJi::~ModBusLiuLiangJi ()
{
}  /* -----  end of method CModBusXHMCU::~CModBusXHMCU  (destructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  print
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void ModBusLiuLiangJi::print ( char *buf ) const
{

#ifdef  XHMCU_PRINT
	OutBusDebug( m_byLineNo, (BYTE *)buf, strlen( buf ), 2 );
#endif     /* -----  not XHMCU_PRINT  ----- */
}		/* -----  end of method CModBusXHMCU::print  ----- */



/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  WhetherBufValue
 * Description:  查看接收报文有效性 
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLiuLiangJi::WhetherBufValue ( BYTE *buf, int &len, int &pos )
{
	BYTE *pointer = buf;
	pos = 0;

	while( len >= 17 )
	{
		//判断同步字
		if( m_wDevAddr != *pointer
				|| 0x01 != *( pointer + 1 )
				|| 0x11 != *( pointer + 2 ) )
		{
			print( (char *)"报文头" );
			pointer ++;
			len --;
			pos ++;
			continue;
		}
		//判断地址
		WORD wCRC = GetCrc( buf, buf[2] + 3 );
		if( ( HIBYTE(wCRC) == buf[ buf[2] + 3 ] ) && ( LOBYTE(wCRC) == buf[ buf[2] + 4 ] ) )
		{
			return TRUE;
		}
		else
		{
			print( (char *)"校验未通过" );
			pointer ++;
			len --;
			pos ++;
			continue;
		}
	}

	print( (char *) "ModBusLiuLiangJi WhetherBufValue not find right buf ");

	return FALSE ;
}		/* -----  end of method ModBusLiuLiangJi::WhetherBufValue  ----- */

	
/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  ProcessRecvBuf
 * Description:  处理报文  更新数据
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLiuLiangJi::ProcessRecvBuf ( BYTE *buf, int len )
{
	BYTE i;
	float wVal;
	
	m_pMethod->SetYcData ( m_SerialNo , 0 , (float)buf[3] );
	for( i=1;i<5;i++ )
	{
		wVal = ((float)(buf[4+((i-1)*3)]<<8 & buf[5+((i-1)*3)])/(float)0x8000)*pow( 2 , buf[6+((i-1)*3)] );
		m_pMethod->SetYcData ( m_SerialNo , i , wVal );
		
	}
	wVal = (buf[16]>>4)*1000000 + (buf[16]&0x0F)*100000 + (buf[17]>>4)*10000 + (buf[17]&0x0F)*1000 /
			(buf[18]>>4)*100 + (buf[18]&0x0F)*10 + (buf[19]>>4) + (float)(buf[19]&0x0F)*0.1;
	m_pMethod->SetYmData( m_SerialNo , 5 , wVal );

	
	return TRUE;
}		/* -----  end of method ModBusLiuLiangJi::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  GetProtocolBuf
 * Description:  获取发送报文  
 *       Input:  缓冲区 长度 消息（无遥控等消息内容 始终为NULL）
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLiuLiangJi::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	if( pBusMsg != NULL )
	{
		return FALSE;	
	}

	len = 0;

	//组织同步头
	buf[len++] = m_wDevAddr;
	buf[len++] = 0x01;
	buf[len++] = 0x00;
	buf[len++] = 0x00;// 寄存器地址 
	buf[len++] = 0x00; 
	buf[len++] = 0x11; // 寄存器个数

	WORD wCRC = GetCrc( buf, len );
    buf[ len++ ] = HIBYTE(wCRC);
    buf[ len++ ] = LOBYTE(wCRC);
	m_bySendCount ++;

	return TRUE;
}		/* -----  end of method ModBusLiuLiangJi::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  ProcessProtocolBuf
 * Description:  处理接收报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLiuLiangJi::ProcessProtocolBuf ( BYTE *buf, int len )
{
	int pos = 0;
	if ( !WhetherBufValue( buf, len, pos ) )
	{
		char szBuf[256] = "";
		sprintf( szBuf, "%s",  "ModBusLiuLiangJi recv buf err !!!\n" );
		print( szBuf );
		m_byRecvCount ++;
		return FALSE;	
	}
	
	ProcessRecvBuf( buf + pos, len );

	m_bLinkStatus = TRUE;
	m_bySendCount = 0;
	m_byRecvCount = 0;

	return TRUE;
}		/* -----  end of method ModBusLiuLiangJi::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  Init
 * Description:  初始化协议
 *       Input:  总线号
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLiuLiangJi::Init ( BYTE byLineNo )
{
	return TRUE;
}		/* -----  end of method ModBusLiuLiangJi::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  TimerProc
 * Description:  时钟处理
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void ModBusLiuLiangJi::TimerProc ( void )
{
	if( m_bySendCount > 3 || m_byRecvCount > 3)
	{
		m_bySendCount = 0;
		m_byRecvCount = 0;
		if( m_bLinkStatus  )
		{
			m_bLinkStatus = FALSE;
			print( (char *) "ModBusLiuLiangJi:unlink\n");
		}
	}
}		/* -----  end of method ModBusLiuLiangJi::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLiuLiangJi
 *      Method:  GetDevCommState
 * Description:  获取装置状态
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLiuLiangJi::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
	{
		return COM_DEV_NORMAL;
	}
	else
	{
		return COM_DEV_ABNORMAL;
	}
}		/* -----  end of method ModBusLiuLiangJi::GetDevCommState  ----- */

