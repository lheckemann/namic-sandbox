/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiSTG.h,v 1.3 2005/09/26 15:41:46 anton Exp $
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
  \brief Defines the ServoToGo device
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiSTG_h
#define _ddiSTG_h

#include <cisstConfig.h>
#if CISST_HAS_STG || defined DOXYGEN

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstDeviceInterface/ddiSTGLowLevelIO.h>

#include <vector>
#include <string>
#include <ostream>

/*!
  \ingroup cisstDeviceInterface

  A specific device class for Servo-To-Go card.
*/
class ddiSTG : public ddiDeviceInterface {

  /*! The base address of the device. */
  unsigned int BaseAddress;
  
  /*! The max number of axis on the card. */
  unsigned int MaxAxis;
  
 public:
  /*! Temp will be deleted. */
  typedef bool (ddiSTG::*_OperationPointerType)(cmnDataObject &);
    
  /*! The enumeration for the PreRead Methods. */
  enum PreReadMethods {
    LATCHENCODERS = 1,   /*! Latch the encoders. */
  };

  /*! The enumeration for the Read Methods. */
  enum ReadMethods {
    POSITIONS = 1,     /*! Get encoder positions. */
    INFO       /*! Get info about the board. */
  };

  /*! The enumeration for the Write Methods. */
  enum WriteMethods {
    MOTORVOLTAGES = 1,   /*! Write motor voltages. */
  };

  /*! The enumeration for the PostWrite Methods. */
  enum PostWriteMethods {
        //  LOADMOTORVOLTAGES = 1,
  };

  /*! The enumeration for the Conversion Methods. */
  enum ConversionMethods {
    ENCODERTODEGS = 1,   /*! Convert encoder counts to revolutions. */
    MOTORVOLTAGESTODAC,   /*! Convert motor voltages to DAC counts. */
  };
    
  /*! Constructor. Prepares the maps. */
  ddiSTG();
    
  /*! Default destructor. Disables motor power. */
  virtual ~ddiSTG();

  /*! Probes for the board address, does some register
    setup. Prepares the board for reading and writing. */
  virtual void Configure(const char *filename=NULL);

  /*! Returns a vector of string. For now it returns Position Info
    and MotorVoltage. */
  virtual std::vector<std::string> Provides(void);
    
 protected:
  /*! Latch the encoder positions.  It is indexed by the string
    LatchEncoders in the PreReadMap. */
  bool LatchEncoders(cmnDataObject &obj);
    
  /*! Get the latest encoder positions.  Assumes that encoders are
    latched using the PreRead operation LatchEncoders.  Is indexed
    by the string Positions in ReadMap. */
  bool GetPositions(cmnDataObject &obj); 

  /*! Get info about the board.  Is indexed by the string Info in
      ReadMap. */
  bool GetInfo(cmnDataObject &obj);

  /*! Write motor votlages to the DAC buffer.
    Is indexed by the string MotorVoltages in WriteMap. */
  bool SetMotorVoltages(cmnDataObject &obj);
    
  //bool LoadMotorVolatages(cmnDataObject &obj);
    
  /*! Convert encoder counts to revolutions.  Is index by the string
    EncodersToDegs in ConversionMap. */
  bool ConvertEncoderCountsToDegs(cmnDataObject &from, cmnDataObject &to);
    
  /*! Convert motor voltages in Volts to DAC counts.  Is index by
    the string MotorVoltagesToDAC in ConversionMap. */
  bool ConvertMotorVoltagesToDACCounts(cmnDataObject &from, cmnDataObject &to);

};


#endif // CISST_HAS_STG

#endif // _ddiSTG_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiSTG.h,v $
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
// Revision 1.3  2004/04/08 06:00:08  kapoor
// Added conditional compilation
//
// Revision 1.2  2004/03/24 20:48:32  kapoor
// Added lots of documentation.
//
// Revision 1.1  2004/03/22 00:31:20  kapoor
// Added new devices.
//
// ****************************************************************************
