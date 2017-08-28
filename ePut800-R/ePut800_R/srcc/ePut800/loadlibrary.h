#ifndef LOADLIBRARY_H
#define LOADLIBRARY_H
#include <QVector>

class CProtocol ;
class CMethod ;
class CLoadLibrary
{
public:
    CLoadLibrary();
    ~CLoadLibrary( );
    void * OpenDll( char * pDllPath );
    void ReleaseHandle(  );
    CProtocol * GetProtoObj( char * pDllPath, CMethod * pMethod );
    void AddHandle( void * pHandle );

    QVector< void * > m_dllhandle ;

};

#endif // LOADLIBRARY_H
