#include "loadlibrary.h"
#include "./Proto/cprotocol.h"
#include "./Proto/cmethod.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>

CLoadLibrary::CLoadLibrary()
{
}

CLoadLibrary::~CLoadLibrary( )
{
    ReleaseHandle( ) ;
}

void * CLoadLibrary::OpenDll( char * pDllPath )
{
        void *handle=NULL;
        char *error;
        if( pDllPath == NULL )
                return NULL ;


        handle = dlopen ( pDllPath, RTLD_LAZY | RTLD_GLOBAL);

        if ((error = dlerror()) != NULL)
        {
            //printf ( "%s\n", error);
            perror( error ) ;
            return NULL;
        }

        return handle ;
}

void CLoadLibrary::ReleaseHandle(  )
{
        int size = m_dllhandle.size() ;
        for( int i = 0 ; i < size ; i++ )
        {
                void * handle = m_dllhandle[ i ] ;
                if( handle )
                        dlclose(handle);
        }

        printf( "Close All Dll \n " );
        m_dllhandle.clear() ;
}


CProtocol * CLoadLibrary::GetProtoObj( char * pDllPath, CMethod * pMethod )
{
        void *handle=NULL;
        char *error;

        handle = OpenDll( pDllPath ) ;
        if( handle == NULL )
        {
            if ((error = dlerror()) != NULL)
                printf ( "%s\n", error);
            perror("Open DLL");
            printf( "Open Dll = %s Failed ! \n " , pDllPath );
                return NULL ;
        }
        else
                printf( "Open Dll = %s Success ! \n " , pDllPath) ;

        typedef CProtocol * ( * ICreate )( CMethod * ) ;

        ICreate pObj = NULL ;
        pObj = ( ICreate )dlsym(handle, "CreateProtocol");
        if( pObj == NULL )
           printf( "Open Dll Failed ! \n " );

    if ((error = dlerror()) != NULL)
    {
        printf ( "%s\n", error);
        perror( error ) ;
        dlclose( handle ) ;
        return NULL;
    }

    CProtocol * pProtocol = NULL ;
    pProtocol = pObj( pMethod ) ;
    if( pProtocol == NULL )
    {
            printf ( "Get ProtoObj Failed !  \n " );
            dlclose( handle ) ;
            return NULL;
    }

    AddHandle( handle ) ;	//dlclose(handle);

    return pProtocol ;
}

void CLoadLibrary::AddHandle( void * pHandle )
{
        if( pHandle == NULL )
                return ;

        m_dllhandle.push_back( pHandle ) ;
}
