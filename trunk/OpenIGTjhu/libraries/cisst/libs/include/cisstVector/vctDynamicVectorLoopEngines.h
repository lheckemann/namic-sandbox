/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicVectorLoopEngines.h,v 1.15 2006/06/09 21:53:29 anton Exp $

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  2004-07-01

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


#ifndef _vctDynamicVectorLoopEngines_h 
#define _vctDynamicVectorLoopEngines_h

/*!
  \file 
  \brief Declaration of vctDynamicVectorLoopEngines
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
  \brief Container class for the vector loop based engines.
  
  Loop engines can be used for dynamic vectors (see vctDynamicVector)
  to apply similar operations (see vctBinaryOperations,
  vctUnaryOperations, vctStoreBackBinaryOperations,
  vctStoreBackUnaryOperations).  Each engine corresponds to an
  operation signature.

  Loop engines are named according to the type of the parameters
  and their role (i.e. input/output).  The order reflects the
  mathematical expression.  For exemple, VoViVi stands for one output
  vector with two input vectors used in \f$v_o = v_i op v_i\f$ and VioSi
  stands for one input/output vector and one scalar input used in
  \f$v_{io} = v_{io} op s_i\f$.

  The implementation is based on loops.  The inner class are templated
  by the operation type and allows to plug any operation with a given
  signature.

  All vector types must support the index operator ([]) to access
  their elements.  The input vector types must have it as const
  method. The output vector type must have it as non-const method.

  The method size() is also required.  For the operations based on two
  or more vectors, this method is used to check that the operands have
  the same size.  If the sizes don't match, an exception of type
  std::runtime_error is thrown.

  \sa vctFixedSizeVectorRecursiveEngines VoViVi VioVi VoViSi VoSiVi VioSi
  VoVi Vio SoVi SoViVi SoVoSi
*/
class vctDynamicVectorLoopEngines {

 public:

  /*! Helper function to throw an exception whenever sizes mismatch.
      This enforces that a standard message is sent. */
    inline static void ThrowException(void) throw(std::runtime_error) {
        cmnThrow(std::runtime_error("vctDynamicVectorLoopEngines: Sizes of vectors don't match"));
    }


    /*!  \brief Implement operation of the form \f$v_o = op(v_{i1},
      v_{i2})\f$ for dynamic vectors.

      This class uses a loop to perform binary vector operations
      of the form 
      \f[
      v_o = \mathrm{op}(v_{i1}, v_{i2})
      \f]
      
      where \f$v_o\f$ is the output vector, and \f$v_{i1}, v_{i2}\f$
      are input vectors, all of the same size, \em op stands for the a
      binary operation performed elementwise between
      \f$v_{i1}\f$ and \f$v_{i2}\f$, and whose result is stored
      elementwise into \f$v_o\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VoViVi {
    public:
        /*! Unroll the loop
          \param outputVector The output vector.
          \param input1Vector The first input vector.
          \param input2Vector The second input vector.
        */
        template<class _outputVectorType, class _input1VectorType, class _input2VectorType>
        static inline void Run(_outputVectorType & outputVector,
                               const _input1VectorType & input1Vector, 
                               const _input2VectorType & input2Vector) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::pointer OutputPointerType;
            typedef typename OutputVectorType::size_type size_type;
            typedef typename OutputVectorType::stride_type stride_type;

            typedef _input1VectorType Input1VectorType;
            typedef typename Input1VectorType::const_pointer Input1PointerType;

            typedef _input2VectorType Input2VectorType;
            typedef typename Input2VectorType::const_pointer Input2PointerType;

            const size_type size = outputVector.size();
            if ((size != input1Vector.size())
                || (size != input2Vector.size())) {
                ThrowException();
            }
            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputStride = outputVector.stride();
            const stride_type input1Stride = input1Vector.stride();
            const stride_type input2Stride = input2Vector.stride();

            OutputPointerType outputPointer = outputVector.Pointer();
            const OutputPointerType outputEnd = outputPointer + size * outputStride;

            Input1PointerType input1Pointer = input1Vector.Pointer();
            Input2PointerType input2Pointer = input2Vector.Pointer();

            for (;
                 outputPointer != outputEnd;
                 outputPointer += outputStride, 
                     input1Pointer += input1Stride,
                     input2Pointer += input2Stride) {
                *outputPointer = _elementOperationType::Operate(*input1Pointer, *input2Pointer);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                outputVector[index] = _elementOperationType::Operate(input1Vector[index], input2Vector[index]);
            }
#endif  // INCREMENTAL_LOOP_ENGINES
        }
    };


    /*!  \brief Implement operation of the form \f$v_{io} = op(v_{io},
      v_i)\f$ for dynamic vectors

      This class uses a loop to perform binary vector operations
      of the form 
      \f[
      v_{io} = \mathrm{op}(v_{io}, v_{i})
      \f]
      
      where \f$v_{io}\f$ is the input output vector, and \f$v_{i}\f$
      is the second input vector, all of same size, \em op stands for
      the a binary operation performed elementwise between
      \f$v_{io}\f$ and \f$v_{i}\f$, and whose result is stored
      elementwise into \f$v_{io}\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VioVi {
    public:
        /*! Unroll the loop
          
        \param inputOutputVector The input output vector.
        \param inputVector The second input vector.
        */
        template<class _inputOutputVectorType, class _inputVectorType>
        static void Run(_inputOutputVectorType & inputOutputVector,
                        const _inputVectorType & inputVector) {
            // check size
            typedef _inputOutputVectorType InputOutputVectorType;
            typedef typename InputOutputVectorType::pointer InputOutputPointerType;
            typedef typename InputOutputVectorType::size_type size_type;
            typedef typename InputOutputVectorType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;

            const size_type size = inputOutputVector.size();
            if (size != inputVector.size()) {
                ThrowException();
            }
            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputOutputStride = inputOutputVector.stride();
            const stride_type inputStride = inputVector.stride();

            InputOutputPointerType inputOutputPointer = inputOutputVector.Pointer();
            const InputOutputPointerType inputOutputEnd = inputOutputPointer + size * inputOutputStride;

            InputPointerType inputPointer = inputVector.Pointer();

            for (;
                 inputOutputPointer != inputOutputEnd; 
                 inputOutputPointer += inputOutputStride, 
                     inputPointer += inputStride) {
                *inputOutputPointer = _elementOperationType::Operate(*inputOutputPointer, *inputPointer);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                _elementOperationType::Operate(inputOutputVector[index], inputVector[index]);
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }
    };
    

    /*!  \brief Implement operation of the form \f$(v_{1}, v_{2}) = op(v_{1},
      v_{2})\f$ for dynamic vectors

      This class uses a loop to perform binary vector operations
      of the form 
      \f[
      (v_{1}, v_{2}) = \mathrm{op}(v_{1}, v_{2})
      \f]
      
      where both \f$v_{1}, v_{2}\f$ are input and output vectors
      of an equal fixed size, determined at compilation time.  The
      operation is evaluated elementwise, that is, \f$(v_{1}[i], v_{2}[i] =
      \mathrm{op}(v_{1}[i], v_{2}[i])\f$.  The typical operation in this case
      is swapping the elements of the two vectors.
      
      \param _elementOperationType The type of the binary operation that inputs and 
      rewrites corresponding elements in both vectors.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VioVio {
    public:
        /*! Unroll the loop
          
        \param inputOutput1Vector The input output vector.
        \param inputOutput2Vector The second input vector.
        */
        template<class _inputOutput1VectorType, class _inputOutput2VectorType>
        static void Run(_inputOutput1VectorType & inputOutput1Vector,
                        _inputOutput2VectorType & inputOutput2Vector) {
            // check size
            typedef _inputOutput1VectorType InputOutput1VectorType;
            typedef typename InputOutput1VectorType::pointer InputOutput1PointerType;
            typedef typename InputOutput1VectorType::size_type size_type;
            typedef typename InputOutput1VectorType::stride_type stride_type;

            typedef _inputOutput2VectorType InputOutput2VectorType;
            typedef typename InputOutput2VectorType::pointer InputOutput2PointerType;

            const size_type size = inputOutput1Vector.size();
            if (size != inputOutput2Vector.size()) {
                ThrowException();
            }
            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputOutput1Stride = inputOutput1Vector.stride();
            const stride_type inputOutput2Stride = inputOutput2Vector.stride();

            InputOutput1PointerType inputOutput1Pointer = inputOutput1Vector.Pointer();
            const InputOutput1PointerType inputOutput1End = inputOutput1Pointer + size * inputOutput1Stride;

            InputOutput2PointerType inputOutput2Pointer = inputOutput2Vector.Pointer();

            for (;
                 inputOutput1Pointer != inputOutput1End; 
                 inputOutput1Pointer += inputOutput1Stride, 
                     inputOutput2Pointer += inputOutput2Stride) {
                _elementOperationType::Operate(*inputOutput1Pointer, *inputOutput2Pointer);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                _elementOperationType::Operate(inputOutput1Vector[index], inputOutput2Vector[index]);
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }
    };
    

    /*!  \brief Implement operation of the form \f$vo = op(vi, si)\f$
      for dynamic vectors
      
      This class uses a loop to perform binary vector operations
      of the form 
      \f[
      v_o = \mathrm{op}(v_i, s_i)
      \f]
      
      where \f$v_o\f$ is the output vector, and \f$v_i, s_i\f$ are
      input vector and scalar, all vectors are of same size, \em op
      stands for the a binary operation performed elementwise
      between \f$v_i\f$ and \f$s_i\f$, and whose result is stored
      elementwise into \f$v_o\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
        class VoViSi {
        public:
        /*! Unroll the loop
          
        \param outputVector The output vector.
        \param inputVector The input vector (first operand).
        \param inputScalar The input scalar (second operand).
        */
        template<class _outputVectorType, class _inputVectorType, class _inputScalarType>
            static inline void Run(_outputVectorType & outputVector,
                                   const _inputVectorType & inputVector, 
                                   const _inputScalarType inputScalar) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::pointer OutputPointerType;
            typedef typename OutputVectorType::size_type size_type;
            typedef typename OutputVectorType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;

            const size_type size = outputVector.size();
            if (size != inputVector.size()) {
                ThrowException();
            }
            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputStride = outputVector.stride();
            const stride_type inputStride = inputVector.stride();

            OutputPointerType outputPointer = outputVector.Pointer();
            const OutputPointerType outputEnd = outputPointer + size * outputStride;

            InputPointerType inputPointer = inputVector.Pointer();

            for (;
                 outputPointer != outputEnd; 
                 outputPointer += outputStride, 
                     inputPointer += inputStride) {
                *outputPointer = _elementOperationType::Operate(*inputPointer, inputScalar);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                outputVector[index] = _elementOperationType::Operate(inputVector[index], inputScalar);
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }
    };


    /*!  \brief Implement operation of the form \f$vo = op(si, vi)\f$ for
      dynamic vectors
            
      This class uses a loop to perform binary vector operations
      of the form 
      \f[
      v_o = \mathrm{op}(s_i, v_i)
      \f]
      
      where \f$v_o\f$ is the output vector, and \f$s_i, v_i\f$ are
      input scalar and vector, all vectors are of same size, \em op
      stands for the a binary operation performed elementwise between
      \f$s_i\f$ and \f$v_i\f$, and whose result is stored elementwise
      into \f$v_o\f$.

      \param _elementOperationType The type of the binary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
        class VoSiVi {
        public:
        /*! Unroll the loop
          
        \param outputVector The output vector.
        \param inputScalar The input scalar (first operand).
        \param inputVector The input vector (second operand).
        */
        template<class _outputVectorType, class _inputScalarType, class _inputVectorType>
            static inline void Run(_outputVectorType & outputVector,
                                   const _inputScalarType inputScalar, 
                                   const _inputVectorType & inputVector) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::pointer OutputPointerType;
            typedef typename OutputVectorType::size_type size_type;
            typedef typename OutputVectorType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;

            const size_type size = outputVector.size();
            if (size != inputVector.size()) {
                ThrowException();
            }
            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputStride = outputVector.stride();
            const stride_type inputStride = inputVector.stride();

            OutputPointerType outputPointer = outputVector.Pointer();
            const OutputPointerType outputEnd = outputPointer + size * outputStride;

            InputPointerType inputPointer = inputVector.Pointer();

            for (;
                 outputPointer != outputEnd; 
                 outputPointer += outputStride, 
                     inputPointer += inputStride) {
                *outputPointer = _elementOperationType::Operate(inputScalar, *inputPointer);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                outputVector[index] = _elementOperationType::Operate(inputScalar, inputVector[index]);
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }
    };


    /*!  \brief Implement operation of the form \f$ v_{io} =
      op(v_{io}, s_i)\f$ for dynamic vectors

      This class uses a loop to perform binary vector
      operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io}, s_{i})
      \f]
      
      where \f$v_{io}\f$ is the input output vector, and \f$s_{i}\f$
      is the scalar input.  \em op stands for the binary operation
      performed elementwise between \f$v_{io}[index]\f$ and
      \f$s_{i}\f$, and whose result is stored elementwise into
      \f$v_{io}[index]\f$.

      \param _elementOperationType the type of the binary operation

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
        class VioSi {
        public:
        /*! Unroll the loop

        \param inputOutputVector The input output vector (first operand).
        \param inputScalar The input scalar (second operand).
        */
        template<class _inputOutputVectorType, class _inputScalarType>
            static void Run(_inputOutputVectorType & inputOutputVector,
                            const _inputScalarType inputScalar) {

            typedef _inputOutputVectorType InputOutputVectorType;
            typedef typename InputOutputVectorType::pointer InputOutputPointerType;
            typedef typename InputOutputVectorType::size_type size_type;
            typedef typename InputOutputVectorType::stride_type stride_type;

            const size_type size = inputOutputVector.size();

#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputOutputStride = inputOutputVector.stride();

            InputOutputPointerType inputOutputPointer = inputOutputVector.Pointer();
            const InputOutputPointerType inputOutputEnd = inputOutputPointer + size * inputOutputStride;

            for (;
                 inputOutputPointer != inputOutputEnd; 
                 inputOutputPointer += inputOutputStride) {
                _elementOperationType::Operate(*inputOutputPointer, inputScalar);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                _elementOperationType::Operate(inputOutputVector[index], inputScalar);
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }
    };


    /*!  \brief Implement operation of the form \f$v_o = op(v_i)\f$ for
      fixed size vectors
      
      This class uses a loop to perform unary vector operations
      of the form 
      \f[
      v_{o} = \mathrm{op}(v_{i})
      \f]
      
      where \f$v_{o}\f$ is the output vector, and \f$v_{i}\f$ is the
      input vector both of same size, \em op stands for the a unary
      operation performed elementwise on \f$v_{i}\f$, and whose result
      is stored elementwise into \f$v_{o}\f$.

      \param _elementOperationType The type of the unary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class VoVi {
    public:
        /*! Unroll the loop

        \param outputVector The output vector.
        \param inputVector The input vector.
        */
        template<class _outputVectorType, class _inputVectorType>
        static inline void Run(_outputVectorType & outputVector, 
                               const _inputVectorType & inputVector) {
            // check sizes
            typedef _outputVectorType OutputVectorType;
            typedef typename OutputVectorType::pointer OutputPointerType;
            typedef typename OutputVectorType::size_type size_type;
            typedef typename OutputVectorType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;

            const size_type size = outputVector.size();
            if (size != inputVector.size()) {
                ThrowException();
            }
            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type outputStride = outputVector.stride();
            const stride_type inputStride = inputVector.stride();

            OutputPointerType outputPointer = outputVector.Pointer();
            const OutputPointerType outputEnd = outputPointer + size * outputStride;

            InputPointerType inputPointer = inputVector.Pointer();

            for (;
                 outputPointer != outputEnd; 
                 outputPointer += outputStride, 
                     inputPointer += inputStride) {
                *outputPointer = _elementOperationType::Operate(*inputPointer);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                outputVector[index] = _elementOperationType::Operate(inputVector[index]);
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }
    };
    


    /*!  \brief Implement operation of the form \f$v_{io} =
      op(v_{io})\f$ for dynamic vectors
      
      This class uses a loop to perform unary store back vector
      operations of the form

      \f[
      v_{io} = \mathrm{op}(v_{io})
      \f]
      
      where \f$v_{io}\f$ is the input output vector.  \em op stands
      for the unary operation performed elementwise on \f$v_{io}\f$
      and whose result is stored elementwise into \f$v_{io}[index]\f$.

      \param _elementOperationType The type of the unary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _elementOperationType>
    class Vio {
    public:
        /*! Unroll the loop
          
        \param inputOutputVector The input output vector.
        */
        template<class _inputOutputVectorType>
        static inline void Run(_inputOutputVectorType & inputOutputVector) {

            typedef _inputOutputVectorType InputOutputVectorType;
            typedef typename InputOutputVectorType::pointer InputOutputPointerType;
            typedef typename InputOutputVectorType::size_type size_type;
            typedef typename InputOutputVectorType::stride_type stride_type;

            const size_type size = inputOutputVector.size();

#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputOutputStride = inputOutputVector.stride();

            InputOutputPointerType inputOutputPointer = inputOutputVector.Pointer();
            const InputOutputPointerType inputOutputEnd = inputOutputPointer + size * inputOutputStride;

            for (;
                 inputOutputPointer != inputOutputEnd; 
                 inputOutputPointer += inputOutputStride) {
                _elementOperationType::Operate(*inputOutputPointer);
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                _elementOperationType::Operate(inputOutputVector[index]);
            }
#endif // INCREMENTAL_LOOP_ENGINES
        }
    };
    


    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i))\f$ for dynamic vectors

      This class uses a loop to perform incremental
      unary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i))}
      \f]
      
      where \f$v_i\f$ is the input vector and \f$s_o\f$ is the scalar
      output.  \em op stands for the unary operation performed
      elementwise on \f$v_i\f$ and whose result are used incrementally
      as input for \f$op_{incr}\f$.  For a vector of size 3, the
      result is \f$s_o = op_{incr}(op_{incr}(op(v[1]), op(v[0])),
      op(v[2])) \f$.
      
      \param _elementOperationType The type of the unary operation.
      
      \sa vctDynamicVectorLoopEngines
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoVi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        /*! Unroll the loop

        \param inputVector The input vector.
        */
        template<class _inputVectorType>
        static OutputType Run(const _inputVectorType & inputVector) {

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;
            typedef typename InputVectorType::size_type size_type;
            typedef typename InputVectorType::stride_type stride_type;

            const size_type size = inputVector.size();
            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputStride = inputVector.stride();

            InputPointerType inputPointer = inputVector.Pointer();
            const InputPointerType inputEnd = inputPointer + size * inputStride;

            for (;
                 inputPointer != inputEnd; 
                 inputPointer += inputStride) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(*inputPointer));
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(inputVector[index]));
            }
#endif //
            return incrementalResult;
        }
    };


    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_{i1}, v_{i2}))\f$ for dynamic vectors
      
      This class uses a loop to perform incremental
      binary vector operations of the form
      
      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_{i1}, v_{i2}))}
      \f]
      
      where \f$v_{i1}\f$ and \f$v_{i2}\f$ are the input vectors and
      \f$s_o\f$ is the scalar output.  The vectors have the same size,
      \em op stands for the unary operation performed elementwise on
      \f$v_{i1}\f$ and \f$v_{i2}\f$ and whose result are used
      incrementally as input for \f$op_{incr}\f$.  For a vector of
      size 3, the result is \f$s_o = op_{incr}(op_{incr}(op(v1[1],
      v2[1]), op(v1[0], v2[0])), op(v1[2], v2[2]))\f$.

      \param _incrementalOperationType The type of the incremental
      operation.
      
      \param _elementOperationType The type of the unary operation.
      
      \sa vctFixedSizeVectorRecursiveEngines
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoViVi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        /*! Unroll the loop
          
        \param input1Vector The first input vector (first operand for
        _elementOperationType).
        
        \param input2Vector The second input vector (second operand
        for _elementOperationType).
        */
        template<class _input1VectorType, class _input2VectorType>
        static inline OutputType Run(const _input1VectorType & input1Vector,
                                     const _input2VectorType & input2Vector) {
            // check sizes
            typedef _input1VectorType Input1VectorType;
            typedef typename Input1VectorType::const_pointer Input1PointerType;
            typedef typename Input1VectorType::size_type size_type;
            typedef typename Input1VectorType::stride_type stride_type;

            typedef _input2VectorType Input2VectorType;
            typedef typename Input2VectorType::const_pointer Input2PointerType;

            const size_type size = input1Vector.size();
            if (size != input2Vector.size()) {
                ThrowException();
            }

            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

            // otherwise
#if INCREMENTAL_LOOP_ENGINES
            const stride_type input1Stride = input1Vector.stride();
            const stride_type input2Stride = input2Vector.stride();

            Input1PointerType input1Pointer = input1Vector.Pointer();
            const Input1PointerType input1End = input1Pointer + size * input1Stride;

            Input2PointerType input2Pointer = input2Vector.Pointer();

            for (;
                 input1Pointer != input1End;
                 input1Pointer += input1Stride,
                     input2Pointer += input2Stride) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(*input1Pointer,
                                                                                                      *input2Pointer));
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(input1Vector[index],
                                                                                                      input2Vector[index]));
            }
#endif // INCREMENTAL_LOOP_ENGINES
            return incrementalResult;            
        }
    };
    

    /*!  \brief Implement operation of the form \f$v_{io} =
      op_{io}(v_{io}, op_{sv}(s, v_i))\f$ for dynamic vectors
      
      This class uses template specialization to perform store-back
      vector-scalar-vector operations

      \f[
      v_{io} = \mathrm{op_{io}}(V_{io}, \mathrm{op_{sv}}(s, v_i))
      \f]
      
      where \f$v_{io}\f$ is an input-output (store-back) vector;
    \f$s\f$ is a scalar; and \f$v_i\f$ is an input vector.  A
    typical example is: \f$v_{io} += s \cdot v_i\f$.  The vectors
    have a fixed size, determined at compilation time; \f$op_{sv}\f$
    is an operation between \f$s\f$ and the elements of \f$v_i\f$;
    \f$op_{io}\f$ is an operation between the output of
    \f$op_{sv}\f$ and the elements of \f$v_{io}\f$.
      
      \param _ioOperationType The type of the store-back operation.
      
      \param _scalarVectorElementOperationType The type of the
    operation between scalar and input vector.

    */
  template<class _ioElementOperationType, class _scalarVectorElementOperationType>
  class VioSiVi {
  public:
        template<class _ioVectorType, class _inputScalarType, class _inputVectorType>
        static inline void Run(_ioVectorType & ioVector,
    const _inputScalarType inputScalar, const _inputVectorType & inputVector)
    {
            // check sizes
      typedef _ioVectorType IoVectorType;
      typedef typename IoVectorType::pointer IoPointerType;
      typedef typename IoVectorType::size_type size_type;
      typedef typename IoVectorType::stride_type stride_type;

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;

            const size_type size = ioVector.size();
            if (size != inputVector.size()) {
                ThrowException();
            }

            const stride_type ioStride = ioVector.stride();
            const stride_type inputStride = inputVector.stride();

            IoPointerType ioPointer = ioVector.Pointer();
            const IoPointerType ioEnd = ioPointer + size * ioStride;

            InputPointerType inputPointer = inputVector.Pointer();

            for (;
                 ioPointer != ioEnd;
                 ioPointer += ioStride,
                     inputPointer += inputStride)
      {
        _ioElementOperationType::Operate(
          *ioPointer,
          _scalarVectorElementOperationType::Operate(
          inputScalar, *inputPointer)
          );
            }
    }

  };


    /*!  \brief Implement operation of the form \f$s_o =
      op_{incr}(op(v_i, s_i))\f$ for dynamic vectors
      
      This class uses a loop to perform incremental
      binary vector operations of the form

      \f[
      s_o = \mathrm{op_{incr}(\mathrm{op}(v_i, s_i))}
      \f]
      
      where \f$v_i\f$ and \f$s_i\f$ are the input vector and scalar
      and \f$s_o\f$ is the scalar output. \em op stands for the
      unary operation performed elementwise on \f$v_i\f$
      and \f$s_i\f$ and whose result are used incrementally as
      input for \f$op_{incr}\f$.  For a vector of size 3, the result
      is \f$s_o = op_{incr}(op_{incr}(op(v[1], s), op(v[0],
      s)), op(v, s))\f$.
      
      \param _incrementalOperationType The type of the incremental
      operation.
      
      \param _elementOperationType The type of the unary operation.

      \sa vctDynamicVectorLoopEngines
    */
    template<class _incrementalOperationType, class _elementOperationType>
    class SoViSi {
    public:
        typedef typename _incrementalOperationType::OutputType OutputType;
        /*! Unroll the loop.
          
        \param inputVector The input vector (first operand for
        _elementOperationType).
        
        \param inputScalar The input scalar (second operand
        for _elementOperationType).
        */
        template<class _inputVectorType, class _inputScalarType>
        static inline OutputType Run(const _inputVectorType & inputVector,
                                     const _inputScalarType & inputScalar) {

            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;
            typedef typename InputVectorType::size_type size_type;
            typedef typename InputVectorType::stride_type stride_type;

            const size_type size = inputVector.size();
            OutputType incrementalResult = _incrementalOperationType::NeutralElement();

#if INCREMENTAL_LOOP_ENGINES
            const stride_type inputStride = inputVector.stride();

            InputPointerType inputPointer = inputVector.Pointer();
            const InputPointerType inputEnd = inputPointer + size * inputStride;

            for (;
                 inputPointer != inputEnd; 
                 inputPointer += inputStride) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(*inputPointer, inputScalar));
            }
#else // INCREMENTAL_LOOP_ENGINES
            size_type index;
            for (index = 0; index < size; ++index) {
                incrementalResult = _incrementalOperationType::Operate(incrementalResult,
                                                                       _elementOperationType::Operate(inputVector[index], inputScalar));
            }
#endif // INCREMENTAL_LOOP_ENGINES
            return incrementalResult;
        }
    };

    class MinAndMax {
    public:
        /*! Unroll the loop

        \param inputVector The input vector.

        \param minValue The minimum value returned.  Passed by reference.

        \param maxValue The maximum value returned.  Passed by reference.
        */
        template<class _inputVectorType>
        static void Run(const _inputVectorType & inputVector, typename _inputVectorType::value_type & minValue,
                        typename _inputVectorType::value_type & maxValue)
        {
            typedef _inputVectorType InputVectorType;
            typedef typename InputVectorType::const_pointer InputPointerType;
            typedef typename InputVectorType::size_type size_type;
            typedef typename InputVectorType::stride_type stride_type;
            typedef typename InputVectorType::value_type value_type;

            InputPointerType inputPointer = inputVector.Pointer();

            if (inputPointer == 0)
                return;

            const size_type size = inputVector.size();

            const stride_type inputStride = inputVector.stride();

            const InputPointerType inputEnd = inputPointer + size * inputStride;

            value_type minElement, maxElement;
            maxElement = minElement = *inputPointer;

            for (;
                inputPointer != inputEnd; 
                inputPointer += inputStride)
            {
                const value_type element = *inputPointer;
                if (element < minElement) {
                    minElement = element;
                }
                else if (maxElement < element) {
                    maxElement = element;
                }
            }

            minValue = minElement;
            maxValue = maxElement;
        }
    };

};



#endif  // _vctDynamicVectorLoopEngines_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctDynamicVectorLoopEngines.h,v $
// Revision 1.15  2006/06/09 21:53:29  anton
// cisstVector: Fixed typos in Doxygen comments for engines.
//
// Revision 1.14  2006/05/21 11:25:48  ofri
// vct[FixedSize|Dynamic] engines: added a new engine CioSiCi , for operations
// such as MultAdd
//
// Revision 1.13  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.12  2005/12/13 03:18:18  anton
// cisstVector: Minor updates for gcc 4.0
//
// Revision 1.11  2005/12/12 23:51:27  ofri
// vct[FixedSize|Dynamic][Vector|Matrix][Recursive|Loop]Engines:
// Added specialized engine MinAndMax for efficient computation of upper
// and lower bounds of data.
//
// Revision 1.10  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.9  2005/09/23 23:52:09  anton
// vctDynamicVectorLoopEngines: New implementation of SoVi was not using the
// element operation.
//
// Revision 1.8  2005/09/16 18:43:29  anton
// vctDynamicVectorLoopEngines: Implemented the fast operation method
// following ticket #163. The fast method has been added to all the engines in
// this file. It is conditionally compiled through setting the flag
// INCREMENTAL_LOOP_ENGINES. This is so that speed comparison can be done in
// the future.
//
// Revision 1.7  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2005/02/04 16:54:21  anton
// vctDynamicVectorLoopEngines.h: Minor Doxygen typo.
//
// Revision 1.5  2004/12/07 04:12:26  ofri
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
// Revision 1.4  2004/12/01 21:51:48  anton
// cisstVector: Use the NeutralElement() of the incremental operations for
// the dynamic container engines (see #97).  Also rewrote the vector engines
// to use a single index with [] instead of multiple interators.
//
// Revision 1.3  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.2  2004/09/21 20:06:28  anton
// vctDynamicVectorLoopEngines: All engines now rely on method ThrowException
// to throw the exception with a unique message (see dynamic matrix engines).
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
// Revision 1.11  2004/08/12 20:21:48  anton
// cisstVector: Removed push/pop of warning disable for Microsoft compilers
// since the warning 4290 is now disabled in cmnPortability.h
// (see ticket #53 and cvs commit 678)
//
// Revision 1.10  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.9  2004/07/23 18:02:35  anton
// vctDynamic vector and sequences:
// *: Added comparison operators (==, !=, <=, etc) for sequences and scalars
// *: Added Abs, Negation, AbsOf, NegationOf, AbsSelf, NegationSelf methods
// *: Added Add, Subtract, Multiply, Divide for scalars as well as
//    +=, -=, /=, *= for vector and scalar
// *: Added global operators +, -, *, / for vector/vector, vector/scalar,
//    scalar/vector (whenever it was pertinent)
// *: Added loop engined {quote: Vio} and {quote: SoViSi}
//
// Revision 1.8  2004/07/16 20:28:44  anton
// vctSequenceLoopEngines: Added a #pragma warning(push) and corresponding
// pop to disable a warning regarding the exception specification ignored
// by Visual C++ 7 (aka .net 2000)
//
// Revision 1.7  2004/07/16 20:10:14  anton
// vctDynamicSequence: Added VoSiVi and VoViSi engines and methods.
//
// Revision 1.6  2004/07/16 14:35:24  anton
// vctSequenceLoopEngines, Added VoViVi engine.
//
// Revision 1.5  2004/07/15 17:57:29  anton
// cisstVector engines using input scalars rely on copies, not reference
// anymore since the scalar used could be modified by the operation itself
// (see ticket #51).  We decided to use a const copy even if this might be
// at a slight efficiency cost.  This modification has an impact on all engines
// (loop/recursive, sequence/matrix) and we decided to also modify the user API
// (e.g. sequence.RatioOf(const sequence &, const scalar)) so that the internal
// behavior can be deduced from the signature on the method.
//
// Revision 1.4  2004/07/13 01:45:52  ofri
// vctSequenceLoopEngines: changed non-changing `end' iterators to const.
// Added engine VioVi
//
// Revision 1.3  2004/07/12 20:35:33  anton
// vctSequenceLoopEngines.h: Added the VoVi, SoViVi and VioSi engines.  All
// engines which require two or more vectors check the sizes and throw a
// runtime_error exception if the sizes are incorrect.
//
// Revision 1.2  2004/07/02 18:20:30  anton
// Still playing with this class:
// - Now try to handle the empty sequence (if size() == 0).
// - Added a const_iterator end = input.end() so that input.end() is not
// called at each iteration.
//
// Revision 1.1  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
//
// ****************************************************************************

