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
#ifndef _RegisterToMeanAndVarianceMultiImageMetric_cxx
#define _RegisterToMeanAndVarianceMultiImageMetric_cxx

#include "RegisterToMeanAndVarianceMultiImageMetric.h"


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
RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >::
RegisterToMeanAndVarianceMultiImageMetric()
{


}


/*
 * Initialize
 */
template <class TFixedImage> 
void
RegisterToMeanAndVarianceMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

  // Number of Spatial Samples must be an even number
  // First half is the set A and second half is the set B
  this->m_NumberOfSpatialSamples -= this->m_NumberOfSpatialSamples%2;

  // Resize w_x_j
  W.resize(this->m_NumberOfThreads);
  for(int i=0; i<this->m_NumberOfThreads; i++)
  {
    W[i].resize(this->m_NumberOfSpatialSamples);
  }
  mean.resize(this->m_NumberOfSpatialSamples);
  variance.resize(this->m_NumberOfSpatialSamples);

  m_MeanStandardDeviation = this->m_ImageStandardDeviation / sqrt(this->m_NumberOfImages);
  m_VarianceStandardDeviation = this->m_ImageStandardDeviation * sqrt(2.0 * this->m_NumberOfImages);

}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >::MeasureType
RegisterToMeanAndVarianceMultiImageMetric <TFixedImage >::
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
RegisterToMeanAndVarianceMultiImageMetric< TFixedImage >
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
RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{

  Superclass::BeforeGetThreadedValue(parameters);

  // Update intensity values
  const int N = this->m_NumberOfImages;
  const unsigned int size = this->m_NumberOfSpatialSamples;
  for(int i=0; i< size; i++ )
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
  
  for(int i=0; i< size; i++ )
  {
    mean[i] = 0.0;
    variance[i] = 0.0;
    for(int j=0; j<N; j++)
    {
      const double sample = this->m_Sample[i].imageValueArray[j];
      mean[i] += sample;
      variance[i] += sample*sample;
    }
    mean[i] /= (double) N;
    variance[i] /= (double) N;
    variance[i] -= mean[i]*mean[i];
  }
  
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void
RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  const int N = this->m_NumberOfImages;
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;
  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */

  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  //Calculate the entropy
  this->m_value[threadId] = 0.0;
  // Loop over first sample set
  for( int l=0; l < N; l++)
  {
    for (int x_i=sampleASize+threadId; x_i<sampleASize+sampleBSize; x_i += this->m_NumberOfThreads )
    {

      double probSum = 0.0;
      for( int x_j=0; x_j < sampleASize; x_j++)
      {

        // Compute d(x_i, x_j)
        const double diff1 = (this->m_Sample[x_i].imageValueArray[l] - this->m_Sample[x_j].imageValueArray[l])
                              /this->m_ImageStandardDeviation;
        const double diff2 = (mean[x_i] - mean[x_j]) / m_MeanStandardDeviation;
        const double diff3 = (variance[x_i] - variance[x_j]) / m_VarianceStandardDeviation;

        // Evaluate the probability of d(x_a, x_b)
        probSum += this->m_KernelFunction->Evaluate
            ( sqrt( diff1*diff1  + diff2*diff2 + diff3*diff3) );
      }
      probSum /= (double)sampleASize;
    
      this->m_value[threadId] += vcl_log( probSum );


    }  // End of sample loop
  }

}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >::MeasureType
RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
  }
  value /= (double)( -0.5 * this->m_NumberOfSpatialSamples * this->m_NumberOfImages );

  return value;
}

/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void 
RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
   Superclass::BeforeGetThreadedValueAndDerivative(parameters);


   // Compute W(x_j)
   const unsigned int sampleSize = this->m_NumberOfSpatialSamples / 2;
   const unsigned int size = this->m_NumberOfSpatialSamples ;
   const unsigned int N = this->m_NumberOfImages;

   // Update mean
   for(unsigned int i=0; i < size; i++)
   {
     mean[i] = 0.0;
     variance[i] = 0.0;
     for( int j=0; j< N; j++)
     {
       const double sample = this->m_Sample[i].imageValueArray[j];
       mean[i] += sample;
       variance[i] += sample*sample;
     }
     mean[i] /= (double) N;
     variance[i] /= (double) N;
     variance[i] -= mean[i]*mean[i];

   }

}

template < class TFixedImage >
void RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >
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
RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  const unsigned int N = this->m_NumberOfImages;
  int numberOfThreads = this->GetNumberOfThreads();
  const unsigned int sampleASize = this->m_NumberOfSpatialSamples / 2;
  const unsigned int sampleBSize = this->m_NumberOfSpatialSamples / 2;
  
  MeasureType value = NumericTraits < MeasureType >::Zero;

  //Initialize the derivative array to zero
  for(int i=0; i<this->m_NumberOfImages;i++)
  {
    this->m_DerivativesArray[threadId][i].Fill(0.0);
  }
  this->m_value[threadId] = 0.0;

  
  // Calculate the derivative
  // Set the image for derivatives
  for( int m=threadId; m < N; m += this->m_NumberOfThreads)
  {
    
    for (int x_i=sampleASize; x_i<sampleASize+sampleBSize; x_i++ )
    {

        // Sum over the second sample set
      double weight = 0.0;
      W[threadId][x_i] = 0.0;
      for (int x_j=0; x_j<sampleASize; x_j++ )
      {

          // Compute d(x_i, x_j)
        const double diff1 = (this->m_Sample[x_i].imageValueArray[m] - this->m_Sample[x_j].imageValueArray[m])
                              /this->m_ImageStandardDeviation;
        const double diff2 = (mean[x_i] - mean[x_j]) / m_MeanStandardDeviation;
        const double diff3 = (variance[x_i] - variance[x_j]) / m_VarianceStandardDeviation;

          // Compute G(d(x_i,x_j))
        const double G = this->m_KernelFunction->Evaluate( sqrt( diff1*diff1  + diff2*diff2 + diff3*diff3 ) );
        const double dir = diff1 / this->m_ImageStandardDeviation + diff2 / m_MeanStandardDeviation / (double) N +
                           diff3 / m_VarianceStandardDeviation * 2.0 / (double) N
                                 * (this->m_Sample[x_i].imageValueArray[m] - mean[x_i]);

        W[threadId][x_i] += G;
        weight += G * dir;
      }

      weight /= W[threadId][x_i];
      this->m_value[threadId] += vcl_log( W[threadId][x_i] / (double)sampleASize );
        
        // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][m], this->m_Sample[x_i], weight, m, threadId);
        
    }  // End of sample loop B


    // Loop over the samples A
    for (int x_i=0; x_i<sampleASize; x_i++ )
    {

        // Sum over the second sample set
      double weight = 0.0;
      for (int x_j=sampleASize; x_j<sampleASize+sampleBSize; x_j++ )
      {

          // Compute d(x_i, x_j)
        const double diff1 = (this->m_Sample[x_i].imageValueArray[m] - this->m_Sample[x_j].imageValueArray[m])
                              /this->m_ImageStandardDeviation;
        const double diff2 = (mean[x_i] - mean[x_j]) / m_MeanStandardDeviation;
        const double diff3 = (variance[x_i] - variance[x_j]) / m_VarianceStandardDeviation;

          // Compute G(d(x_i,x_j))
        const double G = this->m_KernelFunction->Evaluate( sqrt( diff1*diff1  + diff2*diff2 + diff3*diff3 ) );
          
        const double dir = diff1 / this->m_ImageStandardDeviation + diff2 / m_MeanStandardDeviation / (double) N +
                           diff3 / m_VarianceStandardDeviation * 2.0 / (double) N
                                 * (this->m_Sample[x_i].imageValueArray[m] - mean[x_i]);
        
        weight += G * dir / W[threadId][x_j];
        
      }

        // Get the derivative for this sample
      this->UpdateSingleImageParameters( this->m_DerivativesArray[threadId][m], this->m_Sample[x_i], weight, m, threadId);
        
    }  // End of sample loop B

    
    
  }
}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void RegisterToMeanAndVarianceMultiImageMetric < TFixedImage >
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
  value /= -0.5 * (double) this->m_NumberOfSpatialSamples * (double) this->m_NumberOfImages;
  derivative /=(double) (this->m_NumberOfSpatialSamples / 2.0 * this->m_NumberOfImages);

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
RegisterToMeanAndVarianceMultiImageMetric< TFixedImage >
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
