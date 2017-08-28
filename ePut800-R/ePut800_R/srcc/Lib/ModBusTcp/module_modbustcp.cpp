#include "module_modbustcp.h"

CModule_ModBusTcp::CModule_ModBusTcp( CMethod * m_pMethod , WORD wBusNo , WORD wAddr )
    :CModBusTcp( m_pMethod )
{
    m_wBusNO = wBusNo ;
    m_wAddr = wAddr ;

    //SendFlag = 0;
    MsgFlag = 2;
    FunNum = 0;
    m_wErrorTimer = 0;
    memset(ErrorInformation,0,sizeof(ErrorInformation));
    //ErrorFlag = 0;
}

BOOL CModule_ModBusTcp::Init( )
{
    //sprintf( m_szObjName, "%s", m_sDevName );
    //sprintf( m_ComCtrl1, "%s", mt_sMasterAddr );
    //m_wRtuAddr = m_wDevAddr ;
    char szFileName[256] = "";
    //sprintf( m_sTemplatePath, "%s", "rtu09.txt" );
    sprintf( szFileName, "%s%s", "./config/ModBusTcp/", m_sTemplatePath );
    //读取需要转发的数据到该模块
    ReadMapConfig( szFileName );

    //初始化该模块
    //InitRtuBase() ;

    //初始化上传装置状态时间
    //time( &m_DevStateTime ) ;
    return TRUE ;
}

BOOL CModule_ModBusTcp::GetReadBuf(BYTE *pBuf, int &len, BYTE &byType, LPVOID pVoid)
{
    if( pBuf == NULL )
        return FALSE ;

    m_wErrorTimer++;
    if( m_wErrorTimer > ERROR_CONST )
    {
        m_wErrorTimer = ERROR_CONST + 1 ;

        //m_pMethod->SetDevCommState(m_wBusNO,m_wAddr,COMM_ABNORMAL);
    }


    PSMSG pMsg = (PSMSG)pVoid ;
    if( pMsg != NULL )
    {
        PYKST_MSG pYkmsg = NULL ;
        if( pMsg->msgCustomType == SMSG::MSG_YK)
        {
            pYkmsg = ( PYKST_MSG )pMsg ;
            if( pYkmsg != NULL )
            {
                DealBusMsgInfo( pYkmsg );
            }
        }
    }

    if( MsgFlag == 2 )
    {
        return FALSE;
    }
    MsgFlag = 2;
    memcpy( pBuf , MsgBuf , MsgLen );
    len = MsgLen;
    memset( MsgBuf , 0 , MsgLen);
    MsgLen = 0;
/*
    printf("ModBusTcp tx(%d):",len);
    int i=0;
    for( i=0;i<len;i++ )
    {
        printf(" %02x ", pBuf[i]);
    }
    printf(" \n ");
*/
    return TRUE;
}

BOOL CModule_ModBusTcp::ProcessBuf(BYTE *pBuf, int len, BYTE byType, LPVOID pVoid)
{
    BOOL Rtn = FALSE;
    if( len == 0 || pBuf == NULL )
            return FALSE ;
/*    printf("ModBusTcp rx:");
    int i=0;
    for( i=0;i<len;i++ )
    {
        printf(" %02x ", pBuf[i]);
    }
    printf(" \n ");
*/
    if( pBuf[0]==0 && pBuf[1]==0 && pBuf[2]==0 && pBuf[3]==0 && pBuf[4]==0
            && (pBuf[5]+6)==len && pBuf[6]== m_wAddr )//&& len == 12 )
    {
        FunNum = pBuf[7];
        MsgFlag = 0;
        switch( FunNum )
        {
            case 2:
            Rtn = YXPacket( pBuf , len );
            break;
            case 3:
            Rtn = YcYmPacket( pBuf , len );
            break;
            case 4:
            Rtn = YcYmPacket( pBuf , len );
            break;
            case 5:
            Rtn = YKMsg( pBuf , len );
            break;
            default:
            ErrorPacket( ERROR_FUN );
            Rtn = FALSE;
            memset(ErrorInformation,0,sizeof(ErrorInformation));
            sprintf(ErrorInformation,"%s","功能码错误");
            break;
        }
        if( Rtn == TRUE )
        {
            m_wErrorTimer = 0;
            //m_pMethod->SetDevCommState(m_wBusNO,m_wAddr,COMM_NOMAL);
        }
        else
        {
            MsgLen = 0;
        }
        return Rtn ;
    }
    else
    {
        if( !(pBuf[0]==0 && pBuf[1]==0 && pBuf[2]==0 && pBuf[3]==0 && pBuf[4]==0) )
        {
            memset(ErrorInformation,0,sizeof(ErrorInformation));
            sprintf(ErrorInformation,"%s","报文前五个子节不为0");
        }
        else if( (pBuf[5]+6)!=len )
        {
            memset(ErrorInformation,0,sizeof(ErrorInformation));
            sprintf(ErrorInformation,"%s","报文实际数据长度与报文解析长度不一致");
        }
        else if( pBuf[6] != m_wAddr )
        {
            memset(ErrorInformation,0,sizeof(ErrorInformation));
            sprintf(ErrorInformation,"装置地址不匹配，本机地址为%d",m_wAddr);
        }
        else
        {
            memset(ErrorInformation,0,sizeof(ErrorInformation));
            sprintf(ErrorInformation,"ModBusTcp报文错误");
        }
    }

    return FALSE ;
}

int CModule_ModBusTcp::GetErrorMsg( BYTE *pBuf , int len )
{
    strcpy((char *)pBuf,ErrorInformation);
    return strlen(ErrorInformation);
}

BOOL CModule_ModBusTcp::ErrorPacket( BYTE errorflag )
{
    MsgFlag = 1;

    BYTE index = 0;

    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 3;		//ModBusTcp后续字节
    MsgBuf[index++] = m_wAddr;
    MsgBuf[index++] = FunNum | 0x80;;
    MsgBuf[index++] = errorflag;

    MsgLen = index;
    return TRUE;
}

BOOL CModule_ModBusTcp::YXPacket( BYTE * buf , int len )
{
    WORD index = 0;
    WORD mt_register = 256 * buf[8] + buf[9];
    WORD registernum = 256 * buf[10] + buf[11];

    WORD ModBusByteLen = ((registernum-1)/8)+1;
    WORD ModBusTcpByteLen = 3+ModBusByteLen;

    if( ( mt_register > 9999 || mt_register == 0 ) || ( registernum > 9999 || registernum == 0 ) )
    {
            if( mt_register > 9999 || mt_register == 0 )
            {
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"%s","寄存器错误");
            }
            else if( registernum > 9999 || registernum == 0 )
            {
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"%s","寄存器数目错误");
            }
            //printf("ModBusTcp ERROR_REGISTER");
            return FALSE;
    }
    if( mt_register+registernum > 9999 )
    {
        memset(ErrorInformation,0,sizeof(ErrorInformation));
        sprintf(ErrorInformation,"%s","寄存器+寄存器数目超出范围9999");
        return FALSE;
    }
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = ModBusTcpByteLen;		//ModBusTcp后续字节
    MsgBuf[index++] = m_wAddr;
    MsgBuf[index++] = FunNum;
    MsgBuf[index++] = ModBusByteLen;		//ModBus后续字节
    int j = 0;
    for( j=0; j<ModBusByteLen; j++ )
    {
            BYTE i = 0;
            BYTE ByYXVal = 0;
            for( i=0; i<8; i++ )
            {
                if( j==ModBusByteLen-1 && i == registernum%8 && 0 != registernum%8)
                        break;

                    if( m_pDIMapTab[mt_register-1+(8*j+i)].wBusNo > 0 &&
                        m_pDIMapTab[mt_register-1+(8*j+i)].wAddr > 0 && m_pDIMapTab[mt_register-1+(8*j+i)].wPntNum > 0 )
                            //ByYXVal |= m_byYXbuf[mt_register-1+(8*j+i)]<<i;
                    {
                            BYTE tmpval = 0;
                            m_pMethod->GetYx(m_pDIMapTab[mt_register-1+(8*j+i)].wBusNo-1,m_pDIMapTab[mt_register-1+(8*j+i)].wAddr,
                                             m_pDIMapTab[mt_register-1+(8*j+i)].wPntNum-1, tmpval);
                            ByYXVal |= tmpval<<i;
                    }
                    else
                    {
                            ErrorPacket( ERROR_REGISTER );
                            memset(ErrorInformation,0,sizeof(ErrorInformation));
                            sprintf(ErrorInformation,"%s","遥信转发配置错误");
                            return FALSE;
                    }
            }
            MsgBuf[index++] = ByYXVal;
    }

    MsgLen = index;
    return TRUE;
}

BOOL CModule_ModBusTcp::YcYmPacket( BYTE * buf , int len )
{
    DWORD ymval ;
    WORD index = 0,wVal = 0;;
    float fVal = 0.0f ;
    WORD mt_register = 256 * buf[8] + buf[9];
    WORD registernum = 256 * buf[10] + buf[11];

    WORD ModBusByteLen = 2*registernum;
    WORD ModBusTcpByteLen = 3+ModBusByteLen;
    //printf("mt_register = %d registernum = %d\n",mt_register,registernum);
    if( ( mt_register > 9999 || mt_register == 0 ) || ( registernum > 9999 || registernum == 0 )
            || ( mt_register <= 6800 && mt_register+registernum > 6800 ) )
    {
            ErrorPacket( ERROR_REGISTER );
            if( mt_register > 9999 || mt_register == 0 )
            {
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"%s","寄存器错误");
            }
            else if( registernum > 9999 || registernum == 0 )
            {
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"%s","寄存器数目错误");
            }
            else if(mt_register <= 6800 && mt_register+registernum > 6800)
            {
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"%s","寄存器6800前后分别是遥测和遥脉，不能同一帧报文询问");
            }
            return FALSE;
    }
    if( mt_register+registernum > 9999 )
    {
        memset(ErrorInformation,0,sizeof(ErrorInformation));
        sprintf(ErrorInformation,"%s","寄存器+寄存器数目超出范围9999");
        return FALSE;
    }
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = ModBusTcpByteLen;		//ModBusTcp后续字节
    MsgBuf[index++] = m_wAddr;
    MsgBuf[index++] = FunNum;
    MsgBuf[index++] = ModBusByteLen;		//ModBus后续字节
    int j = 0;
    for( j=0; j<registernum; j++ )
    {

            if( mt_register >= 1 && mt_register <= 6800 )
            {
                    if( m_pAIMapTab[mt_register-1+j].wBusNo > 0 &&
                        m_pAIMapTab[mt_register-1+j].wAddr > 0 && m_pAIMapTab[mt_register-1+j].wPntNum > 0 )
                    {

                            m_pMethod->GetYc(m_pAIMapTab[mt_register-1+j].wBusNo-1,m_pAIMapTab[mt_register-1+j].wAddr,
                                             m_pAIMapTab[mt_register-1+j].wPntNum-1, fVal);
                            wVal = ( DWORD )fVal ;
                            MsgBuf[index++] = HIBYTE(wVal);
                            MsgBuf[index++] = LOBYTE(wVal);
                    }
                    else
                    {
                            ErrorPacket( ERROR_REGISTER );
                            memset(ErrorInformation,0,sizeof(ErrorInformation));
                            sprintf(ErrorInformation,"%s","遥测转发配置错误");
                            return FALSE;
                    }
            }
            else if( mt_register >= 6801 && mt_register <= 9999 )
            {
                    if( ( ( mt_register-6801 )%2 == 1 ) || ( registernum%2 == 1 ) )
                    {
                            ErrorPacket( ERROR_REGISTER );
                            if( ( mt_register-6801 )%2 == 1 )
                            {
                                memset(ErrorInformation,0,sizeof(ErrorInformation));
                                sprintf(ErrorInformation,"%s","遥脉寄存器错误，只能为单数");
                            }
                            else if( registernum%2 == 1 )
                            {
                                memset(ErrorInformation,0,sizeof(ErrorInformation));
                                sprintf(ErrorInformation,"%s","遥测寄存器数目错误，只能为偶数");
                            }
                            return FALSE;
                    }
                    else
                    {
                            if( j%2 == 0 )
                            {
                                    if( m_pPIMapTab[(mt_register-6801)/2+j/2].wBusNo > 0 &&
                                        m_pPIMapTab[(mt_register-6801)/2+j/2].wAddr > 0 && m_pPIMapTab[(mt_register-6801)/2+j/2].wPntNum > 0 )
                                    {
                                            //ymval = GetPulseData ( m_pPIMapTab[(mt_register-6801)/2+j/2].wStn,
                                                                  //  m_pPIMapTab[(mt_register-6801)/2+j/2].wPntNum, &flag  );
                                            m_pMethod->GetYm(m_pPIMapTab[(mt_register-6801)/2+j/2].wBusNo-1,m_pPIMapTab[(mt_register-6801)/2+j/2].wAddr,
                                                            m_pPIMapTab[(mt_register-6801)/2+j/2].wPntNum-1, ymval);

                                            MsgBuf[index++] = HIBYTE(HIWORD(ymval));
                                            MsgBuf[index++] = LOBYTE(HIWORD(ymval));
                                            MsgBuf[index++] = HIBYTE(LOWORD(ymval));
                                            MsgBuf[index++] = LOBYTE(LOWORD(ymval));
                                    }
                                    else
                                    {
                                            ErrorPacket( ERROR_REGISTER );
                                            memset(ErrorInformation,0,sizeof(ErrorInformation));
                                            sprintf(ErrorInformation,"%s","遥脉转发配置错误");
                                            return FALSE;
                                    }
                            }
                    }
            }
    }
    MsgLen = index;
    return TRUE;
}

BOOL CModule_ModBusTcp::YKMsg( BYTE * buf , int len )
{
    WORD mt_register = 256 * buf[8] + buf[9];

    if( ( mt_register > 10000 || mt_register == 0 ) ||
            !( ( buf[10] == 0xff || buf[10] == 0x00 ) && buf[11] == 0x00 ) )
    {
        if( mt_register > 10000 || mt_register == 0 )
        {
                ErrorPacket( ERROR_REGISTER );
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"%s","遥控寄存器错误");
        }
        if( !( ( buf[10] == 0xff || buf[10] == 0x00 ) && buf[11] == 0x00 ) )
        {
                ErrorPacket( ERROR_DATA );
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"%s","遥测命令错误");
        }
        return FALSE;
    }

    WORD wBusNoDst;
    WORD wDevAddrDst;
    WORD wPntDst;
    BYTE byStatus;

    switch( buf[10] )
    {
        case 0xff:
        byStatus = YK_CLOSE;
        break;
        case 0x00:
        byStatus = YK_OPEN;
        break;
    }

    wBusNoDst = m_pDOMapTab[mt_register-1].wBusNo - 1;
    wDevAddrDst = m_pDOMapTab[mt_register-1].wAddr;
    wPntDst  = m_pDOMapTab[mt_register-1].wPntNum - 1 ;
    if( m_pDOMapTab[mt_register-1].wBusNo > 0 && m_pDOMapTab[mt_register-1].wPntNum > 0 )
    {
        m_pMethod->SengYK( wBusNoDst,wDevAddrDst,wPntDst,m_wBusNO,m_wAddr,mt_register,YK_EXEC,byStatus);
        //m_wRelayNum = mt_register;

        return TRUE;
    }
    else
    {
        ErrorPacket( ERROR_REGISTER );
        memset(ErrorInformation,0,sizeof(ErrorInformation));
        sprintf(ErrorInformation,"%s","遥控转发配置错误");
        return FALSE;
    }
    return TRUE;
}

BOOL CModule_ModBusTcp::DealBusMsgInfo( PYKST_MSG pBusMsg )
{
    int rtn = FALSE;
    WORD wBusNoDst;
    WORD wDevAddrDst;
    WORD wPntDst;
    switch ( pBusMsg->byAction )
    {
        //case YK_SEL_RTN:
        case YK_EXEC_RTN:
        //case YK_CANCEL_RTN:
        wBusNoDst = m_pDOMapTab[pBusMsg->wPntNoDest-1].wBusNo - 1;
        wDevAddrDst = m_pDOMapTab[pBusMsg->wPntNoDest-1].wAddr;
        wPntDst  = m_pDOMapTab[pBusMsg->wPntNoDest-1].wPntNum - 1 ;

        if( wBusNoDst == pBusMsg->wBusNoSrc && wDevAddrDst == pBusMsg->wAddrSrc &&
            wPntDst == pBusMsg->wPntNoSrc )
        {
            //printf("YKPacket %d %d\n",pBusMsg->wPntNoDest , pBusMsg->wVal);
            rtn = YKPacket( pBusMsg->wPntNoDest , pBusMsg->wVal );
            MsgFlag = 0;
        }
        break;
        default:
        printf("ModBusTcp can't find the YK_DATATYPE %d\n", (int)pBusMsg->byAction);
        break;
    }
    return rtn;
}

BOOL CModule_ModBusTcp::YKPacket( int yk_register , int val )
{
    WORD index = 0;
    WORD ModBusTcpByteLen = 6;

    if( yk_register > 10000 || yk_register == 0 )
    {
        ErrorPacket( ERROR_REGISTER );
        memset(ErrorInformation,0,sizeof(ErrorInformation));
        sprintf(ErrorInformation,"%s","遥控寄存器错误");
        return FALSE;
    }
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = 0;
    MsgBuf[index++] = ModBusTcpByteLen;		//ModBusTcp后续字节
    MsgBuf[index++] = m_wAddr;
    MsgBuf[index++] = FunNum;

    MsgBuf[index++] = yk_register>>8;
    MsgBuf[index++] = yk_register;		//寄存器地址

    if( val==YK_CLOSE )
        MsgBuf[index++] = 0xff;		//	分
    else if( val==YK_OPEN )
        MsgBuf[index++] = 0x00;
    else
    {
        BYTE byVal = 0;
        switch( val )
        {
        case YK_ERROR_FUN_01:
        byVal = ERROR_FUN;
        break;
        case YK_ERROR_REGISTER_02:
        byVal = ERROR_REGISTER;
        break;
        case YK_ERROR_DATA_03:
        byVal = ERROR_DATA;
        break;
        case YK_ERROR_DEVSTATE_04:
        byVal = ERROR_DEVSTATE;
        break;
        default:
        byVal = 0x05;
        break;
        }
        ErrorPacket( byVal );
        memset(ErrorInformation,0,sizeof(ErrorInformation));
        sprintf(ErrorInformation,"%s","遥控寄存器数据错误");
        return FALSE;
    }

    MsgBuf[index++] = 0x00;

    MsgLen = index;

    return TRUE;
}

