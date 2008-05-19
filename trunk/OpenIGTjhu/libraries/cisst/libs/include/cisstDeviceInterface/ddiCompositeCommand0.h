/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCompositeCommand0.h,v 1.3 2006/05/07 04:45:26 kapoor Exp $
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
  \brief Defines a command with two argument for a composite device 
  \ingroup cisstDeviceInterface
*/


#ifndef _ddiCompositeCommand0_h
#define _ddiCompositeCommand0_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiCommand0Base.h>
#include <iostream>

/*!
  \ingroup cisstDeviceInterface

  This class contains a vector of two or more command
  objects. Currently it is used by composite device, where the command
  needs to be chained, such that it is executed on more than one
  device. This is the no argument version, that is the execute method
  take no arguments.
 */
class ddiCompositeCommand0: public ddiCommand0Base {
    typedef ddiCommand0Base BaseType;

    /*! Vector to commands that constitute the composite command. */
    std::vector<ddiCommand0Base*> Commands;

    public:
    /*! Default constructor. Does nothing. */
    ddiCompositeCommand0(): BaseType("", ddiCommandBase::COMPOSITE_COMMAND_0) {}

    /*! Default destructor. Does nothing. */
    ~ddiCompositeCommand0() {}

    /*! Add a command to the composite. */
    void AddCommand(ddiCommand0Base* cmd);

    /*! Execute all the commands in the composite. */
    virtual ddiCommandBase::ReturnType Execute();

    /*! For debugging. Dump to stream debug info of the constituent
      commands. */
    virtual std::string ToString() const;
    virtual void ToStream(std::ostream &out) const;
};


#endif // _ddiCompositeCommand0_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCompositeCommand0.h,v $
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
