/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrixLoopEngines.h,v 1.17 2006/06/09 21:53:29 anton Exp $

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


#ifndef _vctDynamicMatrixLoopEngines_h
#define _vctDynamicMatrixLoopEngines_h

/*!
  \file 
  \brief Declaration of vctDynamicMatrixLoopEngines
 */


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>


// This macro is added as an option to select the implementation of
// loop engines, e.g., for the purpose of speed comparison.
#ifndef INCREMENTAL_LOOP_ENGINES
#define INCREMENTAL_LOOP_ENGINES 1
#else
#define EXTERN_INCREMENTAL_DEF
#endif

/*!
  \brief Container class for the dynamic matrix engines.
  
  \sa MoMiMi MioMi MoMiSi MoSiMi MioSi MoMi Mio SoMi SoMiMi
*/
class vctDynamicMatrixLoopEngines {
 public:    

  /*! Helper function to throw an exception whenever sizes mismatch.
      This enforces that a standard message is sent. */
    inline static void ThrowSizeMismatchException(void) throw(std::runtime_error) {
        cmnThrow(std::runtime_error("vctDynamicMatrixLoopEngines: Sizes of matrices don't match"));
    }


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
            // check sizes
            typedef _outputMatrixType OutputMatrixType;
            typedef typename OutputMatrixType::size_type size_type;
            typedef typename OutputMatrixType::stride_type stride_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();
            if ((rows != input1Matrix.rows())
                || (cols != input1Matrix.cols())
                || (rows != input2Matrix.rows())
                || (cols != input2Matrix.cols()))
            {
                ThrowSizeMismatchException();
            }

            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputColStride = outputMatrix.col_stride();
            const stride_type outputRowStride = outputMatrix.row_stride();
            const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

            const stride_type input1ColStride = input1Matrix.col_stride();
            const stride_type input1RowStride = input1Matrix.row_stride();
            const stride_type input1StrideToNextRow = input1RowStride - cols * input1ColStride;

            const stride_type input2ColStride = input2Matrix.col_stride();
            const stride_type input2RowStride = input2Matrix.row_stride();
            const stride_type input2StrideToNextRow = input2RowStride - cols * input2ColStride;

            OutputPointerType outputPointer = outputMatrix.Pointer();
            const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
            OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            for (; outputPointer != outputRowEnd; 
                outputPointer += outputStrideToNextRow, 
                input1Pointer += input1StrideToNextRow,
                input2Pointer += input2StrideToNextRow,
                outputColEnd += outputRowStride)
            {
                for (; outputPointer != outputColEnd;
                    outputPointer += outputColStride,
                    input1Pointer += input1ColStride,
                    input2Pointer += input2ColStride)
                {
                    *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
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
            typedef typename OutputMatrixType::stride_type stride_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();
            // check sizes
            if ((rows != inputMatrix.rows()) || (cols != inputMatrix.cols()))
            {
                ThrowSizeMismatchException();
            }

            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputColStride = outputMatrix.col_stride();
            const stride_type outputRowStride = outputMatrix.row_stride();
            const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

            OutputPointerType outputPointer = outputMatrix.Pointer();
            const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
            OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

            InputPointerType inputPointer = inputMatrix.Pointer();

            for (; outputPointer != outputRowEnd; 
                outputPointer += outputStrideToNextRow, 
                inputPointer += inputStrideToNextRow,
                outputColEnd += outputRowStride)
            {
                for (; outputPointer != outputColEnd;
                    outputPointer += outputColStride,
                    inputPointer += inputColStride)
                {
                    *outputPointer = _elementOperationType::Operate(*inputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
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
            typedef typename InputOutputMatrixType::stride_type stride_type;
            typedef typename InputOutputMatrixType::pointer PointerType;

            const size_type rows = inputOutputMatrix.rows();
            const size_type cols = inputOutputMatrix.cols();

            // No need for size checking here, since only one operand
#if INCREMENTAL_LOOP_ENGINES
            const stride_type colStride = inputOutputMatrix.col_stride();
            const stride_type rowStride = inputOutputMatrix.row_stride();
            const stride_type strideToNextRow = rowStride - cols * colStride;

            PointerType inputOutputPointer = inputOutputMatrix.Pointer();
            const PointerType rowEnd = inputOutputPointer + rows * rowStride;
            PointerType colEnd = inputOutputPointer + cols * colStride;

            for (; inputOutputPointer != rowEnd; 
                inputOutputPointer += strideToNextRow, 
                colEnd += rowStride)
            {
                for (; inputOutputPointer != colEnd;
                    inputOutputPointer += colStride)
                {
                    _elementOperationType::Operate(*inputOutputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex)
            {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
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
            typedef typename InputOutputMatrixType::stride_type stride_type;
            typedef typename InputOutputMatrixType::pointer InputOutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            const size_type rows = inputOutputMatrix.rows();
            const size_type cols = inputOutputMatrix.cols();
            // check sizes
            if ((rows != inputMatrix.rows()) || (cols != inputMatrix.cols()))
            {
                ThrowSizeMismatchException();
            }
#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputOutputColStride = inputOutputMatrix.col_stride();
            const stride_type inputOutputRowStride = inputOutputMatrix.row_stride();
            const stride_type inputOutputStrideToNextRow = inputOutputRowStride - cols * inputOutputColStride;

            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

            InputOutputPointerType inputOutputPointer = inputOutputMatrix.Pointer();
            const InputOutputPointerType inputOutputRowEnd = inputOutputPointer + rows * inputOutputRowStride;
            InputOutputPointerType inputOutputColEnd = inputOutputPointer + cols * inputOutputColStride;

            InputPointerType inputPointer = inputMatrix.Pointer();

            for (; inputOutputPointer != inputOutputRowEnd; 
                inputOutputPointer += inputOutputStrideToNextRow, 
                inputPointer += inputStrideToNextRow,
                inputOutputColEnd += inputOutputRowStride)
            {
                for (; inputOutputPointer != inputOutputColEnd;
                    inputOutputPointer += inputOutputColStride,
                    inputPointer += inputColStride)
                {
                    _elementOperationType::Operate(*inputOutputPointer, *inputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            // otherwise
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
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
            typedef typename OutputMatrixType::stride_type stride_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();
            // check sizes
            if ((rows != inputMatrix.rows()) || (cols != inputMatrix.cols()))
            {
                ThrowSizeMismatchException();
            }

            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputColStride = outputMatrix.col_stride();
            const stride_type outputRowStride = outputMatrix.row_stride();
            const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

            OutputPointerType outputPointer = outputMatrix.Pointer();
            const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
            OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

            InputPointerType inputPointer = inputMatrix.Pointer();

            for (; outputPointer != outputRowEnd; 
                outputPointer += outputStrideToNextRow, 
                inputPointer += inputStrideToNextRow,
                outputColEnd += outputRowStride)
            {
                for (; outputPointer != outputColEnd;
                    outputPointer += outputColStride,
                    inputPointer += inputColStride)
                {
                    *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
                    outputMatrix.Element(rowIndex, colIndex) = _elementOperationType::Operate(inputMatrix.Element(rowIndex, colIndex), inputScalar);
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }  // Run method
    };  // MoMiSi


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
            typedef typename OutputMatrixType::stride_type stride_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();
            // check sizes
            if ((rows != inputMatrix.rows()) || (cols != inputMatrix.cols()))
            {
                ThrowSizeMismatchException();
            }

            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputColStride = outputMatrix.col_stride();
            const stride_type outputRowStride = outputMatrix.row_stride();
            const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

            OutputPointerType outputPointer = outputMatrix.Pointer();
            const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
            OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

            InputPointerType inputPointer = inputMatrix.Pointer();

            for (; outputPointer != outputRowEnd; 
                outputPointer += outputStrideToNextRow, 
                inputPointer += inputStrideToNextRow,
                outputColEnd += outputRowStride)
            {
                for (; outputPointer != outputColEnd;
                    outputPointer += outputColStride,
                    inputPointer += inputColStride)
                {
                    *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
                    outputMatrix.Element(rowIndex, colIndex) = _elementOperationType::Operate(inputScalar, inputMatrix.Element(rowIndex, colIndex));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
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
            typedef typename InputOutputMatrixType::stride_type stride_type;
            typedef typename InputOutputMatrixType::pointer PointerType;

            const size_type rows = inputOutputMatrix.rows();
            const size_type cols = inputOutputMatrix.cols();

            // No need for size checking here, since only one operand
#if INCREMENTAL_LOOP_ENGINES
            const stride_type colStride = inputOutputMatrix.col_stride();
            const stride_type rowStride = inputOutputMatrix.row_stride();
            const stride_type strideToNextRow = rowStride - cols * colStride;

            PointerType inputOutputPointer = inputOutputMatrix.Pointer();
            const PointerType rowEnd = inputOutputPointer + rows * rowStride;
            PointerType colEnd = inputOutputPointer + cols * colStride;

            for (; inputOutputPointer != rowEnd; 
                inputOutputPointer += strideToNextRow, 
                colEnd += rowStride)
            {
                for (; inputOutputPointer != colEnd;
                    inputOutputPointer += colStride)
                {
                    _elementOperationType::Operate(*inputOutputPointer, inputScalar);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
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
            typedef typename InputMatrixType::stride_type stride_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            const size_type rows = inputMatrix.rows();
            const size_type cols = inputMatrix.cols();

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();
#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;
            InputPointerType inputPointer = inputMatrix.Pointer();
            const InputPointerType inputRowEnd = inputPointer + rows * inputRowStride;
            InputPointerType inputColEnd = inputPointer + cols * inputColStride;


            for (; inputPointer != inputRowEnd; 
                inputPointer += inputStrideToNextRow,
                inputColEnd += inputRowStride)
            {
                for (; inputPointer != inputColEnd;
                    inputPointer += inputColStride)
                {
                    incrementalResult = _incrementalOperationType::Operate(
                        incrementalResult, _elementOperationType::Operate(*inputPointer) );
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
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
                              const _input2MatrixType & input2Matrix) {
            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::size_type size_type;
            typedef typename Input1MatrixType::stride_type stride_type;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            const size_type rows = input1Matrix.rows();
            const size_type cols = input1Matrix.cols();
            // check sizes
            if ((rows != input2Matrix.rows()) || (cols != input2Matrix.cols())) {
                ThrowSizeMismatchException();
            }

            // otherwise
            OutputType incrementalResult = _incrementalOperationType::NeutralElement();
#if INCREMENTAL_LOOP_ENGINES
            const stride_type input1ColStride = input1Matrix.col_stride();
            const stride_type input1RowStride = input1Matrix.row_stride();
            const stride_type input1StrideToNextRow = input1RowStride - cols * input1ColStride;

            const stride_type input2ColStride = input2Matrix.col_stride();
            const stride_type input2RowStride = input2Matrix.row_stride();
            const stride_type input2StrideToNextRow = input2RowStride - cols * input2ColStride;

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            const Input1PointerType input1RowEnd = input1Pointer + rows * input1RowStride;
            Input1PointerType input1ColEnd = input1Pointer + cols * input1ColStride;

            Input2PointerType input2Pointer = input2Matrix.Pointer();

            for (; input1Pointer != input1RowEnd; 
                input1Pointer += input1StrideToNextRow,
                input2Pointer += input2StrideToNextRow,
                input1ColEnd += input1RowStride)
            {
                for (; input1Pointer != input1ColEnd;
                    input1Pointer += input1ColStride,
                    input2Pointer += input2ColStride)
                {
                    incrementalResult = _incrementalOperationType::Operate(
                        incrementalResult, _elementOperationType::Operate(*input1Pointer, *input2Pointer) );
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(input1Matrix.Element(rowIndex, colIndex),
                                                                                                          input2Matrix.Element(rowIndex, colIndex)));
                }
            }
#endif  // INCREMENTAL_LOOP_ENGINES
            return incrementalResult;
        }
    };
    

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
            typedef typename IoMatrixType::stride_type stride_type;
      typedef typename IoMatrixType::pointer IoPointerType;
      typedef _inputMatrixType InputMatrixType;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            const size_type rows = ioMatrix.rows();
            const size_type cols = ioMatrix.cols();
            // check sizes
            if ((rows != inputMatrix.rows()) || (cols != inputMatrix.cols())) {
                ThrowSizeMismatchException();
            }

      const stride_type ioColStride = ioMatrix.col_stride();
            const stride_type ioRowStride = ioMatrix.row_stride();
            const stride_type ioStrideToNextRow = ioRowStride - cols * ioColStride;

      const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;

            IoPointerType ioPointer = ioMatrix.Pointer();
            InputPointerType inputPointer = inputMatrix.Pointer();

            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex,
                ioPointer += ioStrideToNextRow,
                inputPointer += inputStrideToNextRow)
            {
                for (colIndex = 0; colIndex < cols; ++colIndex,
                    ioPointer += ioColStride,
                    inputPointer += inputColStride)
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
            typedef typename InputMatrixType::stride_type stride_type;
            typedef typename InputMatrixType::const_pointer InputPointerType;

            const size_type rows = inputMatrix.rows();
            const size_type cols = inputMatrix.cols();
            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;
            InputPointerType inputPointer = inputMatrix.Pointer();
            const InputPointerType inputRowEnd = inputPointer + rows * inputRowStride;
            InputPointerType inputColEnd = inputPointer + cols * inputColStride;

            for (; inputPointer != inputRowEnd; 
                inputPointer += inputStrideToNextRow,
                inputColEnd += inputRowStride)
            {
                for (; inputPointer != inputColEnd;
                    inputPointer += inputColStride)
                {
                    incrementalResult = _incrementalOperationType::Operate(
                        incrementalResult, _elementOperationType::Operate(*inputPointer, inputScalar) );
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
                    incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                           _elementOperationType::Operate(inputMatrix.Element(rowIndex, colIndex),
                                                                                                          inputScalar));
                }
            }
#endif
            return incrementalResult;
        }
    };



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
            typedef typename OutputMatrixType::stride_type stride_type;
            typedef typename OutputMatrixType::pointer OutputPointerType;

            typedef _input1MatrixType Input1MatrixType;
            typedef typename Input1MatrixType::const_pointer Input1PointerType;

            typedef _input2MatrixType Input2MatrixType;
            typedef typename Input2MatrixType::const_pointer Input2PointerType;

            const size_type rows = outputMatrix.rows();
            const size_type cols = outputMatrix.cols();
            const size_type input1Cols = input1Matrix.cols();
            // check sizes
            if ((rows != input1Matrix.rows())
                || (cols != input2Matrix.cols())
                || (input1Cols != input2Matrix.rows()))
            {
                ThrowSizeMismatchException();
            }

            // Otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputColStride = outputMatrix.col_stride();
            const stride_type outputRowStride = outputMatrix.row_stride();
            const stride_type outputStrideToNextRow = outputRowStride - cols * outputColStride;

            const stride_type input1ColStride = input1Matrix.col_stride();
            const stride_type input1RowStride = input1Matrix.row_stride();

            const stride_type input2ColStride = input2Matrix.col_stride();
            const stride_type input2RowStride = input2Matrix.row_stride();

            OutputPointerType outputPointer = outputMatrix.Pointer();
            const OutputPointerType outputRowEnd = outputPointer + rows * outputRowStride;
            OutputPointerType outputColEnd = outputPointer + cols * outputColStride;

            Input1PointerType input1Pointer = input1Matrix.Pointer();
            Input2PointerType input2Pointer = input2Matrix.Pointer();

            typename Input1MatrixType::ConstRowRefType input1Row;
            typename Input2MatrixType::ConstColumnRefType input2Col;

            for (; outputPointer != outputRowEnd; 
                outputPointer += outputStrideToNextRow, 
                input1Pointer += input1RowStride,
                input2Pointer = input2Matrix.Pointer(),
                outputColEnd += outputRowStride)
            {
                input1Row.SetRef(input1Cols, input1Pointer, input1ColStride);
                for (; outputPointer != outputColEnd;
                    outputPointer += outputColStride,
                    input2Pointer += input2ColStride)
                {
                    input2Col.SetRef(input1Cols, input2Pointer, input2RowStride);
                    *outputPointer = _operationType::Operate(input1Row, input2Col);
                }
            }
#else  // INCREMENTAL_LOOP_ENGINES
            size_type rowIndex, colIndex;
            for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
                for (colIndex = 0; colIndex < cols; ++colIndex) {
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

            const size_type rows = inputMatrix.rows();
            const size_type cols = inputMatrix.cols();

            const stride_type inputColStride = inputMatrix.col_stride();
            const stride_type inputRowStride = inputMatrix.row_stride();
            const stride_type inputStrideToNextRow = inputRowStride - cols * inputColStride;
            InputPointerType inputPointer = inputMatrix.Pointer();
            const InputPointerType inputRowEnd = inputPointer + rows * inputRowStride;
            InputPointerType inputColEnd = inputPointer + cols * inputColStride;

            if (inputPointer == 0)
                return;

            value_type minElement, maxElement;
            maxElement = minElement = *inputPointer;

            for (; inputPointer != inputRowEnd; 
                inputPointer += inputStrideToNextRow,
                inputColEnd += inputRowStride)
            {
                for (; inputPointer != inputColEnd;
                    inputPointer += inputColStride)
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


#ifdef EXTERN_INCREMENTAL_DEF
#undef EXTERN_INCREMENTAL_DEF
#else
#undef INCREMENTAL_LOOP_ENGINES
#endif


#endif  // _vctDynamicMatrixLoopEngines_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctDynamicMatrixLoopEngines.h,v $
// Revision 1.17  2006/06/09 21:53:29  anton
// cisstVector: Fixed typos in Doxygen comments for engines.
//
// Revision 1.16  2006/05/21 11:25:48  ofri
// vct[FixedSize|Dynamic] engines: added a new engine CioSiCi , for operations
// such as MultAdd
//
// Revision 1.15  2005/12/12 23:51:27  ofri
// vct[FixedSize|Dynamic][Vector|Matrix][Recursive|Loop]Engines:
// Added specialized engine MinAndMax for efficient computation of upper
// and lower bounds of data.
//
// Revision 1.14  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.13  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.12  2005/09/24 00:01:04  anton
// cisstVector: Use cmnThrow for all exceptions.
//
// Revision 1.11  2005/08/26 22:00:47  ofri
// vctDynamicMatrixLoopEngines: tiny optimization in the Product engine --
// setting input1RowRef at the beginning of the loop
//
// Revision 1.10  2005/08/26 19:48:54  ofri
// vctDynamicMatrixLoopEngines.h: Implemented the fast operation method following
// ticket #163.  The fast method has been added to all the engines in this file.
// It is conditionally compiled through setting the flag INCREMENTAL_LOOP_ENGINES.
// This is so that speed comparison can be done in the future.
//
// Revision 1.9  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2004/12/01 21:51:48  anton
// cisstVector: Use the NeutralElement() of the incremental operations for
// the dynamic container engines (see #97).  Also rewrote the vector engines
// to use a single index with [] instead of multiple interators.
//
// Revision 1.7  2004/12/01 18:04:11  anton
// cisstVector matrix loop engines:  Replaced iterators by nested loops with
// calls to the Element() method.  See ticket #79.  The iterators were used
// in conjunction with the _incrementalOperation to find the first element
// and continue from there.  For the fixed size matrix, I used the Neutral-
// Element introduced with #71.
//
// Revision 1.6  2004/12/01 17:14:53  anton
// cisstVector:  Replace access to matrix elements by Element() instead of
// double [][] for increased performance.  See ticket #79.
//
// Revision 1.5  2004/11/30 23:35:05  anton
// vctMatrixLoopengines: Now uses the method Element() where [][] was used.
// I still wonder if it is normal to have some engines using iterators. See #79
//
// Revision 1.4  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.3  2004/09/21 20:04:15  anton
// vctDynamicMatrixLoopEngines: Stupid bug, ThrowException() was empty and
// exceptions were not thrown.  Deconcerting.
//
// Revision 1.2  2004/09/08 21:22:17  anton
// vctDynamicMatrix: Added method ProductOf() as well as operator * for two
// matrices.  Needed to implement Column() and Row() methods as well.
// Corrected bugs in ToStream method.
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
// Revision 1.2  2004/08/12 20:21:48  anton
// cisstVector: Removed push/pop of warning disable for Microsoft compilers
// since the warning 4290 is now disabled in cmnPortability.h
// (see ticket #53 and cvs commit 678)
//
// Revision 1.1  2004/08/04 21:11:10  anton
// cisstVector: Added preliminary version of dynamic matrices.  Lots of work
// is still required, this code is not ready to be used.
//
//
// ****************************************************************************

