/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                               *
 *---------------------------------------------------------------------
 * FileName      :   eat_wmmp.h
 * version       :   0.10
 * Description   :   eat wmmp interface define for customer
 * Authors       :   Maobin  
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2013-01-11, maobin, create originally.
 *
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#ifndef _EAT_SMS_H_
#define _EAT_SMS_H_
#include "eat_type.h"
/********************************************************************
 * Macros
 ********************************************************************/
 #define EAT_SMS_MAX_LEN                 170  //packed message conten length
 #define EAT_SMS_NAME_LEN                43   //receive phone name length
 #define EAT_SMS_DATA_TIME_LEN           25   //receive message date and time length
 #define EAT_SMS_DATA_LEN                350  //receive message string length
 #define EAT_SMS_NUMBER_LEN              43  //receive phone number length

/********************************************************************
 * Types
 ********************************************************************/
 #ifdef __SIMCOM_EAT_WMMP__
typedef struct _eat_sms_struct_
{
    u16 len;
    u8 data[EAT_SMS_MAX_LEN];
}EatSms_st;
 #endif

typedef enum _eat_sms_storage_en_
{
    EAT_SM = 0, //SM storage type
    EAT_ME = 1,//ME storage type
    EAT_SM_P = 2, //Priority SM storage type
    EAT_ME_P = 3, //Priority ME storage type
    EAT_MT = 4 // SM and ME storage type
}EatSmsStorage_en;// storage type

typedef struct _eat_sms_read_cnf_st_
{
    u8 name[EAT_SMS_NAME_LEN+1]; //receive phone name
    u8 datetime[EAT_SMS_DATA_TIME_LEN+1];//receive message date and time
    u8 data[EAT_SMS_DATA_LEN+1];   //receive message string
    u8 number[EAT_SMS_NUMBER_LEN+1]; //receive phone number
    u8 status;    //message status
    u16 len;      // message length
}EatSmsReadCnf_st; // read message infomation

typedef struct _eat_sms_new_message_st_
{
    EatSmsStorage_en storage; //storage type
    u16 index; //index in storage
}EatSmsNewMessageInd_st;//new message infomation

typedef struct {
    u8 addr_length; //original address length
    u8 addr_bcd[11];//original address
} EatSmsAddr_st; //original address information

typedef struct {
    u16 ref; /* concat. message reference*/
    u8 total_seg; /* total segments*/
    u8 seg; /* indicate which segment*/
} EatSmsalConcat_st; //concat message infomation

typedef enum
{
   EAT_SMSAL_MTI_DELIVER          = 0x00, //deliver type
   EAT_SMSAL_MTI_DELIVER_REPORT   = 0x00,//deliver report type
   EAT_SMSAL_MTI_SUBMIT           = 0x01, //submit type
   EAT_SMSAL_MTI_SUBMIT_REPORT    = 0x01, //sumbit report type
   EAT_SMSAL_MTI_STATUS_REPORT    = 0x02, // status report type
   EAT_SMSAL_MTI_COMMAND          = 0x02, //command type
   
   EAT_SMSAL_MTI_UNSPECIFIED      = 0x03,  /* MMI shall handle this case,
                                        * eg, displaying "does not support", 
                                        * or "cannot display", etc */
   EAT_SMSAL_MTI_ILLEGAL          = 0x04 // illegal type
} EatSmsalMti_en;//deliver or submit type

typedef struct
{
   u32 dest_port; /*destination port -1: invalid port */
   u32 src_port; /* source port*/
} EatSmsalPort_st;//port information

typedef enum
{
   EAT_SMSAL_GSM7_BIT = 0,   //7bit code
   EAT_SMSAL_EIGHT_BIT,      //7bit code 
   EAT_SMSAL_UCS2,          //ucs2 code
   EAT_SMSAL_ALPHABET_UNSPECIFIED // unspecified   code

} EatSmsalAlphabet_en;//code information

typedef enum
{
   EAT_SMSAL_CLASS0 = 0, //class 0 message
   EAT_SMSAL_CLASS1,//class 1 message
   EAT_SMSAL_CLASS2,//class 2 message
   EAT_SMSAL_CLASS3,//class 3 message
   EAT_SMSAL_CLASS_UNSPECIFIED,//unspecified message
   
   EAT_SMSAL_MW_DISCARD, /* follows are for SMSAL internal use */
   EAT_SMSAL_MW_STORE, /* follows are for SMSAL internal use */
   EAT_SMSAL_RCM, /* follows are for SMSAL internal use */
   EAT_NUM_OF_NMI_MSG_ENUM //illegal 
} EatSmsalMsgClass_en;//message class information

typedef struct {
    u8 reply_flag; /* whether reply is sought*/
    u8 udh_p; /* indicates presence of
                                             user data header*/
    u8 status_rep_flag; /* whether status reports are sought*/
    u8 fill_bits; /* to be ignored*/
    u8 mms; /* more message to send*/
    u8 msg_type; /* sms deliver*/
    u8 no_orig_addr; //original phone number type
    u8 orig_addr_size; //phone number
    u8 orig_addr[EAT_SMS_NUMBER_LEN]; /* assumed to be byte-packed*/
    u8 pid; /* to be bit-wise decoded*/
    u8 dcs; /* to be bit-wise decoded*/
    u8 scts[7]; // data time
    u8 user_data_len; //user data length
    u8 no_user_data; //user data flag
    u8 user_data_size; //user data size
    u8 user_data[EAT_SMS_MAX_LEN]; /* can have user data header also*/
} EatSmsalDeliverPeer_st; //deliver message information

typedef struct {
    u8 reply_flag; /* whether reply is sought*/
    u8 udh_p; /* indicates presence of
                                             user data header*/
    u8 status_rep_flag; /* whether status reports are sought*/
    u8 vp_flag; /* indicates presence ofvalidity period*/
    u8 rej_dup_flag; //status flag
    u8 msg_type; /* sms submit*/
    u8 msg_ref; /* to be read from SIM*/
    u8 no_dest_addr; // destination address
    u8 dest_addr_size;//destination address size
    u8 dest_addr[EAT_SMS_NUMBER_LEN]; /* assumed to be byte-packed*/
    u8 pid; /* to be bit-wise encoded*/
    u8 dcs; /* to be bit-wise encoded*/
    
    u8 user_data_len;// user data length
    u8 no_user_data; //user data flag
    u8 user_data_size;// user data size
    u8 user_data[EAT_SMS_MAX_LEN]; /* can have user data header also*/
} EatSmsalSubmitPeer_st; //submit message information

typedef struct
{
   union 
   {
      EatSmsalDeliverPeer_st  deliver_tpdu; //deliver tpdu information
      EatSmsalSubmitPeer_st   submit_tpdu; //submit tpdu information
   } data; //deliver or submit tpdu information

   EatSmsalConcat_st        concat_info; // concat information
                
   EatSmsalMti_en             mti;    //submit or deliver type  
   u8                  fo;     /* first octet */
   u8                  offset; /* offset to message content */
   u8                  msg_len;/* length of user data */   
  
   u8                  udhl;   /* for calculating offset to unpack */

   /* for decoding DCS */   
   EatSmsalMsgClass_en       msg_class;    // class message information
   EatSmsalAlphabet_en        alphabet_type; // code information
   eat_bool                   is_compress; //message is compress

   EatSmsalPort_st          port; //port information
} EatSmsalTpduDecode_st;// TPDU information

typedef struct
{
   EatSmsAddr_st          sca; // center number information
   EatSmsalTpduDecode_st tpdu; // TPDU information
   u8                pdu_len;   /* length of PDU */
   u8                tpdu_len;  /* length of TPDU */
} EatSmsalPduDecode_st; // PDU information



#if defined(__SIMCOM_EAT_JAVA__) 
typedef eat_bool (* Notify_Incoming_Sms)(u8 msgType, 
                                        char*  sourceAddress,
                                        unsigned char*          msgBuffer,
                                        int                     msgBufferLen,
                                        unsigned short          sourcePortNum,
                                        unsigned short          destPortNum,
                                        u64                     timeStamp
                                        );
#endif

typedef void (* Sms_Send_Completed)(eat_bool result);
typedef void (* Sms_Read_Completed)(EatSmsReadCnf_st  smsReadContent);
typedef void (* Sms_Delete_Completed)(eat_bool result);
typedef void (* Sms_New_Message_Ind)(EatSmsNewMessageInd_st smsNewMessage);
typedef void (* Sms_Flash_Message_Ind)(EatSmsReadCnf_st smsFlashMessage);
typedef void(* Sms_Ready_Ind)(eat_bool result);


/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/

/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/

/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern eat_bool  (*const eat_WPI_SendSMS)(char *sms_data,short sms_len);
/********************************************************************
* Function    :    eat_get_SMS_sc_core
* Description :    Get the SMS center number
* Parameters  :    [OUT] char* - SMS center number str 
* Returns     :    true or  false
********************************************************************/
extern eat_bool  (*const eat_get_SMS_sc)(char *sms_data);

/*****************************************************************************
* Function :  eat_get_sms_ready_state
* Description
*    check sms module whether ready.
* Returns
*         EAT_TRUE: SMS module ready, EAT_FALSE: SMS module not ready.
*****************************************************************************/
extern eat_bool (*const eat_get_sms_ready_state)(void);

/********************************************************************
* Function    :    eat_acsii_to_ucs2
* Description :    this function is using to convert ascii code to ucs2 format
* Parameters  :    
*               src_text: u8* [IN], source string,pointer to input data
*               src_len: u32 [IN], source string len
*               dest_text: u8* [OUT], dest string buffer, pointer to output data
*               size: u32 [IN], dest string buffer size
* Returns:
*    dest string len
********************************************************************/
extern u32 (*const eat_acsii_to_ucs2)(u8* src_text, u32 src_len,u8* dest_text,u32 size);

/********************************************************************
* Function    :    eat_acsii_to_gsm7bit
* Description :    this function is using to convert ascii code to 7bit code
* Parameters  :    
*              src_text: u8* [IN],source string,pointer to input data
*              src_len: u32 [IN], source string len
*              dest_text:u8* [OUT] , dest string buffer, pointer to output data
* Returns : 
*       dest string len
********************************************************************/
extern u32 (*const eat_acsii_to_gsm7bit)(u8* src_text, u32 src_len,u8* dest_text);

/********************************************************************
* Function    :    eat_send_pdu_sms
* Description :    this function is send pdu mode message
*            sms_data: char * [IN],source string,pointer to sms data
*             sms_len: short [IN],sms len
* Returns     :   
*   EAT_TRUE, send message success
*   EAT_FALSE, send message failed
* NOTE:
*       message length: 7bit code  160
*                    8bit code     140
*                    UCS2          70
*       sms_len is PDU length,start PDU TYPE field to compute
*       eg:sms_data is 0011000D91683186613767F20018010400410042, sms_len is 19 (11000D91683186613767F20018010400410042)
********************************************************************/
extern eat_bool (*const eat_send_pdu_sms)(char *sms_data,short sms_len);

/********************************************************************
* Function    :    eat_set_sms_format
* Description :    this function set sms format
* Parameters  :    
*           format : u8 [IN],sms mode
* Returns     :   
*   EAT_TRUE, set format success
*   EAT_FALSE, set format failed
********************************************************************/
extern eat_bool (*const eat_set_sms_format)(u8 format);

/********************************************************************
* Function    :    eat_get_sms_format
* Description :    this function get sms format
* Parameters  :  
            format:u8* [OUT], pointer to sms format
* Returns :   
*       NULL
********************************************************************/
extern void (*const eat_get_sms_format)(u8* format) ;

/********************************************************************
* Function    :    eat_set_sms_cnmi
* Description :    this function set sms cnmi parameter
* Parameters  :    
*              mode: u8 [IN], cnmi parameter mode
*              mt: u8 [IN], cnmi parameter mt
*              bm :u8 [IN], cnmi parameter bm
*              ds:u8 [IN], parameter ds
*              bfr: u8 [IN],cnmi parameter bfr
* Returns     :   
*   EAT_TRUE, set cnmi parameter success
*   EAT_FALSE, set cnmi parameter failed
********************************************************************/
extern eat_bool (*const eat_set_sms_cnmi)(u8 mode, u8 mt, u8 bm,u8 ds, u8 bfr);

/********************************************************************
* Function    :    eat_get_sms_operation_mode
* Description :    this function get sms operation by API or AT mode
* Returns     :    
*       EAT_TRUE:API operate SMS or 
*       EAT_FALSE:AT operate SMS
********************************************************************/
extern eat_bool (*const eat_get_sms_operation_mode)();

/********************************************************************
* Function    :    eat_set_sms_operation_mode
* Description :    this function set sms operation by API or AT mode
* Parameters  :    
              mode: eat_bool [IN],operate SMS mode
              TRUE-API mode
              FALSE-AT command mode
* Returns     :    
*           NULL
********************************************************************/
extern void (*const eat_set_sms_operation_mode)(eat_bool mode);

/********************************************************************
* Function    :    eat_sms_register_send_completed_callback
* Description :    this function is message incoming to callback function
* Parameters  :   
*               sms_send_completed_callback: Sms_Send_Completed[IN] , register callback function
* Returns     :    
*       EAT_TRUE: register function success
*       EAT_FALSE: register function failed
********************************************************************/
extern eat_bool (*const eat_sms_register_send_completed_callback)(Sms_Send_Completed sms_send_completed_callback);

#if defined(__SIMCOM_EAT_JAVA__) 
/********************************************************************
* Function    :    eat_sms_register_incoming_sms_callback
* Description :    this function is sending messaeg completed to callback
* Parameters  :    
                notify_incoming_sms_callback: Notify_Incoming_Sms [IN],register callback function
* Returns     :    
*       EAT_TRUE: register function success
*       EAT_FALSE: register function failed
********************************************************************/
extern eat_bool (*const eat_sms_register_incoming_sms_callback)(Notify_Incoming_Sms notify_incoming_sms_callback);
#endif

/********************************************************************
* Function    :    eat_set_sms_sc
* Description :    this function is setting the SMS center number
* Parameters  :    
                szNumber: u8 [IN],sms sc number
* Returns     :    
*       EAT_TRUE: set sc number success
*       EAT_FALSE: set sc number failed
********************************************************************/
extern eat_bool (*const eat_set_sms_sc)(u8* szNumber);

/********************************************************************
* Function    :    eat_set_sms_storage
* Description :    this function is setting the SMS storage type
* Parameters  :    
                mem1: u8 [IN],for read or delete sms
                mem2: u8 [IN],for save sms
                mem3: u8 [IN],for receive sms
* Returns     :    
*       EAT_TRUE: set SMS storage type success
*       EAT_FALSE: set SMS storage type failed
********************************************************************/
extern eat_bool (*const eat_set_sms_storage)(EatSmsStorage_en mem1, EatSmsStorage_en mem2, EatSmsStorage_en mem3);

/********************************************************************
* Function    :    eat_send_text_sms
* Description :    this function is sending text mode message
* Parameters  :    
                szNumber: u8* [IN],source string,pointer to phone number
                sms_data: u8* [IN],source string,pointer to sms data
* Returns     :    
*       EAT_TRUE: sending text mode message success
*       EAT_FALSE: sending text mode message failed
* NOTE:
*      message length: 7bit code  160
*                    8bit code    140
*                    UCS2         70
********************************************************************/
extern eat_bool (*const eat_send_text_sms)(u8* szNumber,u8* sms_data);

/********************************************************************
* Function    :    eat_read_sms
* Description :    this function is is reading a message
* Parameters  :    
                index: u16 [IN],index of message list
                sms_read_completed_callback: Sms_Read_Completed [IN],callback function for reading a message result
* Returns     :    
*       EAT_TRUE: reading a message success
*       EAT_FALSE: reading a message failed
********************************************************************/
extern eat_bool (*const eat_read_sms)(u16 index,Sms_Read_Completed sms_read_completed_callback);

/********************************************************************
* Function    :    eat_read_sms
* Description :    this function is is reading a message
* Parameters  :    
                index: u16 [IN],index of message list
                sms_delete_completed_callback: Sms_Delete_Completed [IN],callback function for deleting a message result
* Returns     :    
*       EAT_TRUE: deleting a message success
*       EAT_FALSE: deleting a message failed
********************************************************************/
extern eat_bool (*const eat_delete_sms)(u16 index,Sms_Delete_Completed sms_delete_completed_callback);

/********************************************************************
* Function    :    eat_sms_register_new_message_callback
* Description :    this function is new messaeg reported to callback
* Parameters  :    
                sms_new_message_callback: Sms_New_Message_Ind [IN],register callback function
* Returns     :    
*       EAT_TRUE: register function success
*       EAT_FALSE: register function failed
********************************************************************/
extern eat_bool (*const eat_sms_register_new_message_callback)(Sms_New_Message_Ind sms_new_message_callback);

/********************************************************************
* Function    :    eat_sms_register_flash_message_callback
* Description :    this function is reporting flash messaeg to callback
* Parameters  :    
                sms_flash_message_callback: Sms_Flash_Message_Ind [IN],register callback function
* Returns     :    
*       EAT_TRUE: register function success
*       EAT_FALSE: register function failed
********************************************************************/
extern eat_bool (*const eat_sms_register_flash_message_callback)(Sms_Flash_Message_Ind sms_flash_message_callback);

/********************************************************************
* Function    :    eat_sms_decode_tpdu
* Description :    this function decode PDU messaeg
* Parameters  :    
                pdu: u8* [IN],sms's pdu data
                pdu_len: u8 [IN],sms's pdu length
                sms_pdu: EatSmsalPduDecode_st * [OUT],decode sms context
* Returns     :    
*       EAT_TRUE: decode sms success
*       EAT_FALSE: decode sms failed
********************************************************************/
extern eat_bool (*const eat_sms_decode_tpdu)(u8* pdu, u8 pdu_len, EatSmsalPduDecode_st *sms_pdu);

/********************************************************************
* Function    :    eat_sms_orig_address_data_convert
* Description :    this function convert orig address
* Parameters  :    
                ptrOrigAddr: u8* [IN],orig address
                ptrDestAddr: u8* [OUT],converted address
* Returns     :    
*       EAT_TRUE: convert address success
*       EAT_FALSE: convert address failed
********************************************************************/
extern eat_bool (*const eat_sms_orig_address_data_convert)(u8* ptrOrigAddr, u8* ptrDestAddr);

/********************************************************************
* Function    :    eat_sms_register_sms_ready_callback
* Description :    this function is reporting sms ready to callback
* Parameters  :    
                sms_ready_callback: Sms_Ready_Ind [IN],register callback function
* Returns     :    
*       EAT_TRUE: register function success
*       EAT_FALSE: register function failed
********************************************************************/
extern eat_bool (*const eat_sms_register_sms_ready_callback)(Sms_Ready_Ind sms_ready_callback);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
#endif // _EAT_WMMP_H_
