/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCommandBase.h,v 1.12 2006/06/03 00:27:45 kapoor Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2006-05-02
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
  \brief Defines a base class for a command
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiCommandBase_h
#define _ddiCommandBase_h

#include <iostream>
#include <typeinfo>

/*!
  \ingroup cisstDeviceInterface
  */

#define DDI_DECLARE_COMMAND0(commandName) \
public:\
    ddiCommandBase::ReturnType commandName();\
protected:\
    ddiCommandBase::ReturnType commandName##Protected ();\

#define DDI_ADD_METHOD_COMMAND0(className, commandName) \
    this->AddMethodZeroArg(&className::commandName##Protected, this, #commandName);

#define DDI_BEGIN_IMPLEMENT_COMMAND0(className, commandName) \
ddiCommandBase::ReturnType className::commandName() {\
    static ddiCommand0Base* cmdObj = this->GetMethodZeroArg(#commandName);\
        return cmdObj->Execute();\
}    \
ddiCommandBase::ReturnType className::commandName##Protected () {\

#define DDI_END_IMPLEMENT_COMMAND0 }

#define DDI_DECLARE_COMMAND1_IN(commandName, dataType) \
public:\
    typedef dataType commandName##DataType; \
    ddiCommandBase::ReturnType commandName(dataType &data);\
protected:\
    ddiCommandBase::ReturnType commandName##Protected (cmnDataObject &obj);\

#define DDI_DECLARE_COMMAND1_OUT(commandName, dataType) \
public:\
    typedef dataType commandName##DataType; \
    dataType commandName();\
protected:\
    ddiCommandBase::ReturnType commandName##Protected (cmnDataObject &obj);\

#define DDI_ADD_METHOD_COMMAND1(className, commandName) \
    this->AddMethodOneArg(&className::commandName##Protected, this, #commandName,\
            &typeid(className::commandName##DataType), className::commandName##DataType::ClassServices());

#define DDI_BEGIN_IMPLEMENT_COMMAND1_IN(className, commandName) \
ddiCommandBase::ReturnType className::commandName(className::commandName##DataType &data) {\
    static ddiCommand1Base* cmdObj = this->GetMethodOneArg(#commandName);\
    return cmdObj->Execute(data);\
}    \
ddiCommandBase::ReturnType className::commandName##Protected (cmnDataObject &obj) {\
    className::commandName##DataType *pdata = dynamic_cast< className::commandName##DataType* >(&obj); \
    if (pdata == NULL) return ddiCommandBase::BAD_INPUT;

#define DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(className, commandName) \
className::commandName##DataType className::commandName() {\
    static ddiCommand1Base* cmdObj = this->GetMethodOneArg(#commandName);\
    static className::commandName##DataType data; \
    cmdObj->Execute(data);\
    return data;\
}    \
ddiCommandBase::ReturnType className::commandName##Protected (cmnDataObject &obj) {\
    className::commandName##DataType *pdata = dynamic_cast< className::commandName##DataType * >(&obj); \
    if (pdata == NULL) return ddiCommandBase::BAD_INPUT;

#define DDI_END_IMPLEMENT_COMMAND1 }

#define DDI_DECLARE_COMMAND2(commandName, dataTypeIn, dataTypeOut) \
public:\
    typedef dataTypeIn commandName##DataTypeIn;\
    typedef dataTypeOut commandName##DataTypeOut; \
    dataTypeOut commandName(dataTypeIn &from);\
protected:\
    ddiCommandBase::ReturnType commandName##Protected (cmnDataObject &from, cmnDataObject &to);\

#define DDI_ADD_METHOD_COMMAND2(className, commandName) \
    this->AddMethodTwoArg(&className::commandName##Protected, this, #commandName,\
            &typeid(className::commandName##DataTypeIn), &typeid(className::commandName##DataTypeOut));

#define DDI_BEGIN_IMPLEMENT_COMMAND2(className, commandName) \
className::commandName##DataTypeOut className::commandName(className::commandName##DataTypeIn &from) {\
    static ddiCommand2Base* cmdObj = this->GetMethodTwoArg(#commandName);\
    static className::commandName##DataTypeOut to; \
    cmdObj->Execute(from, to);\
    return to;\
}    \
ddiCommandBase::ReturnType className::commandName##Protected (cmnDataObject &from, cmnDataObject &to) {\
    className::commandName##DataTypeIn *fromdata = dynamic_cast< className::commandName##DataTypeIn* >(&from); \
    if (fromdata == NULL) return ddiCommandBase::BAD_INPUT;\
    className::commandName##DataTypeOut *todata = dynamic_cast< className::commandName##DataTypeOut* >(&to); \
    if (todata == NULL) return ddiCommandBase::BAD_INPUT;

#define DDI_END_IMPLEMENT_COMMAND2 }

class ddiCommandBase {
    public:
        std::string Name;
        int Type;

        enum CommandType {
            UNKNOWN = 0,
            DDI_COMMAND_0,
            DDI_COMMAND_1,
            DDI_COMMAND_2,
            COMPOSITE_COMMAND_0,
            COMPOSITE_COMMAND_1,
            COMPOSITE_COMMAND_2,
            RTS_COMMAND_0,
            RTS_COMMAND_1,
            RTS_COMMAND_2
        };

        /* use to bitshift and or for return value of a composite
           would limit the number of composite devices to 31 for
           an int return value
        */
        enum { RETURN_TYPE_BIT_SIZE = 1 };

        /* the error value is -ve of the return value */
        enum ReturnType {
            DEV_OK = 0,
            DEV_NOT_OK = 1,
            BAD_COMMAND = 12,
            NO_MAILBOX = 13,
            BAD_INPUT = 14,
            NO_DEVICE = 15
        };

        /*! The constructor. Does nothing */
        ddiCommandBase():Name("??"), Type(UNKNOWN) {}
        ddiCommandBase(std::string name, CommandType type):
            Name(name), Type(type) {}

        /*! The destructor. Does nothing */
        virtual ~ddiCommandBase() {}

        /*! For debugging. Generate a human readable output for the command
          object */
        virtual std::string ToString() const = 0;
        virtual void ToStream(std::ostream &out) const = 0;

        /*! The overloaded right shift operator to redirect output to a
          ostream */
        friend std::ostream& operator << (std::ostream& out, ddiCommandBase& data);
};


#endif // _ddiCommandBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCommandBase.h,v $
// Revision 1.12  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.11  2006/06/01 18:43:11  kapoor
// cisstDeviceInterface: Simplified the marco for the commands. Only
// one macro requires input and/or output types. The others just the
// class and method name.
//
// Revision 1.10  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.9  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.8  2006/05/05 03:35:08  kapoor
// cisstDeviceInterface: COMMAND_1 is replaced by 1_IN and 1_OUT to specify
// read and write operations. This macro only changes the "C" like API that
// is provided for the wrapping of device class, such that the output is
// the can be assigned directly.
//
// Revision 1.7  2006/05/03 18:36:44  kapoor
// Extended command0,1 to tasks. Work in progress...
//
// Revision 1.6  2006/05/03 05:33:42  kapoor
// MACROS for DDI_COMMAND* declaration, implementation etc.
//
// Revision 1.5  2006/05/03 01:49:59  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.4  2006/05/02 20:20:20  kapoor
// Import of a fresh file for ddiCommandBase, zero, one and two argument
// device, composite and task (thru mailbox) commands are derrived thru this.
//
//
// ****************************************************************************
