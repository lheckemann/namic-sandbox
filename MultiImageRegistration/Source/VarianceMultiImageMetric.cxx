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
#ifndef _CongealingMultiImageMetric_cxx
#define _CongealingMultiImageMetric_cxx

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

  m_KernelFunction =
    dynamic_cast < KernelFunction * >(GaussianKernelFunction::New ().GetPointer ());

  m_FixedImageStandardDeviation = 0.4;
  m_MovingImageStandardDeviation = 0.4;

  // Following initialization is related to
  // calculating image derivatives
  this->SetComputeGradient (false);  // don't use the default gradient for now
  m_DerivativeCalculator = DerivativeFunctionType::New ();

}

template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::
PrintSelf (std::ostream & os, Indent indent) const
  {
    Superclass::PrintSelf (os, indent);
    os << indent << "NumberOfSpatialSamples: ";
    os << m_NumberOfSpatialSamples << std::endl;
    os << indent << "FixedImageStandardDeviation: ";
    os << m_FixedImageStandardDeviation << std::endl;
    os << indent << "MovingImageStandardDeviation: ";
    os << m_MovingImageStandardDeviation << std::endl;
    os << indent << "KernelFunction: ";
    os << m_KernelFunction.GetPointer () << std::endl;
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

  // resize the storage vectors
  m_Sample.resize (m_NumberOfSpatialSamples);
  for (int i = 0; i < m_NumberOfSpatialSamples; i++)
  {
    m_Sample[i].imageValueArray.resize (this->m_NumberOfImages);
  }

}


/*
 * Uniformly sample the fixed image domain. Each sample consists of:
 *  - the fixed image value
 *  - the corresponding moving image values
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::
SampleFixedImageDomain (SpatialSampleContainer & samples) const
{

    m_Sample.resize(m_NumberOfSpatialSamples);
    for (int i = 0; i < m_NumberOfSpatialSamples; i++)
    {
      m_Sample[i].imageValueArray.resize (this->m_NumberOfImages);
    }

    typedef ImageRandomConstIteratorWithIndex < FixedImageType > RandomIterator;
    RandomIterator randIter (this->m_ImageArray[0], this->GetFixedImageRegion ());

    randIter.SetNumberOfSamples(m_NumberOfSpatialSamples);
    randIter.GoToBegin();

    typename SpatialSampleContainer::iterator iter;
    typename SpatialSampleContainer::const_iterator end = samples.end();

    bool allOutside = true;

    this->m_NumberOfPixelsCounted = 0;  // Number of pixels that map into the 
    // fixed and moving image mask, if specified
    // and the resampled fixed grid after 
    // transformation. 

    // Number of random picks made from the portion of fixed image within the fixed mask
    unsigned long numberOfFixedImagePixelsVisited = 0;
    unsigned long dryRunTolerance = this->GetFixedImageRegion().GetNumberOfPixels();

    for (iter = samples.begin (); iter != end; ++iter)
    {
      // Get sampled index
      FixedImageIndexType index = randIter.GetIndex();
      // Get sampled fixed image value
      (*iter).imageValueArray[0] = randIter.Get();
      // Translate index to point
      this->m_ImageArray[0]->TransformIndexToPhysicalPoint(index, (*iter).FixedImagePointValue);

      // If not inside the fixed mask, ignore the point
      if (this->m_ImageMaskArray[0] && !this->m_ImageMaskArray[0]->IsInside ((*iter).FixedImagePointValue))
      {
        ++randIter;    // jump to another random position
        continue;
      }

      if (allOutside)
      {
        ++numberOfFixedImagePixelsVisited;
        if (numberOfFixedImagePixelsVisited > dryRunTolerance)
        {
          // We randomly visited as many points as is the size of the fixed image
          // region.. Too may samples mapped ouside.. go change your transform
          itkExceptionMacro(<<"Too many samples mapped outside the moving buffer");
        }
      }
      
      for (int j = 1; j < this->m_NumberOfImages; j++)
      {
        MovingImagePointType mappedPoint = this->m_TransformArray[j]->TransformPoint ((*iter).FixedImagePointValue);
        // If the transformed point after transformation does not lie within the
        // MovingImageMask, skip it.
        if (this->m_ImageMaskArray[j] && !this->m_ImageMaskArray[j]->IsInside (mappedPoint))
        {
          ++randIter;
          continue;
        }

        // The interpolator does not need to do bounds checking if we have masks, 
        // since we know that the point is within the fixed and moving masks. But
        // a crazy user can specify masks that are bigger than the image. Then we
        // will need bounds checking.. So keep this anyway.
        if (this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint))
        {
          (*iter).imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate (mappedPoint);
          this->m_NumberOfPixelsCounted++;
          allOutside = false;
        }
        else
        {
          (*iter).imageValueArray[j] = 0;
        }
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

    int N = this->m_NumberOfImages;
    ParametersType currentParam (this->m_TransformArray[0]->GetNumberOfParameters ());

    for (int i = 0; i < N; i++)
    {
      for (int j = 0; j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
      {
        currentParam[j] = parameters[i * N + j];
      }
      this->m_TransformArray[i]->SetParameters (currentParam);
    }

    // collect sample set
    this->SampleFixedImageDomain (m_Sample);

    //Calculate variance and mean
    double measure = 0.0;

    typename SpatialSampleContainer::const_iterator iter;
    typename SpatialSampleContainer::const_iterator end = m_Sample.end ();

    double squareSum, meanSum;
    for (iter = m_Sample.begin (); iter != end; ++iter)
      {
        squareSum = 0.0;
        meanSum = 0.0;
        for (int j = 0; j < this->m_NumberOfImages; j++)
        {
          squareSum += (*iter).imageValueArray[j] * (*iter).imageValueArray[j];
          meanSum += (*iter).imageValueArray[j] / N;
        }

        measure += (squareSum - meanSum * meanSum) / N;

      }        // end of sample loop
    measure = measure / ((double) m_NumberOfSpatialSamples);

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
  int numberOfThreads = this->GetNumberOfThreads();

  m_value.SetSize( numberOfThreads );


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
    this->m_TransformArray[i]->SetParameters (currentParam);
  }

  // Each thread has its own derivative pointer
  m_DerivativeCalcVector.resize(numberOfThreads);
  m_derivativeArray.resize(numberOfThreads);
  for(int i=0; i<numberOfThreads; i++)
  {
    m_derivativeArray[i].SetSize(numberOfParameters*this->m_NumberOfImages);
    m_DerivativeCalcVector[i] = DerivativeFunctionType::New ();
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

  const unsigned int numberOfThreads = this->GetNumberOfThreads();

  value = NumericTraits< RealType >::Zero;


  DerivativeType temp (this->m_TransformArray[0]->GetNumberOfParameters () * this->m_NumberOfImages);
  temp.Fill (0.0);

  // Sum over the values returned by threads
  for( unsigned int i=0; i < numberOfThreads; i++ )
  {
    value += m_value[i];
    temp += m_derivativeArray[i];
  }
  value /= (double) m_Sample.size();
  temp /= (double) m_Sample.size();
  derivative = temp;

  
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
  
  DerivativeType derivative(numberOfParameters * this->m_NumberOfImages);
  derivative.Fill (0.0);
  m_derivativeArray[threadId] = derivative;



  // calculate the mutual information
  DerivativeType deriv(numberOfParameters);
  DerivativeType sum(numberOfParameters);

  //Calculate value
  double measure = 0.0;
  double meanSum = 0.0;
  double sumOfSquares = 0.0;
  // Sum over spatial samples
  for (int a=threadId; a<m_Sample.size(); a += numberOfThreads )
  {
    sumOfSquares = 0.0;
    meanSum = 0.0;
    // Sum over images
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      sumOfSquares += pow(m_Sample[a].imageValueArray[j],2);
      meanSum += m_Sample[a].imageValueArray[j];
    }
    meanSum /= N;
    measure += (sumOfSquares - pow(meanSum,2));

  }      // end of sample loop
  m_value[threadId] = measure / N ;


  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {


    // set the DerivativeCalculator
    m_DerivativeCalcVector[threadId]->SetInputImage (this->m_ImageArray[i]);


    //Calculate derivative
    double derI = 0.0;
    sum.Fill (0.0);
    for (int a=threadId; a<m_Sample.size(); a += numberOfThreads )
    {
      meanSum = 0.0;
      derI = 0.0;

      for (int j = 0; j < this->m_NumberOfImages; j++)
        meanSum += m_Sample[a].imageValueArray[j];
      meanSum = meanSum / N;

      //for(int j=0; j<this->m_NumberOfImages;j++)
      derI = 2.0 / N * (m_Sample[a].imageValueArray[i] - meanSum);
      
      // get the image derivative for this B sample
      this->CalculateDerivatives (m_Sample[a].FixedImagePointValue,
                                    deriv,i,threadId);
     sum -= deriv * derI;

    }      // end of sample B loop

    //copy the properpart of the derivative
    for (int j = 0;j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
    {
      m_derivativeArray[threadId][i * numberOfParameters + j] = sum[j];
    }

  } // End of the for loop over images


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
 * Get the both Value and Derivative Measure
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >
::GetValueAndDerivative2 (const ParametersType & parameters,
            MeasureType & value,
            DerivativeType & derivative) const
{

  double N = (double) this->m_NumberOfImages;

  /** The tranform parameters vector holding i'th images parameters 
      Copy parameters in to a collection of arrays */


  value = NumericTraits < MeasureType >::Zero;

  unsigned int numberOfParameters =
      this->m_TransformArray[0]->GetNumberOfParameters ();
  
  DerivativeType temp (numberOfParameters * this->m_NumberOfImages);
  temp.Fill (0.0);
  derivative = temp;

  // collect sample set A
  this->SampleFixedImageDomain (m_Sample);
  
  // calculate the mutual information
  typename SpatialSampleContainer::iterator aiter;
  typename SpatialSampleContainer::const_iterator aend = m_Sample.end ();


  DerivativeType derivA (numberOfParameters);
  DerivativeType sum (numberOfParameters);
  ParametersType currentParam (numberOfParameters);


  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
    {
      currentParam[j] = parameters[numberOfParameters * i + j];
    }
    // make sure the transform has the current parameters
    this->m_TransformArray[i]->SetParameters (currentParam);


    // set the DerivativeCalculator
    m_DerivativeCalculator->SetInputImage (this->m_ImageArray[i]);

    //Calculate value
    double measure = 0.0;
    double meanSum = 0.0;
    double sumOfSquares = 0.0;
    // Sum over spatial samples
    for (aiter = m_Sample.begin(); aiter != aend; ++aiter)
    {
      sumOfSquares = 0.0;
      meanSum = 0.0;
      // Sum over images
      for (int j = 0; j < this->m_NumberOfImages; j++)
      {
        sumOfSquares += pow((*aiter).imageValueArray[j],2);
        meanSum += (*aiter).imageValueArray[j] / N;
      }
      measure += (sumOfSquares - pow(meanSum,2)) / N;

    }      // end of sample loop
    value = measure / ((double) m_NumberOfSpatialSamples);



    //Calculate derivative
    double derI = 0.0;
    sum.Fill (0.0);
    for (aiter = m_Sample.begin(); aiter != aend; ++aiter)
    {
      meanSum = 0.0;
      derI = 0.0;
      for (int j = 0; j < this->m_NumberOfImages; j++)
        meanSum += (*aiter).imageValueArray[j];
      meanSum = meanSum / N;

      derI = 2.0 / ((double) m_NumberOfSpatialSamples) / N *
        ((*aiter).imageValueArray[i] - meanSum);


      // get the image derivative for this B sample
      this->CalculateDerivatives ((*aiter).FixedImagePointValue,
           derivA,i);

      sum -= derivA * derI;
     }      // end of sample B loop

     //copy the properpart of the derivative
    for (int j = 0;j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
    {
      derivative[i * numberOfParameters + j] = sum[j];
    }

  } // End of the for loop over images

  //Remove mean
  sum.Fill (0.0);
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    for (int j = 0; j < numberOfParameters; j++)
    {
        sum[j] += derivative[i * numberOfParameters + j] / N;
    }
  }

//  for (int i = 0; i < this->m_NumberOfImages * numberOfParameters; i++)
//  {
//      derivative[i] -= sum[i % numberOfParameters];
//  }



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
      this->GetValueAndDerivative (parameters, value, derivative);
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
CalculateDerivatives (const FixedImagePointType & point, DerivativeType & derivatives, int i, int threadID) const
{

//   MovingImagePointType mappedPoint = this->m_Transform->TransformPoint( point );
    MovingImagePointType mappedPoint =
    this->m_TransformArray[i]->TransformPoint (point);

    CovariantVector < double, MovingImageDimension > imageDerivatives;

    if (m_DerivativeCalcVector[threadID]->IsInsideBuffer (mappedPoint))
      {
        imageDerivatives = m_DerivativeCalcVector[threadID]->Evaluate (mappedPoint);
      }
    else
      {
       derivatives.Fill (0.0);
       return;
      }

    typedef typename TransformType::JacobianType JacobianType;
//   const JacobianType& jacobian = this->m_Transform->GetJacobian( point );
    const JacobianType & jacobian =
      this->m_TransformArray[i]->GetJacobian (point);

//   unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();
    unsigned int numberOfParameters =
      this->m_TransformArray[i]->GetNumberOfParameters ();

    for (unsigned int k = 0; k < numberOfParameters; k++)
      {
  derivatives[k] = 0.0;
  for (unsigned int j = 0; j < MovingImageDimension; j++)
    {
      derivatives[k] += jacobian[j][k] * imageDerivatives[j];
    }
      }

  }

  template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::
      CalculateDerivatives (const FixedImagePointType & point, DerivativeType & derivatives, int i) const
      {

//   MovingImagePointType mappedPoint = this->m_Transform->TransformPoint( point );
        MovingImagePointType mappedPoint =
            this->m_TransformArray[i]->TransformPoint (point);

        CovariantVector < double, MovingImageDimension > imageDerivatives;

        if (m_DerivativeCalculator->IsInsideBuffer (mappedPoint))
        {
          imageDerivatives = m_DerivativeCalculator->Evaluate (mappedPoint);
        }
        else
        {
          derivatives.Fill (0.0);
          return;
        }

        typedef typename TransformType::JacobianType JacobianType;
//   const JacobianType& jacobian = this->m_Transform->GetJacobian( point );
        const JacobianType & jacobian =
            this->m_TransformArray[i]->GetJacobian (point);

//   unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();
        unsigned int numberOfParameters =
            this->m_TransformArray[i]->GetNumberOfParameters ();

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
void VarianceMultiImageMetric < TFixedImage >::ReinitializeSeed ()
  {
    Statistics::MersenneTwisterRandomVariateGenerator::GetInstance ()->
      SetSeed ();
  }

/*
 * Reinitialize the seed of the random number generator
 */
template < class TFixedImage >
void VarianceMultiImageMetric < TFixedImage >::ReinitializeSeed (int seed)
  {
    Statistics::MersenneTwisterRandomVariateGenerator::GetInstance ()->
      SetSeed (seed);
  }





}        // end namespace itk


#endif
