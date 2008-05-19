/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiDMM16AT.h,v 1.0 2006/07/24 15:41:46 gfischer Exp $
//
//  Author(s):  Greg Fischer, Ankur Kapoor
//  Created on: 2006-07-24
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2006, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************


/*!
  \file
  \brief Defines the ATI Force/Torque sensor with a NiDAQ card. 
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiDMM16AT_h
#define _ddiDMM16AT_h

#include <cisstConfig.h>
#if CISST_HAS_ATIDAQFT || defined DOXYGEN

#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiATIDAQFTLowLevelIO.h>

/*!
  \ingroup cisstDeviceInterface
  
  A specific device class for ATI DAQ Force/Torque sensor.
*/
class ddiDMM16AT: public ddiDeviceInterface {
    CMN_DECLARE_SERVICES(ddiDMM16AT, CMN_NO_DYNAMIC_CREATION);

/*
  enum ATI_STATUS {
    CAL_FILE_ERROR=0,
    INVALID_UNITS,
    FAILED_TEMP_COMP,
    UNKOWN_ERROR
  }; //read not for DAQ_STATUS
*/
  /*! enum for NI-DAQ return codes */
/*  enum DAQ_STATUS {
    DAQ_ERROR=0,
    DAQ_READY
  };
*/
  /*! members related to NI-DAQ */
  /*! This is for now just a place holder for status value */
  short DAQStatus;
    
    short RetVal;
    
  /*! Device number of NI-DAQ. Must be set in CISST config file */
    short Device;
    
  /*! Channel number of NI-DAQ. Must be set in CISST config file */
    int Channel;
    
  /*! Overall gain for NI-DAQ analog inputs */
    short Gain;
    
  /*! Ignore warning from the NI-DAQ card */
    short IgnoreWarning;
    
  /*! Number of channel */
  short  NumberChannels;
    
  /*! members related to ATI F/T */
  /*! If ATI F/T is calibrated, i.e. ATI provided calibration file
      was successflly read and loaded. */
  bool IsCalibrated;
    
  /*! True if ATI F/T was biased using the SetNewBias method */
  bool IsBiased;
    
  unsigned short CalibrationIndex;
    
  /*! This is for now just a place holder for status value for ATI F/T*/
  int ATIStatus;
    
  /*! The number of samples to be taken to calculate bias. This should
      be provided by a CISST config file. */
  int NumSamples;
    
  /*! Sequence number for reading. To make it compatible with older ATI ISA
      cards. */
  unsigned long Sequence;
    
  /*! Channel scan sequence */
  short ChannelVector[7];
    
  /*! Gain setting to use for each channel in chanVector */
  short GainVector[7];
    
  /*! Object for Lowlevel  stuff */
  ddiATIDAQFTLowLevelIO LowLevelObject;
    
  //f64 RawVoltage[7];
    
  /*! members related to ATI F/T */
  /*! Stores the bias voltages from the NI-DAQ. */
  double BiasVoltage[7];
    
  /*! Stores the ATI provided calibration structure. */
  struct Calibration *CalibrationStruct;

  // Var declarations
  BYTE result; // returned error code
  DSCB dscb;   // handle used to refer to the board
  DSCCB dsccb; // structure containing board settings
  DSCADSETTINGS dscadsettings; // structure containing A/D conversion settings
  //ERRPARAMS errorParams;       // structure for returning error code and error string

    
 public:
  /*! Temp will be deleted. */
  typedef bool (ddiATIDAQFT::*_OperationPointerType)(cmnDataObject &);
    
  /*! The enumeration for the Read Methods. */
  enum ReadMethods {
    FORCETORQUES = 1, /*! Read the latest force/torque values. */
  };
    
  /*! The enumeration for the Write Methods. */
  enum WriteMethods {
    BIAS = 1     /*! Recompute the force/torque bias. */
  };
    
  /*! The enumeration for the Conversion Methods. */
  enum ConversionMethods {
    FTCOUNTSTOFTNEWTONS = 1 /*! Convert Force in Counts to Newton and 
                                  Torque to Newton-Meter. */
  };
    
  /*! Default constructor. */
  ddiDMM16AT();
    
  /*! Default destructor. Does nothing. */
  virtual ~ddiDMM16AT();
    
  /*! Configures the board and perpares it to reading. */
  //virtual void Configure(const char *filename=NULL);
  virtual void Configure(const char *filename=NULL);
    
  /*! Returns a vector of string. For now it returns ForceTorques
    and Info. */
  //virtual std::vector<std::string> Provides(void);
    
 protected:
    
  /*! Method to get Forces and Torques.  Is indexed by string
    ForceTorques in the ReadMap. */
  //bool GetForcesTorques(cmnDataObject &obj);
   //bool GetSingleVoltage(cmnDataObject &obj);
   bool GetSingleVoltage(BYTE channel, DFLOAT &voltage);

   //bool GetVoltages(cmnDataObject &obj);
    
  /*! Method to set new bias.  Is indexed by string Bias in the
    WriteMap. */
  //bool SetNewBias(cmnDataObject &obj);
    
  /*! Method to convert force and torques to SI units from counts.
    Is index by string FTCountsToFTNewton in the ConversionMap. */
  //bool ConvertFTCountsToFTNewtons(cmnDataObject &from, cmnDataObject &to);

  //bool ConvertVoltageToPressure(cmnDataObject &from, cmnDataObject &to);
    
  /*! Method to Set Temperature compensation.
      Used by Configure. Probably will be set using cisst config file (XML??)
      \param yes If yes == true sets temperature compensation for F/T readings
  */
  //bool SetTemperatureCompensation(bool yes);
    
  /*! Method to Set Torque units
      Used by Configure. Probably will be set using cisst config file (XML??)
      \param units string specifying units for Troque
  */
  //bool SetTorqueUnits(const std::string &units);
    
  /*! Method to Set Force units
      Used by Configure. Probably will be set using cisst config file (XML??)
      \param units string specifying units for Troque
  */
  //bool SetForceUnits(const std::string &units);
    
  /*! Method to load ATI provided calibration file
      Used by Configure. Probably will be set using cisst config file (XML??)
      \param calFilePath string specifying ATI provided calibration file
  */
  //bool LoadCalibration(const char *calFilePath);
    
};

#endif // CISST_HAS_DMM16AT

#endif // _ddiDMM16AT_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiATIDAQFT.h,v $
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/06/19 21:41:17  kapoor
// ddiDeviceInterface: a working version of ati ft daq (windows) interface.
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
// Revision 1.1  2004/04/08 06:05:05  kapoor
// Added ATI F/T DAQ DI
//
// Revision 1.3  2004/03/26 06:06:08  kapoor
// Added pack and unpack.
//
// Revision 1.2  2004/03/24 20:48:31  kapoor
// Added lots of documentation.
//
// Revision 1.1  2004/03/22 00:31:20  kapoor
// Added new devices.
//
// ****************************************************************************
