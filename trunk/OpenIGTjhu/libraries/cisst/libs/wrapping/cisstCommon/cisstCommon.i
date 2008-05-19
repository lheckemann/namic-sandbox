/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstCommon.i,v 1.14 2006/07/13 19:57:56 pkaz Exp $

  Author(s):	Anton Deguet
  Created on:   2004-10-06

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


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"

#ifdef SWIGPYTHON

%module cisstCommonPython

%include "std_streambuf.i"
%include "std_iostream.i"

// Force insertion of code related to Python iterators in cxx file
%fragment("PySwigIterator_T");

// We define __setitem__ and __getitem__
%ignore *::operator[];

#endif // SWIGPYTHON

#ifdef SWIGJAVA
%module cisstCommonJava
#endif // SWIGJAVA



%include "swigrun.i"

%header %{
    // Put header files here
    #include "cisstCommon/cisstCommon.i.h"
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%ignore *::ToStream;
%ignore operator<<;

#define CISST_EXPORT
#define CISST_DEPRECATED

%ignore CMN_LOG_DETAILS;

// Typemap used so that C++ pointers on cmnGenericObject base class
// can be "casted" to the actual derived class in Python.  This is
// useful for objects stored in cmnObjectRegister.  Without the
// typemap, the user ends-up with a base class object and no derived
// feature whatsoever.
#ifdef SWIGPYTHON
%typemap(out) cmnGenericObject * {
    if (result != NULL) {
        // create a string with a trailing "*" to retrieve the SWIG pointer type info
        std::string className = result->Services()->GetName() + " *";
        swig_type_info* typeInfo = SWIG_TypeQuery(className.c_str());
        // if the type info exists, i.e. this class has been wrapped, convert pointer
        if (typeInfo != NULL) {
            resultobj = SWIG_NewPointerObj((void*)(result), typeInfo, 0);
        } else {
            // fail, maybe a better fall back would be to return the base type, but this is really useless
            char buffer[256];
            sprintf(buffer, "Sorry, can't create a python object of type %s",
            className.c_str());
            PyErr_SetString(PyExc_TypeError, buffer);
            SWIG_fail;
        }
    } else {
        // Return None if object not found
        Py_INCREF(Py_None);
        resultobj = Py_None;
    }
}
#endif // SWIGPYTHON

%include "cisstCommon/cmnGenericObject.h"
%include "cisstCommon/cmnClassServicesBase.h"

#ifdef SWIGPYTHON
// Wrap the class register and add required code to generate python iterators
%include "cisstCommon/cmnClassRegister.h"
%template() std::pair<std::string, cmnClassServicesBase*>;
%template(cmnClassServicesContainer) std::map<std::string, cmnClassServicesBase*>;
%apply std::map<std::string, cmnClassServicesBase*>::const_iterator { 
    cmnClassRegister::const_iterator
};
%newobject cmnClassRegister::iterator(PyObject **PYTHON_SELF);
%extend cmnClassRegister {
    swig::PySwigIterator* iterator(PyObject **PYTHON_SELF) {
        return swig::make_output_iterator(self->begin(), self->begin(), self->end(), *PYTHON_SELF);
    }
    %pythoncode {
        def __iter__(self):
            return self.iterator()
    }
}

// Wrap the object register and add required code to generate python iterators
%include "cisstCommon/cmnObjectRegister.h"
%template() std::pair<std::string, cmnGenericObject*>;
%template(cmnGenericObjectContainer) std::map<std::string, cmnGenericObject*>;
%apply std::map<std::string, cmnGenericObject*>::const_iterator { 
    cmnObjectRegister::const_iterator
};
%newobject cmnObjectRegister::iterator(PyObject **PYTHON_SELF);
%extend cmnObjectRegister {
    swig::PySwigIterator* iterator(PyObject **PYTHON_SELF) {
        return swig::make_output_iterator(self->begin(), self->begin(), self->end(), *PYTHON_SELF);
    }
    %pythoncode {
        def __iter__(self):
            return self.iterator()
    }
}

// Wrap stream code related to the logging system
%template() std::basic_streambuf<char,std::char_traits<char > >;
%include "cisstCommon/cmnLODMultiplexerStreambuf.h"
%template(cmnLODMultiplexerStreambufChar) cmnLODMultiplexerStreambuf<char>;
%include "cisstCommon/cmnCallbackStreambuf.h"
%template(cmnCallbackStreambufChar) cmnCallbackStreambuf<char>;
%include "cisstCommon/cmnLogger.h"

// Wrap cmnPath
%include "cisstCommon/cmnPath.h"

#endif // SWIGPYTHON

// Wrap and instantiate useful type traits
%include "cisstCommon/cmnTypeTraits.h"
%template(cmnTypeTraitsDouble) cmnTypeTraits<double>;
%template(cmnTypeTraitsInt) cmnTypeTraits<int>;

// Wrap cmnDataObject
%include "cisstCommon/cmnDataObject.h"


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cisstCommon.i,v $
//  Revision 1.14  2006/07/13 19:57:56  pkaz
//  wrapping/cisstCommon: moved %module definition to cisstCommon.i (was in cisstCommonPython.i and cisstCommonJava.i). Fixes #233.
//
//  Revision 1.13  2006/05/24 14:23:52  anton
//  cisstCommon wrappers: Attempt to use SWIG to generate Java wrappers.  Work
//  in progress, currently untested and partial wrapping only.
//
//  Revision 1.12  2006/05/10 18:40:25  anton
//  cisstCommon class register: Updated to use a std::map to store class services
//  and added std::type_info to the class services for a more reliable type
//  identification of templated classes.
//
//  Revision 1.11  2006/05/03 02:42:57  anton
//  cisstDeviceInterface wrappers: Added wrappers for ddiTypes, i.e. vectors of
//  data (work in progress ...)
//
//  Revision 1.10  2006/03/20 21:59:34  anton
//  cisstCommon.i: Renamed cmnTypeTraitsFloat to cmnTypeTraitsInt (typo?).
//
//  Revision 1.9  2006/03/17 23:34:03  anton
//  cisstCommon wrappers: Fixed issue re. forward declaration of iterator
//  function.
//
//  Revision 1.8  2006/03/15 14:40:48  anton
//  cisstCommon wrapper: Added default parameter value for make_output_iterator
//  (icc 9.0 requires it).
//
//  Revision 1.7  2006/03/10 14:25:51  anton
//  cisstCommon wrapping:  Added support for python iterators with the object
//  and class registers.  Also added wrapping of cmnLogger to modify global LoD.
//
//  Revision 1.6  2006/03/07 04:28:46  anton
//  cisstCommon wrapping: Work in progress.
//
//  Revision 1.5  2006/02/21 22:22:55  pkaz
//  cisstCommon.i:  fixed cmnObjectRegister.FindObject typemap to return None rather
//  than NULL if string not found in registry.
//  Removed deprecated cmnObjectRegister.Get.  See ticket #214.
//
//  Revision 1.4  2006/02/11 18:06:20  anton
//  wrapping: Added autodoc for IRE in context documentation.
//
//  Revision 1.3  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.2  2005/09/06 01:36:14  anton
//  cisstCommon wrapping: Corrected content of cisstCommon.i.h and use it in
//  cisstCommon.i.
//
//  Revision 1.1  2005/09/01 06:24:26  anton
//  cisst wrapping:
//  *: Reorganized files so that each library has its own CMakeLists.txt
//  *: Added preliminary wrapping for some transformations.  This compiles, loads
//  but is not tested.
//
//  Revision 1.7  2005/08/19 05:30:44  anton
//  Wrapping: Main interface files have only the required #include, i.e. don't
//  include the SWIG generated cisstXyz.h.  Also added license.
//
//  Revision 1.6  2005/08/15 05:49:52  anton
//  cisstCommon.i: Instantiate and wrap cmnTypeTraits for double and float.
//
//  Revision 1.5  2005/07/02 00:16:52  anton
//  cisstCommon SWIG interface: Remove Get method from cmnObjectRegister and use
//  the normal C++ method FindObject with the newly added typemap for
//  cmnGenericObject pointer.  The typemap is much better since now all method
//  returning a pointer to the base class can perform a "cast" using the typemap.
//
//  Revision 1.4  2005/06/23 15:12:58  anton
//  cisstCommon.i: Handle CISST_DEPRECATED for SWIG.
//
//  Revision 1.3  2005/06/15 15:06:20  anton
//  Python wrapping: Split the cisstVector wrapping code in smaller files to
//  increase readability.
//
//  Revision 1.6  2005/06/06 00:30:43  anton
//  cisstCommon wrapping: %ignore ToStream()
//
//  Revision 1.5  2005/06/03 22:31:32  anton
//  cisstCommon wrapping: Minor correction in error message.
//
//  Revision 1.4  2005/01/19 21:06:56  anton
//  cisst wrapping:  Port to SWIG 1.3.24 (see ticket #122).  Main modification is
//  %include "swigrun.i"
//
//  Revision 1.3  2005/01/12 15:51:07  anton
//  Python Wrapping: Minor updates, clean up of interface files.
//
//  Revision 1.2  2004/10/28 04:48:31  anton
//  wrapping: Remove commented code
//
//  Revision 1.1  2004/10/14 16:18:02  anton
//  wrapping: changed the names of the modules from cisstXyz.i to
//  cisstXyzPython.i to avoid dynamic loading issues with Windows/Python.
//  Updated the __str__ method to use ToString().  For the register, used %extend
//  to add the Get() method used in Python to get the right object.
//
//  Revision 1.1  2004/10/07 18:23:41  anton
//  cisstCommon SWIG wrapping preliminary version.
//
//
// ****************************************************************************
