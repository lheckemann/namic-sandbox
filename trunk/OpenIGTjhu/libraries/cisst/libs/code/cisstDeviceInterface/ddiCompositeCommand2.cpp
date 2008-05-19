/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCompositeCommand2.cpp,v 1.7 2006/05/07 04:45:26 kapoor Exp $
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


#include <cisstDeviceInterface/ddiCompositeCommand2.h>


void ddiCompositeCommand2::AddCommand(ddiCommand2Base* cmd) {
    if (cmd) {
        this->Name += std::string("+") + cmd->Name;
        Commands.push_back(cmd);
    }
}


ddiCommandBase::ReturnType ddiCompositeCommand2::Execute(cmnDataObject &from, cmnDataObject &to) {
    int result = (int)ddiCommandBase::DEV_OK;
    for (unsigned int i = 0; i < Commands.size(); i++) {
        if (Commands[i]) result = (result << (int)ddiCommandBase::RETURN_TYPE_BIT_SIZE) | (int)Commands[i]->Execute(from, to);
    }
    return ((ddiCommandBase::ReturnType)result);
}

#include <sstream>
std::string ddiCompositeCommand2::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}

void ddiCompositeCommand2::ToStream(std::ostream &out) const {
    out << " Command Type " << this->Type << ": ";
    for (unsigned int i = 0; i < Commands.size(); i++) {
        if (Commands[i]) Commands[i]->ToStream(out);
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCompositeCommand2.cpp,v $
// Revision 1.7  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.6  2006/05/03 01:51:37  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.5  2006/05/02 20:39:16  kapoor
// Import of fresh files for zero, one or two argument commands.
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
// Revision 1.2  2004/03/26 06:09:13  kapoor
// Removed the keyword const from Execute (const ddiCommand* ==> ddiCommand*)
//
// Revision 1.1  2004/03/24 20:55:34  kapoor
// Moved CompositeCommand* to its own file.
//
// ****************************************************************************
