/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctRodriguezRotation3.h,v 1.10 2006/01/03 03:38:58 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-01-13

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


/*! 
  \file 
  \brief Declaration of vctRodriguezRotation3
 */


#ifndef _vctRodriguezRotation3_h
#define _vctRodriguezRotation3_h


#include <cisstVector/vctRodriguezRotation3Base.h>
#include <cisstVector/vctExport.h>


/*!  \brief Define a rotation based on the rodriguez representation
  for a space of dimension 3.  This representation is based on a
  vector.  The direction of the vector determines the axis of rotation
  and its norm defines the amplitude of the rotation.

  \ingroup cisstVector

  This class is templated by the element type.

  \param _elementType The type of elements.

  \sa vctQuaternion
*/
template <class _elementType>
class vctRodriguezRotation3: public vctRodriguezRotation3Base<vctFixedSizeVector<_elementType, 3> >
{
public:
    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 3};
    typedef vctFixedSizeVector<value_type, 3> ContainerType;
    typedef vctRodriguezRotation3Base<ContainerType> BaseType;
    typedef vctRodriguezRotation3<value_type> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;

public:

    /*! Default constructor. Sets the rotation to (0, 0, 0). */
    inline vctRodriguezRotation3():
        BaseType(BaseType::Identity())
    {}

    inline vctRodriguezRotation3(const ThisType & other):
        BaseType(other)
    {}

    inline vctRodriguezRotation3(const BaseType & other):
        BaseType(other)
    {}


    /*!
      \name Constructors with normalization test.
      
      These constructors will check that the input is valid,
      i.e. normalized.  If the input is not normalized, an exception
      (of type \c std::runtime_error) will be thrown.  Each
      constructor uses the corresponding From() method based on the
      input type.

      \note See the cmnThrow() function if an \c abort is better than
      an exception for your application.
    */
    //@{


    /*! Constructor from a 3D vector.
      \param axis A vector of size 3, its norm represents the angle.
    */
    template<int __stride, class __dataPtrType>
    inline vctRodriguezRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis)
        throw(std::runtime_error)
    {
        From(axis);
    }

    /*! Constructor from a vctQuaternionRotation3. */
    template <class __containerType>
    inline vctRodriguezRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        throw(std::runtime_error)
    {
        From(quaternionRotation);
    }

    /*! Constructor from a vctMatrixRotation3. */
    template <class __containerType>
    inline vctRodriguezRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation)
        throw(std::runtime_error)
    {
        From(matrixRotation);
    }
    
    /*! Constructor from a vctAxisAngleRotation3. */
    inline vctRodriguezRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
        throw(std::runtime_error)
    {
        From(axisAngleRotation);
    }

    //@}



    /*!
      \name Constructors without normalization test
      
      These constructors will either assume that the input is
      normalized or normalize the input (a copy of it, if required)
      based on the last parameter provided.

      <ul>

        <li>If the normalization flag is set to #VCT_DO_NOT_NORMALIZE,
        the input is considered already normalized and the constructor
        will not perform any sanity check.  This can lead to numerical
        instabilities which have to be handled by the caller.

        <li>If the normalization flag is set to #VCT_NORMALIZE, the
        input will be normalized.  This option should be used whenever
        it is important to obtain a result as "normalized" as
        possible.

      </ul>
    */
    //@{

    /*! Constructor from a 3D vector.
      \param axis A vector of size 3, its norm represents the angle.
      \param normalizeInput Normalize the input or convert as is (#VCT_NORMALIZE or #VCT_DO_NOT_NORMALIZE)
    */
    template<int __stride, class __dataPtrType>
    inline vctRodriguezRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(axis);
        } else {
            FromRaw(axis);
        }
    }

    /*! Constructor from a vctQuaternionRotation3. */
    template <class __containerType>
    inline vctRodriguezRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(quaternionRotation);
        } else {
            FromRaw(quaternionRotation);
        }
    }

    /*! Constructor from a vctMatrixRotation3. */
    template <class __containerType>
    inline vctRodriguezRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(matrixRotation);
        } else {
            FromRaw(matrixRotation);
        }
    }
    
    /*! Constructor from a vctAxisAngleRotation3. */
    inline vctRodriguezRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(axisAngleRotation);
        } else {
            FromRaw(axisAngleRotation);
        }
    }

    //@}
};


#endif  // _vctRodriguezRotation3_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctRodriguezRotation3.h,v $
// Revision 1.10  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.9  2005/11/17 18:43:52  anton
// cisstVector transformations: Ongoing work re. ticket #110.
//
// Revision 1.8  2005/11/15 03:37:50  anton
// vctAxisAngle and vctRodriguez: Major updates re. ticket #110. Work in progress.
//
// Revision 1.7  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/09/05 22:54:22  ofri
// Added constructor for vctRodriguezRotation3Base, following ticket #169.  Plus a
// few cosmetic changes, such as replacing const_reference parameters with
// value_type
//
// Revision 1.5  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.4  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.3  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2005/02/22 23:00:16  anton
// vctAxisAngleRotation3 and vctRodriguezRotation3: Added IsNormalized() for
// syntactic completion, i.e. required for templated code such as tests.
//
// Revision 1.1  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
//
// ****************************************************************************

