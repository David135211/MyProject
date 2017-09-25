
#include <stdio.h>
#include "Protocol_UPS.h"

extern "C"
{
	CProtocol * CreateProtocol(  CMethod * pMethod ) ;
}



CProtocol * CreateProtocol(  CMethod * pMethod )
{
	CProtocol * pProtocol = NULL ;
	pProtocol = new CProtocol_UPS ;
	if( pProtocol )
	{
		pProtocol->m_pMethod = pMethod ;
		printf( "UPS DLL  OK.\n " ) ;
	}

	return pProtocol ;
}

