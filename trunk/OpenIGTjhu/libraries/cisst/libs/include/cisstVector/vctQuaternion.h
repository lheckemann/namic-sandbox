/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctQuaternion.h,v 1.23 2005/10/06 16:56:37 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2003-10-07

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
  \brief Declaration of vctQuaternion
 */


#ifndef _vctQuaternion_h
#define _vctQuaternion_h


#include <cisstVector/vctQuaternionBase.h>
#include <cisstVector/vctFixedSizeVector.h>


/*!
  \brief Define a quaternion container.

  \ingroup cisstVector

  This class is templated by the element type.  It is derived from
  vctFixedSizeVector and the main difference is that the size is set
  to 4.  The first three elements are the imaginary vector and the
  fourth is the real part of the quaternion.

  It is important to note that the class vctQuaternion is for
  any quaternion, i.e. it does not necessary represent a unit
  quaternion.

  \param _elementType The type of elements of the vector.

  \sa vctFixedSizeVector
*/
template <class _elementType>
class vctQuaternion : public vctQuaternionBase<vctFixedSizeVector<_elementType, 4> >
{
public:
    /* no need to document, inherit doxygen documentation from base class */
    enum {SIZE = 4};
    typedef vctFixedSizeVectorTraits<_elementType, SIZE, 1> VectorTraits;
    typedef typename VectorTraits::value_type value_type;
    typedef vctQuaternion<value_type> ThisType;
    typedef vctFixedSizeVector<value_type, 4> ContainerType;
    typedef vctQuaternionBase<ContainerType> BaseType;
    typedef cmnTypeTraits<value_type> TypeTraits;
    
    /*! Default constructor.  Does nothing. */
    inline vctQuaternion():
        BaseType()
    {}

    /*! Constructor from 4 elements.

      \param x The first imaginary component
      \param y The second imaginary component
      \param z The third imaginary component
      \param r The real component
    */
    inline vctQuaternion(const value_type & x,
                         const value_type & y,
                         const value_type & z,
                         const value_type & r)
    {
        this->X() = x;
        this->Y() = y;
        this->Z() = z;
        this->R() = r;
    }

};


#endif  // _vctQuaternion_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctQuaternion.h,v $
// Revision 1.23  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.22  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.21  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.20  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.19  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.18  2005/01/11 22:42:18  anton
// cisstVector:  Added normalization methods to the vectors.  Removed useless
// code from vctQuaternion and added some tests.  See ticket #110.
//
// Revision 1.17  2004/12/20 20:47:53  ofri
// vctQuaternion: I changed the name of the method Multiply to PostMultiply, and
// I added the method PreMultiply, because quaternion product is non-commutative.
// I added the methods QuotientOf, Divide(quaterion), and Divide(scalar).  Also
// Added the / operator for dividing quaternions.
//
// Revision 1.16  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.15  2004/10/21 18:39:49  anton
// vctQuaternion: Redefines R() and use only X(), Y() and Z() to access the
// data members of the quaternions (see ticket #74).
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
// Revision 1.12  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.11  2004/04/02 15:29:50  anton
// Removed CISST_EXPORT, the class vctQuaternion is purely templated.
//
// Revision 1.10  2004/03/19 15:31:03  ofri
// Eliminated direct access of the Data container.  Use at() instead.
//
// Revision 1.9  2004/03/16 21:18:44  ofri
// Applying changes deriving from updating the interface of cmnTypeTraits
//
// Revision 1.8  2004/02/18 15:52:44  anton
// In method Multiply(ThisType), code used this instead of *this
//
// Revision 1.7  2004/02/16 19:41:41  anton
// For conjugate, negate the 3 imaginary components instead of the imaginary one
//
// Revision 1.6  2004/02/11 18:39:48  anton
// From(vecIm, scalReal) replaced by ConcatenationOf from vctFixedLengthSequenceBase
//
// Revision 1.5  2004/02/11 15:27:24  anton
// Methods Conjugated() and Normalized() are const
//
// Revision 1.4  2004/02/06 15:31:07  anton
// Used ThisType whenever possible, not vctquaternion<_elementType>
// Renamed methods MakeXxxx to XxxxSelf (Conjugate)
// Added NormalizedSelf, NormalizedOf, IsNormalized methods
//
// Revision 1.3  2004/01/16 22:53:45  anton
// Added constructor from a vector and a value
//
// Revision 1.2  2004/01/15 15:22:21  anton
// Minor documentation updates
//
// Revision 1.1  2004/01/12 18:54:21  anton
// Creation
//
//
// ****************************************************************************

