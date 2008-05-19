/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiDeviceInterface.cpp,v 1.11 2006/06/03 00:27:45 kapoor Exp $
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


#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstDeviceInterface/ddiTypes.h>
#include <cisstDeviceInterface/ddiNOPS.h>

// these should be in independent file
std::ostream& operator << (std::ostream& out, ddiCommand0Base& data) {
    data.ToStream(out);
    return out;
}

std::ostream& operator << (std::ostream& out, ddiCommand1Base& data) {
    data.ToStream(out);
    return out;
}

std::ostream& operator << (std::ostream& out, ddiCommand2Base& data) {
    data.ToStream(out);
    return out;
}

// thses should be in independent file
ddiNOPS *ddiDeviceInterface::NOPSDevice = new ddiNOPS();

ddiCommand0<ddiNOPS> *ddiDeviceInterface::NOPSCommand0 = new ddiCommand0<ddiNOPS>(&ddiNOPS::NOPS0,
        ddiDeviceInterface::NOPSDevice, "NOPS0");

ddiCommand1<ddiNOPS> *ddiDeviceInterface::NOPSCommand1 = new ddiCommand1<ddiNOPS>(&ddiNOPS::NOPS1,
        ddiDeviceInterface::NOPSDevice, "NOPS1", &typeid(ddiInt), ddiInt::ClassServices());

ddiCommand2<ddiNOPS> *ddiDeviceInterface::NOPSCommand2 = new ddiCommand2<ddiNOPS>(&ddiNOPS::NOPS2,
        ddiDeviceInterface::NOPSDevice, "NOPS2", &typeid(ddiInt), &typeid(ddiInt));

ddiCommand0Base* ddiDeviceInterface::GetMethodZeroArg(std::string operation) {
    ZeroArgOperationNameMapType::iterator op = ZeroArgOperations.end();
    op = ZeroArgOperations.find(operation);
    if (op != ZeroArgOperations.end())
        return op->second;
    else {
        CMN_LOG_CLASS(1) << "In method GetMethodZeroArg, can't find \"" << operation << "\"" << std::endl;
        return NOPSCommand0;
    }
}

ddiCommand1Base* ddiDeviceInterface::GetMethodOneArg(std::string operation) {
    OneArgOperationNameMapType::iterator op = OneArgOperations.end();
    op = OneArgOperations.find(operation);
    if (op != OneArgOperations.end())
        return op->second;
    else {
        CMN_LOG_CLASS(1) << "In method GetMethodOneArg, can't find \"" << operation << "\"" << std::endl;
        return NOPSCommand1;
    }
}

ddiCommand2Base* ddiDeviceInterface::GetMethodTwoArg(std::string operation) {
    TwoArgOperationNameMapType::iterator op = TwoArgOperations.end();
    op = TwoArgOperations.find(operation);
    if (op != TwoArgOperations.end())
        return op->second;
    else {
        CMN_LOG_CLASS(1) << "In method GetMethodTwoArg, can't find \"" << operation << "\"" << std::endl;
        return NOPSCommand2;
    }
}

std::vector<std::string> ddiDeviceInterface::Provides (void) {
    //For now we return all the read methods provided by this task
    std::vector<std::string> *provides = new  std::vector<std::string>;
    ZeroArgOperationNameMapType::iterator iter0;
    for (iter0 = ZeroArgOperations.begin(); iter0 != ZeroArgOperations.end(); ++iter0) {
        provides->push_back(iter0->first);
    }
    OneArgOperationNameMapType::iterator iter1;
    for (iter1 = OneArgOperations.begin(); iter1 != OneArgOperations.end(); ++iter1) {
        provides->push_back(iter1->first);
    }
    TwoArgOperationNameMapType::iterator iter2;
    for (iter2 = TwoArgOperations.begin(); iter2 != TwoArgOperations.end(); ++iter2) {
        provides->push_back(iter2->first);
    }
    return *provides;
}

ddiDeviceInterface::ZeroArgOperationNameMapType & ddiDeviceInterface::GetDeviceMapZeroArg() {
    return ZeroArgOperations;
}

ddiDeviceInterface::OneArgOperationNameMapType & ddiDeviceInterface::GetDeviceMapOneArg() {
    return OneArgOperations;
}

ddiDeviceInterface::TwoArgOperationNameMapType & ddiDeviceInterface::GetDeviceMapTwoArg() {
    return TwoArgOperations;
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiDeviceInterface.cpp,v $
// Revision 1.11  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.10  2006/05/13 21:26:04  kapoor
// cisstDeviceInterface: Missed Ampersand (not passed by reference) in concat maps
// lead to a silly bug.
//
// Revision 1.9  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.8  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.7  2006/05/05 05:07:37  kapoor
// cisstDeviceInterface: Provides method is no longer pure virtual. It can be
// overloaded to provide a non-default behaviour. By default it return, the
// list of name strings of all commands.
//
// Revision 1.6  2006/05/04 23:34:18  kapoor
// cisstDeviceInterface: Added missing << operator for ddiCommand0Base.
//
// Revision 1.5  2006/05/03 01:51:37  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/03/04 20:31:46  anton
// ddiDeviceInterface: Added Log whenever a GetMethod() fails.  See #130.
//
// Revision 1.2  2005/02/28 22:43:34  anton
// cisstDevices and real-time:  Added Log (see #130).
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.4  2004/03/26 06:09:13  kapoor
// Removed the keyword const from Execute (const ddiCommand* ==> ddiCommand*)
//
// Revision 1.3  2004/03/24 21:00:28  kapoor
// Return a command object instead of a member function pointer. This was done
// to handle composite devices. The command object contains the information
// on the deivce that has to handle the operation besides just the operation,
// which is needed to resolve who should handle the operaion in case of a
// composite device.
//
// Revision 1.2  2004/03/17 16:48:25  kapoor
// Added full functionality.
//
// Revision 1.1  2004/03/16 17:57:42  kapoor
// Started work on Device Interface.
//
// ****************************************************************************
