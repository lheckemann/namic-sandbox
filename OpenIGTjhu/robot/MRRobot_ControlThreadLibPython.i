/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: controlThreadLibPython.i,v 1.4 2006/07/09 04:05:08 kapoor Exp $

  Author(s): Anton Deguet

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


%module MRRobot_ControlThreadLibPython

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"

#ifdef SWIGPYTHON
%include "std_streambuf.i"
%include "std_iostream.i"

// We define __setitem__ and __getitem__
%ignore *::operator[];

#endif // SWIGPYTHON

%include "swigrun.i"

%ignore *::ToStream;
%ignore operator<<;

#define CISST_EXPORT
#define CISST_DEPRECATED

%header %{
    #include "cisstCommon/cisstCommon.i.h"
    #include "cisstVector/cisstVector.i.h"
    #include "cisstDeviceInterface/cisstDeviceInterface.i.h"
    #include "cisstRealTime/cisstRealTime.i.h"
    #include "ctfControlThread.h"
%}


%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"
%import "cisstDeviceInterface/cisstDeviceInterface.i"
%import "cisstRealTime/cisstRealTime.i"

%include "ctfControlThread.h"


/*
  $Log: controlThreadLibPython.i,v $
  Revision 1.4  2006/07/09 04:05:08  kapoor
  cissstRobot: Flushing the current state of pre-robot libraries for safe keeping. Use at own risk.
  Adds New eye robot and original black steady hand (7 DoF) kinematics.

  Revision 1.3  2006/06/22 03:15:20  kapoor
  cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
  Use at own risk. Adds new 5 BAR + Snake robot

  Revision 1.2  2006/06/03 00:38:55  kapoor
  cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
  Use at own risk.

  Revision 1.1  2006/05/14 03:26:28  kapoor
  cisstDeviceInterface: Cleanup of controlThread and addition of IRE to this
  example. Also location of XML files is now in a more decent location.



*/
