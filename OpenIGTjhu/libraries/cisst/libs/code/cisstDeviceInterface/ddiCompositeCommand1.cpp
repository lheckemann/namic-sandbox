/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCompositeCommand1.cpp,v 1.3 2006/05/07 04:45:26 kapoor Exp $
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


#include <cisstDeviceInterface/ddiCompositeCommand1.h>


void ddiCompositeCommand1::AddCommand(ddiCommand1Base* cmd) {
    if (cmd) {
        this->Name += std::string("+") + cmd->Name;
        Commands.push_back(cmd);
    }
}


ddiCommandBase::ReturnType ddiCompositeCommand1::Execute(cmnDataObject &obj) {
    int result = (int)ddiCommandBase::DEV_OK;
    for (unsigned int i = 0; i < Commands.size(); i++) {
        if (Commands[i]) result = (result << (int)ddiCommandBase::RETURN_TYPE_BIT_SIZE) | (int)Commands[i]->Execute(obj);
    }
    return ((ddiCommandBase::ReturnType)result);
}


#include <sstream>
std::string ddiCompositeCommand1::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}

void ddiCompositeCommand1::ToStream(std::ostream &out)  const{
    out << " Command Type " << this->Type << ": ";
    for (unsigned int i = 0; i < Commands.size(); i++) {
        if (Commands[i]) Commands[i]->ToStream(out);
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCompositeCommand1.cpp,v $
// Revision 1.3  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.2  2006/05/03 01:51:37  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.1  2006/05/02 20:39:16  kapoor
// Import of fresh files for zero, one or two argument commands.
//
//
// ****************************************************************************
