/// \文件:	IEC103_NANZI.h
/// \概要:	南自103协议头文件
/// \作者:	李恩来，lel1132473561@sina.com
/// \版本:	V1.0
/// \时间:	2017-09-25

#ifndef _IEC103_NANZI_H
#define _IEC103_NANZI_H

#include <vector>
#include "../../share/typedef.h"
#include "../../share/CProtocol.h"
#include "../../share/gDataType.h"
#include "CProtocol_IEC103_NANZI.h"

class CIEC103_NANZI : public CProtocol_IEC103_NANZI
{
	public:
		CIEC103_NANZI();
		~CIEC103_NANZI();
};

#endif

