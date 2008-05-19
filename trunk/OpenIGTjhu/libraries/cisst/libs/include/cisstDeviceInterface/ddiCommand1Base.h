/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCommand1Base.h,v 1.7 2006/07/07 02:46:50 pkaz Exp $
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
  \brief Defines a base class for a command with one argument
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiCommand1Base_h
#define _ddiCommand1Base_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstCommon/cmnClassServicesBase.h>
#include <cisstDeviceInterface/ddiCommandBase.h>
#include <iostream>

// Always include last
#include <cisstDeviceInterface/ddiExport.h>

/*!
  \ingroup cisstDeviceInterface

  A base class of command object with an execute method that takes
  one argument. */
class CISST_EXPORT ddiCommand1Base : public ddiCommandBase {
    public:
    typedef ddiCommandBase BaseType;
    const std::type_info *TypeInfo;
    const cmnClassServicesBase *ClassInputTypeServices;
    /*! The constructor. Does nothing */
    ddiCommand1Base():BaseType(), TypeInfo(0), ClassInputTypeServices(0) {}
    ddiCommand1Base(std::string name, BaseType::CommandType type,
            const std::type_info *info, const cmnClassServicesBase *classInputTypeServices):
        BaseType(name, type), TypeInfo(info), ClassInputTypeServices(classInputTypeServices) {}

    /*! The destructor. Does nothing */
    virtual ~ddiCommand1Base() {}

    /*! The execute method. Abstract method to be implemented by
      derived classes to run the actual operation on the receiver

      \param obj The data passed to the operation method

      \result Boolean value, true if success, false otherwise */
    virtual BaseType::ReturnType Execute(cmnDataObject &obj) = 0;

    /*! For debugging. Generate a human readable output for the
      command object */
    virtual std::string ToString() const = 0;
    virtual void ToStream(std::ostream &out) const = 0;

    /*! The overloaded right shift operator to redirect output to a
      ostream */
    friend std::ostream& operator << (std::ostream& out, ddiCommand1Base& data);
};


#endif // _ddiCommand1Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCommand1Base.h,v $
// Revision 1.7  2006/07/07 02:46:50  pkaz
// cisstDeviceInterface: fixed minor typos in Doxygen comments.
//
// Revision 1.6  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.5  2006/05/10 00:41:39  kapoor
// cisstDeviceInterface: Added support for ddiVecShort and ddiVecLong.
//
// Revision 1.4  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.3  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.2  2006/05/03 01:49:59  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.1  2006/05/02 20:28:23  kapoor
// Import of zero and one argument version of ddiCommand.
//
//
// ****************************************************************************
