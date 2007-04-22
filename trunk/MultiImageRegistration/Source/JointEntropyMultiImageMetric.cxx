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
#ifndef _JointEntropyMultiImageMetric_cxx
#define _JointEntropyMultiImageMetric_cxx

#include "JointEntropyMultiImageMetric.h"


#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"
#include <cmath>

namespace itk
{

/*
 * Constructor
 */
template < class TFixedImage >
JointEntropyMultiImageMetric < TFixedImage >::
JointEntropyMultiImageMetric()
{


}


/*
 * Initialize
 */
template <class TFixedImage>
void
JointEntropyMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  // Number of Spatial Samples must be an even number
  // First half is the set A and second half is the set B
  this->m_NumberOfSpatialSamples -= this->m_NumberOfSpatialSamples%2;

  // Resize w_x_j
  W_x_j.resize(this->m_NumberOfSpatialSamples);

  // Resize derivative container
  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();
  m_DerivativeArray.resize(this->m_NumberOfImages);
  for(int l=0; l<this->m_NumberOfImages; l++)
  {
    m_DerivativeArray[l].resize(this->m_NumberOfSpatialSamples);
    for(int i=0; i<this->m_NumberOfSpatialSamples; i++)
    {
      m_DerivativeArray[l][i].set_size(numberOfParameters);
    }
  }


}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename JointEntropyMultiImageMetric < TFixedImage >::MeasureType
JointEntropyMultiImageMetric <TFixedImage >::
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
JointEntropyMultiImageMetric< TFixedImage >
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
JointEntropyMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{

  Superclass::BeforeGetThreadedValue(parameters);
  
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void
JointEntropyMultiImageMetric < TFixedImage >
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
  double logSum = 0.0;
  // Loop over first sample set
  for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i += this->m_NumberOfThreads )
  {

    double probSum = 0.0;
    for( int x_j=0; x_j < sampleASize; x_j++)
    {
      // Evaluate the probability of d(x_a, x_b)
      const double dist = (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray).two_norm(); 
      probSum += this->m_KernelFunction->Evaluate(  dist / this->m_ImageStandardDeviation );
    }
    probSum /= (double)sampleASize;
    
    this->m_value[threadId] += vcl_log( probSum );


  }  // End of sample loop

}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename JointEntropyMultiImageMetric < TFixedImage >::MeasureType
JointEntropyMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
  }
  value /= (double)( -0.5 * this->m_NumberOfSpatialSamples );

  return value;
}

/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void
JointEntropyMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
   Superclass::BeforeGetThreadedValueAndDerivative(parameters);
}

template < class TFixedImage >
void JointEntropyMultiImageMetric < TFixedImage >
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
JointEntropyMultiImageMetric < TFixedImage >
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
  DerivativeType deriv(numberOfParameters);
  Array<double> weight(this->m_NumberOfImages);
  

  // Calculate the derivative
  for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i+= this->m_NumberOfThreads )
  {
    
    // Compute W(x_j)
    W_x_j[x_i] = 0.0;
    // Sum over the second sample set
    weight.Fill(0.0);
    for (int x_j=0; x_j<sampleASize; x_j++ )
    {

      // Compute d(x_i, x_j)
      const double diff =
                   (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray).two_norm();

      // Compute G(d(x_i,x_j))
      const double G = this->m_KernelFunction->Evaluate( diff / this->m_ImageStandardDeviation );

      W_x_j[x_i] += G;
      weight += G* (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray);
    }

    //Calculate joint entropy
    this->m_value[threadId] += vcl_log( W_x_j[x_i] / (double)sampleASize );
    weight /= W_x_j[x_i];

    for( int l=0; l<this->m_NumberOfImages; l++ )
    {

      // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][l], this->m_Sample[x_i], weight[l], l, threadId);
    }  // End of sample loop B
  }

  for (int x_j=0; x_j<sampleASize; x_j++ )
  {

    // Sum over the second sample set
    weight.Fill(0.0);
    for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i+= this->m_NumberOfThreads )    
    {

      // Compute d(x_i, x_j)
      const double diff =
          (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray).two_norm();

      // Compute G(d(x_i,x_j))
      const double G = this->m_KernelFunction->Evaluate( diff / this->m_ImageStandardDeviation );

      weight += G / W_x_j[x_i] *
               (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray);
    }

    for( int l=0; l<this->m_NumberOfImages; l++ )
    {
      // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][l], this->m_Sample[x_j], weight[l], l, threadId);
    }  // End of sample loop B
  }

  this->m_value[threadId] /= -1.0 * (double) sampleBSize;
}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void JointEntropyMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{


  const int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters();

  // Sum over the values returned by threads
  value = this->m_value[0];
  derivative = this->m_derivativeArray[0];
  for( unsigned int i=1; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
    derivative += this->m_derivativeArray[i];
  }
  derivative /= (double) (this->m_NumberOfSpatialSamples / 2.0 * this->m_ImageStandardDeviation * this->m_ImageStandardDeviation);

  //Set the mean to zero
  //Remove mean
  DerivativeType temp(numberOfParameters);
  temp.Fill(0.0);
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    for (int j = 0; j < numberOfParameters; j++)
    {
      temp[j] += derivative[i * numberOfParameters + j];
    }
  }

  
  for (int i = 0; i < this->m_NumberOfImages * numberOfParameters; i++)
  {
    derivative[i] -= temp[i % numberOfParameters] / (double) this->m_NumberOfImages;
  }

}

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyMultiImageMetric< TFixedImage >
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
JointEntropyMultiImageMetric< TFixedImage >
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
JointEntropyMultiImageMetric < TFixedImage >
::GetValueAndDerivativeHelper(int threadId) const
{

  
}


// Callback routine used by the threading library. This routine just calls
// the ComputeDerivative() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyMultiImageMetric< TFixedImage >
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
JointEntropyMultiImageMetric < TFixedImage >
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
