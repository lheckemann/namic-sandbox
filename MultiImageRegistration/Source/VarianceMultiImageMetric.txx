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
#ifndef _VarianceMultiImageMetric_cxx
#define _VarianceMultiImageMetric_cxx

#include "VarianceMultiImageMetric.h"

#include "vnl/vnl_math.h"
#include <cmath>

namespace itk
{

/*
 * Constructor
 */
template < class TImage >
VarianceMultiImageMetric < TImage >::
VarianceMultiImageMetric()
{
}


/*
 * Initialize
 */
template <class TImage> 
void
VarianceMultiImageMetric<TImage>
::Initialize(void) throw ( ExceptionObject )
{
  //First intialize the superclass
  Superclass::Initialize();
}

/*
 * Get the match Measure
 */
template < class TImage >
typename VarianceMultiImageMetric < TImage >::MeasureType
VarianceMultiImageMetric <TImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;


  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallbackGetValue, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  return this->AfterGetThreadedValue();

}

template < class TImage >
ITK_THREAD_RETURN_TYPE
VarianceMultiImageMetric< TImage >
::ThreaderCallbackGetValue( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValue( threadId );

  return ITK_THREAD_RETURN_VALUE;
}

/*
 * Get the match Measure
 */
template < class TImage >
void
VarianceMultiImageMetric < TImage >
::GetThreadedValue(int threadId) const
{


  // Update intensity values
  ImagePointType mappedPoint;
  for(unsigned long int i=threadId; i< this->m_NumberOfSpatialSamples; i += this->m_NumberOfThreads )
  {
    for(unsigned int j=0; j<this->m_NumberOfImages; j++)
    {
      mappedPoint = this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }
  }

  //Calculate metric value
  this->m_value[threadId] = 0.0;
  double mean;
  double sumOfSquares;
  
  // Sum over spatial samples
  for (int a=threadId; a<this->m_NumberOfSpatialSamples; a += this->m_NumberOfThreads )
  {
    sumOfSquares = 0.0;
    mean = 0.0;
    // Sum over images
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      const double currentValue = this->m_Sample[a].imageValueArray[j];
      sumOfSquares += currentValue*currentValue;
      mean += currentValue;
    }
    mean /= (double) this->m_NumberOfImages;
    sumOfSquares /= (double) this->m_NumberOfImages;
    this->m_value[threadId] += sumOfSquares - mean*mean;

  } // End of sample Loop  

}

template < class TImage >
typename ParzenWindowEntropyMultiImageMetric < TImage >::MeasureType
VarianceMultiImageMetric < TImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  // Sum over the values returned by threads
  for( int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
  }
  value /= (double) this->m_NumberOfSpatialSamples * this->m_NumberOfImages;
  
  return value;
}


template < class TImage >
ITK_THREAD_RETURN_TYPE
VarianceMultiImageMetric< TImage >
::ThreaderCallbackGetValueAndDerivative( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValueAndDerivative( threadId );


  return ITK_THREAD_RETURN_VALUE;
}


template < class TImage >
void VarianceMultiImageMetric < TImage >
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
template < class TImage >
void 
VarianceMultiImageMetric < TImage >
::GetThreadedValueAndDerivative(int threadId) const
{


  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */
  MeasureType value = NumericTraits < MeasureType >::Zero;


  //Initialize the derivative array to zero
  for(int i=0; i<this->m_NumberOfImages;i++)
  {
    this->m_DerivativesArray[threadId][i].Fill(0.0);
  }

  
  //Calculate metric value
  this->m_value[threadId] = 0.0;
  double mean;
  double sumOfSquares;
  
  // Sum over spatial samples
  for (int a=threadId; a<this->m_NumberOfSpatialSamples; a += this->m_NumberOfThreads )
  {
    sumOfSquares = 0.0;
    mean = 0.0;
    // Sum over images
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      const double currentValue = this->m_Sample[a].imageValueArray[j];
      sumOfSquares += currentValue*currentValue;
      mean += currentValue;
    }
    mean /= (double) this->m_NumberOfImages;
    sumOfSquares /= (double) this->m_NumberOfImages;
    this->m_value[threadId] += sumOfSquares - mean*mean;

    // Calculate derivative
    for (int i = 0; i < this->m_NumberOfImages; i++)
    {
      //calculate the derivative weight
      const double weight = 2.0 / (double) this->m_NumberOfImages * 
                      (this->m_Sample[a].imageValueArray[i] - mean);
      
      // Get the derivative for this sample
      UpdateSingleImageParameters( this->m_DerivativesArray[threadId][i], this->m_Sample[a], weight, i, threadId);
    }
  } // End of sample Loop

}

/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TImage >
void VarianceMultiImageMetric < TImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
                           DerivativeType & derivative) const
{

  value = NumericTraits< RealType >::Zero;

  derivative.set_size(this->numberOfParameters * this->m_NumberOfImages);
  derivative.Fill (0.0);

  // Sum over the values returned by threads
  for( int i=0; i < this->m_NumberOfThreads; i++ )
  {
    value += this->m_value[i];
    for(unsigned int j=0; j<this->m_NumberOfImages; j++)
    {
      for(unsigned int k=0; k<this->numberOfParameters; k++)
      {
        derivative[j * this->numberOfParameters + k] += this->m_DerivativesArray[i][j][k]; 
      }
    }
  }
  value /= (double) this->m_NumberOfSpatialSamples * this->m_NumberOfImages;
  derivative /= (double) this->m_NumberOfSpatialSamples * this->m_NumberOfImages;

  //Set the mean to zero
  //Remove mean
  DerivativeType sum (this->numberOfParameters);
  sum.Fill(0.0);
  for (unsigned int i = 0; i < this->m_NumberOfImages; i++)
  {
    for (unsigned int j = 0; j < this->numberOfParameters; j++)
    {
      sum[j] += derivative[i * this->numberOfParameters + j];
    }
  }

  
  for (unsigned int i = 0; i < this->m_NumberOfImages * this->numberOfParameters; i++)
  {
    derivative[i] -= sum[i % this->numberOfParameters] / (double) this->m_NumberOfImages;
  }

}

}        // end namespace itk


#endif
