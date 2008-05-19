/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiLoPoMoCoLowLevelIO.h,v 1.5 2006/02/24 19:32:09 pkaz Exp $
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
  \brief Defines the low level IOs for the JHU LoPoMoCo 
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiLoPoMoCoLowLevelIO_h
#define _ddiLoPoMoCoLowLevelIO_h

#include <cisstConfig.h>
#if CISST_HAS_LOPOMOCO || defined DOXYGEN

#include <sys/io.h>
#include <unistd.h>
/*
 * Register addresses
 */
#define VERSION_REGISTER      0
#define CMD_REGISTER        2
#define STATUS_REGISTER        2
#define DATA_REGISTER        4
#define AD_FIFO_REGISTER      6
#define DA_MOTOR_VOLTAGE_REGISTER    8
#define DA_CURRENT_LIMIT_REGISTER    10
#define ENCODER_LOW_REGISTER      12
#define ENCODER_HIGH_REGISTER      14
#define ENCODER_PERIOD        16
#define ENCODER_FREQUENCY      18
#define UNUSED_0        20

/*
 * Functions 
 */
/* Set functions */
#define IOCMD_SET_MOTOR_STATE      0x0000
#define IOCMD_SET_ALL_INDICES      0x0400
#define IOCMD_SET_MOTOR_VOLTAGE_INDICES    0x0800
#define IOCMD_SET_CURRENT_LIMIT_INDICES    0x0C00
#define IOCMD_SET_ENCODER_INDICES    0x1000
#define IOCMD_SET_CURRENT_FEEDBACK_MAX_INDEX  0x1400
#define IOCMD_SET_POT_FEEDBACK_MAX_INDEX  0x1800
#define IOCMD_SET_AD_INTERRUPT_NUMBER    0x1C00
#define IOCMD_SET_TIMER_INTERRUPT_NUMBER  0x2000
#define IOCMD_SET_AD_INTERRUPT      0x2400
#define IOCMD_SET_TIMER_INTERRUPT    0x2800
#define IOCMD_SET_WDT_PERIOD      0x2C00
#define IOCMD_SET_TIMER_PERIOD      0x3000
//#define Unused        0x3400
//#define Unused        0x3800
//#define Unused        0x3C00

/* Get functions */
#define IOCMD_GET_MOTOR_STATE      0x4000
//#define Unused        0x4400
#define IOCMD_GET_MOTOR_VOLTAGE_INDICES    0x4800
#define IOCMD_GET_CURRENT_LIMIT_INDICES    0x4C00
#define IOCMD_GET_ENCODER_INDICES    0x5000
#define IOCMD_GET_CURRENT_FEEDBACK_MAX_INDEX  0x5400
#define IOCMD_GET_POT_FEEDBACK_MAX_INDEX  0x5800
#define IOCMD_GET_AD_INTERRUPT_NUMBER    0x5C00
#define IOCMD_GET_TIMER_INTERRUPT_NUMBER  0x6000
#define IOCMD_GET_AD_INTERRUPT      0x6400
#define IOCMD_GET_TIMER_INTERRUPT    0x6800
#define IOCMD_GET_WDT_PERIOD      0x6C00
#define IOCMD_GET_TIMER_PERIOD      0x7000
#define IOCMD_GET_CURRENT_TIME      0x7400
//#define Unused        0x7800
//#define Unused        0x7C00

/* Pulse functions */
#define IOCMD_LOAD_MOTOR_VOLTAGES    0x8000
#define IOCMD_LOAD_CURRENT_LIMITS    0x8400
#define IOCMD_LOAD_MOTOR_VOLTAGES_CURRENT_LIM  0x8800
#define IOCMD_LATCH_ENCODERS      0x8C00
#define IOCMD_PRELOAD_ENCODERS      0x9000
#define IOCMD_START_CONV_CURRENT_FEEDBACK  0x9400
#define IOCMD_START_CONV_POT_FEEDBACK    0x9800
#define IOCMD_CLEAR_AD_INTERRUPT    0x9C00
#define IOCMD_CLEAR_TIMER_INTERRUPT    0xA000
#define IOCMD_RESET_CURRENT_FEEDBACK    0xA400
#define IOCMD_RESET_POT_FEEDBACK    0xA800
//#define Unused        0xAC00
//#define Unused        0xB000
//#define Unused        0xB400
//#define Unused        0xB800
//#define Unused        0xBC00


#ifdef _BORLANDC_
#define outl (data, address)  outpl (address, data)
#define inl (address)    inpl (address)
#define outw (data, address)  outpw (address, data)
#define inw (address)    inpw (address)
#define outb (data, address)  outpb (address, data)
#define inb (address)    inpb (address)
#endif

/*!
  \ingroup cisstDeviceInterface

  This class is the lowest level interface to the Low Power Motor
  Controller Board.  All methods in this class are inlined and issue
  one or more low level IO commands Since it has outw/inw MACROS, -O
  or -O2 optimization must be turned on especially under Linux/GCC.
  
  This class is main intention to provide a clearner looking upper level code, and
  also to deal with DOS vs Linux differences in low level codes
 */
class ddiLoPoMoCoLowLevelIO {
 private:
    unsigned int numTries;
    unsigned short BaseAddress;
    
 public:
    ddiLoPoMoCoLowLevelIO(unsigned int baseAddress = 0x220) {BaseAddress = baseAddress;numTries = 0;}
    /* Set functions */
    inline void SetMotorState(unsigned short mask, unsigned short state) {
        outw(IOCMD_SET_MOTOR_STATE | ((mask & 0x0F) << 4) | (state & 0x0F),
             BaseAddress + CMD_REGISTER);
    }
    
    inline void SetAllIndices(bool autoIncrement, unsigned short maxIndex, unsigned short initIndex) {
        outw(IOCMD_SET_ALL_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03),
             BaseAddress + CMD_REGISTER);
    }

    inline void SetMotorVoltageIndices(bool autoIncrement, unsigned short maxIndex, unsigned short initIndex) {
        outw(IOCMD_SET_MOTOR_VOLTAGE_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03),
             BaseAddress + CMD_REGISTER);
    }
  
    inline void SetCurrentLimitIndices(bool autoIncrement, unsigned short maxIndex, unsigned short initIndex) {
        outw(IOCMD_SET_CURRENT_LIMIT_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03),
             BaseAddress + CMD_REGISTER);
    }

  inline void SetEncoderIndices(bool autoIncrement, unsigned short maxIndex, unsigned short initIndex) {
        outw(IOCMD_SET_ENCODER_INDICES | (autoIncrement?0x0010:0x0000) | ((maxIndex & 0x03) << 2) | (initIndex & 0x03),
             BaseAddress + CMD_REGISTER);
    }
  
    inline void SetCurrentFeedbackMaxIndex(unsigned short maxIndex)  {
        outw(IOCMD_SET_CURRENT_FEEDBACK_MAX_INDEX | ((maxIndex & 0x03) << 2),
             BaseAddress + CMD_REGISTER);
    }

  inline void SetPotFeedbackMaxIndex(unsigned short maxIndex)  {
        outw(IOCMD_SET_POT_FEEDBACK_MAX_INDEX | ((maxIndex & 0x03) << 2),
             BaseAddress + CMD_REGISTER);
    }
    
  inline void SetADInterruptNumber(unsigned short interruptNumber) {
        outw(IOCMD_SET_AD_INTERRUPT_NUMBER | (interruptNumber & 0x07),
             BaseAddress + CMD_REGISTER);
    }
  
    inline void SetTimerInterruptNumber(unsigned short interruptNumber)  {
        outw(IOCMD_SET_TIMER_INTERRUPT_NUMBER | (interruptNumber & 0x07),
             BaseAddress + CMD_REGISTER);
    }
    
  inline void SetADInterrupt(bool enable)  {
        outw(IOCMD_SET_AD_INTERRUPT | (enable?0x0001:0x0000),
             BaseAddress + CMD_REGISTER);
    }

  inline void SetTimerInterrupt(bool enable) {
        outw(IOCMD_SET_TIMER_INTERRUPT | (enable?0x0001:0x0000),
             BaseAddress + CMD_REGISTER);
    }
  
    inline void SetWDTPeriod(unsigned short period)  {
        outw(IOCMD_SET_WDT_PERIOD | (period & 0x03FF),
             BaseAddress + CMD_REGISTER);
    }

  inline void SetTimerPeriod(unsigned short period) {
        outw(IOCMD_SET_TIMER_PERIOD | (period & 0x03FF),
             BaseAddress + CMD_REGISTER);
    }
    
  /* Get functions */
  inline unsigned short GetMotorState(void) {
        outw(IOCMD_GET_MOTOR_STATE, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x000F);
    }

  inline unsigned short GetMotorVoltageIndices(void) {
        outw(IOCMD_GET_MOTOR_VOLTAGE_INDICES, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x001F);
    }
  
    inline unsigned short GetCurrentLimitIndices(void) {
        outw(IOCMD_GET_CURRENT_LIMIT_INDICES, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x001F);
    }

  inline unsigned short GetEncoderIndices(void) {
        outw(IOCMD_GET_ENCODER_INDICES, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x001F);
    }

  inline unsigned short GetCurrentFeedbackMaxIndex(void) {
        outw(IOCMD_GET_CURRENT_FEEDBACK_MAX_INDEX, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x000C);
    }

  inline unsigned short GetPotFeedbackMaxIndex(void) {
        outw(IOCMD_GET_POT_FEEDBACK_MAX_INDEX, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x000C);
    }
  
    inline unsigned short GetADInterruptNumber(void) {
        outw(IOCMD_GET_AD_INTERRUPT_NUMBER, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x0007);
    }

  inline unsigned short GetTimerInterruptNumber(void)  {
        outw(IOCMD_GET_TIMER_INTERRUPT_NUMBER, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x0007);
    }

  inline bool GetADInterrupt(void) {
        outw(IOCMD_GET_AD_INTERRUPT, BaseAddress + CMD_REGISTER);
        return (((inw(BaseAddress + DATA_REGISTER) & 0x0001)?true:false));
    }

  inline bool GetTimerInterrupt(void) {
        outw(IOCMD_GET_TIMER_INTERRUPT, BaseAddress + CMD_REGISTER);
        return (((inw(BaseAddress + DATA_REGISTER) & 0x0001))?true:false);
    }
  
    inline unsigned short GetWDTPeriod(void) {
        outw(IOCMD_GET_WDT_PERIOD, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x03FF);
    }

  inline unsigned short GetTimerPeriod(void) {
        outw(IOCMD_GET_TIMER_PERIOD, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x03FF);
    }
  
    inline unsigned short GetCurrentTime(void) {
        outw(IOCMD_GET_CURRENT_TIME, BaseAddress + CMD_REGISTER);
        return (inw(BaseAddress + DATA_REGISTER) & 0x03FF);
    }
    
  /* Pulse Functions */

  /* DAC Functions */
  inline void SetMotorVoltage(short data) {
    /*
      Make note of the un-expected behaviour here
      the positive range is upto 2^n and NOT 2^n-1
      whereas the negative range is upto 2^n-1.
          4096 == +5V 
      -4095 == -5V
     */
    //shift for DAC
    data += 0x1000;
    data = (data > 0x1FFF)?0x1FFF:data;
    data = (data < 0)?0:data;
    outw(data, BaseAddress + DA_MOTOR_VOLTAGE_REGISTER);
    }

  inline void SetCurrentLimit(short data) {
    /*
          The second dac is set to be unipolar
      0 == 0V
      8192 = +5V
        */
    data = (data > 0x1FFF)?0x1FFF:data;
    data = (data < 0)?0:data;
    outw(data, BaseAddress + DA_CURRENT_LIMIT_REGISTER);
    }

  inline void LoadMotorVoltages(void) {
        outw(IOCMD_LOAD_MOTOR_VOLTAGES,
             BaseAddress + CMD_REGISTER);
    }

  inline void LoadCurrentLimits(void) {
        outw(IOCMD_LOAD_CURRENT_LIMITS,
             BaseAddress + CMD_REGISTER);
    }
  
    inline void LoadMotorVoltagesCurrentLimits(void) {
        outw(IOCMD_LOAD_MOTOR_VOLTAGES_CURRENT_LIM,
             BaseAddress + CMD_REGISTER);
    }

  /* ADC Functions */
  inline short GetADFIFO(void) {
    /*
      5V == 8191 
          -5V == -8192
     */
    short data = inw(BaseAddress + AD_FIFO_REGISTER) & 0x3FFF;
    /* extend the sign bit to 16bits of data.
      upper 3 bits are ones for -ve numbers
          upper 3 bits are zeros for +ve numbers
        */
    data = (data & 0x2000)?(data | 0xE000):(data & 0x1FFF);
    return data;
  }

  /* 
       A read of the FIFO puts the ADC in the reset (aka Interrupt
       Deactivated) state. This is somewhat un-expected behaviour, I
       was expecting that a #CONVST pulse would also De-activate the
       #INT pin. Keeping this in mind it is a good idea to read the
       FIFO before closing application, irrespective of the interrupt
       state to enusre ADC is in valid state. Also it might be a good
       idea to do a read just after progamming initializtion to make
       sure ADC is in valid state again irrespective of the interrupt
       state.
   
       The cleanest solution is to fix the MAXIM bug in hardware, I
       leave this for the future. A quicker more cleaner solution is
       to have the #INT as part of the status register, check this bit
       before each conversion, and reset the ADC if the interrupt is
       activated. For now, this has been implemented.
   
       Ankur
    */
    inline unsigned int StartConvCurrentFeedback(void) {
    bool devReady = (inw(BaseAddress + STATUS_REGISTER) & 0x0008)?true:false;
    if (devReady == true) {
      inw(BaseAddress + AD_FIFO_REGISTER);
      numTries++;
    }
    bool devInt = (inw(BaseAddress + STATUS_REGISTER) & 0x0002)?true:false;
    if (devInt == true) {
      outw(IOCMD_CLEAR_AD_INTERRUPT, BaseAddress + CMD_REGISTER);
    }
    outw(IOCMD_START_CONV_CURRENT_FEEDBACK, BaseAddress + CMD_REGISTER);
    return numTries;
  }

  inline unsigned int StartConvPotFeedback(void) {
    bool devReady = (inw(BaseAddress + STATUS_REGISTER) & 0x0008)?true:false;
    if (devReady == true) {
      inw(BaseAddress + AD_FIFO_REGISTER);
      numTries++;
    }
    bool devInt = (inw(BaseAddress + STATUS_REGISTER) & 0x0002)?true:false;
    if (devInt == true) {
      outw(IOCMD_CLEAR_AD_INTERRUPT, BaseAddress + CMD_REGISTER);
    }
    outw(IOCMD_START_CONV_POT_FEEDBACK, BaseAddress + CMD_REGISTER);
    return numTries;
  }

  inline bool ADInterruptPending(void) {
        return ((inw(BaseAddress + STATUS_REGISTER) & 0x0002)?true:false);
    }

  inline bool ADInterruptFlag(void) {
        return  ((inw(BaseAddress + STATUS_REGISTER) & 0x0008)?true:false);
    }

  inline void ClearADInterrupt(void) {
        outw(IOCMD_CLEAR_AD_INTERRUPT, BaseAddress + CMD_REGISTER);
    }
    
  inline bool PollADInterruptPending(unsigned int timeout) {
    bool devInt = false;
    unsigned int timeCount = 0;
        while (1) {
            devInt = (inw(BaseAddress + STATUS_REGISTER) & 0x0002)?true:false;
            if (devInt) return true;
            if (timeCount > timeout) return false;
            usleep(1);
            timeCount++;
        }
  }
    
  /* Encoder Functions */
  inline void LatchEncoders(void) {
        outw(IOCMD_LATCH_ENCODERS, BaseAddress + CMD_REGISTER);
    }

  inline void PreLoadEncoders(unsigned short listEncoder[]) {
    unsigned short mask = ((listEncoder[3] & 0x1) << 3) | ((listEncoder[2] & 0x1) << 2) |
      ((listEncoder[1] & 0x1) << 1) | (listEncoder[0] & 0x1);
    outw(IOCMD_PRELOAD_ENCODERS | mask, BaseAddress + CMD_REGISTER);
  }

  inline void SetEncoderPreloadRegister(unsigned int data) {
    outl(data, BaseAddress + ENCODER_LOW_REGISTER);
  }

  inline unsigned int GetEncoder(void) {
    return (inl(BaseAddress + ENCODER_LOW_REGISTER) & 0x00FFFFFF);
  }
    
  inline short GetEncoderPeriod(void) {
    unsigned short ret;
    ret = inw(BaseAddress + ENCODER_PERIOD) & 0x7FFF;
    // sign extend to 2 bytes
    ret = (ret&(0x01 <<13))?ret|0xC000:ret;
    return ret;
  }

  inline short GetEncoderFrequency(void) {
    unsigned short ret;
    ret = inw(BaseAddress + ENCODER_FREQUENCY) & 0xFFFF;
    // sign extend to 2 bytes
    ret = (ret&(0x01 <<14))?ret|0xC000:ret;
    return ret;
  }
    
    /* Timer Functions */
  inline bool TimerInterruptPending(void)  {
        return ((inw(BaseAddress + STATUS_REGISTER) & 0x0001)?true:false);
    }

  inline void ClearTimerInterrupt(void) {
        outw(IOCMD_CLEAR_TIMER_INTERRUPT, BaseAddress + CMD_REGISTER);
    }
    
  /* WDT Functions */
  inline bool WDTStatus(void) {
        return ((inw(BaseAddress + STATUS_REGISTER) & 0x0004)?true:false);
    }

  /* Misc. Functions */
  inline unsigned short BoardVersion(void) {
        return (inw(BaseAddress));
    }

  inline void SetBaseAddress(unsigned short address) {
        BaseAddress = address;
    }

  inline unsigned short GetBaseAddress(void) {
        return BaseAddress;
    }

  inline void EnableMotor(unsigned short motor) {
        SetMotorState(0x01<<motor, 0x01<<motor);
    }

  inline void DisableMotor(unsigned short motor) {
        SetMotorState(0x01<<motor, ~(0x01<<motor));
    }

  inline void EnableAllMotors(void) {
        SetMotorState(0x0F, 0x0F);
    }
    
  inline void DisableAllMotors(void) {
        SetMotorState(0x0F, 0x00);
    }
  
    /* ***provided for testing only*** */
  inline void Write(unsigned short data, unsigned short offset) {
        outw(data, BaseAddress + offset);
    }
  /* ***provided for testing only*** */
  inline unsigned short Read(unsigned short offset) {
        return (inw(BaseAddress + offset));
    }
  /* ***provided for testing only*** */
  inline void SetSimEncoder(bool enable, unsigned short dir, unsigned short divisor) {
        outw(((enable)?0x01:0x00)<<5|(dir & 0x01)<<4|divisor & 0x0F,
             BaseAddress + UNUSED_0);
    }
};

#endif // CISST_HAS_LOPOMOCO

#endif // _ddiLoPoMoCoLowLevelIO_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiLoPoMoCoLowLevelIO.h,v $
//  Revision 1.5  2006/02/24 19:32:09  pkaz
//  ddiLoPoMoCoLowLevelIO.h:  fixed typos.
//
//  Revision 1.4  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2004/09/03 19:56:36  anton
//  Doxygen documentation.
//
//  Revision 1.2  2004/05/27 19:34:22  anton
//  Added || defined DOXYGEN to all #ifdef CISST_HAS_XYZ so that doxygen can
//  create the documentation.
//
//  Revision 1.1  2004/05/27 17:09:25  anton
//  Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
//  The build process will have to be redone whenever will will add dynamic
//  loading.
//
//
// ****************************************************************************

