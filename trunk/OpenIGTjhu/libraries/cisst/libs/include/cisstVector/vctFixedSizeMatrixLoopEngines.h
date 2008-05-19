/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrixLoopEngines.h,v 1.14 2006/06/09 21:53:29 anton Exp $

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  2003-12-16

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


#ifndef _vctFixedSizeMatrixLoopEngines_h
#define _vctFixedSizeMatrixLoopEngines_h

/*!
  \file 
  \brief Declaration of vctFixedSizeMatrixLoopEngines
 */

// This macro is added as an option to select the implementation of
// loop engines, e.g., for the purpose of speed comparison.
#ifndef INCREMENTAL_LOOP_ENGINES
#define INCREMENTAL_LOOP_ENGINES 1
#else
#define EXTERN_INCREMENTAL_DEF
#endif

/*!
  \brief Container class for the matrix engines.
  
  \sa MoMiMi MioMi MoMiSi MoSiMi MioSi MoMi Mio SoMi SoMiMi
*/
class vctFixedSizeMatrixLoopEngines {

 public:    
    /*! Perform elementwise operation between matrices of identical
      size and element type.  The operation semantics is
      \code
      output[row][column] = op(input1[row][column], input2[row][column]);
      \endcode

      This is a similar semantics to the vctFixedSizeVectorRecursiveEngines::VoViVi
      engine.  However, for matrice we implement the operation using
      a loop instead of recursion.

      The engine is declared as a protected class.  This enables us to
      use softer declaration of the template arguments, as opposed to
      the functions calling this engine, which have to declare
      their arguments as matrices templated over stride etc.

      To use the engine, assume that Mout, Mi1, and Mi2 are matrix 
      types; that mout, mi1, and mi2 are objects of corresponding
      types; and that OP is a valid operation in the form
      \f[ value\_type \times value\_type \rightarrow value\_type \f]
      Then write
      \code
      MoMiMi<OP>::Run(mout, mi1, mi2);
      \endcode

      \param _elementOperationType a binary operation class. 
      _elementOperationType must have a static method with the
      signature
      \code
      _elementOperationType::Operate(_elementType, _elementType)
      \endcode
     */
    template<class _elementOperationType>
    class MoMiMi {
    public:
        template<class _outputMatrixType, class _input1MatrixType, class _input2MatrixType>
        static void Run(_outputMatrixType & outputMatrix,
                        const _input1MatrixType & input1Matrix,
                        const _input2MatrixType & input2Matrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;
            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT1_COL_STRIDE = Input1MatrixType::COLSTRIDE,
                INPUT1_ROW_STRIDE = Input1MatrixType::ROWSTRIDE,
                INPUT1_STRIDE_TO_NEXT_ROW = INPUT1_ROW_STRIDE - COLS * INPUT1_COL_STRIDE
            };

            enum {
                INPUT2_COL_STRIDE = Input2MatrixType::COLSTRIDE,
                INPUT2_ROW_STRIDE = Input2MatrixType::ROWSTRIDE,
                INPUT2_STRIDE_TO_NEXT_ROW = INPUT2_ROW_STRIDE - COLS * INPUT2_COL_STRIDE
            };

            OutputPointerType outputPointer = outputMatrix.Pointer();
            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW, 
                input1Pointer += INPUT1_STRIDE_TO_NEXT_ROW,
                input2Pointer += INPUT2_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    input1Pointer += INPUT1_COL_STRIDE,
                    input2Pointer += INPUT2_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < OutputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < OutputMatrixType::COLS; ++colIndex) {
                    outputMatrix.Element(rowIndex, colIndex) = _elementOperationType::Operate(input1Matrix.Element(rowIndex, colIndex), input2Matrix.Element(rowIndex, colIndex));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // MoMiMi class


    template<class _elementOperationType>
    class MoMi {
    public:
        template<class _outputMatrixType, class _inputMatrixType>
            static inline void Run(_outputMatrixType & outputMatrix, 
            const _inputMatrixType & inputMatrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            OutputPointerType outputPointer = outputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW, 
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(*inputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < OutputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < OutputMatrixType::COLS; ++colIndex) {
                    outputMatrix.Element(rowIndex, colIndex) = _elementOperationType::Operate(inputMatrix.Element(rowIndex, colIndex));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // MoMi class


    template<class _elementOperationType>
    class Mio {
    public:
        template<class _inputOutputMatrixType>
            static inline void Run(_inputOutputMatrixType & inputOutputMatrix)
        {
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::size_type size_type;
            typedef typename InputOutputMatrixType::pointer InputOutputPointerType;
#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = InputOutputMatrixType::ROWS,
                COLS = InputOutputMatrixType::COLS,
                COL_STRIDE = InputOutputMatrixType::COLSTRIDE,
                ROW_STRIDE = InputOutputMatrixType::ROWSTRIDE,
                STRIDE_TO_NEXT_ROW = ROW_STRIDE - COLS * COL_STRIDE
            };

            InputOutputPointerType inputOutputPointer = inputOutputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputOutputPointer += STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputOutputPointer += COL_STRIDE)
                {
                    _elementOperationType::Operate(*inputOutputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < InputOutputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < InputOutputMatrixType::COLS; ++colIndex) {
                    _elementOperationType::Operate(inputOutputMatrix.Element(rowIndex, colIndex));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // Mio class


    template<class _elementOperationType>
    class MioMi {
    public:
        template<class _inputOutputMatrixType, class _inputMatrixType>
            static inline void Run(_inputOutputMatrixType & inputOutputMatrix, 
            const _inputMatrixType & inputMatrix)
        {
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::size_type size_type;
            typedef typename InputOutputMatrixType::pointer InputOutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = InputOutputMatrixType::ROWS,
                COLS = InputOutputMatrixType::COLS,
                INPUT_OUTPUT_COL_STRIDE = InputOutputMatrixType::COLSTRIDE,
                INPUT_OUTPUT_ROW_STRIDE = InputOutputMatrixType::ROWSTRIDE,
                INPUT_OUTPUT_STRIDE_TO_NEXT_ROW = INPUT_OUTPUT_ROW_STRIDE - COLS * INPUT_OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            InputOutputPointerType inputOutputPointer = inputOutputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputOutputPointer += INPUT_OUTPUT_STRIDE_TO_NEXT_ROW, 
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputOutputPointer += INPUT_OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    _elementOperationType::Operate(*inputOutputPointer, *inputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < InputOutputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < InputOutputMatrixType::COLS; ++colIndex) {
                    _elementOperationType::Operate(inputOutputMatrix.Element(rowIndex, colIndex), inputMatrix.Element(rowIndex, colIndex));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // MioMi class


    template<class _elementOperationType>
    class MoMiSi {
    public:
        template<class _outputMatrixType, class _inputMatrixType, class _inputScalarType>
            static void Run(_outputMatrixType & outputMatrix,
            const _inputMatrixType & inputMatrix,
            const _inputScalarType inputScalar)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            OutputPointerType outputPointer = outputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW, 
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < OutputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < OutputMatrixType::COLS; ++colIndex) {
                    outputMatrix.Element(rowIndex, colIndex) = _elementOperationType::Operate(inputMatrix.Element(rowIndex, colIndex), inputScalar);
                }
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // MoMiSi class


    template<class _elementOperationType>
    class MoSiMi {
    public:
        template<class _outputMatrixType, class _inputScalarType, class _inputMatrixType>
            static void Run(_outputMatrixType & outputMatrix,
            const _inputScalarType inputScalar,
            const _inputMatrixType & inputMatrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };


            OutputPointerType outputPointer = outputMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW, 
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < OutputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < OutputMatrixType::COLS; ++colIndex) {
                    outputMatrix.Element(rowIndex, colIndex) = _elementOperationType::Operate(inputScalar, inputMatrix.Element(rowIndex, colIndex));
                }
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // MoSiMi class


    template<class _elementOperationType>
    class MioSi {
    public:
        template<class _inputOutputMatrixType, class _inputScalarType>
            static void Run(_inputOutputMatrixType & inputOutputMatrix,
            const _inputScalarType inputScalar)
        {
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::size_type size_type;
            typedef typename InputOutputMatrixType::pointer InputOutputPointerType;
#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = InputOutputMatrixType::ROWS,
                COLS = InputOutputMatrixType::COLS,
                COL_STRIDE = InputOutputMatrixType::COLSTRIDE,
                ROW_STRIDE = InputOutputMatrixType::ROWSTRIDE,
                STRIDE_TO_NEXT_ROW = ROW_STRIDE - COLS * COL_STRIDE
            };

            InputOutputPointerType inputOutputPointer = inputOutputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputOutputPointer += STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputOutputPointer += COL_STRIDE)
                {
                    _elementOperationType::Operate(*inputOutputPointer, inputScalar);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            typedef _inputOutputMatrixType InputOutputMatrixType;
            typedef typename InputOutputMatrixType::size_type size_type;
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < InputOutputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < InputOutputMatrixType::COLS; ++colIndex) {
                    _elementOperationType::Operate(inputOutputMatrix.Element(rowIndex, colIndex), inputScalar);
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // MioSi class


    template<class _incrementalOperationType, class _elementOperationType>
    class SoMi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;

        template<class _inputMatrixType>
            static OutputType Run(const _inputMatrixType & inputMatrix)
        {
            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::size_type size_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();
#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = InputMatrixType::ROWS,
                COLS = InputMatrixType::COLS,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                        _elementOperationType::Operate(*inputPointer) );
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < InputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < InputMatrixType::COLS; ++colIndex) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(inputMatrix.Element(rowIndex, colIndex)));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
            return incrementalResult;
        }  // Run method
    };  // SoMi class


    template<class _incrementalOperationType, class _elementOperationType>
    class SoMiMi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        
        template<class _input1MatrixType, class _input2MatrixType>
        static OutputType Run(const _input1MatrixType & input1Matrix,
                              const _input2MatrixType & input2Matrix)
        {
            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::size_type size_type;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;
            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();
#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = Input1MatrixType::ROWS,
                COLS = Input1MatrixType::COLS,
                INPUT1_COL_STRIDE = Input1MatrixType::COLSTRIDE,
                INPUT1_ROW_STRIDE = Input1MatrixType::ROWSTRIDE,
                INPUT1_STRIDE_TO_NEXT_ROW = INPUT1_ROW_STRIDE - COLS * INPUT1_COL_STRIDE
            };

            enum {
                INPUT2_COL_STRIDE = Input2MatrixType::COLSTRIDE,
                INPUT2_ROW_STRIDE = Input2MatrixType::ROWSTRIDE,
                INPUT2_STRIDE_TO_NEXT_ROW = INPUT2_ROW_STRIDE - COLS * INPUT2_COL_STRIDE
            };

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                input1Pointer += INPUT1_STRIDE_TO_NEXT_ROW,
                input2Pointer += INPUT2_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    input1Pointer += INPUT1_COL_STRIDE,
                    input2Pointer += INPUT2_COL_STRIDE)
                {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                        _elementOperationType::Operate(*input1Pointer, *input2Pointer) );
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < Input1MatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < Input1MatrixType::COLS; ++colIndex) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(input1Matrix.Element(rowIndex, colIndex),
                                                                                                          input2Matrix.Element(rowIndex, colIndex)));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
            return incrementalResult;
        }  // Run method
    };  // SoMiMi class
    
    
    /*!  \brief Implement operation of the form \f$m_{io} =
      op_{io}(m_{io}, op_{sm}(s, m_i))\f$ for fixed size matrices
      
      This class uses template specialization to perform store-back
      matrix-scalar-matrix operations

      \f[
      m_{io} = \mathrm{op_{io}}(m_{io}, \mathrm{op_{sm}}(s, m_i))
      \f]
      
      where \f$m_{io}\f$ is an input-output (store-back) matrix;
    \f$s\f$ is a scalar; and \f$m_i\f$ is an input matrix.  A
    typical example is: \f$m_{io} += s \cdot m_i\f$.  The matrices
    have a fixed size, determined at compilation time; \f$op_{sm}\f$
    is an operation between \f$s\f$ and the elements of \f$m_i\f$;
    \f$op_{io}\f$ is an operation between the output of
    \f$op_{sm}\f$ and the elements of \f$m_{io}\f$.
      
      \param _ioOperationType The type of the store-back operation.
      
      \param _scalarMatrixElementOperationType The type of the
    operation between scalar and input matrix.

      \sa vctFixedSizeMatrixRecursiveEngines
    */
  template<class _ioElementOperationType, class _scalarMatrixElementOperationType>
  class 
    MioSiMi {
  public:

        template<class _ioMatrixType, class _inputScalarType, class _inputMatrixType>
            static inline void Run(_ioMatrixType & ioMatrix, 
      const _inputScalarType & inputScalar, const _inputMatrixType & inputMatrix)
    {
            typedef _ioMatrixType IoMatrixType;
            typedef typename IoMatrixType::size_type size_type;
      typedef typename IoMatrixType::pointer IoPointerType;
      typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            enum {
                ROWS = IoMatrixType::ROWS,
                COLS = IoMatrixType::COLS,
        IO_COL_STRIDE = IoMatrixType::COLSTRIDE,
        IO_ROW_STRIDE = IoMatrixType::ROWSTRIDE,
        IO_STRIDE_TO_NEXT_ROW = IO_ROW_STRIDE - COLS * IO_COL_STRIDE,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            IoPointerType ioPointer = ioMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                ioPointer += IO_STRIDE_TO_NEXT_ROW,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    ioPointer += IO_COL_STRIDE,
                    inputPointer += INPUT_COL_STRIDE)
                {
          _ioElementOperationType::Operate(
            *ioPointer,
            _scalarMatrixElementOperationType::Operate(inputScalar, *inputPointer)
            );
                }
            }
    }

  };

    
    template<class _incrementalOperationType, class _elementOperationType>
    class SoMiSi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        
        template<class _inputMatrixType, class _inputScalarType>
        static OutputType Run(const _inputMatrixType & inputMatrix,
                              const _inputScalarType & inputScalar)
        {
            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::size_type size_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();
#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = InputMatrixType::ROWS,
                COLS = InputMatrixType::COLS,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                        _elementOperationType::Operate(*inputPointer, inputScalar) );
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < InputMatrixType::ROWS; ++rowIndex) {
                for (colIndex = 0; colIndex < InputMatrixType::COLS; ++colIndex) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(inputMatrix.Element(rowIndex, colIndex),
                                                                                                          inputScalar));
                }
            }
#endif
            return incrementalResult;
        }  // Run method
    };  // SoMiSi class



    template<class _operationType>
    class Product {
    public:
        template<class _outputMatrixType, class _input1MatrixType, class _input2MatrixType>
        static void Run(_outputMatrixType & outputMatrix,
                        const _input1MatrixType & input1Matrix,
                        const _input2MatrixType & input2Matrix)
        {
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

#if INCREMENTAL_LOOP_ENGINES
            enum {
                ROWS = OutputMatrixType::ROWS,
                COLS = OutputMatrixType::COLS,
                OUTPUT_COL_STRIDE = OutputMatrixType::COLSTRIDE,
                OUTPUT_ROW_STRIDE = OutputMatrixType::ROWSTRIDE,
                OUTPUT_STRIDE_TO_NEXT_ROW = OUTPUT_ROW_STRIDE - COLS * OUTPUT_COL_STRIDE
            };

            enum {
                INPUT1_COL_STRIDE = Input1MatrixType::COLSTRIDE,
                INPUT1_ROW_STRIDE = Input1MatrixType::ROWSTRIDE
            };

            enum {
                INPUT2_COL_STRIDE = Input2MatrixType::COLSTRIDE,
                INPUT2_ROW_STRIDE = Input2MatrixType::ROWSTRIDE
            };

            OutputPointerType outputPointer = outputMatrix.Pointer();

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            typename Input1MatrixType::ConstRowRefType input1Row;
            typename Input2MatrixType::ConstColumnRefType input2Col;

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex, 
                outputPointer += OUTPUT_STRIDE_TO_NEXT_ROW, 
                input1Pointer += INPUT1_ROW_STRIDE,
                input2Pointer = input2Matrix.Pointer())
            {
                input1Row.SetRef(input1Pointer);
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    outputPointer += OUTPUT_COL_STRIDE,
                    input2Pointer += INPUT2_COL_STRIDE)
                {
                    input2Col.SetRef(input2Pointer);
                    *outputPointer = _operationType::Operate(input1Row, input2Col);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            typedef typename OutputMatrixType::size_type size_type;
            for (size_type rowIndex = 0; rowIndex < OutputMatrixType::ROWS; ++rowIndex) {
                for (size_type colIndex = 0; colIndex < OutputMatrixType::COLS; ++colIndex) {
                    outputMatrix.Element(rowIndex, colIndex) = _operationType::Operate(input1Matrix.Row(rowIndex), input2Matrix.Column(colIndex));                   
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // Product class

    /*! A specialized engine for computing the minimum and maximum elements
     *  of a matrix in one pass.  This implementation is more efficient
     *  than computing them separately.
     */
    class MinAndMax
    {
    public:
        template<class _inputMatrixType>
        static void Run(const _inputMatrixType & inputMatrix,
        typename _inputMatrixType::value_type & minValue, typename _inputMatrixType::value_type & maxValue)
        {
            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::size_type size_type;
            typedef typename InputMatrixType::stride_type stride_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;
            typedef typename InputMatrixType::value_type value_type;

            enum {
                ROWS = InputMatrixType::ROWS,
                COLS = InputMatrixType::COLS,
                INPUT_COL_STRIDE = InputMatrixType::COLSTRIDE,
                INPUT_ROW_STRIDE = InputMatrixType::ROWSTRIDE,
                INPUT_STRIDE_TO_NEXT_ROW = INPUT_ROW_STRIDE - COLS * INPUT_COL_STRIDE
            };

            InputPointerType inputPointer = inputMatrix.Pointer();
            if (inputPointer == 0)
                return;

            value_type minElement, maxElement;
            maxElement = minElement = *inputPointer;

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < ROWS; ++rowIndex,
                inputPointer += INPUT_STRIDE_TO_NEXT_ROW)
            {
                for (colIndex = 0; colIndex < COLS; ++colIndex,
                    inputPointer += INPUT_COL_STRIDE)
                {
                    const value_type element = *inputPointer;
                    if (element < minElement) {
                        minElement = element;
                    }
                    else if (maxElement < element) {
                        maxElement = element;
                    }
                }
            }

            minValue = minElement;
            maxValue = maxElement;
        }  // Run method
    };  // MinAndMax class


};


#endif  // _vctFixedSizeMatrixLoopEngines_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeMatrixLoopEngines.h,v $
// Revision 1.14  2006/06/09 21:53:29  anton
// cisstVector: Fixed typos in Doxygen comments for engines.
//
// Revision 1.13  2006/05/21 11:25:48  ofri
// vct[FixedSize|Dynamic] engines: added a new engine CioSiCi , for operations
// such as MultAdd
//
// Revision 1.12  2006/01/11 19:33:13  anton
// vctFixedSizeMatrixLoopEngines: Corrected typo (bug) in engines MoMiMi and
// SoMiMi.
//
// Revision 1.11  2005/12/12 23:51:27  ofri
// vct[FixedSize|Dynamic][Vector|Matrix][Recursive|Loop]Engines:
// Added specialized engine MinAndMax for efficient computation of upper
// and lower bounds of data.
//
// Revision 1.10  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.9  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.8  2005/09/23 23:53:03  anton
// vctFixedSizeMatrixLoopEngines: Removed useless , after last element of enum
//
// Revision 1.7  2005/08/26 22:01:09  ofri
// vctFixedMatrixLoopEngines.h: Implemented the fast operation method following
// ticket #163.  The fast method has been added to all the engines in this file.
// It is conditionally compiled through setting the flag INCREMENTAL_LOOP_ENGINES.
// This is so that speed comparison can be done in the future.
//
// Revision 1.6  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.5  2004/12/01 21:50:07  anton
// vctFixedSizeMatrixLoopEngines.h: Layout
//
// Revision 1.4  2004/12/01 18:04:11  anton
// cisstVector matrix loop engines:  Replaced iterators by nested loops with
// calls to the Element() method.  See ticket #79.  The iterators were used
// in conjunction with the _incrementalOperation to find the first element
// and continue from there.  For the fixed size matrix, I used the Neutral-
// Element introduced with #71.
//
// Revision 1.3  2004/11/30 23:35:05  anton
// vctMatrixLoopengines: Now uses the method Element() where [][] was used.
// I still wonder if it is normal to have some engines using iterators. See #79
//
// Revision 1.2  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.1  2004/08/13 17:47:40  anton
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
// Revision 1.4  2004/08/04 21:07:57  anton
// vctMatrixLoopEngines.h: An error was introduced when I modified the
// signature of the Run method of {quote: MoSiMi}, the scalar was still passed
// by reference and the matrix was passed by copy.  It was supposed to be the
// inverse.
//
// Revision 1.3  2004/07/15 17:57:29  anton
// cisstVector engines using input scalars rely on copies, not reference
// anymore since the scalar used could be modified by the operation itself
// (see ticket #51).  We decided to use a const copy even if this might be
// at a slight efficiency cost.  This modification has an impact on all engines
// (loop/recursive, sequence/matrix) and we decided to also modify the user API
// (e.g. sequence.RatioOf(const sequence &, const scalar)) so that the internal
// behavior can be deduced from the signature on the method.
//
// Revision 1.2  2004/07/12 20:27:57  anton
// cisstVector: Removed engine SoMoSi used by vctFixedSizeMatrix::SetAll()
// which now relies on MioSi
//
// Revision 1.1  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.3  2004/01/15 15:17:25  anton
// Added SoMiSi engine
//
// Revision 1.2  2003/12/19 21:46:38  anton
// Added Product engine
//
// Revision 1.1  2003/12/18 16:23:06  anton
// Creation
//
//
// ****************************************************************************


