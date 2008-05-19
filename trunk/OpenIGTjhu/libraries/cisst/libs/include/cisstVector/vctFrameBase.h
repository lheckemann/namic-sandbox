/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFrameBase.h,v 1.12 2006/01/20 04:46:36 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2004-02-11

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
  \brief Declaration of vctFrameBase
 */


#ifndef _vctFrameBase_h
#define _vctFrameBase_h

#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFixedSizeMatrixBase.h>
#include <cisstVector/vctExport.h>

/*!
  \brief Template base class for a frame.

  The template parameter allows to change the orientation
  representation as well as the dimension.  The orientation (or
  rotation) type must define an enum DIMENSION which is used to
  determine the size of the vector representing the translation (see
  vctFixedSizeVector).

  This templating allows to create a frame based on a quaternion
  (dimension 3) or a matrix (dimension 2 or 3).

  \param _rotationType The type of rotation

  \sa vctQuaternionRotation3 vctMatrixRotation3 vctFixedSizeVector
*/
template<class _rotationType>
class vctFrameBase
{
public:
    enum {DIMENSION = _rotationType::DIMENSION};
    typedef typename _rotationType::value_type value_type;
    typedef vctFrameBase<_rotationType> ThisType;
    typedef _rotationType RotationType;
    typedef vctFixedSizeVector<value_type, DIMENSION> TranslationType;
    typedef cmnTypeTraits<value_type> TypeTraits;
    
protected:
    RotationType RotationMember;
    TranslationType TranslationMember;
    
public:


    /*! Default constructor. Sets the rotation matrix to identity. */
    vctFrameBase() {
        Assign(Identity());
    }
    
    /*! Constructor from a translation and a rotation. */
    template<int __stride, class __dataPtrType>
    vctFrameBase(const RotationType & rotation,
                 const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & translation):
        RotationMember(rotation),
        TranslationMember(translation)
    {}
    
    /*! Const reference to the identity.  In this case, the
        translation is set to <tt>(0, 0, 0)</tt> and the rotation is
        set to identity using its own method
        <tt>RotationType::Identity()</tt>. */
    static CISST_EXPORT const ThisType & Identity();
    
    
    inline ThisType & Assign(const ThisType & otherFrame) {
        RotationMember.Assign(otherFrame.Rotation());
        TranslationMember.Assign(otherFrame.Translation());
        return *this;
    }


    inline const TranslationType & Translation(void) const {
        return TranslationMember;
    }

    inline TranslationType & Translation(void) {
        return TranslationMember;
    }

    inline const RotationType & Rotation(void) const {
        return RotationMember;
    }

    inline RotationType & Rotation(void) {
        return RotationMember;
    }


    /*! Inverse this frame. */
    inline const ThisType & InverseSelf(void) {
        // R -> Rinv
        RotationMember.InverseSelf();
        // T -> Rinv * (-T)
        TranslationType temp = -TranslationMember;
        RotationMember.ApplyTo(temp, TranslationMember);
        return *this;
    }

    
    inline const ThisType & InverseOf(const ThisType & otherFrame) {
        TranslationMember = otherFrame.Translation();
        RotationMember = otherFrame.Rotation();
        InverseSelf();
        return *this;
    }

    inline ThisType Inverse(void) const {
        ThisType result;
        result.InverseOf(*this);
        return result;
    }


    /*! Apply the transformation to a vector of fixed size DIMENSION. The
      result is stored into a vector of size DIMENSION provided by the caller
      and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        RotationMember.ApplyTo(input, output);
        output.Add(TranslationMember);
    }


    /*! Apply the transformation to a vector of fixed size DIMENSION. The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Compose this transform over the input transform to obtain a
      new output transform passed by reference by the caller.  The
      semantics of the operation are:
      
      output = (*this) * input

      if (*this) is [R1 | p1], input is [R2 | p2], then output will be
      [R1*R2 | R1*p2 + p1]
    */
    inline void ApplyTo(const ThisType & input, ThisType & output) const {
        TranslationType temp;
        RotationMember.ApplyTo(input.Translation(), temp);
        (output.Rotation()).ProductOf(RotationMember, input.Rotation());
        (output.Translation()).SumOf(temp, TranslationMember);
    }


    /*! Apply the transformation to another transformation.  The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input transformation
      \return The output transformation
    */
    inline ThisType ApplyTo(const ThisType & input) const {
        ThisType result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the transformation to a dynamic vector.  The result is
      stored into another dynamic vector.  It is assumed that both are
      of size DIMENSION.
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        RotationMember.ApplyTo(input, output);
        output[0] += TranslationMember[0];
        output[1] += TranslationMember[1];
        output[2] += TranslationMember[2];
    }


    /*! Apply this transform to a matrix of three rows */
    template<unsigned int __cols, int __rowStride1, int __colStride1, class __dataPtrType1, 
             int __rowStride2, int __colStride2, class __dataPtrType2>
    inline void ApplyTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
                        vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        typedef vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> inputType;
        typedef typename inputType::ConstColumnRefType inputColumnType;
        typedef vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> outputType;
        typedef typename outputType::ColumnRefType outputColumnType;
        unsigned int columnIndex;
        for (columnIndex = 0; columnIndex < __cols; ++columnIndex) {
            const inputColumnType inputColumn(input.Column(columnIndex));
            outputColumnType outputColumn(output.Column(columnIndex));
            this->ApplyTo(inputColumn, outputColumn);
        }
    }


    /*! Apply this transform to an array of DIMENSION-vectors given as a
      pointer and a size.  This method can be generalized to be
      templated with different vector types, defined by strides and
      dataPtrType.  For simplicity, this version is writtend for a
      DIMENSION-vector object.
    */
    inline void ApplyTo(unsigned int inputSize, const vctFixedSizeVector<value_type, DIMENSION> *input,
                        vctFixedSizeVector<value_type, DIMENSION> * output) const
    {
        unsigned int index;
        for (index = 0; index < inputSize; ++index) {
            this->ApplyTo(input[index], output[index]);
        }
    }


    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        TranslationType temp;
        temp.DifferenceOf(input, TranslationMember);
        RotationMember.ApplyInverseTo(temp, output);
    }

    /*! Apply the inverse of the transformation to a vector of fixed
      size DIMENSION. The result is stored into a vector of size DIMENSION provided by
      the caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const {
        const TranslationType temp(input[0] - TranslationMember[0], input[1] - TranslationMember[1], input[2] - TranslationMember[2]);
        TranslationType result;
        RotationMember.ApplyInverseTo(temp, result);
        output[0] = result[0];
        output[1] = result[1];
        output[2] = result[2];
    }


    /*! Apply the the inverse of the transformation to a vector of
      fixed size DIMENSION. The result is returned by copy.  This interface
      might be more convenient for some but one should note that it is
      less efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    inline void ApplyInverseTo(const ThisType & input, ThisType & output) const {  
        ThisType inverse;
        inverse.InverseOf(*this);
        inverse.ApplyTo(input, output);
    }


    inline ThisType ApplyInverseTo(const ThisType & input) const {
        ThisType result;
        this->ApplyInverseTo(input, result);
        return result;
    }

    /*! Return true if this transformation is exactly equal to the
      other transformation.  The result is based on the Equal()
      methods provided by the different rotation representations
      (vctQuaternionRotation3, vctMatrixRotation3, ...) and the
      translation.
      
      \sa AlmostEqual
    */
    //@{
    inline bool Equal(const ThisType & other) const {
        return (RotationMember.Equal(other.Rotation())
                && TranslationMember.Equal(other.Translation()));
    }

    inline bool operator==(const ThisType & other) const {
        return this->Equal(other);
    }
    //@}


    /*! Return true if this transformation is equal to the other
      transformation, up to the given tolerance.  The result is based
      on the AllowsEqual() methods provided by the different rotation
      representations (vctQuaternionRotation3, vctMatrixRotation3,
      ...) and the translation.
      
      The tolerance factor is used to compare both the translations
      and rotations.
      
      \sa AlmostEquivalent
    */
    inline bool AlmostEqual(const ThisType & other,
                            value_type tolerance = TypeTraits::Tolerance()) const {
        return (RotationMember.AlmostEqual(other.Rotation(), tolerance)
                && TranslationMember.AlmostEqual(other.Translation(), tolerance));
    }

    
    /*! Return true if this transformation is equivalent to the other
      transformation, up to the given tolerance.  The result is based
      on the AlmostEquivalent() methods provided by the different
      rotation representations (vctQuaternionRotation3,
      vctMatrixRotation3, ...) and AlmostEqual for the translation.

      The tolerance factor is used to compare both the translations
      and rotations.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        return (RotationMember.AlmostEquivalent(other.Rotation(), tolerance)
                && TranslationMember.AlmostEqual(other.Translation(), tolerance));
    }


    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    
    void ToStream(std::ostream & outputStream) const {
        outputStream << "translation: "
                     << std::endl
                     << this->Translation()
                     << std::endl
                     << "rotation: "
                     << std::endl
                     << this->Rotation();
    }

};


template <class _elementType, unsigned int _size, class _rotationType,
          int _stride, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator * (const vctFrameBase<_rotationType> & frame,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector) {
    vctFixedSizeVector<_elementType, _size> result;
    frame.ApplyTo(vector, result);
    return result;
}


template <class _rotationType>
inline vctFrameBase<_rotationType>
operator * (const vctFrameBase<_rotationType> & frame1,
            const vctFrameBase<_rotationType> & frame2) {
    vctFrameBase<_rotationType> result;
    frame1.ApplyTo(frame2, result);
    return result;
}

template<unsigned int _cols, int _rowStride, int _colStride, class _rotationType, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, 3, _cols >
operator *(const vctFrameBase<_rotationType> & frame,
           const vctFixedSizeConstMatrixBase<3, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<typename _rotationType::value_type, 3, _cols > result;
    frame.ApplyTo(matrix, result);
    return result;
}

/*! Stream out operator. */
template<class _rotationType>
std::ostream & operator << (std::ostream & output,
                            const vctFrameBase<_rotationType> & frame) {
    frame.ToStream(output);
    return output;
}

#endif  // _vctFrameBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFrameBase.h,v $
// Revision 1.12  2006/01/20 04:46:36  anton
// cisstVector: Added methods AlmostEquivalent for all transformations as well
// as some missing Equal (and ==).  See ticket #204.
//
// Revision 1.11  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.10  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.9  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2005/04/28 03:58:35  anton
// cisstVector: Added methods ApplyTo(input) returning by copy as specified
// in ticket #108.  Added Doxygen documentation and moved from EqualRotation()
// to AlmostEqual().
//
// Revision 1.7  2005/02/03 19:09:01  anton
// vctFrameBase.h: Added method ToString() and ToStream(), updated << operator
//
// Revision 1.6  2005/01/11 20:19:02  anton
// cisstVector frames:  Removed useless method and uniform API to get and set
// both rotation and translation (see tickets #104 and #115).
//
// Revision 1.5  2004/11/18 20:57:30  ofri
// RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
// instead of const ThisType &.  This included corresponding revision of the
// Assign methods, and also definition from scratch of operator= for fixed and
// dynamic matrix ref.
//
// Revision 1.4  2004/10/31 22:44:02  ofri
// Fixing bug in vctFrameBase::ApplyTo(const ThisType &, ThisType &). See
// ticket #86.
//
// Revision 1.3  2004/10/31 22:05:57  ofri
// Fixing bug in vctFrameBase::InverseSelf.  See ticket #85
//
// Revision 1.2  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.1  2004/10/21 20:14:47  anton
// cisstVector: Added missing file in check-in [829]
//
// Revision 1.13  2004/10/14 19:34:52  anton
// cisstVector: For the frames, constructors are now based on (rotation,
// translation) instead of (translation, rotation) as requested in #72.
//
// Revision 1.12  2004/10/06 22:53:03  ofri
// classes vctMatrixRotation3, vctQuaternionRotation, vctFrame: Added methods
// ApplyTo(vctDynamicConstVectorBase, vctDynamicVectorBase) and
// ApplyInverseTo(vctDynamicConstVectorBase, vctDynamicVectorBase) .  The library
// and the test were compiled successfully on .net7, gcc.  No correctess tests
// were performed.
//
// Revision 1.11  2004/08/13 19:27:55  anton
// cisstVector:  For all the code related to rotations, added some assert to
// check that the input is valid (normalized quaternion, normalized axis,
// normalized matrix) for all the From method.  Added a
// vctQuaternionRotation.FromRaw(matrix) since this method is used to normalize
// the rotation matrices.  It's input doesn't have to be normalized.  As a side
// effect these additions, found a couple of IsNormalized methods which were
// not const.
//
// Revision 1.10  2004/08/13 17:47:40  anton
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
// Revision 1.9  2004/08/12 20:24:58  anton
// cisstVector: Removed useless #include <math.h> which is included by
// cmnPortability.  This allows a better control over the system includes.
//
// Revision 1.8  2004/07/12 20:25:48  anton
// cisstVector: Removed redundant #include statements.  These didn't look that
// great on the nice Doxygen graphs.
//
// Revision 1.7  2004/05/20 21:30:33  ofri
// Added version of vctFrame::ApplyTo that takes a regular C-array (pointer)
// of 3-vectors.  Useful for transforming a mesh.
//
// Revision 1.6  2004/04/02 16:17:11  anton
// Removed CISST_EXPORT for the whole class, method Identity() is the only one
// exported (instantiated in vctFrame.cpp)
//
// Revision 1.5  2004/03/30 21:41:35  ofri
// Fixed error in vctFrame.ApplyTo(matrix<3,n>)   :
// Instantiating column references as named object to avoid passing a const
// argument in the internal ApplyTo(vector)
//
// Revision 1.4  2004/03/16 21:14:19  ofri
// Added operations and methods to apply a frame to a 3xm fixed-size matrix
//
// Revision 1.3  2004/02/27 02:41:31  ofri
// The Identity rotation and frame objects are now declared through a static
// method instead of a static object instance. This fixes a bug related to the
// order of static object instantiations.
//
// Revision 1.2  2004/02/18 22:30:42  anton
// Added DIMENSION for all transformation to simplify the vctFrame template
//
// Revision 1.1  2004/02/18 15:55:29  anton
// Incomplete version
//
//
// ****************************************************************************

