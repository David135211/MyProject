
#include "Protocol_ModBusSlave.h"

extern "C"
{
	CProtocol * CreateProtocol(  CMethod * pMethod ) ;
}

CProtocol * CreateProtocol(  CMethod * pMethod )
{
	CProtocol * pProtocol = NULL ;
	pProtocol = new CProtocol_ModBusSlave ;
	if( pProtocol )
	{
		pProtocol->m_pMethod = pMethod ;
		printf( "ModBusSlave DLL  OK.\n " ) ;
	}
	
	return pProtocol ;
}

