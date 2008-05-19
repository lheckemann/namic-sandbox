/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */


/*
  $Id: ddiBasicTypeDataObject.h,v 1.2 2006/06/03 00:27:45 kapoor Exp $
  
  Author(s):  Ankur Kapoor, Anton Deguet
  Created on:  2006-05-05

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

#ifndef _ddiBasicTypeDataObject_h
#define _ddiBasicTypeDataObject_h

#include <cisstCommon/cmnDataObject.h>

template <class _elementType>
class ddiBasicTypeDataObject:public cmnDataObject {

    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

    public:
        typedef _elementType DataType;
        DataType Data;
        virtual long int TypeId(void) const {return 2;}
        virtual std::string TypeName(void) const { return cmnTypeTraits<DataType>::TypeName(); }

    virtual std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

        virtual void ToStream(std::ostream &out) const {out << Data << " ";}
        virtual void pack(PackedTypeArray &packarray) {packarray << Data;}
        virtual cmnDataObject& unpack(PackedTypeArray &unpackarray) {unpackarray >> Data; return *this;}
        ddiBasicTypeDataObject():Data(DataType()) {}
        ddiBasicTypeDataObject(DataType value):Data(DataType(value)) {}
        ~ddiBasicTypeDataObject() {}
};

#endif

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiBasicTypeDataObject.h,v $
// Revision 1.2  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.1  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
//
// ****************************************************************************
