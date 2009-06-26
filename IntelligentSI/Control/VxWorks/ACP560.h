/***************************************************************************
 * FileName      : ACP560.h
 * Created       : 2007/06/15
 * LastModified  :
 * Author        : Jumpei ARATA
 * Aim           : ACP560 Driver for a Robot Control
 ***************************************************************************/

#ifndef ACP560__H
#define ACP560__H

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
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#include "BoardBase.h"

#define ACP560_MAX_BOARD 5

/* PCI Vender ID and PCI Device ID */
#define PCI_DEVICE_560   0x9050      /* Device ID¡§ PCI9050 Device  */
#define PCI_VENDOR_560   0x10b5      /* Vender ID¡§ PLX Technology  */
#define PCI_SYSTEM_560   0xf016      /* ACP-560          */
#define PCI_SVENDR_560   0x1264      /* AVAL             */

/* Definitions to internal register offset of ACP-560  */
#define ACP560_CMD       0x00
#define ACP560_STAT_L    0x00
#define ACP560_STAT_H    0x01
#define ACP560_INT       0x03
#define ACP560_MODE      0x04
#define ACP560_INTPERM   0x05
#define ACP560_SCANCH    0x06
#define ACP560_SAMPCNT_L 0x08
#define ACP560_TIME_L    0x0c
#define ACP560_TIME_H    0x0d
#define ACP560_TIMEUNIT  0x0e
#define ACP560_CH1       0x10
#define ACP560_CH2       0x12
#define ACP560_CH3       0x14
#define ACP560_CH4       0x16
#define ACP560_FIFO_L    0x18
#define ACP560_FIFO_H    0x19

// Memory map definition. (refer to WTN-49 for the details.)
#define PCI2CPU_IO_TRANSLATE(x)  ((int)(x) + (PCI_MSTR_IO_LOCAL_B - PCI_MSTR_IO_BUS_B))

/// Control block structure for ACP560
typedef struct {
  int     bus;                /* PCI Bus Number                */
  int     dev;                /* PCI Device Number             */
  int     func;               /* PCI Function Number           */
  UINT32  plxBase;            /* Address to I/O mapped PCI9050 */
  UINT32  baseAdrs;           /* ACP-560 IO Address                */
  UINT32  intrLvl;            /* Interrupt Level               */
  UINT8   boardID;            /* ACP-560 Bord ID               */
  int     unitNum;                /* unit Number                           */
  UINT32  runMode;
  UINT32  EnableIntStat;
  UINT32  isrIntStat;
  SEM_ID  semIntSem;
  VOIDFUNCPTR callback;           /* callback Function Pointer*/
} ACP560_INFO;

/// D/A board driver
class ACP560 : public BoardBase{
private:
  /// board base asddress
  int base_address;

  /// Device initialize
  /// \param unitNum Board unit number
  /// \return
  int DeviceInit(int unitNum);

  /// Seek board
  /// \return
  int DeviceLookup();

  /// Interrupt handdler
  /// \return
  int interruptHandler();

  /// Board ch
  static const int CH_NUM=4;

  /// Board number
  int board_num;

  /// Voltage
  double** Volt;

public:
  /// A constructor
  ACP560();

  /// A destructor
  ~ACP560();

  /// ACP560 Information
  ACP560_INFO devInfo560[ ACP560_MAX_BOARD ];

  /// Set volt
  /// \param unitNum Board unit number
  /// \pram ch Ch
  /// \param data voltage
  void Set(int unitNum, int ch, double data);

  /// Write volt to DA Board
  void Write(int unitNum, BoardType type);

  /// Get board ch
  /// \return Board number of ch
  int CHNum();

  /// Get number of board
  /// \return number of board
  int BoardNum();

  /// Get voltage data potinter
  /// \param unitNum Board unit number
  /// \pram ch Ch
  /// \return Voltage data potinter
  double* GetDataPtr(int unitNum, int ch);
};


#endif // ACP560__H

