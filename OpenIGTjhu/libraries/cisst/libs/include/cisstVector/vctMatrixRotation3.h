/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation3.h,v 1.40 2006/01/03 03:38:58 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2004-01-12

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
  \brief Declaration of vctMatrixRotation3
 */


#ifndef _vctMatrixRotation3_h
#define _vctMatrixRotation3_h


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctMatrixRotation3Base.h>

#include <cisstVector/vctExport.h>


/*!
  \brief Define a rotation matrix for a space of dimension 3

  \ingroup cisstVector

  This class is templated by the element type.  It is derived from
  vctFixedSizeMatrix and the main difference is that the size is set
  to 3 by 3.

  \param _elementType The type of elements of the matrix.

  \sa vctFixedSizeMatrix
*/
template <class _elementType>
class vctMatrixRotation3: public vctMatrixRotation3Base<vctFixedSizeMatrix<_elementType, 3, 3> >
{
 public:
    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {ROWS = 3, COLS = 3};
    enum {DIMENSION = 3};
    typedef vctFixedSizeMatrix<value_type, 3, 3> ContainerType;
    typedef vctMatrixRotation3Base<ContainerType> BaseType;
    typedef vctMatrixRotation3<_elementType> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<_elementType> TypeTraits;

    /*! Default constructor. Sets the rotation matrix to identity. */
    inline vctMatrixRotation3():
        BaseType(BaseType::Identity())
    {}

    inline vctMatrixRotation3(const ThisType & other):
        BaseType(other)
    {}

    inline vctMatrixRotation3(const BaseType & other):
        BaseType(other)
    {}

    /*! The assignment from BaseType (i.e. a 3 by 3 fixed size matrix)
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


    /*! Constructor from 9 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation3<double> matrix( 0.0, 1.0, 0.0,
                                        -1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0);
      \endcode
    */
    inline vctMatrixRotation3(const value_type & element00, const value_type & element01, const value_type & element02,
                              const value_type & element10, const value_type & element11, const value_type & element12,
                              const value_type & element20, const value_type & element21, const value_type & element22)
        throw(std::runtime_error)
    {
        From(element00, element01, element02,
             element10, element11, element12,
             element20, element21, element22);
    }



    /*!
      Constructor from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline vctMatrixRotation3(const vctFixedSizeConstVectorBase<3, __stride1, _elementType, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<3, __stride2, _elementType, __dataPtrType2>& v2,
                              const vctFixedSizeConstVectorBase<3, __stride3, _elementType, __dataPtrType3>& v3,
                              bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        From(v1, v2, v3, vectorsAreColumns);
    }

    /*!
      Constructor from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline vctMatrixRotation3(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
                              bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        From(v1, v2, v3, vectorsAreColumns);
    }

    /*! Construction from a vctAxisAngleRotation3. */
    inline vctMatrixRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
        throw(std::runtime_error)
    {
        From(axisAngleRotation);
    }


    /*! Constructor from a rotation quaternion. It is important to
      note that this constructor doesn't normalize the rotation
      quaternion but asserts that it is normalized (in debug mode
      only).  See also the method From(quaternion).

      \param quaternionRotation A unit quaternion 
    */
    template <class __containerType>
    inline vctMatrixRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        throw(std::runtime_error)
    {
        From(quaternionRotation);
    }


    /*! Constructor from a vctRodriguezRotation3. */
    template <class __containerType>
    inline vctMatrixRotation3(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation)
        throw(std::runtime_error)
    {
        From(rodriguezRotation);
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


    /*! Constructor from 9 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation3<double> matrix( 0.0, 1.0, 0.0,
                                        -1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0);
      \endcode
    */
    inline vctMatrixRotation3(const value_type & element00, const value_type & element01, const value_type & element02,
                              const value_type & element10, const value_type & element11, const value_type & element12,
                              const value_type & element20, const value_type & element21, const value_type & element22,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(element00, element01, element02,
                           element10, element11, element12,
                           element20, element21, element22);
        } else {
            FromRaw(element00, element01, element02,
                    element10, element11, element12,
                    element20, element21, element22);
        }
    }



    /*!
      Constructor from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline vctMatrixRotation3(const vctFixedSizeConstVectorBase<3, __stride1, _elementType, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<3, __stride2, _elementType, __dataPtrType2>& v2,
                              const vctFixedSizeConstVectorBase<3, __stride3, _elementType, __dataPtrType3>& v3,
                              bool vectorsAreColumns, bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(v1, v2, v3, vectorsAreColumns);
        } else {
            FromRaw(v1, v2, v3, vectorsAreColumns);
        }
    }

    /*!
      Constructor from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline vctMatrixRotation3(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
                              bool vectorsAreColumns, bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(v1, v2, v3, vectorsAreColumns);
        } else {
            FromRaw(v1, v2, v3, vectorsAreColumns);
        }
    }

    /*! Construction from a vctAxisAngleRotation3. */
    inline vctMatrixRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(axisAngleRotation);
        } else {
            FromRaw(axisAngleRotation);
        }
    }


    /*! Constructor from a rotation quaternion. It is important to
      note that this constructor doesn't normalize the rotation
      quaternion but asserts that it is normalized (in debug mode
      only).  See also the method From(quaternion).

      \param quaternionRotation A unit quaternion 
      \param normalizeInput Normalize the input or convert as is (#VCT_NORMALIZE or #VCT_DO_NOT_NORMALIZE)
    */
    template <class __containerType>
    inline vctMatrixRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(quaternionRotation);
        } else {
            FromRaw(quaternionRotation);
        }
    }


    /*! Constructor from a vctRodriguezRotation3. */
    template <class __containerType>
    inline vctMatrixRotation3(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(rodriguezRotation);
        } else {
            FromRaw(rodriguezRotation);
        }
    }
    //@}



    /*! Initialize this rotation matrix with a 3x3 matrix.  This constructor only takes a matrix of the
      same element type.
      \note This constructor does not verify normalization.  It is introduced to allow using results
      of matrix operations and assign them to a rotation matrix.
      \note The constructor is declared explicit, to force the user to be aware of the conversion
      being made.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    explicit inline
    vctMatrixRotation3(const vctFixedSizeMatrixBase<3, 3, __rowStride, __colStride, _elementType, __dataPtrType> & matrix):
        BaseType(matrix)
    {}




    /*!
      \name Deprecated constructors
    */
    //@{


    /*! Constructor from an axis and an angle.  It is important to
      note that this constructor doesn't normalize the axis vector but
      asserts that it is normalized (in debug mode only).  See also
      the method From(axis,angle).

      \param axis A unit vector of size 3.
      \param angle The angle in radians.

      \note This constructor is deprecated.  Use the constructor from
      vctAxisAngleRotation3 instead.
    */
    template<int __stride, class __dataPtrType>
    inline CISST_DEPRECATED vctMatrixRotation3(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & axis,
                                               const _elementType angle) {
        From(axis, angle);
    }

    /*!  Construct a matrix rotation from a rotation Rodriguez vector, whose
      magnitude is the rotation angle and direction is the axis.  See  also
      method From(rodriguez).

      \param rodriguezRotation A Rodriguez rotation (3-element vector)

      \note This constructor is deprecated.  Use constructor from
      vctRodriguezRotation3 instead.
    */
    template<int __stride, class __dataPtrType>
    inline CISST_DEPRECATED
    vctMatrixRotation3(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & rodriguezRotation) {
        From(rodriguezRotation);
    }

    //@}
};


#endif  // _vctMatrixRotation3_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctMatrixRotation3.h,v $
// Revision 1.40  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.39  2005/12/01 22:01:49  anton
// ivctMatrixRotation3.h: Indentation.
//
// Revision 1.38  2005/10/07 03:32:35  anton
// vctMatrixRotation3: Major update to implement ticket #110 re. initial
// conditions for conversion methods (i.e. normalization of input).
//
// Revision 1.37  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.36  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.35  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.34  2005/08/08 17:13:44  anton
// Minor Doxygen typos.
//
// Revision 1.33  2005/07/22 19:06:46  ofri
// vct[Quaternion|Matrix]Rotation[2|3]: Fixing compilation error related to
// check-in [1253].
//
// Revision 1.32  2005/07/22 15:08:21  ofri
// Answer for ticket #155: added operator * that returns ThisType to
// vctMatrixRotation2, vctMatrixRotation3, vctQuaternionRotation3
//
// Revision 1.31  2005/07/19 18:24:28  anton
// vctMatrixRotation{2,3}: Added constructor and From methods to set a rotation
// matrix from 2 (respectively 3) vectors.  See ticket #151.
//
// Revision 1.30  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.29  2005/05/23 15:20:42  ofri
// vctMatrixRotation3::IsNotmalized : Breakup of expression into statements,
// following ticket #145.  Successfully tested on linux/gcc.
//
// Revision 1.28  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.27  2005/05/13 18:23:51  anton
// cisstVector: Both vctMatrixRotation{2,3}::Normalized() where normalizing
// identity.  Result is now initialized from *this.
//
// Revision 1.26  2005/04/28 03:58:35  anton
// cisstVector: Added methods ApplyTo(input) returning by copy as specified
// in ticket #108.  Added Doxygen documentation and moved from EqualRotation()
// to AlmostEqual().
//
// Revision 1.25  2005/02/11 18:30:30  ofri
// vctMatrixRotation3: added constructor and From method from
// vctFixedSizeConstMatrixBase.  It was needed, for example, to assign output of
// rot*rot (which is a matrix) to rot.  Note that this version does not assert
// normalization.
//
// Revision 1.24  2005/02/07 02:54:02  anton
// vctMatrixRotation3: Minor Doxygen update.
//
// Revision 1.23  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
// Revision 1.22  2004/12/01 17:14:53  anton
// cisstVector:  Replace access to matrix elements by Element() instead of
// double [][] for increased performance.  See ticket #79.
//
// Revision 1.21  2004/11/18 20:57:30  ofri
// RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
// instead of const ThisType &.  This included corresponding revision of the
// Assign methods, and also definition from scratch of operator= for fixed and
// dynamic matrix ref.
//
// Revision 1.20  2004/11/08 18:09:07  anton
// cisstVector: Methods Assign(ThisType) require a cast.
//
// Revision 1.19  2004/11/04 23:11:02  ofri
// vctQuaternionRotation3 and vctMatrixRotation3: Added method EqualRotation
// and From(ThisType) with their documentation.
//
// Revision 1.18  2004/10/25 19:19:54  anton
// cisstVector:  Created vctForwardDeclarations.h and removed forward
// declarations of classes in all other files.  Added some constructors
// for vector references to reference fixed size from dynamic and vice versa.
//
// Revision 1.17  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.16  2004/10/22 18:07:06  anton
// Doxygen: Made sure that all the classes are documented for Doxygen.
//
// Revision 1.15  2004/10/06 22:53:03  ofri
// classes vctMatrixRotation3, vctQuaternionRotation, vctFrame: Added methods
// ApplyTo(vctDynamicConstVectorBase, vctDynamicVectorBase) and
// ApplyInverseTo(vctDynamicConstVectorBase, vctDynamicVectorBase) .  The library
// and the test were compiled successfully on .net7, gcc.  No correctess tests
// were performed.
//
// Revision 1.14  2004/08/13 19:27:55  anton
// cisstVector:  For all the code related to rotations, added some assert to
// check that the input is valid (normalized quaternion, normalized axis,
// normalized matrix) for all the From method.  Added a
// vctQuaternionRotation.FromRaw(matrix) since this method is used to normalize
// the rotation matrices.  It's input doesn't have to be normalized.  As a side
// effect these additions, found a couple of IsNormalized methods which were
// not const.
//
// Revision 1.13  2004/08/13 17:47:40  anton
// cisstVector: Massive renaming to replace the word "sequence" by "vector"
// (see ticket #50) as well as another more systematic naming for the engines.
// The changes for the API are as follow:
// *: vctFixedLengthSequenceBase -> vctFixedSizeVectorBase (and Const)
// *: vctFixedLengthSequenceRef -> vctFixedSizeVectorRef (and Const)
// *: vctDynamicSequenceBase -> vctDynamicVectorBase (and Const)
// *: vctDynamicSequenceRef -> vctDynamicVectorRef (and Const)
// *: vctDynamicSequenceRefOwner -> vctDynamicVectorRefOwner
// *: vctFixedStrideSequenceIterator -> vctFixedStrideVectorIterator (and Const)
// *: vctVarStrideSequenceIterator -> vctVarStrideVectorIterator (and Const)
// *: vctSequenceRecursiveEngines -> vctFixedSizeVectorRecursiveEngines
// *: vctSequenceLoopEngines -> vctDynamicVectorLoopEngines
// *: vctMatrixLoopEngines -> vctFixedSizeMatrixLoopEngines
// *: vctDynamicMatrixEngines -> vctDynamicMatrixLoopEngines
// Also updated and corrected the documentation (latex, doxygen, figures) as
// well as the tests and examples.
//
// Revision 1.12  2004/08/12 20:24:58  anton
// cisstVector: Removed useless #include <math.h> which is included by
// cmnPortability.  This allows a better control over the system includes.
//
// Revision 1.11  2004/06/03 21:18:39  ofri
// And a couple more typos.
//
// Revision 1.10  2004/06/03 20:47:13  ofri
// ... And, as usual, needed a couple adjustments after testing on Linux.
// cisstVectorTests were successful.
//
// Revision 1.9  2004/06/03 20:38:14  ofri
// Changes in vctQuaternionRotation3 and vctMatrixRotation3:
// 1) Added constructor and conversion from ``Jacobian vector''
// 2) Some cosmetic changes: documentation, easier debugging.
//
// Revision 1.8  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.7  2004/04/02 16:13:35  anton
// Removed CISST_EXPORT for the whole class, exports only the methods
// Identity, From(vctQuaternionRotation3) and NormalizedSelf.
//
// Revision 1.6  2004/03/19 15:29:41  ofri
// vctMatrixRotation3 initialization from elements now uses base-type constructor
//
// Revision 1.5  2004/03/16 21:18:44  ofri
// Applying changes deriving from updating the interface of cmnTypeTraits
//
// Revision 1.4  2004/02/27 02:41:31  ofri
// The Identity rotation and frame objects are now declared through a static
// method instead of a static object instance. This fixes a bug related to the
// order of static object instantiations.
//
// Revision 1.3  2004/02/18 22:30:42  anton
// Added DIMENSION for all transformation to simplify the vctFrame template
//
// Revision 1.2  2004/02/11 15:28:15  anton
// Added ApplyTo(ThisType), ApplyInverseTo(ThisType) and ApplyInverseTo(sequence)
//
// Revision 1.1  2004/02/06 15:40:20  anton
// Renamed vctRotationQuaternion3 to vctQuaternionRotation3
// Renamed vctRotationMatrix3 to vctMatrixRotation3
// Added Inverse and Normalized methods with consistant naming and signatures
// Added constructors from/to quaternion, matrix, axis/angle
//
//
// Revision 1.2  2004/01/16 22:51:16  anton
// Added Inverse methods, ApplyTo and = for assignement from matrix
//
// Revision 1.1  2004/01/15 15:21:50  anton
// Creation
//
//
// ****************************************************************************

