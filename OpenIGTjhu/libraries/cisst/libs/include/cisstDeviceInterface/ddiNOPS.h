/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiNOPS.h,v 1.8 2006/07/13 22:08:02 pkaz Exp $
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
  \brief Defines a NO OperationS 
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiNOPS_h
#define _ddiNOPS_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstDeviceInterface/ddiCommandBase.h>

#include <vector>
#include <string>

#include <cisstDeviceInterface/ddiExport.h>

/*!
  \ingroup cisstDeviceInterface
  
  We need something that is returned when a lookup on a map fails.
  This device class NOPS (NO OPerationS - thats a standard term at
  least I believe it is standard)
*/
class CISST_EXPORT ddiNOPS: public ddiDeviceInterface 
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 1);

public:
  /*! The constructor. Does nothing. */
  ddiNOPS(): ddiDeviceInterface("NOPS") {}
    
  /*! The destructor. Does nothing. */
  virtual ~ddiNOPS() {}
    
  /*! The default noperation object method with one argument. */
  virtual ddiCommandBase::ReturnType NOPS0() {
        return ddiCommandBase::NO_DEVICE;
    }
    
  /*! The default noperation object method with one argument. */
  virtual ddiCommandBase::ReturnType NOPS1(cmnDataObject &obj) {
        return ddiCommandBase::NO_DEVICE;
    }

  /*! The default noperation object method with two arguments.*/
  virtual ddiCommandBase::ReturnType NOPS2(cmnDataObject &from, cmnDataObject &to) {
        return ddiCommandBase::NO_DEVICE;
    }

  /*! The configure method. Does nothing. */
  virtual void Configure(const char *filename = NULL) {}

};


CMN_DECLARE_SERVICES_INSTANTIATION(ddiNOPS);


#endif // _ddiNOPS_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiNOPS.h,v $
// Revision 1.8  2006/07/13 22:08:02  pkaz
// ddiDeviceInterface: added CISST_EXPORT for Windows.
//
// Revision 1.7  2006/05/11 03:33:47  anton
// cisstCommon: New implementation of class services and registration to
// support templated classes.  This code has not been extensively tested and
// needs further work.
//
// Revision 1.6  2006/05/05 05:09:38  kapoor
// cisstDeviceInterface: See checkin [1945] <--cool number.
//
// Revision 1.5  2006/05/03 01:49:59  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/03/03 18:36:54  anton
// All libraries: Updated calls to CMN_DECLARE_SERVICES to use CMN_{NO,}_DYNAMIC_CREATION
// (see ticket #132 for details).
//
// Revision 1.2  2005/02/28 22:43:34  anton
// cisstDevices and real-time:  Added Log (see #130).
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.1  2004/03/24 20:52:19  kapoor
// Moved the command* classes to own files.
//
// ****************************************************************************
