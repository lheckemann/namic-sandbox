/***************************************************************************
 * FileName      : ACP560.cpp
 * Created       : 2007/06/15
 * LastModified  : 2007/06/20
 * Author        : Jumpei ARATA @ Nitech, Japan, jumpei@nitech.ac.jp
 * Aim           : ACP560 Driver for a Robot Control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "ACP560.h"

ACP560::ACP560() {
  int boardloop;

  cout<<"ACP560: The program is setting up the board(s).."<<endl;

  // Looking Up ACP560 Board on the system. --->
  board_num = DeviceLookup();
  if( board_num == 0 ){
    cout<<"ACP560: ERROR - ACP560: No ACP560 found on the system."<<endl;
  }
  else{
    Volt = new double*[board_num];
    cout<<"ACP560: "<<board_num<<" of ACP560 found."<<endl;
  }
  // Initializing the ACP560 Board. --->
  for (boardloop = 0; boardloop < board_num; boardloop++) {
    DeviceInit(boardloop);
    Volt[boardloop] = new double[CH_NUM];
    memset( &Volt[boardloop][0], 0, sizeof(double)*CH_NUM );
    this->Write(boardloop, BoardBase::TYPE_DA);
    cout<<"ACP560: Board unit "<<boardloop<<" is ready."<<endl;
  }

}

ACP560::~ACP560() {
  for (int boardloop = 0; boardloop < board_num; boardloop++) {
    memset( &Volt[boardloop][0], 0, sizeof(double)*CH_NUM );
    this->Write(boardloop, BoardBase::TYPE_DA );
    delete [] Volt[boardloop];
  }
  delete [] Volt;
  cout<<"End ACP560"<<endl;
}

int
ACP560::DeviceLookup() {
  int         bus;            /* PCI bus number                       */
  int         dev;            /* PCI device number                    */
  int         fun;            /* PCI function number                  */
  int         bdIndex;        /* Device search index count            */
  int         unitNum;        /* Board unit number                    */
  UINT8       intrLvl;        /* Assign interrupt level (AutoConfig)  */
  UINT8       boardid;        /* PCI Board ID (Configured by DIPSw)   */
  UINT32      pcisubid;       /* Sub-system vendor ID/ID code         */
  UINT32      regBase;        /* Temporary register base address      */
  int   lockKey;

  bdIndex = unitNum = 0;      /* Clear the index and unit number      */

  // Board Detection at first
  do {
    if( pciFindDevice (PCI_VENDOR_560, PCI_DEVICE_560,
                       bdIndex, &bus, &dev, &fun) != OK )
      break; /* Finished board detection */

    /* Get Subsystem vendor ID and Subsystem ID from device  */
    pciConfigInLong(bus, dev, fun, PCI_CFG_SUB_VENDER_ID, &pcisubid);

    /* Is this board ACP-560 */
    if( (pcisubid == (UINT32)((PCI_SYSTEM_560<<16)|PCI_SVENDR_560)) ){
      lockKey = intLock();

      /* latency timer set */
      pciConfigOutByte (bus, dev, fun, PCI_CFG_LATENCY_TIMER, 0x20);

      /* Get PCI base address 1 - PLX9050 Local I/O space  */
      pciConfigInLong(bus, dev, fun, PCI_CFG_BASE_ADDRESS_1, &regBase);
      regBase &= (UINT32) PCI_IOBASE_MASK;
      devInfo560[ unitNum ].plxBase  = PCI2CPU_IO_TRANSLATE ((UINT32)regBase);

      /* Get PCI base address 2 - ACP-560 IO  space  */
      pciConfigInLong(bus, dev, fun, PCI_CFG_BASE_ADDRESS_2, &regBase);
      regBase &= (UINT32) PCI_IOBASE_MASK;
      devInfo560[ unitNum ].baseAdrs = PCI2CPU_IO_TRANSLATE ((UINT32)regBase);

      /* Get assigned interrupt level (line) register  */
      pciConfigInByte(bus, dev, fun, PCI_CFG_DEV_INT_LINE, &intrLvl);
      devInfo560[ unitNum ].intrLvl = (UINT32) intrLvl;

      /* Get Board ID */
      boardid = (sysInByte(devInfo560[unitNum].baseAdrs + ACP560_STAT_L)) & (UINT8)0x0f;

      devInfo560[ unitNum ].bus = bus;         /* Get Bus number */
      devInfo560[ unitNum ].dev = dev;         /* Get Device number */
      devInfo560[ unitNum ].func = fun;        /* Get Function number */
      devInfo560[ unitNum ].unitNum = unitNum; /* save the board unit number */
      devInfo560[ unitNum ].boardID = boardid; /* save the board uint number */

      unitNum++;          /* Update the board unit number         */
      intUnlock( lockKey );
    }

    bdIndex++;
  } while( unitNum < ACP560_MAX_BOARD );

  return( unitNum );
}

int
ACP560::DeviceInit(int unitNum) {
  UINT8 reg;

  /* Set Command reg (Enable Output) */
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_CMD),(UINT8)0x20);

  /* Clear FIFO --- Set Command reg (FIFO Clear)  */
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_CMD),(UINT8)0x10);

  /* Set Command reg (Enable Output) */  //test
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_CMD),(UINT8)0x20);

  /* Clear Mode Status --- Set Mode to 1Shot Enable and Trigger Disabled */
  reg = sysInByte(devInfo560[ unitNum ].baseAdrs + ACP560_MODE) & (UINT8)0xf0;
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_MODE), reg);

  /* Set Channel --- Set all 4 Channels */
  reg = sysInByte(devInfo560[ unitNum ].baseAdrs + ACP560_SCANCH) & (UINT8)0x0f;
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_SCANCH), reg);

  /* Set Sample Num = 1 sample for each channel */
  sysOutLong((devInfo560[ unitNum ].baseAdrs + ACP560_SAMPCNT_L), (UINT8)0x01);

  /* Sampling period = Timer Num (28) * Timer Unit (micro s) = 28 micro sec */
  /* Set Timer Num const. value = 28 (minimum value) */
  sysOutWord((devInfo560[ unitNum ].baseAdrs + ACP560_TIME_L),(UINT16)0x1c);

  /* Set Timer Unit = micro sec */
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_TIMEUNIT),(UINT8)0x00);
  return( OK );
}


void
ACP560::Set(int unitNum, int ch, double data) {
  Volt[unitNum][ch] = data;
}

void
ACP560::Write(int unitNum, BoardType type) {
  UINT16 comm;
  /* Clear FIFO --- Set Command reg (FIFO Clear)  */
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_CMD),(UINT8)0x10);

  /* Write FIFO */
  for (int i=0; i<4; i++) {
    comm = (UINT16)((Volt[unitNum][i] / 10 * 0x7fff ) + 0x7fff);
    sysOutWord((devInfo560[ unitNum ].baseAdrs + ACP560_FIFO_L),comm);
  }

  /* Sampling Start */
  sysOutByte((devInfo560[ unitNum ].baseAdrs + ACP560_CMD),(UINT8)0x03);
}


int
ACP560::CHNum(){
  return CH_NUM;
}

int
ACP560::BoardNum(){
  return board_num;
}


double*
ACP560::GetDataPtr(int unitNum, int ch){
  return &Volt[unitNum][ch];
}


