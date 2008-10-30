/***************************************************************************
 * FileName      : acp420.cpp
 * Created       : 2007/08/05
 * LastModified  : 2007/08/
 * Author        : hiroaki KOZUKA
 * Aim           : ACP420 Driver for a Robot Control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "acp420.h"

ACP420* ACP420::m_Instance = NULL;

ACP420::ACP420() {
  int boardloop;
    //logMsg("ACP420: The program is setting up the board(s)..\n", 0,0,0,0,0,0);
    cout<<"ACP420: The program is setting up the board(s).."<<endl;
    
    // Looking Up ACP420 Board on the system. --->
    board_num = DeviceLookup();
    if( board_num == 0 ){
    //logMsg("ACP420: ERROR - ACP420: No ACP420 found on the system.\n", 0,0,0,0,0,0);
    cout<<"ACP420: ERROR - ACP420: No ACP420 found on the system."<<endl;
    }
    else{
    //logMsg("ACP420: %d of ACP420 found.\n", board_num,0,0,0,0,0);
    cout<<"ACP420: "<<board_num<<" of ACP420 found."<<endl;
    }
    // Initializing the ACP420 Board. --->
    for (boardloop = 0; boardloop < board_num; boardloop++) {
    DeviceInit(boardloop);
    //logMsg("ACP420: Board unit %d is ready.\n", 0,0,0,0,0,0);
    cout<<"ACP420: Board unit "<<boardloop<<" is ready."<<endl;
    }
}

ACP420::~ACP420() {
  m_Instance = NULL;
}

ACP420* ACP420::getInstance() {
    if (m_Instance == NULL )
        m_Instance = new ACP420();
    return m_Instance;
}

int
ACP420::DeviceLookup() {
    int  bus, busA;     // PCI bus number
    int  dev, devA;     // PCI device number
    int  fun, funA;     // PCI function number
    int  bdIndex = 0;   // Device search index count
    int  unitNum = 0;   // Board unit number
    UINT8   intrLvl;    // Assign interrupt level
    UINT8   boardType;  // Board type (PCI/CompactPCI)
    UINT32  regBase;    // Temporary register base address 
    STATUS  judge, judgeA;

    ACP420_BSN_CNV acp420BsnCnv[ ACP420_SLOT_NUM ];

    /* Board detection */
    do {
  /* AVAL,COSMO */
  judge  = pciFindDevice( PCI_VENDER_ID, PCI_DEVICE_ID_COSMO, bdIndex, &bus , &dev , &fun );
        judgeA = pciFindDevice( PCI_VENDER_ID, PCI_DEVICE_ID_AVAL, bdIndex, &busA, &devA, &funA );

  if((judge != OK) && (judgeA != OK)) /* Check board */
       break;
  if(judgeA == OK) {     /* ACP-420 Data copy */
      bus = busA;
      dev = devA;
      fun = funA;
      boardType = ACP_420_BOARD;
  }
  else {
      boardType = PCPG_46_BOARD;
  }

  /* Configuration */
  /* Get PCI resouce information */
        acp420Inf[ unitNum ].mbus  = bus;
        acp420Inf[ unitNum ].mdev  = dev;
        acp420Inf[ unitNum ].mfunc = fun;
  /* Get I/O base address */
  pciConfigInLong( bus, dev, fun, PCI_CFG_BASE_ADDRESS1, &regBase );
        regBase &= (UINT32) PCI_IOBASE_MASK;
        acp420Inf[ unitNum ].mregBase = PCI2CPU_IO_TRANSLATE( (UINT32)regBase );
        pciConfigInByte( bus, dev, fun, PCI_CFG_DEV_INT_LINE, &intrLvl );
  
  acp420Inf[ unitNum ].mintLvl = (UINT32)intrLvl;     /* Get interrupt revel */
        acp420Inf[ unitNum ].mboardType = boardType;        /* Record kind of board */ 
        acp420Inf[ unitNum ].munitNum   = unitNum;          /* Note board number */

  /* Get board ID ( Board Serect Number ) */
  acp420Inf[unitNum].mdpSW =
      ~sysInByte( acp420Inf[ unitNum ].mregBase + ACP420_PORT_BSN_SWITCH );

  /* Reset declaration of using board  */
  acp420Inf[unitNum].museFlag = FALSE;
  /* Board/Uint number conversion table */
  acp420BsnCnv[acp420Inf[ unitNum ] .mdpSW].munitNum = unitNum;

        unitNum++;
        bdIndex++;
    } while( unitNum < ACP420_SLOT_NUM );
    return( unitNum );
}

int
ACP420::DeviceInit(int unitNum) {
    /* OPEN */
    acp420Inf[unitNum].museFlag = TRUE;
    /* Mode2 "set 4 multiplier" */
    for( int ch = 0; ch < 4; ch++ ){  
  sysOutByte( acp420Inf[ unitNum ].mregBase +(UINT32)ch *
        ACP420_IO_RANGE_PMC540 + ACP420_PORT_MODE2, (UINT8)0xc0 );
    }
    return( OK );
}

void
ACP420::Read(int unitNum , long* encData,int  dataSize) {
  int num = dataSize/sizeof(long);
  if( num>CH_NUM ) return;
  
  UINT8 data3, data2, data1, data0;
  int  key;
  for(int ch=0; ch<num; ch++){
    key = intLock(); /* mutual exclusion start */
    
    /* Send command "External_Counter_Read" */
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
          ACP420_IO_RANGE_PMC540 + ACP420_PORT_COMMAND, (UINT8)0x2d );

    /* Get data */
    data3 = sysInByte( acp420Inf[ unitNum ].mregBase +  (UINT32)ch *
               ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA1 );
    data2 = sysInByte( acp420Inf[ unitNum ].mregBase +  (UINT32)ch *
               ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA2 );
    data1 = sysInByte( acp420Inf[ unitNum ].mregBase +  (UINT32)ch *
               ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA3 );
    data0 = sysInByte( acp420Inf[ unitNum ].mregBase +  (UINT32)ch *
               ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA4 );

    intUnlock( key ); /* mutual exclusion stop */

    encData[ch] = ( ( UINT32 )data3 << 24 ) | ( ( UINT32 )data2 << 16 ) |
                ( ( UINT32 )data1 << 8  ) |   ( UINT32 )data0;

        // When data is Minus number
    if(data3 > 7)
      encData[ch] = -( (~encData[ch] + 1) & (UINT32)0x07FFFFFF );
  }
}

void
ACP420::Write(int unitNum, long* encData, int dataSize){
  int num = dataSize/sizeof(long);
  if( num>CH_NUM ) return;

  UINT8 data;
  int key;
  
  for(int ch=0; ch<num; ch++){
    key = intLock();/* mutual exclusion start */
     
    /* Write data */
    data = ( UINT8 )( encData[ch] >> 24 );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
          ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA1, data );

    data = ( UINT8 )( encData[ch] >> 16 );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
          ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA2, data );
    
    data = ( UINT8 )( encData[ch] >> 8 );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
          ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA3, data );

    data = ( UINT8 )( encData[ch] );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
          ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA4, data );

    /* Send command "External_Counter_Write" */
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
          ACP420_IO_RANGE_PMC540 + ACP420_PORT_COMMAND,(UINT8)0x2c );
    
    intUnlock( key );/* mutual exclusion stop */
  }
}

int
ACP420::CHNum(){
  return CH_NUM;
}

int
ACP420::BoardNum(){
  return board_num;
}

