/***************************************************************************
 * FileName      : ACP420.h
 * Created       : 2007/08/06
 * LastModified  : 2007/08/11
 * Author        : Hiroaki KOZUKA
 * Aim           : ACP420 Driver for a Robot Control
 ***************************************************************************/

#ifndef ACP420__H
#define ACP420__H

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

/* PCI vender ID & device ID */
#define PCI_DEVICE_ID_COSMO 0xA004      /* device ID  PCPG-46 */
#define PCI_DEVICE_ID_AVAL  0xA001      /* device ID  ACP-420 */
#define PCI_VENDER_ID       0x136C      /* vender ID */
/* Board Type   */
#define ACP_420_BOARD       0           /* ACP-420 */
#define PCPG_46_BOARD       1           /* PCPG-46 */

/* PCI9050 */
#define PCI_CFG_BASE_ADDRESS1   0x14    /* PLXTec. */
/* BSN  */
#define ACP420_SLOT_NUM                             16
/* BSN switch I/O address */
#define ACP420_PORT_BSN_SWITCH                      0x20
/* I/O range */
#define ACP420_IO_RANGE_PMC540                      0x08

/* I/O address  */
#define ACP420_PORT_DATA1                           0x00
#define ACP420_PORT_DATA2                           0x01
#define ACP420_PORT_DATA3                           0x02
#define ACP420_PORT_DATA4                           0x03
#define ACP420_PORT_COMMAND                         0x04
#define ACP420_PORT_MODE1                           0x05
#define ACP420_PORT_MODE2                           0x06
#define ACP420_PORT_SIGNAL                          0x07

/* Board/Uint Number conversion table  */
typedef struct _ACP420_BSN_CNV {
  int munitNum;      /* Uint number */
} ACP420_BSN_CNV;

// Memory map definition. (refer to WTN-49 for the details.)
#define PCI2CPU_IO_TRANSLATE(x)  ((int)(x) + (PCI_MSTR_IO_LOCAL_B - PCI_MSTR_IO_BUS_B))

/// ACP420 Infomation Structure
typedef struct _ACP420_INF {
  int         mbus;                   // Bus Number
  int         mdev;                   // Device Bumber
  int         mfunc;                  // Function Number
  int         munitNum;               // Unit Number
  BOOL        museFlag;               // Compatibility for Windows ver.
  UINT8       mboardType;             // Board type
  UINT32      mregBase;               // Base register I/O address
  UINT32      mdpSW;                  // Board number
  UINT32      mintLvl;                // Interrupt level
} ACP420_INF;

/// Counter/Input board driver
class ACP420 : public BoardBase{
private:
  /// Board base address
  int base_adress;

  /// Number of board
  int board_num;

  /// Board information struct
  ACP420_INF acp420Inf[ACP420_SLOT_NUM];

  /// Number of board ch
  static const int CH_NUM = 4;

  /// Count data
  long** Cnt;

  /// Input data
  bool** In;

  /// Initialize board
  /// \param unitNum Unit number(0~15)
  int DeviceInit(int unitNum);

  /// board loop up
  // \return Number of board
  int DeviceLookup();

  /// Read input data
  /// \param unitNum Unit number(0~15)
  void ReadIn(int unitNum);

  /// Read counter data
  void ReadCnt(int unitNum);

public:
  /// A constructor
  ACP420();

  /// A destructor
  ~ACP420();

  /// Write count data to board
  /// \param unitNum Unit number(0~15)
  void Write(int unitNum);

  /// Read data(count/input) from board
  /// \param unitNum Unit number(0~15)
  /// \param type Count/input (0==Count, 1==input)
  void Read(int unitNum, BoardType type);

  /// Get count data
  /// \param unitNum Unit number(0~15)
  /// \param ch Ch
  long GetCount(int unitNum, int ch);

  /// Get input data
  /// \param unitNum Unit number(0~15)
  /// \param ch Ch
  bool GetIn(int unitNum, int ch);

  /// Set count data
  /// \param unitNum Unit number(0~15)
  /// \param ch Ch
  /// \param data Count data
  void Set(int unitNum, int ch, long data);

  /// Get number of ch
  /// \return number of ch
  int CHNum();

  /// Get number of board
  /// \return number of board
  int BoardNum();

  /// Get count data pointer
  /// \param unitNum Unit number(0~15)
  /// \param ch Ch
  long* GetCntDataPtr(int unitNum, int ch);

  /// Get input data potinter
  /// \param unitNum Unit number(0~15)
  /// \param ch Ch
  bool* GetInDataPtr(int unitNum, int ch);
};

#endif // ACP420__H

