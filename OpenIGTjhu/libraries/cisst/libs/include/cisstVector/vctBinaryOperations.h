/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctBinaryOperations.h,v 1.16 2005/09/26 15:41:47 anton Exp $
  
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
  \brief Declaration of vctBinaryOperations.
 */

#ifndef _vctBinaryOperations_h
#define _vctBinaryOperations_h

#include <cisstCommon/cmnTypeTraits.h>

/*!
  \brief Define binary operations between objects as classes.

  Class vctBinaryOperations is an envelope that wraps binary
  operations between objects as classes.  BinaryOperations defines
  internal classes such as Addition, Subtraction, etc., and each of
  the internal classes has one static function named Operate(), which
  wraps the corresponding operation.  The signature of a typical
  Operate() is

  <PRE>
  static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2)
  </PRE>

  Where OutputType, Input1Type, Input2Type are the types of the result
  and the two operands.

  By abstracting very simple operations as inline functions, we can
  plug the BinaryOperations internal classes into templated vector
  operation engines.  See an example of such engine in
  vctFixedSizeVectorRecursiveEngines.h , and an example of plugging an operation into
  the engine in vctFixedSizeVectorOperations.h .

  \param outputType the type of the result
  \param input1Type the type of the first (left-side) operand
  \param input2Type the type of the second (right-side) operand)

  \sa Addition Subtraction Multiplication Division FirstOperand SecondOperand Maximum Minimum
*/
template<class _outputType, class _input1Type = _outputType, class _input2Type = _outputType>
class vctBinaryOperations {
    public:
    typedef _outputType OutputType;
    typedef _input1Type Input1Type;
    typedef _input2Type Input2Type;
    
    /*!
      \brief Returns the sum of the two InputType object.
      \sa vctBinaryOperations
    */
    class Addition {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1+input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(0);
        }
    };
    
    /*!
      \brief Returns the difference of the two InputType object.
      \sa vctBinaryOperations
    */
    class Subtraction {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1-input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(0);
        }
    };
    
    /*!
      \brief Returns the product of the two InputType object.
      \sa vctBinaryOperations
    */
    class Multiplication {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1*input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(1);
        }
    };
    
    /*!
      \brief Returns the ratio of the two InputType object.
      \sa vctBinaryOperations
    */
    class Division {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */ 
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 / input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(1);
        }
    };
    
    
    /*!
      \brief Return the first operand of (input1, input2), i.e., input1.
      \sa vctBinaryOperations
    */
    class FirstOperand {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1);
        }
    };
    
    /*!
      \brief Return the second operand of (input1, input2), i.e., input2.
      \sa vctBinaryOperations
      Usage example is in recursive assignment.  See the function
      vctFixedSizeVectorOperations::SetAll
    */
    class SecondOperand {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input2);
        }
    };
    
    /*!
      \brief Return the greater of (input1, input2).
      \sa vctBinaryOperations
      Return the greater of (input1, input2).  Note that here all
      arguments are of the same type, because they have to be
      compared.
    */
    class Maximum {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline const OutputType & Operate(const OutputType & input1, const OutputType & input2) {
            return (input1 > input2) ? input1 : input2;
        }

        static inline OutputType NeutralElement()
        {
            return cmnTypeTraits<OutputType>::MinNegativeValue();
        }
    };
    
    /*!
      \brief Return the lesser of (input1, input2).
      \sa vctBinaryOperations
      Return the lesser of (input1, input2).  Note that here all
      arguments are of the same type, because they have to be
      compared.
    */
    class Minimum {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline const OutputType & Operate(const OutputType & input1, const OutputType & input2) {
            return (input1 < input2) ? input1 : input2;
        }

        static inline OutputType NeutralElement()
        {
            return cmnTypeTraits<OutputType>::MaxPositiveValue();
        }
    };
 
    /*!
      \brief Test for equality between input1 and input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is equal to input2.
      Note that input1 and input2 should be of the same type.
    */
    class Equal {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 == input2);
        }
    };

    /*!
      \brief Test for non equality between input1 and input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is not equal to input2.
      Note that input1 and input2 should be of the same type.
    */
    class NotEqual {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 != input2);
        }
    };

    /*!
      \brief Test if input1 is lesser than input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is lesser than input2.
      Note that input1 and input2 should be of the same type.
    */
    class Lesser {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 < input2);
        }
    };

    /*!
      \brief Test if input1 is lesser than or equal to input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is lesser than or
      equal to input2.  Note that input1 and input2 should be of the
      same type.
    */
    class LesserOrEqual {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 <= input2);
        }
    };

    /*!
      \brief Test if input1 is greater than input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is greater than input2.
      Note that input1 and input2 should be of the same type.
    */
    class Greater {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 > input2);
        }
    };

    /*!
      \brief Test if input1 is greater than or equal to input2
      \sa vctBinaryOperations

      Returns a boolean, i.e. true if the input1 is greater than or
      equal to input2.  Note that input1 and input2 should be of the
      same type.
    */
    class GreaterOrEqual {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 >= input2);
        }
    };

    /*!
      \brief Logical And operator between input1 and input2
      \sa vctBinaryOperations

      Note that here all arguments are of the same type.
    */
    class And {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType(input1 && input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(true);
        }
    };

    /*!
      \brief Logical Or operator between input1 and input2
      \sa vctBinaryOperations

      Note that here all arguments are of the same type.
    */
    class Or {
    public:
        typedef _outputType OutputType;
        /*!
          Execute the operation.
          \param input1 First operand.
          \param input2 Second operand.
        */
        static inline OutputType Operate(const OutputType & input1, const OutputType & input2) {
            return OutputType(input1 || input2);
        }

        static inline OutputType NeutralElement()
        {
            return OutputType(false);
        }
    };

    /*!
      \brief Test if the first argument is bound by the second argument.
      \sa vctBinaryOperations
      
      Tests if if abs(input1) <= input2.  The test does not use the
      abs function, but tests if (-input2 <= input1) && (input1 <=
      input2).  Note that if input2 is negative, the result is always
      false.
    */
    class Bound {
    public:
        typedef _outputType OutputType;
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return OutputType( (-input2 <= input1) && (input1 <= input2) );
        }
    };


    /*!
      \brief Dot product.
      \sa vctBinaryOperations

      This method is basically a wrapper around the method
      input1.DotProduct(input2).  It is used for the multiplication of
      two matrices (see vctFixedSizeMatrix).
    */
    class DotProduct {
    public:
        typedef _outputType OutputType;
        static inline OutputType Operate(const Input1Type & input1, const Input2Type & input2) {
            return input1.DotProduct(input2);
        }
    };


};


#endif  // _vctBinaryOperations_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctBinaryOperations.h,v $
// Revision 1.16  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.15  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.14  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.13  2004/10/14 21:34:33  ofri
// Following discussion of ticket #71, I am added a NeutralElement() method to
// some of the operations in vctBinaryOperations.  This should simplify the
// transition to base-case 0 in the recursive engines, if it is decided upon.
//
// Revision 1.12  2004/08/13 17:47:40  anton
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
// Revision 1.11  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.10  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.9  2003/12/19 21:44:12  anton
// Added DotProduct operation
//
// Revision 1.8  2003/12/18 16:13:30  anton
// Changed the order of the template parameters so that the output is first (consistant with the rest of code)
//
// Revision 1.7  2003/12/17 17:05:19  ofri
// 1) Removed the definition of OutputType from the main class
// 2) Removed `const' qualifier from return type of operations that return
// value (generates warning in icc)
// 3) Provided explicit construction of OutputType object in some operations
//
// Revision 1.6  2003/12/16 16:35:39  anton
// Added typedef OutputType for each operation so that the type can be retrieved from the recursive engines
//
// Revision 1.5  2003/12/03 15:51:33  ofri
// Added the operation Bound to check for boundedness of an absolute value
//
// Revision 1.4  2003/11/13 19:13:59  anton
// Added Equal, NotEqual, Lesser(OrEqual), Greater(OrEqual), And and Or
//
// Revision 1.3  2003/10/03 19:10:05  anton
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

