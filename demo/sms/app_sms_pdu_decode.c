/*
 * =====================================================================================
 *
 *       Filename:  app_sms_pdu_decode.c
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

/* #####   HEADER FILE INCLUDES   ################################################### */
#include "app_sms_pdu_decode.h"

/* #####   EXPORTED MACROS   ######################################################## */
#define EAT_SMSAL_ADDR_LEN              (11)
#define EAT_SMSAL_INVALID_PORT_NUM  (-1)
#define EAT_SMSAL_MAX_REG_PORT_NUM  (10)
#define EAT_SMSAL_MTI_BITS        (0x03)
#define EAT_SMSAL_ONE_MSG_OCTET      (140)
#define EAT_PART_OF_DELIVER_HEADER_LEN     (13)

#define EAT_SMSAL_CONC8_MSG_IEI         (0x00)
#define EAT_SMSAL_CONC16_MSG_IEI        (0x08)
#define EAT_SMSAL_SPECIAL_MSG_IND_IEI   (0x01)
#define EAT_SMSAL_ENHANCED_VM_INFO_IEI  (0x23)

/* EMS */
#define EAT_SMSAL_EMS_TEXT_FORMAT_IEI   (0x0a)   /* Text Formating */
#define EAT_SMSAL_EMS_PREDEF_SND_IEI    (0x0b)   /* Predefined Sound */
#define EAT_SMSAL_EMS_USER_DEF_SND_IEI  (0x0c)   /* User Defined Sound */
#define EAT_SMSAL_EMS_PREDEF_ANM_IEI    (0x0d)   /* Predefined Animation */
#define EAT_SMSAL_EMS_LARGE_ANM_IEI     (0x0e)   /* Large Animation */
#define EAT_SMSAL_EMS_SMALL_ANM_IEI     (0x0f)   /* Small Animation */
#define EAT_SMSAL_EMS_LARGE_PIC_IEI     (0x10)   /* Large Picture */
#define EAT_SMSAL_EMS_SMALL_PIC_IEI     (0x11)   /* Small Picture */
#define EAT_SMSAL_EMS_VAR_PIC_IEI       (0x12)   /* Variable Picture */

/* MISC */
#define EAT_SMSAL_APP_PORT8_IEI         (0x04)   /* application port - 8 bit */
#define EAT_SMSAL_APP_PORT16_IEI        (0x05)   /* application port - 16 bit */
#define EAT_SMSAL_NL_SINGLE_IEI         (0x24)   /* National Language single shift */
#define EAT_SMSAL_NL_LOCKING_IEI        (0x25)   /* National Language looking shift */
#define EAT_SMSAL_CPHS_VM_ADDR_TYPE  (0xd0)

#define MASK(_w) ( ((_w)>31) ? 0xffffffff : (0x1u << (_w)) - 1 )


/* _uc is the pointer to the byte stream from which bits to be read */
/* _s is the offset in bits from MSB of the byte stream _uc  */
/* _w is the number of bits to be read  */

#define GET_BITS_1_8(_uc, _s, _w) (                                  \
   (((_s)+(_w)>8) ? *(_uc) << ((_s)+(_w)-8) |                        \
    *((_uc)+1) >> (16-(_s)-(_w)) :                                   \
     *(_uc) << (_s) >> (8-(_w)))                                     \
    & MASK(_w) )

static void eat_smsal_reset_tpdu_decode_struct(eat_smsal_tpdu_decode_struct *tpdu_decode_ptr)
{
    u8 i = 0;
    
    /* Reset TPDU Decode struct */
    tpdu_decode_ptr->offset = 0;
    tpdu_decode_ptr->msg_len = 0;
    tpdu_decode_ptr->udhl = 0;

    /* if sms is not concatenated , set total_seg & seg to 1 */
    tpdu_decode_ptr->concat_info.total_seg = 1; /* not concat. msg */
    tpdu_decode_ptr->concat_info.seg = 1;
    tpdu_decode_ptr->concat_info.ref = 0;
    tpdu_decode_ptr->msg_class = EAT_AT_SMSAL_CLASS_UNSPECIFIED;
    tpdu_decode_ptr->msg_wait.type_of_info = 0;
    tpdu_decode_ptr->msg_wait.need_store = EAT_FALSE;
    tpdu_decode_ptr->msg_wait.is_msg_wait = EAT_FALSE;


    for (i = 0; i < EAT_NUM_OF_MSG_WAITING_TYPE; i++)
    {
        tpdu_decode_ptr->msg_wait.is_show_num[i] = EAT_FALSE;
        tpdu_decode_ptr->msg_wait.is_clear[i] = EAT_FALSE;
        tpdu_decode_ptr->msg_wait.ind_flag[i] = EAT_FALSE;
    }
    //tpdu_decode_ptr->msg_wait.mwis = EAT_FALSE;

    tpdu_decode_ptr->port.dest_port = EAT_SMSAL_INVALID_PORT_NUM;
    tpdu_decode_ptr->port.src_port = EAT_SMSAL_INVALID_PORT_NUM;


}    


void eat_smsal_deliver_peer_struct_alloc(void *mtk_d)
{
    eat_smsal_deliver_peer_struct *s = (eat_smsal_deliver_peer_struct *) mtk_d;

    memset(s, 0, sizeof(eat_smsal_deliver_peer_struct));
    s->orig_addr_size = 12;
    s->no_orig_addr = 0;
    s->orig_addr = (u8 *) eat_mem_alloc (12 * sizeof(u8));
    {
        int i = 0;

        for (i = 0; i < 7; i++)
        {
        }
    }
    s->user_data_size = 140;
    s->no_user_data = 0;
    s->user_data = (u8 *) eat_mem_alloc (140 * sizeof(u8));
}

/*****************************************************************************
* FUNCTION
*  smsal_decode_dcs
* DESCRIPTION
*   This function decodes the data coding scheme.
*
* PARAMETERS
*  a  IN       dcs
*  b  IN/OUT   alphabet_type
*  c  IN/OUT   msg_class
*  d  IN/OUT   compress
*  e  IN/OUT   msg_wait
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void eat_smsal_decode_dcs(u8                 dcs,
                      eat_smsal_alphabet_enum       *alphabet_type,
                      eat_smsal_msg_class_enum      *msg_class,
                      eat_bool                  *is_compress,
                      eat_smsal_msg_waiting_struct  *msg_wait)
{
    u8 coding_group;

    /* Default DCS value */
    *alphabet_type = EAT_AT_SMSAL_GSM7_BIT;
    *msg_class = EAT_AT_SMSAL_CLASS_UNSPECIFIED;
    *is_compress = EAT_FALSE;

    if (dcs == 0)
        return;

    coding_group = dcs >> 4;

    if (coding_group == 0x0f)           /* Data Coding/Message Class */
    {
        /*
         * SCR: 2001,
         * * ensure the reserved bit is zero
         */
        if ((dcs & 0x08) == 0x08)
        {
            *alphabet_type = EAT_AT_SMSAL_ALPHABET_UNSPECIFIED;
        }
        else
        {
            *msg_class = (eat_smsal_msg_class_enum) (dcs & 0x03);
            *alphabet_type = (eat_smsal_alphabet_enum) ((dcs & 0x04) >> 2);
        }
    }
    else if ((coding_group | 0x07) == 0x07)     /* General Data Coding Scheme */
    {
        if ((dcs & 0x10) == 0x10)
            *msg_class = (eat_smsal_msg_class_enum) (dcs & 0x03);
        else
            *msg_class = EAT_AT_SMSAL_CLASS_UNSPECIFIED;

        *alphabet_type = (eat_smsal_alphabet_enum) ((dcs >> 2) & 0x03);
        if (((coding_group & 0x02) >> 1) == 1)
            *is_compress = EAT_TRUE;
    }
    else if ((coding_group & 0x0c) == 0x08)
    {
        /*
         * according GSM 23.038 clause 4:
         * "Any reserved codings shall be assumed to be the GSM 7 bit default alphabet."
         */
    }
    else if (((coding_group & 0x0f) == 0x0c) || /* discard */
             ((coding_group & 0x0f) == 0x0d) || /* store, gsm-7 */
             ((coding_group & 0x0f) == 0x0e))   /* store, ucs2 */
    {

        /* 1110: msg wait ind (store, ucs2) */
        if ((coding_group & 0x0f) == 0x0e)
            *alphabet_type = EAT_AT_SMSAL_UCS2;

        if (msg_wait != NULL)
        {
            msg_wait->type_of_info |= EAT_SMSAL_MSG_WAIT_DCS;
            msg_wait->is_msg_wait = EAT_TRUE;
        }
    }

    /*
     * if the reserved bit been set or the alphabet_type uses the reserved one,
     * then according GSM 23.038 clause 4:
     * "Any reserved codings shall be assumed to be the GSM default alphabet."
     * we change it as SMSAL_GSM7_BIT
     */

    if (*alphabet_type == EAT_AT_SMSAL_ALPHABET_UNSPECIFIED)
    {
        *alphabet_type = EAT_AT_SMSAL_GSM7_BIT;
    }

}                                       /* end of smsal_decode_dcs */

/*****************************************************************************
* FUNCTION
*  smsal_is_len_in8bit
* DESCRIPTION
*   This function determines the unit of TP-User-Data.
*
* PARAMETERS
*  a  IN          dcs
* RETURNS
*  KAL_TRUE: length in octet
*  KAL_TRUE: length in septet
* GLOBALS AFFECTED
*  none
*****************************************************************************/
eat_bool eat_smsal_is_len_in8bit(u8 dcs)
{
    eat_bool is_compress;
    eat_smsal_alphabet_enum alphabet_type;
    eat_smsal_msg_class_enum mclass;

    eat_smsal_decode_dcs(dcs, &alphabet_type, &mclass, &is_compress, NULL);

    if ((is_compress == EAT_TRUE) ||
        (alphabet_type == EAT_AT_SMSAL_EIGHT_BIT) ||
        (alphabet_type == EAT_AT_SMSAL_UCS2))
    {
        return EAT_TRUE;
    }
    else
    {
        return EAT_FALSE;
    }

}   

/*****************************************************************************
* FUNCTION
*  eat_smsal_msg_len_in_octet
* DESCRIPTION
*   This function return the number of bytes of TP-User-Data.
*
* PARAMETERS
*  a  IN          dcs
*  b  IN          len, TP-User-Data-Len
* RETURNS
*  length in octets
* GLOBALS AFFECTED
*  none
*****************************************************************************/
u16 eat_smsal_msg_len_in_octet(u8 dcs, u16 len)
{
    if (eat_smsal_is_len_in8bit(dcs) == EAT_TRUE)
    {
        return len;
    }
    else
    {
        return (len * 7 + 7) / 8;
    }
}                                       /* end of smsal_msg_len_in_octet */

/*****************************************************************************
* FUNCTION
*  smsal_submit_peer_struct_unpack
* DESCRIPTION
*   This function is used to unpack SMS-SUBMIT.
*  
* PARAMETERS
*  a  IN       *s
*  b  IN       *frame
*  c  IN       bit_offset
*  d  IN       fr_size
*  e  IN       err_hndl, error handler
* RETURNS
*  number of unpacked bits
* GLOBALS AFFECTED
*  none
*****************************************************************************/
 unsigned int eat_smsal_deliver_peer_struct_unpack(void *mtk_d,
                                                  u8 * frame,
                                                  unsigned int bit_offset,
                                                  unsigned int fr_size,
                                                  void *err_hndl)
{
    eat_smsal_deliver_peer_struct *s = (eat_smsal_deliver_peer_struct *) mtk_d;
    int bits = bit_offset, skip_bits = 0;
    int i = 0;

    s->reply_flag = GET_BITS_1_8(frame, (bits & 0x07), 1);
    /* frame += (((bits&0x07) + 1) >> 3); */
    bits += 1;
    s->udh_p = GET_BITS_1_8(frame, (bits & 0x07), 1);
    /* frame += (((bits&0x07) + 1) >> 3); */
    bits += 1;
    s->status_rep_flag = GET_BITS_1_8(frame, (bits & 0x07), 1);
    /* frame += (((bits&0x07) + 1) >> 3); */
    bits += 1;
    s->fill_bits = GET_BITS_1_8(frame, (bits & 0x07), 2);
    /* frame += (((bits&0x07) + 2) >> 3); */
    bits += 2;
    s->mms = GET_BITS_1_8(frame, (bits & 0x07), 1);
    /* frame += (((bits&0x07) + 1) >> 3); */
    bits += 1;
    s->msg_type = GET_BITS_1_8(frame, (bits & 0x07), 2);
    frame += (((bits & 0x07) + 2) >> 3);
    bits += 2;

    /* unpack length and address type of TP-OA */
    for (i = 0; i < 2; i++)
    {
        s->orig_addr[i] = GET_BITS_1_8(frame, (bits & 0x07), 8);
        frame += (((bits & 0x07) + 8) >> 3);
        bits += 8;
    }

    /* max: 20 digits */
    if (s->orig_addr[0] > ((EAT_SMSAL_ADDR_LEN - 1) * 2))
    {
        s->orig_addr[0] = (EAT_SMSAL_ADDR_LEN - 1) * 2;
    }

    /* unpack address (BCD number) of TP-OA */
    for (i = 0; i < (s->orig_addr[0] + 1) / 2; i++)
    {
        s->orig_addr[i + 2] = GET_BITS_1_8(frame, (bits & 0x07), 8);
        frame += (((bits & 0x07) + 8) >> 3);
        bits += 8;
    }

    s->pid = GET_BITS_1_8(frame, (bits & 0x07), 8);
    frame += (((bits & 0x07) + 8) >> 3);
    bits += 8;
    s->dcs = GET_BITS_1_8(frame, (bits & 0x07), 8);
    frame += (((bits & 0x07) + 8) >> 3);
    bits += 8;

    for (i = 0; i < 7; i++)
    {
        s->scts[i] = GET_BITS_1_8(frame, (bits & 0x07), 8);
        frame += (((bits & 0x07) + 8) >> 3);
        bits += 8;
    }

    s->user_data_len = GET_BITS_1_8(frame, (bits & 0x07), 8);
    frame += (((bits & 0x07) + 8) >> 3);
    bits += 8;

    if (s->user_data_len > 0)
    {
        /* unpack User Data according TP-DCS */
        s->no_user_data = (u8) eat_smsal_msg_len_in_octet(s->dcs, s->user_data_len);

        if (s->no_user_data > EAT_SMSAL_ONE_MSG_OCTET)
            s->no_user_data = EAT_SMSAL_ONE_MSG_OCTET;

        for (i = 0; i < s->no_user_data; i++)
        {
            s->user_data[i] = GET_BITS_1_8(frame, (bits & 0x07), 8);
            frame += (((bits & 0x07) + 8) >> 3);
            bits += 8;
        }
    }                                   /* if */

    return bits - bit_offset - skip_bits;
}

eat_bool eat_smsal_detect_udh(u8 * data)
{
    u8 udhl = 0;                 /* user data header length */
    u8 read_byte = 0;            /* already read bytes */
    u8 iei;                      /* IE identifier */
    u8 ie_len;                   /* IE length */

    udhl = *data;

    if (udhl < 3 || udhl > EAT_SMSAL_ONE_MSG_OCTET)
        return EAT_FALSE;

    read_byte += 1;

    while (read_byte <= udhl)
    {
        /* Get IEI */
        iei = *(data + read_byte);

        /* Get IE Length */
        ie_len = *(data + read_byte + 1);

        read_byte += 2;

        if ( ((iei == 2) || iei == 0x23) || ((iei >= 0x26 && iei <=0x6F) || (iei >= 0x80 )) )
            return EAT_FALSE;

        if (ie_len == 0 || ie_len > (udhl + 1 - read_byte))
            return EAT_FALSE;

        switch (iei)
        {
            case EAT_SMSAL_NL_SINGLE_IEI:
            case EAT_SMSAL_NL_LOCKING_IEI:
                if (ie_len != 1)
                    return EAT_FALSE;
                break;                

            case EAT_SMSAL_SPECIAL_MSG_IND_IEI:
            case EAT_SMSAL_APP_PORT8_IEI:
            case EAT_SMSAL_EMS_PREDEF_SND_IEI:
            case EAT_SMSAL_EMS_PREDEF_ANM_IEI:
                if (ie_len != 2)
                    return EAT_FALSE;
                break;

            case EAT_SMSAL_CONC8_MSG_IEI:
                if (ie_len != 3)
                    return EAT_FALSE;
                break;

            case EAT_SMSAL_EMS_TEXT_FORMAT_IEI:
                if (ie_len != 3 && ie_len != 4)
                    return EAT_FALSE;
                break;

            case EAT_SMSAL_CONC16_MSG_IEI:
            case EAT_SMSAL_APP_PORT16_IEI:
                if (ie_len != 4)
                    return EAT_FALSE;
                break;

            case EAT_SMSAL_EMS_LARGE_PIC_IEI:
            case EAT_SMSAL_EMS_LARGE_ANM_IEI:
                if (ie_len != 129)
                    return EAT_FALSE;
                break;

            case EAT_SMSAL_EMS_SMALL_ANM_IEI:
            case EAT_SMSAL_EMS_SMALL_PIC_IEI:
                if (ie_len != 33)
                    return EAT_FALSE;
                break;

                /* Modify 2005.11.15 */
            default:
                return EAT_FALSE;
        }                               /* switch */

        /* illegal PDU */
        if ((read_byte + ie_len) > EAT_SMSAL_ONE_MSG_OCTET)
            return EAT_FALSE;

        read_byte += ie_len;

    }                                   /* while */

    if (read_byte != (udhl + 1))
        return EAT_FALSE;

    return EAT_TRUE;
}    

u8 eat_smsal_decode_udh(u8                *data,
                           eat_smsal_concat_struct      *concat_info,
                           eat_smsal_msg_waiting_struct *msg_wait,
                           eat_smsal_port_struct        *port)
{
    u8 udhl = 0;                 /* user data header length */
    u8 read_byte = 0;            /* already read bytes */
    u8 iei;                      /* IE identifier */
    u8 ie_len;                   /* IE length */
#if defined(__REL6__) && defined(__SMS_R6_ENHANCED_VOICE_MAIL__)
    u8 ind_num;
#endif

    if( msg_wait != NULL )
    {
        // set default
        msg_wait->msp = 0;
    }

    /* reset concat_info */
    if (concat_info != NULL)
        concat_info->total_seg = 1;

    if (port != NULL)
    {
        port->dest_port = EAT_SMSAL_INVALID_PORT_NUM;
        port->src_port = EAT_SMSAL_INVALID_PORT_NUM;
    }

    udhl = *data;

    if (udhl > EAT_SMSAL_ONE_MSG_OCTET)
        return 0;

    read_byte += 1;

    while (read_byte <= udhl)
    {
        /* Get IEI */
        iei = *(data + read_byte);

        /* Get IE Length */
        ie_len = *(data + read_byte + 1);

        read_byte += 2;

        switch (iei)
        {
          /*----------------------------------------------
           * Concatenated short message , 8-bit reference 
           *----------------------------------------------*/
            case EAT_SMSAL_CONC8_MSG_IEI:

                if (concat_info != NULL)
                {
                    /* seg = 0 is invalid, we shall ignore this IE */
                    if ((*(data + read_byte + 2)) > 0)
                    {
                        concat_info->ref = *(data + read_byte);
                        concat_info->total_seg = *(data + read_byte + 1);
                        concat_info->seg = *(data + read_byte + 2);
                    }
                }
                break;

         /*----------------------------------------------- 
          * Concatenated short message , 16-bit reference 
          *-----------------------------------------------*/
            case EAT_SMSAL_CONC16_MSG_IEI:

                if (concat_info != NULL)
                {
                    /* seg = 0 is invalid, we shall ignore this IE */
                    if ((*(data + read_byte + 3)) > 0)
                    {
                        concat_info->ref = *(data + read_byte) * 256 + *(data + read_byte + 1);
                        concat_info->total_seg = *(data + read_byte + 2);
                        concat_info->seg = *(data + read_byte + 3);
                    }
                }

                break;

         /*---------------------------------------------- 
          * Special SMS Message Indication 
          *----------------------------------------------*/
            case EAT_SMSAL_SPECIAL_MSG_IND_IEI:
                if (msg_wait != NULL)
                {
                    msg_wait->type_of_info |= EAT_SMSAL_MSG_WAIT_UDH;
                    msg_wait->is_msg_wait = EAT_TRUE;
                }                       /* if(msg_wait != NULL) */

                break;

#ifdef __SMS_R6_ENHANCED_VOICE_MAIL__
         /*---------------------------------------------- 
          * Enhanced Voice Mail Information
          *----------------------------------------------*/
            case EAT_SMSAL_ENHANCED_VM_INFO_IEI:
                if (msg_wait != NULL)
                {
                    // decode and get result
                    // kal_mem_set(&(msg_wait->evmi), 0, sizeof(smsal_evmi_struct));
                    ind_num = 0;//smsal_decode_evmi(data+read_byte-2, (kal_uint16)(ie_len+2), (smsal_evmi_struct *)&(msg_wait->evmi));
                        
                    if( ind_num != 0 )
                    {
                        msg_wait->type_of_info |= EAT_SMSAL_MSG_WAIT_UDH_EVM;
                        msg_wait->is_msg_wait = EAT_TRUE;
                    }
                }                       /* if(msg_wait != NULL) */

                break;
#endif

         /*---------------------------------------------- 
          * Application Port 
          *----------------------------------------------*/
            case EAT_SMSAL_APP_PORT8_IEI:
                if (port != NULL)
                {
                    port->dest_port = *(data + read_byte);
                    port->src_port = *(data + read_byte + 1);
                }
                break;

            case EAT_SMSAL_APP_PORT16_IEI:
                if (port != NULL)
                {
                    port->dest_port = *(data + read_byte) * 256 + *(data + read_byte + 1);
                    port->src_port = *(data + read_byte + 2) * 256 + *(data + read_byte + 3);
                }
                break;

            default:
                break;
        }                               /* switch */

        /* illegal PDU */
        if ((read_byte + ie_len) > EAT_SMSAL_ONE_MSG_OCTET)
            return 0;

        read_byte += ie_len;

    }                                   /* while */

#ifndef __REL4__
    if (msg_wait != NULL)
    {
        msg_wait->msp = 0;
    }
#endif

    return udhl;
}   

/*****************************************************************************
* FUNCTION
*  smsal_decode_tpdu
* DESCRIPTION
*   This function decodes the TPDU and update the entry of message box.
*
* PARAMETERS
*  a  IN/OUT   tpdu_decode_ptr, store the decoded results
*  b  IN       data, pointer to TPDU
*  c  IN       len, length of TPDU
*
*    Octet:  0                   len-1 
*            +--------------------+
*            |        TPDU        |
*            +--------------------+
* RETURNS
*  KAL_TRUE:  success
*  KAL_FALSE: fail
* GLOBALS AFFECTED
*  none
*****************************************************************************/
eat_bool eat_smsal_decode_tpdu(eat_smsal_tpdu_decode_struct *tpdu_decode_ptr,                          
                           u8                *data,
                           u8                len)
{
    u8 addr_len = 0;
    u8 msg_offset = 0;

    if(tpdu_decode_ptr == NULL)
    {
        eat_trace("eat_smsal_decode_tpdu(),failed");
        return EAT_FALSE;
    }
    //ASSERT(tpdu_decode_ptr != NULL);

    tpdu_decode_ptr->mti = (eat_smsal_mti_enum) (EAT_SMSAL_MTI_BITS & (*data));

    if (tpdu_decode_ptr->mti == EAT_AT_SMSAL_MTI_UNSPECIFIED)
    {
        tpdu_decode_ptr->mti = EAT_AT_SMSAL_MTI_DELIVER;
    }

    // set default
    tpdu_decode_ptr->msg_wait.msp = 0;

    /* Get Message Type */
    switch (tpdu_decode_ptr->mti)
    {
        case EAT_AT_SMSAL_MTI_DELIVER:
        {
            eat_smsal_deliver_peer_struct *deliver_ptr;

            deliver_ptr = &tpdu_decode_ptr->data.deliver_tpdu;
            if (deliver_ptr->user_data_size == 0)
            {
                eat_smsal_deliver_peer_struct_alloc((void *)deliver_ptr);

                /* Unpack the pdu to access the message contents */
                eat_smsal_deliver_peer_struct_unpack((void *)deliver_ptr, data, 0, len, NULL);
            }

            tpdu_decode_ptr->fo = *data;
            addr_len = (deliver_ptr->orig_addr[0] + 1) / 2;

            /* offset : fo, pid, dcs, scts, udl, oa_len, oa_type, oa */
            msg_offset += (EAT_PART_OF_DELIVER_HEADER_LEN + addr_len);

            /*
             * NOTE that decoding DCS shall precedes decoding UDH,
             * * because message waiting in UDH shall override the info in DCS 
             * * if there is conflict 
             */
            eat_smsal_decode_dcs(deliver_ptr->dcs,
                             &tpdu_decode_ptr->alphabet_type,
                             &tpdu_decode_ptr->msg_class, 
                             &tpdu_decode_ptr->is_compress,
                             &tpdu_decode_ptr->msg_wait);

            if ((deliver_ptr->udh_p == EAT_FALSE) &&
                (*(deliver_ptr->user_data) <= (deliver_ptr->no_user_data - 1)))
            {
                deliver_ptr->udh_p = eat_smsal_detect_udh(deliver_ptr->user_data);
            }

            if (deliver_ptr->udh_p == EAT_TRUE)
            {
                /*
                 * Decode User Data Header, 
                 * * get the concatenated infomation if present 
                 */

                /* 
                 * if any following condition meet, this UDH is invalid:               
                 *  UDHL > no. bytes of user data - 1
                 */
                if (*(deliver_ptr->user_data) <= (deliver_ptr->no_user_data - 1))
                {
                    tpdu_decode_ptr->udhl = eat_smsal_decode_udh(deliver_ptr->user_data,
                                                             &tpdu_decode_ptr->concat_info,
                                                             &tpdu_decode_ptr->msg_wait,
                                                             &tpdu_decode_ptr->port);

                    if (tpdu_decode_ptr->udhl > 0)
                        tpdu_decode_ptr->udhl += 1;     /* udhl + udh */
                }
                else
                {
                    /* invalid UDH, set message to null */
                    deliver_ptr->udh_p = 0;
                    /* keep message content and send to MMI */
                    /* deliver_ptr->user_data_len = 0; */
                    /* deliver_ptr->no_user_data  = 0; */
                }
            }

#if defined (__CPHS__)
            /* 
             * check whether this message is a voice mail, 
             * check following by CPHS:
             * 1) length of TP-OA = 4
             * 2) type of TP-OA = 1 101 0000 
             * 3) 01111110 (7e)
             *                 |set/clear
             *     x001000x
             * 4) 00111111 (3f)
             *     xx000000
             */
            if ((deliver_ptr->orig_addr[0] == 4) &&
                (deliver_ptr->orig_addr[1] == EAT_SMSAL_CPHS_VM_ADDR_TYPE) &&
                ((deliver_ptr->orig_addr[2] & 0x7e) == 0x10) &&
                ((deliver_ptr->orig_addr[3] & 0x3f) == 0x00))
            {
                tpdu_decode_ptr->msg_wait.type_of_info |= EAT_SMSAL_MSG_WAIT_CPHS;
                tpdu_decode_ptr->msg_wait.is_msg_wait = EAT_TRUE;
            }
#endif

            tpdu_decode_ptr->msg_len = deliver_ptr->no_user_data;

            break;
        }
        
        default:                       /* wrong message type */

            /*
             * in this case , the message type is reserved,
             * * we shall ignore this message but do not delete it 
             */
            break;

    }                                   /* switch (mti) */

    tpdu_decode_ptr->offset = msg_offset;

    return EAT_TRUE;

}    

/*****************************************************************************
* FUNCTION
*  smsal_decode_sms_pdu
* DESCRIPTION
*   This function decodes the short message read from SIM/NVM,
*   update the entry of message box.
*
* PARAMETERS
*  a  IN       data[]
*
*    Octet:  0         1        X       X+1       Y               174
*            +----------+-------+-----------------------------------+
*            |  SCA LEN | TOSCA |   SCA   |          TPDU           |
*            +----------+-------+-----------------------------------+
*
*  b  IN       length (must equals to 175)
*  c  IN/OUT   sms_pdu, contain the decoded result
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
eat_bool eat_decode_sms_pdu(u8               *pdu, 
                              u8               pdu_len,
                              eat_smsal_pdu_decode_struct *sms_pdu)
{
    eat_bool retval;

    if((pdu == NULL) || (pdu_len == 0) || (sms_pdu == NULL))
    {
        eat_trace("eat_decode_sms_pdu(),failed");
        return EAT_FALSE;
    }
    //ASSERT((pdu != NULL) && (pdu_len != 0) && (sms_pdu != NULL));

    sms_pdu->pdu_len = 0;
    sms_pdu->tpdu_len = 0;

   /*----------------
    * Get SC address 
    *----------------*/
    /* length exceed max sc address length */
    if(*pdu > EAT_SMSAL_ADDR_LEN)
    {
        sms_pdu->tpdu.mti = EAT_AT_SMSAL_MTI_ILLEGAL;
        return EAT_FALSE;
    }

    sms_pdu->sca.addr_length = *pdu;

    if (*pdu > 0)
    {
        memcpy(sms_pdu->sca.addr_bcd, (pdu + 1), sms_pdu->sca.addr_length);
    }
    sms_pdu->pdu_len += (1 + *pdu);     /* for sca length */

    eat_smsal_reset_tpdu_decode_struct(&(sms_pdu->tpdu));

    /* reset user_data_size */
    sms_pdu->tpdu.mti = (eat_smsal_mti_enum) (*(pdu + 1 + sms_pdu->sca.addr_length) & EAT_SMSAL_MTI_BITS);

    if (sms_pdu->tpdu.mti == EAT_AT_SMSAL_MTI_UNSPECIFIED)
    {
        sms_pdu->tpdu.mti = EAT_AT_SMSAL_MTI_DELIVER;
    }
    
    if (sms_pdu->tpdu.mti == EAT_AT_SMSAL_MTI_DELIVER)
        sms_pdu->tpdu.data.deliver_tpdu.user_data_size = 0;
    else if (sms_pdu->tpdu.mti == EAT_AT_SMSAL_MTI_SUBMIT)
        sms_pdu->tpdu.data.submit_tpdu.user_data_size = 0;

    if ((retval = eat_smsal_decode_tpdu(&(sms_pdu->tpdu),
                                    (pdu + 1 + sms_pdu->sca.addr_length),     /* exclude sca */
                                    (u8) (pdu_len - 1 - sms_pdu->sca.addr_length)))
                                    == EAT_TRUE)
    {
        sms_pdu->tpdu_len = sms_pdu->tpdu.offset + sms_pdu->tpdu.msg_len;
        sms_pdu->pdu_len += sms_pdu->tpdu_len;
        sms_pdu->tpdu.offset += (1 + *pdu);     /* for sca */
    }

    if (sms_pdu->tpdu.msg_wait.is_msg_wait == EAT_TRUE)
    {
        eat_trace("eat_decode_sms_pdu,is_msg_wait is true!!! ");
        //smsal_message_waiting_handler(sms_pdu);
    }
    
    return retval;

}  

void eat_smsal_deliver_peer_struct_dealloc(void *mtk_d)
{
    eat_smsal_deliver_peer_struct *s = (eat_smsal_deliver_peer_struct *) mtk_d;

    if (s->orig_addr != NULL)
    {
        eat_mem_free (s->orig_addr);
        s->orig_addr = 0;
    }

    if (s->user_data != NULL)
    {
        eat_mem_free (s->user_data);
        s->user_data = 0;
        s->user_data_size = 0;
    }
}

void eat_sms_free_tpdu_decode_struct(eat_smsal_tpdu_decode_struct *tpdu_decode_ptr)
{
    if (tpdu_decode_ptr->mti == EAT_AT_SMSAL_MTI_DELIVER)
    {
        /* Even enable MCD, this function shall be called !! */
        eat_smsal_deliver_peer_struct_dealloc((void *)&tpdu_decode_ptr->data.deliver_tpdu);
    }
}
