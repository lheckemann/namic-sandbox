/***************************************************************************
 * FileName      : acp550.cpp
 * Created       : 2007/07/21
 * LastModified  : 2007/08/06
 * Author        : Hiroaki KOZUKA
 * Aim           : ACP550 Driver for a Robot Control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "acp550.h"

ACP550* ACP550::m_Instance = NULL;

ACP550::ACP550() {
    //logMsg("ACP550: The program is setting up the board(s)..\n", 0,0,0,0,0,0);
    cout<<"ACP550: The program is setting up the board(s).."<<endl;

    // Looking Up ACP550 Board on the system. --->
    board_num = DeviceLookup();
    if( board_num == 0 ){
  //logMsg("ACP550: ERROR - ACP550: No ACP550 found on the system.\n", 0,0,0,0,0,0);
  cout<<"ACP550: ERROR - ACP550: No ACP550 found on the system."<<endl;
    }
    else{
  //logMsg("ACP550: %d of ACP550 found.\n", board_num,0,0,0,0,0);
  cout<<"ACP550: "<<board_num<<" of ACP550 found."<<endl;
    }
    // Initializing the ACP550 Board. --->
    for (int boardloop = 0; boardloop < board_num; boardloop++) {
  DeviceInit(boardloop, 4, ACP550_SING_0_10V ,&D1, &D2);
  //logMsg("ACP550: Board unit %d is ready.\n", 0,0,0,0,0,0);
  cout<<"ACP550: Board unit "<<boardloop<<" is ready."<<endl;
    }
}

ACP550::~ACP550() {
  m_Instance = NULL;
}

ACP550* ACP550::getInstance() {
    if (m_Instance == NULL )
  m_Instance = new ACP550();
    return m_Instance;
}

int
ACP550::DeviceLookup() {
  int         bus;            /* PCI bus number                   */
  int         dev;            /* PCI device number                */
  int         fun;            /* PCI function number              */
  int         unitNum;        /* Board unit number                */
  int         bdIndex;        /* Device search index count        */
  UINT8       intrLvl;        /* Assign interrupt level           */
  UINT32      regBase;        /* Temporary register base address  */
  UINT16      sVendor;        /* Sub Vender                       */
  UINT16      sSystem;        /* Sub System                       */
  int         lock;
  
  bdIndex = unitNum = 0;      /* Clear the index and unit number      */
  
  // Board Detection at first
  do {
    if( pciFindDevice( PCI_VENDOR_550, PCI_DEVICE_550,bdIndex, &bus, &dev, &fun) != OK )
      break;              /* Finished board detection */
    
    /* PCI sub-system vendor ID & PCI sub-system ID read out */
    pciConfigInWord( bus, dev, fun, PCI_CFG_SUB_VENDER_ID, &sVendor );
    pciConfigInWord( bus, dev, fun, PCI_CFG_SUB_SYSTEM_ID, &sSystem );
    
    /* Sub-System vendor ID & Sub-System ID */
    if( (sVendor == PCI_SVENDR_550) && (sSystem == PCI_SYSTEM_550) ){
      lock = intLock();           /* ### intLock ### */
    
      /* Set latency timer */
      pciConfigOutByte( bus, dev, fun, PCI_CFG_LATENCY_TIMER, 0x20 );
      
      /* Get PCI base address 1 - PLX9050 Local space  */
      pciConfigInLong( bus, dev, fun, PCI_CFG_BASE_ADDRESS_1, &regBase );
      regBase &= (UINT32)PCI_IOBASE_MASK;
      devInfo550[ unitNum ].plxBase  = PCI2CPU_IO_TRANSLATE( regBase );
      
      /* Get PCI base address 2 - ACP-550 Register space  */
      pciConfigInLong( bus, dev, fun, PCI_CFG_BASE_ADDRESS_2, &regBase );
      regBase &= (UINT32)PCI_IOBASE_MASK;
      devInfo550[ unitNum ].baseAdrs = PCI2CPU_IO_TRANSLATE( regBase );
      
      /* Get assigned interrupt level (line) register  */
      pciConfigInByte( bus, dev, fun, PCI_CFG_DEV_INT_LINE, &intrLvl );
      devInfo550[ unitNum ].intrLvl = (UINT32)intrLvl;
      /* Get Board ID */
      devInfo550[ unitNum ].boardID = devInfo550[ unitNum ].boardID & ACP550_STAT_ID03_MASK;
      
      intUnlock( lock );          /* ### intUnlock ### */
      
      devInfo550[ unitNum ].bus = bus;            /* Get Bus number       */
      devInfo550[ unitNum ].dev = dev;            /* Get Device number    */
      devInfo550[ unitNum ].func = fun;           /* Get Function number  */
      devInfo550[ unitNum ].unitNum = unitNum;    /* save the board unit number */
      
      unitNum++;          /* Update the board unit number */
    } 
    bdIndex++;
  } while( unitNum < ACP550_MAX_BOARD );
  return( unitNum );
}

int
ACP550::DeviceInit(int unitNum, int slotNum, int range ,double *D1, double *D2) {
  UINT16 setData;
  double com1=0, com2=0;
  switch(range){
  case  0: com1 =  10; break;
  case  1: com1 =   5; break;
  case  2: com1 = 2.5; break;
  case  3: com1 =   1; break;
  case  4: com1 = com2 = 20; break;
  case  5: com1 = com2 = 10; break;
  case  6: com1 = com2 =  5; break;
  case  7: com1 = com2 =  2; break;
  case  8: com1 =  10; break;
  case  9: com1 =   5; break;
  case 10: com1 = 2.5; break;
  case 11: com1 =   1; break;
  case 12: com1 = com2 = 20; break;
  case 13: com1 = com2 = 10; break;
  case 14: com1 = com2 =  5; break;
  case 15: com1 = com2 =  2; break;
  }
  *D1 = com1/65536;
  *D2 = com2/2;
  
  /* Set Slot Number */
  sysOutByte((devInfo550[unitNum].baseAdrs + ACP550_SLOT_NUM_ADRS),(UINT8)slotNum);
  
  for (int i = 0; i < slotNum; i++ ){
    /* Set Scan Slot Pointer */
    sysOutByte( (devInfo550[ unitNum ].baseAdrs + ACP550_SLOT_PTR_ADRS),(UINT8)i );
    setData = ( (UINT16)range<<8 | (UINT16)i );
    /* Set Scan ch & range */
    sysOutWord((devInfo550[unitNum].baseAdrs + ACP550_SLOT_CH_ADRS), setData);
  }
  return( OK );
}

void
ACP550::Read( int unitNum, double* data, int dataSize) {
  int num = dataSize/sizeof(double);
  if( num>CH_NUM ) return;
  
  for(int ch=0; ch<num; ch++){
    /* Set Cmd */
    sysOutByte( (devInfo550[ unitNum ].baseAdrs + ACP550_CMD_ADRS), (UINT8)(ch + 0x10) );
    // Wait till SPT & STC become 0
    while((sysInByte(devInfo550[ unitNum ].baseAdrs + ACP550_CMD_ADRS) & (UINT8)0xC0) != 0);
    /* FIFO Read */
    data[ch] = (double)( sysInWord(devInfo550[ unitNum ].baseAdrs + ACP550_AD_ADRS) ) * D1 - D2;
  }
}

int
ACP550::CHNum(){
  return CH_NUM;
}

int
ACP550::BoardNum(){
  return board_num;
}

