/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiThreadedMutualInformationImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2007/12/20 22:17:30 $
  Version:   $Revision: 1.64 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkMultiThreadedMutualInformationImageToImageMetric_txx
#define _itkMultiThreadedMutualInformationImageToImageMetric_txx

#include "itkMultiThreadedMutualInformationImageToImageMetric.h"
#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"

// For debugging.
// This allows us to turn off the execution
// of multiple threads and instead call the thread
// callback functions for each threadID, one after
// the other.
//#define SYNCHRONOUS_COMPUTATION

namespace itk
{

/*
 * Constructor
 */
template < class TFixedImage, class TMovingImage >
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::MultiThreadedMutualInformationImageToImageMetric()
{

  m_NumberOfSpatialSamples = 0;
  this->SetNumberOfSpatialSamples( 50 );

  m_KernelFunction  = dynamic_cast<KernelFunction*>(
    GaussianKernelFunction::New().GetPointer() );

  m_FixedImageStandardDeviation = 0.4;
  m_MovingImageStandardDeviation = 0.4;

  m_MinProbability = 0.0001;

  //
  // Following initialization is related to
  // calculating image derivatives
  this->SetComputeGradient(false); // don't use the default gradient for now
  m_DerivativeCalculator = DerivativeFunctionType::New();

#ifdef ITK_USE_ORIENTED_IMAGE_DIRECTION
    m_DerivativeCalculator->UseImageDirectionOn();
#endif

  // FIXME: Should this go to initialize?
  this->m_Threader = MultiThreaderType::New();
  this->m_NumberOfThreads = this->m_Threader->GetNumberOfThreads();
  this->m_ThreaderParameter.metric = this;
}


template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "NumberOfSpatialSamples: ";
  os << m_NumberOfSpatialSamples << std::endl;
  os << indent << "FixedImageStandardDeviation: ";
  os << m_FixedImageStandardDeviation << std::endl;
  os << indent << "MovingImageStandardDeviation: ";
  os << m_MovingImageStandardDeviation << std::endl;
  os << indent << "KernelFunction: ";
  os << m_KernelFunction.GetPointer() << std::endl;
}


/*
 * Set the number of spatial samples
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::SetNumberOfSpatialSamples( 
  unsigned int num )
{
  if ( num == m_NumberOfSpatialSamples ) return;

  this->Modified();
 
  // clamp to minimum of 1
  m_NumberOfSpatialSamples = ((num > 1) ? num : 1 );

  // resize the storage vectors
  m_SampleA.resize( m_NumberOfSpatialSamples );
  m_SampleB.resize( m_NumberOfSpatialSamples );

}


/*
 * Uniformly sample the fixed image domain. Each sample consists of:
 *  - the fixed image value
 *  - the corresponding moving image value
 *
 * \warning Note that this method has a different signature than the one in
 * the base OptImageToImageMetric and therefore they are not intended to
 * provide polymorphism. That is, this function is not overriding the one in
 * the base class. 
 *
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::SampleFixedImageDomain(
  SpatialSampleContainer& samples ) const
{

  typedef ImageRandomConstIteratorWithIndex<FixedImageType> RandomIterator;
  RandomIterator randIter( this->m_FixedImage, this->GetFixedImageRegion() );

  randIter.SetNumberOfSamples( m_NumberOfSpatialSamples );
  randIter.GoToBegin();

  SamplesIterator iter;
  SamplesConstIterator end = samples.end();

  bool allOutside = true;

  this->m_NumberOfPixelsCounted = 0;    // Number of pixels that map into the 
                                        // fixed and moving image mask, if specified
                                        // and the resampled fixed grid after 
                                        // transformation. 
                                        
  // Number of random picks made from the portion of fixed image within the fixed mask
  unsigned long numberOfFixedImagePixelsVisited = 0; 
  unsigned long dryRunTolerance = this->GetFixedImageRegion().GetNumberOfPixels();

  for( iter=samples.begin(); iter != end; ++iter )
    {
    // Get sampled index
    FixedImageIndexType index = randIter.GetIndex();
    // Get sampled fixed image value
    (*iter).FixedImageValue = randIter.Get();
    // Translate index to point
    this->m_FixedImage->TransformIndexToPhysicalPoint( index,
                                                 (*iter).FixedImagePointValue );
    
    // If not inside the fixed mask, ignore the point
    if( this->m_FixedImageMask && 
        !this->m_FixedImageMask->IsInside( (*iter).FixedImagePointValue ) )
      {
      ++randIter; // jump to another random position
      continue;
      }

    if( allOutside )
      {
      ++numberOfFixedImagePixelsVisited;
      if( numberOfFixedImagePixelsVisited > dryRunTolerance )
        {
        // We randomly visited as many points as is the size of the fixed image
        // region.. Too may samples mapped ouside.. go change your transform
        itkExceptionMacro( << "Too many samples mapped outside the moving buffer" );
        }
      }

    MovingImagePointType mappedPoint = 
      this->m_Transform->TransformPoint( (*iter).FixedImagePointValue );

    // If the transformed point after transformation does not lie within the 
    // MovingImageMask, skip it.
    if( this->m_MovingImageMask && 
        !this->m_MovingImageMask->IsInside( mappedPoint ) )
      {
      ++randIter;
      continue;
      }

    // The interpolator does not need to do bounds checking if we have masks, 
    // since we know that the point is within the fixed and moving masks. But
    // a crazy user can specify masks that are bigger than the image. Then we
    // will need bounds checking.. So keep this anyway.
    if( this->m_Interpolator->IsInsideBuffer( mappedPoint ) )
      {
      (*iter).MovingImageValue = this->m_Interpolator->Evaluate( mappedPoint );
      this->m_NumberOfPixelsCounted++;
      allOutside = false;
      }
    else
      {
      (*iter).MovingImageValue = 0;
      }

    // Jump to random position
    ++randIter;

    }

  if( allOutside )
    {
    // if all the samples mapped to the outside throw an exception
    itkExceptionMacro(<<"All the sampled point mapped to outside of the moving image" );
    }

}

template < class TFixedImage, class TMovingImage  >
typename MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::MeasureType
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValue( const ParametersType& parameters ) const
{
  //return this->GetValueDefault( parameters );
  return this->GetValueMultiThreaded( parameters );
}

/*
 * Get the match Measure
 */
template < class TFixedImage, class TMovingImage  >
typename MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::MeasureType
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueDefault( const ParametersType& parameters ) const
{

  // make sure the transform has the current parameters
  this->m_Transform->SetParameters( parameters );

  // collect sample set A
  this->SampleFixedImageDomain( m_SampleA );

  // collect sample set B
  this->SampleFixedImageDomain( m_SampleB );

  // calculate the mutual information
  double dLogSumFixed = 0.0;
  double dLogSumMoving    = 0.0;
  double dLogSumJoint  = 0.0;

  SamplesConstIterator aiter;
  SamplesConstIterator aend = m_SampleA.end();
  SamplesConstIterator biter;
  SamplesConstIterator bend = m_SampleB.end();

  unsigned long totalSamples = 0;

  // std::ofstream output("default.txt");

  for( biter = m_SampleB.begin() ; biter != bend; ++biter )
    {
    double dSumFixed  = m_MinProbability;
    double dSumMoving     = m_MinProbability;
    double dSumJoint   = m_MinProbability;

    for( aiter = m_SampleA.begin() ; aiter != aend; ++aiter )
      {
      double valueFixed;
      double valueMoving;

      valueFixed = ( (*biter).FixedImageValue - (*aiter).FixedImageValue ) /
        m_FixedImageStandardDeviation;

      valueFixed = m_KernelFunction->Evaluate( valueFixed );

      valueMoving = ( (*biter).MovingImageValue - (*aiter).MovingImageValue ) /
        m_MovingImageStandardDeviation;

      valueMoving = m_KernelFunction->Evaluate( valueMoving );

      dSumFixed += valueFixed;
      dSumMoving    += valueMoving;
      dSumJoint  += valueFixed * valueMoving;

      totalSamples++;
      } // end of sample A loop

    // output << dSumFixed << " , " << dSumMoving << " , " << dSumJoint << std::endl;

    dLogSumFixed  -= ( dSumFixed > 0.0 ) ? vcl_log(dSumFixed  ) : 0.0;
    dLogSumMoving -= ( dSumMoving> 0.0 ) ? vcl_log(dSumMoving ) : 0.0;
    dLogSumJoint  -= ( dSumJoint > 0.0 ) ? vcl_log(dSumJoint  ) : 0.0;

    } // end of sample B loop

  // std::cout << "totalSamples : " << totalSamples << std::endl;
  double nsamp   = double( m_NumberOfSpatialSamples );

  double threshold = -0.5 * nsamp * vcl_log(m_MinProbability );
  if( dLogSumMoving > threshold || dLogSumFixed > threshold ||
      dLogSumJoint > threshold  )
    {
    // at least half the samples in B did not occur within
    // the Parzen window width of samples in A
    itkExceptionMacro(<<"Standard deviation is too small" );
    }

  MeasureType measure = dLogSumFixed + dLogSumMoving - dLogSumJoint;
  measure /= nsamp;
  measure += vcl_log(nsamp );

  return measure;

}

template < class TFixedImage, class TMovingImage  >
typename MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::MeasureType
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueMultiThreaded( const ParametersType& parameters ) const
{
  // FIXME: Partial results containers need to be cleaned up.
  // FIXME: This probably could be done in each thread...
  this->ClearPartialResults();

  // Before multithreading make sure the transform has the current parameters
  this->m_Transform->SetParameters( parameters );

  // Do this before multithreading, or sample over a fixed region
  // collect sample set A
  this->SampleFixedImageDomain( m_SampleA );

  // Do this before multithreading, or sample over a fixed region
  // collect sample set B
  this->SampleFixedImageDomain( m_SampleB );

  // Setup the sample iterators for each thread.
  unsigned int samplesPerThread  = static_cast< unsigned int >( 
    vcl_floor(static_cast<double>(this->m_NumberOfSpatialSamples) /
              static_cast<double>(m_NumberOfThreads)) );

  // The last thread needs to handle more/fewer samples depending on how the 
  // samples are divided by the threads.
  unsigned int lastThreadSamples = this->m_NumberOfSpatialSamples - (m_NumberOfThreads -1) * samplesPerThread;

  // These itertators point to the start/end A & B samples for each thread.
  // Sample iterators need to point 1 beyond the end of the samples.
  for (unsigned int t = 0; t < m_NumberOfThreads - 1; t++)
    {
    this->m_SampleAStartIterators[t] = m_SampleA.begin() + t * samplesPerThread;
    this->m_SampleAEndIterators[t]   = m_SampleA.begin() + (t + 1) * samplesPerThread;

    // Always use all the B samples.
    this->m_SampleBStartIterators[t] = m_SampleB.begin();
    this->m_SampleBEndIterators[t]   = m_SampleB.end();
    }

  //  Set the iterators for the last thread. Last thread 
  // needs to account for samples that remain when dividing by the 
  // number of threads.
  this->m_SampleAStartIterators[m_NumberOfThreads - 1] = m_SampleA.begin() + (m_NumberOfThreads - 1) * samplesPerThread;
  this->m_SampleAEndIterators[m_NumberOfThreads - 1] = m_SampleA.begin() + (m_NumberOfThreads - 1) * samplesPerThread + lastThreadSamples;

  // Always use all the B samples
  this->m_SampleBStartIterators[m_NumberOfThreads - 1] = m_SampleB.begin();
  this->m_SampleBEndIterators[m_NumberOfThreads - 1] = m_SampleB.end();

  // Some error checking, mainly for debugging right now.
  if (this->m_SampleAEndIterators[m_NumberOfThreads - 1] != m_SampleA.end())
    {
    std::cerr << "Last sample A end iterator -  not far enough" << std::endl;
    exit( EXIT_FAILURE );
    }
  if (this->m_SampleAStartIterators[0] != m_SampleA.begin())
    {
    std::cerr << "First sample A start iterator - not at begin." << std::endl;
    exit( EXIT_FAILURE );
    }

  // START THREADING, SPAWN THREADS HERE
#ifdef SYNCHRONOUS_COMPUTATION // For debug
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    this->GetValueMultiThreadedInternal( t );
    }
#else
  this->GetValueThreadedInitiate();
#endif
  // END THREADING

  // THREAD
  // ASSUME : Every thread uses the same B samples.
  //
  // Partial results are for A samples split across threads. 
  // Next, collapse partial A sample results over threads.
  //
  PartialResultsType sumFixedBResults;
  PartialResultsType sumMovingBResults;

  unsigned int numBSampleResults = m_SumFixedPartialAResults[0].size();

  // As Stephen suggested, we could split this summation over threads, with each thread
  // taking part of the bsamples.
  for (unsigned int bSample = 0; 
        bSample < numBSampleResults;
        bSample++)
    {
    // Add the other partial A sample results to the thread0 sample A results.
    for ( unsigned int thread = 1; 
          thread < m_NumberOfThreads;
          thread++ )
      {
      m_SumFixedPartialAResults[0][bSample]  += m_SumFixedPartialAResults[thread][bSample];
      m_SumMovingPartialAResults[0][bSample] += m_SumMovingPartialAResults[thread][bSample];
      m_SumJointPartialAResults[0][bSample]  += m_SumJointPartialAResults[thread][bSample];
      }
    }

  //
  // We've combined across threads. Now we need to combine across the bsamples, as in 
  // the default implementation.
  //
  double dLogSumFixed = 0.0;
  double dLogSumMoving    = 0.0;
  double dLogSumJoint  = 0.0;

  PartialResultsType::iterator fixedIter  = m_SumFixedPartialAResults[0].begin();
  PartialResultsType::iterator movingIter = m_SumMovingPartialAResults[0].begin();
  PartialResultsType::iterator jointIter  = m_SumJointPartialAResults[0].begin();

  //std::ofstream output("threaded.txt", std::ios::app);

  for (   ;

         fixedIter     != m_SumFixedPartialAResults[0].end() 
         && movingIter != m_SumMovingPartialAResults[0].end()
         && jointIter  != m_SumJointPartialAResults[0].end(); 
       
         ++fixedIter, ++movingIter, ++jointIter )
    {
    // We subtract off the extra m_MinProbability used for initialization in each thread.
    double dSumFixed  = *fixedIter - (m_NumberOfThreads - 1) * m_MinProbability;
    double dSumMoving = *movingIter - (m_NumberOfThreads - 1) * m_MinProbability;
    double dSumJoint  = *jointIter - (m_NumberOfThreads - 1) * m_MinProbability;

    // output << dSumFixed << " , " << dSumMoving << " , " << dSumJoint << std::endl;

    dLogSumFixed  -= ( dSumFixed > 0.0 ) ? vcl_log(dSumFixed  ) : 0.0;
    dLogSumMoving -= ( dSumMoving> 0.0 ) ? vcl_log(dSumMoving ) : 0.0;
    dLogSumJoint  -= ( dSumJoint > 0.0 ) ? vcl_log(dSumJoint  ) : 0.0;
    } 

  // std::cout << "dLogSumFixed  : " << dLogSumFixed  << std::endl;
  // std::cout << "dLogSumMoving : " << dLogSumMoving << std::endl;
  // std::cout << "dLogSumJoint  : " << dLogSumJoint  << std::endl;

  double nsamp   = double( m_NumberOfSpatialSamples );

  // Do this after threading.
  double threshold = -0.5 * nsamp * vcl_log(m_MinProbability );
  if( dLogSumMoving > threshold || dLogSumFixed > threshold ||
      dLogSumJoint > threshold  )
    {
    // at least half the samples in B did not occur within
    // the Parzen window width of samples in A
    itkExceptionMacro(<<"Standard deviation is too small" );
    }

  MeasureType measure = dLogSumFixed + dLogSumMoving - dLogSumJoint;
  measure /= nsamp;
  measure += vcl_log(nsamp );

  return measure;
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueMultiThreadedInternal( unsigned int threadID ) const
{
  // calculate the mutual information
  double dLogSumFixed = 0.0;
  double dLogSumMoving    = 0.0;
  double dLogSumJoint  = 0.0;

  // These need to point to our samples....
  SamplesConstIterator aiter;
  SamplesConstIterator astart = m_SampleAStartIterators[threadID];
  SamplesConstIterator aend   = m_SampleAEndIterators[threadID];
  SamplesConstIterator biter;
  SamplesConstIterator bstart = m_SampleBStartIterators[threadID];
  SamplesConstIterator bend   = m_SampleBEndIterators[threadID];

  for( biter = bstart ; biter != bend; ++biter )
    {
    double dSumFixed  = m_MinProbability;
    double dSumMoving     = m_MinProbability;
    double dSumJoint   = m_MinProbability;

    for( aiter = astart ; aiter != aend ; ++aiter )
      {
      double valueFixed;
      double valueMoving;

      valueFixed = ( (*biter).FixedImageValue - (*aiter).FixedImageValue ) /
        m_FixedImageStandardDeviation;
     
      // We need a reentrant kernel function or our own kernelfunction.
      valueFixed = m_KernelFunction->Evaluate( valueFixed );

      valueMoving = ( (*biter).MovingImageValue - (*aiter).MovingImageValue ) /
        m_MovingImageStandardDeviation;

      // We need a reentrant kernel function or our own kernelfunction.
      valueMoving = m_KernelFunction->Evaluate( valueMoving );

      dSumFixed  += valueFixed;
      dSumMoving += valueMoving;
      dSumJoint  += valueFixed * valueMoving;

      } // end of sample A loop

    // Log must be done later since log(A + B) neq log(A) + log(B).
    m_SumFixedPartialAResults[threadID].push_back( dSumFixed );
    m_SumMovingPartialAResults[threadID].push_back( dSumMoving );
    m_SumJointPartialAResults[threadID].push_back( dSumJoint );

    } // end of sample B loop

  return;
}


/*
 * Get the both Value and Derivative Measure
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivative(
  const ParametersType& parameters,
  MeasureType& value,
  DerivativeType& derivative) const
{

  value = NumericTraits< MeasureType >::Zero;
  unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();
  DerivativeType temp( numberOfParameters );
  temp.Fill( 0 );
  derivative = temp;

  // make sure the transform has the current parameters
  this->m_Transform->SetParameters( parameters );

  // set the DerivativeCalculator
  m_DerivativeCalculator->SetInputImage( this->m_MovingImage );

  // collect sample set A
  this->SampleFixedImageDomain( m_SampleA );

  // collect sample set B
  this->SampleFixedImageDomain( m_SampleB );


  // calculate the mutual information
  double dLogSumFixed = 0.0;
  double dLogSumMoving    = 0.0;
  double dLogSumJoint  = 0.0;

  SamplesIterator aiter;
  SamplesConstIterator aend = m_SampleA.end();
  SamplesIterator biter;
  SamplesConstIterator bend = m_SampleB.end();

  // precalculate all the image derivatives for sample A
  typedef std::vector<DerivativeType> DerivativeContainer;
  DerivativeContainer sampleADerivatives;
  sampleADerivatives.resize( m_NumberOfSpatialSamples );

  typename DerivativeContainer::iterator aditer;
  DerivativeType tempDeriv( numberOfParameters );

  for( aiter = m_SampleA.begin(), aditer = sampleADerivatives.begin();
       aiter != aend; ++aiter, ++aditer )
    {
    /*** FIXME: is there a way to avoid the extra copying step? *****/
    this->CalculateDerivatives( (*aiter).FixedImagePointValue, tempDeriv );
    (*aditer) = tempDeriv;
    }


  DerivativeType derivB(numberOfParameters);

  for( biter = m_SampleB.begin(); biter != bend; ++biter )
    {
    double dDenominatorMoving = m_MinProbability;
    double dDenominatorJoint = m_MinProbability;

    double dSumFixed = m_MinProbability;

    for( aiter = m_SampleA.begin(); aiter != aend; ++aiter )
      {
      double valueFixed;
      double valueMoving;

      valueFixed = ( (*biter).FixedImageValue - (*aiter).FixedImageValue )
        / m_FixedImageStandardDeviation;
      valueFixed = m_KernelFunction->Evaluate( valueFixed );

      valueMoving = ( (*biter).MovingImageValue - (*aiter).MovingImageValue )
        / m_MovingImageStandardDeviation;
      valueMoving = m_KernelFunction->Evaluate( valueMoving );

      dDenominatorMoving += valueMoving;
      dDenominatorJoint += valueMoving * valueFixed;

      dSumFixed += valueFixed;

      } // end of sample A loop

    if( dSumFixed > 0.0 )
      {
      dLogSumFixed -= vcl_log(dSumFixed );
      }
    if( dDenominatorMoving > 0.0 )
      {
      dLogSumMoving    -= vcl_log(dDenominatorMoving );
      }
    if( dDenominatorJoint > 0.0 )
      {
      dLogSumJoint  -= vcl_log(dDenominatorJoint );
      }

    // get the image derivative for this B sample
    this->CalculateDerivatives( (*biter).FixedImagePointValue, derivB );

    double totalWeight = 0.0;

    for( aiter = m_SampleA.begin(), aditer = sampleADerivatives.begin();
         aiter != aend; ++aiter, ++aditer )
      {
      double valueFixed;
      double valueMoving;
      double weightMoving;
      double weightJoint;
      double weight;

      valueFixed = ( (*biter).FixedImageValue - (*aiter).FixedImageValue ) /
        m_FixedImageStandardDeviation;
      valueFixed = m_KernelFunction->Evaluate( valueFixed );

      valueMoving = ( (*biter).MovingImageValue - (*aiter).MovingImageValue ) /
        m_MovingImageStandardDeviation;
      valueMoving = m_KernelFunction->Evaluate( valueMoving );

      weightMoving = valueMoving / dDenominatorMoving;
      weightJoint = valueMoving * valueFixed / dDenominatorJoint;

      weight = ( weightMoving - weightJoint );
      weight *= (*biter).MovingImageValue - (*aiter).MovingImageValue;

      totalWeight += weight;
      derivative -= (*aditer) * weight;

      } // end of sample A loop

    derivative += derivB * totalWeight;

    } // end of sample B loop


  double nsamp    = double( m_NumberOfSpatialSamples );

  double threshold = -0.5 * nsamp * vcl_log(m_MinProbability );
  if( dLogSumMoving > threshold || dLogSumFixed > threshold ||
      dLogSumJoint > threshold  )
    {
    // at least half the samples in B did not occur within
    // the Parzen window width of samples in A
    itkExceptionMacro(<<"Standard deviation is too small" );
    }


  value  = dLogSumFixed + dLogSumMoving - dLogSumJoint;
  value /= nsamp;
  value += vcl_log(nsamp );

  derivative  /= nsamp;
  derivative  /= vnl_math_sqr( m_MovingImageStandardDeviation );

}


/*
 * Get the match measure derivative
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetDerivative( const ParametersType& parameters, DerivativeType & derivative ) const
{
  MeasureType value;
  // call the combined version
  this->GetValueAndDerivative( parameters, value, derivative );
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
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::CalculateDerivatives(
  const FixedImagePointType& point,
  DerivativeType& derivatives ) const
{
  MovingImagePointType mappedPoint = this->m_Transform->TransformPoint( point );
  
  CovariantVector<double,MovingImageDimension> imageDerivatives;

  if ( m_DerivativeCalculator->IsInsideBuffer( mappedPoint ) )
    {
    imageDerivatives = m_DerivativeCalculator->Evaluate( mappedPoint );
    }
  else
    {
    derivatives.Fill( 0.0 );
    return;
    }

  typedef typename TransformType::JacobianType JacobianType;
  const JacobianType& jacobian = this->m_Transform->GetJacobian( point );

  unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();

  for ( unsigned int k = 0; k < numberOfParameters; k++ )
    {
    derivatives[k] = 0.0;
    for ( unsigned int j = 0; j < MovingImageDimension; j++ )
      {
      derivatives[k] += jacobian[j][k] * imageDerivatives[j];
      }
    } 

}

/*
 * Reinitialize the seed of the random number generator
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::ReinitializeSeed()
{
  Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed();
}

/*
 * Reinitialize the seed of the random number generator
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::ReinitializeSeed(int seed)
{
  Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed(seed);
}

template < class TFixedImage, class TMovingImage  >
ITK_THREAD_RETURN_TYPE
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueThreadCallback( void * arg ) 
{
  int threadID;
  MultiThreaderParameterType * mtParam;

  threadID = ((MultiThreaderType::ThreadInfoStruct *)(arg))->ThreadID;

  mtParam = (MultiThreaderParameterType *)
            (((MultiThreaderType::ThreadInfoStruct *)(arg))->UserData);

  mtParam->metric->GetValueMultiThreadedInternal(threadID);

  return ITK_THREAD_RETURN_VALUE;
}
  
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueThreadedInitiate( void ) const
{
  m_Threader->SetSingleMethod( GetValueThreadCallback,
                               (void *)(&m_ThreaderParameter));
  m_Threader->SingleMethodExecute();
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::Initialize( void ) throw ( ExceptionObject )
{
  this->Superclass::Initialize();
  this->MultiThreadingInitialize();
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::MultiThreadingInitialize( void ) throw ( ExceptionObject )
{
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    PartialResultsType fresult;
    this->m_SumFixedPartialAResults.push_back( fresult );
    PartialResultsType mresult;
    this->m_SumMovingPartialAResults.push_back( mresult );
    PartialResultsType jresult;
    this->m_SumJointPartialAResults.push_back( jresult );
    }

  // These itertators point to the start/end A & B samples for each thread.
  this->m_SampleAStartIterators = new SamplesConstIterator[this->m_NumberOfThreads];
  this->m_SampleBStartIterators = new SamplesConstIterator[this->m_NumberOfThreads];
  this->m_SampleAEndIterators   = new SamplesConstIterator[this->m_NumberOfThreads];
  this->m_SampleBEndIterators   = new SamplesConstIterator[this->m_NumberOfThreads];

}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::ClearPartialResults( void ) const
{
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    this->m_SumFixedPartialAResults[t].clear();
    this->m_SumMovingPartialAResults[t].clear();
    this->m_SumJointPartialAResults[t].clear();
    }
}

} // end namespace itk


#endif
