/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCompositeCommand1.h,v 1.5 2006/06/03 00:27:45 kapoor Exp $
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
  \brief Defines a command with one argument for a composite device
  \ingroup cisstDeviceInterface
*/


#ifndef _ddiCompositeCommand1_h
#define _ddiCompositeCommand1_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiCommand1Base.h>
#include <iostream>

// Always include last
#include <cisstDeviceInterface/ddiExport.h>

/*!
  \ingroup cisstDeviceInterface

  This class contains a vector of two or more command
  objects. Currently it is used by composite device, where the command
  needs to be chained, such that it is executed on more than one
  device. This is the one argument version, that is the execute method
  take one argument.
 */
class CISST_EXPORT ddiCompositeCommand1 : public ddiCommand1Base {
    typedef ddiCommand1Base BaseType;

    /*! Vector to commands that constitute the composite command. */
    std::vector<ddiCommand1Base*> Commands;

    public:
    /*! Default constructor. Does nothing. */
    ddiCompositeCommand1(): BaseType("", ddiCommandBase::COMPOSITE_COMMAND_1, 0, 0) {}

    /*! Default destructor. Does nothing. */
    ~ddiCompositeCommand1() {}

    /*! Add a command to the composite. */
    void AddCommand(ddiCommand1Base* cmd);

    /*! Execute all the commands in the composite. */
    virtual ddiCommandBase::ReturnType Execute(cmnDataObject &obj);

    /*! For debugging. Dump to stream debug info of the constituent
      commands. */
    virtual std::string ToString() const;
    virtual void ToStream(std::ostream &out) const;
};


#endif // _ddiCompositeCommand1_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCompositeCommand1.h,v $
// Revision 1.5  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
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
