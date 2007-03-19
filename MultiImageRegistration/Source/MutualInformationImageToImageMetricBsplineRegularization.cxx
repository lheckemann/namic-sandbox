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
#ifndef _MutualInformationImageToImageMetricBsplineRegularization_cxx
#define _MutualInformationImageToImageMetricBsplineRegularization_cxx

#include "MutualInformationImageToImageMetricBsplineRegularization.h"
#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "vnl/vnl_math.h"
#include "itkGaussianKernelFunction.h"

namespace itk
{

/*
 * Constructor
 */
template < class TFixedImage, class TMovingImage >
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
::MutualInformationImageToImageMetricBsplineRegularization()
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

  int m_NumberOfThreads = 1;
  int m_NumberOfImages = 1;

  m_Regularization = false;
  m_RegularizationFactor = 1e-5;
}

/*
 * Initialize
 */
template < class TFixedImage, class TMovingImage > 
void
MutualInformationImageToImageMetricBsplineRegularization< TFixedImage, TMovingImage >
::Initialize(void) throw ( ExceptionObject )
{

  //First intialize the superclass
  Superclass::Initialize();

 
  //this->m_NumberOfPixelsCounted = m_NumberOfSpatialSamples;
  
  // reinitilize the seed for the random iterator
  this->ReinitializeSeed();

  // Sample the image domain
  this->SampleFixedImageDomain(m_SampleA);
  this->SampleFixedImageDomain(m_SampleB);
  
  // Initialize the variables for regularization term
  if( m_Regularization &&
      strcmp(this->m_Transform->GetNameOfClass(), "BSplineDeformableTransform") )
  {
    itkExceptionMacro(<<"Cannot use regularization with transforms" <<
        " other than BSplineDeformableTransform" );
  }

  // If using regularization check that the Bspline Transform is supplied
  if( m_Regularization &&
      !strcmp(this->m_Transform->GetNameOfClass(), "BSplineDeformableTransform") )
  {
    if( !m_BSplineTransform )
    {
      itkExceptionMacro(<<"Bspline Transform Array not initialized" );
    }
      
    if( (void *) m_BSplineTransform != (void*)this->m_Transform )
    {
      itkExceptionMacro(<<"While using Bspline regularization transform array and Bspline array should have the pointers to the same transform" );
    }
  }

  
  
  //Prepare the gradient filters if Regularization is on
  if(m_Regularization)
  {

    //Prepare hessian filters
    m_BSplineGradientArray.resize(1);
    m_BSplineHessianArray.resize(1);
    m_BSplineGradientImagesArray.resize(1);
    for(int i=0; i<1; i++)
    {
      m_BSplineGradientArray[i].resize(MovingImageType::ImageDimension);
      m_BSplineHessianArray[i].resize(MovingImageType::ImageDimension);
      m_BSplineGradientImagesArray[i].resize(MovingImageType::ImageDimension);

      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {
        // Connect bspline coeff images to gradient filters
        m_BSplineGradientArray[i][j] = GradientFilterType::New();
        m_BSplineGradientArray[i][j]->SetInput(m_BSplineTransform->GetCoefficientImage()[j]);
        m_BSplineGradientArray[i][j]->Update();

        m_BSplineHessianArray[i][j].resize(MovingImageType::ImageDimension);
        m_BSplineGradientImagesArray[i][j].resize(MovingImageType::ImageDimension);
        
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          // allocate the gradient images which hold k'th image of the graadient of the
          // Bspline coeff image
          m_BSplineGradientImagesArray[i][j][k] = BSplineParametersImageType::New();
          m_BSplineGradientImagesArray[i][j][k]->SetRegions( m_BSplineTransform->GetGridRegion() );
          m_BSplineGradientImagesArray[i][j][k]->Allocate();
          m_BSplineGradientImagesArray[i][j][k]->SetSpacing( m_BSplineTransform->GetGridSpacing() );
          m_BSplineGradientImagesArray[i][j][k]->SetOrigin( m_BSplineTransform->GetGridOrigin() );
          m_BSplineGradientImagesArray[i][j][k]->FillBuffer( 0.0 );

          // connect the bspline gradient images
          // to gradient filter to compute hessian
          m_BSplineHessianArray[i][j][k] = GradientFilterType::New();
          m_BSplineHessianArray[i][j][k]->SetInput(m_BSplineGradientImagesArray[i][j][k]);

        }
      }
    }

    // Allocate the images for cost function derivative update
    m_BSplineGradientUpdateImagesArray.resize(MovingImageType::ImageDimension);
    m_BSplineGradientUpdateArray.resize(MovingImageType::ImageDimension);
    for (int i=0; i< MovingImageType::ImageDimension; i++)
    {
      m_BSplineGradientUpdateImagesArray[i].resize(MovingImageType::ImageDimension);
      m_BSplineGradientUpdateArray[i].resize(MovingImageType::ImageDimension);
      for( int j=0; j<= i; j++ )
      {
        m_BSplineGradientUpdateImagesArray[i][j].resize(1);
        m_BSplineGradientUpdateArray[i][j].resize(1);
        
        // separate for each thread
        for(int k=0; k<1; k++)
        {
          m_BSplineGradientUpdateImagesArray[i][j][k] = BSplineParametersImageType::New();
          m_BSplineGradientUpdateImagesArray[i][j][k]->SetRegions( m_BSplineTransform->GetGridRegion() );
          m_BSplineGradientUpdateImagesArray[i][j][k]->Allocate();
          m_BSplineGradientUpdateImagesArray[i][j][k]->SetSpacing( m_BSplineTransform->GetGridSpacing() );
          m_BSplineGradientUpdateImagesArray[i][j][k]->SetOrigin( m_BSplineTransform->GetGridOrigin() );
          m_BSplineGradientUpdateImagesArray[i][j][k]->FillBuffer( 0.0 );

        // connect the bspline gradient images
          m_BSplineGradientUpdateArray[i][j][k] = GradientFilterType::New();
          m_BSplineGradientUpdateArray[i][j][k]->SetInput(m_BSplineGradientUpdateImagesArray[i][j][k]);
          m_BSplineGradientUpdateArray[i][j][k]->Update();
        
        }
      }
    }
  }
  
}

template < class TFixedImage, class TMovingImage  >
void
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
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
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
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
 */
template < class TFixedImage, class TMovingImage  >
void
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
::SampleFixedImageDomain(
  SpatialSampleContainer& samples ) const
{

  typedef ImageRandomConstIteratorWithIndex<FixedImageType> RandomIterator;
  RandomIterator randIter( this->m_FixedImage, this->GetFixedImageRegion() );

  randIter.SetNumberOfSamples( m_NumberOfSpatialSamples );
  randIter.GoToBegin();

  typename SpatialSampleContainer::iterator iter;
  typename SpatialSampleContainer::const_iterator end = samples.end();

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


/*
 * Get the match Measure
 */
template < class TFixedImage, class TMovingImage  >
typename MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
::MeasureType
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
::GetValue( const ParametersType& parameters ) const
{

  // make sure the transform has the current parameters
  this->m_Transform->SetParameters( parameters );

  // collect sample set A
  // this->SampleFixedImageDomain( m_SampleA );

  // collect sample set B
  // this->SampleFixedImageDomain( m_SampleB );

  // Update intensity values
  MovingImagePointType mappedPoint;
  for(int i=0; i< m_SampleA.size(); i ++ )
  {
    mappedPoint = this->m_Transform->TransformPoint(m_SampleA[i].FixedImagePointValue);
    if(this->m_Interpolator->IsInsideBuffer (mappedPoint) )
    {
      m_SampleA[i].MovingImageValue = this->m_Interpolator->Evaluate(mappedPoint);
    }
  }
  for(int i=0; i< m_SampleB.size(); i ++ )
  {
    mappedPoint = this->m_Transform->TransformPoint(m_SampleB[i].FixedImagePointValue);
    if(this->m_Interpolator->IsInsideBuffer (mappedPoint) )
    {
      m_SampleB[i].MovingImageValue = this->m_Interpolator->Evaluate(mappedPoint);
    }
  }

  // calculate the mutual information
  double dLogSumFixed = 0.0;
  double dLogSumMoving    = 0.0;
  double dLogSumJoint  = 0.0;

  typename SpatialSampleContainer::const_iterator aiter;
  typename SpatialSampleContainer::const_iterator aend = m_SampleA.end();
  typename SpatialSampleContainer::const_iterator biter;
  typename SpatialSampleContainer::const_iterator bend = m_SampleB.end();

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

      } // end of sample A loop

    dLogSumFixed  -= ( dSumFixed > 0.0 ) ? vcl_log(dSumFixed  ) : 0.0;
    dLogSumMoving -= ( dSumMoving> 0.0 ) ? vcl_log(dSumMoving ) : 0.0;
    dLogSumJoint  -= ( dSumJoint > 0.0 ) ? vcl_log(dSumJoint  ) : 0.0;

    } // end of sample B loop

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


  //Add the regularization term
  if(m_Regularization )
  {
    double sumOfSquares = 0.0;
    const unsigned int dim = MovingImageType::ImageDimension*(MovingImageType::ImageDimension+1)/2;
    
    typedef typename BSplineTransformType::RegionType RegionType;
    
    GradientPixelType gradientVoxel;
    
    for (int i=0; i<1; i++)
    {
      const RegionType region = m_BSplineTransform->GetGridRegion();
      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {
        
        typedef itk::ImageRegionConstIterator<GradientImageType> GradientIteratorType;
        typedef itk::ImageRegionIterator<BSplineParametersImageType> BSplineImageIteratorType;

        m_BSplineTransform->Modified();
        m_BSplineGradientArray[i][j]->Modified();
        m_BSplineGradientArray[i][j]->Update();
        GradientIteratorType gradientIt( m_BSplineGradientArray[i][j]->GetOutput(), region );

        std::vector< BSplineImageIteratorType > imageIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          imageIterators[k] = BSplineImageIteratorType( m_BSplineGradientImagesArray[i][j][k], region );
        }

        //Extract the gradient images
        while( !gradientIt.IsAtEnd() )
        {
          gradientVoxel = gradientIt.Get();
          for(int k=0; k<MovingImageType::ImageDimension; k++)
          {
            imageIterators[k].Set(gradientVoxel[k]);
            ++imageIterators[k];
          }
          ++gradientIt;
        }

        std::vector< GradientIteratorType > hessianIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          m_BSplineHessianArray[i][j][k]->Modified();
          m_BSplineHessianArray[i][j][k]->Update();
          hessianIterators[k] = GradientIteratorType( m_BSplineHessianArray[i][j][k]->GetOutput(), region );
        }
        
        
        while ( !hessianIterators[0].IsAtEnd() )
        {
          for(int k=0; k<MovingImageType::ImageDimension; k++)
          {
            gradientVoxel = hessianIterators[k].Get();

            for(int l=0; l<k; l++)
            {
              sumOfSquares += 1.0 / 8.0 *gradientVoxel[l]*gradientVoxel[l];
            }
            sumOfSquares += gradientVoxel[k]*gradientVoxel[k];
            
            ++hessianIterators[k];
          }
        }
        
      }
    }
    measure += m_RegularizationFactor * sumOfSquares;
  
  }

  
  return measure;

}


/*
 * Get the both Value and Derivative Measure
 */
template < class TFixedImage, class TMovingImage  >
void
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
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

  typename SpatialSampleContainer::iterator aiter;
  typename SpatialSampleContainer::const_iterator aend = m_SampleA.end();
  typename SpatialSampleContainer::iterator biter;
  typename SpatialSampleContainer::const_iterator bend = m_SampleB.end();

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

  
  //Add the regularization term
  if(m_Regularization )
  {
    double sumOfSquares = 0.0;

    typedef typename BSplineTransformType::RegionType RegionType;
    
    GradientPixelType gradientVoxel;
    int parametersIndex = 0;

    for (int i=0; i<1; i++)
    {
      const RegionType region = m_BSplineTransform->GetGridRegion();
      parametersIndex = 0;

      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {

        typedef itk::ImageRegionConstIterator<GradientImageType> GradientIteratorType;
        typedef itk::ImageRegionIterator<BSplineParametersImageType> BSplineImageIteratorType;

        m_BSplineTransform->Modified();
        m_BSplineGradientArray[i][j]->Modified();
        m_BSplineGradientArray[i][j]->Update();
        GradientIteratorType gradientIt( m_BSplineGradientArray[i][j]->GetOutput(), region );

        std::vector< BSplineImageIteratorType > imageIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          imageIterators[k] = BSplineImageIteratorType( m_BSplineGradientImagesArray[i][j][k], region );
        }

        //Extract the gradient images
        while( !gradientIt.IsAtEnd() )
        {
          gradientVoxel = gradientIt.Get();
          for(int k=0; k<MovingImageType::ImageDimension; k++)
          {
            imageIterators[k].Set(gradientVoxel[k]);
            ++imageIterators[k];
          }
          ++gradientIt;
        }

        std::vector< GradientIteratorType > hessianIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          m_BSplineHessianArray[i][j][k]->Modified();
          m_BSplineHessianArray[i][j][k]->Update();
          hessianIterators[k] = GradientIteratorType( m_BSplineHessianArray[i][j][k]->GetOutput(), region );
        }

        while ( !hessianIterators[0].IsAtEnd() )
        {

          for(int k=0; k<MovingImageType::ImageDimension; k++)
          {
            gradientVoxel = hessianIterators[k].Get();

            for(int l=0; l<k; l++)
            {
              sumOfSquares += 1.0 / 8.0 *gradientVoxel[l]*gradientVoxel[l];
            }
            sumOfSquares += gradientVoxel[k]*gradientVoxel[k];

            ++hessianIterators[k];
          }
        }

        //Calculate the derivative
          
        //Extract the components of the hessian matrix
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][0], region );
          }
        }
        std::vector< GradientIteratorType > hessianDerivativeIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          hessianDerivativeIterators[k] = GradientIteratorType( m_BSplineHessianArray[i][j][k]->GetOutput(), region );
        }

        //Copy the hessian matrix into update images
        while ( !hessianDerivativeIterators[0].IsAtEnd() )
        {
          for(int k=0; k<MovingImageType::ImageDimension; k++)
          {
            gradientVoxel = hessianDerivativeIterators[k].Get();

            for(int l=0; l<=k; l++)
            {
              derivativeIterators[k][l].Set(gradientVoxel[l]);
              ++derivativeIterators[k][l];
            }
            ++hessianDerivativeIterators[k];
          }
        }

        // Compute the gradient of the hessian components
        // Copy the hessian matrix into update images
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          for(int l=0; l<=k; l++)
          {
            m_BSplineGradientUpdateImagesArray[k][l][0]->Modified();
            m_BSplineGradientUpdateArray[k][l][0]->Modified();
            m_BSplineGradientUpdateArray[k][l][0]->Update();
          }
        }

        // Extract the proper part of the gradient update arrays
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators2(MovingImageType::ImageDimension);
        
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators2[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators2[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][0], region );
          }
        }
        std::vector< std::vector< GradientIteratorType > > gradientUpdateDerivativeIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          gradientUpdateDerivativeIterators[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            gradientUpdateDerivativeIterators[k][l] = GradientIteratorType( m_BSplineGradientUpdateArray[k][l][0]->GetOutput(), region );
          }
        }

        // Get only the k'th component
        //Copy the hessian matrix into update images
        while ( !gradientUpdateDerivativeIterators[0][0].IsAtEnd() )
        {
          for(int k=0; k<MovingImageType::ImageDimension; k++)
          {
            for(int l=0; l<=k; l++)
            {
              gradientVoxel = gradientUpdateDerivativeIterators[k][l].Get();
              derivativeIterators2[k][l].Set(gradientVoxel[k]);
              ++gradientUpdateDerivativeIterators[k][l];
              ++derivativeIterators2[k][l];
            }
          }
        }

        // Now the m_BSplineGradientUpdateImagesArray hold the first derivatives
        // Compute the second derivatives
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          for(int l=0; l<=k; l++)
          {
            m_BSplineGradientUpdateImagesArray[k][l][0]->Modified();
            m_BSplineGradientUpdateArray[k][l][0]->Modified();
            m_BSplineGradientUpdateArray[k][l][0]->Update();
          }
        }

        // Get the l'th component of the gradient images
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators3(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators3[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators3[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][0], region );
          }
        }
        std::vector< std::vector< GradientIteratorType > > gradientUpdateDerivativeIterators2(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          gradientUpdateDerivativeIterators2[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            gradientUpdateDerivativeIterators2[k][l] = GradientIteratorType( m_BSplineGradientUpdateArray[k][l][0]->GetOutput(), region );
          }
        }

        while ( !gradientUpdateDerivativeIterators2[0][0].IsAtEnd() )
        {
          for(int k=0; k<MovingImageType::ImageDimension; k++)
          {
            for(int l=0; l<=k; l++)
            {
              gradientVoxel = gradientUpdateDerivativeIterators2[k][l].Get();
              derivativeIterators3[k][l].Set(gradientVoxel[l]);
              ++gradientUpdateDerivativeIterators2[k][l];
              ++derivativeIterators3[k][l];
            }
          }
        }

        // The derivative of the parameters is the sum over the m_BSplineGradientUpdateImagesArray
        // Sum over m_BSplineGradientUpdateImagesArray
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators4(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators4[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators4[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][0], region );
          }
        }

        while ( !derivativeIterators4[0][0].IsAtEnd() )
        {
          for(int k=1; k<MovingImageType::ImageDimension; k++)
          {
            for(int l=0; l<k; l++)
            {
              derivativeIterators4[0][0].Set(derivativeIterators4[0][0].Get() + 1.0 / 8.0 * derivativeIterators4[k][l].Get() );
              ++derivativeIterators4[k][l];
            }
            derivativeIterators4[0][0].Set(derivativeIterators4[0][0].Get() + derivativeIterators4[k][k].Get() );
            ++derivativeIterators4[k][k];
          }
          ++derivativeIterators4[0][0];
        }

        // first image of m_BSplineGradientUpdateImagesArray holds the updates
        // write them into parameters array
        BSplineImageIteratorType parametersUpdateIterators( m_BSplineGradientUpdateImagesArray[0][0][0], region );
        while ( !parametersUpdateIterators.IsAtEnd() )
        {
          derivative[ parametersIndex++] -= 2.0 * m_RegularizationFactor * parametersUpdateIterators.Get();
          ++parametersUpdateIterators;
        }

      }
    }
    value += m_RegularizationFactor * sumOfSquares;

  }
}


/*
 * Get the match measure derivative
 */
template < class TFixedImage, class TMovingImage  >
void
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
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
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
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
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
::ReinitializeSeed()
{
  Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed();
}

/*
 * Reinitialize the seed of the random number generator
 */
template < class TFixedImage, class TMovingImage  >
void
MutualInformationImageToImageMetricBsplineRegularization<TFixedImage,TMovingImage>
::ReinitializeSeed(int seed)
{
  Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed(seed);
}

  



} // end namespace itk


#endif

