/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCompositeCommand2.h,v 1.8 2006/05/09 03:29:47 kapoor Exp $
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


#ifndef _ddiCompositeCommand2_h
#define _ddiCompositeCommand2_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiCommand2Base.h>
#include <iostream>

/*!
  \ingroup cisstDeviceInterface

  This class contains a vector of two or more command
  objects. Currently it is used by composite device, where the command
  needs to be chained, such that it is executed on more than one
  device. This is the two argument version, that is the execute method
  take two arguments.
 */
class ddiCompositeCommand2: public ddiCommand2Base {
    typedef ddiCommand2Base BaseType;

    /*! Vector to commands that constitute the composite command. */
    std::vector<ddiCommand2Base*> Commands;

    public:
    /*! Default constructor. Does nothing. */
    ddiCompositeCommand2(): BaseType("", ddiCommandBase::COMPOSITE_COMMAND_2, 0, 0) {}

    /*! Default destructor. Does nothing. */
    ~ddiCompositeCommand2() {}

    /*! Add a command to the composite. */
    void AddCommand(ddiCommand2Base* cmd);

    /*! Execute all the commands in the composite. */
    virtual ddiCommandBase::ReturnType Execute(cmnDataObject &from, cmnDataObject &to);

    /*! For debugging. Dump to stream debug info of the constituent
      commands. */
    virtual std::string ToString() const;
    virtual void ToStream(std::ostream &out) const;
};


#endif // _ddiCompositeCommand2_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCompositeCommand2.h,v $
// Revision 1.8  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.7  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.6  2006/05/03 01:49:59  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.5  2006/05/02 20:23:21  kapoor
// Two argument version of the ddiCommand (device & composites)
//
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2004/05/11 17:55:27  anton
// Switch back to ddiCompositeCommand from ddiCommandComposite.  There has been
// a minor communication problem between Ankur and I.  Sorry.
//
//
// Revision 1.1  2004/05/04 20:45:45  anton
// Added code for composite devices
//
//
// Revision 1.2  2004/03/26 06:05:34  kapoor
// Removed keyword const from Execute (const ddiCommand* ==> ddiCommand*)
//
// Revision 1.1  2004/03/24 20:54:25  kapoor
// Moved CompositeCommand* to its own file.
//
// ****************************************************************************
