/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiMEI.h,v 1.13 2006/05/11 18:45:36 anton Exp $
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
  \brief Defines the MEI device (Motion Engineering Inc) 
  \ingroup cisst
*/


#ifndef _ddiMEI_h
#define _ddiMEI_h

#include <cisstConfig.h>
#if CISST_HAS_MEI || defined DOXYGEN

#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstDeviceInterface/ddiTypes.h>
#include <cisstCommon/cmnDataObject.h>

#define DDI_MAX_MEI_AXIS 16

/*!
  \ingroup cisstDeviceInterface
*/
class ddiMEI: public ddiDeviceInterface {
  CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
  /*! Starting number of axis on the board. */
  int StartAxis;

  /*! Ending number of axis on the board. */
  int EndAxis;

  /*! Max number of axis on the board. */
  unsigned int MaxAxis;

  /*! Base address of the board. */
  int BaseAddress;

  ddiDynamicVectorDataObject<int, DDI_MAX_MEI_AXIS> AxisNumber;

  ddiDynamicVectorDataObject<ddiGainData, DDI_MAX_MEI_AXIS> Gains;

  ddiDynamicVectorDataObject<double, DDI_MAX_MEI_AXIS> DegsPerCount;

  ddiDynamicVectorDataObject<double, DDI_MAX_MEI_AXIS> OffsetInSI;

  ddiDynamicVectorDataObject<double, DDI_MAX_MEI_AXIS> MaxVelocities;

  ddiDynamicVectorDataObject<double, DDI_MAX_MEI_AXIS> MaxAcceleration;

  ddiDynamicVectorDataObject<double, DDI_MAX_MEI_AXIS> TargetPositions;
 protected:

  DDI_DECLARE_COMMAND1_OUT(IsMoving, ddiVecBool16 );
  DDI_DECLARE_COMMAND1_OUT(GetAccelerations, ddiVecDouble16);
  DDI_DECLARE_COMMAND1_OUT(GetVelocities, ddiVecDouble16);
  DDI_DECLARE_COMMAND1_OUT(GetError, ddiVecDouble16);
  DDI_DECLARE_COMMAND1_OUT(GetCommand, ddiVecDouble16);
  DDI_DECLARE_COMMAND1_OUT(GetPositions, ddiVecDouble16);
  DDI_DECLARE_COMMAND1_IN(Stop, ddiBasicTypeDataObject<int>);
  DDI_DECLARE_COMMAND1_IN(Abort, ddiBasicTypeDataObject<int>);
  DDI_DECLARE_COMMAND1_IN(StopAll, ddiVecBool16);
  DDI_DECLARE_COMMAND1_IN(AbortAll, ddiVecBool16);
  DDI_DECLARE_COMMAND1_IN(EnableAxes, ddiVecBool16);
  DDI_DECLARE_COMMAND1_IN(DisableAxes, ddiVecBool16);
  DDI_DECLARE_COMMAND1_IN(SetTargetPositions, ddiVecDouble16);
  DDI_DECLARE_COMMAND1_IN(SetMaxVelocities, ddiVecDouble16);
  DDI_DECLARE_COMMAND1_IN(SetMaxAccelerations, ddiVecDouble16);
  DDI_DECLARE_COMMAND0(LoadTargetPositions);
  DDI_DECLARE_COMMAND0(LoadTargetVelocities);
  DDI_DECLARE_COMMAND1_OUT(GetGains, ddiGainData);
  DDI_DECLARE_COMMAND1_IN(SetGains, ddiGainData);

  DDI_DECLARE_COMMAND2(SIToEncoder, ddiVecDouble16, ddiVecDouble16);
  DDI_DECLARE_COMMAND2(EncoderToSI, ddiVecDouble16, ddiVecDouble16);

public:

  /*!  Default constructor. */
  ddiMEI();

  /*!  Default destructor. */
  virtual~ddiMEI();

  /*!  Virtual method that gets called when the task/device needs
      to be configured. Should it take XML info? */
  virtual void Configure(const char *filename = NULL);
};


CMN_DECLARE_SERVICES_INSTANTIATION(ddiMEI);


#endif // CISST_HAS_MEI

#endif // _ddiMEI_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiMEI.h,v $
//  Revision 1.13  2006/05/11 18:45:36  anton
//  ddiMEI and ddiLoPoMoCo: Updated for new class registration.
//
//  Revision 1.12  2006/05/09 02:26:58  kapoor
//  cisstDeviceInterface and IRE: wrapping of the ddiCommand[0-2]Base class.
//
//  Revision 1.11  2006/05/07 05:19:55  kapoor
//  ddiDeviceInterface and IRE: Wrapping of ddiCommand[0-2] objects. Added
//  a new macro, which returns the exact type of command instead of base type.
//  This needs to be changed, not the most optimal way.
//
//  Revision 1.10  2006/05/07 04:45:26  kapoor
//  cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
//  Revision 1.9  2006/05/05 05:09:38  kapoor
//  cisstDeviceInterface: See checkin [1945] <--cool number.
//
//  Revision 1.8  2006/05/05 03:35:08  kapoor
//  cisstDeviceInterface: COMMAND_1 is replaced by 1_IN and 1_OUT to specify
//  read and write operations. This macro only changes the "C" like API that
//  is provided for the wrapping of device class, such that the output is
//  the can be assigned directly.
//
//  Revision 1.7  2006/05/03 05:35:03  kapoor
//  Version using MACROS for python API and protected methods.
//
//  Revision 1.6  2006/03/04 19:07:32  kapoor
//  ddDeviceInterface: Split the Encoder to SI unit conversion ratio into three seperate values that are read from the xml-config files. This is the current standard for other servo loops as well, as it makes it easy to change these according to different robots.
//
//  Revision 1.5  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.4  2005/09/07 08:22:09  kapoor
//  ddiDeviceInterface: Minor fix in ATI DAQ interface. Added SI to Enc and Enc To SI conversion for MEI.
//
//  Revision 1.3  2005/06/19 21:40:57  kapoor
//  ddiDeviceInterface: a working version of mei interface.
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

