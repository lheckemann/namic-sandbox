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
  this->m_TransformIsBSpline = false;
  this->m_TransformArray = NULL;
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
      (*iter).MovingImagePointValue = mappedPoint;
      this->m_NumberOfPixelsCounted++;
      allOutside = false;
      }
    else
      {
      (*iter).MovingImageValue = 0;
      (*iter).MovingImagePointValue = mappedPoint;
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
    std::cout << " FIX PUSH_BACK in GetValueMultiThreadedInternal " << std::endl;
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
  //this->GetValueAndDerivativeDefault( parameters, value, derivative );
  //this->GetValueAndDerivativeThreaded( parameters, value, derivative );
  this->GetValueAndDerivativeThreaded2( parameters, value, derivative );
}

/*
 * Get the both Value and Derivative Measure
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeDefault(
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
 * Get the both Value and Derivative Measure
 */
template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeThreaded(
  const ParametersType& parameters,
  MeasureType& value,
  DerivativeType& derivative) const
{
  // THREAD STRATEGY
  // Rework the GetValue threading implementation to return
  // dDenominatorMoving and dDenominatorJoint.
  // Then, split the A samples across threads again.
  // Accumulate the derivatives for each A sample in each thread.
  // Join the thread results at the end.
  // Need to cache one A derivative per thread to add when 
  // back to one thread.
  // Need to cache one "totalWeight" per B Sample per thread 
  // so that we can accumulate B sample weights across threads
  // and then use them to weight the B sample derivatives.


  // Check for b-spline transform...
  /** FIXME -- hardcoded spline order*/
  typedef BSplineDeformableTransform2< CoordinateRepresentationType,
                      ::itk::GetImageDimension<FixedImageType>::ImageDimension,
                                      3 >             BSplineTransformType;

  BSplineTransformType* bSplineTransformPtr = dynamic_cast<BSplineTransformType *>(
                                               m_Transform.GetPointer() );
  if (bSplineTransformPtr != NULL)
    {
    m_TransformIsBSpline = true;
    }

  value = NumericTraits< MeasureType >::Zero;
  unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();
  DerivativeType temp( numberOfParameters );
  // temp.Fill( 0 );
  memset( temp.data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  derivative = temp;

  // FIXME: Partial results containers need to be cleaned up.
  // FIXME: This probably could be done in each thread...
  this->ClearPartialResults();

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

  typename SpatialSampleContainer::iterator aiter;
  typename SpatialSampleContainer::const_iterator aend = m_SampleA.end();
  typename SpatialSampleContainer::iterator biter;
  typename SpatialSampleContainer::const_iterator bend = m_SampleB.end();

  // precalculate all the image derivatives for sample A
  typedef SparseVector< double > SparseDerivativeType;

  // typedef std::vector<DerivativeType> DerivativeContainer;
  typedef std::vector<SparseDerivativeType> DerivativeContainer;
  DerivativeContainer sampleADerivatives;
  sampleADerivatives.resize( m_NumberOfSpatialSamples );

  typename DerivativeContainer::iterator aditer;
  // SparseDerivativeType tempDeriv; //( numberOfParameters );

  // THREAD: This could be done in separate threads if CalculateDerivative is thread safe
#if 0
  for( aiter = m_SampleA.begin(), aditer = sampleADerivatives.begin();
       aiter != aend; ++aiter, ++aditer )
    {
    // THREAD: CalculateDerivatives is NOT THREAD SAFE
    //SparseDerivativeType tempDeriv;

    (*aditer).Fill( 0.0 );
    /*** FIXME: is there a way to avoid the extra copying step? *****/
    this->CalculateDerivativesThreadedBSplineSparse( (*aiter).FixedImagePointValue, 
                                                     (*aiter).MovingImagePointValue,
                                                     (*aditer) ) ; //tempDeriv );
    // (*aditer) = tempDeriv;
    }
#endif

  DerivativeType derivB(numberOfParameters);
  DerivativeType aTempDeriv( numberOfParameters );

  // THREAD: Use all B samples in each thread, as in GetValue
  for( biter = m_SampleB.begin(); biter != bend; ++biter )
    {
    double dDenominatorMoving = m_MinProbability;
    double dDenominatorJoint = m_MinProbability;

    double dSumFixed = m_MinProbability;

    // THREAD: Split A samples across threads.
    // THREAD: This looks like GetValue...
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

    // THREAD: See if up to here matches GetValue.

    // THREAD: Calculate derivatives is NOT THREAD SAFE
    // get the image derivative for this B sample
    this->CalculateDerivativesThreaded( (*biter).FixedImagePointValue, 
                                        (*biter).MovingImagePointValue, 
                                        derivB );

    double totalWeight = 0.0;

    // THREAD: This is the second pass through the A samples, 
    // THREAD: inside a bsample loop.
    /*
    for( aiter = m_SampleA.begin(), aditer = sampleADerivatives.begin();
         aiter != aend; ++aiter, ++aditer )
    */
    for( aiter = m_SampleA.begin();
         aiter != aend; ++aiter)
      {
      double valueFixed;
      double valueMoving;
      double weightMoving;
      double weightJoint;
      double weight;

      // THREAD: valueFixed and valueMoving look the same as GetValue?
      valueFixed = ( (*biter).FixedImageValue - (*aiter).FixedImageValue ) /
        m_FixedImageStandardDeviation;
      valueFixed = m_KernelFunction->Evaluate( valueFixed );

      valueMoving = ( (*biter).MovingImageValue - (*aiter).MovingImageValue ) /
        m_MovingImageStandardDeviation;
      valueMoving = m_KernelFunction->Evaluate( valueMoving );

      // THREAD: weightMoving, weightJoint new to GetValueAndDerivative
      // dDenomiatorMoving and dDenominatorJoint need a pass through
      // all the A samples to compute
      weightMoving = valueMoving / dDenominatorMoving;
      weightJoint = valueMoving * valueFixed / dDenominatorJoint;

      // THREAD: weight is new to GetValueAndDerivative
      // weight = valueMoving / dDenomiatorMoving - (valueMoving * valueFixed) / dDenominatorJoint;
      // weight = (valueMoving * dDenominatorJoint - (valueMoving * valueFixed) * dDenominatorMoving)/( dDenominatorMoving * dDenominatorJoint )
      weight = ( weightMoving - weightJoint );
      weight *= (*biter).MovingImageValue - (*aiter).MovingImageValue;

      totalWeight += weight;

      // THREAD: EXPENSIVE!!!      
      // No caching
      this->CalculateDerivativesThreaded( (*aiter).FixedImagePointValue, 
                                          (*aiter).MovingImagePointValue,
                                          aTempDeriv );
      derivative -= aTempDeriv * weight;

      // Sparse caching
      /*
      (*aditer) *= weight;
      (*aditer).SubtractFromNonSparse( derivative );
      */

      // Default: 
      //derivative -= (*aditer) * weight;
      } // end of sample A loop

    // THREAD: Derivative could be accumulated across B samples
    // THREAD: Or across A samples.
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
    //derivatives.Fill( 0.0 );
    memset( derivatives.data_block(),
            0,
            this->m_NumberOfParameters * sizeof(double) );

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

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::CalculateDerivativesThreaded(
  const FixedImagePointType& point,
  const MovingImagePointType& mappedPoint,
  DerivativeType& derivatives,
  unsigned int threadID ) const
{
  // THREAD: To be thread safe
  // THREAD: m_Transform->TransformPoint needs to be thread safe (NOT OK)
  //            BSpline transform point may not be thread safe
  //              Non-caching BSpline may be thread safe.
  // THREAD: m_Transform->GetJacobian( point ) needs to be thread safe. (???)
  // THREAD: m_Transform->GetNumberOfParameters() needs to be thread safe.
  // THREAD: that m_DerivativeCalculator->IsInsideBuffer needs to be thread safe (LOOKS OK)
  // THREAD: that m_DerivativeCalculator->Evaluate needs to be thread safe (LOOKS OK).
  //            ConvertPointToNearestIndex - Look ok
  //            EvaluateAtIndex - Look ok
  // THREAD: Is mappedPoint already available somewhere else?
  //            Computed during sampling, but thrown out.


  // THREAD: We will keep the mapping from when the samples are drawn, so 
  // we don't need to recompute it here.
  // MovingImagePointType mappedPoint = this->m_Transform->TransformPoint( point );
  
  CovariantVector<double,MovingImageDimension> imageDerivatives;

  if ( m_DerivativeCalculator->IsInsideBuffer( mappedPoint ) )
    {
    imageDerivatives = m_DerivativeCalculator->Evaluate( mappedPoint );
    }
  else
    {
    // THREAD: Do memcopy -- faster
    //derivatives.Fill( 0.0 );
    memset( derivatives.data_block(),
            0,
            this->m_NumberOfParameters * sizeof(double) );

    return;
    }

  typedef typename TransformType::JacobianType JacobianType;

  if ( true == m_TransformIsBSpline )
    {
    /** FIXME -- hardcoded spline order*/
    typedef BSplineDeformableTransform2< CoordinateRepresentationType,
                        ::itk::GetImageDimension<FixedImageType>::ImageDimension,
                                        3 >             BSplineTransformType;

    BSplineTransformType* bSplineTransformPtr = dynamic_cast<BSplineTransformType *>(
                                                               m_TransformArray[threadID].GetPointer() );
    //                                                         m_Transform.GetPointer() );

    int numberOfContributions = bSplineTransformPtr->GetNumberOfAffectedWeights();
    int numberOfParametersPerDimension = bSplineTransformPtr->GetNumberOfParametersPerDimension();

    // THREAD: Need thread safe GetJacobian( point )
    typedef BSplineTransformType::WeightsType JacobianValueArrayType;
    JacobianValueArrayType jacobianValues(numberOfContributions);
    
    typedef BSplineTransformType::ParameterIndexArrayType JacobianIndexType;
    JacobianIndexType jacobianIndices(numberOfContributions);
    //jacobianIndices.set_size(numberOfContributions);

    // FIXME -- Need to duplicate transforms for GetJacobian to be thread safe
    bSplineTransformPtr->GetJacobian( point, 
                                      jacobianValues,
                                      jacobianIndices 
                                      );

    memset( derivatives.data_block(),
            0,
            this->m_NumberOfParameters * sizeof(double) );


    for ( unsigned int k = 0; k < numberOfContributions; k++ )
      {
      for ( unsigned int j = 0; j < MovingImageDimension; j++ )
        {
        // Need contribution indices to be a map into the derivative index...
        // Derivatives here is NOT sparse...
        // derivatives[ jacobianIndices[ k ] ] += jacobianValues[k][j] * imageDerivatives[j];

        // Serdar: 
        derivatives[j*numberOfParametersPerDimension + jacobianIndices[k]] += jacobianValues[k] * imageDerivatives[j];
        }
      }

    }
  else // Default implementation
    {
    // THREAD: Need thread safe GetJacobian( point )
    // FIXME -- Need to duplicate transforms for GetJacobian to be thread safe
    // const JacobianType& jacobian = this->m_Transform->GetJacobian( point );
    const JacobianType& jacobian = this->m_TransformArray[threadID]->GetJacobian( point );

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
  /*
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    PartialResultsType fresult;
    this->m_SumFixedPartialAResults.push_back( fresult );
    PartialResultsType mresult;
    this->m_SumMovingPartialAResults.push_back( mresult );
    PartialResultsType jresult;
    this->m_SumJointPartialAResults.push_back( jresult );
    }
  */
  this->m_SumFixedPartialAResults.resize( m_NumberOfThreads );
  this->m_SumMovingPartialAResults.resize( m_NumberOfThreads );
  this->m_SumJointPartialAResults.resize( m_NumberOfThreads );

  /** Move to each thread? */
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    this->m_SumFixedPartialAResults[t].resize( this->m_NumberOfSpatialSamples );
    this->m_SumMovingPartialAResults[t].resize( this->m_NumberOfSpatialSamples );
    this->m_SumJointPartialAResults[t].resize( this->m_NumberOfSpatialSamples );    
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

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::CalculateDerivativesThreadedBSplineSparse(
  const FixedImagePointType& point,
  const MovingImagePointType& mappedPoint,
  SparseVector<double>& derivatives ) const
{
  // THREAD: To be thread safe
  // THREAD: m_Transform->TransformPoint needs to be thread safe (NOT OK)
  //            BSpline transform point may not be thread safe
  //              Non-caching BSpline may be thread safe.
  // THREAD: m_Transform->GetJacobian( point ) needs to be thread safe. (???)
  // THREAD: m_Transform->GetNumberOfParameters() needs to be thread safe.
  // THREAD: that m_DerivativeCalculator->IsInsideBuffer needs to be thread safe (LOOKS OK)
  // THREAD: that m_DerivativeCalculator->Evaluate needs to be thread safe (LOOKS OK).
  //            ConvertPointToNearestIndex - Look ok
  //            EvaluateAtIndex - Look ok
  // THREAD: Is mappedPoint already available somewhere else?
  //            Computed during sampling, but thrown out.


  // THREAD: We will keep the mapping from when the samples are drawn, so 
  // we don't need to recompute it here.
  // MovingImagePointType mappedPoint = this->m_Transform->TransformPoint( point );
  
  CovariantVector<double,MovingImageDimension> imageDerivatives;

  if ( m_DerivativeCalculator->IsInsideBuffer( mappedPoint ) )
    {
    imageDerivatives = m_DerivativeCalculator->Evaluate( mappedPoint );
    }
  else
    {
    memset( derivatives.data_block(),
            0,
            this->m_NumberOfParameters * sizeof(double) );

    return;
    }

  typedef typename TransformType::JacobianType JacobianType;

  /** FIXME -- hardcoded spline order*/
  typedef BSplineDeformableTransform2< CoordinateRepresentationType,
                      ::itk::GetImageDimension<FixedImageType>::ImageDimension,
                                      3 >             BSplineTransformType;

  // THREAD: We know before calling this method that the transform is a b-spline
  BSplineTransformType* bSplineTransformPtr = dynamic_cast<BSplineTransformType *>(
                                                           m_Transform.GetPointer() );

  int numberOfContributions = bSplineTransformPtr->GetNumberOfAffectedWeights();
  int numberOfParametersPerDimension = bSplineTransformPtr->GetNumberOfParametersPerDimension();

  // THREAD: Need thread safe GetJacobian( point )
  typedef BSplineTransformType::WeightsType JacobianValueArrayType;
  JacobianValueArrayType jacobianValues(numberOfContributions);
  
  typedef BSplineTransformType::ParameterIndexArrayType JacobianIndexType;
  JacobianIndexType jacobianIndices(numberOfContributions);

  bSplineTransformPtr->GetJacobian( point, 
                                    jacobianValues,
                                    jacobianIndices 
                                    );

  memset( derivatives.data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  for ( unsigned int k = 0; k < numberOfContributions; k++ )
    {
    for ( unsigned int j = 0; j < MovingImageDimension; j++ )
      {
      // Need contribution indices to be a map into the derivative index...

      // From Serdar's code 
      unsigned int derivativeIndex = j*numberOfParametersPerDimension + jacobianIndices[k];
      derivatives[derivativeIndex] += jacobianValues[k] * imageDerivatives[j];
      }
    }

}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::CalculateDerivativesThreadedDefaultTransform(
  const FixedImagePointType&  point,
  const MovingImagePointType& mappedPoint,
  DerivativeType&             derivatives ) const
{
  // THREAD: To be thread safe
  // THREAD: m_Transform->TransformPoint needs to be thread safe (NOT OK)
  //            BSpline transform point may not be thread safe
  //              Non-caching BSpline may be thread safe.
  // THREAD: m_Transform->GetJacobian( point ) needs to be thread safe. (**LOOK AT**)
  // THREAD: m_Transform->GetNumberOfParameters() needs to be thread safe.
  // THREAD: that m_DerivativeCalculator->IsInsideBuffer needs to be thread safe (LOOKS OK)
  // THREAD: that m_DerivativeCalculator->Evaluate needs to be thread safe (LOOKS OK).
  //            ConvertPointToNearestIndex - Look ok
  //            EvaluateAtIndex - Look ok
  // THREAD: Is mappedPoint already available somewhere else?
  //            Computed during sampling, but thrown out.


  // THREAD: We will keep the mapping from when the samples are drawn, so 
  // we don't need to recompute it here.
  // MovingImagePointType mappedPoint = this->m_Transform->TransformPoint( point );
  
  CovariantVector<double,MovingImageDimension> imageDerivatives;

  if ( m_DerivativeCalculator->IsInsideBuffer( mappedPoint ) )
    {
    imageDerivatives = m_DerivativeCalculator->Evaluate( mappedPoint );
    }
  else
    {
    // THREAD: Do memcopy -- faster
    // derivatives.Fill( 0.0 );
    memset( derivatives.data_block(),
            0,
            this->m_NumberOfParameters * sizeof(double) );

    return;
    }

  typedef typename TransformType::JacobianType JacobianType;

  // THREAD: Need thread safe GetJacobian( point )
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

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeThreaded2(
  const ParametersType& parameters,
  MeasureType& value,
  DerivativeType& derivative) const
{
  // THREAD STRATEGY
  // Rework the GetValue threading implementation to return
  // dDenominatorMoving and dDenominatorJoint.
  // Then, split the A samples across threads again.
  // Accumulate the derivatives for each A sample in each thread.
  // Join the thread results at the end.
  // Need to cache one A derivative per thread to add when 
  // back to one thread.
  // Need to cache one "totalWeight" per B Sample per thread 
  // so that we can accumulate B sample weights across threads
  // and then use them to weight the B sample derivatives.
  value = NumericTraits< MeasureType >::Zero;
  m_NumberOfParameters = this->m_Transform->GetNumberOfParameters();
  DerivativeType temp( m_NumberOfParameters );
  //temp.Fill( 0 );
  memset( temp.data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  derivative = temp;


  // Check for b-spline transform...
  /** FIXME -- hardcoded spline order*/
  typedef BSplineDeformableTransform2< CoordinateRepresentationType,
                      ::itk::GetImageDimension<FixedImageType>::ImageDimension,
                                      3 >             BSplineTransformType;

  BSplineTransformType* bSplineTransformPtr = dynamic_cast<BSplineTransformType *>(
                                               m_Transform.GetPointer() );
  if (bSplineTransformPtr != NULL)
    {
    m_TransformIsBSpline = true;
    }

  // make sure the transform has the current parameters
  this->m_Transform->SetParameters( parameters );
  //this->SetTransformParameters( parameters );
  this->SetupThreadTransforms( );
  this->SynchronizeTransforms();

  // set the DerivativeCalculator
  m_DerivativeCalculator->SetInputImage( this->m_MovingImage );

  // collect sample set A
  this->SampleFixedImageDomain( m_SampleA );

  // collect sample set B
  this->SampleFixedImageDomain( m_SampleB );

  // Setup the thread iterators for the A samples.
  this->SetupSampleThreadIterators();

  // DEBUG
  this->SetupDerivativePartialResults();

  if (false == this->ValidatePartialResultSizes())
    {
    std::cerr << "Partial result size error before phase 1" << std::endl;
    }

  // START THREADING, SPAWN THREADS HERE
#ifdef SYNCHRONOUS_COMPUTATION // For debug
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    this->GetValueAndDerivativeMultiThreadedInternalPhase1( t );
    }
#else
  this->GetValueAndDerivativePhase1ThreadedInitiate();
#endif

  this->GetValueAndDerivativeMultiThreadedInternalPhase1Combine( value );

  if (false == this->ValidatePartialResultSizes())
    {
    std::cerr << "Partial result size error after phase 1" << std::endl;
    }

  // this->SetupDerivativePartialResults();

#ifdef SYNCHRONOUS_COMPUTATION // For debug
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    this->GetValueAndDerivativeMultiThreadedInternalPhase2( t );
    }
#else
  this->GetValueAndDerivativePhase2ThreadedInitiate();
#endif

  this->GetValueAndDerivativeMultiThreadedInternalPhase2Combine( derivative );

  if (false == this->ValidatePartialResultSizes())
    {
    std::cerr << "Partial result size error after phase 2" << std::endl;
    }

  this->SetupPhase3BSampleIterators();

#ifdef SYNCHRONOUS_COMPUTATION // For debug
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    this->GetValueAndDerivativeMultiThreadedInternalPhase3( t );
    }
#else
  this->GetValueAndDerivativePhase3ThreadedInitiate();
#endif

  this->GetValueAndDerivativeMultiThreadedInternalPhase3Combine( derivative );

  if (false == this->ValidatePartialResultSizes())
    {
    std::cerr << "Partial result size error after phase 3" << std::endl;
    }

}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::SetupSampleThreadIterators() const
{
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
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeMultiThreadedInternalPhase1( unsigned int threadID ) const
{
  std::stringstream msg;
  msg << "Phase 1 - thread " << threadID << "\n";
  std::cout << msg.str();

  // THREAD: FIXME iterators need to use samples based on threads.
  SamplesConstIterator aiter;
  SamplesConstIterator astart    = m_SampleAStartIterators[threadID];
  SamplesConstIterator aend = m_SampleAEndIterators[threadID];
  SamplesConstIterator biter;
  SamplesConstIterator bstart    = m_SampleBStartIterators[threadID];
  SamplesConstIterator bend = m_SampleBEndIterators[threadID];

  // precalculate all the image derivatives for sample A
  typedef std::vector<DerivativeType> DerivativeContainer;
  DerivativeContainer sampleADerivatives;
  sampleADerivatives.resize( m_NumberOfSpatialSamples );

  //unsigned int numberOfParameters = this->m_Transform->GetNumberOfParameters();
  DerivativeType derivA( m_NumberOfParameters );
  DerivativeType derivB( m_NumberOfParameters );

  // Break into two B sample loops?
  unsigned int bSampleCount = 0;
  for( biter = bstart; biter != bend; ++biter, ++bSampleCount )
    {
    double dDenominatorMoving = m_MinProbability;
    double dDenominatorJoint  = m_MinProbability;
    double dSumFixed          = m_MinProbability;

    for( aiter = astart; aiter != aend; ++aiter )
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
      dDenominatorJoint  += valueMoving * valueFixed;

      dSumFixed += valueFixed;

      } // end of sample A loop

    // Cache results across threads.
    /*
    m_SumFixedPartialAResults[threadID].push_back( dSumFixed );
    m_SumMovingPartialAResults[threadID].push_back( dDenominatorMoving );
    m_SumJointPartialAResults[threadID].push_back( dDenominatorJoint );
    */
    m_SumFixedPartialAResults[threadID][bSampleCount] = dSumFixed ;
    m_SumMovingPartialAResults[threadID][bSampleCount] = dDenominatorMoving;
    m_SumJointPartialAResults[threadID][bSampleCount] = dDenominatorJoint;

    } // end of sample B loop


}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeMultiThreadedInternalPhase1Combine(MeasureType& measure) const
{
  std::cout << "Phase 1 combine " << std::endl;

  unsigned int numBSampleResults = m_SumFixedPartialAResults[0].size();

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

  //MeasureType measure = dLogSumFixed + dLogSumMoving - dLogSumJoint;
  measure = dLogSumFixed + dLogSumMoving - dLogSumJoint;
  measure /= nsamp;
  measure += vcl_log(nsamp );

}

template < class TFixedImage, class TMovingImage  >
double
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::CombineASampleResults()
{
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
::GetValueAndDerivativeMultiThreadedInternalPhase2( unsigned int threadID ) const
{
  std::stringstream msg;
  msg << "Phase 2 - thread " << threadID << "\n";
  std::cout << msg.str();

  DerivativeType derivA( this->m_NumberOfParameters );

  // THREAD: Set derivative for this thread to 0.
  memset( this->m_ThreadDerivatives[threadID].data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  // THREAD: Iterators use samples based on thread ID.
  SamplesConstIterator aiter;
  SamplesConstIterator astart    = m_SampleAStartIterators[threadID];
  SamplesConstIterator aend      = m_SampleAEndIterators[threadID];
  SamplesConstIterator biter;
  SamplesConstIterator bstart    = m_SampleBStartIterators[threadID];
  SamplesConstIterator bend      = m_SampleBEndIterators[threadID];

  // THREAD: Results combined across previous threads are in threadID 0.
  PartialResultsType::iterator miter = m_SumMovingPartialAResults[ 0 ].begin();
  PartialResultsType::iterator jiter = m_SumJointPartialAResults[ 0 ].begin();

  // Alias the thread derivative so that we don't have to 
  // alway dereference.
  DerivativeType& threadDerivative = this->m_ThreadDerivatives[ threadID ];

  unsigned int bSampleCount = 0;
  for( biter = bstart; biter != bend; ++biter, ++miter, ++jiter, ++bSampleCount )
    {
    double totalWeight = 0.0;

    // THREAD: Get these from partial results...
    double dDenominatorMoving = *miter;
    double dDenominatorJoint  = *jiter;

    for( aiter = astart; 
         aiter != aend; 
         ++aiter )
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

      // THREAD: Denominators are functions of each b sample and all a samples.
      // The denominators were precomputed and cached.
      weightMoving = valueMoving / dDenominatorMoving;
      weightJoint = valueMoving * valueFixed / dDenominatorJoint;

      weight = ( weightMoving - weightJoint );
      weight *= (*biter).MovingImageValue - (*aiter).MovingImageValue;

      totalWeight += weight;

      this->CalculateDerivativesThreaded( (*aiter).FixedImagePointValue, 
                                          (*aiter).MovingImagePointValue, 
                                          derivA,
                                          threadID );

      // THREAD: Just need to make sure that weight is correct and that the 
      // thread derivative is initialized correctly. We will combine them
      // later.
      //derivative -= derivA * weight;
      //this->m_ThreadDerivatives[ threadID ] -= derivA * weight;
      threadDerivative -= derivA * weight;

      } // end of sample A loop

    // THREAD: Cache totalWeight per b-sample totalWeight is summed over all 
    //         A samples, so one totalWeight per B sample per thread. 
    //         Need to combine over threads later.
    this->m_TotalWeightBSamplePartialResult[ threadID ][bSampleCount] = totalWeight;
    } // end of sample B loop
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeMultiThreadedInternalPhase2Combine( DerivativeType& derivative ) const
{
  std::cout << "Phase 2 combine." <<  std::endl;

  memset( derivative.data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  // THREAD: Combine derivatives across threads.
  for (unsigned int t = 0; t < this->m_NumberOfThreads; t++)
    {
    derivative += this->m_ThreadDerivatives[ t ];
    }

  // THREAD: Different start index than previous loop
  for (unsigned int t = 1; t < this->m_NumberOfThreads; t++)
    {
    // THREAD: Combine totalWeight across threads for each B sample.
    for (unsigned int bSample = 0; 
          bSample < this->m_NumberOfSpatialSamples;
          bSample++)
      {
      m_TotalWeightBSamplePartialResult[0][bSample]  += m_TotalWeightBSamplePartialResult[t][bSample];
      }

    }

}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::SetupDerivativePartialResults() const
{
  this->m_TotalWeightBSamplePartialResult.resize( this->m_NumberOfThreads );

  for (unsigned int t = 0; t < this->m_NumberOfThreads; t++)
    {
    this->m_TotalWeightBSamplePartialResult[ t ].resize( this->m_NumberOfSpatialSamples );
    }

  this->m_ThreadDerivatives.resize( this->m_NumberOfThreads, 
                                    DerivativeType( this->m_NumberOfParameters ) );
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::SetupPhase3BSampleIterators() const
{
  this->m_SampleBPhase3StartIterators.resize( this->m_NumberOfThreads );
  this->m_SampleBPhase3EndIterators.resize( this->m_NumberOfThreads );

  // Setup the sample iterators for each thread.
  unsigned int samplesPerThread  = static_cast< unsigned int >( 
    vcl_floor(static_cast<double>(this->m_NumberOfSpatialSamples) /
              static_cast<double>(m_NumberOfThreads)) );

  // The last thread needs to handle more/fewer samples depending on how the 
  // samples are divided by the threads.
  unsigned int lastThreadSamples = this->m_NumberOfSpatialSamples - (m_NumberOfThreads -1) * samplesPerThread;

  // Sample iterators need to point 1 beyond the end of the samples.
  for (unsigned int t = 0; t < m_NumberOfThreads - 1; t++)
    {
    this->m_SampleBPhase3StartIterators[t] = m_SampleB.begin() + t * samplesPerThread;
    this->m_SampleBPhase3EndIterators[t]   = m_SampleB.begin() + (t + 1) * samplesPerThread;
    }

  //  Set the iterators for the last thread. Last thread 
  // needs to account for samples that remain when dividing by the 
  // number of threads.
  this->m_SampleBPhase3StartIterators[m_NumberOfThreads - 1] = m_SampleB.begin() + (m_NumberOfThreads - 1) * samplesPerThread;
  this->m_SampleBPhase3EndIterators[m_NumberOfThreads - 1] = m_SampleB.begin() + (m_NumberOfThreads - 1) * samplesPerThread + lastThreadSamples;

}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeMultiThreadedInternalPhase3( unsigned int threadID ) const
{
  std::stringstream msg;
  msg << "Phase 1 - thread " << threadID << "\n";
  std::cout << msg.str();

  memset( this->m_ThreadDerivatives[threadID].data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  DerivativeType derivB( this->m_NumberOfParameters );
  memset( derivB.data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  // Alias for convenience
  DerivativeType& threadDerivatives = this->m_ThreadDerivatives[ threadID ];

  SamplesConstIterator biter;
  SamplesConstIterator bstart = this->m_SampleBPhase3StartIterators[threadID];
  SamplesConstIterator bend   = this->m_SampleBPhase3EndIterators[threadID];

  std::vector<double>::iterator twiter = this->m_TotalWeightBSamplePartialResult[ threadID ].begin();

  for( biter = bstart; biter != bend; ++biter, ++twiter )
    {
    // get the image derivative for this B sample
    this->CalculateDerivativesThreaded( (*biter).FixedImagePointValue, 
                                        (*biter).MovingImagePointValue, 
                                        derivB,
                                        threadID );
    
    // totalWeight is summed over all A samples, so one totalWeight per B sample
    
    double totalWeight = *twiter;
    //this->m_ThreadDerivatives[ threadID ] += derivB * totalWeight;
    threadDerivatives += derivB * totalWeight;
    }

}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativeMultiThreadedInternalPhase3Combine( DerivativeType& derivative ) const
{
  std::cout << "Phase 3 Combine " << std::endl;

  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    derivative += this->m_ThreadDerivatives[ t ];
    }
}

template < class TFixedImage, class TMovingImage  >
ITK_THREAD_RETURN_TYPE
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativePhase1ThreadCallback( void * arg ) 
{
  int threadID;
  MultiThreaderParameterType * mtParam;

  threadID = ((MultiThreaderType::ThreadInfoStruct *)(arg))->ThreadID;

  mtParam = (MultiThreaderParameterType *)
            (((MultiThreaderType::ThreadInfoStruct *)(arg))->UserData);

  mtParam->metric->GetValueAndDerivativeMultiThreadedInternalPhase1(threadID);

  return ITK_THREAD_RETURN_VALUE;
}

template < class TFixedImage, class TMovingImage  >
ITK_THREAD_RETURN_TYPE
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativePhase2ThreadCallback( void * arg ) 
{
  int threadID;
  MultiThreaderParameterType * mtParam;

  threadID = ((MultiThreaderType::ThreadInfoStruct *)(arg))->ThreadID;

  mtParam = (MultiThreaderParameterType *)
            (((MultiThreaderType::ThreadInfoStruct *)(arg))->UserData);

  mtParam->metric->GetValueAndDerivativeMultiThreadedInternalPhase2(threadID);

  return ITK_THREAD_RETURN_VALUE;
}

template < class TFixedImage, class TMovingImage  >
ITK_THREAD_RETURN_TYPE
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativePhase3ThreadCallback( void * arg ) 
{
  int threadID;
  MultiThreaderParameterType * mtParam;

  threadID = ((MultiThreaderType::ThreadInfoStruct *)(arg))->ThreadID;

  mtParam = (MultiThreaderParameterType *)
            (((MultiThreaderType::ThreadInfoStruct *)(arg))->UserData);

  mtParam->metric->GetValueAndDerivativeMultiThreadedInternalPhase3(threadID);

  return ITK_THREAD_RETURN_VALUE;
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativePhase1ThreadedInitiate( void ) const
{
  m_Threader->SetSingleMethod( GetValueAndDerivativePhase1ThreadCallback,
                               (void *)(&m_ThreaderParameter));
  m_Threader->SingleMethodExecute();
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativePhase2ThreadedInitiate( void ) const
{
  m_Threader->SetSingleMethod( GetValueAndDerivativePhase2ThreadCallback,
                               (void *)(&m_ThreaderParameter));
  m_Threader->SingleMethodExecute();
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivativePhase3ThreadedInitiate( void ) const
{
  m_Threader->SetSingleMethod( GetValueAndDerivativePhase3ThreadCallback,
                               (void *)(&m_ThreaderParameter));
  m_Threader->SingleMethodExecute();
}

template < class TFixedImage, class TMovingImage  >
bool
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::ValidatePartialResultSizes() const
{
  for (unsigned int t = 0; t < m_NumberOfThreads; t++)
    {
    if ( this->m_NumberOfSpatialSamples !=  this->m_SumFixedPartialAResults[t].size() )
      {
      return false;
      }
    if ( this->m_NumberOfSpatialSamples != this->m_SumMovingPartialAResults[t].size() )
      {
      return false;
      }
    if ( this->m_NumberOfSpatialSamples != this->m_SumJointPartialAResults[t].size() )
      {
      return false;
      }
    if ( this->m_NumberOfSpatialSamples != this->m_TotalWeightBSamplePartialResult[t].size() )
      {
      return false;
      }
    }

  return true;
}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::SetupThreadTransforms( ) const
{
  if ( NULL == this->m_TransformArray )
    {
    this->m_TransformArray = new TransformPointer[ m_NumberOfThreads ];
    }

  // So we don't have to check other places if we're in thread 0 or not.
  this->m_TransformArray[0] = this->m_Transform.GetPointer();

  for( unsigned int ithread=1; ithread < m_NumberOfThreads; ++ithread)
    {
    // Create a copy of the main transform to be used in this thread.
    itk::LightObject::Pointer anotherTransform = this->m_Transform->CreateAnother();
    // This static_cast should always work since the pointer was created by
    // CreateAnother() called from the transform itself.
    TransformType * transformCopy = static_cast< TransformType * >( anotherTransform.GetPointer() );
    /** Set the fixed parameters first. Some transforms have parameters which depend on 
        the values of the fixed parameters. For instance, the BSplineDeformableTransform
        checks the grid size (part of the fixed parameters) before setting the parameters. */
    transformCopy->SetFixedParameters( this->m_Transform->GetFixedParameters() );
    transformCopy->SetParameters( this->m_Transform->GetParameters() );
    this->m_TransformArray[ithread] = transformCopy;
    }

}

template < class TFixedImage, class TMovingImage  >
void
MultiThreadedMutualInformationImageToImageMetric<TFixedImage,TMovingImage>
::SynchronizeTransforms() const
{
  for( unsigned int threadID = 1; threadID<m_NumberOfThreads; threadID++ )
    {
    /** Set the fixed parameters first. Some transforms have parameters which depend on 
        the values of the fixed parameters. For instance, the BSplineDeformableTransform
        checks the grid size (part of the fixed parameters) before setting the parameters. */
    this->m_TransformArray[threadID]->SetFixedParameters( this->m_Transform->GetFixedParameters() );
    this->m_TransformArray[threadID]->SetParameters( this->m_Transform->GetParameters() );
    }
}



} // end namespace itk

#endif
