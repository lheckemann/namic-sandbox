/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiSTGLowLevelIO.h,v 1.3 2005/09/26 15:41:46 anton Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************


/*!
  \file
  \brief Defines the low level IOs for ServoToGo
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiSTGLowLevelIO_h
#define _ddiSTGLowLevelIO_h

#include <cisstConfig.h>
#if CISST_HAS_STG || defined DOXYGEN


#define STG_SUCCESS  0
#define STG_FAILURE  1

#define STG_NUM_AXIS_IN_SYS     8
#define STG_MAX_AXIS            8

//
// digital I/O port stuff
//
// begin to use the enumerated type, rather than the defines
//
typedef enum { A_dir = 0x01, B_dir = 0x02, C_lo_dir = 0x04,
               C_hi_dir = 0x08, C_dir = 0x0c, D_lo_dir = 0x10,
         D_hiDir = 0x20, D_dir = 0x30 }   eDir_typ;
typedef enum { A_port, B_port, C_port, D_port } ePort_typ;
typedef enum { HomeSwitchFORWARD, HomeSwitchREVERSE } eHomeDir_typ;

#define STG_PORT_A        0x01
#define STG_PORT_B        0x02
#define STG_PORT_C_LO     0x04
#define STG_PORT_C_HI     0x08
#define STG_PORT_C        (STG_PORT_C_LO | STG_PORT_C_HI)
#define STG_PORT_D_LO     0x10
#define STG_PORT_D_HI     0x20
#define STG_PORT_D        (STG_PORT_D_LO | STG_PORT_D_HI)
#define STG_PORT_INPUT    0
#define STG_PORT_OUTPUT   1

/* parameters to the SelectInterruptPeriod Command */
#define _500_MICROSECONDS        500
#define _1_MILLISECOND           1000
#define _2_MILLISECONDS          2000
#define _3_MILLISECONDS          3000
#define _4_MILLISECONDS          4000
#define _5_MILLISECONDS          5000
#define _10_MILLISECONDS         10000
#define _100_MILLISECONDS        100000L
#define _1_SECOND                1000000L
#define MAX_PERIOD               -1L

/*
 * offsets.h
 *
 * Define offsets for various registers on Servo To Go, Inc. Servo I/O card.
 *
 * Copyright (c) 1995, 1998 Servo To Go, Inc.
 * 8117 Groton Ln.
 * Indianapolis, IN 46260
 *
 * For registers in the "high page", an offset of 0x400 is included.
 */

#define CNT0_D     0x00
#define CNT1_D     0x01
#define CNT0_C     0x02
#define CNT1_C     0x03
#define CNT2_D     0x04
#define CNT3_D     0x05
#define CNT2_C     0x06
#define CNT3_C     0x07
#define CNT4_D     0x08
#define CNT5_D     0x09
#define CNT4_C     0x0a
#define CNT5_C     0x0b
#define CNT6_D     0x0c
#define CNT7_D     0x0d
#define CNT6_C     0x0e
#define CNT7_C     0x0f
#define DAC_0      0x10
#define DAC_1      0x12
#define DAC_2      0x14
#define DAC_3      0x16
#define DAC_4      0x18
#define DAC_5      0x1a
#define DAC_6      0x1c
#define DAC_7      0x1e
#define ADC        0x410
#define ADC_0      0x410
#define ADC_1      0x412
#define ADC_2      0x414
#define ADC_3      0x416
#define ADC_4      0x418
#define ADC_5      0x41a
#define ADC_6      0x41c
#define ADC_7      0x41e
#define CNTRL0     0x401     /* Model 2 */
#define DIO_A      0x400     /* Model 1 */
#define DIO_B      0x402     /* Model 1 */
#define DIO_C      0x404     /* Model 1 */
#define DIO_D      0x401     /* Model 1 */
#define PORT_A     0x400     /* Model 2, replaces DIO_A */
#define PORT_B     0x402     /* Model 2, replaces DIO_B */
#define PORT_C     0x404     /* Model 2, replaces DIO_C */
#define PORT_D     0x405     /* Model 2, replaces DIO_D */
#define INTC       0x405     /* Model 1 */
#define BRDTST     0x403
#define MIO_1      0x406
#define ABC_DIR    0x406     /* Model 2 */
#define MIO_2      0x407
#define D_DIR      0x407     /* Model 2 */
#define ODDRST     0x407
#define TIMER_0    0x408
#define TIMER_1    0x40a
#define TIMER_2    0x40c
#define TMRCMD     0x40e
#define ICW1       0x409
#define ICW2       0x40b
#define OCW1       0x40b
#define OCW2       0x409
#define OCW3       0x409
#define IRRreg     0x409   // there's something called IRR in ntddk.h
#define ISR        0x409
#define IDLEN      0x409
#define IMR        0x40b
#define SELDI      0x40b
#define IDL        0x40d
#define CNTRL1     0x40f

/*
 * Some bit masks for various registers
 */
// for IRR, ISR, and IMR
#define IXEVN      0x80
#define IXODD      0x40
#define LIXEVN     0x20
#define LIXODD     0x10
#define EOC        0x08
#define TP0        0x04
#define USR_INT    0x02
#define TP2        0x01
// for INTC
#define AUTOZERO   0x80
#define IXLVL      0x40
#define IXS1       0x20
#define IXS0       0x10
#define USRINT     0x08
#define IA2        0x04
#define IA1        0x02
#define IA0        0x01

#define CNTRL0_AZ   0x80
#define CNTRL0_AD2  0x40
#define CNTRL0_AD1  0x20
#define CNTRL0_AD0  0x10
#define CNTRL0_CAL  0x08
#define CNTRL0_IA2  0x04
#define CNTRL0_IA1  0x02
#define CNTRL0_IA0  0x01

#define CNTRL1_WDTOUT    0x80
#define CNTRL1_INT_G2    0x40
#define CNTRL1_INT_T0    0x10
#define CNTRL1_INT_T2    0x20
#define CNTRL1_NOT_SLAVE 0x08
#define CNTRL1_SLAVE     0x00
#define CNTRL1_IEN_G2    0x04
#define CNTRL1_IEN_T0    0x01
#define CNTRL1_IEN_T2    0x02

#define BRDTST_EOC       0x08

/*
 * We're hardwired to irq 5 (sorry).  This is normally reserved for
 * lpt2, but most people don't have two printers.  Sometimes a modem,
 * sound card, etc. will be using this interrupt.  It would be easiest
 * to remove any offending card temporarily.  Otherwise you'll have
 * to figure out how to change the following define's.
 */
/*
#define INTR 13              // irq5 is short 13
#define INT_MASK 0x20        // mask for motherboard interrupt controller
#define IC_ADD   0x20        // address of the interrupt controller on your
                             // motherboard (NOT the one on the Servo To Go
                             // board).  For low numbered interrupts (0-7)
                             // it's 0x20, for higher numbered interrupts
                             // (8-15) it's 0xa0
#define EOI_5    0x65        // Specific EOI for IRQ 5 (used with above)
#define IC_MASK  0x21        // The interrupt mask register on your
                             // motherboard
*/

#define  IRQSL   0x84        // IRQ selection for INTC register
// end of irq related defines

extern "C" {
  extern unsigned short __STG_BaseAddress;

  typedef union {
    long Long;
    unsigned char Byte[4];
  } LONGBYTE;

  typedef union {
    unsigned short Word;
    struct {unsigned char high, low;} Byte;
  } WORDBYTE;

  unsigned short BrdtstOK(unsigned short);
  unsigned short BaseFind(void);

  void Init8255(void);
  void SetDDir(unsigned short, unsigned short);
  void RawDO(unsigned char, unsigned char, unsigned short);
  void RawDAC(unsigned short, long);

  void EncoderInit(void);
  void EncoderLatch(void);
  void EncoderReadAll(LONGBYTE *);
  void EncoderSet(unsigned short, LONGBYTE);

  void SetIrq(unsigned short);
  void SelectInterruptPeriod(long);
} // extern "C"


#endif // CISST_HAS_STG

#endif // _ddiSTGLowLevelIO_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiSTGLowLevelIO.h,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2004/05/27 19:34:22  anton
// Added || defined DOXYGEN to all #ifdef CISST_HAS_XYZ so that doxygen can
// create the documentation.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.2  2004/04/08 06:00:08  kapoor
// Added conditional compilation
//
// Revision 1.1  2004/03/22 00:31:20  kapoor
// Added new devices.
//
// ****************************************************************************
