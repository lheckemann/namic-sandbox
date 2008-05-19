/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ddiBasicTypeDataObject.i,v 1.3 2006/05/10 00:41:39 kapoor Exp $

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
%include "cisstDeviceInterface/ddiBasicTypeDataObject.h"

%define DDI_BASIC_TYPE_INSTANTIATE(name, elementType)

// Instantiate the template
%template(name) ddiBasicTypeDataObject<elementType>;

// Add for __str__
%extend ddiBasicTypeDataObject<elementType> {
    inline const char * __str__() {
        return (*self).ToString().c_str();
    }

}

// Type addition for dynamic type checking
%{
    typedef ddiBasicTypeDataObject<elementType> name;
%}

typedef ddiBasicTypeDataObject<elementType> name;

%types(name *);
%enddef

DDI_BASIC_TYPE_INSTANTIATE(ddiDouble, double);
DDI_BASIC_TYPE_INSTANTIATE(ddiInt, int);
DDI_BASIC_TYPE_INSTANTIATE(ddiShort, short);
DDI_BASIC_TYPE_INSTANTIATE(ddiLong, long);
DDI_BASIC_TYPE_INSTANTIATE(ddiBool, bool);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiBasicTypeDataObject.i,v $
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
