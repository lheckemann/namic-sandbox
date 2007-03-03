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
#ifndef _ParzenWindowEntropyMultiImageMetric_cxx
#define _ParzenWindowEntropyMultiImageMetric_cxx

#include "ParzenWindowEntropyMultiImageMetric.h"


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
ParzenWindowEntropyMultiImageMetric < TFixedImage >::
    ParzenWindowEntropyMultiImageMetric()
{

  m_NumberOfSpatialSamples = 0;
  this->SetNumberOfSpatialSamples(50);

  m_KernelFunction =
    dynamic_cast < KernelFunction * >(GaussianKernelFunction::New ().GetPointer ());

  m_ImageStandardDeviation = 0.4;

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
ParzenWindowEntropyMultiImageMetric<TFixedImage>
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

  
}


template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::
PrintSelf (std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "NumberOfSpatialSamples: ";
  os << m_NumberOfSpatialSamples << std::endl;
  os << indent << "FixedImageStandardDeviation: ";
  os << m_ImageStandardDeviation << std::endl;
  os << indent << "MovingImageStandardDeviation: ";
  os << m_KernelFunction.GetPointer () << std::endl;
}


/*
 * Set the number of spatial samples
 */
template < class TFixedImage >
void
ParzenWindowEntropyMultiImageMetric < TFixedImage >::
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
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::
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
typename ParzenWindowEntropyMultiImageMetric < TFixedImage >::MeasureType
ParzenWindowEntropyMultiImageMetric <TFixedImage >::
GetValue(const ParametersType & parameters) const
{
  // Call a method that perform some calculations prior to splitting the main
  // computations into separate threads

  this->BeforeGetThreadedValue(parameters);
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Metric =  this;


  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(ThreaderCallbackGetValue, &str);
  
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
ParzenWindowEntropyMultiImageMetric< TFixedImage >
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
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::BeforeGetThreadedValue(const ParametersType & parameters) const
{
  //Make sure that each transform parameters are updated
  int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters();
  ParametersType currentParam (numberOfParameters);
  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < numberOfParameters; j++)
    {
      currentParam[j] = parameters[numberOfParameters * i + j];
    }
    this->m_TransformArray[i]->SetParametersByValue (currentParam);
  }

}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::GetThreadedValue(int threadId) const
{

  double N = (double) this->m_NumberOfImages;

  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */

  unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();

  // Update intensity values
  MovingImagePointType mappedPoint;
  for(int i=threadId; i< m_Sample.size(); i += m_NumberOfThreads )
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
  m_value[threadId] = 0.0;
  double dSum;

  // Loop over the pixel stacks
  for (int a=threadId; a<m_Sample.size(); a += m_NumberOfThreads )
  {
    double dLogSum = 0.0;
    
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      dSum = m_MinProbability;
      
      for(int k = 0; k < this->m_NumberOfImages; k++)
      {
        dSum += m_KernelFunction->Evaluate( ( m_Sample[a].imageValueArray[j] - m_Sample[a].imageValueArray[k] ) /
            m_ImageStandardDeviation );
      }
      dSum /= static_cast<double> (this->m_NumberOfImages);
      dLogSum  -= ( dSum > 0.0 ) ? vcl_log( dSum ) : 0.0;
      
    }
    m_value[threadId] += dLogSum;


  }  // End of sample loop

  
}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
typename ParzenWindowEntropyMultiImageMetric < TFixedImage >::MeasureType
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::AfterGetThreadedValue() const
{

  MeasureType value = NumericTraits< RealType >::Zero;

  const int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters();

  // Sum over the values returned by threads
  for( unsigned int i=0; i < m_NumberOfThreads; i++ )
  {
    value += m_value[i];
  }
  value /= (MeasureType) (m_Sample.size()*this->m_NumberOfImages);

  //cout << value << endl;
  return value;
}


/*
 * Get the match Measure
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >
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

// Callback routine used by the threading library. This routine just calls
// the GetThreadedValue() method after setting the correct partition of data
// for this thread.
template < class TFixedImage >
ITK_THREAD_RETURN_TYPE
ParzenWindowEntropyMultiImageMetric< TFixedImage >
::ThreaderCallbackGetValueAndDerivative( void *arg )
{
  ThreadStruct *str;

  int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Metric->GetThreadedValueAndDerivative( threadId );


  return ITK_THREAD_RETURN_VALUE;
}


/**
 * Prepare auxiliary variables before starting the threads
 */
template < class TFixedImage >
void 
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const
{
  //cout << "checking derivative" << endl;
  // collect sample set
  this->SampleFixedImageDomain (m_Sample);

  //Make sure that each transform parameters are updated
  int numberOfParameters = this->m_TransformArray[0]->GetNumberOfParameters();
  ParametersType currentParam (numberOfParameters);
  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < numberOfParameters; j++)
    {
      currentParam[j] = parameters[numberOfParameters * i + j];
    }
    //cout << currentParam << endl;
    this->m_TransformArray[i]->SetParametersByValue (currentParam);
  }

}


/**
 * Consolidate auxiliary variables after finishing the threads
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >
::AfterGetThreadedValueAndDerivative(MeasureType & value,
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
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{

  double N = (double) this->m_NumberOfImages;

  /** The tranform parameters vector holding i'th images parameters 
  Copy parameters in to a collection of arrays */
  MeasureType value = NumericTraits < MeasureType >::Zero;

  unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters();
  
  //Initialize the derivative array to zero
  m_derivativeArray[threadId].Fill(0.0);


  //Calculate variance and mean
  m_value[threadId] = 0.0;
  std::vector<double> dSum(this->m_NumberOfImages);
  DerivativeType deriv(numberOfParameters);

  // Loop over the pixel stacks
  for (int a=threadId; a<m_Sample.size(); a += m_NumberOfThreads )
  {
    double dLogSum = 0.0;
    
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      dSum[j] = m_MinProbability;
      
      for(int k = 0; k < this->m_NumberOfImages; k++)
      {
        dSum[j] += m_KernelFunction->Evaluate( ( m_Sample[a].imageValueArray[j] - m_Sample[a].imageValueArray[k] ) /
            m_ImageStandardDeviation );
      }
      dSum[j] /= static_cast<double> (this->m_NumberOfImages);
      dLogSum  -= ( dSum[j] > 0.0 ) ? vcl_log( dSum[j] ) : 0.0;
      
    }
    m_value[threadId] += dLogSum / (double) this->m_NumberOfImages;


    // Calculate derivative
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      double innerSum = 0.0;
      for (int k = 0; k < this->m_NumberOfImages; k++)
      {
        double diff = ( m_Sample[a].imageValueArray[j] - m_Sample[a].imageValueArray[k] ) /
                        m_ImageStandardDeviation;
        innerSum += m_KernelFunction->Evaluate( diff ) * diff;
      }
      innerSum /= static_cast<double>(this->m_NumberOfImages);

      // Get the derivative for this sample
      m_DerivativeCalcVector[threadId]->SetInputImage(this->m_ImageArray[j]);
      this->CalculateDerivatives(m_Sample[a].FixedImagePoint, deriv, j, threadId);

      double weight = 2.0 / static_cast<double>(this->m_NumberOfImages) * innerSum / dSum[j];

      //Copy the proper part of the derivative
      for (int l = 0; l < numberOfParameters; l++)
      {
        m_derivativeArray[threadId][j * numberOfParameters + l] += weight*deriv[l];
      }
    }
  }  // End of sample loop

  

}




/*
 * Get the match measure derivative
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >
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
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::
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
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::ReinitializeSeed()
{
  Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed();
}

/*
 * Reinitialize the seed of the random number generator
 */
template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::ReinitializeSeed(int seed)
{
    Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed(seed);
}





}        // end namespace itk


#endif
