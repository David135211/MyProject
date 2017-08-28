#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#define  CONFIG_FILE_MAX_SIZE 64 /*the config file size In KByte unit */


#define  CONFIG_SECTION_STR_END_FLAG		"ENDFLAG"
#define  CONFIG_SECTION_STR_DEFAULT		"DEFAULT"

#define  CONFIG_SECTION_STR_GLOBAL	"GLOBAL"
#define  CONFIG_SECTION_STR_COM		"COM"
#define  CONFIG_SECTION_STR_NET		"NET"


#define  CONFIG_SECTION_GLOBAL	101
#define  CONFIG_SECTION_COM		102
#define  CONFIG_SECTION_NET		103

#define  CONFIG_PORT_NUM_DEFAULT		0xffff /* a big value not within valid port range */

#define  DEFAULT_DST_IP "192.168.1.20"

/* In Cfg file, one line must contain no more than MAX_CHARS_ONE_LINE chars*/
#define  MAX_CHARS_ONE_LINE 1024 
/* in Cfg file, every command, parametres must be less MAX_PARAMS_ONE_CMD */
#define  MAX_PARAMS_ONE_CMD 16
/* in Cfg file, every paramsmust be less MAX_CHARS_ONE_PARAM chars */
#define  MAX_CHARS_ONE_PARAM 64


/*********************************************************************/
/*
 Global Params
*/
typedef struct{
	unsigned int ID;
	unsigned char Name[32];
}CmdItem;


/* functions pre declare */
int ConvertToUpper(unsigned char *pString);
int WMLConfigFile(const char *cfgfile,DEV_CONFIG *pDevConfig);
int WMLReadOneLine(unsigned char *pBuf);
int WMLParseLine(unsigned char *pBuf);
int WMLParseSegment(void);
int CMDIsValid(CmdItem *pCMDTable,unsigned char *pCMD);
int StringIsSame(unsigned char *pCMDSrc,char *pCMDDst);
int WMLDefaultConfigLoad(void);
int WMLIsSpaceKey(unsigned char Key);
int WMLIsASCIICode(unsigned char Key);
int WMLConfigCOM(void );
int WMLConfigGlobal(void );

#endif /* End of _CONFIG_FILE_H_ */
