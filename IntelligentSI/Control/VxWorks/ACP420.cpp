/***************************************************************************
 * FileName      : ACP420.cpp
 * Created       : 2007/08/05
 * LastModified  : 2007/08/
 * Author        : hiroaki KOZUKA
 * Aim           : ACP420 Driver for a Robot Control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "ACP420.h"

ACP420::ACP420() {
  int boardloop;
  cout<<"ACP420: The program is setting up the board(s).."<<endl;

  // Looking Up ACP420 Board on the system. --->
  board_num = DeviceLookup();
  if( board_num == 0 ){
    cout<<"ACP420: ERROR - ACP420: No ACP420 found on the system."<<endl;
  }
  else{
    Cnt = new long*[board_num];
    In = new bool*[board_num];
    cout<<"ACP420: "<<board_num<<" of ACP420 found."<<endl;
  }
  // Initializing the ACP420 Board. --->
  for (boardloop = 0; boardloop < board_num; boardloop++) {
    DeviceInit(boardloop);
    Cnt[boardloop] = new long[CH_NUM];
    memset(&Cnt[boardloop][0], 0, sizeof(long)*CH_NUM );
    this->Write(boardloop);

    In[boardloop] = new bool[CH_NUM];
    memset(&In[boardloop][0], 0, sizeof(bool)*CH_NUM );

    cout<<"ACP420: Board unit "<<boardloop<<" is ready."<<endl;
  }
}

ACP420::~ACP420() {
  for (int boardloop = 0; boardloop < board_num; boardloop++) {
    memset(&Cnt[boardloop][0], 0, sizeof(long)*CH_NUM );
    this->Write(boardloop);
    delete [] Cnt[boardloop];
    delete [] In[boardloop];
  }
  delete [] Cnt;
  delete [] In;
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
  /* Mode1 "set IN-n L" */
  /* Mode2 "set 4 multiplier" */
  for( int ch = 0; ch < 4; ch++ ){
    sysOutByte( acp420Inf[ unitNum ].mregBase +(UINT32)ch *
                ACP420_IO_RANGE_PMC540 + ACP420_PORT_MODE1, (UINT8)0x06 );
    sysOutByte( acp420Inf[ unitNum ].mregBase +(UINT32)ch *
                ACP420_IO_RANGE_PMC540 + ACP420_PORT_MODE2, (UINT8)0xc0 );
  }
  return( OK );
}

void
ACP420::ReadIn(int unitNum){
  UINT8 data;
  int  key;
  key = intLock(); /* mutual exclusion start */
  for(int ch = 0; ch<CH_NUM; ch++ ){

    data = sysInByte( acp420Inf[ unitNum ].mregBase +  (UINT32)ch *
                      ACP420_IO_RANGE_PMC540 + ACP420_PORT_SIGNAL ) &(UINT8)0x40 ;
    if(data == 64)
      In[unitNum][ch] = true;
    else
      In[unitNum][ch] = false;
  }
  intUnlock( key ); /* mutual exclusion stop */
}

void
ACP420::ReadCnt(int unitNum) {
//  int num = dataSize/sizeof(long);
//  if( num>CH_NUM ) return;

  UINT8 data3, data2, data1, data0;
  int  key;
  for(int ch=0; ch<CH_NUM; ch++){
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

    Cnt[unitNum][ch] = ( ( UINT32 )data3 << 24 ) | ( ( UINT32 )data2 << 16 ) |
      ( ( UINT32 )data1 << 8  ) |   ( UINT32 )data0;

    // When data is Minus number
    if(data3 > 7)
      Cnt[unitNum][ch] = -( (~Cnt[unitNum][ch] + 1) & (UINT32)0x07FFFFFF );
  }
}

void
ACP420::Read(int unitNum, BoardType type){
  if(type == TYPE_CNT){
    this->ReadCnt(unitNum);
  }
  else if(type == TYPE_I){
    this->ReadIn(unitNum);
  }
}

void
ACP420::Write(int unitNum){
  UINT8 data;
  int key;

  for(int ch=0; ch<CH_NUM; ch++){
    key = intLock();/* mutual exclusion start */

    // Write data
    data = ( UINT8 )( Cnt[unitNum][ch] >> 24 );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
                ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA1, data );

    data = ( UINT8 )( Cnt[unitNum][ch] >> 16 );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
                ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA2, data );

    data = ( UINT8 )( Cnt[unitNum][ch] >> 8 );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
                ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA3, data );

    data = ( UINT8 )( Cnt[unitNum][ch] );
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
                ACP420_IO_RANGE_PMC540 + ACP420_PORT_DATA4, data );

    // Send command "External_Counter_Write"
    sysOutByte( acp420Inf[ unitNum ].mregBase + (UINT32)ch *
                ACP420_IO_RANGE_PMC540 + ACP420_PORT_COMMAND,(UINT8)0x2c );

    intUnlock( key );/* mutual exclusion stop */

  }
}

long
ACP420::GetCount(int unitNum, int ch){
  return Cnt[unitNum][ch];
}

bool
ACP420::GetIn(int unitNum, int ch){
  return In[unitNum][ch];
}

void
ACP420::Set(int unitNum, int ch, long data){
  Cnt[unitNum][ch] = data;
}

int
ACP420::CHNum(){
  return CH_NUM;
}

int
ACP420::BoardNum(){
  return board_num;
}

long*
ACP420::GetCntDataPtr(int unitNum,int ch){
  return &Cnt[unitNum][ch];
}

bool*
ACP420::GetInDataPtr(int unitNum,int ch){
  return &In[unitNum][ch];
}


