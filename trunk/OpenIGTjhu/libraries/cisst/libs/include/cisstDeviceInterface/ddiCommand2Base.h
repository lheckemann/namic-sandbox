/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCommand2Base.h,v 1.8 2006/07/07 02:46:50 pkaz Exp $
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

#ifndef _ddiCommand2Base_h
#define _ddiCommand2Base_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiCommandBase.h>
#include <iostream>

/*!
  \ingroup cisstDeviceInterface

  A base class command object with an execute method that takes two
  arguments.  To be used to contain Convert*Methods. */
class ddiCommand2Base : public ddiCommandBase {
    public:
    typedef ddiCommandBase BaseType;
    const std::type_info *TypeInfoFrom, *TypeInfoTo;
    /*! The constructor. Does nothing */
    ddiCommand2Base():BaseType(), TypeInfoFrom(0), TypeInfoTo(0) {}
    ddiCommand2Base(std::string name, BaseType::CommandType type, const std::type_info *infoFrom, const std::type_info *infoTo):
        BaseType(name, type), TypeInfoFrom(infoFrom), TypeInfoTo(infoTo) {}

    /*! The destructor. Does nothing */
    virtual ~ddiCommand2Base() {}

    /*! The execute method. Abstract method to be implemented by derived
      classes to run the actual operation on the receiver
      \param from The data passed to the conversion operation
      \param to The result obtained from the conversion operation
      \result Boolean value, true if success, false otherwise */
    virtual BaseType::ReturnType Execute(cmnDataObject &from, cmnDataObject &to) = 0;

    /*! For debugging. Generate a human readable output for the command
      object */
    virtual std::string ToString() const = 0;
    virtual void ToStream(std::ostream &out) const = 0;

    /*! The overloaded right shift operator to redirect output to a
      ostream */
    friend std::ostream& operator << (std::ostream& out, ddiCommand2Base& data);
};


#endif // _ddiCommand2Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCommand2Base.h,v $
// Revision 1.8  2006/07/07 02:46:50  pkaz
// cisstDeviceInterface: fixed minor typos in Doxygen comments.
//
// Revision 1.7  2006/05/10 00:41:39  kapoor
// cisstDeviceInterface: Added support for ddiVecShort and ddiVecLong.
//
// Revision 1.6  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.5  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.4  2006/05/03 01:49:59  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.3  2006/05/02 20:23:21  kapoor
// Two argument version of the ddiCommand (device & composites)
//
// Revision 1.2  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.2  2004/03/26 06:05:34  kapoor
// Removed keyword const from Execute (const ddiCommand* ==> ddiCommand*)
//
// Revision 1.1  2004/03/24 20:52:19  kapoor
// Moved the command* classes to own files.
//
// ****************************************************************************
