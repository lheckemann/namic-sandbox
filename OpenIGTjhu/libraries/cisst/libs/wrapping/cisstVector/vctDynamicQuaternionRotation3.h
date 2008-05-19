/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicQuaternionRotation3.h,v 1.4 2005/10/08 20:54:10 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-08-18

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


#ifndef _vctDynamicQuaternionRotation3_h
#define _vctDynamicQuaternionRotation3_h

#include "cisstVector/vctQuaternionRotation3Base.h"
#include "cisstVector/vctDynamicVector.h"

template <class _elementType>
class vctDynamicQuaternionRotation3: public vctQuaternionRotation3Base<vctDynamicVector<_elementType> > {
public:
    enum {SIZE = 4};
    typedef _elementType value_type;
    typedef vctDynamicQuaternionRotation3<value_type> ThisType;
    typedef vctDynamicVector<value_type> ContainerType;
    typedef vctQuaternionRotation3Base<ContainerType> BaseType;
    typedef cmnTypeTraits<value_type> TypeTraits;

    inline vctDynamicQuaternionRotation3() {
        this->SetSize(SIZE);
        this->Assign(this->Identity());
    }

    inline vctDynamicQuaternionRotation3(const value_type & x, const value_type & y, const value_type & z,
                                         const value_type & r)
        throw(std::runtime_error)
    {
        this->SetSize(SIZE);
        this->From(x, y, z, r);
    }

    inline vctDynamicQuaternionRotation3(const vctAxisAngleRotation3<value_type>& axisAngleRotation)
        throw(std::runtime_error)
    {
        this->SetSize(SIZE);
        this->From(axisAngleRotation);
    }

    inline vctDynamicQuaternionRotation3(const vctMatrixRotation3Base<vctDynamicMatrix<value_type> > & matrixRotation)
        throw(std::runtime_error)
    {
        this->SetSize(SIZE);
        this->From(matrixRotation);
    }

    inline vctDynamicQuaternionRotation3(const value_type & x, const value_type & y, const value_type & z,
                                         const value_type & r,
                                         bool normalizeInput)
    {
        this->SetSize(SIZE);
        if (normalizeInput) {
            this->FromNormalized(x, y, z, r);
        } else {
            this->FromRaw(x, y, z, r);
        }
    }

    inline vctDynamicQuaternionRotation3(const vctAxisAngleRotation3<value_type>& axisAngleRotation,
                                         bool normalizeInput)
    {
        this->SetSize(SIZE);
        if (normalizeInput) {
            this->FromNormalized(axisAngleRotation);
        } else {
            this->FromRaw(axisAngleRotation);
        }
    }

    inline vctDynamicQuaternionRotation3(const vctMatrixRotation3Base<vctDynamicMatrix<value_type> > & matrixRotation,
                                         bool normalizeInput)
    {
        this->SetSize(SIZE);
        if (normalizeInput) {
            this->FromNormalized(matrixRotation);
        } else {
            this->FromRaw(matrixRotation);
        }
    }
};


#endif // _vctDynamicQuaternionRotation3_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicQuaternionRotation3.h,v $
//  Revision 1.4  2005/10/08 20:54:10  anton
//  cisstVector wrapping: Added missing license.
//
//  Revision 1.3  2005/10/08 20:13:30  anton
//  cisstVector wrapping: Updated vctQuatRot3 to match features of vctMatRot3.
//
//  Revision 1.2  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.1  2005/09/01 06:24:26  anton
//  cisst wrapping:
//  *: Reorganized files so that each library has its own CMakeLists.txt
//  *: Added preliminary wrapping for some transformations.  This compiles, loads
//  but is not tested.
//
//
// ****************************************************************************
