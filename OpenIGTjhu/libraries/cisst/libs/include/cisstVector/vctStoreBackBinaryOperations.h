/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctStoreBackBinaryOperations.h,v 1.10 2005/09/26 15:41:47 anton Exp $

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  2003-08-18

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
  \brief Declaration of vctStoreBackBinaryOperations
 */


#ifndef _vctStoreBackBinaryOperations_h
#define _vctStoreBackBinaryOperations_h


/*!
  \brief Define binary operations between objects as classes.

  Class vctStoreBackBinaryOperations is an envelope that wraps binary
  store back operations between objects as classes.
  StoreBackOperations defines internal classes such as Addition,
  Subtraction, etc., and each of the internal classes has one static
  function named Operate(), which wraps the corresponding operation.
  The signature of a typical Operate() is

  <PRE>
  static inline OutputType & Operate(Input1Type & input1, const Input2Type & input2)
  </PRE>

  Where OutputType, Input1Type, Input2Type are the types of the result
  and the two operands.

  By abstracting very simple operations as inline functions, we can
  plug the StoreBackOperations internal classes into templated vector
  operation engines.  See an example of such engine in
  vctFixedSizeVectorRecursiveEngines.h , and an example of plugging an
  operation into the engine in vctFixedSizeVectorOperations.h .

  \param inputOutputType the type of the first (left-side) operand, which is an input output
  \param input2Type the type of the second (right-side) operand)

  \note This class contains the binary operation Swap, whose operation signature
  is
  <PRE>
  static inline void Operate(Input1Type & input1, Input2Type & input2)
  </PRE>
  with no const or return type.  It was included here since we didn't want
  to have a special file for just one operation.

  \sa Addition Subtraction Multiplication Division FirstOperand SecondOperand Maximum Minimum
*/
template<class _inputOutputType, class _input2Type = _inputOutputType>
class vctStoreBackBinaryOperations {
    public:
    typedef _inputOutputType InputOutputType;
    typedef _input2Type Input2Type;

    /*!
      \brief Returns the sum of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */
    class Addition {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            return (input1 += input2);
        }
    };
    
    
    /*!
      \brief Returns the difference of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */
    class Subtraction {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            return (input1 -= input2);
        }
    };
    

    /*!
      \brief Returns the product of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */
    class Multiplication {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            return (input1 *= input2);
        }
    };
    

    /*!
      \brief Returns the ratio of the two InputType object.
      \sa vctStoreBackBinaryOperations
    */    
    class Division {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            return (input1 /= input2);
        }
    };
    
    
    /*!
      \brief Return the first operand of (input1, input2), i.e., input1.
      \sa vctStoreBackBinaryOperations
    */
    class FirstOperand {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            return (input1);
        }
    };
    
    
    /*!
      \brief Return the first operand of (input1, input2), i.e., input2.
      \sa vctStoreBackBinaryOperations
      Usage example is in recursive assignment.  See the function FixedSizeVectorOperations::SetAll
    */
    class SecondOperand {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline InputOutputType & Operate(InputOutputType & input1, const Input2Type & input2) {
            return (input1 = input2);
        }
    };


    /*!
      \brief Return the greater of (input1, input2).
      \sa vctStoreBackBinaryOperations
      Return the greater of (input1, input2).  Note that here all arguments are of the
      same type, because they have to be compared.
    */
    class Maximum {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline const InputOutputType & Operate(InputOutputType & input1,
                                                      const InputOutputType & input2) {
            return (input1 = (input1 > input2) ? input1 : input2);
        }
    };


    /*!
      \brief Return the of lesser (input1, input2).
      \sa vctStoreBackBinaryOperations
      Return the lesser of (input1, input2).  Note that here all arguments are of the
      same type, because they have to be compared.
    */
    class Minimum {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand.
        */
        static inline const InputOutputType & Operate(InputOutputType & input1,
                                                      const InputOutputType & input2) {
            return (input1 = (input1 < input2) ? input1 : input2);
        }
    };

    /*!
      \brief Swap the values of input1, input2.
      \sa vctStoreBackBinaryOperations
      Swap the values of input1 and input2.  The operation has no return value.
      The operands have to be of the same type.
    */
    class Swap {
    public:
        /*!
          Execute the operation.
          \param input1 First operand (input and output).
          \param input2 Second operand (input and output).
        */
        static inline void Operate(InputOutputType & input1,
                                   InputOutputType & input2) {
            const InputOutputType tmp(input1);
            input1 = input2;
            input2 = tmp;
        }
    };

};


#endif  // _vctStoreBackBinaryOperations_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctStoreBackBinaryOperations.h,v $
// Revision 1.10  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.9  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2004/12/07 04:12:26  ofri
// Following ticket #99:
// 1) Added engine VioVio in vctFixedSizeVectorRecursiveEngines and in
// vctDynamicVectorLoopEngines
// 2) Added operation Swap in vctStoreBackBinaryOperations
// 3) Added operation SwapElementsWith in vctFixedSizeVectorBase and
// vctDynamicVectorBase
// 4) Rewrote the methods EchangeRows and ExchangeColumns in
// vctFixedSizeMatrixBase; added them to vctDynamicMatrixBase.
// 5) Same for row/column permutation methods.
// I am committing the files after compiling the library successfully and running
// the tests successfully on Cygwin.
//
// Revision 1.7  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.6  2004/08/13 17:47:40  anton
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
// Revision 1.5  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.4  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.3  2003/10/03 19:10:46  anton
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

