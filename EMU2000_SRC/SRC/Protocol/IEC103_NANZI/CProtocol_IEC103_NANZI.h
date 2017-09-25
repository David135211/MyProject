/// \文件:	CProtocol_IEC103_NANZI.h
/// \概要:	南自103协议头文件
/// \作者:	李恩来，lel1132473561@sina.com
/// \版本:	V1.0
/// \时间:	2017-09-25

#ifndef _CPROTOCOL_IEC103_NANZI_H
#define _CPROTOCOL_IEC103_NANZI_H

#include <time.h>
#include <sys/time.h>
#include "../../share/CProtocol.h"
#include "../../share/CMethod.h"

#define IEC103_NANZI_FCB_0			0xdf			/*FCB位0 1101 1111 */
#define IEC103_NANZI_FCB_1			0x20			/*FCB位1 0010 0000 */

#define	IEC103_YX_DATATYPE			1				/* 遥信数据类型 */
#define	IEC103_YC_DATATYPE			2				/* 遥测数据类型 */
#define	IEC103_YM_DATATYPE			3				/* 遥脉数据类型 */
#define	IEC103_YK_DATATYPE			4				/* 遥控数据类型 */
#define	IEC103_DD_DATATYPE			5				/* 定值数据类型 */

// --------------------------------------------------------
/// \概要:	南自103协议类，继承CProtocol 类
// --------------------------------------------------------
class CProtocol_IEC103_NANZI : public CProtocol
{
	public:
		CProtocol_IEC103_NANZI();
		~CProtocol_IEC103_NANZI();
		virtual BOOL GetProtocolBuf(BYTE *buf, int &len, PBUSMSG pBusMsg = NULL);
		virtual BOOL ProcessProtocolBuf(BYTE *buf, int len);
		virtual BOOL Init(BYTE byLineNo);
		//获取校验和
		virtual BYTE GetCs(BYTE *pBuf, int len);
		virtual BOOL BroadCast(BYTE *buf, int &len);
		//判断报文有效性
		virtual BOOL WhetherBufValue(BYTE *buf, int &len, int &pos);
	protected:
		BOOL GetDevData();
		//改变校验位
		BYTE ChangeFcb(BYTE byCtlBit, BOOL &bFCB);
	protected:
		BOOL ProcessFileData(CProfile &profile);
		BOOL CreateModule(int iModule, int iSerialNo, WORD wAddr, char *sName, char *sTemplatePath);
};

#endif
