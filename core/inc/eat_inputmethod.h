#ifndef _EAT_INPUTMETHOD_H_
#define _EAT_INPUTMETHOD_H_

/***************************************************************************
* Include Files
***************************************************************************/

#include "eat_type.h"

/***************************************************************************
* Macros
***************************************************************************/

/***************************** ime key value define begin******************************/
/* digital : 0~9 */
#define WDK_NORMAL_0		0x10
#define WDK_NORMAL_1		0x11
#define WDK_NORMAL_2		0x12
#define WDK_NORMAL_3		0x13
#define WDK_NORMAL_4		0x14
#define WDK_NORMAL_5		0x15
#define WDK_NORMAL_6		0x16
#define WDK_NORMAL_7		0x17
#define WDK_NORMAL_8		0x18
#define WDK_NORMAL_9		0x19

/* letter : A~Z */

#define WDK_NORMAL_LETTER   0xB0					    
#define WDK_NORMAL_A		(WDK_NORMAL_LETTER+0x01)	//  1
#define WDK_NORMAL_B		(WDK_NORMAL_LETTER+0x02)    //  2
#define WDK_NORMAL_C		(WDK_NORMAL_LETTER+0x03)    //  3
#define WDK_NORMAL_D		(WDK_NORMAL_LETTER+0x04)	//  4
#define WDK_NORMAL_E		(WDK_NORMAL_LETTER+0x05)    //  5
#define WDK_NORMAL_F		(WDK_NORMAL_LETTER+0x06)	//  6
#define WDK_NORMAL_G		(WDK_NORMAL_LETTER+0x07)	//  7
#define WDK_NORMAL_H		(WDK_NORMAL_LETTER+0x08)	//  8
#define WDK_NORMAL_I		(WDK_NORMAL_LETTER+0x09)	//  9
#define WDK_NORMAL_J		(WDK_NORMAL_LETTER+0x0A)	//  10
#define WDK_NORMAL_K		(WDK_NORMAL_LETTER+0x0B)	//  11
#define WDK_NORMAL_L		(WDK_NORMAL_LETTER+0x0C)	//  12
#define WDK_NORMAL_M		(WDK_NORMAL_LETTER+0x0D)	//  13
#define WDK_NORMAL_N		(WDK_NORMAL_LETTER+0x0E)	//  14
#define WDK_NORMAL_O		(WDK_NORMAL_LETTER+0x0F)	//  15
#define WDK_NORMAL_P		(WDK_NORMAL_LETTER+0x10)	//  16
#define WDK_NORMAL_Q		(WDK_NORMAL_LETTER+0x11)	//  17
#define WDK_NORMAL_R		(WDK_NORMAL_LETTER+0x12)	//  18
#define WDK_NORMAL_S		(WDK_NORMAL_LETTER+0x13)	//  19
#define WDK_NORMAL_T		(WDK_NORMAL_LETTER+0x14)	//  20
#define WDK_NORMAL_U		(WDK_NORMAL_LETTER+0x15)	//  21
#define WDK_NORMAL_V		(WDK_NORMAL_LETTER+0x16)	//  22
#define WDK_NORMAL_W		(WDK_NORMAL_LETTER+0x17)	//  23
#define WDK_NORMAL_X		(WDK_NORMAL_LETTER+0x18)	//  24
#define WDK_NORMAL_Y		(WDK_NORMAL_LETTER+0x19)	//  25
#define WDK_NORMAL_Z		(WDK_NORMAL_LETTER+0x1A)    //  26

/* function key */
#define WDK_STAR			0x1A	/* "*" */
#define WDK_SHARP			0x1B	/* "#" */
#define WDK_UP				0x1C	/* up */
#define WDK_DOWN			0x1D	/* down */
#define WDK_LEFT				0x1E	/* left */
#define WDK_RIGHT			0x1F	/* right */
#define WDK_C				0x20	/* back */
#define WDK_OK				0x21	/* ok */
#define WDK_MENU			0x22	/* FN --full-keybroad */
#define WDK_SOFTLEFT		0x23	/* left softkey */
#define WDK_SOFTRIGHT		0x24	/* right softkey */
#define WDK_SHIFT			0x3A	/* Shift */
#define WDK_CAPLOCK		0x3B	/* caplock */
#define WDK_SEPERATER		0x3C	/* seperater */
#define WDK_FUNC			0x3D	/* FN */
#define WDK_ENTER			0x3E	/* enter */
#define WDK_ALT 				0x3F	/* Alt */
#define WDK_CTRL			0x40	/* Ctrl */
#define WDK_SPACE			0x41	/* space */
#define WDK_SYMBOL			0x42	/* symbol */
#define WDK_SWITCHIME       	0x43	/*switch inputmethod type*/
/***************************** ime key value define end******************************/



/***************************************************************************
* Types
***************************************************************************/

typedef enum{
	EAT_IME_DIGIT,                 /*digital*/
	EAT_IME_ENGLISH_ABC,    /*english abc*/
	EAT_IME_CN_FULLPINYIN,  /*chinese full pinyin*/
	EAT_IME_CN_CPINYIN,       /*chinese pure pinyin*/
	EAT_IME_ENGLISH,            /*english*/
	EAT_IME_CN_BIHUA,         /*chinese bihua*/
	EAT_IME_MAX
}EAT_IME_TYPE;

typedef struct _IME_KEY_MAP
{
	u32 uEatValue;	/*EatKey_enum键值*/
	u32 uIMEValue;	/*输入法对应键值*/
}IME_KEY_MAP;


#define MAX_KEYS_LEN				24		/* 输入的键码数的最大值	*/
#define MAX_CHINESE_WORD_LEN		8		/* 中文单词的最大长度 */
#define MAX_KEY_LETTERCOUNT		26		/* 最大的键位上的字母数 */
#define MAX_PINYIN_LENGTH			28		/* 每个拼音的最大长度 */
#define MAX_ONEPAGE_RECORD		20		/* 每页备选的最大词条数 */
#define MAX_DISPLAY_LINE			3       	/* 输入法最大显示行数 */
#define MAX_MESSAGE_LEN			60		/* 提示信息的长度 */

typedef struct WD_imeResult_t
{
	/* original key code */
	/* 原始键码序列 */
	u16 inputKeys[MAX_KEYS_LEN+3];

	/* output key code: all "inputKeys" are converted into output keys' code*/
	/* 输出键码序列：可用于显示已输入的键码或相对应的拼音或注音（中文）
	该键码是已被分隔的（对于中文词输入），和已替换成拼音或注音的(对已确认的音) */
	u16 outputKeys[MAX_KEYS_LEN*MAX_CHINESE_WORD_LEN+1];

	/* current edit word`s focus */
	/* 当前的焦点。
	中文：当前编辑的键码段(第几个汉字)；外语：当前编辑的是第几个字母 */
	u8 idxFocus;

	/* current page of selecting words */
	/*当前的页数(从0开始)，可用于判断是否已经翻到最后一页，是否显示下翻页键头 */	
	u8 curPage;

	/* Chinese inputmethod types: all candidate pinyin
	    English inputmethod types: all candidate characters */
	/* 中文：对于拼音和注音输入法当前焦点字的键码所对应的所有的拼音或注音组合；
		  对于数拼输入法，在第一键时表示所有备选声母；
	    外文：当前焦点键码所对应的所有字母；*/
	u16 letterSelecting[MAX_KEY_LETTERCOUNT][MAX_PINYIN_LENGTH+1];

	/* Current index of "letterSelecting" */
	/* 当前备选音或字母的序号,从0开始 */
	u8 idxLetterSelecting;
	
	/* All candidate words */
	/* 备选项：最多有MAX_ONEPAGE_RECORD项。
	中文：存放已查出的词或字，每个字词一条；
	外文：符合已输入键码的单词，存放Unicode。*/
	u16 selecting[MAX_ONEPAGE_RECORD][MAX_KEYS_LEN+3];	
	
	/* Current index of all candidate words */
	/* 当前备选项序号，,从0开始 */
	u8 idxSelecting;
	
	/* Selected word(s) of this time,it will be clear before next response of input key */
	/* 已确认字词，返回给调用函数 */
	u16 output[MAX_KEYS_LEN*MAX_CHINESE_WORD_LEN+1];
	
	/* The value of return when input key respond failed */
	/* 返回键码：输入法不处理的按键被直接传回，或对于非法键，传出错误报警 */
	u16 returnKey;					   
	
	/* Inputmethod type`s value */
	/* 当前输入法的内码: 按照输入法定义 */
	u8 imeType; 				   
	
	/* Inputmethod type`s name */
	/* 输入法种类提示字符串，中文用GB码或者unicode码，外文用Unicode码。*/
	u16 imeString[4];				   
	
	/* Invalid data at this version */
	/* 提示信息：需显示的提示信息；中文用GB码，外文用Unicode。
	此版本不提供提示信息功能 */
	u16 message[MAX_DISPLAY_LINE][MAX_MESSAGE_LEN];  
	
	/* Invalid data at this version */
	/*此版本不可用；*/
	u8 resultEmpty;
	
	/* Current status of input method */
	/*输入法当前的状态 */
	u8 imeStatus;
	
	/* Previous status of input method */
	/*输入法前一的状态 */
	u8 preStatus;

	/* 0: Lowercase state;
	    1: Capitalize the first letterl;
	    2: Capital state*/
	/*表示大小写状态0为小写状态，1为首字母大写，2为全大写状态 */
	u8 capLock;

	/* Amount of "selecting" */
	/*每次搜索到的结果数*/
	u8 selectingCount;

	/* 0:chinese mix rank,otherwise 1 */
	/*中文是否混排，增加一个标志为指示搜索出来的备选项是否混排，是则为0，否则为1 */
	u8 cnMix;

	/*  */
	/*待上屏的焦点 */
	u8 screenFocus;

	/*  */
	/*变数字时此标识置1 */
	u8 numFlag;

	/*  */
	/*进入DIY状态为1，否则为0 */
	u8 statusDIY;

	/* This value equal 1 when switch to English with "#" key */
	/*按#键无缝切换到英文时为1*/
	u8 CnEnglishFlag;

	/* Previous input method type */
	/*前一输入法*/
	u8 preIme;

	/*  */
	/*确认中文词上屏数量*/
	u8 confirmCnWordNum;	
} WD_imeResult;

typedef enum{
	EAT_IME_SUCCESS,
	EAT_IME_TYPE_ERR,    //输入法类型不存在。Failed type of inputmethod
	EAT_IME_ERR_AMOUNT_BEYOND,    //输入法启动时的个数超过最大值。Overmuch inputmethod types (Overmuch) when start ime	
	EAT_UNKNOWN_ERR,    //库接口调用失败。Interface called failed
	EAT_INIT_ERR		//输入法注册key map失败。Failed to register inputmethod key map.
	
}EAT_IME_ERROR;


/***************************************************************************
* External Functions Prototypes
***************************************************************************/

/*****************************************************************************
* Function :  eat_ime_register_keymap
 * Description
 *    You must config a key map array before calling eat_ime_entry. Call this interface to register it.
 * Parameters
 *   pKeyMapTable[in] : The array of key map.
 *   u8TableCount[in] : Length of pKeyMapTable.
 * Returns
 *         EAT_TRUE: Mean register successful,otherwise return EAT_FALSE.
 * attention
 *     1. This interface must called before interface of eat_ime_entry called at the first time.
*****************************************************************************/
extern eat_bool (* const eat_ime_register_keymap)(IME_KEY_MAP * pKeyMapTable, u8 u8TableCount);

/*****************************************************************************
* Function :  eat_ime_entry
 * Description
 *    Start ime.
 * Parameters
 *   ime_types[in] : Input method type`s array.
 *   ime_types_count[in] : ime_types length.
 *   default_ime_type[in] : The first input method type when inputmethod starts.
 *   displayed_words_perline[in] :  Max number of words displayed when a input key responsed(to response a input key) 
 * Returns
 *         EAT_IME_ERROR
 * attention
 *     1. You must call eat_ime_register_keymap at least one time before this interface used.
*****************************************************************************/
extern s32 (* const eat_ime_entry)(EAT_IME_TYPE ime_types[],u8 ime_types_count,EAT_IME_TYPE default_ime_type,u8 displayed_words_perline);

/*****************************************************************************
* Function :  eat_ime_exit
 * Description
 *    Exit ime.
 * Parameters
 * Returns
*****************************************************************************/

extern void (* const eat_ime_exit)();

#endif	/*_EAT_INPUTMETHOD_H_*/



