#ifndef MODBUSRTU_GLOBAL_H
#define MODBUSRTU_GLOBAL_H

#include <QtCore/qglobal.h>


#if defined(MODBUSRTU_LIBRARY)
#define MODBUSRTUSHARED_EXPORT Q_DECL_EXPORT
#else
#define MODBUSRTUSHARED_EXPORT Q_DECL_IMPORT
#endif

#define ERROR_CONST			5

typedef struct
{
        BYTE type;
        BYTE func;
        WORD registe;
        WORD registe_num;
        BYTE skew_byte;
        WORD get_num;
        WORD start_num;
        BYTE data_len;
        DWORD mask_code;
        BYTE data_form;
        BYTE sign;
        BYTE yk_form;
        BYTE cir_flag;
        WORD YkClose;
        WORD YkOpen;
        BYTE SoeFlag;
        BYTE SetTimeFlag;
        BYTE YkSelFlag;
        BYTE YkExctFlag;
}MODBUSCONF;

typedef struct
{
        BYTE pos;
        WORD start_num;
        WORD get_num;
}INFO;



#endif // MODBUSRTU_GLOBAL_H
