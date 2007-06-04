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

  m_ImageStandardDeviation = 0.4;
  m_MinProbability = 1e-10;
  
  // Following initialization is related to
  // calculating image derivatives
  this->SetComputeGradient (false);  // don't use the default gradient for now

  this->m_BSplineTransformArray.resize(0);
  

  m_UseMask = false;
  m_NumberOfFixedImages = 0;
  m_NumberOfParametersPerdimension = 0;

}

/*
 * Constructor
 */
template < class TFixedImage >
ParzenWindowEntropyMultiImageMetric < TFixedImage >::
~ParzenWindowEntropyMultiImageMetric()
{
  


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

  // Set the kernel function
  m_KernelFunction.resize(this->m_NumberOfImages);
  for(int i=0; i<this->m_NumberOfImages; i++)
  {
    m_KernelFunction[i] =
        dynamic_cast < KernelFunction * >(GaussianKernelFunction::New().GetPointer ());
  }

  if (this->m_NumberOfSpatialSamples == 0)
  {
    this->SetNumberOfSpatialSamples(50);
  }

  //check whether there is a mask
  for (int j = 0; j < this->m_NumberOfImages; j++)
  {
    if ( this->m_ImageMaskArray[j] )
    {
      m_UseMask = true;
    }
  }



  m_TransformParametersArray.resize(this->m_NumberOfImages);
  for(int i=0; i<this->m_NumberOfImages; i++)
  {
    m_TransformParametersArray[i].set_size(this->numberOfParameters);
  }
  //Get number of threads
  m_NumberOfThreads = this->GetNumberOfThreads();

  m_value.SetSize( m_NumberOfThreads );

  // Each thread has its own derivative pointer
  m_DerivativesArray.resize(m_NumberOfThreads);
  for(int i=0; i<m_NumberOfThreads; i++)
  {
    m_DerivativesArray[i].resize(this->m_NumberOfImages);
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      m_DerivativesArray[i][j].SetSize(this->numberOfParameters);
    }
  }

  // Each image has its own derivative calculator
  m_DerivativeCalculator.resize(this->m_NumberOfImages);
  for(int i=0; i<this->m_NumberOfImages; i++)
  {
    // separate for each thread
    m_DerivativeCalculator[i].resize(m_NumberOfThreads);
    for(int j=0; j<m_NumberOfThreads; j++)
    {
      m_DerivativeCalculator[i][j] = DerivativeFunctionType::New ();
      m_DerivativeCalculator[i][j]->SetInputImage(this->m_ImageArray[i]);
    }
  }

  // Sample the image domain
  randIter = new NonRepeatingRandomIterator(this->m_ImageArray[0], this->GetFixedImageRegion());

  randIter->SetNumberOfSamples(this->m_NumberOfSpatialSamples);
  randIter->ReinitializeSeed();
  randIter->GoToBegin();
  
  this->SampleFixedImageDomain(this->m_Sample);
  
  // Initialize the variables for regularization term
  if( this->m_UserBsplineDefined && this->m_Regularization &&
      strcmp(this->m_TransformArray[0]->GetNameOfClass(), "UserBSplineDeformableTransform") )
  {
    itkExceptionMacro(<<"Cannot use regularization with transforms" <<
        " other than BSplineDeformableTransform" );
  }


  // Initialize the variables for regularization term
  if( this->m_UserBsplineDefined )
  {
    // Get nonzero indexes
    numberOfWeights = this->m_BSplineTransformArray[0]->GetNumberOfAffectedWeights();
    bsplineIndexes.set_size(numberOfWeights);
    m_NumberOfParametersPerdimension = this->m_BSplineTransformArray[0]->GetNumberOfParametersPerDimension();
  }
  else
  {
    this->m_Regularization = false;
  }

  // Initialize the variables for regularization term
  if( this->m_Regularization &&
      strcmp(this->m_TransformArray[0]->GetNameOfClass(), "UserBSplineDeformableTransform") )
  {
    itkExceptionMacro(<<"Cannot use regularization with transforms" <<
        " other than BSplineDeformableTransform" );
  }

  // If using regularization check that the Bspline Transform is supplied
  if( this->m_Regularization &&
      !strcmp(this->m_TransformArray[0]->GetNameOfClass(), "BSplineDeformableTransform") )
  {
    for(int i=0; i<this->m_NumberOfImages;i++)
    {
      if( !this->m_BSplineTransformArray[i] )
      {
        itkExceptionMacro(<<"Bspline Transform Array not initialized" );
      }
      
      if( (void *) this->m_BSplineTransformArray[i] != (void*)this->m_TransformArray[i])
      {
        itkExceptionMacro(<<"While using Bspline regularization transform array and Bspline array should have the pointers to the same transform" );
      }
    }

  }
  
  //Prepare the gradient filters if Regularization is on
  if(this->m_Regularization)
  {
        //Prepare hessian filters
    m_BSplineGradientArray.resize(this->m_NumberOfImages);
    m_BSplineHessianArray.resize(this->m_NumberOfImages);
    m_BSplineGradientImagesArray.resize(this->m_NumberOfImages);
    for(int i=0; i<this->m_NumberOfImages; i++)
    {
      m_BSplineGradientArray[i].resize(MovingImageType::ImageDimension);
      m_BSplineHessianArray[i].resize(MovingImageType::ImageDimension);
      m_BSplineGradientImagesArray[i].resize(MovingImageType::ImageDimension);

      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {
        // Connect bspline coeff images to gradient filters
        m_BSplineGradientArray[i][j] = GradientFilterType::New();
        m_BSplineGradientArray[i][j]->SetInput(this->m_BSplineTransformArray[0]->GetCoefficientImage()[j]);
        m_BSplineGradientArray[i][j]->Update();

        m_BSplineHessianArray[i][j].resize(MovingImageType::ImageDimension);
        m_BSplineGradientImagesArray[i][j].resize(MovingImageType::ImageDimension);
        
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          // allocate the gradient images which hold k'th image of the graadient of the
          // Bspline coeff image
          m_BSplineGradientImagesArray[i][j][k] = BSplineParametersImageType::New();
          m_BSplineGradientImagesArray[i][j][k]->SetRegions( this->m_BSplineTransformArray[0]->GetGridRegion() );
          m_BSplineGradientImagesArray[i][j][k]->Allocate();
          m_BSplineGradientImagesArray[i][j][k]->SetSpacing( this->m_BSplineTransformArray[0]->GetGridSpacing() );
          m_BSplineGradientImagesArray[i][j][k]->SetOrigin( this->m_BSplineTransformArray[0]->GetGridOrigin() );
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
        m_BSplineGradientUpdateImagesArray[i][j].resize(m_NumberOfThreads);
        m_BSplineGradientUpdateArray[i][j].resize(m_NumberOfThreads);
        
        // separate for each thread
        for(int k=0; k<m_NumberOfThreads; k++)
        {
          m_BSplineGradientUpdateImagesArray[i][j][k] = BSplineParametersImageType::New();
          m_BSplineGradientUpdateImagesArray[i][j][k]->SetRegions( this->m_BSplineTransformArray[0]->GetGridRegion() );
          m_BSplineGradientUpdateImagesArray[i][j][k]->Allocate();
          m_BSplineGradientUpdateImagesArray[i][j][k]->SetSpacing( this->m_BSplineTransformArray[0]->GetGridSpacing() );
          m_BSplineGradientUpdateImagesArray[i][j][k]->SetOrigin( this->m_BSplineTransformArray[0]->GetGridOrigin() );
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

/*
 * Finalize
 */
template <class TFixedImage> 
void
ParzenWindowEntropyMultiImageMetric<TFixedImage>
::Finalize(void)
{

  // deallocate randomiterator
  delete randIter;

 //Finalize superclass
  Superclass::Finalize();
}

template < class TFixedImage >
void ParzenWindowEntropyMultiImageMetric < TFixedImage >::
PrintSelf (std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf (os, indent);
  os << indent << "NumberOfSpatialSamples: ";
  os << this->m_NumberOfSpatialSamples << std::endl;
  os << indent << "FixedImageStandardDeviation: ";
  os << m_ImageStandardDeviation << std::endl;
  os << indent << "MovingImageStandardDeviation: ";
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

  bool allOutside = true;

  // Number of random picks made from the portion of fixed image within the fixed mask
  unsigned long numberOfFixedImagePixelsVisited = 0;
  unsigned long dryRunTolerance = 3*this->GetFixedImageRegion().GetNumberOfPixels();


  for (int i=0; i<this->m_NumberOfSpatialSamples; i++)
  {
    // Get sampled index
    FixedImageIndexType index = randIter->GetIndex();
    // Translate index to point
    this->m_ImageArray[0]->TransformIndexToPhysicalPoint(index, this->m_Sample[i].FixedImagePoint);

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
    bool pointInsideMask = false;
    for (int j = 0; j < this->m_NumberOfImages && allPointsInside; j++)
    {
      this->m_Sample[i].mappedPointsArray[j] = this->m_TransformArray[j]->TransformPoint (this->m_Sample[i].FixedImagePoint);

      //check whether sampled point is in one of the masks
      if ( this->m_ImageMaskArray[j] && !this->m_ImageMaskArray[j]
           ->IsInside (this->m_Sample[i].mappedPointsArray[j])  )
      {
        pointInsideMask = true;
      }

      allPointsInside = allPointsInside && this->m_InterpolatorArray[j]
          ->IsInsideBuffer (this->m_Sample[i].mappedPointsArray[j]);
    }

    // If not all points are inside continue to the next random sample
    if (allPointsInside == false || (m_UseMask && pointInsideMask == false) )
    {
      ++(*randIter);
      i--;
      continue;
    }

    // write the mapped samples intensity values inside an array
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(this->m_Sample[i].mappedPointsArray[j]);
      //this->m_Sample[i].gradientArray[j] = this->m_GradientInterpolatorArray[j]->Evaluate(this->m_Sample[i].mappedPointsArray[j]);
      allOutside = false;
    }
    // Jump to random position
    ++(*randIter);

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
  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j];
    }
    this->m_TransformArray[i]->SetParameters(m_TransformParametersArray[i]);
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

  // Update intensity values
  MovingImagePointType mappedPoint;
  for(int i=threadId; i< this->m_NumberOfSpatialSamples; i += m_NumberOfThreads )
  {
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      mappedPoint = this->m_TransformArray[j]->TransformPoint(this->m_Sample[i].FixedImagePoint);
      if(this->m_InterpolatorArray[j]->IsInsideBuffer (mappedPoint) )
      {
        this->m_Sample[i].imageValueArray[j] = this->m_InterpolatorArray[j]->Evaluate(mappedPoint);
      }
    }
  } 

  //Calculate variance and mean
  m_value[threadId] = 0.0;
  double dSum;

  // Loop over the pixel stacks
  for (int a=threadId; a<this->m_NumberOfSpatialSamples; a += m_NumberOfThreads )
  {

    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      dSum = 0.0;
      
      for(int k = 0; k < this->m_NumberOfImages; k++)
      {

        dSum += m_KernelFunction[threadId]->Evaluate( ( this->m_Sample[a].imageValueArray[j] - this->m_Sample[a].imageValueArray[k] )
                                                /m_ImageStandardDeviation );
      }
      dSum /= static_cast<double> (this->m_NumberOfImages );
      m_value[threadId]  += vcl_log( dSum );
      
    }

  }  // End of sample loop

   //Add the regularization term
  if(this->m_Regularization )
  {
    double sumOfSquares = 0.0;
    const unsigned int dim = MovingImageType::ImageDimension*(MovingImageType::ImageDimension+1)/2;
    
    typedef typename BSplineTransformType::RegionType RegionType;
    
    GradientPixelType gradientVoxel;
    
    for (int i=threadId; i<this->m_NumberOfImages; i+= m_NumberOfThreads)
    {
      const RegionType region = this->m_BSplineTransformArray[i]->GetGridRegion();

      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {
        
        typedef itk::ImageRegionConstIterator<GradientImageType> GradientIteratorType;
        typedef itk::ImageRegionIterator<BSplineParametersImageType> BSplineImageIteratorType;

        this->m_BSplineTransformArray[i]->Modified();
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
    m_value[threadId] += this->m_RegularizationFactor * sumOfSquares;
  
  }
  
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

  // Sum over the values returned by threads
  for( unsigned int i=0; i < m_NumberOfThreads; i++ )
  {
    value += m_value[i];
  }
  value /= (MeasureType) (-1.0 * this->m_NumberOfSpatialSamples*this->m_NumberOfImages);

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
  // cout << "checking derivative" << endl;
  // collect sample set
  this->SampleFixedImageDomain (this->m_Sample);

  // Loop over images
  for (int i = 0; i < this->m_NumberOfImages; i++)
  {
    //Copy the parameters of the current transform
    for (int j = 0; j < this->numberOfParameters; j++)
    {
      m_TransformParametersArray[i][j] = parameters[this->numberOfParameters * i + j];
    }
    // cout << currentParam << endl;
    this->m_TransformArray[i]->SetParameters(m_TransformParametersArray[i]);
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

  derivative.set_size(this->numberOfParameters * this->m_NumberOfImages);
  derivative.Fill (0.0);

  // Sum over the values returned by threads
  for( unsigned int i=0; i < m_NumberOfThreads; i++ )
  {
    value += m_value[i];
    for(int j=0; j<this->m_NumberOfImages; j++)
    {
      for(int k=0; k<this->numberOfParameters; k++)
      {
        derivative[j * this->numberOfParameters + k] += m_DerivativesArray[i][j][k]; 
      }
    }
  }
  value /= (double) ( -1.0 * this->m_NumberOfSpatialSamples * this->m_NumberOfImages );
  derivative /= (double) (this->m_NumberOfSpatialSamples * this->m_NumberOfImages *
                     m_ImageStandardDeviation * m_ImageStandardDeviation );
  
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


/*
 * Get the match Measure
 */
template < class TFixedImage >
void 
ParzenWindowEntropyMultiImageMetric < TFixedImage >
::GetThreadedValueAndDerivative(int threadId) const
{
  
  MeasureType value = NumericTraits < MeasureType >::Zero;

  //Initialize the derivative array to zero
  for(int i=0; i<this->m_NumberOfImages;i++)
  {
    m_DerivativesArray[threadId][i].Fill(0.0);
  }


  //Calculate variance and mean
  m_value[threadId] = 0.0;
  std::vector<double> W_j(this->m_NumberOfImages);

  // Loop over the pixel stacks
  for (int x=threadId; x<this->m_NumberOfSpatialSamples; x += m_NumberOfThreads )
  {
    // Calculate the entropy
    for (int j = 0; j < this->m_NumberOfImages; j++)
    {
      W_j[j] = 0.0;
      
      for(int k = 0; k < this->m_NumberOfImages; k++)
      {

        W_j[j] += m_KernelFunction[threadId]->Evaluate( ( this->m_Sample[x].imageValueArray[j] - this->m_Sample[x].imageValueArray[k] ) /
            m_ImageStandardDeviation );

      }
      m_value[threadId]  += vcl_log( W_j[j] / (double) (this->m_NumberOfImages ) );
      
    }


    // Calculate derivative
    for (int l = 0; l < this->m_NumberOfImages; l++)
    {

      double weight = 0.0;
      for(int j=0; j<this->m_NumberOfImages; j++)
      {

        const double diff = (this->m_Sample[x].imageValueArray[l] - this->m_Sample[x].imageValueArray[j] );
        const double g = m_KernelFunction[threadId]->Evaluate( diff / m_ImageStandardDeviation );

        weight += (1.0/W_j[l] + 1.0/W_j[j]) * g * diff;
      }
      
      // Get the derivative for this sample
      UpdateSingleImageParameters( m_DerivativesArray[threadId][l], this->m_Sample[x], weight, l, threadId);
    }
  }  // End of sample loop

//Add the regularization term
  if(this->m_Regularization )
  {
    double sumOfSquares = 0.0;

    typedef typename BSplineTransformType::RegionType RegionType;
    
    GradientPixelType gradientVoxel;
    int parametersIndex = 0;

    for (int i=threadId; i<this->m_NumberOfImages; i+= m_NumberOfThreads)
    {
      const RegionType region = this->m_BSplineTransformArray[i]->GetGridRegion();
      parametersIndex = 0;

      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {

        typedef itk::ImageRegionConstIterator<GradientImageType> GradientIteratorType;
        typedef itk::ImageRegionIterator<BSplineParametersImageType> BSplineImageIteratorType;

        this->m_BSplineTransformArray[i]->Modified();
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
            derivativeIterators[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
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
            m_BSplineGradientUpdateImagesArray[k][l][threadId]->Modified();
            m_BSplineGradientUpdateArray[k][l][threadId]->Modified();
            m_BSplineGradientUpdateArray[k][l][threadId]->Update();
          }
        }

        // Extract the proper part of the gradient update arrays
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators2(MovingImageType::ImageDimension);
        
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators2[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators2[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
          }
        }
        std::vector< std::vector< GradientIteratorType > > gradientUpdateDerivativeIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          gradientUpdateDerivativeIterators[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            gradientUpdateDerivativeIterators[k][l] = GradientIteratorType( m_BSplineGradientUpdateArray[k][l][threadId]->GetOutput(), region );
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
            m_BSplineGradientUpdateImagesArray[k][l][threadId]->Modified();
            m_BSplineGradientUpdateArray[k][l][threadId]->Modified();
            m_BSplineGradientUpdateArray[k][l][threadId]->Update();
          }
        }

        // Get the l'th component of the gradient images
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators3(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators3[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators3[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
          }
        }
        std::vector< std::vector< GradientIteratorType > > gradientUpdateDerivativeIterators2(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          gradientUpdateDerivativeIterators2[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            gradientUpdateDerivativeIterators2[k][l] = GradientIteratorType( m_BSplineGradientUpdateArray[k][l][threadId]->GetOutput(), region );
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
            derivativeIterators4[k][l] = BSplineImageIteratorType( m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
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
        BSplineImageIteratorType parametersUpdateIterators( m_BSplineGradientUpdateImagesArray[0][0][threadId], region );
        while ( !parametersUpdateIterators.IsAtEnd() )
        {
          m_DerivativesArray[threadId][i][parametersIndex++] -= 2.0 * this->m_RegularizationFactor * parametersUpdateIterators.Get();
          ++parametersUpdateIterators;
        }

      }
    }
    m_value[threadId] += this->m_RegularizationFactor * sumOfSquares;


  }
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
UpdateSingleImageParameters( DerivativeType & inputDerivative, const SpatialSample& sample, const RealType& weight, const int& imageNumber, const int& threadID) const
{

  const CovarientType gradient = m_DerivativeCalculator[imageNumber][threadID]->Evaluate(sample.mappedPointsArray[imageNumber]);
  //typedef FixedArray < double, MovingImageDimension > FixedArrayType;
  //const FixedArrayType gradient = this->m_GradientInterpolatorArray[imageNumber]->Evaluate(sample.mappedPointsArray[imageNumber]);

  typedef typename TransformType::JacobianType JacobianType;


  if(this->m_UserBsplineDefined == false )
  {
    const JacobianType & jacobian =
        this->m_TransformArray[imageNumber]->GetJacobian( sample.FixedImagePoint);

    double currentValue;
    for (unsigned int k = 0; k < this->numberOfParameters; k++)
    {
      currentValue = 0.0;
      for (unsigned int j = 0; j < MovingImageDimension; j++)
      {
        currentValue += jacobian[j][k] * gradient[j];
      }
      inputDerivative[k] += currentValue*weight;
     
    }

  }
  else
  {
    // Get nonzero indexes
    typedef itk::Array<RealType> WeigtsType;
    WeigtsType bsplineWeights(numberOfWeights);
    this->m_BSplineTransformArray[imageNumber]->GetJacobian(sample.FixedImagePoint, bsplineIndexes, bsplineWeights);

    for (unsigned int k = 0; k < numberOfWeights; k++)
    {

        for (unsigned int j = 0; j < MovingImageDimension; j++)
        {

          inputDerivative[j*m_NumberOfParametersPerdimension + bsplineIndexes[k]] += bsplineWeights[k] * gradient[j] * weight;
        }

    }

  }

}



}        // end namespace itk


#endif
