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
#ifndef _JointEntropyFixedMultiImageMetric_cxx
#define _JointEntropyFixedMultiImageMetric_cxx

#include "JointEntropyFixedMultiImageMetric.h"


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
JointEntropyFixedMultiImageMetric < TFixedImage >::
JointEntropyFixedMultiImageMetric()
{

}

/*
 * Destructor
 */
template < class TFixedImage >
JointEntropyFixedMultiImageMetric < TFixedImage >::
~JointEntropyFixedMultiImageMetric()
{
}


/*
 * Initialize
 */
template <class TFixedImage>
void
JointEntropyFixedMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  // Number of Spatial Samples must be an even number
  // First half is the set A and second half is the set B
  this->m_NumberOfSpatialSamples -= this->m_NumberOfSpatialSamples%2;

}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename JointEntropyFixedMultiImageMetric < TFixedImage >::MeasureType
JointEntropyFixedMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;

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
JointEntropyFixedMultiImageMetric< TFixedImage >
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
JointEntropyFixedMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{

  Superclass::BeforeGetThreadedValue(parameters);

  // Update intensity values
  for(int i=0; i< this->m_NumberOfSpatialSamples; i++ )
  {
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      this->m_Sample[i].mappedPointsArray[j] =
          this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (this->m_Sample[i].mappedPointsArray[j]) )
      {
        // Get the intensity value
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(this->m_Sample[i].mappedPointsArray[j]);

      }
    }
  }
  
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void
JointEntropyFixedMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  const int N = this->m_NumberOfImages;
  const unsigned int size = this->m_NumberOfSpatialSamples;

  //Calculate joint entropy
  this->m_value[threadId] = 0.0;
  // Loop over first sample set
  for (int x_i=threadId; x_i<size; x_i += this->m_NumberOfThreads )
  {

    double probSum = 0.0;
    for( int x_j=0; x_j < size; x_j++)
    {
      // Evaluate the probability of d(x_a, x_b)
      const double diff =
          (this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray).two_norm();
      probSum += this->m_KernelFunction->Evaluate(  diff / this->m_ImageStandardDeviation );
    }
    probSum /= (double)size;
    
    this->m_value[threadId] += vcl_log( probSum );

  }  // End of sample loop

}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename JointEntropyFixedMultiImageMetric < TFixedImage >::MeasureType
JointEntropyFixedMultiImageMetric < TFixedImage >
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
JointEntropyFixedMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
   Superclass::BeforeGetThreadedValueAndDerivative(parameters);
}

template < class TFixedImage >
void JointEntropyFixedMultiImageMetric < TFixedImage >
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
JointEntropyFixedMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  double N = (double) this->m_NumberOfImages;

  
  MeasureType value = NumericTraits < MeasureType >::Zero;
  
  //Initialize the derivative array to zero
  for(int i=0; i<this->m_NumberOfImages;i++)
  {
    this->m_DerivativesArray[threadId][i].Fill(0.0);
  }
  this->m_value[threadId] = 0.0;
  
  Array<double> weight(this->m_NumberOfImages);

  // Calculate the derivative
  for (int x_i=threadId; x_i<this->m_NumberOfSpatialSamples; x_i+= this->m_NumberOfThreads )
  {
    
    // Compute W(x_j)
    double Wsum = 0.0;
    // Sum over the second sample set
    weight.Fill(0.0);
    for (int x_j=0; x_j<this->m_NumberOfSpatialSamples; x_j++ )
    {

      // Compute d(x_i, x_j)
      const vnl_vector<double> diff =
          this->m_Sample[x_i].imageValueArray - this->m_Sample[x_j].imageValueArray;

      // Compute G(d(x_i,x_j))
      const double G = this->m_KernelFunction->Evaluate( diff.two_norm() / this->m_ImageStandardDeviation );

      Wsum += G;
      weight += G * diff;
    }

    //Calculate joint entropy
    this->m_value[threadId] += vcl_log( Wsum / (double)this->m_NumberOfSpatialSamples );
    weight /= Wsum;

    for( int l=0; l<this->m_NumberOfImages; l++ )
    {

      // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][l], this->m_Sample[x_i], weight[l], l, threadId);
    }  // End of sample loop B
  }


}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void JointEntropyFixedMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{


  value = NumericTraits< RealType >::Zero;

  derivative.set_size(this->numberOfParameters * this->m_NumberOfImages);
  derivative.Fill (0.0);

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      for(int k=0; k<this->numberOfParameters; k++)
      {
        derivative[j * this->numberOfParameters + k] += this->m_DerivativesArray[i][j][k];
      }
    }
  }
  value /= -0.5 * (double) this->m_NumberOfSpatialSamples;
  derivative /=(double) (this->m_NumberOfSpatialSamples / 2.0 /
      this->m_ImageStandardDeviation / this->m_ImageStandardDeviation );

  //Set the mean to zero
  //Remove mean
  DerivativeType sum (this->numberOfParameters);
  sum.Fill(0.0);
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      sum[j] += derivative[i * this->numberOfParameters + j];
    }
  }

  
  for (int i = 0; i < this->m_NumberOfImages * this->numberOfParameters; i++)
  {
    derivative[i] -= sum[i % this->numberOfParameters] / (double) this->m_NumberOfImages;
  }
  
} 

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
JointEntropyFixedMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivative( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValueAndDerivative( threadId );


  return ITK_THREAD_RETURN_VALUE;
}



}        // end namespace itk


#endif
