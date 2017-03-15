#if !defined (__EAT_AUDIO_H__)
#define __EAT_AUDIO_H__

#include "eat_type.h"

typedef enum {
   EAT_TONE_DTMF_0, /* select out path by AT+CHFA, sound data: {  { 941, 1336, 0,  0,   0 }   } */
   EAT_TONE_DTMF_1, /* select out path by AT+CHFA, sound data: {  { 697, 1209, 0,  0,   0 }   } */
   EAT_TONE_DTMF_2, /* select out path by AT+CHFA, sound data: {  { 697, 1336, 0,  0,   0 }   } */
   EAT_TONE_DTMF_3, /* select out path by AT+CHFA, sound data: {  { 697, 1477, 0,  0,   0 }   } */
   EAT_TONE_DTMF_4, /* select out path by AT+CHFA, sound data: {  { 770, 1209, 0,  0,   0 }   } */
   EAT_TONE_DTMF_5, /* select out path by AT+CHFA, sound data: {  { 770, 1336, 0,  0,   0 }   } */
   EAT_TONE_DTMF_6, /* select out path by AT+CHFA, sound data: {  { 770, 1477, 0,  0,   0 }   } */
   EAT_TONE_DTMF_7, /* select out path by AT+CHFA, sound data: {  { 852, 1209, 0,  0,   0 }   } */
   EAT_TONE_DTMF_8, /* select out path by AT+CHFA, sound data: {  { 852, 1336, 0,  0,   0 }   } */
   EAT_TONE_DTMF_9, /* select out path by AT+CHFA, sound data: {  { 852, 1477, 0,  0,   0 }   } */
   EAT_TONE_KEY_STAR, /* select out path by AT+CHFA, sound data: {  { 941, 1209, 0,  0,   0 }   } */
   EAT_TONE_KEY_HASH, /* select out path by AT+CHFA, sound data: {  { 941, 1477, 0,  0,   0 }   } */
   EAT_TONE_KEY_NORMAL, /* select out path by AT+CHFA, sound data: {  { 1200,  0, 100,  0,   0 }   } */
   EAT_TONE_DTMF_A,  /* sound data: {{697, 1633, 0, 0, 0}} */	
   EAT_TONE_DTMF_B,  /* sound data: {{770, 1633, 0, 0, 0}} */
   EAT_TONE_DTMF_C,  /* sound data: {{852, 1633, 0, 0, 0}} */
   EAT_TONE_DTMF_D,  /* sound data: {{941, 1633, 0, 0, 0}} */
   EAT_TONE_DTMF_E, /* sound data: {{1400, 0, 0, 0, 0}} */
   EAT_TONE_DTMF_F,  /* sound data: {{2300, 0, 0, 0, 0}} */
   EAT_TONE_CUSTOM_DTMF,  
   EAT_TONE_KEY_CLICK,  /* sound data: {  { 1500, 0, 200,  0,   0 }   } */
   EAT_TONE_CALL_CONNECT,  /* sound data:  {  { 700, 0, 200,  0,   0 }   }*/
   EAT_TONE_MT_SMS_IN_CALL,    /* sound data:  {  { 852, 1209, 200,  0,   0 }   }*/
   EAT_TONE_CALL_TIME_REMINDER,    /* sound data:  {  { 852, 0, 200,  0,   0 }   }*/
   EAT_TONE_DIAL_CALL_GSM,    /* sound data:  {  { 425,   0,   0,   0,   0 }   }*/
   EAT_TONE_DIAL_CALL_PCS,    /* sound data:  {  { 350, 440,   0,   0,   0 }   }*/
   EAT_TONE_BUSY_CALL_GSM,    /* sound data:  {  { 425,   0, 500, 500,   0 }   }*/
   EAT_TONE_BUSY_CALL_PCS,    /* sound data:  {  { 480, 620, 500, 500,   0 }   }*/
   EAT_TONE_CONGESTION_GSM,     /* sound data:  {  { 425,   0, 200, 200,   0 }   }*/
   EAT_TONE_CONGESTION_PCS,    /* sound data:  {  { 480, 620, 200, 200,   0 }   }*/
   EAT_TONE_RADIO_PATH,    /* sound data:  {  { 425,   0, 200,   0,   0 }   }*/
   EAT_TONE_RADIO_NA,    /* sound data:  {  { 425,   0, 200, 200,   1 },
                                                                  { 425,   0, 200, 200,   2 },
                                                                  { 425,   0, 200,   0,   0 }   }*/
   EAT_TONE_ERROR_INFO,    /* sound data:  {  { 950,   0, 330,1000,   0 }   }*/
   EAT_TONE_NUM_UNOBTAIN,    /* sound data:  {  {1400,   0, 330, 0,   0 }   }*/
   EAT_TONE_AUTH_ERROR,    /* sound data:  {  { 950,   0, 330,1000,   0 }   }*/
   EAT_TONE_CALL_WAITING,    /* sound data:  {  { 425,   0, 200, 600,   1 },
                                                                     { 425,   0, 200,3000,   2 },
                                                                     { 425,   0, 200, 600,   3 },
                                                                     { 425,   0, 200,   0,   0 }   }*/
   EAT_TONE_RINGING_CALL_GSM,    /* sound data:  {  { 425,   0, 2000, 1000,   0 }   }*/
   EAT_TONE_RINGING_CALL_PCS,    /* sound data:  {  { 425,   0, 2000, 4000,   0 }   }*/
   EAT_TONE_POSITIVE_ACK,    /* sound data:  {  { 600,   0, 200, 100,   1 },
                                                                     { 800,   0, 200, 100,   2 },
                                                                     { 1000,   0, 200,   0,   0 }   }*/
   EAT_TONE_NEGATIVE_ACK,       /* sound data:  {  { 600,   0, 300, 100,   1 },
                                                                     { 300,   0, 700,   0,   0 }   }*/
   EAT_TONE_DIAL_CALL_GSM_STK,    /* sound data:  {  { 425,   0,   0,   0,   0 }   }*/
   EAT_TONE_DIAL_CALL_PCS_STK,    /* sound data:  {  { 350, 440,   0,   0,   0 }   }*/
   EAT_TONE_BUSY_CALL_GSM_STK,    /* sound data:  {  { 425,   0, 500, 500,   0 }   }*/
   EAT_TONE_BUSY_CALL_PCS_STK,    /* sound data:  {  { 480, 620, 500, 500,   0 }   }*/
   EAT_TONE_CONGESTION_GSM_STK,    /* sound data:  {  { 425,   0, 200, 200,   0 }   }*/
   EAT_TONE_CONGESTION_PCS_STK,    /* sound data:  {  { 480, 620, 200, 200,   0 }   }*/
   EAT_TONE_RADIO_PATH_STK,    /* sound data:  {  { 425,   0, 200,   0,   0 }   }*/
   EAT_TONE_RADIO_NA_STK,    /* sound data:  {  { 425,   0, 200, 200,   1 },
                                                                  { 425,   0, 200, 200,   2 },
                                                                  { 425,   0, 200,   0,   0 }   }*/
   EAT_TONE_ERROR_INFO_STK,    /* sound data:  {  { 950,   0, 330,1000,   0 }   }*/
   EAT_TONE_NUM_UNOBTAIN_STK,    /* sound data:  {  {1400,   0, 330, 0,   0 }   }*/
   EAT_TONE_AUTH_ERROR_STK,    /* sound data:  {  { 950,   0, 330,1000,   0 }   }*/
   EAT_TONE_CALL_WAITING_STK,    /* sound data:  {  { 425,   0, 200, 600,   1 },
                                                                     { 425,   0, 200,3000,   2 },
                                                                     { 425,   0, 200, 600,   3 },
                                                                     { 425,   0, 200,   0,   0 }   }*/
   EAT_TONE_RINGING_CALL_GSM_STK,    /* sound data:  {  { 425,   0, 2000, 1000,   0 }   }*/
   EAT_TONE_RINGING_CALL_PCS_STK,    /* sound data:  {  { 425,   0, 2000, 4000,   0 }   }*/
   EAT_TONE_POSITIVE_ACK_STK,    /* sound data:  {  { 600,   0, 200, 100,   1 },
                                                                     { 800,   0, 200, 100,   2 },
                                                                     { 1000,   0, 200,   0,   0 }   }*/
   EAT_TONE_NEGATIVE_ACK_STK,    /* sound data:  {  { 600,   0, 300, 100,   1 },
                                                                     { 300,   0, 700,   0,   0 }   }*/
   EAT_TONE_WARNING1,    /* sound data:  {  { 852, 0, 200,  0,   0 }   }*/
   EAT_TONE_WARNING2,    /* sound data:  {  { 852, 0, 200,  0,   0 }   }*/
   EAT_TONE_ERROR1,    /* sound data:  {  { 1209, 0, 200,  0,   0 }   }*/
   EAT_TONE_ERROR2,    /* sound data:  {  { 1209, 0, 200,  0,   0 }   }*/
   EAT_TONE_GENERAL_BEEP,    /* sound data:  {  { 700, 0, 200,  0,   0 }   }*/
   EAT_TONE_BATTERY_LOW,    /* sound data: {  { 1100,   0, 300, 100,   1 },
                                                                     { 900,   0, 300,   0,   0 }   } */
   EAT_TONE_BATTERY_WARNING,    /* sound data: {  { 1400,   0, 300, 100,   1 },
                                                                     { 1400,   0, 300, 100,   2 },
                                                                     { 1400,   0, 300,   0,   0 }   } */
   EAT_TONE_AUX_IND,    /* sound data:  {  { 700, 0, 200,  0,   0 }   }*/
   EAT_TONE_DATA_CALL,    /* sound data:  {  { 425,   0, 2000, 1000,   0 }   }*/
   EAT_MAX_TONE_ID
} EatAudioToneId_enum;

/* audio play style */
typedef enum {
    EAT_AUDIO_PLAY_INFINITE       ,  /* Play sound for infinite. */
    EAT_AUDIO_PLAY_ONCE           ,  /* Play sound for once. */
}EatAudioStyle_enum;


/* audio out path */
typedef enum {
    EAT_AUDIO_PATH_SPK1, /* out from 22/23PIN SPK1P/SPK1N */ 
    EAT_AUDIO_PATH_SPK2  /*  out from 24/25PIN SPK2P/SPK2N */ 
} EatAudioPath_enum;

/* the format of audio string */
typedef enum {
    EAT_AUDIO_FORMAT_MIDI, /* sound data of midi format*/
    EAT_AUDIO_FORMAT_WAV, /* sound data of wav format*/    
    EAT_AUDIO_FORMAT_AMR,  /* sound data of amr format*/    
    EAT_AUDIO_FORMAT_MP3  /* sound data of MP3 format*/
} EatAudioFormat_enum;

/* the data struct of tone id  */
typedef struct {
    unsigned int freq1;            /* First frequency                                    */
    unsigned int freq2;            /* Second frequency                                   */
    unsigned int on_duration;      /* Tone on duation, in ms unit, 0 for continuous tone */
    unsigned int off_duration;     /* Tone off duation, in ms unit, 0 for end of playing */
    unsigned char next_operation;   /* Index of the next tone                             */
} EatAudioToneData_st;

typedef enum
{
    EAT_AUDIO_PLAY_LOCAL,   
    EAT_AUDIO_PLAY_REMOTE,  
    EAT_AUDIO_PLAY_BOTH   
}eat_audio_play_mode;

/* the audio max volume */
#define EAT_AUDIO_VOL_MAX_LEVEL 15

/* this value MUST NOT LARGER than 20*/
#define EAT_AUDIO_TONE_MAX 20

/* the data struct of tone id by custom */
typedef struct {
    EatAudioToneData_st tone[EAT_AUDIO_TONE_MAX];
} EatAudioToneTable_st;

#define MED_AUDIO_SUCCESS               0

#define MED_EAT_PARA_ERR                -1      

#define MED_AUDIO_FAIL                  -9001
#define MED_AUDIO_BUSY                  -9002
#define MED_AUDIO_DISC_FULL             -9003
#define MED_AUDIO_OPEN_FILE_FAIL        -9004
#define MED_AUDIO_END_OF_FILE           -9005
#define MED_AUDIO_TERMINATED            -9006   
#define MED_AUDIO_BAD_FORMAT            -9007  
#define MED_AUDIO_INVALID_FORMAT        -9008   
#define MED_AUDIO_ERROR                 -9009
#define MED_AUDIO_NO_DISC               -9010
#define MED_AUDIO_NO_SPACE              -9011   
#define MED_AUDIO_INVALID_HANDLE        -9012   
#define MED_AUDIO_NO_HANDLE             -9013   
#define MED_AUDIO_RESUME                -9014   
#define MED_AUDIO_BLOCKED               -9015  
#define MED_AUDIO_MEM_INSUFFICIENT      -9016
#define MED_AUDIO_BUFFER_INSUFFICIENT   -9017
#define MED_AUDIO_FILE_EXIST            -9018   
#define MED_AUDIO_WRITE_PROTECTION      -9019
#define MED_AUDIO_PARAM_ERROR           -9020
#define MED_AUDIO_UNSUPPORTED_CHANNEL   -9021
#define MED_AUDIO_UNSUPPORTED_FREQ      -9022
#define MED_AUDIO_UNSUPPORTED_TYPE      -9023
#define MED_AUDIO_UNSUPPORTED_OPERATION -9024
#define MED_AUDIO_PARSER_ERROR          -9025

#define MED_AUDIO_AUDIO_ERROR           -9027
#define MED_AUDIO_MP4_NO_AUDIO_TRACK    -9032
#define MED_AUDIO_STOP_FM_RECORD        -9065   
#define MED_AUDIO_UNSUPPORTED_SPEED     -9066
#define MED_AUDIO_DECODER_NOT_SUPPORT   -9101
#define MED_AUDIO_DEMO_END              -9116
#define MED_AUDIO_HFP_SCO_CONNECTED     -9200  
#define MED_AUDIO_DRM_PROHIBIT          -9201   
#define MED_AUDIO_DRM_TIMEOUT           -9202  

typedef s32 eat_med_result;
typedef void (*eat_med_callback) (eat_med_result result);

/*****************************************************************************
* Function:  eat_audio_play_tone_id
 * Description
 *    This function is used to play key tone sound and audio tone sound.
 * Parameters
 *   id[in] the kind of the tone.
 *   style[in]   play only one time or alway play until stop. 
 *         but whether it play once or infinitely is deceided by style and the on_duration member of the sound.
 *         1. the key tone (the tone id is less than EAT_TONE_KEY_CLICK)is played by aud_keytone_play, the style is useful, 
 *         but the sound data's on_duration IS 0, so these key tones play infinitely.
 *         2. audio tone sound(the tone id id larger or equal than EAT_TONE_KEY_CLICK)is played by aud_tone_play, the style is not useful.
 *         the style is decided by sound data: the on_duaration is NOT 0 and off_duartion is 0, the sound play only once,or it will play infinitely.
 *   vol[in]    the tone volume, the rang is 0-15 (EAT_AUDIO_VOL_MAX_LEVEL)
 *   path[in]   the out device of the tone sound.
 *         1. EAT_AUDIO_PATH_SPK1: SPK1 at 22/23PIN SPK1P/SPK1N 
 *         2. EAT_AUDIO_PATH_SPK2: the SPK2 at 24/25PIN SPK2P/SPK2N 
 * Returns
 *         EAT_TRUE: Sucess to send the msg to mod_med,otherwise EAT_FALSE.
 * attention
 *         1. the priority of audio tone (id > EAT_TONE_KEY_CLICK ) is higher than the key tone(id <=  EAT_TONE_KEY_CLICK ). 
 *            for example: 
 *             when it is playing key tone id1, now call the eat_audio_play_tone_id by tone id2 : 
 *             if id1 is key tone (id1<=EAT_TONE_KEY_CLICK) and d2 is key tone (id2<=EAT_TONE_KEY_CLICK) the tone id1 will stop and play tone id2; 
 *             if id1 is key tone (id1<=EAT_TONE_KEY_CLICK ) and id2 is audio tone(id2>EAT_TONE_KEY_CLICK), the tone id1 also will stop and play tone id2. 
 *             if id1 is audio tone (id1>EAT_TONE_KEY_CLICK ) and id2 is key tone(id2>EAT_TONE_KEY_CLICK), the tone id1 also will keep PLAYING and NOT play tone id2 . 
 *             if id1 is audio tone (id1>EAT_TONE_KEY_CLICK ) and id2 is audio tone(id2>EAT_TONE_KEY_CLICK), the tone id1 also will keep PLAYING and NOT play tone id2. 
 *         2. if call  eat_audio_play_tone_id by tone id1, and eat_audio_stop_tone_id by id2:
 *             only when id1 and id2 is same kind of tone(both key tone or both audio tone),the tone will stop. for example: 
 *             if id1 is key tone (id1<=EAT_TONE_KEY_CLICK) and d2 is key tone (id2<=EAT_TONE_KEY_CLICK) the tone id1 will stop; 
 *             if id1 is audio tone (id1>EAT_TONE_KEY_CLICK ) and id2 is audio tone(id2>EAT_TONE_KEY_CLICK), the tone id1 also will stop . 
 *             other case: the tone will not stop. 
 *             PLEASE keep the play id and stop id is same, or maybe the tone will not be stopped. 
 *         3. the midi and wav sound played by eat_audio_play_data will stop the tone sound(include key tone and audio tone). 
 *         4. Call will also stop the tone sound. 
    
*****************************************************************************/
extern eat_bool (* const eat_audio_play_tone_id)(EatAudioToneId_enum id, EatAudioStyle_enum style, unsigned char vol, EatAudioPath_enum path);


/*****************************************************************************
* Function :  eat_audio_play_tone_id
 * Description
 *    Stops tone sound.
 * Parameters
 *   id[in]  the kind of the tone
 * Returns
 *         EAT_TRUE: Sucess to send the msg to mod_med,otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_audio_stop_tone_id)(EatAudioToneId_enum id);


/*****************************************************************************
* Function :  eat_audio_play_data
 * Description
 *    Plays midi/wav sound.
 * Parameters
 *   audio_data[in]  audio data
 *   len[in]  the len of the audio data
 *   format[in]  the midi or wav format 
 *             only suppport the midi and wav format sound.
 *   style[in]  play only one time or alway play until stop.
 *   vol[in]  the tone volume, the rang is 0-15(EAT_AUDIO_VOL_MAX_LEVEL)
 *   path[in]  the out device of the tone sound
 *             1. EAT_AUDIO_PATH_SPK1: the sound out from the SPK1(22/23PIN SPK1P/SPK1N )
 *             2. EAT_AUDIO_PATH_SPK2: the sound out from the SPK2(24/25PIN SPK2P/SPK2N )
 * Returns
 *         EAT_TRUE: Sucess to send the msg to mod_med,otherwise EAT_FALSE.
 * attention
 *     1. During it is playing audio_string1,if call eat_audio_play_data by audio_string2,
 *         it will play the audio_string2 and stop audio_string1. 
 *     2. At one time, only one kind audio string is playing.
 *     3. During call,  audio sound will not be played by this function; 
 *     4. During play audio sound, the incoming call will stop the audio sound.  
*****************************************************************************/

extern eat_bool (* const eat_audio_play_data)(const void *audio_data, unsigned int len, EatAudioFormat_enum format, EatAudioStyle_enum style, unsigned char vol, EatAudioPath_enum path);

/*****************************************************************************
* Function :  eat_audio_stop_data
 * Description
 *    Stops the audio sound.
 * Parameters
 *   NONE 
 * Returns
 *         EAT_TRUE: Sucess to send the msg to mod_med,otherwise EAT_FALSE.
 * attention
 *     1. it will stop the current playing audio string. at one time, only one kind audio string is playing. 
*****************************************************************************/
extern eat_bool (* const eat_audio_stop_data)(void);

/*****************************************************************************
* Function :  eat_audio_set_custom_tone
 * Description
 *    Set custom tone.
 * Parameters
 * param[in,out]  tone data
 * Returns
 *         if the tone is NULL, return EAT_FALSE, or  EAT_TRUE.
 * attention
 *   1. for example:  
 *        EatAudioToneTable_st tone =   
 *            {   300,   0,   200, 600,   1,   
 *                 2000,   0, 200, 600,   2,    
 * ......   
 *                  1100, 1200, 500, 10, 0  
 *           };  
 *    or  
 *        EatAudioToneTable_st tone =  
 *            {   
 *                  1100, 1200, 500, 10, 0    
 *           };   
 *   2. the max number of the ton sequence is EAT_AUDIO_TONE_MAX and cann't larger than 20(EAT_AUDIO_TONE_MAX).  
 *   3. the next_operation member of EatAudioToneTable_st must be 1,2,3,4,....0; the last one is 0 .  
 *   4. if one of the tone's off_duration  is 0, the tone sound will stop at this sequence even if this next_operation is not 0   
 *       and the later of this sequence will not be played.   
 *   5. after call eat_audio_set_custom_tone then must call eat_audio_play_tone_id and the first param of tone id must be  EAT_TONE_CUSTOM_DTMF. 
*****************************************************************************/
 
extern eat_bool (* const eat_audio_set_custom_tone)(const EatAudioToneTable_st * tone);

/*****************************************************************************
 * Function :   eat_audio_play_file
 * Description
 *    Start to play audio file.
 * Parameters
 *   filename[in]  File full name, unicode format.The max len is 128 bytes.
 *   repeat  [in]  EAT_TRUE:repeat EAT_FALSE:once
 *   handler [in]  Callback function.Please refer to the attention.
 *                 It will be called,if playback finishes or 
 *                 is interrupted by stop event.
 *                 if play sucessful,handler will get MED_AUDIO_END_OF_FILE or MED_AUDIO_SUCCESS(in call);
 *   volume  [in]  0~100
 *   outputpath  [in]  the output device of the sound.
 *             1. EAT_AUDIO_PATH_SPK1: the sound in from the SPK1(SPK1P/SPK1N )
 *             2. EAT_AUDIO_PATH_SPK2: the sound in from the SPK2(SPK2P/SPK2N )
 * Returns
 *     MED_AUDIO_SUCCESS if sucessful. 
 * attention
 *   1.The callback function parameter:
 *       MED_AUDIO_END_OF_FILE: play normal finish.
 *       MED_AUDIO_SUCCESS: play normal finish(in call)
 *       MED_AUDIO_TERMINATED: Be interrupted by other events, include execute "eat_audio_stop_file".
 *   2.Auto stop playing,if CALL event occurs.
 *   3.The max file fullname len is 128 bytes.
 *   4.If handler is NULL,will receive  EAT_EVENT_AUD_PLAY_FINISH_IND(not in call) or EAT_EVENT_SND_PLAY_FINISH_IND(in call) message when play finished.
 *****************************************************************************/
extern eat_med_result (*const eat_audio_play_file)(unsigned short* filename, 
        eat_bool repeat, 
        eat_med_callback handler, 
        unsigned char volume,
        EatAudioPath_enum outputpath );
/*****************************************************************************
 * Function :  eat_audio_stop_file
 * Description
 *    Stop audio file playing.
 * Parameters
 *    void
 * Returns
 *     EAT_TRUE if sucessful. 
 *****************************************************************************/
extern eat_bool (*const eat_audio_stop_file)(void);

/*****************************************************************************
 * Function :  eat_audio_set_play_mode_in_call
 * Description
 *    Set audio play mode,takes effect only in call. if not in call, only play locally no matter what the mode is. Setting takes effect before play.
 * Parameters
 *    simcom_audio_play_mode:
 *      SIMCOM_AUDIO_PLAY_LOCAL,  
 *      SIMCOM_AUDIO_PLAY_REMOTE,  
 *      SIMCOM_AUDIO_PLAY_BOTH,
 * Returns
 *     EAT_TRUE if sucessful. 
 *****************************************************************************/
extern eat_bool (*const eat_audio_set_play_mode_in_call)(eat_audio_play_mode mode);

/*****************************************************************************
 * Function :  eat_audio_set_play_mode_in_call
 * Description
 *    Get audio play mode
 * Parameters
 *    void
 * Returns
 *     Current play mode: 
 *     simcom_audio_play_mode 
 *****************************************************************************/
extern eat_audio_play_mode (*const eat_audio_get_play_mode_in_call)(void);
#endif
