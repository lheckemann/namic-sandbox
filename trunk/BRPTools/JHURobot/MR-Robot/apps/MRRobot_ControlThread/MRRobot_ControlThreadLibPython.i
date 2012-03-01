/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: controlThreadLibPython.i,v 1.4 2006/07/09 04:05:08 kapoor Exp $

  Author(s): Anton Deguet

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

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
    #include "cisstCommon/cmnPython.h"
    #include "cisstVector/vctPython.h"
    #include "cisstMultiTask/mtsPython.h"
    #include "ctfControlThread.h"
    #include "ctfGainData.h"
%}

%init %{
      import_array()   /* Initial function for NumPy */
%}


%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"
%import "cisstMultiTask/cisstMultiTask.i"

%include "ctfControlThread.h"
%include "ctfGainData.h"
