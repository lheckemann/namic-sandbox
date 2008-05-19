/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiLoPoMoCo.h,v 1.18 2006/05/14 03:21:14 kapoor Exp $
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
  \brief Defines the JHU LoPoMoCo device 
  \ingroup cisstDeviceInterface
*/


#ifndef _ddiDMM16AT_h
#define _ddiDMM16AT_h

#include <cisstConfig.h>
//#if CISST_HAS_LOPOMOCO || defined DOXYGEN

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiDeviceInterface.h>

#include <vector>
#include <string>
#include <ostream>

//GSF
#include <cisstDeviceInterface/dscud.h>

//#include <cisstDeviceInterface/ddiLoPoMoCoLowLevelIO.h>
//#include <cisstDeviceInterface/BoardIO.h>
//#include <cisstDeviceInterface/Offsets.h>

// number of axis per card
//#define DDI_LOPOMOCO_NB_AXIS 4

// No more that four cards can be put together, creating a 16 axis
// composite device.  If one is creating a ddiSixLoPoMoCo,one will
// have to update this number
//#define DDI_LOPOMOCO_MAX_NB_AXIS (4 * 4)

/*!
  \ingroup cisstDeviceInterface
  A specific device class for LoPoMoCo card.
 */
class ddiDMM16AT: public ddiDeviceInterface {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

  /*! A pointer to the low level object that converts make bit level
    operation more readable. */
  //ddiLoPoMoCoLowLevelIO *Board;
  //BoardIO *Board;

  /*! Starting number of ADC channel used on the board. */
  int LowChannel;

  /*! Ending number of ADC channel used on the board. */
  int HighChannel;

  /*! Max number of axis on the board. */
  //unsigned int MaxAxis;

  /*! Base address of the board. */
  int BaseAddress;

  
  //GSF
  DSCSAMPLE* ADC_samples;
  double ADC_voltage;
  BYTE result;    // returned error code
  DSCB dscb;      // handle used to refer to the board
  DSCCB dsccb;    // structure containing board settings
  DSCADSCAN dscadscan;         // structure containing A/D scan settings
  DSCADSETTINGS dscadsettings; // structure containing A/D conversion settings
  

  /*! Need to save motor voltages so that correct correction could
      be applied to the current limit. */
  //ddiDynamicVectorDataObject<short, DDI_LOPOMOCO_NB_AXIS> MotorVoltages;

  /*! Need to read encoder periods along with each encoder read. */
  //ddiDynamicVectorDataObject<short, DDI_LOPOMOCO_NB_AXIS> EncoderPeriods;
    
  /*! Need to read encoder frequencies along with each encoder
      read. */
  //ddiDynamicVectorDataObject<short, DDI_LOPOMOCO_NB_AXIS> EncoderFrequencies;
    
  /*! FrequencyToRPSRatio */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> FrequencyToRPSRatio;
    
  /*! CountsToDeg */
  // anton ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> CountsToDeg;
    
  /*! CountsToMotorCurrents */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> CountsToMotorCurrents;
    
  /*! CountsToPotFeedback */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> CountsToPotFeedback;
    
  /*! MotorSpeedToCounts */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> MotorSpeedToCounts;
    
  /*! PositiveSlope */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> PositiveSlope;
    
  /*! PositiveIntercept */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> PositiveIntercept;
    
  /*! NegativeSlope */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> NegativeSlope;

  /*! NegativeIntercept */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> NegativeIntercept;

  /*! VoltageToCounts */
  //ddiDynamicVectorDataObject<double, DDI_LOPOMOCO_NB_AXIS> VoltageToCounts;

  public:
  /*! Temp will be deleted. */
  typedef ddiCommandBase::ReturnType (ddiDMM16AT::*_OperationPointerType)(cmnDataObject &);

  /*! Constructor. Prepares the maps. */
  ddiDMM16AT(const char* deviceName = "DMM16AT");
    
  /*! Default destructor. Disables motor power. */
  virtual ~ddiDMM16AT();
    
  /*! Prepares the board for reading and writing. Sets up some registers on
    the board. */
  virtual void Configure(const char *filename = NULL);

 protected:
  /*! Latch the encoder positions.  It is indexed by the string
    LatchEncoders in the PreReadMap. */
    //DDI_DECLARE_COMMAND0(LatchEncoders);
    
  /*! Start conversion on ADC connected to current limits.  It is
    indexed by the string StartMotorCurrentConv in the
    PreReadMap. */
    //DDI_DECLARE_COMMAND0(StartMotorCurrentConv);
    
  /*! Start conversion on ADC connected to pots.  It is indexed by
    the string StartPotFeedbackConv in the PreReadMap. */
    //DDI_DECLARE_COMMAND0(StartPotFeedbackConv);
    
  /*! Get the latest encoder positions.  Assumes that encoders are
    latched using the PreRead operation LatchEncoders.  It is index
    by the string Positions in the ReadMap. */
    //DDI_DECLARE_COMMAND1_OUT(GetPositions, ddiVecLong16);
    
  /*! Get the velocity.  Assumes that encoders are latched using the
    PreRead operation LatchEncoders and that Encoders are read using
    GetPositioni.  It is index by the string Velocities in the
    ReadMap. */
    //DDI_DECLARE_COMMAND1_OUT(GetVelocities, ddiVecShort16);
    
  /*! Get the latest motor currents.  Assumes that a conversion has
    been started using the PreRead operation StartMotorCurrentConv.
    It is indexed by the string MotorCurrents in the ReadMap. */
    //DDI_DECLARE_COMMAND1_OUT(GetMotorCurrents, ddiVecShort16);
    
  /*! Get the latest pot position.  Assumes that a conversion has
    been started using the PreRead operation StartPotFeedbackConv.
    It is indexed by the string PotFeedbacks in the ReadMap. */
    //DDI_DECLARE_COMMAND1_OUT(GetPotFeedbacks, ddiVecShort16);
    
  /*! Write motor votlages to the DAC buffer.  Assumes that these
    would be loaded using the PostWrite operation LoadMotorVolatages
    or LoadMotorVoltagesCurrentLimits.  It is indexed by the string
    MotorVoltages in the WriteMap. */
    //DDI_DECLARE_COMMAND1_IN(SetMotorVoltages, ddiVecShort16);
    
  /*! Write current limits to the DAC buffer.  Assumes that these
    would be loaded using the PostWrite operation LoadCurrentLimits
    or LoadMotorVoltagesCurrentLimits.  It is indexed by the string
    CurrentLimits in the WriteMap. */
    //DDI_DECLARE_COMMAND1_IN(SetCurrentLimits, ddiVecShort16);
    
    /*! Write the current positions to the encoder buffer.
      It is indexed by the string SetPositions in the
      WriteMap. */
    //DDI_DECLARE_COMMAND1_IN(SetPositions, ddiVecLong16);

  /*! Load the motor voltage DAC buffer to actual analog output.  It
    is indexed by the string LoadMotorVolatages in the
    PostWriteMap. */
    //DDI_DECLARE_COMMAND0(LoadMotorVoltages);
    
  /*! Load the current limits DAC buffer to actual analog output.
    It is indexed by the string LoadCurrentLimits in the
    PostWriteMap. */
    //DDI_DECLARE_COMMAND0(LoadCurrentLimits);
    
  /*! Load the motor voltages and current limits DAC buffer to
    actual analog output.  It is indexed by the string
    LoadMotorVoltagesCurrentLimits in the PostWriteMap. */
    //DDI_DECLARE_COMMAND0(LoadMotorVoltagesCurrentLimits);

  /*! Convert encoder counts to revolutions.  It is indexed by the
    string EncodersToDegs in the ConversionMap. */
  // anton bool ConvertEncoderCountsToDegs(cmnDataObject &from, cmnDataObject &to);
    
  /*! Convert encodeer frequencies to revolutions per second.  It is
    indexed by the string FrequencyToRPS in the ConversionMap. */
    //DDI_DECLARE_COMMAND2(FrequencyToRPS, ddiVecShort16, ddiVecDouble16);

  /*! Convert motor currents in ADC counts to mA.  It is indexed by
    the string ADCToMotorCurrents in the ConversionMap. */
    //DDI_DECLARE_COMMAND2(ADCToMotorCurrents, ddiVecShort16, ddiVecDouble16);
    
  /*! Convert pot feedback in ADC counts to V.  It is indexed by the
      string ADCToPotFeedbacks in the ConversionMap. */
    //DDI_DECLARE_COMMAND2(ADCToPotFeedbacks, ddiVecShort16, ddiVecDouble16);
  
  /*! Convert motor voltages in V to DAC counts.  It is indexed by
    the string MotorVoltagesToDAC in the ConversionMap. */
    //DDI_DECLARE_COMMAND2(MotorVoltagesToDAC, ddiVecDouble16, ddiVecShort16);
    
  /*! Convert currents limits in mA to DAC counts.  It is indexed by
    the string CurrentLimitsToDAC in the ConversionMap. */
    //DDI_DECLARE_COMMAND2(CurrentLimitsToDAC, ddiVecDouble16, ddiVecShort16);

  //GSF
  /*! Capture all channels on the ADC.
    Return the 16 voltages. */
    DDI_DECLARE_COMMAND1_OUT(GetInputAll, ddiVecDouble16);

  /*! Enable the specified axis
   */
    //DDI_DECLARE_COMMAND1_IN(Enable, ddiBasicTypeDataObject<short>);

  /*! Disable the specified axis
   */
    //DDI_DECLARE_COMMAND1_IN(Disable, ddiBasicTypeDataObject<short>);
  
  /*! Reset Encoder
   */
    //DDI_DECLARE_COMMAND1_IN(ResetEncoders, ddiBasicTypeDataObject<short>);
};


CMN_DECLARE_SERVICES_INSTANTIATION(ddiDMM16AT);


//#endif // CISST_HAS_LOPOMOCO

#endif // _ddiLoPoMoCo_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiLoPoMoCo.h,v $
// Revision 1.18  2006/05/14 03:21:14  kapoor
// cisstDeviceInterface: ddiLoPoMoCo, fixed typo in command name.
//
// Revision 1.17  2006/05/11 18:45:36  anton
// ddiMEI and ddiLoPoMoCo: Updated for new class registration.
//
// Revision 1.16  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.15  2006/05/09 02:26:58  kapoor
// cisstDeviceInterface and IRE: wrapping of the ddiCommand[0-2]Base class.
//
// Revision 1.14  2006/05/07 05:19:55  kapoor
// ddiDeviceInterface and IRE: Wrapping of ddiCommand[0-2] objects. Added
// a new macro, which returns the exact type of command instead of base type.
// This needs to be changed, not the most optimal way.
//
// Revision 1.13  2006/05/05 05:09:38  kapoor
// cisstDeviceInterface: See checkin [1945] <--cool number.
//
// Revision 1.12  2006/05/05 03:35:08  kapoor
// cisstDeviceInterface: COMMAND_1 is replaced by 1_IN and 1_OUT to specify
// read and write operations. This macro only changes the "C" like API that
// is provided for the wrapping of device class, such that the output is
// the can be assigned directly.
//
// Revision 1.11  2006/05/03 05:35:03  kapoor
// Version using MACROS for python API and protected methods.
//
// Revision 1.10  2006/05/03 02:28:34  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.9  2006/05/02 20:35:22  anton
// Data objects for ddi and rts: Moved data definition to the device interface
// level and renamed the two classes of "vectors".  Updated LoPoMoCo to verify
// compilation.
//
// Revision 1.8  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/04/19 23:59:58  kapoor
// ddiLoPoMoCo: Corrected doxygen documentation. SetPosition doesnt require
// to be followed by a Load.
//
// Revision 1.6  2005/03/15 04:38:38  anton
// ddiLoPoMoCo: Added SetPositions (whatever).
//
// Revision 1.5  2005/03/03 18:36:54  anton
// All libraries: Updated calls to CMN_DECLARE_SERVICES to use CMN_{NO,}_DYNAMIC_CREATION
// (see ticket #132 for details).
//
// Revision 1.4  2005/02/28 22:33:09  anton
// ddiLoPoMoCo:  Added LOG (see ticket #130) and use DDI_LOPOMOCO_NB_AXIS to
// set the lenght of data arrays.
//
// Revision 1.3  2005/01/12 05:03:43  kapoor
// Fixed bug in resetting encoder. See Ticket #117
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
// Revision 1.3  2004/04/08 06:00:30  kapoor
// Added conditional compilation
//
// Revision 1.2  2004/03/24 20:48:32  kapoor
// Added lots of documentation.
//
// Revision 1.1  2004/03/22 00:31:20  kapoor
// Added new devices.
//
// ****************************************************************************
