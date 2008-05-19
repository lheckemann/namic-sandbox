/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrixRotation3.h,v 1.5 2005/10/08 20:54:10 anton Exp $
  
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


#ifndef _vctDynamicMatrixRotation3_h
#define _vctDynamicMatrixRotation3_h

 
#include "cisstVector/vctMatrixRotation3Base.h"
#include "cisstVector/vctDynamicMatrix.h"

template <class _elementType>
class vctDynamicMatrixRotation3: public vctMatrixRotation3Base<vctDynamicMatrix<_elementType> > {
public:
    enum {ROWS = 3, COLS = 3};
    typedef _elementType value_type;
    typedef vctDynamicMatrixRotation3<value_type> ThisType;
    typedef vctDynamicMatrix<value_type> ContainerType;
    typedef vctMatrixRotation3Base<ContainerType> BaseType;
    typedef cmnTypeTraits<value_type> TypeTraits;

    inline vctDynamicMatrixRotation3() {
        this->SetSize(ROWS, COLS);
        this->Assign(this->Identity());
    }

    inline vctDynamicMatrixRotation3(const value_type & element00, const value_type & element01, const value_type & element02,
                                     const value_type & element10, const value_type & element11, const value_type & element12,
                                     const value_type & element20, const value_type & element21, const value_type & element22)
        throw(std::runtime_error)
    {
        this->SetSize(ROWS, COLS);
        this->From(element00, element01, element02,
                   element10, element11, element12,
                   element20, element21, element22);
    }

    inline vctDynamicMatrixRotation3(const vctAxisAngleRotation3<_elementType> & axisAngleRotation)
        throw(std::runtime_error)
    {
        this->SetSize(ROWS, COLS);
        this->From(axisAngleRotation);
    }

    inline vctDynamicMatrixRotation3(const vctQuaternionRotation3Base<vctDynamicVector<_elementType> > & quaternionRotation)
        throw(std::runtime_error)
    {
        this->SetSize(ROWS, COLS);
        this->From(quaternionRotation);
    }
    
    inline vctDynamicMatrixRotation3(const value_type & element00, const value_type & element01, const value_type & element02,
                                     const value_type & element10, const value_type & element11, const value_type & element12,
                                     const value_type & element20, const value_type & element21, const value_type & element22,
                                     bool normalizeInput)
    {
        this->SetSize(ROWS, COLS);
        if (normalizeInput) {
            this->FromNormalized(element00, element01, element02,
                                 element10, element11, element12,
                                 element20, element21, element22);
        } else {
            this->FromRaw(element00, element01, element02,
                          element10, element11, element12,
                          element20, element21, element22);
        }
    }

    inline vctDynamicMatrixRotation3(const vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                                     bool normalizeInput)
    {
        this->SetSize(ROWS, COLS);
        if (normalizeInput) {
            this->FromNormalized(axisAngleRotation);
        } else {
            this->FromRaw(axisAngleRotation);
        }
    }

    inline vctDynamicMatrixRotation3(const vctQuaternionRotation3Base<vctDynamicVector<_elementType> > & quaternionRotation,
                                     bool normalizeInput)
    {
        this->SetSize(ROWS, COLS);
        if (normalizeInput) {
            this->FromNormalized(quaternionRotation);
        } else {
            this->FromRaw(quaternionRotation);
        }
    }

};


#endif // _vctDynamicMatrixRotation3_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicMatrixRotation3.h,v $
//  Revision 1.5  2005/10/08 20:54:10  anton
//  cisstVector wrapping: Added missing license.
//
//  Revision 1.4  2005/10/08 20:12:12  anton
//  vctDynamicMatrixRotation3: Added ctor with flag _normalizeInput_ (see #110)
//
//  Revision 1.3  2005/10/07 09:35:28  anton
//  cisstVector wrappers: Start to add some flesh to vctMatRot3.
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
// ****************************************************************************
