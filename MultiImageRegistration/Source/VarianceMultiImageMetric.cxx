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

  m_NumberOfSpatialSamples = 0;
  this->SetNumberOfSpatialSamples(50);


  // Following initialization is related to
  // calculating image derivatives
  this->SetComputeGradient (false);  // don't use the default gradient for now
  m_DerivativeCalculator = DerivativeFunctionType::New ();

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

  // resize the sample array
  m_Sample.resize (m_NumberOfSpatialSamples);
  for (int i = 0; i < m_NumberOfSpatialSamples; i++)
  {
    m_Sample[i].imageValueArray.resize (this->m_NumberOfImages);
  }

  // reinitilize the seed for the random iterator
  this->ReinitializeSeed();

  // Sample the image domain
  this->SampleFixedImageDomain(m_Sample);

  int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  //Get number of threads
  m_NumberOfThreads = this->GetNumberOfThreads();

  m_value.SetSize( m_NumberOfThreads );

  // Each thread has its own derivative pointer
  m_DerivativeCalcVector.resize(m_NumberOfThreads);
  m_derivativeArray.resize(m_NumberOfThreads);
  for(int i=0; i<m_NumberOfThreads; i++)
  {
    m_derivativeArray[i].SetSize(numberOfParameters*this->m_NumberOfImages);
    m_DerivativeCalcVector[i] = DerivativeFunctionType::New ();
  }

  this->m_NumberOfPixelsCounted = GetNumberOfSpatialSamples();

}


template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::
PrintSelf (std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "NumberOfSpatialSamples: ";
  os << m_NumberOfSpatialSamples << std::endl;
}


/*
 * Set the number of spatial samples
 */
template < class TFixedImage >
void
VarianceMultiImageMetric < TFixedImage >::
SetNumberOfSpatialSamples (unsigned int num)
{
  if (num == m_NumberOfSpatialSamples)
    return;

  this->Modified();

  // clamp to minimum of 1
  m_NumberOfSpatialSamples = ((num > 1) ? num : 1);

}


/*
 * Uniformly sample the fixed image domain. Each sample consists of:
 *  - the sampled point
 *  - Corresponding moving image intensity values
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::
SampleFixedImageDomain (SpatialSampleContainer & samples) const
{
  
  typedef ImageRandomConstIteratorWithIndex < FixedImageType > RandomIterator;
  RandomIterator randIter(this->m_ImageArray[0], this->GetFixedImageRegion());

  randIter.SetNumberOfSamples(m_NumberOfSpatialSamples);
  randIter.GoToBegin();

  typename SpatialSampleContainer::iterator iter;
  typename SpatialSampleContainer::const_iterator end = samples.end();

  bool allOutside = true;

  // Number of random picks made from the portion of fixed image within the fixed mask
  unsigned long numberOfFixedImagePixelsVisited = 0;
  unsigned long dryRunTolerance = this->GetFixedImageRegion().GetNumberOfPixels();

  // Vector to hold mapped points
  std::vector<MovingImagePointType> mappedPointsArray(this->m_NumberOfImages);

    
  for (iter = samples.begin (); iter != end; ++iter)
  {
    // Get sampled index
    FixedImageIndexType index = randIter.GetIndex();
    // Translate index to point
    this->m_ImageArray[0]->TransformIndexToPhysicalPoint(index, (*iter).FixedImagePoint);

    // Check the total number of sampled points
    ++numberOfFixedImagePixelsVisited;
    if (numberOfFixedImagePixelsVisited > dryRunTolerance)
    {
      // We randomly visited as many points as is the size of the fixed image
      // region.. Too may samples mapped outside.. go change your transform
      itkExceptionMacro(<<"Too many samples mapped outside the moving buffer");
    }


    //Check whether all points are inside mask
    bool allPointsInside = true;
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      mappedPointsArray[j] = this->m_TransformArray[j]->TransformPoint ((*iter).FixedImagePoint);
      
      if ( (this->m_ImageMaskArray[j] && !this->m_ImageMaskArray[j]->IsInside (mappedPointsArray[j]) )
            || !this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPointsArray[j]) )
      {
        allPointsInside = false;
      }
    }

    // If not all points are inside continue to the next random sample
    if (allPointsInside == false)
    {
      ++randIter;
      --iter;
      continue;
    }

    // write the mapped samples intensity values inside an array
    // write the mapped samples intensity values inside an array
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      (*iter).imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPointsArray[j]);

      allOutside = false;
    }
    // Jump to random position
    ++randIter;

  }

  if (allOutside)
  {
    // if all the samples mapped to the outside throw an exception
    itkExceptionMacro(<<"All the sampled point mapped to outside of the moving image");
  }
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
typename VarianceMultiImageMetric < TFixedImage >::MeasureType
VarianceMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // cout << "Checking GetValue" << endl;

  int N = this->m_NumberOfImages;
  int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters ();
  ParametersType currentParam (numberOfParameters);

  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < numberOfParameters; j++)
    {
      currentParam[j] = parameters[i * numberOfParameters + j];
    }
    // cout << currentParam << endl;
    this->m_TransformArray[i]->SetParametersByValue (currentParam);
  }

  // collect sample set
  //
  // or dont collect a new sample set if used with
  // regular gradient descent
  // this->SampleFixedImageDomain(m_Sample);

  // Update intensity values
  MovingImagePointType mappedPoint;
  for(int i=0; i< m_Sample.size(); i++ )
  {
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      mappedPoint = this->m_TransformArray[j]->TransformPoint(m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }
  } 

  //Calculate variance and mean
  double measure = 0.0;
  double squareSum, meanSum;

  for(int i=0; i< m_Sample.size(); i++ )
  {
    squareSum = 0.0;
    meanSum = 0.0;
    double currentValue;
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      squareSum += m_Sample[i].imageValueArray[j] * m_Sample[i].imageValueArray[j];
      meanSum += m_Sample[i].imageValueArray[j];
    }

    meanSum /= N;
    squareSum /= N;
    measure += squareSum - meanSum * meanSum;
  }        // end of sample loop
  
  measure /= static_cast<double>(m_NumberOfSpatialSamples);

  cout << measure << endl;
  return measure;

}


/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void 
VarianceMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue (const ParametersType & parameters) const
{

  cout << "checking derivative" << endl;

  // collect sample set
  this->SampleFixedImageDomain (m_Sample);

  //Make sure that each transform parameters are updated
  int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters ();
  ParametersType currentParam (numberOfParameters);
  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < numberOfParameters; j++)
    {
      currentParam[j] = parameters[numberOfParameters * i + j];
    }
    cout << currentParam << endl;
    this->m_TransformArray[i]->SetParametersByValue (currentParam);
  }

}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >
::AfterGetThreadedValue (MeasureType & value,
                           DerivativeType & derivative) const
{

  value = NumericTraits< RealType >::Zero;

  const int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters ();
  DerivativeType temp (numberOfParameters * this->m_NumberOfImages);
  temp.Fill (0.0);

  // Sum over the values returned by threads
  for( unsigned int i=0; i < m_NumberOfThreads; i++ )
  {
    value += m_value[i];
    temp += m_derivativeArray[i];
  }
  value /= (double) m_Sample.size();
  temp /= (double) m_Sample.size();
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


/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
VarianceMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  double N = (double) this->m_NumberOfImages;
  int numberOfThreads = this->GetNumberOfThreads();
  
  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */
  MeasureType value = NumericTraits < MeasureType >::Zero;

  unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  //Initialize the derivative to zero
  m_derivativeArray[threadId].Fill(0.0);

  
  //Calculate metric value
  double measure = 0.0;
  double mean;
  double variance;
  double sumOfSquares = 0.0;
  DerivativeType deriv(numberOfParameters);
  // Sum over spatial samples
  for (int a=threadId; a<m_Sample.size(); a += numberOfThreads )
  {
    sumOfSquares = 0.0;
    mean = 0.0;
    // Sum over images
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      sumOfSquares += m_Sample[a].imageValueArray[j]*m_Sample[a].imageValueArray[j];
      mean += m_Sample[a].imageValueArray[j];
    }
    mean /= N;
    sumOfSquares /= N;
    variance = sumOfSquares - mean*mean;
    if(variance <= 1e-5)
    {
      variance = 1e-5;
    }
    measure += variance;

    // Calculate derivative
    for (int i = 0; i < this->m_NumberOfImages; i++)
    {
      //calculate the derivative weight
      double weight = 2.0 / static_cast<double>(this->m_NumberOfImages) *
                      (m_Sample[a].imageValueArray[i] - mean) / variance;
      
      //Get the derivative at this pixel
      m_DerivativeCalcVector[threadId]->SetInputImage(this->m_ImageArray[i]);
      this->CalculateDerivatives (m_Sample[a].FixedImagePoint, deriv, i, threadId);

      //copy the properpart of the derivative
      for (int j = 0;j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
      {
        m_derivativeArray[threadId][i * numberOfParameters + j] += weight*deriv[j];
      }
    }
  } // End of sample Loop
  m_value[threadId] = vcl_log(measure);
  
}

/*
 * Get the match Measure
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >
::GetValueAndDerivative(const ParametersType & parameters,
                          MeasureType & value,
                          DerivativeType & derivative) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters); 
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;


  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallback, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  this->AfterGetThreadedValue(value, derivative);

}



// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
VarianceMultiImageMetric< TFixedImage >
::ThreaderCallback( void *arg )
{
  ThreadStruct *str;

  int threadId;
  int threadCount;

  threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValue( threadId );


  return ITK_THREAD_RETURN_VALUE;
}




/*
 * Get the match measure derivative
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >
::GetDerivative (const ParametersType & parameters,
          DerivativeType & derivative) const
{
  MeasureType value;
  // call the combined version
  this->GetValueAndDerivative(parameters, value, derivative);
}


/*
 * Calculate derivatives of the image intensity with respect
 * to the transform parmeters.
 *
 * This should really be done by the mapper.
 *
 * This is a temporary solution until this feature is implemented
 * in the mapper. This solution only works for any transform
 * that support GetJacobian()
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::
CalculateDerivatives(const FixedImagePointType & point, DerivativeType & derivatives, int i, int threadID) const
{

  MovingImagePointType mappedPoint =
                     this->m_TransformArray[i]->TransformPoint(point);

  CovariantVector < double, MovingImageDimension > imageDerivatives;

  if (m_DerivativeCalcVector[threadID]->IsInsideBuffer(mappedPoint))
  {
    imageDerivatives = m_DerivativeCalcVector[threadID]->Evaluate(mappedPoint);
  }
  else
  {
   derivatives.Fill (0.0);
   return;
  }

  typedef typename TransformType::JacobianType JacobianType;
  const JacobianType & jacobian =
                       this->m_TransformArray[i]->GetJacobian(point); //Is it mappedPoint?

  unsigned int numberOfParameters =
               this->m_TransformArray[i]->GetNumberOfParameters();

  for (unsigned int k = 0; k < numberOfParameters; k++)
  {
    derivatives[k] = 0.0;
    for (unsigned int j = 0; j < MovingImageDimension; j++)
    {
      derivatives[k] += jacobian[j][k] * imageDerivatives[j];
    }
  }

}


/*
 * Reinitialize the seed of the random number generator
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::ReinitializeSeed()
{
  Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed();
}

/*
 * Reinitialize the seed of the random number generator
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::ReinitializeSeed(int seed)
{
    Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed(seed);
}





}        // end namespace itk


#endif
