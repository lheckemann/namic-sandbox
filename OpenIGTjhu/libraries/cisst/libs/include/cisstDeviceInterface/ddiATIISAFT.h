/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiATIISAFT.h,v 1.4 2005/09/26 15:41:46 anton Exp $
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
  \brief Defines the ISA ATI Force/Torque sensor device. 
  \ingroup cisst
*/


#ifndef _ddiATIISAFT_h
#define _ddiATIISAFT_h

#include <cisstConfig.h>
#if CISST_HAS_ATIISAFT || defined DOXYGEN

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstDeviceInterface/ddiATIISAFTLowLevelIO.h>

#include <vector>
#include <string>
#include <ostream>


/*!
  \ingroup cisstDeviceInterface
  
  A specific device class for ATI ISA Force/Torque sensor.
*/
class ddiATIISAFT: public ddiDeviceInterface {
    CMN_DECLARE_SERVICES(ddiATIISAFT, CMN_NO_DYNAMIC_CREATION);
  /*! The private base address of the board. Defaults to 0x280. */
  unsigned int BaseAddress;
  //struct SENSOR_DATA SensorData;
  //struct CARD_DATA CardData;
  //struct OUTPUT_INFO OutputInfo;
  //need to save this so that correct conversion
  //could be applied to the current limit
 public:
  /*! Temp will be deleted. */
  typedef bool (ddiATIISAFT::*_OperationPointerType)(cmnDataObject &);
    
  /*! The enumeration for the Read Methods. */
  enum ReadMethods {
    FORCETORQUES = 1, /*! Read the latest force/torque values. */
    INFO       /*! Read the board info. */
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
    
  /*! Default constructor. The base address is set to 0x280 by default. */
  ddiATIISAFT(unsigned int base = 0x280);
    
  /*! Default destructor. Does nothing. */
  virtual ~ddiATIISAFT();
    
  /*! Configures the board and perpares it to reading. */
  virtual void Configure(const char *filename=NULL);
    
  /*! Returns a vector of string. For now it returns ForceTorques
    and Info. */
  virtual std::vector<std::string> Provides(void);

protected:

  /*! Method to get Forces and Torques.  Is indexed by string
    ForceTorques in the ReadMap. */
  bool GetForcesTorques(cmnDataObject &obj);
    
  /*! Method to get board info such as card and sensor serial
    number.  Is indexed by string Info in the ReadMap. */
  bool GetInfo(cmnDataObject &obj);
    
  /*! Method to set new bias.  Is indexed by string Bias in the
    WriteMap. */
  bool SetNewBias(cmnDataObject &obj);
    
  /*! Method to convert force and torques to SI units from counts.
    Is index by string FTCountsToFTNewton in the ConversionMap. */
  bool ConvertFTCountsToFTNewtons(cmnDataObject &from, cmnDataObject &to);
};


#endif // CISST_HAS_ATIISAFT

#endif // _ddiATIISAFT_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiATIISAFT.h,v $
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/07/09 04:47:46  kapoor
// Added CMN_DECLARE_SERVICES
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
// Revision 1.4  2004/04/08 06:04:25  kapoor
// Added conditional compilation
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
