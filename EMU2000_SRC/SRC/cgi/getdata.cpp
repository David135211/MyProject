#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "../share/rdbFun.h"
#include "../librtdb/rdbObj.h"
//#include "../libdbase/dbasedatatype.h"




using namespace std;

typedef struct _CurrentData 
{
	char DataType[10];                          /* 数据类型 */
	WORD wSerialNo;                             /* 序号 */
	WORD wPnt;                                  /* 点号 */
	char value[15];              				/* 值 */
}CURRENTDATA;				/* ----------  end of struct _CurrentData  ---------- */

vector<CURRENTDATA> v_CurrentData;
//extern CRTDBObj *g_pRTDBObj; 
CRTDBObj *g_pRTDBObj_cgi; 


int GetCurrentDataFromShare ( void )
{
	CURRENTDATA tmpData;
	int i, j;
	STNPARAM *pStnUnit;

	for( i=0; i<g_pRTDBObj_cgi->m_wStnSum; i++ )
	{
		WORD wNum = 0;
		pStnUnit = &g_pRTDBObj_cgi->m_pRTDBSpace->RTDBase.StnUnit[i];
		if( NULL == pStnUnit )
		{
			printf ( "cgi GetCurrentDataFromShare NULL = pStnUnit\n" );
			break;
		}

		//遥测数据
		wNum = pStnUnit->wAnalogSum;
		for ( j=0; j<wNum; j++)
		{
			// printf ( "name=%s %d %d\n", pStnUnit->szStnName , pStnUnit->wStnNum, pStnUnit->dwAnalogPos);
			ANALOGITEM *pItem = &g_pRTDBObj_cgi->m_pRTDBSpace->RTDBase.AnalogTable[pStnUnit->dwAnalogPos+j];
			strcpy(tmpData.DataType , "YC");
			tmpData.wSerialNo = pStnUnit->wStnNum;
			tmpData.wPnt = pItem->wPntID;

			
			if( pItem->fRealVal >= 9.99999 * pow( 10, 9 ) || pItem->fRealVal <= -9.99999 * pow( 10, 8 ))
			{
				strcpy(tmpData.value, "9999999999.999" );
			}
			else
			{
				sprintf( tmpData.value, "%.4f", (float)( pItem->fRealVal * pItem->fRatio ) );
				//printf("yc v*x=xv %f*%f=%.3f\n",pItem->fRealVal,pItem->fRatio,(float)( pItem->fRealVal * pItem->fRatio ));
			}
			tmpData.value[15 - 1] = '\0';
			v_CurrentData.push_back( tmpData );
		}

		//遥信数据
		wNum = g_pRTDBObj_cgi->m_pRTDBSpace->RTDBase.StnUnit[i].wDigitalSum;
		for ( j=0; j<wNum; j++)
		{
			DIGITALITEM *pItem = &g_pRTDBObj_cgi->m_pRTDBSpace->RTDBase.DigitalTable[pStnUnit->dwDigitalPos+j];
			strcpy(tmpData.DataType , "YX");
			tmpData.wSerialNo = pStnUnit->wStnNum;
			tmpData.wPnt = pItem->wPntID;
			sprintf( tmpData.value, "%u", pItem->wStatus & 0x03 );

			v_CurrentData.push_back( tmpData );
		}

		//遥脉数据
		wNum = g_pRTDBObj_cgi->m_pRTDBSpace->RTDBase.StnUnit[i].wPulseSum;
		for ( j=0; j<wNum; j++)
		{
			PULSEITEM *pItem = &g_pRTDBObj_cgi->m_pRTDBSpace->RTDBase.PulseTable[pStnUnit->dwPulsePos+j];
			strcpy(tmpData.DataType , "YM");
			tmpData.wSerialNo = pStnUnit->wStnNum;
			tmpData.wPnt = pItem->wPntID;
			double tmpCalVal = ( double )( pItem->dwRawVal * pItem->fRatio );
			if( tmpCalVal >= 9.99999 * pow( 10, 9 ) || tmpCalVal <= -9.99999 * pow( 10, 8 ) )
			{
				strcpy(tmpData.value, "9999999999.999" );
				continue;
			}
			else
			{
				sprintf( tmpData.value, "%.4lf", ( double )( pItem->dwRawVal * pItem->fRatio ));
			}
			
			
			v_CurrentData.push_back( tmpData );
		}
	}
	return v_CurrentData.size(  );
}		/* -----  end of method CDBaseManager_EEM::GetInsertHistoryDataNumFromShare  ----- */

int Open_SHM_DBase_Cgi()
{
	char szText[128];
	//构造数据库管理对象
	if( !g_pRTDBObj_cgi )
		g_pRTDBObj_cgi = new CRTDBObj();
	if( !g_pRTDBObj_cgi ) return -1;
	//连接共享内存数据库
	if( g_pRTDBObj_cgi->OpenRTDBObj_Cgi(szText) < 0 )
	{
		delete g_pRTDBObj_cgi;
		g_pRTDBObj_cgi = NULL;
		return -2;
	}
	printf("%s\n",szText);
	printf("--------------   Open RTDB OK   ---------------\n");
	return 0;
}


int main(int argc, char **argv)
{
	int i = 0;
	g_pRTDBObj_cgi = NULL;
	if( Open_SHM_DBase_Cgi() < 0 )
	{
		printf("cgi open Dbase err!!!");
		return (-1);
	}
	
	v_CurrentData.clear();
	int DataNum = GetCurrentDataFromShare();

	printf("Content-type:text/html\n\n");
	
	for( i=0; i<DataNum ; i++ )
	{
		printf("<tr>");
		printf("<td>%d</td><td>%s</td><td>%d</td><td>%s</td>",
			v_CurrentData[i].wSerialNo,v_CurrentData[i].DataType,
			v_CurrentData[i].wPnt,v_CurrentData[i].value);
		printf("</tr>");
	}

	return 0;
} 
