/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMutualInformationImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2006/03/19 04:36:55 $
  Version:   $Revision: 1.60 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _JointEntropyKNNFixedMultiImageMetric_cxx
#define _JointEntropyKNNFixedMultiImageMetric_cxx

#include "JointEntropyKNNFixedMultiImageMetric.h"


#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"
#include <cmath>

#include <ANN/ANN.h>             // ANN declarations

namespace itk
{

/*
 * Constructor
 */
template < class TFixedImage >
JointEntropyKNNFixedMultiImageMetric < TFixedImage >::
JointEntropyKNNFixedMultiImageMetric()
{

}

/*
 * Destructor
 */
template < class TFixedImage >
JointEntropyKNNFixedMultiImageMetric < TFixedImage >::
~JointEntropyKNNFixedMultiImageMetric()
{
}


/*
 * Initialize
 */
template <class TFixedImage>
void
JointEntropyKNNFixedMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  probMatrix.resize(this->m_NumberOfSpatialSamples/2);
  marginalProb.resize(this->m_NumberOfSpatialSamples/2);
  for(int i=0; i<this->m_NumberOfSpatialSamples/2; i++)
  {
    probMatrix[i].resize(this->m_NumberOfNearestNeigbors);
    for(int j=0; j<this->m_NumberOfNearestNeigbors; j++)
    {
      probMatrix[i][j].set_size(this->m_NumberOfImages);
    }    
  }
}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename JointEntropyKNNFixedMultiImageMetric < TFixedImage >::MeasureType
JointEntropyKNNFixedMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;

  // Prepare the variables for getvalue
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValueHelper, &str);
  this->GetMultiThreader()->SingleMethodExecute();
  
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValue, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  return this->AfterGetThreadedValue();

}

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNFixedMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValue( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValue( threadId );


  return ITK_THREAD_RETURN_VALUE;
}



/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void
JointEntropyKNNFixedMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{

  Superclass::BeforeGetThreadedValue(parameters);

  
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void
JointEntropyKNNFixedMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  const int N = this->m_NumberOfImages;
  const unsigned int size = this->m_NumberOfSpatialSamples;
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;
  /** The tranform parameters vector holding i'th images parameters
  Copy parameters in to a collection of arrays */

  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();


  //Calculate joint entropy
  this->m_value[threadId] = 0.0;
  // Loop over first sample set
  for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i += this->m_NumberOfThreads )
  {

    double probSum = 0.0;
    for( int j=0; j < this->m_NumberOfNearestNeigbors; j++)
    {
      // Evaluate the probability of d(x_a, x_b)
      const double diff =
          (this->m_Sample[x_i].imageValueArray - this->m_Sample[this->nnIdx[x_i-sampleASize][j]].imageValueArray).two_norm();
      probSum += this->m_KernelFunction->Evaluate(  diff / this->m_ImageStandardDeviation );
    }
    probSum /= (double)this->m_NumberOfNearestNeigbors;
    
    this->m_value[threadId] += vcl_log( probSum );

  }  // End of sample loop

}


/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void
JointEntropyKNNFixedMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
   Superclass::BeforeGetThreadedValueAndDerivative(parameters);
  
}

template < class TFixedImage >
void JointEntropyKNNFixedMultiImageMetric < TFixedImage >
::GetValueAndDerivative(const ParametersType & parameters,
                          MeasureType & value,
                          DerivativeType & derivative) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValueAndDerivative(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;

  // Compute the derivatives
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValueAndDerivativeHelper, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValueAndDerivative, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  this->AfterGetThreadedValueAndDerivative(value, derivative);

}
/*
 * Get the match Measure
 */
template < class TFixedImage >
void
JointEntropyKNNFixedMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  double N = (double) this->m_NumberOfImages;
  int numberOfThreads = this->GetNumberOfThreads();
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;

  
  /** The tranform parameters vector holding i'th images parameters
  Copy parameters in to a collection of arrays */
  MeasureType value = NumericTraits < MeasureType >::Zero;

  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  //Initialize the derivative to zero
  this->m_derivativeArray[threadId].Fill(0.0);
  this->m_value[threadId] = 0.0;
  Array<double> weight(this->m_NumberOfImages);


  // Calculate the derivative
  for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i+= this->m_NumberOfThreads )
  {
    
    // Sum over the second sample set
    weight.Fill(0.0);
    for (int j=0; j<this->m_NumberOfNearestNeigbors; j++ )
    {

      const unsigned int x_j = this->nnIdx[x_i-sampleASize][j];

      weight += probMatrix[x_i-sampleASize][j] * ( 1.0 / marginalProb[x_i-sampleASize] );//+ 1.0 / marginalProb[x_i] );
    }

    //Calculate joint entropy
    this->m_value[threadId] += vcl_log( marginalProb[x_i-sampleASize] / (double)this->m_NumberOfNearestNeigbors );

    for( int l=0; l<this->m_NumberOfImages; l++ )
    {

      // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][l], this->m_Sample[x_i], weight[l], l, threadId);

    }  // End of sample loop B
  }

  // Second term
  for (int j=0; j<this->m_NumberOfNearestNeigbors; j++ )
  {

    // Sum over the second sample set
    for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i+= this->m_NumberOfThreads )
    {
      
      const unsigned int x_j = this->nnIdx[x_i-sampleASize][j];

      this->weigtsArray[x_j] += probMatrix[x_i-sampleASize][j] / marginalProb[x_i-sampleASize];
    }
  }

  for(int x_j=0; x_j<sampleASize; x_j++)
  {
    for( int l=0; l<this->m_NumberOfImages; l++ )
    {
      // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][l], this->m_Sample[x_j], this->weigtsArray[x_j][l], l, threadId);
    }  // End of sample loop B
  }
  
  this->m_value[threadId] /= -1.0 * (double) sampleBSize;
}


// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNFixedMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivative( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValueAndDerivative( threadId );


  return ITK_THREAD_RETURN_VALUE;
}

// Callback routine used by the threading library. This routine just calls
// the ComputeDerivative() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNFixedMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivativeHelper( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetValueAndDerivativeHelper( threadId );


  return ITK_THREAD_RETURN_VALUE;
}

/*
 * Compute the derivatives before getthreadedvalueandDerivative
 */
template < class TFixedImage >
void
JointEntropyKNNFixedMultiImageMetric < TFixedImage >
::GetValueAndDerivativeHelper(int threadId) const
{

  double N = (double) this->m_NumberOfImages;
  const int numberOfThreads = this->GetNumberOfThreads();
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;
  
  // Computs the probability matrix
  for(int x_i=sampleASize+threadId; x_i<sampleASize*2; x_i+=numberOfThreads)
  {
    marginalProb[x_i-sampleASize] = 0.0;
    for (int j=0; j<this->m_NumberOfNearestNeigbors; j++ )
    {

      const unsigned int x_j = this->nnIdx[x_i-sampleASize][j];

      // Compute d(x_i, x_j)
      probMatrix[x_i-sampleASize][j] =
          this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray;

      // Compute G(d(x_i,x_j))
      const double G = this->m_KernelFunction->Evaluate( probMatrix[x_i-sampleASize][j].two_norm()
                        / this->m_ImageStandardDeviation );

      probMatrix[x_i-sampleASize][j] *= G;
      marginalProb[x_i-sampleASize] += G;
    }

  }
  
  for (int i=threadId; i<sampleASize; i+=numberOfThreads )
  {
    this->weigtsArray[i].Fill(0.0);
  }

}


// Callback routine used by the threading library. This routine just calls
// the ComputeDerivative() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyKNNFixedMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueHelper( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetValueHelper( threadId );


  return ITK_THREAD_RETURN_VALUE;
}
/*
 * Update the intensities before getthreadedvalue
 */
template < class TFixedImage >
void
JointEntropyKNNFixedMultiImageMetric < TFixedImage >
::GetValueHelper(int threadId) const
{

  // Update intensity values
  const int N = this->m_NumberOfImages;
  const unsigned int size = this->m_NumberOfSpatialSamples;
  for(int i=threadId; i< size; i+=this->m_NumberOfThreads )
  {
    for(int j=0; j<N; j++)
    {
      const MovingImagePointType mappedPoint = this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }

  }
  

  
}

}        // end namespace itk


#endif
