/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiWinSTG.h,v 1.4 2005/09/26 15:41:46 anton Exp $
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
  \brief Defines the WinSTG device 
  \ingroup cisstDeviceInterface
*/


#ifndef _ddiWinSTG_h
#define _ddiWinSTG_h

#include <cisstConfig.h>
#if CISST_HAS_WINSTG || defined DOXYGEN

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiDeviceInterface.h>

#include <cisstDeviceInterface/ddiWinSTGLowLevelIO.h>

/*!
 \ingroup cisstDeviceInterface
*/
class ddiWinSTG: public ddiDeviceInterface {
  /*! Pointer to low level driver and interface */
  ddiWinSTGLowLevelIO *BoardIO;
    
  /*! A copy, if True closedloop is maintained by STG Need to
      maintain a local copy, because IOCTLs could be expensive in
      terms of time. */
  bool ClosedLoop[MAX_AXIS];
    
  /*! A copy, same states as the low level driver. Again maintained
      locally for eficiency. */
  long InterpolateMode[MAX_AXIS];

  /*! Default Velocities for the posmove in counts/sample. Will be
      set during configure form a CISST config file. */
  long DefaultVelocities[MAX_AXIS];

  /*! Default Accelerations for the posmove & velmove in
      counts/sample^2.  Will be set during configure form a CISST
      config file. */
  long DefaultAccelerations[MAX_AXIS];
  
 protected:
  bool IsMoving(cmnDataObject &obj);
  bool GetAccelerations(cmnDataObject &obj);
  bool GetVelocities(cmnDataObject &obj);
  bool GetError(cmnDataObject &obj);
  bool GetCommand(cmnDataObject &obj);
  bool GetPositions(cmnDataObject &obj);
  bool GetGains(cmnDataObject &obj);
  bool SetGains(cmnDataObject &obj);
  bool EnableAxes(cmnDataObject &obj);
  bool VelMove(cmnDataObject &obj);
  bool PosMove(cmnDataObject &obj);
  bool PosDMove(cmnDataObject &obj);
    
 public:
  /*! Enum for the PreRead operation methods. */
  enum PreReadMethods {
    PREREAD_LASTELEMENT
  };

  /*! Enum for the Read operation methods. */
  enum ReadMethods {
    ISMOVING = 1,
    GETACCELERATIONS,
    GETVELOCITIES,
    GETERROR,
    GETCOMMAND,
    GETPOSITIONS,
    GETGAINS,
    READ_LASTELEMENT
  };

  /*! Enum for the PostRead operation methods. */
  enum PostReadMethods {
    POSTREAD_LASTELEMENT
  };
    
  /*! Enum for the PreWrite operation methods. */
  enum PreWriteMethods {
    PREWRITE_LASTELEMENT
  };
    
  /*!  Enum for the Write operation methods. */
  enum WriteMethods {
    SETGAINS = 1,
    ENABLEAXES,
    //VELMOVE,
    POSMOVE,
    POSDMOVE,
    WRITE_LASTELEMENT
  };
    
  /*!  Enum for the PostWrite operation methods. */
  enum PostWriteMethods {
    POSTWRITE_LASTELEMENT
  };

  /*!  Default constructor. */
  ddiWinSTG();

  /*! Default destructor. */
  virtual~ddiWinSTG();

  /*! Virtual method that gets called when the task/device needs to
      be configured. Should it take XML info? */
  virtual void Configure(const char *filename = NULL);
    
  /*! The virtual method so that the device or tasks returns what
      operations they provide. */
  virtual std::vector<std::string> Provides(void);
    
};


#endif // CISST_HAS_WINSTG

#endif // _ddiWinSTG_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiWinSTG.h,v $
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
