/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstDeviceInterface.i,v 1.9 2006/05/14 04:12:43 kapoor Exp $

  Author(s):	Anton Deguet
  Created on:   2006-05-02

--- begin cisst license - do not edit ---

CISST Software License Agreement(c)

Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.

This software ("CISST Software") is provided by The Johns Hopkins
University on behalf of the copyright holders and
contributors. Permission is hereby granted, without payment, to copy,
modify, display and distribute this software and its documentation, if
any, for research purposes only, provided that (1) the above copyright
notice and the following four paragraphs appear on all copies of this
software, and (2) that source code to any modifications to this
software be made publicly available under terms no more restrictive
than those in this License Agreement. Use of this software constitutes
acceptance of these terms and conditions.

The CISST Software has not been reviewed or approved by the Food and
Drug Administration, and is for non-clinical, IRB-approved Research
Use Only.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

--- end cisst license ---

*/


%module cisstDeviceInterfacePython


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"


%header %{
    // Put header files here
    #include "cisstDeviceInterface/cisstDeviceInterface.i.h"
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%rename(__str__) ToString;
%ignore *::ToStream;
%ignore operator<<;

%ignore *::operator[]; // We define __setitem__ and __getitem__

#define CISST_EXPORT
#define CISST_DEPRECATED

// Include per type of container
%include "cisstDeviceInterface/ddiBasicTypeDataObject.i"
%include "cisstDeviceInterface/ddiDynamicVectorDataObject.i"
%include "cisstDeviceInterface/ddiCommand.i"

// Wrap devices
%import "cisstDeviceInterface/ddiCommandBase.h"
%include "cisstDeviceInterface/ddiDeviceInterface.h"
%include "cisstDeviceInterface/ddiCompositeDevice.h"
%include "cisstDeviceInterface/ddiLoPoMoCo.h"
%include "cisstDeviceInterface/ddiTwoLoPoMoCo.h"
%include "cisstDeviceInterface/ddiThreeLoPoMoCo.h"
%include "cisstDeviceInterface/ddiFourLoPoMoCo.h"
%include "cisstDeviceInterface/ddiMEI.h"
%include "cisstDeviceInterface/ddiTypes.h"

%include "cisstDeviceInterface/ddiDMM16AT.h"
%include "cisstDeviceInterface/ddiRMM1612.h"

%include "cisstDeviceInterface/ddiCompositeMRRobotController.h"


%extend ddiGainData {
    inline const char * __str__() {
        return (*self).ToString().c_str();
    }
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cisstDeviceInterface.i,v $
//  Revision 1.9  2006/05/14 04:12:43  kapoor
//  cisstDeviceInterface Examples: Fixed Get/Set Gain data from IRE. Need
//  wrapping for ddiGainData.
//
//  Revision 1.8  2006/05/13 21:26:50  kapoor
//  cisstDeviceInterface and IRE: Added wrappers for 2, 3, 4 lopomocos.
//
//  Revision 1.7  2006/05/09 02:26:58  kapoor
//  cisstDeviceInterface and IRE: wrapping of the ddiCommand[0-2]Base class.
//
//  Revision 1.6  2006/05/07 05:19:55  kapoor
//  ddiDeviceInterface and IRE: Wrapping of ddiCommand[0-2] objects. Added
//  a new macro, which returns the exact type of command instead of base type.
//  This needs to be changed, not the most optimal way.
//
//  Revision 1.5  2006/05/07 04:45:26  kapoor
//  cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
//  Revision 1.4  2006/05/05 05:05:46  kapoor
//  cisstDeviceInterface wrapping: Moved GainData to ddiTypes (to be split later),
//  wrappers for basic types (double, int etc) to be passed through mailbox.
//
//  Revision 1.3  2006/05/04 23:29:50  kapoor
//  cisst wrappers: Addition of bool type to ddiVec*
//
//  Revision 1.2  2006/05/03 21:37:36  anton
//  cisstDeviceInterface wrappers: Added prelimiary wrapping for LoPoMoCo and MEI
//
//  Revision 1.1  2006/05/03 02:42:57  anton
//  cisstDeviceInterface wrappers: Added wrappers for ddiTypes, i.e. vectors of
//  data (work in progress ...)
//
//
// ****************************************************************************
