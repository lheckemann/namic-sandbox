/***************************************************************************
 * FileName      : acp550.h
 * Created       : 2007/07/21
 * LastModified  : 
 * Author        : Hiroaki KOZUKA
 * Aim           : ACP550 Driver for a Robot Control
 ***************************************************************************/

#include <vxWorks.h>
#include <taskLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <semLib.h>
#include <sysLib.h>
#include <logLib.h>
#include <intLib.h>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"


/* ACP-500 Max Board  */
#define ACP550_MAX_BOARD             7

// Memory map definition. (refer to WTN-49 for the details.)
#define PCI2CPU_IO_TRANSLATE(x) ((int)(x) + (PCI_MSTR_IO_LOCAL_B - PCI_MSTR_IO_BUS_B))

// PCI Vender ID and PCI Device ID
#define PCI_DEVICE_550  0x9050      // DeviceID device of PCI9050
#define PCI_VENDOR_550  0x10b5      // VenderID PLX Technology
#define PCI_SYSTEM_550  0xf015      // ACP-550
#define PCI_SVENDR_550  0x1264      // avaldata

// Definitions to internal register offset of ACP-550
#define ACP550_CMD_ADRS             0x00
#define ACP550_STAT_ADRS            0x00
#define ACP550_INT_CLR_ADRS         0x03
#define ACP550_INT_STAT_ADRS        0x03
#define ACP550_MODE_ADRS            0x04
#define ACP550_INT_PERMIT_ADRS      0x05
#define ACP550_SLOT_NUM_ADRS        0x08
#define ACP550_SLOT_PTR_ADRS        0x09
#define ACP550_SLOT_CH_ADRS         0x0a
#define ACP550_SLOT_RANGE_ADRS      0x0b
#define ACP550_SAMP_CNT_ADRS        0x0c
#define ACP550_TIME_ADRS            0x10
#define ACP550_TIME_UNIT_ADRS       0x12
#define ACP550_AD_ADRS              0x14
#define ACP550_FIFO_DATA_ADRS       0x18
#define ACP550_REST_SAMP_CNT_ADRS   0x1c
#define ACP550_TRG_LVL_ADRS         0x20
#define ACP550_EPROM_ADRS           0x3a
#define ACP550_EPROM_READ_ADRS      0x3e
#define ACP550_EPROM_INST_ADRS      0x3e

/* Range list (offset:0x0b) */
#define ACP550_SING_0_10V   0x00
#define ACP550_SING_0_5V    0x01
#define ACP550_SING_0_2V    0x02
#define ACP550_SING_0_1V    0x03
#define ACP550_SING_10V     0x04
#define ACP550_SING_5V      0x05
#define ACP550_SING_2V      0x06
#define ACP550_SING_1V      0x07
#define ACP550_DIFF_0_10V   0x08
#define ACP550_DIFF_0_5V    0x09
#define ACP550_DIFF_0_2V    0x0a
#define ACP550_DIFF_0_20mA  0x0a
#define ACP550_DIFF_0_1V    0x0b
#define ACP550_DIFF_10V     0x0c
#define ACP550_DIFF_5V      0x0d
#define ACP550_DIFF_2V      0x0e
#define ACP550_DIFF_1V      0x0f

/* EPROM Read Instruction */
#define ACP550_EPROM_READ           0x1d

/* Board ID Mask */
#define ACP550_STAT_ID03_MASK       0x0f

// PPC is big endian
#define PCISWAP(x)       LONGSWAP(x)
#define PCISWAP_SHORT(x) (MSB(x) | LSB(x) << 8)

// Control block structure for ACP-550
typedef struct {
    int     bus;                /* PCI Bus Number           */
    int     dev;                /* PCI Device Number        */
    int     func;               /* PCI Function Number      */
    UINT32  plxBase;            /* PCI9050 Local Address    */
    UINT32  baseAdrs;           /* ACP-550 Register Address */
    UINT32  intrLvl;            /* Interrupt Level          */
    UINT8   boardID;            /* ACP-550 Board ID         */
    int     unitNum;            /* unit Number              */
    UINT8   intStat;            /* Interrupt Stat           */
    VOIDFUNCPTR callback;       /* callback Function Pointer*/
} ACP550_INFO;

class ACP550 {
 private:
  int base_address;
  double D1, D2;
  static const int CH_NUM = 4;
  ACP550();
  int DeviceInit(int unitNum, int slotNum, int range ,double *D1, double *D2);
  int DeviceLookup();
  int interruptHandler();
 public:
  ~ACP550();
  int board_num;
  double buf;
  ACP550_INFO devInfo550[ ACP550_MAX_BOARD ];
  static ACP550* m_Instance;
  static ACP550* getInstance();
  void Read( int unitNum, double* data, int dataSize );
  int CHNum();
  int BoardNum();
};


