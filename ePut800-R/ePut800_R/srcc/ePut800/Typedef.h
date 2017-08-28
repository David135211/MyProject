#ifndef TYPEDEF_H
#define TYPEDEF_H
#include <qglobal.h>
#include <QVector>
#include <QMap>
#include "stddef.h"
#include "math.h"

//定义数据类型
typedef unsigned char BYTE ;
typedef unsigned short int WORD ;
typedef unsigned long DWORD ;
typedef unsigned int   UINT;
typedef bool          BOOL ;
typedef int           HANDLE ;
typedef void *        LPVOID ;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#define UINT8         BYTE
#define UINT16        WORD
#define FALSE         false
#define TRUE          true


class CElement ;
typedef struct sSYSPAGE *PST_SYSPAGE ;
class CPage ;
typedef struct tagTHREAD_STRUCT * PSTHREAD_PROC ;
typedef struct tagDataRoot  *PDataType ;
typedef struct tagDevst *PDEVST ;
typedef struct _BUS_LINE   *PBUS_LINEST ;
typedef struct _CABINET  *PCABINET ;
typedef struct _SINGALCIRCUIT   *PSINGALCIRCUIT ;
typedef struct _ELE_FST *PELE_FST ; //read pic file structure
typedef struct _PAGE_ST *PPAGE_FST ; //read system picture file structure
typedef struct _Pic_Attribute_Data_ST *PPIC_ATTRIBUTE_DATA_FST;
typedef struct _Pic_Attribute_Data_Element *PPIC_ATTRIBUTE_DATA_ELEMENT;
typedef struct _Pic_Attribute_ST *PPIC_ATTRIBUT_FST;
typedef struct tagPicNode *PPICNODE;

typedef QVector< CElement * > QELEMENT_ARRAY ;  //save all kinds of pic elements in one page
typedef QMap< WORD , CElement * > QELEMENT_PICOBJ_MAP; //save all kinds of pic elements in all pages ; key:objectID value:element
//typedef QELEMENT_PICOBJ_MAP::iterator QELEMENT_PICOBJ_MAP_ITOR ;
typedef QMap< WORD , PST_SYSPAGE > QSYS_PAGE_ARRAY ; //save all pages key:page index value:page pointer
typedef QMap< BYTE , CPage * > QPAGE_ARRAY;
typedef void *(*THREAD_PROC) ( void* arg );
typedef QMap< DWORD , PSTHREAD_PROC > QTHREAD_ARRAY ;
typedef QVector< PDataType >QDEV_DATA_ARRAY ;

typedef QMap< WORD , PDEVST >QDEV_ARRAY ;
typedef QDEV_ARRAY::iterator QDEV_ARRAY_ITOR ;

typedef QMap< WORD , PBUS_LINEST >QBUS_ARRAY ;
typedef QBUS_ARRAY::iterator QBUS_ARRAY_ITOR ;

typedef QVector< PCABINET > QCAB_ARRAY ;
typedef QVector< PSINGALCIRCUIT >QSCIRCUIT_ARRAY ;

typedef QMap< WORD , PSINGALCIRCUIT >QSCIRCUIT_MAP ;
/*lel*/
//typedef QMap<WORD, new QPixmap>QPIXMAP_MAP ;
/*end*/
typedef QSCIRCUIT_MAP::iterator QSCIRCUIT_MAP_ITOR ;

typedef QVector< PELE_FST > QELEMENT_FILEST_ARRAY ;
typedef QVector< PPAGE_FST > QPAGE_FILEST_ARRAY ;

typedef QVector< PPIC_ATTRIBUTE_DATA_FST > QPIC_ATTRIBUTE_DATA_FST ;
typedef QVector< PPIC_ATTRIBUTE_DATA_ELEMENT > QPIC_ATTRIBUTE_DATA_ELEMENT ;
typedef QMap< WORD , PPIC_ATTRIBUT_FST > QPIC_ATTRIBUTE_FST_MAP ;
typedef QVector< PPIC_ATTRIBUT_FST > QPIC_ATTRIBUTE_FST_ARRAY ;
typedef QMap< WORD , PPICNODE > QPIC_NODE_TO_ID_MAP ;
typedef QVector< PPICNODE > QPIC_NODE_TO_ID_ARRAY ;
#endif // TYPEDEF_H
