#include "module_modbusrtu.h"


/*
typedef struct _tagTIMEDATA
{
    unsigned int MiSec:10;
    unsigned int Second:6;
    unsigned int Minute:6;
    unsigned int Hour:5;
    unsigned int Day:5;
    unsigned int Month:4;
    unsigned int Year:12;
} TIMEDATA;
*/
CModule_ModBusRtu::CModule_ModBusRtu( CMethod * m_pMethod , WORD wBusNo , WORD wAddr )
    :CModBusRtu( m_pMethod )
{
    m_wBusNO = wBusNo ;
    m_wAddr = wAddr ;

    pos_flag = -1;
    pos = 0;
    settime_pos = 0;
    yk_pos_num = 0;
    yk_flag = 0;
    readval_flag = -1;
    writeval_flag = -1;
    last_settime = 0;
    timeflag = 0;
    YkNo = 0;
    MsgErrorFlag = MSGTRUE;
    ESL411SOEFlag = 0;
    //memset(store_buf,0,sizeof(store_buf));
    //m_hSem.Create( 20150101 );
    m_wErrorTimer = ERROR_CONST+1;
    //m_byPortStatus = COMSTATUS_FAULT;
    DevCirFlag = FALSE;
    modbus_conf.reserve(100);
    memset(ErrorInformation,0,sizeof(ErrorInformation));
    memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
    MessageChangeState = FALSE;
    MessageChangeByte = 0;
}

BOOL CModule_ModBusRtu::Init( )
{
    //if add data successfully return true or return false
    //default return true
    //这里初始化模板数据
    char szFileName[256] = "";
 /*   if( m_wBusNO == 0 )
    {
        if( m_wAddr == 1 )
            sprintf( m_sTemplatePath, "%s", "EM310.txt" );
        else if( m_wAddr == 2 )
            sprintf( m_sTemplatePath, "%s", "EM330.txt" );
    }
    else if( m_wBusNO == 1 )
    {
        if( m_wAddr == 1 )
            sprintf( m_sTemplatePath, "%s", "EM310.txt" );
        else if( m_wAddr == 2 )
            sprintf( m_sTemplatePath, "%s", "EM330.txt" );
    }
    else if( m_wBusNO == 7 )
        sprintf( m_sTemplatePath, "%s", "modbusEM600LCD-E-DI4.txt" );
    else
        sprintf( m_sTemplatePath, "%s", "modbusEM600LCD-E-DI4.txt" );
 */
    //sprintf( m_sTemplatePath, "%s", "modbusEM600LCD-E-DI4.txt" );
    sprintf( szFileName, "%s%s", MODBUSRTUPREFIXFILENAME, m_sTemplatePath);
    line = ReadConf(szFileName);				//读取配置文件

    if( line <= 0 )
    {
        printf("don't found %s\n",szFileName);
        MODBUSCONF mc;
        DefaultValConfig(&mc);
        modbus_conf.push_back( mc );
        line = 1;
    }
    return TRUE ;
}

BOOL CModule_ModBusRtu::GetReadBuf( BYTE *pBuf , int &len , BYTE &byType, LPVOID pVoid  )
{
    if( modbus_conf.size() <= 0 )
            return FALSE;

    PSMSG pMsg = (PSMSG)pVoid ;
    if( pMsg != NULL )
    {
        PYKST_MSG pYkmsg = NULL ;
        if( pMsg->msgCustomType == SMSG::MSG_YK)
        {
            pYkmsg = ( PYKST_MSG )pMsg ;
            if( pYkmsg != NULL )
            {

                if( TRUE == GetYKBuffer( pBuf , len , pYkmsg ) )
                {
                    if( pYkmsg->byAction == YK_EXEC || ( pYkmsg->byAction == YK_PREPARE && modbus_conf[pos_flag].YkSelFlag == 1 ) )
                    {
                        m_wErrorTimer++;
                        if( m_wErrorTimer > 60000 )
                                m_wErrorTimer = ERROR_CONST + 1 ;
                        MsgRegisteAndData[0] = pBuf[2];
                        MsgRegisteAndData[1] = pBuf[3];
                        MsgRegisteAndData[2] = pBuf[4];
                        MsgRegisteAndData[3] = pBuf[5];
                        return TRUE;
                    }
                }
                else
                    return FALSE;
            }
        }
    }

    if( yk_flag > 0 )
    {
        yk_flag++;
        if( yk_flag >= 3 )
        {
            yk_flag = 0;
        }
        memcpy( pBuf , YkBuf , YkLen );
        len = YkLen;
    }
    else
    {
        if( DevCirFlag == FALSE )
                return FALSE;

        while(1)
        {
            if( ( modbus_conf[pos].cir_flag == 0 ) )
            {
                pos = (pos+1)%line;
                continue;
            }
            if( ( modbus_conf[pos].type == 8 ) )					//记录对时配置所在的位置
            {
                time_t cur_time;
                time(&cur_time);
                timeflag = abs( (int)(cur_time - last_settime) );
                if( timeflag > 300 )
                {
                    timeflag = 0;
                    SendBuf( modbus_conf[settime_pos] , pBuf , &len );
                    pos_flag = settime_pos;
                    time(&last_settime);
                    pos = (pos+1)%line;
                    break;
                }
                else
                {
                    pos = (pos+1)%line;
                    continue;
                }
            }
            else if( ( modbus_conf[pos].type == 3 ) )					//记录遥控配置所在的位置
            {
                pos = (pos+1)%line;
                continue;
            }

            SendBuf( modbus_conf[pos] , pBuf , &len );					//普通处理，遥测遥信
            pos_flag = pos;
            pos = (pos+1)%line;
            break;
        }
        if( ESL411SOEFlag == 1 )
        {
            ;//Esl411SoeSendBuf( pBuf , &len );
        }

    }
    m_wErrorTimer++;
    if( m_wErrorTimer > 60000 )
        m_wErrorTimer = ERROR_CONST + 1 ;
    if( m_wErrorTimer > ERROR_CONST )
        m_pMethod->SetDevCommState(m_wBusNO,m_wAddr,COMM_ABNORMAL);
    MsgRegisteAndData[0] = pBuf[2];
    MsgRegisteAndData[1] = pBuf[3];
    MsgRegisteAndData[2] = pBuf[4];
    MsgRegisteAndData[3] = pBuf[5];
/*
    printf("%d %d tx:",m_wBusNO,m_wAddr);
    int i=0;
    for( i=0;i<len;i++ )
    {
        printf(" %02x ", pBuf[i]);
    }
    printf(" \n ");
*/
    return TRUE ;
}

BOOL CModule_ModBusRtu::ProcessBuf( BYTE *pBuf , int len , BYTE byType , LPVOID pVoid )
{
    BYTE temp_flag = 0;
    BYTE CRC_flag = 0;
    WORD wCRC = 0 ;
/*    printf(" rx:");
    int i=0;
    for( i=0;i<len;i++ )
    {
        printf(" %02x ", pBuf[i]);
    }
    printf(" \n ");
*/
    memset(ErrorInformation,0,sizeof(ErrorInformation));
    memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
    MessageChangeState = FALSE;
    /*判断接受的报文是否正确*/
    while( len > 4 )
    {
        if( pBuf[0] == m_wAddr )
        {
            if( pBuf[1] == modbus_conf[pos_flag].func )
            {
                if( ( ( pBuf[1] > 0 ) && ( pBuf[1] < 5 ) ) || ( pBuf[1] == 0x55 ) || ( pBuf[1] == 0x56 ) )
                {
                    if( ( pBuf[2] + 5 ) <= len )
                    {
                        if( modbus_conf[pos_flag].type == 1 )
                        {
                            if( ( modbus_conf[pos_flag].registe_num%8 != 0) && ( pBuf[2] == 1 + modbus_conf[pos_flag].registe_num/8 ) )
                            {
                                CRC_flag = 1;
                            }
                            else if( ( modbus_conf[pos_flag].registe_num%8 == 0) && ( pBuf[2] == modbus_conf[pos_flag].registe_num/8 ) )
                            {
                                CRC_flag = 1;
                            }
                            else if ( pBuf[2] == modbus_conf[pos_flag].registe_num*2 )
                            {
                                CRC_flag = 1;
                            }
                        }
                        else
                        {
                            if( pBuf[1] == 0x55 || ( pBuf[1] == 0x56 ) )
                            {
                                CRC_flag = 1;
                            }
                            if ( pBuf[2] == modbus_conf[pos_flag].registe_num*2 )
                            {
                                CRC_flag = 1;
                            }

                        }


                        if( CRC_flag == 1 )
                        {
                            wCRC = GetCRC( pBuf, pBuf[2] + 3 );
                            if( ( HIBYTE(wCRC) == pBuf[ pBuf[2] + 3 ] ) && ( LOBYTE(wCRC) == pBuf[ pBuf[2] + 4 ] ) )
                            {
                                temp_flag = 1;
                                break;
                            }
                            else
                            {
                                if( !MessageChangeState )
                                {
                                    memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                                    sprintf(FirstErrorInformation,"%s","CRC校验不正确");
                                }
                            }
                        }
                        else
                        {
                            if( !MessageChangeState )
                            {
                                memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                                sprintf(FirstErrorInformation,"%s","接受报文的解析字节长度与配置寄存器数目不匹配");
                            }
                        }
                    }
                    else
                    {
                        if( !MessageChangeState )
                        {
                            memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                            sprintf(FirstErrorInformation,"%s","实际接受报文长度小于解析的报文字节长度");
                        }
                    }
                }
                else if ( pBuf[1] == 0x5 || pBuf[1] == 0x6 || pBuf[1] == 0xf || pBuf[1] == 0x10 )
                {
                    if( MsgRegisteAndData[0] == pBuf[2] && MsgRegisteAndData[1] == pBuf[3] &&
                            MsgRegisteAndData[2] == pBuf[4] && MsgRegisteAndData[3] == pBuf[5] )
                    {
                        wCRC = GetCRC( pBuf, 6 );
                        if( ( HIBYTE(wCRC) == pBuf[ 6 ] ) && ( LOBYTE(wCRC) == pBuf[ 7 ] ) )
                        {
                            temp_flag = 1;
                            break;
                        }
                        else
                        {
                            if( !MessageChangeState )
                            {
                                memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                                sprintf(FirstErrorInformation,"%s","CRC校验不正确");
                            }
                        }
                    }
                }
            }
            else if( pBuf[1] == ( modbus_conf[pos_flag].func | 0x80 ) )
            {
                wCRC = GetCRC( pBuf, 3 );
                if( ( HIBYTE(wCRC) == pBuf[ 3 ] ) && ( LOBYTE(wCRC) == pBuf[ 4 ] ) )
                {
				//	printf("%s %d pBuf[0] = %x, pBuf[1] = %x, pBuf[2] = %x\n", __func__, __LINE__, pBuf[0], pBuf[1], pBuf[2]);
                    switch(modbus_conf[pos_flag].type)
                    {
                        case 3:
                        WORD wVal = 0;
                        yk_flag = 0;
                        switch( pBuf[2] )
                        {
                        case 0x01:
                        wVal = YK_ERROR_FUN_01;
                        break;
                        case 0x02:
                        wVal = YK_ERROR_REGISTER_02;
                        break;
                        case 0x03:
                        wVal = YK_ERROR_DATA_03;
                        break;
                        case 0x04:
                        wVal = YK_ERROR_DEVSTATE_04;
                        break;
                        default:
                        wVal = YK_ERROR_DEVSTATE_04;
                        break;
                        }
                        if( wSrcBusNo == (WORD)-1 || wSrcDevAddr == (WORD)-1 || wSrcPnt == (WORD)-1 )
                            break;
                        m_pMethod->SengYK(wSrcBusNo,wSrcDevAddr,wSrcPnt,
                                          m_wBusNO,m_wAddr,YkNo,YK_EXEC_RTN,wVal);
                        break;
                    }
                    char buffer[100] = "";
                    sprintf(buffer,"func %x errno num:%d\n",pBuf[1],pBuf[2]);
                    memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                    sprintf(FirstErrorInformation,"%s","装置返回错误报文,请查看对应协议");

                    break;
                }
            }
            else if( modbus_conf[pos_flag].YkSelFlag == 1 && pBuf[1] == 0x10 && modbus_conf[pos_flag].func == 0x05 )
            {
                if( pBuf[2] == 0x40 && pBuf[3] == 0x59 && pBuf[4] == 0x00 && pBuf[5] == 0x03 )
                {
                    wCRC = GetCRC( pBuf, 6 );
                    if( ( HIBYTE(wCRC) == pBuf[ 6 ] ) && ( LOBYTE(wCRC) == pBuf[ 7 ] ) )
                    {
                        //m_pMethod->SetYkSelRtn ( this , bySrcBusNo, wSrcDevAddr , YkNo , YkVal );
                        return TRUE ;
                    }
                    else
                    {
                        if( !MessageChangeState )
                        {
                            memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                            sprintf(FirstErrorInformation,"%s","CRC校验不正确");
                        }
                    }
                }
            }
            else if( modbus_conf[pos_flag].SoeFlag == 2 && ( pBuf[1] == 0x02 || pBuf[1] == 0x03 || pBuf[1] == 0x0c) )
            {
                wCRC = GetCRC( pBuf, pBuf[2] + 3 );
                if( ( HIBYTE(wCRC) == pBuf[ pBuf[2] + 3 ] ) && ( LOBYTE(wCRC) == pBuf[ pBuf[2] + 4 ] ) )
                {
                    temp_flag = 1;
                    break;
                }
                else
                {
                    if( !MessageChangeState )
                    {
                        memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                        sprintf(FirstErrorInformation,"%s","CRC校验不正确");
                    }
                }
            }
            else
            {
                if( !MessageChangeState )
                {
                    memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                    sprintf(FirstErrorInformation,"%s","功能码不一致");
                }
            }
        }
        else
        {
            if( !MessageChangeState )
            {
                memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                sprintf(FirstErrorInformation,"%s","收发报文装置地址不一致");
            }
        }

        pBuf = pBuf + 1 ;
        MessageChangeState = TRUE;
        len--;
        MessageChangeByte++;
    }

    if( temp_flag == 1 )
    {
        switch(modbus_conf[pos_flag].type)
        {
            case 1:
            ModBusYxDeal(pBuf,modbus_conf[pos_flag]);
            break;
            case 2:
            ModBusYcDeal(pBuf,modbus_conf[pos_flag]);
            break;
            case 3:
            ModBusYkDeal(pBuf,modbus_conf[pos_flag] );
            break;
            case 4:
            ModBusYmDeal(pBuf,modbus_conf[pos_flag]);
            break;
/*          case 5:
            //ModBusReadVal(pBuf,modbus_conf[pos_flag]);
            if( !MessageChangeState )
            {
                memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                sprintf(FirstErrorInformation,"%s","暂不支持读定值功能");
            }
            break;
            case 6:
            //ModBusWriteVal(pBuf,modbus_conf[pos_flag]);
            if( !MessageChangeState )
            {
                memset(FirstErrorInformation,0,sizeof(FirstErrorInformation));
                sprintf(FirstErrorInformation,"%s","暂不支持写定值功能");
            }
            break;
*/
            case 8:
            ModBusSetTime(pBuf,modbus_conf[pos_flag]);
            break;
            case 9:
            ModBusSoeDeal(pBuf,modbus_conf[pos_flag]);
            break;
            default:
            memset(ErrorInformation,0,sizeof(ErrorInformation));
            sprintf(ErrorInformation,"%s","配置功能类型错误");
            return FALSE;
        }
    }
    else if( temp_flag == 0 )
    {
        memset(ErrorInformation,0,sizeof(ErrorInformation));
        strcpy(ErrorInformation,FirstErrorInformation);

        return FALSE;
    }
    if( MsgErrorFlag == MSGERROR )
    {
        MsgErrorFlag = MSGTRUE;
        //char buffer[100] = "the config is wrong!";
        //printf("--%s\n",buffer);
        //OutBusDebug(m_wBusNO, (BYTE *)buffer, strlen(buffer),3);
        return FALSE;
    }
    m_wErrorTimer = 0;								//接收到消息置0
    m_pMethod->SetDevCommState(m_wBusNO,m_wAddr,COMM_NOMAL);
    if( MessageChangeState )
    {
        memset(ErrorInformation,0,sizeof(ErrorInformation));
        sprintf(ErrorInformation,"偏移%d个字节后，解析正确",MessageChangeByte);
    }

    return TRUE ;
}

int CModule_ModBusRtu::GetErrorMsg( BYTE *pBuf , int len )
{
    strcpy((char *)pBuf,ErrorInformation);
    return strlen(ErrorInformation);
}

//读取配置文件
int CModule_ModBusRtu::ReadConf(char *filename)
{
    FILE *hFile;
    char szText[160];
    char *temp;
    int num = 0;
    BYTE i = 0;
    BYTE conflag = 0;
    MODBUSCONF mc;
    INFO yk,readval,writeval;

    //m_hSem.semTake();

    hFile = fopen(filename, "r");

    if(hFile == NULL)
    {
            //printf("FT:fopen conf error!\n");
            //m_hSem.semGive();
            return 0;
    }

    while( fgets(szText, sizeof(szText), hFile) != NULL )
    {
        //rtrim(szText);
        if( szText[0]=='#' || szText[0]==';' )
                continue;
        i = 0;
        conflag = 0;
        memset(&mc,0,sizeof(mc));

        temp = strtok(szText,",");
        if(temp == NULL)
            continue;
        if( ( atoi(temp) > 0 ) && ( atoi(temp) < 10 ) )
                mc.type = atoi(temp);
        else
        {
                conflag = 1;
                DefaultValConfig(&mc);
        }
        while( ( temp = strtok(NULL,",") ) )
        {
            switch(++i)
            {
                case 1:
                if( ( ( Atoh(temp) > 0 ) && ( Atoh(temp) <= 7 ) ) || ( Atoh(temp) == 0x0f )
                        || ( Atoh(temp) == 0x10 ) || ( Atoh(temp) == 0x55 ) || ( Atoh(temp) == 0x0c )
                        || ( Atoh(temp) == 0x56 ))
                        mc.func = Atoh(temp);
                else
                        conflag = 1;
                break;
                case 2:
                if( Atoh(temp) <= 0xffff )
                        mc.registe = Atoh(temp);
                else
                        conflag = 1;
                break;
                case 3:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 0xffff ) )
                        mc.registe_num = atoi(temp);
                else
                        conflag = 1;
                break;
                case 4:
                        //偏移字节大于零小于最大传输字节数 260 - 2
                if(  atoi(temp) >= 0 && atoi( temp ) <= 258 )
                        mc.skew_byte = atoi(temp);
                else
                        conflag = 1;
                break;
                case 5:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 0xffff ) )
                        mc.get_num = atoi(temp);
                else
                        conflag = 1;
                break;
                case 6:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 0xffff ) )
                        mc.start_num = atoi(temp);
                else
                        conflag = 1;
                break;
                case 7:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 4 ) )
                        mc.data_len = atoi(temp);
                else
                        conflag = 1;
                break;
                case 8:
                if( Atoh(temp) <= 0xffffffff )
                        mc.mask_code = Atoh(temp);
                else
                        conflag = 1;
                break;
                case 9:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 5 ) )
                        mc.data_form = atoi(temp);
                else
                        conflag = 1;
                break;
                case 10:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 3 ) )
                        mc.sign = atoi(temp);
                else
                        conflag = 1;
                break;
                case 11:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 2 ) )
                        mc.yk_form = atoi(temp);
                else
                        conflag = 1;
                break;
                case 12:
                if( ( atoi(temp) == 0 ) || ( atoi(temp) == 1 ) )
                {
                        mc.cir_flag = atoi(temp);
                        if( mc.cir_flag == 1 )
                                DevCirFlag = TRUE;
                }
                else
                        conflag = 1;
                break;
                case 13:
                {
                    if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 0xffff ) )
                            mc.YkClose= Atoh(temp);
                    else
                            conflag = 1;
                }
                break;
                case 14:
                {
                    if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 0xffff ) )
                            mc.YkOpen= Atoh(temp);
                    else
                            conflag = 1;
                }
                break;
                case 15:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 2 ) )
                        mc.SoeFlag = atoi(temp);
                else
                        conflag = 1;
                break;
                case 16:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 2 ) )
                        mc.SetTimeFlag = atoi(temp);
                else
                        conflag = 1;
                break;
                case 17:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 2 ) )
                        mc.YkSelFlag = atoi(temp);
                else
                        conflag = 1;
                break;
                case 18:
                if( ( atoi(temp) >= 0 ) && ( atoi(temp) <= 2 ) )
                        mc.YkExctFlag = atoi(temp);
                else
                        conflag = 1;
                break;
                default:
                conflag = 1;
                printf("\n\n\n%d > 18\n\n\n",i+1);
                break;
            }
            if( conflag == 1 )
            {
                conflag = 1;
                printf("ModBus config file error:\n");
               // continue;
            }

            }
            if( conflag == 1 )
            {
                printf("%s num is %d %d\n\n\n",filename,num+1,i+1);
                DefaultValConfig(&mc);
            }
            else if( i < 18)
            {
                switch( i + 1 )
                {
                    case 13:mc.YkClose= 0xff00;
                    case 14:mc.YkOpen= 0x0000;
                    case 15:mc.SoeFlag = 0;
                    case 16:mc.SetTimeFlag = 0;
                    case 17:mc.YkSelFlag = 0;
                    case 18:mc.YkExctFlag = 0;break;
                }
            }
            /*   printf("%d %d %d %d %d %d %d %d %02x %d %d %d %d %d %d %d \n",mc.type,mc.func,
                    mc.registe,mc.registe_num,mc.skew_byte,mc.get_num,mc.start_num,
                    mc.data_len,mc.mask_code,mc.data_form,mc.sign,mc.yk_form,mc.cir_flag,mc.SetTimeFlag ,
                    mc.SoeFlag,mc.YkSetFlag); */
            modbus_conf.push_back( mc );
            num++;
        }
        int freturn = fclose(hFile);//perror("fclose");
        if( freturn )
            perror("fclose");

        //m_hSem.semGive();

        for(i=0;i<num;i++)
        {
            if( modbus_conf[i].type == 3 )
            {
                yk.pos =  i;
                yk.start_num =  modbus_conf[i].start_num;
                yk.get_num =  modbus_conf[i].get_num;

        //	printf("yk_pos_num===%d %d %d %d\n",yk_pos_num,yk.pos,yk.start_num,yk.get_num);
                yk_info.push_back( yk );
                yk_pos_num++;
            }
            if( modbus_conf[i].type == 5 )
            {
                readval.pos =  i;
                readval.start_num =  modbus_conf[i].start_num;
                readval.get_num =  modbus_conf[i].get_num;

                // printf("readval_pos_num===%d %d %d %d\n",readval_pos_num,readval.pos,readval.start_num,readval.get_num);
        //	readval_info.push_back( readval );
                readval_pos_num++;
            }
            if( modbus_conf[i].type == 6 )
            {
                writeval.pos =  i;
                writeval.start_num =  modbus_conf[i].start_num;
                writeval.get_num =  modbus_conf[i].get_num;

                // printf("writeval_pos_num===%d %d %d %d\n",writeval_pos_num,writeval.pos,writeval.start_num,writeval.get_num);
        //	writeval_info.push_back( writeval );
                writeval_pos_num++;
            }
            else if( modbus_conf[i].type == 8 )
            {
                settime_pos = i;
            }
        }
    return num;
}

//字符串转十六进制，读取配置文件时使用
DWORD CModule_ModBusRtu::Atoh(char *buf)
{
    BYTE i = 0;
    DWORD tempvalue = 0;
    DWORD value = 0;
    BYTE len = strlen(buf);

    for(i=0;i<len;i++)
    {
        if( (buf[i]>='A') && (buf[i]<='F') )
        {
                tempvalue = buf[i]-'A'+10;
        }
        if( (buf[i]>='a') && (buf[i]<='f') )
        {
                tempvalue = buf[i]-'a'+10;
        }
        if( (buf[i]>='0') && (buf[i]<='9') )
        {
                tempvalue = buf[i]-'0';
        }
        value = value*16+tempvalue;
    }
    return value;
}

//当配置文件某行出现问题时，想该行换成默认配置，该默认配置，可发非正常报文
void CModule_ModBusRtu::DefaultValConfig(MODBUSCONF * mc)
{
        mc->type = 1;
        mc->func = 2;
        mc->registe = 0x0064;
        mc->registe_num = 0;
        mc->skew_byte = 3;
        mc->get_num = 0;
        mc->start_num = 0;
        mc->data_len = 1;
        mc->mask_code = 0xffffffff;
        mc->data_form = 0;
        mc->sign = 0;
        mc->yk_form = 2;
        mc->cir_flag = 1;
}

//遥信，遥测，对时等发送报文
void CModule_ModBusRtu::SendBuf( MODBUSCONF modbusconf, BYTE * buf ,int *len )
{
        BYTE index = 0;

        buf[ index++ ] = m_wAddr;
        buf[ index++ ] = modbusconf.func;
        if( modbusconf.type == 9 && modbusconf.SoeFlag != 1 )
        {
                ;
        }
        else
        {
                buf[ index++ ] = modbusconf.registe >> 8;
                buf[ index++ ] = modbusconf.registe ;
                buf[ index++ ] = modbusconf.registe_num >> 8;
                buf[ index++ ] = modbusconf.registe_num ;

        }
        if( modbusconf.type == 8 )
        {
                index = SysLocalTime(modbusconf,buf,index);
        }

        WORD wCRC = GetCRC( buf, index );
        buf[ index++ ] = HIBYTE(wCRC);
        buf[ index++ ] = LOBYTE(wCRC);
        *len = index;
/*
        printf("sendbuf %d %d len = %dtx:",m_wBusNO,m_wAddr,*len);
        int i=0;
        for( i=0;i<*len;i++ )
        {
            printf(" %02x ", buf[i]);
        }
        printf(" \n ");
        */
}

int CModule_ModBusRtu::TimePackMsecBigEdian(MODBUSCONF modbusconf,unsigned char *buffer,int i,struct tm *p,WORD msec)
{
        buffer[i++] = modbusconf.registe_num*2;
        buffer[i++] = (p->tm_year)-100;
        buffer[i++] = 1+(p->tm_mon);
        buffer[i++] = p->tm_mday;
        buffer[i++] = p->tm_hour;
        buffer[i++] = p->tm_min;
        buffer[i++] = p->tm_sec;
        buffer[i++] = msec >> 8;
        buffer[i++] = msec ;
        return i;
}

int CModule_ModBusRtu::TimePackMsecLittleEdian(MODBUSCONF modbusconf,unsigned char *buffer,int i,struct tm *p,WORD msec)
{
        buffer[i++] = modbusconf.registe_num*2;
        buffer[i++] = (p->tm_year)-100;
        buffer[i++] = 1+(p->tm_mon);
        buffer[i++] = p->tm_mday;
        buffer[i++] = p->tm_hour;
        buffer[i++] = p->tm_min;
        buffer[i++] = p->tm_sec;
        buffer[i++] = msec;
        buffer[i++] = msec >> 8;
        return i;
}

int CModule_ModBusRtu::TimePackEM310(MODBUSCONF modbusconf,unsigned char *buffer,int i,struct tm *p,WORD msec)
{
    WORD tmpSecond = p->tm_min*60+p->tm_sec;
    i -= 2;
    buffer[i++] = HIBYTE(tmpSecond);
    buffer[i++] = LOBYTE(tmpSecond);
    //printf("time %d:%d  buf%d %d\n",p->tm_min,p->tm_sec,HIBYTE(tmpSecond),LOBYTE(tmpSecond));
    return i;
}

//时间组包
int CModule_ModBusRtu::SysLocalTime(MODBUSCONF modbusconf,unsigned char *buffer,int i)
{
    struct timeval tv;
    struct timezone tz;

    //struct tm *p;
        struct tm p;

    gettimeofday( &tv , &tz );

   // p = localtime( &tv.tv_sec );
        localtime_r( &tv.tv_sec  , &p );

        char usec[10] = "";
        sprintf( usec, "%06ld", tv.tv_usec);
        switch( strlen(usec) )
        {
                case 4:
                usec[1] = '\0';
                break;
                case 5:
                usec[2] = '\0';
                break;
                case 6:
                usec[3] = '\0';
                break;
                default:
                memset( usec , 0 , sizeof(usec) );
                break;
        }
        WORD msec = atoi(usec);

        switch( modbusconf.SetTimeFlag )
        {
            case 0:
                    i = TimePackMsecBigEdian(modbusconf,buffer,i,&p,msec);
            break;
            case 1:
                    i = TimePackMsecLittleEdian(modbusconf,buffer,i,&p,msec);
            break;
            case 2:
                    i = TimePackEM310(modbusconf,buffer,i,&p,msec);
            break;
            default:
                    i = TimePackMsecBigEdian(modbusconf,buffer,i,&p,msec);
            break;
        }
        return i;
}

//遥信安位处理
void CModule_ModBusRtu::ModBusYxBitDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{
        WORD i = 0;
        WORD j = 0;
        BYTE pos = modbusconf.skew_byte;
        DWORD temp_buf = buffer[pos];
        DWORD temp_mask = modbusconf.mask_code;
        WORD get_num = modbusconf.get_num;
        WORD start_num = modbusconf.start_num;
        BYTE data_len = modbusconf.data_len;
        WORD real_get_num = buffer[2] * 8;

        WORD num = 0;
        WORD wVal = 0;
        DWORD real_temp_mask =0;

        if( modbusconf.SoeFlag == 2 )
        {
                get_num = 25;
        }
        //配置的采集数量不能大于可以解析的数量
        if( real_get_num < get_num )
        {
                get_num = real_get_num;
        }

        for(i=0;i<(( (real_get_num-1) / (8*data_len) )+1);i++)
        {
                switch( data_len )
                {
                        case 1:
                        real_temp_mask = ModBusYXTempValue( (unsigned char*)&temp_mask , 3 , modbusconf );
                        break;
                        case 2:
                        real_temp_mask = ModBusYXTempValue( (unsigned char*)&temp_mask , 2 , modbusconf );
                        break;
                        case 4:
                        real_temp_mask = ModBusYXTempValue( (unsigned char*)&temp_mask , 0 , modbusconf );
                        break;
                        default:
                        MsgErrorFlag = MSGERROR;
                        memset(ErrorInformation,0,sizeof(ErrorInformation));
                        sprintf(ErrorInformation,"数据长度错误配置为%d,没有该长度处理方式",data_len);
                        return;
                }

                temp_buf = ModBusYXTempValue( buffer , pos , modbusconf );
                //printf("real_temp_mask =  %x temp_buf = %04x\n",real_temp_mask,temp_buf);
                pos += data_len;

                for(j=0;j<data_len*8;j++)
                {
                        if( num >= modbusconf.get_num )
                        {
                                return;
                        }
                        if(real_temp_mask%2)												//屏蔽码处理
                        {
                                wVal = temp_buf%2;
                                //printf("ModBusRtu SetYxData \n");
                                //char buf[100];
                                //sprintf(buf,"YX1 m_byLineNo:%d m_wDevAddr%d num:%d val:%d\n", m_wBusNO, m_wAddr , num+start_num , wVal);
                                //printf("--%s\n",buf);
                                //m_pMethod->SetYxData ( m_SerialNo , num+start_num , wVal );
                           // printf("SetYX  BusNO=%d , Addr=%d, Ponit=%d val =%d\n",m_wBusNO,m_wAddr,num+start_num,wVal);
                                if( !m_pMethod->SetYx(m_wBusNO,m_wAddr,num+start_num,wVal) )
                                    printf("SetYX error BusNO=%d , Addr=%d, Ponit=%d\n",m_wBusNO,m_wAddr,num+start_num);

                                //OutBusDebug(m_byLineNo, (BYTE *)buf, strlen(buf),3);
                                num++;
                        }
                        real_temp_mask /= 2;
                        temp_buf /= 2;
                }
        }
}

DWORD CModule_ModBusRtu::ModBusYXTempValue(unsigned char *buffer, int pos, MODBUSCONF modbusconf)
{
        int value_i = 0;
        unsigned int value_ui = 0;
        short value_s = 0;
        unsigned short value_us = 0;
        BYTE value_b;

        if(modbusconf.data_len == 1)
        {
                value_b =  buffer[pos] ;
                return (DWORD)value_b;
        }
        else if(modbusconf.data_len == 2)
        {
                value_s = (unsigned short)TwoByteValue(buffer,pos,modbusconf);
                memcpy( &value_us , &value_s , 2 );
                return (DWORD)value_us;
        }
        else if(modbusconf.data_len == 4)
        {
                value_i = (unsigned int)FourByteValue(buffer,pos,modbusconf);
                memcpy( &value_ui , &value_i , 4 );
                return value_ui;
        }
        else
        {
                MsgErrorFlag = MSGERROR;
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"数据长度配置错误，错误配置为%d，没有该长度处理方式",modbusconf.data_len);
        }
        return 0;
}

//遥信处理
void CModule_ModBusRtu::ModBusYxDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{
    ModBusYxBitDeal(buffer,modbusconf);
}

//遥测处理
void CModule_ModBusRtu::ModBusYcDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{
        BYTE pos = modbusconf.skew_byte;
        WORD get_num = modbusconf.get_num;
        WORD start_num = modbusconf.start_num;
        WORD real_get_num = buffer[2] / modbusconf.data_len;

        WORD i = 0;
        float fVal = 0 ;
        for(i=0;i<get_num;i++)
        {
                fVal = 0 ;
                if( i < real_get_num )
                        fVal = ModBusValue(buffer,pos,modbusconf);
                if( MsgErrorFlag == MSGERROR )
                {
                        return;
                }
                pos += modbusconf.data_len;
                //m_pMethod->SetYcData( m_SerialNo , i+start_num , wVal );
                //printf("ModBusRtu SetYcData \n");
                //char buf[100];
                //sprintf(buf,"YC m_byLineNo:%d m_wDevAddr%d num:%d val:%f\n",m_wBusNO, m_wAddr , i+start_num , wVal);
                //printf("--%s\n",buf);

                if( !m_pMethod->SetYc(m_wBusNO,m_wAddr,i+start_num,fVal) )
                    printf("SETYC error BusNO=%d , Addr=%d, Ponit=%d\n",m_wBusNO,m_wAddr,i+start_num);

                //OutBusDebug(m_byLineNo, (BYTE *)buf, strlen(buf),3);
        }
}

void CModule_ModBusRtu::ModBusYkDeal(unsigned char *buffer,MODBUSCONF modbusconf )
{
        switch( modbusconf.YkExctFlag )
        {
                case 0:
                ModBusRtuYkDeal( buffer , modbusconf );
                break;
                case 1:
                //ModBusJ05YkDeal( buffer , modbusconf );
                break;
                default:
                MsgErrorFlag = MSGERROR;
                return;
        }
}

//遥控处理
void CModule_ModBusRtu::ModBusRtuYkDeal(unsigned char *buffer,MODBUSCONF modbusconf )
{
        WORD i = 0;
        BYTE ykVal= 2;
        BYTE pos = modbusconf.skew_byte ;
        DWORD get_num = modbusconf.get_num;
        DWORD start_num = modbusconf.start_num;
        DWORD registe = modbusconf.registe;

        unsigned int recv_buf[get_num];
        memset(recv_buf,0,get_num);

        yk_flag = 0;

        for(i=0;i<get_num;i++)
        {
                recv_buf[i] = (unsigned int )ModBusValue(buffer,pos,modbusconf);
                if( MsgErrorFlag == MSGERROR )
                {
                        return;
                }
                pos += modbusconf.data_len;
        }
        int rrrrrr = recv_buf[0];
        if( ( registe + YkNo - start_num ) == recv_buf[0] )
        {
                if( 0xff00 == recv_buf[1] )
                        ykVal = YK_CLOSE;
                else if( 0x0000 == recv_buf[1] )
                        ykVal = YK_OPEN;
                //printf("ModBusRtu SetYkExeRtn\n");
                //char buf[100];
                //sprintf(buf,"YK m_byLineNo:%d m_wDevAddr%d num:%d val:%d\n", m_wBusNO, m_wAddr , YkNo , ykVal);
                //printf("--%s\n",buf);
                //m_pMethod->SetYkExeRtn ( this , bySrcBusNo, wSrcDevAddr , YkNo , ykVal );
                if( wSrcBusNo == (WORD)-1 || wSrcDevAddr == (WORD)-1 || wSrcPnt == (WORD)-1 )
                    return ;
                m_pMethod->SengYK(wSrcBusNo,wSrcDevAddr,wSrcPnt,
                                  m_wBusNO,m_wAddr,YkNo,YK_EXEC_RTN,ykVal);

                // OutBusDebug(m_byLineNo, (BYTE *)buf, strlen(buf),3);
        }
}

//遥脉处理
void CModule_ModBusRtu::ModBusYmDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{
        BYTE pos = modbusconf.skew_byte;
        WORD get_num = modbusconf.get_num;
        WORD start_num = modbusconf.start_num;
        WORD real_get_num = buffer[2] / modbusconf.data_len;

        WORD i = 0;
        float wVal = 0 ;

        for(i=0;i<get_num;i++)
        {
                wVal = 0;
                if( i < real_get_num )
                        wVal = ModBusValue(buffer,pos,modbusconf);
                if( MsgErrorFlag == MSGERROR )
                {
                        return;
                }
                pos += modbusconf.data_len;
                //printf("ModBusRtu SetYmData \n");
                //char buf[100];
                //sprintf(buf,"YM m_byLineNo:%d m_wDevAddr%d num:%d val:%f\n", m_wBusNO, m_wAddr , i+start_num , wVal);
                //printf("--%s\n",buf);
                //m_pMethod->SetYmData ( m_SerialNo, i+start_num, wVal );

                if( !m_pMethod->SetYm(m_wBusNO,m_wAddr,i+start_num,wVal) )
                    printf("SETYM error BusNO=%d , Addr=%d, Ponit=%d\n",m_wBusNO,m_wAddr,i+start_num);

                // OutBusDebug(m_byLineNo, (BYTE *)buf, strlen(buf),3);
        }
}

//对时处理
void CModule_ModBusRtu::ModBusSetTime(unsigned char *buffer,MODBUSCONF modbusconf)
{
        BYTE pos = modbusconf.skew_byte ;
        WORD get_num = modbusconf.get_num;
        // unsigned int registe = modbusconf.registe;
        // unsigned int registe_num = modbusconf.registe_num;

        unsigned int recv_buf[get_num];
        memset(recv_buf,0,get_num);

        WORD i = 0;

        for(i=0;i<get_num;i++)
        {
                recv_buf[i] = (unsigned int )ModBusValue(buffer,pos,modbusconf);
                pos += modbusconf.data_len;
        }
/* 	if( (registe==recv_buf[0]) && (registe_num==recv_buf[1]))
                printf("settime success!!\n");
        else
                printf("settime defeat!!\n"); */
}

void CModule_ModBusRtu::ModBusRtuSoeDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{
        BYTE pos = modbusconf.skew_byte;
        //WORD start_num = modbusconf.start_num;
        int i = 0;
        BYTE soeflag = 0;
        REALTIME ptmData;
        //printf("bus%d ModBusRtuSoeDeal buffer[%d-1] = %d\n",m_wBusNO,pos,buffer[pos-1]);
        for(i=1;i<buffer[pos-1];i+=8)						//从1开始，如果无soe，报文字节数(第三个字节)为1
        {
                if( 0x0 == ( buffer[i+pos] & 0xc0 ) )
                {
                        soeflag = 1;
                }
                else if( 0xc0 == ( buffer[i+pos] & 0xc0 ) )
                {
                        soeflag = 0;
                }
                else if( 0x80 == ( buffer[i+pos] & 0xc0 ) )
                {
                        soeflag = 1;
                }
                else
                {
                        continue;
                }

                ptmData.wMilliSec= ( (buffer[i+pos+6] & 0xc0) << 2 ) | buffer[i+pos+7];
                ptmData.wSecond= buffer[i+pos+6] & 0x3f;
                ptmData.wMinute= buffer[i+pos+5];
                ptmData.wHour = buffer[i+pos+4];
                ptmData.wDay = buffer[i+pos+3];
                ptmData.wMonth = buffer[i+pos+2];
                ptmData.wYear = 2000+buffer[i+pos+1];

                m_pMethod->SetSoe(m_wBusNO,m_wAddr,(buffer[i+pos] & 0x3f)+modbusconf.start_num,soeflag,&ptmData);
               // m_pMethod->SetYxDataWithTime ( m_SerialNo, buffer[i+pos] & 0x3f, soeflag, &ptmData );
               /* printf("ModBusRtu SetYxDataWithTime \n");
                char buf[100] = "";
                sprintf(buf,"SOE m_byLineNo:%d m_wDevAddr%d num:%d val:%d time:%d.%d.%d-%d:%d:%d.%d\n",
                                m_wBusNO, m_wAddr , buffer[i+pos] & 0x3f , soeflag ,
                                ptmData.wYear,ptmData.wMonth,ptmData.wDay,ptmData.wHour,ptmData.wMinute,ptmData.wSecond,ptmData.wMilliSec);
                printf("--%s\n",buf);
                printf("%d %d tx:",m_wBusNO,m_wAddr);
                int i=0,len=buffer[pos-1]+5;
                for( i=0;i<len;i++ )
                {
                    printf(" %02x ", buffer[i]);
                }
                printf(" \n ");
             */   // OutBusDebug(m_byLineNo, (BYTE *)buf, strlen(buf),3);
        }
}

void CModule_ModBusRtu::ModBusYZ202SoeDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{
        BYTE pos = modbusconf.skew_byte;
        //WORD start_num = modbusconf.start_num;
        //int i = 0;
        WORD wPnt = 0;
        BYTE soeflag = 0;
        REALTIME ptmData;

        if( ( buffer[pos+1] & 0x02 ) == 0x02 )
                soeflag = 1;
        else if( ( buffer[pos+1] & 0x02 ) == 0x00 )
                soeflag = 0;

        switch( buffer[pos] )
        {
                case 4:wPnt = 0;break;
                case 8:wPnt = 1;break;
                case 9:wPnt = 2;break;
                case 16:
                        soeflag = 1;
                        switch( buffer[pos+10] )
                        {
                                case 0x00:wPnt = 3;break;
                                case 0x01:wPnt = 4;break;
                                case 0x02:wPnt = 5;break;
                                case 0x03:wPnt = 6;break;
                                case 0x04:
                                        switch( buffer[pos+11] )
                                        {
                                                case 0x33:wPnt = 7;soeflag = 0;break;
                                                case 0x55:wPnt = 7;break;
                                                default:
                                                MsgErrorFlag = MSGERROR;
                                                return;
                                        }
                                break;
                                case 0x05:
                                        switch( buffer[pos+11] )
                                        {
                                                case 0x33:wPnt = 8;soeflag = 0;break;
                                                case 0x55:wPnt = 8;break;
                                                default:
                                                MsgErrorFlag = MSGERROR;
                                                return;
                                        }
                                break;
                                default:
                                MsgErrorFlag = MSGERROR;
                                return;
                        }
                break;
                case 17:wPnt = 9;break;
                case 18:wPnt = 10;break;
                case 19:wPnt = 11;break;
                case 21:wPnt = 12;break;
                case 22:wPnt = 13;break;
                case 23:wPnt = 14;break;
                case 24:wPnt = 15;break;
                case 25:wPnt = 16;break;
                case 26:wPnt = 17;break;
                case 27:wPnt = 18;break;
                case 28:wPnt = 19;break;
                case 31:wPnt = 20;break;
                case 35:wPnt = 21;break;
                case 37:wPnt = 22;break;
                case 38:wPnt = 23;break;
                case 39:wPnt = 24;break;
                case 42:wPnt = 25;break;
                default:
                MsgErrorFlag = MSGERROR;
                return;
        }



        ptmData.wMilliSec= buffer[pos+8] | (buffer[pos+9] << 8);
        ptmData.wSecond= buffer[pos+7];
        ptmData.wMinute= buffer[pos+6];
        ptmData.wHour = buffer[pos+5];
        ptmData.wDay = buffer[pos+4];
        ptmData.wMonth = buffer[pos+3];
        ptmData.wYear = buffer[pos+2];
        m_pMethod->SetSoe(m_wBusNO,m_wAddr,wPnt,soeflag,&ptmData);
        //m_pMethod->SetYxDataWithTime ( m_SerialNo, wPnt, soeflag, &ptmData );
        //printf("ModBusRtu SetYxDataWithTime \n");
        // char buf[100] = "";
        // sprintf(buf,"SOE m_byLineNo:%d m_wDevAddr%d num:%d val:%d time:%d.%d.%d-%d:%d:%d.%d\n",
                        // m_byLineNo, m_wDevAddr , wPnt , soeflag ,
                        // ptmData.Year,ptmData.Month,ptmData.Day,ptmData.Hour,ptmData.Minute,ptmData.Second,ptmData.MiSec);
        // OutBusDebug(m_byLineNo, (BYTE *)buf, strlen(buf),3);

}

void CModule_ModBusRtu::ModBusEM310SoeDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{
    BYTE pos = modbusconf.skew_byte;
    //WORD start_num = modbusconf.start_num;
    int i = 0;
    BYTE soeflag = 0;
    REALTIME ptmData;
    WORD YxNo=0;

    for(i=1;i<buffer[pos-1];i+=4)						//1开始，如果无soe，报文字节数(第三个字节)为0
    {
            if( 0x0 == ( buffer[i+pos] & 0xc0 ) )
            {
                    soeflag = 1;
            }
            else if( 0xc0 == ( buffer[i+pos] & 0xc0 ) )
            {
                    soeflag = 0;
            }
            else
            {
                    continue;
            }
            YxNo = (buffer[i+pos] & 0x3f)+modbusconf.start_num;

            m_pMethod->GetSysTime((void*)&ptmData);

            ptmData.wMilliSec= buffer[i+pos+3] | ((buffer[i+pos+2] & 0x0f) << 8);
            ptmData.wSecond= (buffer[i+pos+1] | ((buffer[i+pos+2] >> 4)<<8))%60;
            ptmData.wMinute= (buffer[i+pos+1] | ((buffer[i+pos+2] >> 4)<<8))/60;

            m_pMethod->SetSoe(m_wBusNO,m_wAddr,YxNo,soeflag,&ptmData);
           // m_pMethod->SetYxDataWithTime ( m_SerialNo, buffer[i+pos] & 0x3f, soeflag, &ptmData );
           /* printf("ModBusRtu SetYxDataWithTime \n");
            char buf[100] = "";
            sprintf(buf,"SOE m_byLineNo:%d m_wDevAddr%d num:%d val:%d time:%d.%d.%d-%d:%d:%d.%d\n",
                            m_wBusNO, m_wAddr , YxNo , soeflag ,
                            ptmData.wYear,ptmData.wMonth,ptmData.wDay,ptmData.wHour,
                            ptmData.wMinute,ptmData.wSecond,ptmData.wMilliSec);
            printf("--%s\n",buf);
            printf("%d %d tx:",m_wBusNO,m_wAddr);
            int i=0,len=buffer[pos-1]+5;
            for( i=0;i<len;i++ )
            {
                printf(" %02x ", buffer[i]);
            }
            printf(" \n ");
            */// OutBusDebug(m_byLineNo, (BYTE *)buf, strlen(buf),3);
    }
}

//SOE处理
void CModule_ModBusRtu::ModBusSoeDeal(unsigned char *buffer,MODBUSCONF modbusconf)
{//printf("ModBusSoeDeal %d\n",modbusconf.SoeFlag);
        switch( modbusconf.SoeFlag )
        {
                case 0:
                ModBusRtuSoeDeal( buffer , modbusconf );
                break;
                case 1:
                ModBusYZ202SoeDeal( buffer , modbusconf );
                break;
                case 2:
                ModBusEM310SoeDeal( buffer , modbusconf );
                break;
                default:
                MsgErrorFlag = MSGERROR;
                return;
        }
}

//数据长度，数据格式处理
short CModule_ModBusRtu::TwoByteValue(unsigned char *buffer, int a, MODBUSCONF modbusconf)
{
        short val = 0;
        unsigned char buf[2] = "";

        //屏蔽码处
        //unsigned int temp_mask = modbusconf.mask_code;
        //buf[0] = buffer[a] & ((temp_mask<<16)>>24);
        //buf[1] = buffer[a+1] & ((temp_mask<<24)>>24);

        buf[0] = buffer[a];
        buf[1] = buffer[a+1];
        switch(modbusconf.data_form)
        {
                case 0:
                val = (buf[0]<<8 | buf[1]);
                break;
                case 1:
                val = (buf[0] | buf[1]<<8);
                break;
                default:
                MsgErrorFlag = MSGERROR;
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"该数据长度配置为2，数据格式错误配置为%d，不匹配",modbusconf.data_form);
                break;
        }
        return val;
}

//数据长度，数据格式处理
int CModule_ModBusRtu::FourByteValue(unsigned char *buffer, int a, MODBUSCONF modbusconf)
{
        int val = 0;

        unsigned char buf[4] = "";
        //屏蔽码处理
        //unsigned int temp_mask = modbusconf.mask_code;
        //buf[0] = buffer[a] & (temp_mask>>24);
        //buf[1] = buffer[a+1] & ((temp_mask<<8)>>24);
        //buf[2] = buffer[a+2] & ((temp_mask<<16)>>24);
        //buf[3] = buffer[a+3] & ((temp_mask<<24)>>24);

        buf[0] = buffer[a];
        buf[1] = buffer[a+1];
        buf[2] = buffer[a+2];
        buf[3] = buffer[a+3];
        switch(modbusconf.data_form)
        {
                case 2:
                val = (buf[0]<<24 | buf[1]<<16 | buf[2]<<8 | buf[3]);
                break;
                case 3:
                val = (buf[0]<<16 | buf[1]<<24 | buf[2] | buf[3]<<8);
                break;
                case 4:
                val = (buf[0]<<8 | buf[1] | buf[2]<<24 | buf[3]<<16);
                break;
                case 5:
                val = (buf[0] | buf[1]<<8 | buf[2]<<16 | buf[3]<<24);
                break;
                default:
                MsgErrorFlag = MSGERROR;
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"该数据长度配置为4，数据格式错误配置为%d，不匹配",modbusconf.data_form);
                break;
        }
        return val;
}

//有符号为float，数据格式处理
float CModule_ModBusRtu::FloatValue(unsigned char *buffer, int a, MODBUSCONF modbusconf)
{
        float val = 0;
        unsigned char float_buf[4] = "";

        unsigned char buf[4] = "";
        //屏蔽码处理
        unsigned int temp_mask = modbusconf.mask_code;
        buf[0] = buffer[a] & (temp_mask>>24);
        buf[1] = buffer[a+1] & ((temp_mask<<8)>>24);
        buf[2] = buffer[a+2] & ((temp_mask<<16)>>24);
        buf[3] = buffer[a+3] & ((temp_mask<<24)>>24);
        switch(modbusconf.data_form)
        {
                case 2:
                float_buf[3] = buf[0];
                float_buf[2] = buf[1];
                float_buf[1] = buf[2];
                float_buf[0] = buf[3];
                break;
                case 3:
                float_buf[2] = buf[0];
                float_buf[3] = buf[1];
                float_buf[0] = buf[2];
                float_buf[1] = buf[3];
                break;
                case 4:
                float_buf[1] = buf[0];
                float_buf[0] = buf[1];
                float_buf[3] = buf[2];
                float_buf[2] = buf[3];
                break;
                case 5:
                float_buf[0] = buf[0];
                float_buf[1] = buf[1];
                float_buf[2] = buf[2];
                float_buf[3] = buf[3];
                break;
                default:
                MsgErrorFlag = MSGERROR;
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"该数据长度配置为4，数据格式错误配置为%d，不匹配",modbusconf.data_form);
                break;
        }
        val = *(float *)float_buf;
        return val;
}

//数据格式、有符号
float CModule_ModBusRtu::ModBusValue(unsigned char *buffer, int a, MODBUSCONF modbusconf)
{
        float value_f = 0;
        int value_i = 0;
        unsigned int value_ui = 0;
        short value_s = 0;
        unsigned short value_us = 0;
        switch(modbusconf.sign)
        {
                //无符号整形处理
                case 0:
                if(modbusconf.data_len == 2)
                {
                        value_us = (unsigned short)TwoByteValue(buffer,a,modbusconf);
                        return (float)value_us;
                }
                else if(modbusconf.data_len == 4)
                {
                        value_ui = (unsigned int)FourByteValue(buffer,a,modbusconf);
                        return (float)value_ui;
                }
                break;
                //有符号整形处理：首位为符号位，1 为负数，0 为正数。其余为绝对值
                case 1:
                if(modbusconf.data_len == 2)
                {
                        value_s = TwoByteValue(buffer,a,modbusconf);
                        if( value_s < 0 )
                                value_s = ( (~value_s) | 0x8000 ) + 1;
                        return (float)value_s;
                }
                else if(modbusconf.data_len == 4)
                {
                        value_i = FourByteValue(buffer,a,modbusconf);
                        if( value_i < 0 )
                                value_i = ( (~value_i) | 0x80000000 ) + 1;
                        return (float)value_i;
                }
                break;
                //有符号整形处理：补码
                case 2:
                if(modbusconf.data_len == 2)
                {
                        value_s = TwoByteValue(buffer,a,modbusconf);
                        return (float)value_s;
                }
                else if(modbusconf.data_len == 4)
                {
                        value_i = FourByteValue(buffer,a,modbusconf);
                        return (float)value_i;
                }
                break;
                //float浮点型处理
                case 3:
                value_f = FloatValue(buffer,a,modbusconf);
                return value_f;
                break;
                default:
                MsgErrorFlag = MSGERROR;
                memset(ErrorInformation,0,sizeof(ErrorInformation));
                sprintf(ErrorInformation,"有符号位错误配置为%d，没有该类型",modbusconf.sign);
                break;
        }
        return value_f;
}

//遥控信号处理
BOOL CModule_ModBusRtu::GetYKBuffer( BYTE * buf , int &len , PYKST_MSG pBusMsg )
{
        int i = 0;

        // SrcwSerialNo = pBusMsg->SrcInfo.wSerialNo;

        //printf("GetYKBuffer pBusMsg->byAction = %d \n ",pBusMsg->byAction);
        if( ( pBusMsg == NULL ) && ( yk_pos_num == 0 ) )
                return FALSE ;
        wSrcBusNo = pBusMsg->wBusNoSrc;
        wSrcDevAddr = pBusMsg->wAddrSrc;
        wSrcPnt = pBusMsg->wPntNoSrc;

        if( pBusMsg->byAction == YK_PREPARE || pBusMsg->byAction == YK_CANCEL )					//遥控选择
        {
                for( i=0 ; i<yk_pos_num ; i++ )
                {
                         if( pBusMsg->wPntNoDest == yk_info[i].start_num )
                                //&&( yk_data->wPnt < yk_info[i].start_num + yk_info[i].get_num ) )
                        {
                                if( ( modbus_conf[yk_info[i].pos].yk_form == 2 ) ||
                                        ( modbus_conf[yk_info[i].pos].yk_form == pBusMsg->wVal ) )
                                {
                                        if( pBusMsg->byAction == YK_PREPARE )
                                        {
                                                yk_flag = 0;
                                                switch( modbus_conf[yk_info[i].pos].YkSelFlag )
                                                {
                                                        case 0:
                                                        m_pMethod->SengYK(wSrcBusNo,wSrcDevAddr,wSrcPnt,
                                                                          m_wBusNO,m_wAddr,pBusMsg->wPntNoDest,YK_PREPARE_RTN,pBusMsg->wVal);
                                                        break;
                                                        default:
                                                        return FALSE ;
                                                }
                                        }
                                        else
                                        {
                                                yk_flag = 0;
                                                m_pMethod->SengYK(wSrcBusNo,wSrcDevAddr,wSrcPnt,
                                                                    m_wBusNO,m_wAddr,pBusMsg->wPntNoDest,YK_CANCEL_RTN,pBusMsg->wVal);
                                        }
                                        return TRUE ;
                                }
                        }
                }
                pBusMsg->wVal = YK_ERROR_REGISTER_02;
                yk_flag = 0;
                m_pMethod->SengYK(wSrcBusNo,wSrcDevAddr,wSrcPnt,
                                  m_wBusNO,m_wAddr,pBusMsg->wPntNoDest,YK_PREPARE_RTN,pBusMsg->wVal);
                return TRUE ;
        }
        else if( pBusMsg->byAction == YK_EXEC )			//遥控执行
        {
                //printf( "YK EXEC");
                for( i=0 ; i<yk_pos_num ; i++ )
                {
                         if( pBusMsg->wPntNoDest == yk_info[i].start_num )
                                //&&( yk_data->wPnt < yk_info[i].start_num + yk_info[i].get_num ) )
                        {
                                if( ( modbus_conf[yk_info[i].pos].yk_form == 2 ) ||
                                        ( modbus_conf[yk_info[i].pos].yk_form == pBusMsg->wVal ) )
                                {
                                        switch( modbus_conf[yk_info[i].pos].YkExctFlag )
                                        {
                                                case 0:
                                                YkSendBuf( modbus_conf[yk_info[i].pos] , pBusMsg , buf , &len );
                                                break;
                                                default:
                                                return FALSE;
                                        }

                                        YkNo = pBusMsg->wPntNoDest;
                                        pos_flag = yk_info[i].pos;
                                        return TRUE ;
                                }
                        }

                }
                pBusMsg->wVal = YK_ERROR_REGISTER_02;
                yk_flag = 0;
                if( wSrcBusNo == (WORD)-1 || wSrcDevAddr == (WORD)-1 || wSrcPnt == (WORD)-1 )
                    return TRUE;
                m_pMethod->SengYK(wSrcBusNo,wSrcDevAddr,wSrcPnt,
                                  m_wBusNO,m_wAddr,pBusMsg->wPntNoDest,YK_EXEC_RTN,pBusMsg->wVal);
                return TRUE ;
        }
        else if( pBusMsg->byAction == YK_CANCEL )
                printf( "YK_CANCEL \n" );

        return TRUE ;
}

//发送遥控报文
void CModule_ModBusRtu::YkSendBuf( MODBUSCONF modbusconf , PYKST_MSG pBusMsg , BYTE * buf ,int *len )
{

        BYTE index = 0;
        buf[ index++ ] = m_wAddr;
        buf[ index++ ] = modbusconf.func;
        buf[ index++ ] = modbusconf.registe >> 8;
        buf[ index++ ] = ( modbusconf.registe << 8 ) >> 8;

        if(pBusMsg->wVal == YK_OPEN && modbusconf.yk_form ==2)
        {
                buf[ index++ ] = 0x00;
                buf[ index++ ] = 0x00;
        }
        else
        {
                buf[ index++ ] = 0xff;
                buf[ index++ ] = 0x00;
        }
        WORD wCRC = GetCRC( buf, index );
        buf[ index++ ] = HIBYTE(wCRC);
        buf[ index++ ] = LOBYTE(wCRC);
        *len = index;
}
