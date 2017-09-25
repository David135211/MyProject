#ifndef _MODBUSTCP_H_
#define _MODBUSTCP_H_

#ifndef UINT
typedef unsigned int UINT;
typedef unsigned long long ULONG;
#endif
#include "CProtocol_ModBusTcp.h"
#include <vector>
#include <time.h>
using namespace std;

class frame{
	public:
		BYTE type;						//0:遥测	1:遥脉	2:遥信	3:soe	4:写寄存器(通过遥控执行命令实现对应功能码6)	指定不同类型的数据是为了更好的适配功能码!
		WORD affair;					//事务类别!		具体范围不明!
		WORD protype;					//协议类型!		具体范围不明!

		BYTE func;						//功能码! 3&4 for yc&ym.	2 for yx.	5&6 for yk. 6功能码通过遥控命令向寄存器写数据，也可用作遥控!	10:for settime! 0x55 for soe(暂未实现!)
		WORD register_addr;				//寄存器地址!	遥控不用!
		WORD register_addr_yk[6];		//从小到大依次为：预置合、预置分、执行、取消、取消合预置、取消分预置!
		WORD num;						//对于遥测遥脉,对时.指寄存器数目!	如果取遥信则指点数!
		BYTE data_len;					//一个数据占用字节数!
		BYTE yktype;					//0:脉冲型	1:自保持!	//没什么用了!
		UINT yk_instruction;			//寄存器指令：0xFF00 & 0x0000!
		WORD preset_value;				//6功能码写入值，通过遥控执行命令完成!	这个应该和yk_instructor合并!
		UINT yx_mask;					//遥信掩码，控制取的位!
		BYTE datatype;					//数据类型!0:无符号整数	1:有符号整数	2:float型!
		BYTE dataform;					//数据格式!表征字节序!
		BYTE soeflag;					//表征具体表对应的SOE!
};

class ModBusTcp_Gather : public CProtocol_ModBusTcp_Gather{
	public:
		ModBusTcp_Gather();
		~ModBusTcp_Gather();
		BOOL GetProtocolBuf(BYTE *, int &, PBUSMSG pBusMsg = NULL);
		void getykframe(BYTE *, int &);
		void getycframe(BYTE *, int &);
		void getymframe(BYTE *, int &);
		void getyxframe(BYTE *, int &);
		void getsynframe(BYTE *, int &);
		void getsoeframe(BYTE *, int &);
		BOOL ProcessProtocolBuf(BYTE *, int);
		BOOL resolveyxframe(BYTE *);
		BOOL resolveymframe(BYTE *, int);
		BOOL resolveycframe(BYTE *, int);
		void resolveykframe(BYTE *);
		void resolvetcpsoeframe(BYTE *, frame);
		void resolveYZ202soeframe(BYTE *, frame);
		void resolveEsl_411soeframe(BYTE *, frame);
		BOOL resolvesoeframe(BYTE *, frame);
		short dealtwobyte(BYTE *);
		int dealfourbyte(BYTE *);
		float dealfloatbyte(BYTE *, int);
		void readdefaultconfig(frame *);
		BYTE readconf(char *);
		void getgathernumber();
		BOOL Init(BYTE byLineNo);
		void TimerProc();
		BOOL GetDevCommState();
	protected:
		vector <frame> vec_conf;
		YK_DATA * yk_data;
		BYTE yktype;
		BYTE byBusNo;
		BYTE wDevNo;
		BYTE wPnt;
		BYTE byVal;
		BYTE linesum;
		BYTE line;
		UINT errortimes;
		UINT time_flag;
		UINT portstatus;
		WORD yc_order;
		WORD ym_order;
		WORD yx_order;

		WORD yx_sum;
		WORD yc_sum;
		WORD ym_sum;
};
#endif
