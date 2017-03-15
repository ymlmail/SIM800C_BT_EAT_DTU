/*
 * =====================================================================================
 *
 *       Filename:  app_sms_pdu_decode.h
 *
 *    Description:  the head file
 *
 *        Version:  1.0
 *        Created:  2014-3-11 15:58:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhudingfen
 *   Organization:  www.sim.com
 *
 * =====================================================================================
 */

#ifndef _APP_SMS_PDU_DECODE_H_
#define _APP_SMS_PDU_DECODE_H_
/* #####   HEADER FILE INCLUDES   ################################################### */
#include "eat_interface.h"

/* #####   EXPORTED MACROS   ######################################################## */
#define EAT_MAX_CC_ADDR_LEN          42

typedef struct {
    u8 addr_length;
    u8 addr_bcd[11];
} eat_sms_addr_struct;

typedef struct
{
   u8 type;
   u8 length;
   u8 number[EAT_MAX_CC_ADDR_LEN];
} eat_l4c_number_struct;

typedef enum
{
    EAT_CONVERT_TO_DIGIT_NORMAL,
    EAT_CONVERT_TO_DIGIT_WILD,
    EAT_CONVERT_TO_DIGIT_EXT,
    EAT_CONVERT_TO_DIGIT_24008
}eat_convert_to_digit_enum;

typedef struct EatRtcTimeTag
{
    u8 hours;
    u8 minutes;
    u8 seconds;
} eat_rtc_time;

typedef struct EatRtcDateTag
{
    u16 year;
    u8  month;
    u8  day;
}eat_rtc_date;

typedef struct {
    u8 reply_flag; /* whether reply is sought*/
    u8 udh_p; /* indicates presence of
                                             user data header*/
    u8 status_rep_flag; /* whether status reports
                                               are sought*/
    u8 fill_bits; /* to be ignored*/
    u8 mms; /* more message to send*/
    u8 msg_type; /* sms deliver*/
    u8 no_orig_addr;
    u8 orig_addr_size;
    u8 *orig_addr; /* assumed to be
                                                         byte-packed*/
    u8 pid; /* to be bit-wise decoded*/
    u8 dcs; /* to be bit-wise decoded*/
    u8 scts[7];
    u8 user_data_len;
    u8 no_user_data;
    u8 user_data_size;
    u8 *user_data; /* can have user data header also*/
} eat_smsal_deliver_peer_struct;

typedef struct {
    u8 reply_flag; /* whether reply is sought*/
    u8 udh_p; /* indicates presence of
                                             user data header*/
    u8 status_rep_flag; /* whether status reports
                                             are sought*/
    u8 vp_flag; /* indicates presence of
                                             validity period*/
    u8 rej_dup_flag;
    u8 msg_type; /* sms submit*/
    u8 msg_ref; /* to be read from SIM*/
    u8 no_dest_addr;
    u8 dest_addr_size;
    u8 *dest_addr; /* assumed to be
                                                         byte-packed*/
    u8 pid; /* to be bit-wise encoded*/
    u8 dcs; /* to be bit-wise encoded*/
    u8 no_validity_period;
    u8 validity_period_size;
    u8 *validity_period; /* to be
                                                       bit/byte-wise 
                                                       encoded, cannot 
                                                       be even represnted 
                                                       using CHOICE*/
    u8 user_data_len;
    u8 no_user_data;
    u8 user_data_size;
    u8 *user_data; /* can have user data header also*/
} eat_smsal_submit_peer_struct;

typedef struct {
    u8 fill_bits1; /* to be ignored*/
    u8 udh_p; /* indicates presence of
                                              user data header*/
    u8 status_rep_type; /* sms command or sms
                                              submit*/
    u8 fill_bits2; /* to be ignored*/
    u8 mms;
    u8 msg_type; /* sms status report*/
    u8 msg_ref;
    u8 no_rcpnt_addr;
    u8 rcpnt_addr_size;
    u8 *rcpnt_addr;
    u8 scts[7];
    u8 dis_time[7];
    u8 status; /* actual status of submit*/
    u8 params_p; /* depicts presence of
                                                optional parameters*/
    u8 pid; /* to be bit-wise decoded*/
    u8 dcs; /* to be bit-wise decoded*/
    u8 user_data_len;
    u8 no_user_data;
    u8 user_data_size;
    u8 *user_data; /* can have user data header also*/
} eat_smsal_status_report_peer_struct;

typedef struct {
    u16 ref; /* concat. message reference*/
    u8 total_seg; /* total segments*/
    u8 seg; /* indicate which segment*/
} eat_smsal_concat_struct;

typedef enum
{
   EAT_AT_SMSAL_MTI_DELIVER          = 0x00,
   EAT_AT_SMSAL_MTI_DELIVER_REPORT   = 0x00,
   EAT_AT_SMSAL_MTI_SUBMIT           = 0x01,
   EAT_AT_SMSAL_MTI_SUBMIT_REPORT    = 0x01,
   EAT_AT_SMSAL_MTI_STATUS_REPORT    = 0x02,
   EAT_AT_SMSAL_MTI_COMMAND          = 0x02,
   
   EAT_AT_SMSAL_MTI_UNSPECIFIED      = 0x03,  /* MMI shall handle this case,
                                        * eg, displaying "does not support", 
                                        * or "cannot display", etc */
   EAT_AT_SMSAL_MTI_ILLEGAL          = 0x04
} eat_smsal_mti_enum;

typedef enum
{
   EAT_AT_SMSAL_CLASS0 = 0,
   EAT_AT_SMSAL_CLASS1,
   EAT_AT_SMSAL_CLASS2,
   EAT_AT_SMSAL_CLASS3,
   EAT_AT_SMSAL_CLASS_UNSPECIFIED,
   
   EAT_AT_SMSAL_MW_DISCARD, /* follows are for SMSAL internal use */
   EAT_AT_SMSAL_MW_STORE,
   EAT_AT_SMSAL_RCM,
   EAT_AT_NUM_OF_NMI_MSG_ENUM
} eat_smsal_msg_class_enum;

typedef enum
{
   EAT_AT_SMSAL_GSM7_BIT = 0,    
   EAT_AT_SMSAL_EIGHT_BIT,
   EAT_AT_SMSAL_UCS2,       
   EAT_AT_SMSAL_ALPHABET_UNSPECIFIED       

} eat_smsal_alphabet_enum;

typedef enum
{
   EAT_SMSAL_MW_VM = 0,
   EAT_SMSAL_MW_FAX,
   EAT_SMSAL_MW_EMAIL, 
   EAT_SMSAL_MW_OTHER,
#ifdef __REL6__
   EAT_SMSAL_MW_VIDEO_MSG,       
#endif
   EAT_NUM_OF_MSG_WAITING_TYPE
} smsal_msg_waiting_type_enum;

typedef struct
{
   //kal_uint8 waiting_ind;
   u8 waiting_num[EAT_NUM_OF_MSG_WAITING_TYPE];
} smsal_mwis_struct;

typedef struct 
{
   u8         type_of_info;
   eat_bool          need_store;
   eat_bool          is_msg_wait;
   eat_bool          is_show_num[EAT_NUM_OF_MSG_WAITING_TYPE];
   eat_bool          is_clear[EAT_NUM_OF_MSG_WAITING_TYPE];
   eat_bool          ind_flag[EAT_NUM_OF_MSG_WAITING_TYPE];
   smsal_mwis_struct mwis;    
   u8         msp;
   u8         line_no;
} eat_smsal_msg_waiting_struct;

typedef struct
{
   u32 dest_port; /* -1: invalid port */
   u32 src_port;
} eat_smsal_port_struct;

typedef struct
{
   union 
   {
      eat_smsal_deliver_peer_struct  deliver_tpdu;
      eat_smsal_submit_peer_struct   submit_tpdu;
      eat_smsal_status_report_peer_struct  report_tpdu;
   } data;

   eat_smsal_concat_struct        concat_info;
                
   eat_smsal_mti_enum             mti;      
   u8                  fo;     /* first octet */
   u8                  offset; /* offset to message content */
   u8                  msg_len;/* length of user data */   
  
   u8                  udhl;   /* for calculating offset to unpack */

   /* for decoding DCS */   
   eat_smsal_msg_class_enum       msg_class;    
   eat_smsal_alphabet_enum        alphabet_type;
   eat_bool                   is_compress;
   eat_smsal_msg_waiting_struct   msg_wait;

   eat_smsal_port_struct          port;
} eat_smsal_tpdu_decode_struct;

typedef struct
{
   eat_sms_addr_struct          sca;
   eat_smsal_tpdu_decode_struct  tpdu; 
   u8                pdu_len;   /* length of PDU */
   u8                tpdu_len;  /* length of TPDU */
} eat_smsal_pdu_decode_struct;

typedef enum
{
   EAT_SMSAL_MSG_WAIT_CPHS  = 0x01 ,
   EAT_SMSAL_MSG_WAIT_DCS   = 0x02 ,
   EAT_SMSAL_MSG_WAIT_UDH       = 0x04 ,
   EAT_SMSAL_MSG_WAIT_UDH_EVM   = 0x08
} eat_smsal_type_of_msg_waiting_enum;

extern eat_bool eat_decode_sms_pdu(u8               *pdu, 
                              u8               pdu_len,
                              eat_smsal_pdu_decode_struct *sms_pdu);

extern void eat_sms_free_tpdu_decode_struct(eat_smsal_tpdu_decode_struct *tpdu_decode_ptr);
#endif
