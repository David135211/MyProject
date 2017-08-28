#include "modbustcp.h"
#include "module_modbustcp.h"

#define  MAPMAX_AI_LEN    6800
#define  MAPMAX_DI_LEN    9999
#define  MAPMAX_PI_LEN    1599
#define  MAPMAX_DO_LEN    512
#define  MAPMAX_AO_LEN    128



char g_lpcszTypeDef[][20] =
{
    "@#$%^&",
    "YC_AMOUNT", //遥测数量
    "YX_AMOUNT", //遥信数量
    "YM_AMOUNT", //遥脉数量
    "YK_AMOUNT", //遥控数量
    "PARAM_AMOUNT", //定值数量
    "YC_DEAD" ,//遥测死区
    "YC_PROPERTY", //遥测属性
    "TIMING" //允许对时
};

CModBusTcp::CModBusTcp( CMethod * pMethod ):CProtocol( pMethod )
{
    m_wAISum=0;
    m_wDISum=0;
    m_wPISum=0;
    m_wDOSum=0;
    m_wAOSum=0;
    m_wSignSum=0;
    m_wDZSum =0;
    if( !m_pMethod )
        throw "m_pMethod in CModBusTcp is empty!";
}
CModBusTcp::~CModBusTcp()
{
    if(m_wAISum > 0)
    {
        if(m_pAIMapTab) delete []m_pAIMapTab;
        //if(m_pwAITrans) delete []m_pwAITrans;
    }
    if(m_wDISum > 0)
    {
        if(m_pDIMapTab) delete []m_pDIMapTab;
        //if(m_pwDITrans) delete []m_pwDITrans;
    }
    if(m_wPISum > 0)
    {
        if(m_pPIMapTab) delete []m_pPIMapTab;
        //if(m_pwPITrans) delete []m_pwPITrans;
    }
    if(m_wDOSum > 0)
    {
        if(m_pDOMapTab) delete []m_pDOMapTab;
        //if(m_pwDOTrans) delete []m_pwDOTrans;
    }
    if(m_wAOSum > 0)
    {
        if(m_pAOMapTab) delete []m_pAOMapTab;
        //if(m_pwAOTrans) delete []m_pwAOTrans;
    }
    if(m_pwCITrans != NULL)
    {
        //if(m_pwCITrans) delete []m_pwCITrans;
    }

    if( m_wDZSum > 0 )
    {
        //if( m_pwDZTrans )
        //        delete [ ] m_pwDZTrans ;

        if( m_pDZMapTab )
                delete [ ] m_pDZMapTab ;
    }

}

BOOL CModBusTcp::Init( )
{
    return FALSE ;
}

BOOL CModBusTcp::AddModule(WORD wBusNo , WORD wAddr , WORD wModule, char * stemplate)
{
    if( !m_pMethod )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    CProtocol * pProto = NULL ;
    switch( wModule )
    {
    case PROTO_MODULE_MODBUSTCP:
        pProto = new CModule_ModBusTcp( m_pMethod , wBusNo , wAddr ) ;
        break;
    default :
        return FALSE ;
    }
    strcpy(pProto->m_sTemplatePath,stemplate);
    //Init Module data
    if( !pProto  || !pProto->Init() )
        return FALSE ;

    m_ProtoModule.push_back( pProto );
    return TRUE ;
}

void CModBusTcp::ReadMapConfig(char* lpszFile)
{
    FILE*   fd;
    char    strLine[256];
    int     nType;
    WORD wNum, wVal1, wVal2, wVal3, wVal4, wVal5;

    fd = fopen(lpszFile, "r");
    if(fd == NULL ) {printf("\n Open file %s failure!\n", lpszFile); return;}
    while( fgets(strLine, sizeof(strLine), fd) )
    {
        //ltrim(strLine);
        if( strLine[0]==';' || strLine[0]=='#' ) continue;
        nType = ParseMapLine(strLine, &wNum, &wVal1, &wVal2, &wVal3, &wVal4, &wVal5);
        switch(nType)
        {
        case YC_NUM:
            if(m_wAISum>0) break;
            m_wAISum = wVal1;
            if(m_wAISum > MAPMAX_AI_LEN) m_wAISum = MAPMAX_AI_LEN;
            if(m_wAISum==0) break;
            m_pAIMapTab = new MAPITEM[m_wAISum];
            memset((void*)m_pAIMapTab, 0, sizeof(MAPITEM)*m_wAISum );
            break;
        case YX_NUM:
            if(m_wDISum>0) break;
            m_wDISum = wVal1;
            if(m_wDISum > MAPMAX_DI_LEN) m_wDISum = MAPMAX_DI_LEN;
            if(m_wDISum==0) break;
            m_pDIMapTab = new MAPITEM[m_wDISum];
            memset((void*)m_pDIMapTab, 0, sizeof(MAPITEM)*m_wDISum );
                        break;
        case YM_NUM:
            if(m_wPISum>0) break;
            m_wPISum = wVal1;
            if(m_wPISum > MAPMAX_PI_LEN) m_wPISum = MAPMAX_PI_LEN;
            if(m_wPISum==0) break;
            m_pPIMapTab = new MAPITEM[m_wPISum];
            memset((void*)m_pPIMapTab, 0, sizeof(MAPITEM)*m_wPISum );
            break;
        case YK_NUM:
            if(m_wDOSum>0) break;
            m_wDOSum = wVal1;
            if(m_wDOSum > MAPMAX_DO_LEN) m_wDOSum = MAPMAX_DO_LEN;
            if(m_wDOSum==0) break;
            m_pDOMapTab = new MAPITEM[m_wDOSum];
            memset((void*)m_pDOMapTab, 0, sizeof(MAPITEM)*m_wDOSum );
            break;
        case DZ_NUM:
            if(m_wDZSum>0) break;
            m_wDZSum = wVal1;
            if(m_wDZSum > MAPMAX_AO_LEN) m_wDZSum = MAPMAX_AO_LEN;
            if(m_wDZSum==0) break;
            m_pDZMapTab = new MAPITEM[m_wDZSum];
            memset((void*)m_pDZMapTab, 0, sizeof(MAPITEM)*m_wDZSum );
            break;
        case YC_DEAD: //遥测死区
            m_wDeadVal = wVal1 ;
            break;
        case YC_PROPTY://遥测属性
            m_wRipeVal = wVal1 ;
            break;
        case TIMING://对时允许
            m_wRecvClock = wVal1 ;
            break;
        case YC_PROPERTY :
            if((wNum-1)<m_wAISum)
            {
                m_pAIMapTab[wNum-1].wBusNo   = wVal1;
                m_pAIMapTab[wNum-1].wAddr = wVal2;
                m_pAIMapTab[wNum-1].wPntNum = wVal3;
            }
            break;
        case YX_PROPERTY:
            if((wNum-1)<m_wDISum)
            {
                m_pDIMapTab[wNum-1].wBusNo   = wVal1;
                m_pDIMapTab[wNum-1].wAddr   = wVal2;
                m_pDIMapTab[wNum-1].wPntNum = wVal3;
            }
            break;
        case YM_PROPERTY:
            if((wNum-1)<m_wPISum)
            {
                m_pPIMapTab[wNum-1].wBusNo  = wVal1;
                m_pPIMapTab[wNum-1].wAddr   = wVal2;
                m_pPIMapTab[wNum-1].wPntNum = wVal3;
            }
            break;
        case YK_PROPERTY:
            if((wNum-1)<m_wDOSum)
            {
                m_pDOMapTab[wNum-1].wBusNo  = wVal1;
                m_pDOMapTab[wNum-1].wAddr   = wVal2;
                m_pDOMapTab[wNum-1].wPntNum = wVal3;
            }
            break;
                 case DZ_PROPERTY:
            if((wNum-1)<m_wDZSum)
            {
                m_pDZMapTab[wNum-1].wBusNo  = wVal1;
                m_pDZMapTab[wNum-1].wAddr   = wVal2;
                m_pDZMapTab[wNum-1].wPntNum = wVal3;
            }
            break;
        }
    }
    fclose(fd);
}


extern "C" CProtocol * CreateProtocol( CMethod *) ;
CProtocol * CreateProtocol( CMethod * pMethod )
{
    if( !pMethod )
    {
        Q_ASSERT( FALSE ) ;
        return NULL ;
    }

    CProtocol * pProto = NULL ;
    try
    {
         pProto = new CModBusTcp( pMethod ) ;
    }
    catch( char * pErrorMsg )
    {
        perror( pErrorMsg );
        Q_ASSERT( FALSE );
        return NULL ;
    }
    catch( ... )
    {
        printf( "CreateProtocol generate errors\n" );
        Q_ASSERT( FALSE ) ;
        return NULL ;
    }

    return pProto ;
}

int CModBusTcp::GetMapItem(char *strItem, WORD *pwNum)
{
    char  strType[32];
    int   i, nLen, nType=-1;
        int size = sizeof( g_lpcszTypeDef ) / sizeof( g_lpcszTypeDef[ 0 ] ) ;

        for( int i = 0 ; i < size ; i++ )
        {
                if( strstr(strItem, g_lpcszTypeDef[ i ] ) )
                        return i ;
        }

    i = 0;
    nLen = strlen(strItem);
    while( (!isdigit(strItem[i])) && i<32 )
    {
        strType[i] = toupper(strItem[i]);//isalpha()
        if( ++i >= nLen ) break;
    }
    strType[i] = '\0';
    if( i >= nLen ) *pwNum = 0;
    else *pwNum = (WORD)atoi(&strItem[i]);

    if( strcmp(strType, "YC") == 0 ) return YC_PROPERTY ;
    if( strcmp(strType, "YX") == 0 ) return YX_PROPERTY ;
    if( strcmp(strType, "DD") == 0 ) return YM_PROPERTY ;
    if( strcmp(strType, "YK") == 0 ) return YK_PROPERTY ;
    if( strcmp(strType, "DZ") == 0 ) return DZ_PROPERTY ;
    return nType;
}

void CModBusTcp::GetMapParam(char* strParam,
              WORD *pwVal1, WORD *pwVal2, WORD *pwVal3, WORD *pwVal4, WORD *pwVal5)
{
    int i, nLen;
    char *pValue;

    nLen = strlen(strParam);
    if( nLen <= 0 ) return;
    pValue = strtok(strParam, ", ");

    for(i=0; pValue != NULL; i++)
    {
        switch(i)
        {
        case 0:
            if( pwVal1 ) *pwVal1 = (WORD)atoi(pValue);
            break;
        case 1:
            if( pwVal2 ) *pwVal2 = (WORD)atoi(pValue);
            break;
        case 2:
            if( pwVal3 ) *pwVal3 = (WORD)atoi(pValue);
            break;
        case 3:
            if( pwVal4 ) *pwVal4 = (WORD)atoi(pValue);
            break;
        case 4:
            if( pwVal5 ) *pwVal5 = (WORD)atoi(pValue);
            break;
        }
                pValue = strtok(NULL, ", ");
        }
}

/*
返回值=
-1：错误
0：注释行
1：遥测点数
2：遥信点数
3：电量点数
4：遥控路数
5：遥调路数
6：保护信息
10：遥测属性
20：遥信属性
30：电量属性
40：遥控属性
50：遥调属性
60：保护属性
*/
int CModBusTcp::ParseMapLine(char* strLine, WORD *pwNum,
                          WORD *pwVal1, WORD *pwVal2, WORD *pwVal3, WORD *pwVal4, WORD *pwVal5)
{
    char *pItem, *pParam;
    int nType=-1;

    pItem = strtok(strLine, "=,\t ");

        if( pItem == NULL )
                return -1;

        pParam = strtok(NULL, "=");
        if( pParam == NULL )
                return -2;

    nType = GetMapItem(pItem, pwNum);

    GetMapParam(pParam, pwVal1, pwVal2, pwVal3, pwVal4, pwVal5);
    return nType;
}




