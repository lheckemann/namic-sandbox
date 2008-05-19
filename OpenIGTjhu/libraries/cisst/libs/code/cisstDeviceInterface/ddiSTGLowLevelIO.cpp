/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiSTGLowLevelIO.cpp,v 1.2 2005/09/26 15:41:46 anton Exp $
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


#include <cisstDeviceInterface/ddiSTGLowLevelIO.h>
#if CISST_HAS_STG

#include <sys/io.h>

extern "C" {
  unsigned short __STG_BaseAddress;

  unsigned short BrdtstOK(unsigned short baseaddress) {
    unsigned short brdtstaddress;
    unsigned short serseq, highnibble;
    int j;

    brdtstaddress = baseaddress + BRDTST;

    serseq = 0;
    for (j=7; j>=0; j--) {
      highnibble = inb(brdtstaddress) >> 4;
      if (highnibble & 8) {
        serseq |= 1 << (highnibble & 7);
      }
    }
    if (serseq == 0x74) {
      return 1;
    }
    return 0;
  }

  unsigned short BaseFind(void) {
    short i;
    unsigned short io_add;

    for (i = 15; i >= 0; i--) {
      io_add = i * 0x20 + 0x200;
      if (BrdtstOK(io_add))
        return io_add;
    }
    return (0xFFF);
  }

  void Init8255(void) {
    // Port D is input, the other bits are need to make CNTRL0
    // register work. Refer to pg 18 of the manual.
    // Bits |1|0|0|0|DH|0|1|DL|
    //      |1|0|0|0|0 |0|1|0 |
    // 1 = Input, and 0 = output
    outb(0x82, __STG_BaseAddress + MIO_2);

    // Port A, B & C are inputs
    // Bits |1|0|0|A|CH|0|B|CL|
    //      |1|0|0|1|1 |0|1|1 |
    // 1 = Input, and 0 = output
    outb(0x9b, __STG_BaseAddress + ABC_DIR);
  }

  void SetDDir(unsigned short setoutorin, unsigned short port) {
    unsigned char data;

    data = inb(__STG_BaseAddress + ABC_DIR);
    if (setoutorin == 1) {/*input*/
      switch (port) {
        case PORT_A:
          data |= 0x10;
          break;
        case PORT_B:
          data |= 0x02;
          break;
        case PORT_C:
          data |= 0x09;
      }
    }
    else {/*output*/
      switch (port) {
        case PORT_A:
          data &= 0x8B;
          break;
        case PORT_B:
          data &= 0x99;
          break;
        case PORT_C:
          data &= 0x92;
          break;
      }
    }
    outb(data, __STG_BaseAddress + ABC_DIR);
  }

  void RawDO(unsigned char bitnumber, unsigned char set0or1,
      unsigned short port) {
    unsigned char data;

    data = inb(__STG_BaseAddress + port);
    if (set0or1)
      data |= 1 << bitnumber;
    else
      data &= ~(1 << bitnumber);

    outb(data, __STG_BaseAddress + port);
  }

  void RawDAC(unsigned short axis, long counts) {
    //input / output:

    //    lCounts (decimal) ... -lCounts ... +0x1000 ... volts
    //
    //     0x1000  (4096)     0xfffff000           0       +10
    //          0                      0      0x1000         0
    // 0xfffff001 (-4095)          0xfff      0x1fff       -10

    // So, the domain might be different than you expected. I expected:
    //     0xf000 (-4096)  to  0xfff (4095), rather than
    //     0xf001 (-4095)  to 0x1000 (4096)

    // reverse slope so positive counts give positive voltage
    counts = -counts;

    // shift for DAC
    counts += 0x1000;

    if (counts > 0x1FFF)
      counts = 0x1FFF;
    if (counts < 0)
      counts = 0;
    outw((unsigned short)counts, __STG_BaseAddress + DAC_0 + (axis << 1));
  }

  void EncoderInit(void) {
    unsigned short add;

    for (add = __STG_BaseAddress + CNT0_C; add <= __STG_BaseAddress + CNT6_C; add +=4) {
      outw(0x2020, add);    //master reset
      outw(0x6868, add);    //set counter cmd register - input control
      outw(0x8080, add);    //set counter cmd register - output control
      outw(0xc3c3, add);    //set counter cmd register - quadrature
      outw(0x0404, add);    //reset counter to zero
    }
  }

  void EncoderLatch(void) {
    unsigned short add;

    for (add = __STG_BaseAddress + CNT0_C; add <= __STG_BaseAddress + CNT6_C; add +=4)
      outw(0x0303, add);
  }

  /* this is some real slimy slick thing that the stg does to speed up reads
   * of encoders. the registers are arranged like data, data, control, control
   * one can do two at a time by using word operations on them instead of byte
   * real slimy isnt it!
   */
  unsigned char OldByte2[STG_MAX_AXIS];
  unsigned char EncHighByte[STG_MAX_AXIS];

  void EncoderReadAll(LONGBYTE *enc) {
    WORDBYTE transfer;
    short i;
    short add;

    //reset counter internal addr ptr to point to first byte
    for (add = __STG_BaseAddress + CNT0_C; add <= __STG_BaseAddress + CNT6_C; add +=4)
      outw(0x0101, add);

    for (i = 0; i < 3; i++) {  // 24 bits means get 3 bytes each

      transfer.Word = inw(__STG_BaseAddress + CNT0_D);
      enc[0].Byte[i] = transfer.Byte.high;
      enc[1].Byte[i] = transfer.Byte.low;

      transfer.Word = inw(__STG_BaseAddress + CNT2_D);

      enc[2].Byte[i] = transfer.Byte.high;
      enc[3].Byte[i] = transfer.Byte.low;

      transfer.Word = inw(__STG_BaseAddress + CNT4_D);

      enc[4].Byte[i] = transfer.Byte.high;
      enc[5].Byte[i] = transfer.Byte.low;

      transfer.Word = inw(__STG_BaseAddress + CNT6_D);

      enc[6].Byte[i] = transfer.Byte.high;
      enc[7].Byte[i] = transfer.Byte.low;
    }

    // maintain the high byte, to extend the counter to 32 bits
    // base decisions to increment or decrement the high byte
    // on the highest 2 bits of the 24 bit value.  To get the
    // highest 2 bits, use 0xc0 as a mask on byte [2] (the third
    // byte).

    for (i = 0; i < STG_MAX_AXIS; i++) {
      // check for -1 to 0 transition

      if (((OldByte2[i]    & 0xc0) == 0xc0 )   // 11xxxxxx
          && ((enc[i].Byte[2] & 0xc0) == 0 ))  // 00xxxxxx
        EncHighByte[i]++;

      // check for 0 to -1 transition

      if (((OldByte2[i]    & 0xc0) == 0 )    // 00xxxxxx
          && ((enc[i].Byte[2] & 0xc0) == 0xc0 )) // 11xxxxxx
        EncHighByte[i]--;

      enc[i].Byte[3] = EncHighByte[i];
      OldByte2[i] = enc[i].Byte[2];    // current byte 2 becomes old one
    }
  }

  void EncoderSet(unsigned short axis, LONGBYTE count) {
    unsigned short add;

    add = __STG_BaseAddress + CNT0_D;
    add += (axis & 0x06) << 1;

    add += axis & 1;
    outb(count.Byte[0], add);
    outb(count.Byte[1], add);
    outb(count.Byte[2], add);

    outb(0x09, add + 2);
    EncHighByte[axis] = count.Byte[3];
    OldByte2[axis] = count.Byte[2];
  }

  void SetIrq(unsigned short requestedirq)
  {
    unsigned char intreg;
    //wIrq = requestedirq;  // assume it's OK for now, check later
    intreg = 0x88;    // cal high too, not calibrating ADC

    // now put low bits into intreg to select irq
    switch (requestedirq) {
      case 3: break;  // add zero
      case 5: intreg |= 4;
        break;
      case 7: intreg |= 2;
        break;
      case 9: intreg |= 6;
        break;
      case 10: intreg |= 5;
         break;
      case 11: intreg |= 7;
         break;
      case 12: intreg |= 3;
         break;
      case 15: intreg |= 1;
         break;
      default: //wIrq = 5;  // ERROR, requested irq not valid, use 5
         intreg |= 4;  // There is no safe value, leaving zero
         // here would select IRQ 3 which is worse
         // than 5 because IRQ 3 is usually for COM 2
         break;
    }
    outb(intreg, __STG_BaseAddress + CNTRL0);
  }

  void SelectInterruptPeriod(long periodselect) {

    if (periodselect != MAX_PERIOD) {
      outb(0x56, __STG_BaseAddress + TMRCMD);  // timer 1, read/load LSB (MSB is 0)
      // mode 3 (square wave)
      outb(0xb4, __STG_BaseAddress + TIMER_1);  // 0xb4 = 180 -> 25 uSec period
    } else {
      outb(0x76, __STG_BaseAddress + TMRCMD);  // timer 1, read/load LSB then MSB
      // mode 3 (square wave)
      outb(0xff, __STG_BaseAddress + TIMER_1);  // LSB
      outb(0xff, __STG_BaseAddress + TIMER_1);  // MSB
    }

    switch (periodselect) {
      case _500_MICROSECONDS:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0x14, __STG_BaseAddress + TIMER_0);  // 0x14 = 20 = .5 mS
        outb(0x00, __STG_BaseAddress + TIMER_0);
        break;
      case _1_MILLISECOND:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0x28, __STG_BaseAddress + TIMER_0);  // 0x28 = 40 = 1 mS
        outb(0x00, __STG_BaseAddress + TIMER_0);
        break;
      case _2_MILLISECONDS:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0x50, __STG_BaseAddress + TIMER_0);  // 0x50 = 80 = 2 mS
        outb(0x00, __STG_BaseAddress + TIMER_0);
        break;
      case _3_MILLISECONDS:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0x78, __STG_BaseAddress + TIMER_0);  // 0x78 = 120 = 3 mS
        outb(0x00, __STG_BaseAddress + TIMER_0);
        break;
      case _4_MILLISECONDS:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0xa0, __STG_BaseAddress + TIMER_0);  // 0xA0 = 160 = 4 mS
        outb(0x00, __STG_BaseAddress + TIMER_0);
        break;
      case _5_MILLISECONDS:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0xc8, __STG_BaseAddress + TIMER_0);  // 0xC8 = 200 = 5 mS
        outb(0x00, __STG_BaseAddress + TIMER_0);
        break;
      case _10_MILLISECONDS:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0x90, __STG_BaseAddress + TIMER_0);  // 0x0190 = 400 = 10 mS
        outb(0x01, __STG_BaseAddress + TIMER_0);
        break;
      case _100_MILLISECONDS:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0xa0, __STG_BaseAddress + TIMER_0);  // 0x0FA0 = 4000 = 100 mS
        outb(0x0f, __STG_BaseAddress + TIMER_0);
        break;
      case _1_SECOND:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0x40, __STG_BaseAddress + TIMER_0);  // 0x9C40 = 40000 = 1 S
        outb(0x9c, __STG_BaseAddress + TIMER_0);
        break;
      case MAX_PERIOD:
        outb(0x34, __STG_BaseAddress + TMRCMD);  // timer 0, read/load LSB followed by
        // MSB, mode 2 (real-time interrupt)
        outb(0xff, __STG_BaseAddress + TIMER_0);  // LSB
        outb(0xff, __STG_BaseAddress + TIMER_0);  // MSB
        break;
      default:
        // wrong input? then don't change it
        break;
    }
  }
} // extern "C"

#endif // CISST_HAS_STG


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiSTGLowLevelIO.cpp,v $
// Revision 1.2  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.2  2004/04/08 06:11:47  kapoor
// Added conditional compilation
//
// Revision 1.1  2004/03/22 00:30:53  kapoor
// Added new devices.
//
// ****************************************************************************
