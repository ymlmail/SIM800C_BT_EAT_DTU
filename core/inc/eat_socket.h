/*
 * =====================================================================================
 *
 *       Filename:  eat_socket.h
 *
 *    Description:  the head file of eat socket function
 *
 *        Version:  1.0
 *        Created:  2013-12-12 14:49:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jumping (apple), zhangping@sim.com
 *   Organization:  www.sim.com
 *
 * =====================================================================================
 */
#ifndef __EAT_SOCKET_H__

#define __EAT_SOCKET_H__



/* #####   HEADER FILE INCLUDES   ################################################### */

/* #####   EXPORTED MACROS   ######################################################## */
#define MAX_SOCK_ADDR_LEN (28)                  /* max socket address length  */
#define MAX_IP_SOCKET_NUM (8)                   /* Max IP socket number */

//Control message protocol. Currently, only allow applicaion to set this type when RAW_SOCKET is used 
#define SOC_IPPROTO_ICMP  (1)

#define SOC_IPPROTO_RAW   (255)                 /* raw IP packet  */

/* #####   EXPORTED TYPE DEFINITIONS   ############################################## */

/* Bearer state */
typedef enum
{
    CBM_DEACTIVATED             = 0x01, /* deactivated */
    CBM_ACTIVATING              = 0x02, /* activating */
    CBM_ACTIVATED               = 0x04, /* activated */
    CBM_DEACTIVATING            = 0x08, /* deactivating */
    CBM_CSD_AUTO_DISC_TIMEOUT   = 0x10, /* csd auto disconnection timeout */
    CBM_GPRS_AUTO_DISC_TIMEOUT  = 0x20, /* gprs auto disconnection timeout */
    CBM_NWK_NEG_QOS_MODIFY      = 0x040, /* negotiated network qos modify notification */
    CBM_WIFI_STA_INFO_MODIFY      = 0x080, /* wifi hot spot sta number is changed */
    CBM_BEARER_STATE_TOTAL
} cbm_bearer_state_enum;
/* Socket return codes, negative values stand for errors */
typedef enum
{
    SOC_SUCCESS           = 0,     /* success */
    SOC_ERROR             = -1,    /* error */
    SOC_WOULDBLOCK        = -2,    /* not done yet */
    SOC_LIMIT_RESOURCE    = -3,    /* limited resource */
    SOC_INVALID_SOCKET    = -4,    /* invalid socket */
    SOC_INVALID_ACCOUNT   = -5,    /* invalid account id */
    SOC_NAMETOOLONG       = -6,    /* address too long */
    SOC_ALREADY           = -7,    /* operation already in progress */
    SOC_OPNOTSUPP         = -8,    /* operation not support */
    SOC_CONNABORTED       = -9,    /* Software caused connection abort */
    SOC_INVAL             = -10,   /* invalid argument */
    SOC_PIPE              = -11,   /* broken pipe */
    SOC_NOTCONN           = -12,   /* socket is not connected */
    SOC_MSGSIZE           = -13,   /* msg is too long */
    SOC_BEARER_FAIL       = -14,   /* bearer is broken */
    SOC_CONNRESET         = -15,   /* TCP half-write close, i.e., FINED */
    SOC_DHCP_ERROR        = -16,   /* DHCP error */
    SOC_IP_CHANGED        = -17,   /* IP has changed */
    SOC_ADDRINUSE         = -18,   /* address already in use */
    SOC_CANCEL_ACT_BEARER = -19    /* cancel the activation of bearer */
} soc_error_enum;
/* error cause */
typedef enum
{
    CBM_OK                  = 0,  /* success */
    CBM_ERROR               = -1, /* error */
    CBM_WOULDBLOCK          = -2, /* would block */
    CBM_LIMIT_RESOURCE      = -3, /* limited resource */
    CBM_INVALID_ACCT_ID     = -4, /* invalid account id*/
    CBM_INVALID_AP_ID       = -5, /* invalid application id*/
    CBM_INVALID_SIM_ID      = -6, /* invalid SIM id */
    CBM_BEARER_FAIL         = -7, /* bearer fail */
    CBM_DHCP_ERROR          = -8, /* DHCP get IP error */
    CBM_CANCEL_ACT_BEARER   = -9, /* cancel the account query screen, such as always ask or bearer fallback screen */
    CBM_DISC_BY_CM          = -10 /* bearer is deactivated by the connection management */
} cbm_result_error_enum;

/* Socket Type */
typedef enum
{
    SOC_SOCK_STREAM = 0,  /* stream socket, TCP */
    SOC_SOCK_DGRAM,       /* datagram socket, UDP */
    SOC_SOCK_SMS,         /* SMS bearer */
    SOC_SOCK_RAW          /* raw socket */
} socket_type_enum;

/* Socket Options */
typedef enum
{
    SOC_OOBINLINE     = 0x01 << 0,  /* not support yet */
    SOC_LINGER        = 0x01 << 1,  /* linger on close */
    SOC_NBIO          = 0x01 << 2,  /* Nonblocking */
    SOC_ASYNC         = 0x01 << 3,  /* Asynchronous notification */   

    SOC_NODELAY       = 0x01 << 4,  /* disable Nagle algorithm or not */
    SOC_KEEPALIVE     = 0x01 << 5,  /* enable/disable the keepalive */
    SOC_RCVBUF        = 0x01 << 6,  /* set the socket receive buffer size */
    SOC_SENDBUF       = 0x01 << 7,  /* set the socket send buffer size */

    SOC_NREAD         = 0x01 << 8,  /* no. of bytes for read, only for soc_getsockopt */
    SOC_PKT_SIZE      = 0x01 << 9,  /* datagram max packet size */
    SOC_SILENT_LISTEN = 0x01 << 10, /* SOC_SOCK_SMS property */
    SOC_QOS           = 0x01 << 11, /* set the socket qos */

    SOC_TCP_MAXSEG    = 0x01 << 12, /* set the max segmemnt size */
    SOC_IP_TTL        = 0x01 << 13, /* set the IP TTL value */
    SOC_LISTEN_BEARER = 0x01 << 14, /* enable listen bearer */
    SOC_UDP_ANY_FPORT = 0x01 << 15, /* enable UDP any foreign port */

    SOC_WIFI_NOWAKEUP = 0x01 << 16, /* send packet in power saving mode */
    SOC_UDP_NEED_ICMP = 0x01 << 17, /* deliver NOTIFY(close) for ICMP error */
    SOC_IP_HDRINCL    = 0x01 << 18,  /* IP header included for raw sockets */
    SOC_IPSEC_POLICY      = 0x01 << 19, /* ip security policy */
    SOC_TCP_ACKED_DATA  = 0x01 << 20,  /* TCPIP acked data */
    SOC_TCP_DELAYED_ACK = 0x01 << 21, /* TCP delayed ack */
    SOC_TCP_SACK        = 0x01 << 22, /* TCP selective ack */
    SOC_TCP_TIME_STAMP  = 0x01 << 23,  /* TCP time stamp */
    SOC_TCP_ACK_MSEG  = 0x01 << 24   /* TCP ACK multiple segment */
} soc_option_enum;

/* event */
typedef enum
{
    SOC_READ    = 0x01,  /* Notify for read */
    SOC_WRITE   = 0x02,  /* Notify for write */
    SOC_ACCEPT  = 0x04,  /* Notify for accept */
    SOC_CONNECT = 0x08,  /* Notify for connect */
    SOC_CLOSE   = 0x10,   /* Notify for close */
    SOC_ACKED   = 0x20  /* Notify for acked */

} soc_event_enum;

/* socket address structure */
typedef struct 
{
    socket_type_enum	sock_type; /* socket type */
    s16 addr_len; /* address length */
    u16 port; /* port number */
    u8	addr[MAX_SOCK_ADDR_LEN];
    /* IP address. For keep the 4-type boundary, 
     * please do not declare other variables above "addr"
    */
} sockaddr_struct;

/* Macros for SOC_SELECT */
typedef struct
{
    u8	fds_bits[MAX_IP_SOCKET_NUM]; /* select set based on socket id */
} soc_fd_set;

/* select timeout structure */
typedef struct 
{
    u32 tv_sec;  /* select timeout: no. of second */
    u32 tv_usec; /* select timeout: no. of microsecond */
} soc_timeval_struct;

/* socket linger timeout structure */
typedef struct
{
    eat_bool   onoff; /* TRUE: enable linger timeout FALSE: disable linger timeout */
    u16 linger_time; /* linger timeout (unit in seconds) */
} soc_linger_struct;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: void
 *  Description: call back function  about soc_create
 *        Input:
 *               s:: the socket ID
 *               event:: the notify event about current socket, please refer to soc_event_enum
 *               result:: if event is SOC_CONNECT, this paramter indicate the result of connect
 *               ack_size:: if event is  SOC_ACKED, this parameter is valid, it indicate the acked 
 *                          size fo send data.   
 *       Output:
 *       Return:
 *       author: Jumping create at 2013-12-16
 * =====================================================================================
 */
typedef void(*eat_soc_notify)(s8 s,soc_event_enum event,eat_bool result, u16 ack_size);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: void
 *  Description: call back function about eat_gprs_bearer_open
 *        Input:
 *               state:: the current state about bearer, please refer to  cbm_bearer_state_enum
 *               ip_addr[4]:: if state is CBM_ACTIVATED, this parameter is valid, it indicate the
 *                            local IP address.
 *       Output:
 *       Return:
 *               typedef::
 *       author: Jumping create at 2013-12-16
 * =====================================================================================
 */
typedef void(*eat_bear_notify)(cbm_bearer_state_enum state,u8 ip_addr[4]);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: void
 *  Description: call back function about eat_soc_gethostbyname
 *        Input:
 *               request_id:: it set by  eat_soc_gethostbyname
 *               result:: the result about gethostname.
 *               ip_addr[4]:: if the result is TRUE, this parameter indicate the IP address
 *                            of hostname.
 *       Output:
 *       author: Jumping create at 2013-12-16
 * =====================================================================================
 */
typedef void(*eat_hostname_notify)(u32 request_id,eat_bool result,u8 ip_addr[4]);
/* #####   EXPORTED DATA TYPES   #################################################### */

/* #####   EXPORTED VARIABLES   ##################################################### */

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_gprs_bearer_open
 *  Description: open the gprs bearer
 *        Input:
 *               apn:: apn name
 *               user:: user name
 *               password:: password
 *               call_back:: bearer notify call back function.it will report the status
 *               of current bearer,include ip address.
 *       Output:
 *       Return:
 *             CBM_OK : bearer is in activated state.
 *             CBM_INVALID_AP_ID : invalid application id.
 *             CBM_INVALID_ACCT_ID : invalid network account id.
 *             CBM_WOULDBLOCK : the bearer is in deactivating state and the bearer info 
 *                              will be reported later.
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_gprs_bearer_open)(u8 *apn,u8 *user, u8 *password,eat_bear_notify call_back);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_gprs_bearer_hold
 *  Description: Application can use this API to hold the bearer 
 *               if it doesn't want the bearer to be automatically deactivated.
 *        Input:
 *       Output:
 *       Return:
 *             CBM_OK : open the bearer successfully.
 *             CBM_INVALID_AP_ID : invalid application id.
 *             CBM_LIMIT_RESOURCE : limited resource.
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_gprs_bearer_hold)();

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_gprs_bearer_release
 *  Description: release the bearer
 *        Input:
 *       Output:
 *       Return:
 *             CBM_OK : bearer is in deactivated state. 
 *             CBM_INVALID_AP_ID : invalid application id.
 *             CBM_WOULDBLOCK : the bearer is in deactivating state and the bearer info 
 *                  will be reported later. any socket is actived, it will be blocked. 
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_gprs_bearer_release)();

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_create
 *  Description: create a socket 
 *        Input:
 *               type:: Socket type. please take the reference of soc_type_enum.
 *               protocol::Socket protocol. This one shall set to zero except type is SOC_SOCK_RAW.
 *                            When type is SOC_SOCK_RAW, the protocol can be SOC_IPPROTO_ICMP or
 *                            SOC_IPPROTO_RAW.
 *       Output:
 *       Return:
 *             >=0 : socket id
 *	           SOC_INVAL :			  wrong parameters, domain, type, protocol
 *             SOC_INVALID_ACCOUNT : wrong account id, nwk_account_id
 *             SOC_LIMIT_RESOURCE :  no socket available
 *             SOC_BEARER_FAIL :	  bearer broken
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_create)(socket_type_enum type, u8 protocol);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: void
 *  Description: registe call back function while socket notify 
 *        Input:
 *               call_back:: the socket call back function, it will report the status of socket.
 *       Output:
 *       Return:
 *               ::
 *       author: Jumping create at 2014-2-28
 * =====================================================================================
 */
extern void (*const eat_soc_notify_register)(eat_soc_notify call_back);
    
/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_connet
 *  Description: This function connects to the server 
 *        Input:
 *               s:: socket id
 *               addr:: pointer of server address 
 *       Output:
 *       Return:
 *             >=0 : socket id of new connection
 *             SOC_INVALID_SOCKET : invalid socket id
 *             SOC_INVAL : addr is NULL
 *             SOC_WOULDBLOCK : in progressing
 *             SOC_ALREADY : already connected or connecting (only happened for non-blocking)
 *             SOC_BEARER_FAIL : bearer broken
 *             SOC_OPNOTSUPP : the socket is already be listened
 *             SOC_ERROR : unspecified error
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_connect)(s8 s, sockaddr_struct *addr);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_getsockopt
 *  Description: This function gets the socket options. 
 *        Input:
 *               s:: socket id
 *               option:: option type (soc_option_enum)
 *               val:: option value 
 *               val_size:: option value size
 *       Output:
 *       Return:
 *             SOC_SUCCESS :            success
 *             SOC_INVALID_SOCKET :	 invalid socket id
 *             SOC_INVAL :			     invalid arguments: option, val and val_size
 *             SOC_ERROR :			     unspecified error
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_setsockopt)(s8 s, u32 option, void *val, u8 val_size);
/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_getsockopt
 *  Description: This function sets the socket options. 
 *        Input:
 *               s:: socket id
 *               option:: option type (soc_option_enum)
 *               val:: option value (soc_event_enum for SOC_ASYNC)
 *               val_size:: option value size
 *       Output:
 *       Return:
 *             SOC_SUCCESS :            success
 *             SOC_INVALID_SOCKET :	 invalid socket id
 *             SOC_INVAL :			     invalid arguments: option, val and val_size
 *             SOC_ERROR :			     unspecified error
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_getsockopt)(s8 s, u32 option, void *val, u8 val_size);
/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_bind
 *  Description: This function binds a local address which contain IP and port to a socket
 *        Input:
 *               s:: socket id  
 *               addr:: local addr to be binded 
 *       Output:
 *       Return:
 *             SOC_SUCCESS : success
 *             SOC_INVALID_SOCKET : invalid socket id
 *             SOC_BEARER_FAIL : bearer broken
 *             SOC_ERROR : unspecified error
 *             SOC_ADDRINUSE : the local port has been bound by other socket
 * EXAMPLE
 * SOC_NBIO:
 * this option must to be set if you want to use the non-blocking mode or
 * non-blocking plus asyn mode.
 * val contains KAL_TRUE indicates non-blocking shall be adopted, 
 * KAL_FALSE indicates blocking mode shall be adopted. 
 * For example, set a socket to non-blocking mode:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_NBIO, &val, sizeof(kal_bool));
 *
 * SOC_SILENT_LISTEN:
 * val contains KAL_TRUE indicates the silent listen shall be adopted, 
 * KAL_FALSE indicates non-silent listen shall be adopted. 
 * This is SOC_SOCK_SMS property. For example, if you want to let the socket be 
 * the passive to activate the bear, you can set a socket to silent listen mode:
 * For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_SILENT_LISTEN, &val, sizeof(kal_bool))
 *
 * SOC_UDP_NEED_ICMP:
 * This option is used to deliver NOTIFY(close) for ICMP error.
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. 
 * For example, if you want to let set a socket to UDP NEED ICMP mode:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_UDP_NEED_ICMP, &val, sizeof(kal_bool))
 *
 * SOC_ASYNC:
 * this option needs to be set if you want to use the non-blocking plus asyn mode.
 * val contains the events application wants to receive. 
 * For example, if application wants to receive 
 * READ/WRITE/CLOSE/CONNECT events, 
 * it shall call soc_setsockopt like the following:
 * kal_uint8 val = 0;
 * val = SOC_READ | SOC_WRITE | SOC_CLOSE | SOC_CONNECT;
 * soc_setsockopt(s, SOC_ASYNC, &val, sizeof(kal_uint8));
 * P.S. if you are using the server socket, please remember to set the SOC_ACCPET event.
 *
 * SOC_TCP_MAXSEG:
 * A process can only decrease the MSS. When a TCP socket is created, 
 * the default MSS is set to 512. The MSS is updated according to the value announced 
 * from peer TCP after the socket completes TCP three-way handshake. 
 * Therefore, after a socket is created but before connected, a process can only decrease 
 * the MSS from its default of 512. After a connection is established, 
 * the process can decrease the MSS from the new updated value.
 *
 * The value of TCP MSS is reduced to fit the interface MTU if the application set an 
 * unacceptable large value.
 * For example:
 * kal_uint8 val = 512;
 * soc_setsockopt(s, SOC_TCP_MAXSEG, &val, sizeof(kal_uint8)); 
 *
 * SOC_LISTEN_BEARER:
 * val contains bear type indicates which kind of bear we want to receive packets from. 
 * For example, set a socket to receive the WiFi bear:
 * kal_uint8 val = SOC_BEARER_WIFI;
 * soc_setsockopt(s, SOC_LISTEN_BEARER, &val, sizeof(kal_uint8));
 *
 * SOC_WIFI_NOWAKEUP:
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. 
 * This option is to make WNDRV sends outgoing packet to in power saving mode. 
 * For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_WIFI_NOWAKEUP, &val, sizeof(kal_bool))
 *
 * SOC_IP_HDRINCL:
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. If this option is set for a raw IP socket, 
 * we must build our own IP header for all the datagrams that we send on the raw socket. 
 * We can only set this socket option if the socket type in soc_create is 
 * SOC_SOCK_RAW(raw socket) and protocol in soc_create is not SOC_IPPROTO_ICMP.
 * For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_IP_HDRINCL, &val, sizeof(kal_bool))
 *
 * SOC_UDP_ANY_FPORT:
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. For a connected UDP socket, 
 * this option makes socket deliver incoming packet from the same remote 
 * address but different remote port. 
 * For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_UDP_ANY_FPORT, &val, sizeof(kal_bool))
 *
 * SOC_IP_TTL:
 * val contains the TTL values . If you want to change the default TTL (64), 
 * you can set this option. For example:
 * kal_uint8 val = 128;
 * soc_setsockopt(s, SOC_IP_TTL, &val, sizeof(kal_uint8))
 *
 * SOC_QOS:
 * val contains the TOS values . If you want to set the TOS values, you can set this option. 
 * For example:
 * kal_uint8 val = 0x80;
 * soc_setsockopt(s, SOC_QOS, &val, sizeof(kal_uint8))
 *
 * SOC_KEEPALIVE:
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. 
 * You can use this option to enable or disable the keep-alive timer in TCP. 
 * For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_KEEPALIVE, &val, sizeof(kal_bool))
 *
 * SOC_NODELAY:
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. 
 * If you want to send the packet directly without waiting a packet size larger than MSS, 
 * you can set this option. For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_NODELAY, &val, sizeof(kal_bool))
 *
 * SOC_SENDBUF:
 * val contains the value of send buffer's high water mark.  
 * If you want to set the high water mark, you can set this option. 
 * If the value you set is smaller than the MSS, the mss would be adjusted to this value. 
 * The value must be greater than 0 and smaller than the SSPDU + SSPDU_GROW. 
 * For example:
 * kal_uint32 val = 1024;
 * soc_setsockopt(s, SOC_SENDBUF, &val, sizeof(kal_uint32))
 *
 * SOC_RECVBUF:
 * val contains the value of receive buffer's high water mark.  
 * If you want to set the high water mark, you can set this option. 
 * This value must be greater than 0 and smaller than the SRPDU size.  
 * For example:
 * kal_uint32 val = 4096;
 * soc_setsockopt(s, SOC_RECVBUF, &val, sizeof(kal_uint32))
 *
 * SOC_LINGER:
 * val contains the value of enable/diable LINGER option and the value of LINGER time. 
 * This option is used in linger on close. 
 * For example:
 * soc_linger_struct val; 
 * val.onoff = KAL_TRUE; val.
 * linger_time = 3; (sec)
 * soc_setsockopt(s, SOC_LINGER, &val, sizeof(soc_linger_struct_val));
 *
 * SOC_IPSEC_POLICY:
 * val contains sadb_x_policy structure content.
 * You can set this option to set the IPSEC policy.
 * If you want to get the IPSEC policy, 
 * you can set this option. For example:
 * struct sadb_x_policy policy;
 * policy.sadb_x_policy_len = PFKEY_UNIT64(sizeof(policy));
 * policy.sadb_x_policy_exttype = SADB_X_EXT_POLICY;
 * policy.sadb_x_policy_type = 4;  // IPSEC_POLICY_BYPASS;
 * policy.sadb_x_policy_dir = 1;   // IPSEC_DIR_INBOUND;
 * soc_setsockopt(s, SOC_IPSEC_POLICY, &policy, sizeof(sadb_x_policy))
 *
 * SOC_TCP_DELAYED_ACK:
 * The SOC_TCP_DELAYED_ACK function is enable by default. That is to say, 
 * if you don't set this option. The default SOC_TCP_DELAYED_ACK value is TRUE.
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. 
 * If you want to enable the "TCP no delay" function, 
 * you can set this option. For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_TCP_DELAYED_ACK, &val, sizeof(kal_bool))
 *
 * SOC_TCP_SACK:
 * The SOC_TCP_SACK function is enable by default. That is to say, 
 * if you don't set this option. The default SOC_TCP_SACK value is TRUE.
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. 
 * If you want to enable the "TCP SACK" function, 
 * you can set this option. For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_TCP_SACK, &val, sizeof(kal_bool)) 
 *
 * SOC_TCP_TIME_STAMP:
 * The SOC_TCP_TIME_STAMP function is enable by default. That is to say, 
 * if you don't set this option. The default SOC_TCP_TIME_STAMP value is TRUE.
 * val contains KAL_TRUE indicates this option shall be enabled, 
 * KAL_FALSE indicates this option shall be disabled. 
 * If you want to enable the "TCP TIME STAMP" function, 
 * you can set this option. For example:
 * kal_bool val = KAL_TRUE;
 * soc_setsockopt(s, SOC_TCP_TIME_STAMP, &val, sizeof(kal_bool)) 
 *
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_bind)(s8 s, sockaddr_struct *addr);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_listen
 *  Description: makes a socket to a server socket to wait client connections.
 *        Input:
 *               s:: socket id
 *               backlog:: max number of connections
 *       Output:
 *       Return:
 *             SOC_SUCCESS : success
 *             SOC_INVALID_SOCKET : invalid socket id
 *             SOC_BEARER_FAIL : bearer broken
 *             SOC_OPNOTSUPP :	only TCP can be listened
 *             SOC_ALREADY : already listened
 *             SOC_ERROR :	unspecified error
 *             SOC_BEARER_FAIL : bearer broken
 * =====================================================================================
 */

extern s8 (* const eat_soc_listen)(s8 s, u8 backlog);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_accept
 *  Description: waits for the incoming connections and return a socket id of new connection.
 *        Input:
 *               s:: socket id
 *               backlog:: max number of connections
 *       Output:
 *       Return:
 *              >=0 : socket id of new connection
 *              SOC_INVALID_SOCKET : invalid socket id
 *              SOC_INVAL : socket is not calling soc_listen
 *              SOC_WOULDBLOCK : non-blocking
 *              SOC_CONNABORTED : server socket cannot receive anymore 
 *              SOC_BEARER_FAIL : bearer broken
 *              SOC_ERROR : unspecified error
 * =====================================================================================
 */

extern s8 (* const eat_soc_accept)(s8 s, sockaddr_struct *addr);
/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_send
 *  Description: Send the data to the destination 
 *        Input:
 *               s:: socket id
 *               buf:: contain data to be sent 
 *               len:: data length
 *       Output:
 *       Return:
 *             >=0 : SUCCESS
 *             SOC_INVALID_SOCKET : invalid socket id
 *             SOC_INVAL : buf is NULL or len equals to zero
 *             SOC_WOULDBLOCK : buffer not available or bearer is establishing
 *             SOC_BEARER_FAIL : bearer broken
 *             SOC_NOTCONN : socket is not connected in case of TCP
 *             SOC_PIPE : socket is already been shutdown
 *             SOC_MSGSIZE : message is too long
 *             SOC_ERROR : unspecified error
 *             SOC_NOTBIND : in case of sending ICMP Echo Request, shall bind before
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s32 (* const eat_soc_send)(s8 s,const void *buf, s32 len);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_recv
 *  Description: Receive data and return the source address. 
 *        Input:
 *               s:: socket id
 *               buf:: buffer for receiving data
 *               len:: buffer size
 *               flags::
 *       Output:
 *       Return:
 *             0 :                   receive the FIN from the server
 *             SOC_INVALID_SOCKET :  invalid socket id
 *             SOC_INVAL :           buf is NULL or len equals to zero
 *             SOC_WOULDBLOCK :      no data available
 *             SOC_BEARER_FAIL :     bearer broken
 *             SOC_NOTCONN :         socket is not connected in case of TCP
 *             SOC_PIPE :            socket is already been shutdown
 *             SOC_ERROR :           unspecified error
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s32 (* const eat_soc_recv)(s8 s, void *buf, s32 len);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_sendto
 *  Description: Send the data to the destination. 
 *        Input:
 *               s:: socket id  
 *               buf:: contain data to be sent
 *               len:: data length
 *               addr:: destination address 
 *       Output:
 *       Return:
 *             >=0 : SUCCESS
 *             SOC_INVALID_SOCKET : invalid socket id
 *             SOC_INVAL : buf is NULL or len equals to zero
 *             SOC_WOULDBLOCK : buffer not available or bearer is establishing
 *             SOC_BEARER_FAIL : bearer broken
 *             SOC_NOTCONN : socket is not connected in case of TCP
 *             SOC_PIPE : socket is already been shutdown
 *             SOC_MSGSIZE : message is too long
 *             SOC_ERROR : unspecified error
 *             SOC_NOTBIND : in case of sending ICMP Echo Request, shall bind before
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s32 (* const eat_soc_sendto)(s8 s, const void *buf, s32 len, sockaddr_struct *addr);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_recvfrom
 *  Description: Receive data and return the source address. 
 *        Input:
 *               s:: socket id
 *               buf:: buffer for receiving data
 *               len:: buffer size
 *               fromaddr:: remote addess 
 *       Output:
 *       Return:
 *             0 :                   receive the FIN from the server
 *             SOC_INVALID_SOCKET :  invalid socket id
 *             SOC_INVAL :           buf is NULL or len equals to zero
 *             SOC_WOULDBLOCK :      no data available
 *             SOC_BEARER_FAIL :     bearer broken
 *             SOC_NOTCONN :         socket is not connected in case of TCP
 *             SOC_PIPE :            socket is already been shutdown
 *             SOC_ERROR :           unspecified error
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s32 (* const eat_soc_recvfrom)(s8 s, void *buf, s32 len, sockaddr_struct *fromaddr);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_select
 *  Description: Allow the application to query the status of multiple sockets. 
 *        Input:
 *               ndesc::  max selected socket id plus 1
 *               in:: input soc_fd_set (readfds)
 *               out:: output soc_fd_set (writefds)
 *               ex:: exception soc_fd_set (exceptfds)
 *               tv:: specify the timeout value,
 *                    tv == NULL: block until ready
 *                    tv != NULL && value == 0: return immediately
 *                    tv != NULL && value != 0: return after timeout
 *       Output:
 *       Return:
 *             >0 :                     number of ready sockets
 *              0 :                     timeout expires before anything interesting happens
 *             SOC_INVALID_SOCKET :	 selected sockets are not belong to this module
 *             SOC_INVAL : 			 invalid arguments: ndesc, in, out, ex
 *             SOC_WOULDBLOCK :		 non-blocking
 *             SOC_BEARER_FAIL :		 bearer broken
 *             SOC_ERROR :	    		 unspecified error
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_select)(u8 ndesc, soc_fd_set *in, soc_fd_set *out, soc_fd_set *ex, soc_timeval_struct *tv);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_fd_set
 *  Description: set the socket id which you want to select 
 *        Input:
 *               s:: socket id
 *               p:: 
 *       Output:
 *       Return:
 *               void::
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern void (* const eat_soc_fd_set)(u8 s,soc_fd_set *p);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_gethostbyname
 *  Description: This function gets the IP of the given domain name. 
 *        Input:
 *               domain_name:: Domain_name
 *               addr:: resolved address
 *               addr_len:: address length
 *               request_id::embedded in call back function
 *       Output:
 *       Return:
 *             SOC_SUCCESS :			     Get IP address successfully
 *             SOC_INVAL :			         invalid arguments: null domain_name, etc.
 *             SOC_ERROR :			         unspecified error
 *             SOC_WOULDBLOCK :		     wait response from network
 *             SOC_LIMIT_RESOURCE :		 socket resources not available
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_gethostbyname)(const char *domain_name,u8 *addr,u8 *addr_len,u32 request_id);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: void
 *  Description: set call back function about eat_soc_gethostbyname 
 *        Input:
 *              call_back:: if SOC_WOULDBLOCK return when call eat_soc_gethostbyname, th ip address will get from this call back function. 
 *       Output:
 *       Return:
 *               ::
 *       author: Jumping create at 2014-3-6
 * =====================================================================================
 */
extern void (*const eat_soc_gethost_notify_register)(eat_hostname_notify call_back);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: s8
 *  Description: returns the remote or local address assoicated with given socket 
 *        Input:
 *               s:: socket id 
 *               is_local:: true: local   false: remote
 *       Output:
 *               addr:: copy the addres
 *       Return:
 *             SOC_SUCCESS :             success
 *             SOC_INVAL :			     invalid argument
 *             SOC_INVALID_SOCKET :   	 not a valid socket
 *             SOC_PIPE :	    		 socket is broken
 *             SOC_NOTCONN :			 not a connected socket
 *             SOC_ERROR :	    		 cannot found corresponding DNS query (or socket not bound for SOC_SOCK_SMS)
 *       author: Jumping create at 2014-2-11
 * =====================================================================================
 */
extern s8 (* const eat_soc_getsockaddr)(s8 s,eat_bool is_local,sockaddr_struct *addr);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_soc_close
 *  Description: Close the socket. 
 *        Input:
 *               s:: socket id  
 *       Output:
 *       Return:
 *             SOC_SUCCESS : success
 *             SOC_INVALID_SOCKET : invalid socket id
 *       author: Jumping create at 2013-12-12
 * =====================================================================================
 */
extern s8 (* const eat_soc_close)(s8 s);

#endif /* end of include guard: __EAT_SOCKET_H__ */
