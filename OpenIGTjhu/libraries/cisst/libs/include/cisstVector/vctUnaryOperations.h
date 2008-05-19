/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctUnaryOperations.h,v 1.14 2005/12/29 23:39:26 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on:  8/18/2003

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
  \brief Declaration of vctUnaryOperations.
 */


#ifndef _vctUnaryOperations_h
#define _vctUnaryOperations_h


/*!
  \brief Define unary operations on an object as classes.

  Class vctUnaryOperations is an envelope that wraps unary operations
  on an object as classes.  vctUnaryOperations defines internal
  classes such as Identity, AbsValue, etc., and each of the internal
  classes has one static function named Operate(), which wraps the
  corresponding operation.  The signature of a typical Operate() is

  <PRE> static inline OutputType Operate(const InputType & input)
  </PRE>

  Where OutputType, InputType, are the types of the result and
  the operand.

  By abstracting very simple operations as inline functions, we can
  plug the vctUnaryOperations internal classes into templated vector
  operation engines.  See an example of such engine in
  vctFixedSizeVectorRecursiveEngines.h , and an example of plugging an operation into
  the engine in vctFixedSizeVectorOperations.h .

  \param inputElementType the type of the first (left-side) operand
  \param outputElementType the type of the result

  \sa Identity AbsValue Square Negation
*/
template<class _outputElementType, class _inputElementType = _outputElementType>
class vctUnaryOperations
{
    public:
    typedef _outputElementType OutputElementType;
    typedef _inputElementType InputElementType;
    
    /*!
      \brief Returns the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class Identity {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input);
        }
    };
    
    /*!
      \brief Returns the absolute value of the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class AbsValue {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return (input > InputElementType(0)) ? OutputElementType(input) : OutputElementType(-input);
        }
    };

    /*! \brief Returns the floor of the input, that is, the largest integer less-than
      or equal to the input, as an OutputType object.
      \sa vctUnaryOperations
     */
    class Floor {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType( floor((double)input) );
        }
    };

    /*! \brief Returns the ceiling of the input, that is, the smallest integer greater-than
      or equal to the input, as an OutputType object.
      \sa vctUnaryOperations
     */
    class Ceil {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType( ceil((double)input) );
        }
    };
    

    
    /*!
      \brief Returns the square of the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class Square {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input * input);
        }
    };
    
    /*!
      \brief Returns the negative of the input as an OutputType object.
      \sa vctUnaryOperations
     */
    class Negation {
    public:
        /*!
          Execute the operation.
          \param input Input.
        */
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(-input);
        }
    };

    /*! Return true if the input element is strictly positive, false otherwise */
    class IsPositive {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input > InputElementType(0));
        }
    };

    /*! Return true if the input element is non-negative, false otherwise */
    class IsNonNegative {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input >= InputElementType(0));
        }
    };
    
    /*! Return true if the input element is non-positive, false otherwise */
    class IsNonPositive {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input <= InputElementType(0));
        }
    };
    
    /*! Return true if the input element is strictly negative, false otherwise */
    class IsNegative {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input < InputElementType(0));
        }
    };

    /*! Return true if the input element is nonzero */
    class IsNonzero {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(input != InputElementType(0));
        }
    };

    /*    template<unsigned int EXPONENT>
    class Power {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            OutputElementType rootOfPower = Power<EXPONENT / 2>::OperationType(input);
            return ((EXPONENT % 2 == 0) 
                    ? (rootOfPower * rootOfPower) 
                    : (rootOfPower * rootOfPower * input));
        }
    };

    class Power<0> {
    public:
        static inline OutputElementType Operate(const InputElementType & input) {
            return OutputElementType(1);
        }
    }; 
    */ 
};


#endif  // _vctUnaryOperations_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctUnaryOperations.h,v $
// Revision 1.14  2005/12/29 23:39:26  anton
// cisstVector: Cast values to double before calling floor or ceil (for .net 2003)
//
// Revision 1.13  2005/12/12 23:49:21  ofri
// vct[StoreBack]UnaryOperations : Added operations for [Make]Floor and
// [Make]Ceil to be used in the Floor and Ceil operations on vectors and matrices
//
// Revision 1.12  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.11  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.10  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.9  2004/08/13 17:47:40  anton
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
// Revision 1.8  2004/07/14 16:20:26  ofri
// Added class vctUnaryOperations::IsNonzero and methods
// vctFixedLengthConstSequenceBase::All, vctFixedLengthConstSequenceBase::Any
//
// Revision 1.7  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.6  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.5  2003/12/17 17:07:54  ofri
// Sign determining operations now return OutputElementType instead of
// bool
//
// Revision 1.4  2003/12/03 15:50:29  ofri
// Added sign determination operations: IsPositive, IsNonNegative,
// IsNonPositive, IsNegative.
//
// Revision 1.3  2003/10/03 19:09:53  anton
// cleaned doxygen documentation
//
// Revision 1.2  2003/09/17 17:41:56  anton
// added an _ to prefix all the template arguments
//
// Revision 1.1  2003/09/09 18:51:46  anton
// creation
//
//
// ****************************************************************************

