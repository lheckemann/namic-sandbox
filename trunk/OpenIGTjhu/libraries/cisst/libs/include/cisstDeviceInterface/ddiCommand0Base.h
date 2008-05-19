/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCommand0Base.h,v 1.5 2006/07/07 02:46:50 pkaz Exp $
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
  \brief Defines a base class for a command with 2 arguments
  \ingroup cisstDeviceInterface
 */

#ifndef _ddiCommand0Base_h
#define _ddiCommand0Base_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiCommandBase.h>
#include <iostream>

/*!
  \ingroup cisstDeviceInterface

  A base class command object with an execute method that takes no
  arguments.  To be used to contain 0*Methods. */
class ddiCommand0Base : public ddiCommandBase {
    public:
    typedef ddiCommandBase BaseType;
    /*! The constructor. Does nothing */
    ddiCommand0Base():BaseType() {}
    ddiCommand0Base(std::string name, ddiCommandBase::CommandType type):
        BaseType(name, type) {}

    /*! The destructor. Does nothing */
    virtual ~ddiCommand0Base() {}

    /*! The execute method. Abstract method to be implemented by derived
      classes to run the actual operation on the receiver
      \result Boolean value, true if success, false otherwise */
    virtual ddiCommandBase::ReturnType Execute() = 0;

    /*! For debugging. Generate a human readable output for the command
      object */
    virtual std::string ToString() const = 0;
    virtual void ToStream(std::ostream &out) const = 0;

    /*! The overloaded right shift operator to redirect output to a
      ostream */
    friend std::ostream& operator << (std::ostream& out, ddiCommand0Base& data);
};


#endif // _ddiCommand0Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCommand0Base.h,v $
// Revision 1.5  2006/07/07 02:46:50  pkaz
// cisstDeviceInterface: fixed minor typos in Doxygen comments.
//
// Revision 1.4  2006/05/10 00:41:39  kapoor
// cisstDeviceInterface: Added support for ddiVecShort and ddiVecLong.
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
