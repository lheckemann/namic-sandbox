/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ddiDynamicVectorDataObject.i,v 1.4 2006/06/03 00:27:45 kapoor Exp $

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


// Wrap basic types
%include "cisstDeviceInterface/ddiDynamicVectorDataObject.h"

%define DDI_DYNAMIC_VECTOR_INSTANTIATE(name, elementType, size)

// Instantiate the template
%template(name) ddiDynamicVectorDataObject<elementType, size>;

// Add operators [] for set/get
%extend ddiDynamicVectorDataObject<elementType, size> {
    inline const char * __str__() {
        return (*self).ToString().c_str();
    }

    inline elementType __getitem__(index_type index) const throw(std::out_of_range) {
        return (*self).Data.at(index);
    }

    inline void __setitem__(index_type index, elementType value) throw(std::out_of_range) {
        (*self).Data.at(index) = value;
    }
}

// Type addition for dynamic type checking
%{
    typedef ddiDynamicVectorDataObject<elementType, size> name;
%}

typedef ddiDynamicVectorDataObject<elementType, size> name;

%types(name *);
%enddef


DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecDouble16, double, 16);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecDouble8, double, 8);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecDouble4, double, 4);

DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecInt16, int, 16);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecInt8, int, 8);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecInt4, int, 4);

DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecShort16, short, 16);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecShort8, short, 8);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecShort4, short, 4);

DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecLong16, long, 16);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecLong8, long, 8);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecLong4, long, 4);

DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecBool16, bool, 16);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecBool8, bool, 8);
DDI_DYNAMIC_VECTOR_INSTANTIATE(ddiVecBool4, bool, 4);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiDynamicVectorDataObject.i,v $
//  Revision 1.4  2006/06/03 00:27:45  kapoor
//  cisstDeviceInterface: Attempt to come with a self describing command object.
//  We now also store the they of input/output data type for the cmnDataObject
//  class and its derivatives. Moreover some of these could be dynamically created.
//  See sine7 (the new example in realtime tutorial task).
//
//  Revision 1.3  2006/05/10 00:41:39  kapoor
//  cisstDeviceInterface: Added support for ddiVecShort and ddiVecLong.
//
//  Revision 1.2  2006/05/09 02:26:58  kapoor
//  cisstDeviceInterface and IRE: wrapping of the ddiCommand[0-2]Base class.
//
//  Revision 1.1  2006/05/07 04:45:26  kapoor
//  cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
//
//
// ****************************************************************************
