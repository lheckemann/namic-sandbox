/*** rds.h  --  Include File for Raw Data Server
*
*    Description:
*
*        This include file contains definitions, data structures and
*        function prototypes for the Raw Data Server.
*
*****************************************************************************/

#ifndef RDS_INCL
#define RDS_INCL

#define RDS_CONFIG_PATHNAME     "/usr/g/bin/rds_config.cfg"

/* Debugging flags OR'd together into APS global rds_debug */
#define RDS_DEBUG_FLOW1         0x0001  /* Detailed info of program flow */
#define RDS_DEBUG_FLOW2         0x0002  /* "must have" info for program flow */
#define RDS_DEBUG_INIT          0x0004
#define RDS_DEBUG_SETUP         0x0008
#define RDS_DEBUG_XFER1         0x0010
#define RDS_DEBUG_XFER2         0x0020
#define RDS_DEBUG_COMMS_AP      0x0040
#define RDS_DEBUG_COMMS_HOST    0x0080
#define RDS_DEBUG_ERRORS        0x0100
#define RDS_DEBUG_WARNINGS      0x0200
#define RDS_DEBUG_SOCKET        0x0400
#define RDS_DEBUG_MCSOCK        0x0800
#define RDS_DEBUG_FLOW3         0x1000  /* Overall info that will not kill drf */
#define RDS_DEBUG_AQH_COMMS     0x2000
#define RDS_DEBUG_SOCKTEST      0x4000
#define RDS_DEBUG_DMA           0x8000  /* DMA from AP to APS memory */
#define RDS_DEBUG_AQSOP1        0x10000 /* AQS non common opcode debug */
#define RDS_DEBUG_AQSOP2        0x20000 /* AQS common (NEX) opcode debug */
#define RDS_DEBUG_DATA1         0x40000
#define RDS_DEBUG_DATA2         0x80000

/* Size of APS buffer in bytes - start with 2Mb */
#define RDS_APS_BUF_BYTES   (2 * 1024*1024)

/* Status return value from RDS */
#define RDS_OK                          0       /* Good return status */
#define RDS_NOT_CONNECTED               400     /* No host process connected */

/* Opcodes in packets sent between RDS to host */
#define OP_RDS_RAW_READY        1
#define OP_RDS_INIT             2
#define OP_RDS_PREPPED          3


#define RDS_XFER_DMA    1
#define RDS_XFER_PUSH   2

/* Data shared between RDS transfer and RDS socket tasks */

typedef struct
{
    int         connections;            /* RDS internal */
    int         host_socket_fd;         /* RDS internal */
    int         host_buff_offset;       /* RDS internal */
    int         views_per_xfer;         /* Rest of these loaded from host process init pkt */
   unsigned int aqs_pkt_flags;
    int         xfer_mode;
   unsigned int host_buff_addr;
    int         host_buff_size;
} RDS_DATA_STRUCT;

/* Messages passed between RDS tasks and the host process have the RDS_MESSAGE structure */
/* which consists of a RDS_MSG_HDR header followed by a variable length packet */
/* The size of the packet is given in the header. */

#define RDS_MAX_MSG_BYTES 256

typedef struct
{
  int           opcode;         /* OP_RDS_INIT */
  int           pktlen;         /* bytes in this packet (excludes header) */
} RDS_MSG_HDR;

typedef struct
{
  RDS_MSG_HDR hdr;
  char pkt[RDS_MAX_MSG_BYTES - sizeof(RDS_MSG_HDR)];
} RDS_MESSAGE;

/* From host to RDS with OP_RDS_INIT */
/* Following connection to RDS_socket task */
typedef struct
{
  int           views_per_xfer; /* Views for AQS to accumulate before transfer */
  unsigned int  aqs_pkt_flags;  /* Enable bits for AQS packets */
  int           xfer_mode;      /* DMA and/or push/pull flags */
  unsigned int  host_buff_addr; /* If DMA-push: Target buffer start address */
  int           host_buff_size; /* If DMA-push: Target buffer size in bytes */
} RDS_INIT_PKT;


/* From RDS_transfer to host with OP_RDS_RAW_READY */
/* Shows data is ready to be pulled or will be followed by data */
typedef struct
{
  unsigned int  id;             /* ID from AQS */
  int           timestamp;      /* APS timer3 timestamp */
  int           seq_num;        /* sequence number from RDS */
  unsigned int  raw_cs;         /* Checksum for raw data */
  unsigned int  msg_cs;         /* Checksum for AQS message data */
  int           raw_size;       /* Raw data section size in bytes */
  int           msg_size;       /* AQS message section size in bytes */
  unsigned int  aps_buff_addr;  /* PCI bus address of buffer */
  int           aps_buff_size;  /* size of APS buffer in bytes */
  int           start_offset;   /* Byte offset in buffer to start of this data */
  unsigned int  control_flags;  /* Bit settings */
} RDS_RAW_READY_PKT;


/* Bit masks for control_flags member of RDS_RAW_READY_PKT */
#define RDS_DATA_SPLIT                  0x01
#define RDS_DATA_SEND_VIEW_COUNT        0x02    /* View count was exceeded */
#define RDS_DATA_SEND_EOP               0x04    /* End of pass was received */
#define RDS_DATA_SEND_EOS               0x08    /* End of scan */
#define RDS_DATA_SEND_NEWPATH           0x10    /* Datapath switched to Recon */
#define RDS_DATA_SEND_MSG_LIST          0x20    /* Message list is full */

typedef struct
{
  unsigned int  timestamp;      /* APS timer3 timestamp */
  int           unused;
} RDS_PREPPED_PKT;


#endif
