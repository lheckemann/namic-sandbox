/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCommand2.h,v 1.8 2006/07/07 02:46:50 pkaz Exp $
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

#ifndef _ddiCommand2_h
#define _ddiCommand2_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiCommand2Base.h>
#include <iostream>

// forward declaration
class ddiDeviceInterface;

/*!
  \ingroup cisstDeviceInterface

  A templated version of command object with two arguments for
  execute. The template argument is the device type whose method is
  contained in the command object. */
template<class _deviceType>
class ddiCommand2: public ddiCommand2Base {
    public:
    typedef ddiCommand2Base BaseType;

    /*! Typedef for the specific device. */
    typedef _deviceType DeviceType;

    /*! Typedef for pointer to member function of the specific device 
      class. */
    typedef ddiCommandBase::ReturnType (_deviceType::*ActionType)(cmnDataObject &, cmnDataObject &);

    protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command*/
    ActionType Action;

    /*! Stores the receiver object of the command */
    DeviceType *Device;

    public:
    /*! The constructor. Does nothing */
    ddiCommand2():BaseType() {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param device Pointer to the receiver of the command
      \param name A string to identify the command. Useful for debugging. */
    ddiCommand2(ActionType action, DeviceType *device, std::string name, const std::type_info *infoFrom, const std::type_info *infoTo):
        BaseType(name, ddiCommandBase::DDI_COMMAND_2, infoFrom, infoTo),
        Action(action), Device(device) {}

    /*! The destructor. Does nothing */
    virtual ~ddiCommand2() {}

    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver. 
      \param from The data passed to the conversion operation
      \param to The result obtained from the conversion operation
      \result Boolean value, true if success, false otherwise */
    virtual ddiCommandBase::ReturnType Execute(cmnDataObject &from, cmnDataObject &to) {
        ddiCommandBase::ReturnType result;
        result = (Device->*Action)(from, to);
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
        const ddiCommand2<_deviceType> & command) {
    command.ToStream(output);
    return output;
}


#endif // _ddiCommand2_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCommand2.h,v $
// Revision 1.8  2006/07/07 02:46:50  pkaz
// cisstDeviceInterface: fixed minor typos in Doxygen comments.
//
// Revision 1.7  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.6  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.5  2006/05/04 23:33:05  kapoor
// cisstDeviceInterface: missing << operator for ddiCommand2.
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
// Revision 1.3  2004/04/08 06:02:58  kapoor
// Windows doesnt like typedefs with pointer or what? Anyways some no standard fix
//
// Revision 1.2  2004/03/26 06:05:34  kapoor
// Removed keyword const from Execute (const ddiCommand* ==> ddiCommand*)
//
// Revision 1.1  2004/03/24 20:52:19  kapoor
// Moved the command* classes to own files.
//
// ****************************************************************************
