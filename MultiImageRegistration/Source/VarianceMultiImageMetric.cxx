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
VarianceMultiImageMetric < TFixedImage >::
VarianceMultiImageMetric()
{


}


/*
 * Initialize
 */
template <class TFixedImage> 
void
VarianceMultiImageMetric<TFixedImage>
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

}

/*
 * Get the match Measure
 */
template < class TFixedImage >
typename VarianceMultiImageMetric < TFixedImage >::MeasureType
VarianceMultiImageMetric <TFixedImage >::
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



/*
 * Get the match Measure
 */
template < class TFixedImage >
void
VarianceMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  const int N = this->m_NumberOfImages;
  const unsigned int size = this->m_Sample.size();
  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */

  const unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  // Update intensity values
  MovingImagePointType mappedPoint;
  for(int i=threadId; i< size; i += this->m_NumberOfThreads )
  {
    for(int j=0; j<N; j++)
    {
      mappedPoint = this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }
  }

  //Calculate variance and mean
  this->m_value[threadId] = 0.0;
  double squareSum, meanSum;

  for(int i=threadId; i< size; i+=this->m_NumberOfThreads )
  {
    squareSum = 0.0;
    meanSum = 0.0;
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      const double currentValue = this->m_Sample[i].imageValueArray[j];
      squareSum += currentValue * currentValue;
      meanSum += currentValue;
    }

    meanSum /= (double) N;
    squareSum /= (double) N;
    this->m_value[threadId] += squareSum - meanSum * meanSum;
  }        // end of sample loop

}


template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >
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
VarianceMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  double N = (double) this->m_NumberOfImages;
  int numberOfThreads = this->GetNumberOfThreads();
  
  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */
  MeasureType value = NumericTraits < MeasureType >::Zero;

  unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  //Initialize the derivative array to zero
  for(int i=0; i<this->m_NumberOfImages;i++)
  {
    this->m_DerivativesArray[threadId][i].Fill(0.0);
  }

  
  //Calculate metric value
  double measure = 0.0;
  double mean;
  double variance;
  double sumOfSquares = 0.0;
  DerivativeType deriv(numberOfParameters);
  // Sum over spatial samples
  for (int a=threadId; a<this->m_Sample.size(); a += numberOfThreads )
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
    mean /= N;
    sumOfSquares /= N;
    variance = sumOfSquares - mean*mean;
    measure += variance;

    // Calculate derivative
    for (int i = 0; i < this->m_NumberOfImages; i++)
    {
      //calculate the derivative weight
      const double weight = 2.0 / static_cast<double>(this->m_NumberOfImages) *
                      (this->m_Sample[a].imageValueArray[i] - mean);
      
      // Get the derivative for this sample
      UpdateSingleImageParameters( this->m_DerivativesArray[threadId][i], this->m_Sample[a], weight, i, threadId);
    }
  } // End of sample Loop
  this->m_value[threadId] = measure;

}





}        // end namespace itk


#endif
