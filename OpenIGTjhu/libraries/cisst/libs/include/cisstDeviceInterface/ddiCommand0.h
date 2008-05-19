/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCommand0.h,v 1.4 2006/07/07 02:46:50 pkaz Exp $
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
  \brief Defines a command with two arguments 
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiCommand0_h
#define _ddiCommand0_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiCommand0Base.h>
#include <iostream>

// forward declaration
class ddiDeviceInterface;

/*!
  \ingroup cisstDeviceInterface
  
  A templated version of command object with two arguments for
  execute. The template argument is the device type whose method is
  contained in the command object. */
template<class _deviceType>
class ddiCommand0: public ddiCommand0Base {

    public:
    typedef ddiCommand0Base BaseType;

    /*! Typedef for the specific device. */
    typedef _deviceType DeviceType;

    /*! Typedef for pointer to member function of the specific device 
      class. */
    typedef ddiCommandBase::ReturnType (_deviceType::*ActionType)();
    protected:

    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command*/
    ActionType Action;

    /*! Stores the receiver object of the command */
    DeviceType *Device;

    public:
    /*! The constructor. Does nothing */
    ddiCommand0():BaseType() {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param device Pointer to the receiver of the command
      \param name A string to identify the command. Useful for debugging. */
    ddiCommand0(ActionType action, DeviceType *device, std::string name):
        BaseType(name, ddiCommandBase::DDI_COMMAND_0),
        Action(action), Device(device) {}

    /*! The destructor. Does nothing */
    virtual ~ddiCommand0() {}

    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver. 
      \param from The data passed to the conversion operation
      \param to The result obtained from the conversion operation
      \result Boolean value, true if success, false otherwise */
    virtual ddiCommandBase::ReturnType Execute() {
        ddiCommandBase::ReturnType result;
        result = (Device->*Action)();
        return result;
    }

    /*! For debugging. Generate a human readable output for the command
      object */
    virtual std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    virtual void ToStream(std::ostream &out) const {
        out << " Command Type " << Type << ": ";
        if (Device) out << Device->GetName();
        else out << "Nodevice";
        out << "::" << Name << " ";
    }

};


template<class _deviceType>
std::ostream & operator << (std::ostream & output,
        const ddiCommand0<_deviceType> & command) {
    command.ToStream(output);
    return output;
}

#endif // _ddiCommand0_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCommand0.h,v $
// Revision 1.4  2006/07/07 02:46:50  pkaz
// cisstDeviceInterface: fixed minor typos in Doxygen comments.
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
//
// ****************************************************************************
