/// \文件:	CProtocol_IEC103_NANZI.cpp
/// \概要:	南自103协议
/// \作者:	李恩来，lel1132473561@sina.com
/// \版本:	V1.0
/// \时间:	2017-09-25

#include "CProtocol_IEC103_NANZI.h"
#include "IEC103_NANZI.h"

#define MODULE_RTU 1

extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);

// --------------------------------------------------------
/// \概要:	构造函数
// --------------------------------------------------------
CProtocol_IEC103_NANZI::CProtocol_IEC103_NANZI()
{
	memset(m_sTemplatePath, 0, sizeof(m_sTemplatePath));
}

// --------------------------------------------------------
/// \概要:	析构函数
// --------------------------------------------------------
CProtocol_IEC103_NANZI::~CProtocol_IEC103_NANZI()
{
	int size = m_module.size();
	for(int i = 0; i < size; i++)
		delete m_module[i];
	m_module.clear();
	printf("Delete All CProtocol_IEC103_NANZI OK.\n");
}

// --------------------------------------------------------
/// \概要:	得到协议报文
///
/// \参数:	buf
/// \参数:	len
/// \参数:	pBusMsg
///
/// \返回:	FALSE
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::GetProtocolBuf(BYTE *buf, int &len, PBUSMSG pBusMsg)
{
	return FALSE;
}

// --------------------------------------------------------
/// \概要:	处理协议报文
///
/// \参数:	buf
/// \参数:	len
///
/// \返回:	FALSE
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::ProcessProtocolBuf(BYTE *buf, int len)
{
	return FALSE;
}

// --------------------------------------------------------
/// \概要:	初始化
///
/// \参数:	byLineNo
///
/// \返回:	BOOL
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::Init(BYTE byLineNo)
{
	/*增加ModBus 采集模块数据*/
	/*通过总线号获取读取的装置文件路径*/
	m_byLineNo = byLineNo;
	/*读取模板文件*/
	m_ProtoType = PROTOCO_GATHER;

	return GetDevData();
}

// --------------------------------------------------------
/// \概要:	打开配置文件(.ini)
///
/// \返回:	BOOL
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::GetDevData()
{
	memset(m_sDevPath, 0, sizeof(m_sDevPath));
	sprintf(m_sDevPath, "%s/IEC103_NANZI/%s/%02d.ini", SYSDATAPATH, DEVNAME, m_byLineNo + 1);
	CProfile profile(m_sDevPath);

	return ProcessFileData(profile);
}

// --------------------------------------------------------
/// \概要:	处理文件数据
///
/// \参数:	profile
///
/// \返回:	BOOL
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::ProcessFileData(CProfile &profile)
{
	BOOL bRtn = FALSE;
	if(!profile.IsValid()){
		printf("Open file %s Failed!\n", profile.m_szFileName);
		return FALSE;
	}

	char sDevnum[50]   = "DEVNAME";
	char sDev[50]      = "DEV";
	char sKey[20][100] = {
		"module", "serialno", "addr", "name", "template"
	};
	int iNum      = 0;
	WORD wModule  = 0;
	int iSerialNo = 0;
	WORD iAddr    = 0;
	char sName[50];
	char sStemplate[50];

	memset(sName, 0, sizeof(sName));
	memset(sStemplate, 0, sizeof(sStemplate));

	if((iNum = profile.GetProfileInt(sDevnum, (char *)"NUM", 0)) == 0){
		printf("Get DEVNUM Failed!!!\n");
		return FALSE;
	}

	for(int i = 0; i < iNum; i++){
		sprintf(sDev, "%03d", i + 1);

		wModule = profile.GetProfileInt(sDev, sKey[0], 0);
		iSerialNo = profile.GetProfileInt(sDev, sKey[1], 0);
		iAddr = profile.GetProfileInt(sDev, sKey[2], 0);
		profile.GetProfileString(sDev, sKey[3], (char *)"NULL", sName, sizeof(sName));
		profile.GetProfileString(sDev, sKey[4], (char *)"NULL", sStemplate, sizeof(sStemplate));

		/*创建相应模块子类*/
		bRtn = CreateModule(wModule, iSerialNo, iAddr, sName, sStemplate);
		if(!bRtn){
			printf("Create ModBus module = %d serialno = %d addr = %d name = %s stemplate = %s Error!!!\n", wModule, iSerialNo, iAddr, sName, sStemplate);
			return FALSE;
		}
	}

	return TRUE;
}

// --------------------------------------------------------
/// \概要:	创建模块
///
/// \参数:	iModule
/// \参数:	iSerialNo
/// \参数:	wAddr
/// \参数:	sName
/// \参数:	sTemplatePath
///
/// \返回:	BOOL
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::CreateModule(int iModule, int iSerialNo, WORD wAddr, char *sName, char *sTemplatePath)
{
#if 0
	CProtocol_IEC103_NANZI *pProtocol = NULL;
	switch(iModule){
		case MODULE_RTU:
			pProtocol = new CIEC103_NANZI;
			pProtocol->m_byLineNo = m_byLineNo;
			pProtocol->m_wModuleType = iModule;
			pProtocol->m_wDevAddr = wAddr;
			pProtocol->m_SerialNo = iSerialNo;
			strcpy(pProtocol->m_sTemplatePath, sTemplatePath);
			m_pMethod->m_pRtuObj = pProtocol;
			pProtocol->m_pMethod = m_pMethod;
			pProtocol->m_ProtoType = PROTOCO_GATHER;
			/*初始化模块数据*/
			if(!pProtocol->Init(m_byLineNo))
				return FALSE;
			printf("Add bus = %d addr = %d serialno = %d\n", m_byLineNo, wAddr, iSerialNo);
			break;
		default:
			printf("ModBus don't contain this module Failed.\n");
			return FALSE;
	}
	m_module.push_back(pProtocol);
#endif
	return TRUE;
}

// --------------------------------------------------------
/// \概要:	获得校验和
///
/// \参数:	pBuf
/// \参数:	len
///
/// \返回:	BYTE 校验和
// --------------------------------------------------------
BYTE CProtocol_IEC103_NANZI::GetCs(BYTE *pBuf, int len)
{
	BYTE byRtn = 0x00;
	if(pBuf == NULL || len <= 0)
		return byRtn;

	for(int i = 0; i < len; i++)
		byRtn += pBuf[i];

	return byRtn;
}

// --------------------------------------------------------
/// \概要:	广播报文
///
/// \参数:	buf
/// \参数:	len
///
/// \返回:	BOOL
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::BroadCast(BYTE *buf, int &len)
{
	/*组织该模块的广播报文*/
	int index = 0;
	buf[index++] = 0xFF;
	buf[index++] = 0x02;
	buf[index++] = 0x03;
	buf[index++] = 0x04;

	WORD wCRC = GetCs(buf, index);
	buf[index++] = HIBYTE(wCRC);
	buf[index++] = LOBYTE(wCRC);

	len = index;
	printf("\n CProtocol_IEC103_NANZI TestBroadCast\n");

	return TRUE;
}

// --------------------------------------------------------
/// \概要:	判断报文是否有效
///
/// \参数:	buf
/// \参数:	len
/// \参数:	pos
///
/// \返回:	BOOL
// --------------------------------------------------------
BOOL CProtocol_IEC103_NANZI::WhetherBufValue(BYTE *buf, int &len, int &pos)
{
	BYTE *pointer = buf;
	int datalen   = 0;
	pos           = 0;
	BYTE byCrc;
	char PrintBuf[256];

	if(buf == NULL || len <= 0)
		return FALSE;

	while(len > 0){
		switch(*pointer){
			case 0xE5:				//ABB 61850-103 单字节错误
				len--, pointer++, pos++;

				break;
			case 0x68:				//判断可变帧
				/*判断68 len len 68 合理性*/
				if(((*pointer + 3) != *pointer) || (*(pointer + 1) != *(pointer + 2))){
					len--, pointer++, pos++;
					continue;
				}

				/*判断数据长度合理性*/
				datalen = *(pointer + 1);
				if(datalen + 6 != len){
					len--, pointer++, pos++;
					sprintf(PrintBuf, "IEC103_NANZI variable frame recv len error datalen = %d len = %d\n", datalen, len);
					OutBusDebug(m_byLineNo, (BYTE *)PrintBuf, strlen(PrintBuf), 2);
					continue;
				}

				/*判断校验位 及最后一个字节0x16*/
				byCrc = GetCs(pointer + 4, datalen);
				if((*(pointer + datalen + 4) != byCrc) || (*(pointer + datalen + 5) != 0x16)){
					len--, pointer++, pos++;
					sprintf(PrintBuf, "IEC103_NANZI variable frame recv cs error GetCrc = %d crc = %d last byte = %x\n", byCrc, *(pointer + datalen + 4), *(pointer + datalen + 5));
					OutBusDebug(m_byLineNo, (BYTE *)PrintBuf, strlen(PrintBuf), 2);
					continue;
				}

				/*判断地址*/
				if((*(pointer + 5) != m_wDevAddr) && (*(pointer + 5) != 0xff)){
					len--, pointer++, pos++;
					sprintf(PrintBuf, "CIEC103_NANZI variable frame recv addr error GetAddr = %d, addr = %d\n", *(pointer + 5), m_wDevAddr);
					OutBusDebug(m_byLineNo, (BYTE *)PrintBuf, strlen(PrintBuf), 2);
					continue;
				}

				/*获取正确的报文长度*/
				len = datalen + 6;
				buf = buf + pos;
				return TRUE;

				break;
			case 0x10:				//判断固定帧
				/*判断校验位 及最后一个字节0x16*/
				byCrc = GetCs(pointer + 1, 2);
				if((*(pointer + 3) != byCrc) || (*(pointer + 4) != 0x16)){
					len--, pointer++, pos++;
					sprintf(PrintBuf, "IEC103_NANZI fiexd frame recv cs error GetCrc = %d crc = %d last byte = %x\n", byCrc, *(pointer + 3), *(pointer + 4));
					OutBusDebug(m_byLineNo, (BYTE *)PrintBuf, strlen(PrintBuf), 2);
					continue;
				}

				/*判断地址*/
				if(*(pointer + 2) != m_wDevAddr){
					len--, pointer++, pos++;
					sprintf(PrintBuf, "CIEC103_NANZI fiexd frame recv addr error GetAddr = %d, addr = %d\n", *(pointer + 2), m_wDevAddr);
					OutBusDebug(m_byLineNo, (BYTE *)PrintBuf, strlen(PrintBuf), 2);
					continue;
				}
				buf += pos;
				return TRUE;

				break;
			default:
				len--, pointer++, pos++;
				continue;

				break;
		}
	}

	return FALSE;
}

// --------------------------------------------------------
/// \概要:	改变FCB
///
/// \参数:	byCtlBit
/// \参数:	bFCB
///
/// \返回:	BYTE 改变后的报文数据
// --------------------------------------------------------
BYTE CProtocol_IEC103_NANZI::ChangeFcb(BYTE byCtlBit, BOOL &bFCB)
{
	if(bFCB)
		byCtlBit &= IEC103_NANZI_FCB_0;
	else
		byCtlBit |= IEC103_NANZI_FCB_1;

	bFCB ^= 1;

	return byCtlBit;
}


