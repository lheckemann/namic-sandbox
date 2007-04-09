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
  for (int b=sampleASize+threadId; b<sampleASize+sampleBSize; b += this->m_NumberOfThreads )
  {

    double probSum = 0.0;
    for( int a=0; a < sampleASize; a++)
    {

      // Compute d(x_a, x_b)
      double sumOfSquares = 0.0;
      for(int i=0; i<N; i++)
      {
        const double diff = this->m_Sample[a].imageValueArray[i] - this->m_Sample[b].imageValueArray[i];
        sumOfSquares += diff*diff;
      }

      // Evaluate the probability of d(x_a, x_b)
      probSum += this->m_KernelFunction->Evaluate(sqrt(sumOfSquares) / this->m_ImageStandardDeviation );
    }
    probSum /= sampleASize;
    
    this->m_value[threadId] += vcl_log( probSum );


  }  // End of sample loop

  this->m_value[threadId] /= -1.0 * sampleBSize;

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

   // Compute W(x_j)
   const unsigned int sampleSize = this->m_NumberOfSpatialSamples / 2;
   const unsigned int N = this->m_NumberOfImages;
   // Loop over set B
   for(unsigned int b=sampleSize; b< sampleSize*2; b++)
   {
     W_x_j[b] = 0.0;
     //Loop over set A
     for( unsigned int a=0; a<sampleSize; a++)
     {
       // Compute d(x(a),x(b))
       double sumOfSquares = 0.0;
       for(int m=0; m<N; m++)
       {
         const double diff = this->m_Sample[b].imageValueArray[m] - this->m_Sample[a].imageValueArray[m];
         sumOfSquares += diff*diff;
       }
       
       W_x_j[b] += this->m_KernelFunction->Evaluate( sqrt(sumOfSquares) / this->m_ImageStandardDeviation );
     }
   }

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

  
  //Calculate joint entropy
  double logSum = 0.0;
  // Loop over first sample set
  for (int b=sampleASize+threadId; b<sampleASize+sampleBSize; b += this->m_NumberOfThreads )
  {

    double probSum = 0.0;
    for( int a=0; a < sampleASize; a++)
    {

      // Compute d(x_a, x_b)
      double sumOfSquares = 0.0;
      for(int i=0; i<N; i++)
      {
        const double diff = this->m_Sample[a].imageValueArray[i] - this->m_Sample[b].imageValueArray[i];
        sumOfSquares += diff*diff;
      }

      // Evaluate the probability of d(x_a, x_b)
      probSum += this->m_KernelFunction->Evaluate(sqrt(sumOfSquares) / this->m_ImageStandardDeviation );
    }
    probSum /= sampleASize;
    
    this->m_value[threadId] += vcl_log( probSum );


  }  // End of sample loop

  this->m_value[threadId] /= -1.0 * sampleBSize;


  // Calculate the derivative
  for( int l=threadId; l<N; l += this->m_NumberOfThreads )
  {
    // Set the image for derivatives
    this->m_DerivativeCalcVector[threadId]->SetInputImage(this->m_ImageArray[l]);
    
    for (int i=sampleASize; i<sampleASize+sampleBSize; i++ )
    {

      // Sum over the second sample set
      double weight = 0.0;
      for (int j=0; i<sampleASize; j++ )
      {

        // Compute d(x_i, x_j)
        double sumOfSquares = 0.0;
        for(int m=0; m<N; m++)
        {
          const double diff = this->m_Sample[i].imageValueArray[m] - this->m_Sample[j].imageValueArray[m];
          sumOfSquares += diff*diff;
        }

        // Compute G(d(x_i,x_j))
        const double G = this->m_KernelFunction->Evaluate( sqrt(sumOfSquares) / this->m_ImageStandardDeviation );

        weight += G* (this->m_Sample[i].imageValueArray[l] - this->m_Sample[j].imageValueArray[l]);
      }

      weight /= W_x_j[i];

      // Compute the gradient
      this->CalculateDerivatives(this->m_Sample[i].FixedImagePoint, deriv, l, threadId);

      //Copy the proper part of the derivative
      for (int n = 0; n < numberOfParameters; n++)
      {
        this->m_derivativeArray[threadId][l * numberOfParameters + n] += weight*deriv[n];
      }
    }  // End of sample loop B


    
    // Loop over the samples A
    for (int i=0; i<sampleASize; i++ )
    {

      // Sum over the second sample set
      double weight = 0.0;
      for (int j=sampleASize; j<sampleASize+sampleBSize; j++ )
      {

        // Compute d(x_i, x_j)
        double sumOfSquares = 0.0;
        for(int m=0; m<N; m++)
        {
          const double diff = this->m_Sample[i].imageValueArray[m] - this->m_Sample[j].imageValueArray[m];
          sumOfSquares += diff*diff;
        }

        // Compute G(d(x_i,x_j))
        const double G = this->m_KernelFunction->Evaluate( sqrt(sumOfSquares) / this->m_ImageStandardDeviation );

        weight += G * (this->m_Sample[i].imageValueArray[l] - this->m_Sample[j].imageValueArray[l]) / W_x_j[j];
        
      }

      // Compute the gradient
      this->CalculateDerivatives(this->m_Sample[i].FixedImagePoint, deriv, l, threadId);

      //Copy the proper part of the derivative
      for (int n = 0; n < numberOfParameters; n++)
      {
        this->m_derivativeArray[threadId][l * numberOfParameters + n] += weight*deriv[n];
      }
    }  // End of sample loop B
    
  }
}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void JointEntropyMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{

  value = NumericTraits< RealType >::Zero;

  const int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters();
  DerivativeType temp (numberOfParameters * this->m_NumberOfImages);
  temp.Fill (0.0);

  // Sum over the values returned by threads
  for( unsigned int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
    temp += this->m_derivativeArray[i];
  }
  temp /= (double) this->m_NumberOfSpatialSamples / 2 * this->m_ImageStandardDeviation * this->m_ImageStandardDeviation;
  derivative = temp;
  
  //Set the mean to zero
  //Remove mean
  temp.SetSize(numberOfParameters);
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

}        // end namespace itk


#endif
