#include     <stdio.h>      
#include     <stdlib.h>     
#include     <termios.h>    
#include     <unistd.h>     
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      
#include     <pthread.h>      
#include     <string.h>      
#include     <errno.h>     
#include	<signal.h>

#include	"VirtCOM.h"

#include "ConfigFile.h"

void VirtCOM_PrintBuf(unsigned char *Buffer,int Len);
int  VirtCOM_InitDevParamDefault(DEV_CONFIG *OK);
int WML_SetDefaultConfig(void);



extern VIRT_COM_DEVSTATE VirtCOM_DevState;
extern DEV_CONFIG gDevConfig;

#define DEBUG
#undef DEBUG

#ifdef DEBUG
#define DEBUG_CONFIGFILE(fmt,args...) printf(fmt ,##args)
#else
#define DEBUG_CONFIGFILE(fmt,args...)
#endif 

unsigned int AppFileSize; /* Config File Size*/
unsigned char *pAppMemFileCur;/* point to the config file */
unsigned char pAppMemFile[CONFIG_FILE_MAX_SIZE* 1024];
static DEV_CONFIG *pLocalDevConfig;
/* 
   config level				
   bit[31..16] = port type number 0 = global , other number,port type number
   bit[2..0] =	000: setting
001: rxtx
 */
unsigned int AppCfgGrade; 
int LineNum;

/*
   start from 1 to locate the port, 0 means all ports,-1 means invalid
 */
int PortNum =-1;


CmdItem ValidCfgType[]={
	{CONFIG_SECTION_GLOBAL,CONFIG_SECTION_STR_GLOBAL}, /* Global Config */
	{CONFIG_SECTION_COM,CONFIG_SECTION_STR_COM}, /* ATM Config */
	{CONFIG_SECTION_NET,CONFIG_SECTION_STR_NET}, /* ATM Config */

	{0xffffffff,CONFIG_SECTION_STR_END_FLAG} /* END FLAG*/
};

/*
   Valid Global Command Table
 */
CmdItem ValidGlobalCMD[]={
	/* GLOBAL*/
	{0,"DEVICE NAME"}, /* 0 : Char string as Device Name */
	
	{0xffffffff,CONFIG_SECTION_STR_END_FLAG} /* END FLAG*/
};


/*
   Valid COM Command Table
 */
CmdItem ValidCOMCMD[]={
	
	{1,"BAUDRATE"}, /*baudrate :115200,9600,... */
	{2,"DATABIT"}, /*2 : 8/7/6/5 */
	{3,"PARITY"}, /*3: EVEN/ODD/NONE  */
	{4, "STOPBIT"}, /* 4: 1/15/2*/
	{5, "NETPORT"}, /* 5: 4000/5000/... */
	{6, "CONNECT_TYPE"}, /* 6:  TCP/UDP*/
	{7, "WORK_MODE"}, /* 7:  Server/Client*/
	{8, "SERVER_IP"}, /* 8:  Server IP address */
	{9, "INTERFACE"}, /* 9:  Interface , RS232? RS485 ? RS422*/

	{0xffffffff,CONFIG_SECTION_STR_END_FLAG} /* END FLAG*/
};

/*
   Valid COM Command Table
 */
CmdItem ValidNETCMD[]={
	
	{1,"IPADDR"}, /*192.168.1.24 */

	{0xffffffff,CONFIG_SECTION_STR_END_FLAG} /* END FLAG*/
};


unsigned char CmdParams[MAX_PARAMS_ONE_CMD][MAX_CHARS_ONE_PARAM];



unsigned char pBuf[2048];

FILE *pConfigFile;

/* Config device
   read File  cfgfile  and parse it to struct channel_params
returns:
0: OK,SUCCESS
-1: Open failed.
-2: Read File Error
-3: parse segment error
-4: parse command error
-5: invalid command,means can not decide if segment or command or comments
-6: confige file is too big.
 */
int WMLConfigFile_BIN(DEV_CONFIG *pDevConfig)
{
	int Ret = 0, Ret1, RetFlag;
	FILE *pCFGFILE;

	printf("Config file %s.\n",FILE_NAME_CONFIGFILE_BIN);
	/*  read Config file */
	pCFGFILE = fopen(FILE_NAME_CONFIGFILE_BIN, "rb");
//	printf("%s->Open file return %d.\n " ,__FUNCTION__, pCFGFILE);
	if(pCFGFILE == NULL){ //open failed
		printf("Open file %s failed.\n ", FILE_NAME_CONFIGFILE_BIN);
		printf("Create Default Config File.\n");
		WML_SetDefaultConfig();
	}
	else{ //open OK
		printf("Open file %s OK.\n ",FILE_NAME_CONFIGFILE_BIN);

		fseek(pCFGFILE, 0, SEEK_END);
		AppFileSize =  ftell(pCFGFILE);
		if (AppFileSize > sizeof(DEV_CONFIG)) {
			printf("Config File size %d > Limits %d \n",AppFileSize,sizeof(DEV_CONFIG));
			return -6;
		}
		rewind(pCFGFILE);

		fread(pDevConfig, 1, sizeof(DEV_CONFIG), pCFGFILE);
		fclose(pCFGFILE);
	}

	return 0;
}

/*
 */
int WML_GetStatus(VIRT_COM_DEVSTATE *pStatus)
{
	memcpy(pStatus, &VirtCOM_DevState,sizeof(VIRT_COM_DEVSTATE));
	return 0;
}

/*
 */
int WML_SetConfig(DEV_CONFIG *pNewConfig)
{
	int Ret = 0;
	FILE *pCFGFILE;

	printf("Config file %s.\n", FILE_NAME_CONFIGFILE_BIN);
	/*  write Config file */
	pCFGFILE = fopen(FILE_NAME_CONFIGFILE_BIN, "wb");
	if(pCFGFILE == NULL){
		printf("Create file %s failed.\n ",FILE_NAME_CONFIGFILE_BIN);
		return -1;
	}

	rewind(pCFGFILE);

#if 0
	for(Ret=0;Ret<16;Ret++){
		printf("Serial:%d, Parity:%d(%c)\n",Ret+1,
			pNewConfig->COMConfig[Ret].parity,
			pNewConfig->COMConfig[Ret].parity);
	}
#endif

	fwrite(pNewConfig, 1, sizeof(DEV_CONFIG), pCFGFILE);

	fclose(pCFGFILE);

	return 0;
}

/*
 */
int WML_GetConfig(DEV_CONFIG *pNewConfig)
{
	memcpy(pNewConfig, &gDevConfig,sizeof(DEV_CONFIG));
	return 0;
}


/*
 */
int WML_SetDefaultConfig(void)
{
	VirtCOM_InitDevParamDefault(&gDevConfig);
	WML_SetConfig(&gDevConfig);
	return 0;
}

/* Config device
   read File  cfgfile  and parse it to struct channel_params
returns:
0: OK,SUCCESS
-1: Open failed.
-2: Read File Error
-3: parse segment error
-4: parse command error
-5: invalid command,means can not decide if segment or command or comments 
-6: confige file is too big.
 */
int WMLConfigFile(const char *cfgfile,DEV_CONFIG *pDevConfig)
{
	unsigned char LineRead[MAX_CHARS_ONE_LINE];
	int Ret=0,Ret1,RetFlag;
	FILE *pCFGFILE; 


	printf("Config file %s.\n",cfgfile);
	pLocalDevConfig = pDevConfig;
	/*  read Config file */
	pCFGFILE = fopen(cfgfile,"rb");
	if(pCFGFILE == NULL){
		printf("Open file %s failed.\n ",cfgfile);
		return -1;
	}
	AppFileSize= ftell(pCFGFILE);
	fseek(pCFGFILE,0,SEEK_END);
	AppFileSize =  ftell(pCFGFILE) - AppFileSize;
	if (AppFileSize > sizeof(pAppMemFile)) {
		printf("Config File size %d > Limits %d \n",AppFileSize,sizeof(pAppMemFile));
		return -6;
	}
	pAppMemFileCur = pAppMemFile;
	rewind(pCFGFILE);	

	/* temply use LineNum to store the file size */
	for(LineNum=0;LineNum<AppFileSize;LineNum ++){
		if(feof(pCFGFILE)){
			break;
		}
		pAppMemFile[LineNum] = fgetc(pCFGFILE);
		//printf("%d, ",pAppMemFile[LineNum]);
	}
	fclose(pCFGFILE);
	//DEBUG_CONFIGFILE("%s->Line %d\n ",__FUNCTION__,__LINE__);
	printf("%s->Line %d\n ",__FUNCTION__,__LINE__);

	LineNum =0;
	RetFlag =0;
	while(1){
		memset(LineRead,0,sizeof(LineRead));
		memset(CmdParams,0,sizeof(CmdParams));

		/* Read one line out */
		Ret  = WMLReadOneLine(LineRead);
		if(Ret==2){ /* End of File */
			break;
		}else if(Ret == 1){ /* Read Failed */
			printf("Read File Line %d Error.\n",LineNum);
			RetFlag ++;
			return -2;
		}
		DEBUG_CONFIGFILE("Line:%d, Content:%s\n",LineNum,LineRead);

		/* parse the line */
		Ret = WMLParseLine(LineRead);
		if(Ret == 1){ /* failed or comment line or space line or invalid line  */
			continue;
		}else if(Ret == 2){ /* segment line  */
			Ret = WMLParseSegment();
			if(Ret == 1){ /* failed  */
				printf("Line %d Segment Parse  Error.\n",LineNum);
				Ret= -3;
			}
			continue;
		}else if(Ret !=0){ /* 0 means valid command line */
			printf("Line %d is invalid command.\n",LineNum);
			Ret = -5;
			RetFlag ++;
			continue;
		}

		/* convert command to upper format
		   Now the Params have been copy to CmdParams. CmdParams[0] is the content before =
		   CmdParams[1] is the content after =
		*/
		/* do  the  command params */
		switch(AppCfgGrade) {
			case CONFIG_SECTION_GLOBAL: /* Global */
				Ret1=WMLConfigGlobal();
				break;
			case CONFIG_SECTION_COM: /* COM */
				Ret1=WMLConfigCOM();
				break;
			case CONFIG_SECTION_NET: /* Ethernet */
				Ret1=WMLConfigNET();
				break;
			default:
				break;
		}
		if(Ret1 == 1){/* failed */
			printf("Line %d: Error Command.\n",LineNum);
			RetFlag ++;
		}
	}/* end while*/

	return RetFlag;
}




/*
   read one line from pCFGFILE to pBuf
   in Windows,one line end from 0x0D 0x0A
   in Linux,one line end from 0x0D 0x0A
returns:
0: OK,SUCCESS
1: Failed
2: End of File
 */
int WMLReadOneLine(unsigned char *pBuf)
{
	unsigned int i;
	unsigned char *pBufDst,*pBufSrc;
	unsigned char BufTemp[MAX_CHARS_ONE_LINE];

	while(1){
		i=0;
		if((pAppMemFileCur - pAppMemFile)>AppFileSize){
			return 2;
		}
		LineNum ++;
		//while((*pAppMemFile !=0xd)&&(*(pAppMemFile+1)!=0xa)){ /* end of 0x0D 0x0A*/
		/* in windows, end of 0x0D 0x0A, In Linux, end of only 0x0A*/
		//printf("WMLReadOneLine-> Line %d, config file line:%d\n",__LINE__,LineNum);
		while(1){ /* end of 0x0A*/
			if((pAppMemFileCur-pAppMemFile)>AppFileSize){
					break;
			}
			if((*pAppMemFileCur) == 0x0D) pAppMemFileCur++; //to jump the windows 0x0D
			//printf("WMLReadOneLine-> Line %d,0x%02x\n",__LINE__,*pAppMemFileCur);
			if((*pAppMemFileCur) == 0x0a) 
				break; //end of 0x0a

			BufTemp[i] = *pAppMemFileCur;
			i++;
			pAppMemFileCur++;
		}
		//printf("WMLReadOneLine-> Line %d\n",__LINE__);
		BufTemp[i] =0;
		pAppMemFileCur ++; /* cover the last 0xa */
		//printf("WMLReadOneLine-> Line %d,BufTemp:%s\n",__LINE__,BufTemp);
		//printf("WMLReadOneLine-> Line %d\n",__LINE__);
			
		/* Tell the first non-space or Tab key is not '#' */
		pBufSrc = BufTemp;
		while (WMLIsSpaceKey(*pBufSrc)) {
			pBufSrc ++;
		}
		if ((*pBufSrc != '#') &&( *pBufSrc )) {
			break; /* Valid Command Line */
		}			
	}
	
	/* cover the appended space key or Tab key or '#' line */
	pBufSrc --;
	pBufDst = pBuf;
	while (1) {
		pBufSrc ++;
		if (*pBufSrc == 0) {
			break;
		}
		if (!WMLIsSpaceKey(*pBufSrc) && (*pBufSrc != '#')) {
			*pBufDst = *pBufSrc;
			pBufDst++;
			continue;
		}
		if (*pBufSrc == '#') { /* The last all is commment,we do not need them */
			break;
		}
		if (WMLIsSpaceKey(*pBufSrc)) {
			if ((pBufDst - pBuf) && (*(pBufDst -1) != 0x20)) {
				*pBufDst = 0x20;
				pBufDst++;
			}
		}

	}
	*pBufDst = 0;

	/* Convert all the command to UPPER CASE */
	ConvertToUpper(pBuf);

	//printf("WMLReadOneLine-> Line %d,%s\n",LineNum,pBuf);
	return 0;
}


/*
   parse the input pBuf line to CmdParams 
   if it is a segment line,return the string between '[' and ']'

returns:
0: OK,SUCCESS, command line and parse OK
1: not a valid command line.maybe comment or space or others
2: Segment line
3: Invalid command line

 */
int WMLParseLine(unsigned char *pBuf)
{
	int i,j;
	unsigned char *pBufStart;

	i=0;

	/*find the first non-space key*/
	while(WMLIsSpaceKey(*pBuf)) /*space key or TAB key*/ 
		pBuf++;
	if((*pBuf==0)||(*pBuf == '#')) { /* end of line or a comment line */
		return 1;
	}

	if(*pBuf=='['){ /* Segment line */
		pBuf++;
		while(*pBuf != ']'){
			CmdParams[0][i] = * pBuf;
			i++;
			pBuf++;
		}
		CmdParams[0][i] =0;
		return 2;
	}


	/* valid command line */
	j=0;
	pBufStart = pBuf;
	/* find the '=' Flag */
	while ((pBufStart[j] != '=') &&(pBufStart[j])) {
		j++;
	}
	if (pBufStart[j] != '=') {
		printf("Invaid Command Line, not find =, line %d\n",LineNum);
		printf("j:%d,Buf:%s\n",j,pBufStart);
		return 3;
	}

	/* now pBufStart point to a non-space key value */
	for(i=0;i<j;i++){
		CmdParams[0][i] = pBufStart[i];
	}
	CmdParams[0][i] =0;
	i--;
	while (WMLIsSpaceKey(CmdParams[0][i]) && i>=0) {
		CmdParams[0][i] = 0;
		i--;
	}
	if (i<0) {
		printf("Invaid Command before, line %d\n",LineNum);
		return 3;
	}

	
	/* find the '=' Flag */	
	j++; /* loc j = space key  */
	while (WMLIsSpaceKey(pBufStart[j])) {
		j++;
	}
	i =0;
	while (pBufStart[j]) {
		CmdParams[1][i] = pBufStart[j];
		i++;
		j++;
	}	
	CmdParams[1][i] =0;
	i--;
	while (WMLIsSpaceKey(CmdParams[1][i]) && i>=0) {
		CmdParams[1][i] = 0;
		i--;
	}
	return 0;
}


/*
   process the input Segment  line  to relocate the config port type
returns:
1: Failed
0: Success
 */
int WMLParseSegment(void)
{
	int i,j,PointLoc;
	int BDNumber;
	unsigned char *pBuf;
	unsigned char SubSegment1[64],SubSegment2[64];
	CmdItem *pSegItem;
	unsigned int PreCfgGrade,PrePortNum; /* used in ATM Config */

	PreCfgGrade = AppCfgGrade;
	PrePortNum = PortNum;
 	BDNumber = 0;
	pBuf = CmdParams[0];
	/*find the first non-space key*/
	while (WMLIsSpaceKey(pBuf[0]))  
		pBuf++;
	/*------------ GLOBAL SET -------------------*/
	if (StringIsSame(pBuf, CONFIG_SECTION_STR_GLOBAL)) {
		/* Global setting */
		AppCfgGrade = CONFIG_SECTION_GLOBAL;
		PortNum = -1; /* -1 means invalid */
		return 2; /* return global */
	}

	/* find the '.' Location */
	PointLoc =0;
	while(pBuf[PointLoc] && (pBuf[PointLoc] != '.')){
		PointLoc ++;
	}
	if(!PointLoc){
		printf("Not find . in segment line, line %d\n",LineNum);
		return 1; /* wrong segment line */
	}
	/* copy the segment 1 */
	for(i=0;i<PointLoc;i++){
		SubSegment1[i] = pBuf[i];
	}
	SubSegment1[i] = 0;
	while (WMLIsSpaceKey(SubSegment1[i]) && (i>0))  { /* delete the end space key */
		SubSegment1[i] = 0;
		i--;
	}
	if (i == 0) {
		printf("No Valid Key before . in segment line ,line %d\n",LineNum);
		return 1;/* wrong segment line */
	}
	
	/* copy the segment 2 */
	j= PointLoc + 1; /* PointLoc = '.' */
	while (WMLIsSpaceKey(pBuf[j]))  {
		j++;
	}
	i=0;
	while (pBuf[j]) {
		SubSegment2[i++] = pBuf[j++];
	}
	SubSegment2[i] = 0;
	while (WMLIsSpaceKey(SubSegment2[i]))  {
		SubSegment2[i] = 0;
		i--;
	}
	if (i == 0) {
		printf("No Valid Key after . in segment line, line %d\n",LineNum);
		return 1;/* wrong segment line */
	}


	/* Tell the segment after .  */
	if (StringIsSame(SubSegment2, CONFIG_SECTION_STR_DEFAULT)) {
		PortNum = CONFIG_PORT_NUM_DEFAULT;
	}else{ /* specify a special Port number */
		/* confirm all the key is 0-9 */
		i=0;
		while (SubSegment2[i]) {
			if ((SubSegment2[i]<'0' ) ||(SubSegment2[i]>'9' )) {
				printf("Invalid PortNum after . in segment line, line %d\n",LineNum);
				return 1;
			}
			i++;
		}
		i--;
		j = 1; /* j is the times of 10 */
		PortNum = 0;
		for(;i>=0;i--){
			PortNum += j*(SubSegment2[i]-'0');
			j *= 10;
		}
	}
	
	/* Tell the segment before . */
	pSegItem = NULL;
	i=0;
	while (!StringIsSame(ValidCfgType[i].Name, CONFIG_SECTION_STR_END_FLAG)) {
		if (StringIsSame(ValidCfgType[i].Name, SubSegment1)) {
			pSegItem = &ValidCfgType[i];
			AppCfgGrade = pSegItem->ID;
		}
		i++;
	}
	if (!pSegItem) {
		printf("Invalid Type  %s before . in segment line, line %d\n",SubSegment1,LineNum);
		return 1;
	}
	
	
	return 0;
}

/*
   according the CMDTable, tell the pCMD is in Table
returns:
-1: Not in Table
0,1,...: in Table item number
 */
int CMDIsValid(CmdItem *pCMDTable,unsigned char *pCMD)
{
	CmdItem *pCMDItem;
	unsigned int DifFlag =0;

	pCMDItem = pCMDTable;
	while(1){
		if(StringIsSame(pCMDItem->Name,CONFIG_SECTION_STR_END_FLAG)){ /* END FLAG Indicator */
			DifFlag =1;
			return -1;	
		}
		if(StringIsSame(pCMDItem->Name,(char *)pCMD)){ 
			break;		
		}
		pCMDItem ++; /* point to next command item*/
	}
	return pCMDItem->ID;
}

/*
   tell wthether pCMDSrc and pCMDDst is the same string 
returns:
0: difference
1: same
 */
int StringIsSame(unsigned char *pCMDSrc,char *pCMDDst)
{

	while(*pCMDSrc && *pCMDDst){
		if(*pCMDDst != *pCMDSrc){
			return 0;
		}
		pCMDSrc++;
		pCMDDst++;
	}

	if(*pCMDDst || *pCMDSrc)
		return 0;
	return 1;
}

/*
   changed the a-z to A-Z
 */
int ConvertToUpper(unsigned char *pString)
{
	while(*pString){
		if((*pString >='a') &&(*pString <='z')){
			*pString -= 'a' - 'A';
		}
		pString++;
	}
	return 0;
}

/*
   config Global Param
returns:
0: SUCCESS
1: not for current config,failed
2: invalid command
 */
int WMLConfigGlobal(void )
{
	int Ret;

#if 0
	if(AppCfgGrade != CONFIG_SECTION_GLOBAL){ /* faild,current config type not for global */
		return 1;
	}
	Ret = CMDIsValid(ValidGlobalCMD,CmdParams[0]);
	switch(Ret) {
		case 0: /* DEVICE NAME */
			memcpy(pCurCfg->DevName,CmdParams[1],sizeof(pCurCfg->DevName));
			pCurCfg->DevName[sizeof(pCurCfg->DevName) -1 ] =0;
			break;
			
		default:
			printf("Invaid RetValue %d in GLOBAL.\n",Ret);
			return 1;
	}
#endif
	
	return 0;
}



/*
   config Global Param
   Note:
     the config for every VC or CID channel is done while a new segment starts
returns:
0: SUCCESS
1: not for current config,failed
2: invalid command
 */
int WMLConfigCOM(void)
{
	unsigned int Num,i;
	int Ret;
	int Data32;
	
	if (!pLocalDevConfig) {
		printf("Null pLocalDevConfig for COM\n");
		return 1;
	}
	if(AppCfgGrade != CONFIG_SECTION_COM){ /* faild,current config type not for ATM */
		printf("Now CfgGrade %d is not for COM\n",AppCfgGrade);
		return 1;
	}

	
	Ret = CMDIsValid(ValidCOMCMD,CmdParams[0]);
	switch(Ret) {
	case 1: /* BAUDRATE */
		Num = strtoul(CmdParams[1],NULL,10);
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].rate = Num;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].rate = Num;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		
		break;
	case 2: /*2 : DATABIT */
		Num = strtoul(CmdParams[1],NULL,10);
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].databits= Num;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].databits = Num;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		break;
	case 3: /*3 :PARITY */
		if(StringIsSame(CmdParams[1],"ODD")){
			Num='O';
		}else if(StringIsSame(CmdParams[1],"EVEN")){
			Num='E';
		}else if(StringIsSame(CmdParams[1],"NONE")){
			Num='N';
		}else{
			printf("Invalid parity set :%s, line:%d\n",CmdParams[1],LineNum);
			return -1;
		}
		
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].parity= Num;
			}
			DEBUG_CONFIGFILE("%s->Line %d,set parity to default %c\n ",__FUNCTION__,__LINE__,Num);
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].parity = Num;
				DEBUG_CONFIGFILE("%s->Line %d,set parity to %c\n ",__FUNCTION__,__LINE__,Num);
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		break;
	case 4: /* 4:STOPBIT*/
		Num = strtoul(CmdParams[1],NULL,10);

		if((Num != 1)&&
			(Num != 15)&&
			(Num != 2) ){
			printf("Invalid stopbit set :%s, line:%d\n",CmdParams[1],LineNum);
			return -1;
		}
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].stopbit= Num;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].stopbit = Num;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		break;
	case 5: /*5:NETPORT */
		Num = strtoul(CmdParams[1],NULL,10);
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].net_port= Num + i;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].net_port = Num;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		break;
	case 6: /*6:CONNECT_TYPE*/
		
		if(StringIsSame(CmdParams[1],"TCP")){
			Num=1;
		}else if(StringIsSame(CmdParams[1],"UDP")){
			Num=2;
		}else{
			printf("Invalid CONNECT_TYPE set :%s, line:%d\n",CmdParams[1],LineNum);
			return -1;
		}
		
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].socket_type= Num;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].socket_type = Num;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		break;

	case 7: /*7:WORK_MODE*/
		
		if(StringIsSame(CmdParams[1],"SERVER")){
			Num=0;
		}else if(StringIsSame(CmdParams[1],"CLIENT")){
			Num=1;
		}else{
			printf("Invalid WORK_MODE set :%s, line:%d\n",CmdParams[1],LineNum);
			return -1;
		}
		
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].ClientMode= Num;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].ClientMode = Num;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		break;
		
	case 8: /* Server IP: such as ;192.168.1.24 */
		Ret= inet_aton(CmdParams[1],(struct in_addr *)&Data32);
		if(Ret<0){
			printf("Invalid IP Format:%s at line %d\n",CmdParams[1],LineNum);
			return -1;
		}
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].ServerIP =Data32 ;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].ServerIP = Data32 ;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}	
		break;
	
	case 9: /*9:Interface,, RS232? RS485 ? RS422 */
		
		if(StringIsSame(CmdParams[1],"RS232")){
			Num=1;
		}else if(StringIsSame(CmdParams[1],"RS422")){
			Num=2;
		}else{
			Num=0; //0=RS485
			//printf("Invalid WORK_MODE set :%s, line:%d\n",CmdParams[1],LineNum);
			//return -1;
		}
		
		if (PortNum == CONFIG_PORT_NUM_DEFAULT) {
			for(i=0;i<pLocalDevConfig->ValidCOMNum;i++){
				pLocalDevConfig->COMConfig[i].Interface= Num;
			}
		}else if((PortNum <= MAX_SERIAL_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->COMConfig[PortNum-1].Interface= Num;
		}else{
			printf("Invalid Port Number %d\n",PortNum);
			return -1;
		}
		break;
	default:
		return 5;
	}

	return 0;
}


/*
   config NET Param
returns:
0: SUCCESS
1: not for current config,failed
2: invalid command
 */
int WMLConfigNET(void )
{
	int Ret,i;
	int Data32;

	if (!pLocalDevConfig) {
		return 1;
	}
	if(AppCfgGrade != CONFIG_SECTION_NET){ /* faild,current config type not for NET */
		return 1;
	}
	Ret = CMDIsValid(ValidNETCMD,CmdParams[0]);
	switch(Ret) {
		case 1: /* IPADDR: such as ;192.168.1.24 */
			Ret= inet_aton(CmdParams[1],(struct in_addr *)&Data32);
			if(Ret<0){
				printf("Invalid IP Format:%s at line %d\n",CmdParams[1],LineNum);
				return -1;
			}
		
			if((PortNum <= MAX_NET_PORTS_NUM) &&(PortNum >0)){
				pLocalDevConfig->IPConfig[PortNum -1].IPAddr = Data32;
			}else{
				printf("Invalid Port Number %d\n",PortNum);
				return -1;
			}
			break;
			
		default:
			printf("Invaid RetValue %d in NET.\n",Ret);
			return 1;
	}
	
	return 0;
}




/* Tell a key is SpaceKey or not 
return:
1: Is SpaceKey
0: not a SpackeKey
 */
int WMLIsSpaceKey(unsigned char Key)
{
	if ((Key == 0x20) ||( Key == 0x9)) { /* Space key or Tab key */
		return 1;
	}else{
		return 0;
	}
	return 0;
}

/* Tell a key is ASCII code
return:
1: Is 
0: not 
 */
int WMLIsASCIICode(unsigned char Key)
{
	if ((Key >= 30) && ( Key <= 127)) { /*ASCII  key */
		return 1;
	}else{
		return 0;
	}
	return 0;
}



int WMConfigIsOK(DEV_CONFIG *pNewConfig,unsigned char *pError,unsigned int MaxLen)
{
	int i,j;
	unsigned int SrcIP,DstIP;

	printf("%s->Line %d, COM3 Proto=%d\n",__FUNCTION__,__LINE__,
		pNewConfig->COMConfig[2].Interface);

	//check the serial config
	for(i=0;i<pNewConfig->ValidCOMNum -1;i++){
		for(j=i+1;j<pNewConfig->ValidCOMNum;j++){
			if(pNewConfig->COMConfig[j].net_port == 
				pNewConfig->COMConfig[i].net_port){
				 sprintf(pError,"串口%d 与 串口%d 网络端口相同。\n",i+1,j+1);
				 printf("串口%d 与 串口%d 网络端口相同。\n",i+1,j+1);
				 return -1;
			}
			//printf("%s->Line %d, i=%d, j=%d\n",__FUNCTION__,__LINE__,i,j);
		}	
		if(pNewConfig->COMConfig[i].net_port <MIN_PORT_FOR_SERIAL){
			 sprintf(pError,"串口%d 网络端口错误，应该大于%d。\n",i+1,MIN_PORT_FOR_SERIAL);
			 printf("串口%d 网络端口错误，应该大于%d。\n",i+1,MIN_PORT_FOR_SERIAL);
			 return -11;
		}
		
		if(pNewConfig->COMConfig[i].net_port > MAX_PORT_FOR_SERIAL){
			 sprintf(pError,"串口%d 网络端口错误，应该小于%d。\n",i+1,MAX_PORT_FOR_SERIAL);
			 printf("串口%d 网络端口错误，应该小于%d。\n",i+1,MIN_PORT_FOR_SERIAL);
			 return -11;
		}
	}
	
	printf("%s->Line %d, start check net\n",__FUNCTION__,__LINE__);
	//check the net config
	for(i=0;i<pNewConfig->ValidNetNum -1;i++){
		for(j=i+1;j<pNewConfig->ValidNetNum;j++){
			SrcIP = pNewConfig->IPConfig[j].IPAddr & gDevConfig.IPConfig[i].NetMask;
			DstIP = pNewConfig->IPConfig[i].IPAddr & gDevConfig.IPConfig[j].NetMask;
			if( SrcIP == DstIP){
				 sprintf(pError,"网口%d 与 网口%d IP网段地址相同。\n",i+1,j+1);
				 printf("网口%d 与 网口%d IP网段地址相同。\n",i+1,j+1);
				 return -2;
			}
		}	
	}
	return 0;
}
