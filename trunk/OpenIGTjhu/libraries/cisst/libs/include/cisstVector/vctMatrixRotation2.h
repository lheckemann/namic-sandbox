/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation2.h,v 1.12 2006/01/03 03:38:58 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2005-02-23

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
  \brief Declaration of vctMatrixRotation2
 */


#ifndef _vctMatrixRotation2_h
#define _vctMatrixRotation2_h


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctMatrixRotation2Base.h>

#include <cisstVector/vctExport.h>


/*!
  \brief Define a rotation matrix for a space of dimension 2

  \ingroup cisstVector

  This class is templated by the element type.  It is derived from
  vctFixedSizeMatrix and the main difference is that the size is set
  to 2 by 2.

  \param _elementType The type of elements of the matrix.

  \sa vctFixedSizeMatrix
*/
template <class _elementType>
class vctMatrixRotation2: public vctMatrixRotation2Base<vctFixedSizeMatrix<_elementType, 2, 2> >
{
public:
    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {ROWS = 2, COLS = 2};
    enum {DIMENSION = 2};
    typedef vctFixedSizeMatrix<value_type, 2, 2> ContainerType;
    typedef vctMatrixRotation2Base<ContainerType> BaseType;
    typedef vctMatrixRotation2<_elementType> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<_elementType> TypeTraits;

    /*! Default constructor. Sets the rotation matrix to identity. */
    inline vctMatrixRotation2():
        BaseType(BaseType::Identity())
    {}

    inline vctMatrixRotation2(const ThisType & other):
        BaseType(other)
    {}

    inline vctMatrixRotation2(const BaseType & other):
        BaseType(other)
    {}

    /*! The assignment from BaseType (i.e. a 2 by 2 fixed size matrix)
      has to be redefined for this class (C++ restriction).  This
      operator uses the Assign() method inherited from the BaseType.
      This operator (as well as the Assign method) allows to set a
      rotation matrix to whatever value without any further validity
      checking.  It is recommended to use it with caution. */ 
    inline ThisType & operator = (const ContainerType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    inline ThisType & operator = (const ThisType & other) {
  return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    inline ThisType & operator = (const BaseType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }


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


    /*! Constructor from 4 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation2<double> matrix( 0.0, 1.0,
                                        -1.0, 0.0);
      \endcode
    */
    inline vctMatrixRotation2(const value_type & element00, const value_type & element01,
                              const value_type & element10, const value_type & element11)
        throw(std::runtime_error)
    {
        this->From(element00, element01,
                   element10, element11);
    }



    /*!
      Constructor from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2>
    inline vctMatrixRotation2(const vctFixedSizeConstVectorBase<2, __stride1, _elementType, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<2, __stride2, _elementType, __dataPtrType2>& v2,
                              bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->From(v1, v2, vectorsAreColumns);
    }

    /*!
      Constructor from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline vctMatrixRotation2(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->From(v1, v2, vectorsAreColumns);
    }

    /*! Construction from a vctAngleRotation2. */
    inline vctMatrixRotation2(const vctAngleRotation2 & angleRotation)
        throw(std::runtime_error)
    {
        this->From(angleRotation);
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
      it is important to obtain a result as "normalized" as possible.

      </ul>
    */
    //@{


    /*! Constructor from 4 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation2<double> matrix( 0.0, 1.0,
                                        -1.0, 0.0);
      \endcode
    */
    inline vctMatrixRotation2(const value_type & element00, const value_type & element01,
                              const value_type & element10, const value_type & element11,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(element00, element01,
                                 element10, element11);
        } else {
            this->FromRaw(element00, element01,
                          element10, element11);
        }
    }



    /*!
      Constructor from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2>
    inline vctMatrixRotation2(const vctFixedSizeConstVectorBase<2, __stride1, _elementType, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<2, __stride2, _elementType, __dataPtrType2>& v2,
                              bool vectorsAreColumns, bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(v1, v2, vectorsAreColumns);
        } else {
            this->FromRaw(v1, v2, vectorsAreColumns);
        }
    }

    /*!
      Constructor from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline vctMatrixRotation2(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              bool vectorsAreColumns, bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(v1, v2, vectorsAreColumns);
        } else {
            this->FromRaw(v1, v2, vectorsAreColumns);
        }
    }

    /*! Construction from a vctAngleRotation2. */
    inline vctMatrixRotation2(const vctAngleRotation2 & angleRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(angleRotation);
        } else {
            this->FromRaw(angleRotation);
        }
    }
    //@}



    /*! Initialize this rotation matrix with a 2x2 matrix.  This constructor only takes a matrix of the
      same element type.
      \note This constructor does not verify normalization.  It is introduced to allow using results
      of matrix operations and assign them to a rotation matrix.
      \note The constructor is declared explicit, to force the user to be aware of the conversion
      being made.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    explicit inline
    vctMatrixRotation2(const vctFixedSizeMatrixBase<2, 2, __rowStride, __colStride, _elementType, __dataPtrType> & matrix):
        BaseType(matrix)
    {}

};


#endif  // _vctMatrixRotation2_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctMatrixRotation2.h,v $
// Revision 1.12  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.11  2005/12/02 02:40:23  anton
// vctRotationMatrix2: Update for gcc 4.0.
//
// Revision 1.10  2005/12/01 22:11:57  anton
// cisstVector: 2D transformations, added a base class for rotation matrix 2.
//
// Revision 1.9  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.8  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/07/22 19:06:46  ofri
// vct[Quaternion|Matrix]Rotation[2|3]: Fixing compilation error related to
// check-in [1253].
//
// Revision 1.6  2005/07/22 15:08:21  ofri
// Answer for ticket #155: added operator * that returns ThisType to
// vctMatrixRotation2, vctMatrixRotation3, vctQuaternionRotation3
//
// Revision 1.5  2005/07/19 18:24:28  anton
// vctMatrixRotation{2,3}: Added constructor and From methods to set a rotation
// matrix from 2 (respectively 3) vectors.  See ticket #151.
//
// Revision 1.4  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.3  2005/05/13 18:23:51  anton
// cisstVector: Both vctMatrixRotation{2,3}::Normalized() where normalizing
// identity.  Result is now initialized from *this.
//
// Revision 1.2  2005/04/28 03:58:35  anton
// cisstVector: Added methods ApplyTo(input) returning by copy as specified
// in ticket #108.  Added Doxygen documentation and moved from EqualRotation()
// to AlmostEqual().
//
// Revision 1.1  2005/02/24 01:51:50  anton
// cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
// code has not been tested.   The normalization and conversion methods have
// to be reviewed.
//
//
// ****************************************************************************

