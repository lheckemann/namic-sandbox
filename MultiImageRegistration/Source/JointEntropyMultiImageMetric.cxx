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
    probSum /= sampleBSize;
    
    if( probSum > 0.0)
    {
      this->m_value[threadId] -= vcl_log( probSum );
    }

  }  // End of sample loop

  this->m_value[threadId] /= sampleBSize;

  //Add the regularization term
  if(this->m_Regularization )
  {
    double sumOfSquares = 0.0;
    const unsigned int dim = MovingImageType::ImageDimension*(MovingImageType::ImageDimension+1)/2;
    
    typedef typename BSplineTransformType::RegionType RegionType;
    
    GradientPixelType gradientVoxel;
    
    for (int i=threadId; i<this->m_NumberOfImages; i+= this->m_NumberOfThreads)
    {
      const RegionType region = this->m_BSplineTransformArray[i]->GetGridRegion();
      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {
        
        typedef itk::ImageRegionConstIterator<GradientImageType> GradientIteratorType;
        typedef itk::ImageRegionIterator<BSplineParametersImageType> BSplineImageIteratorType;

        this->m_BSplineTransformArray[i]->Modified();
        this->m_BSplineGradientArray[i][j]->Modified();
        this->m_BSplineGradientArray[i][j]->Update();
        GradientIteratorType gradientIt( this->m_BSplineGradientArray[i][j]->GetOutput(), region );

        std::vector< BSplineImageIteratorType > imageIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          imageIterators[k] = BSplineImageIteratorType( this->m_BSplineGradientImagesArray[i][j][k], region );
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
          this->m_BSplineHessianArray[i][j][k]->Modified();
          this->m_BSplineHessianArray[i][j][k]->Update();
          hessianIterators[k] = GradientIteratorType( this->m_BSplineHessianArray[i][j][k]->GetOutput(), region );
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
    this->m_value[threadId] += this->m_RegularizationFactor * sumOfSquares;
  
  }
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

  
  //Calculate the joint entropy
  double logSum = 0.0;
  // Loop over first sample set
  for (int b=sampleASize+threadId; b<sampleASize+sampleBSize; b += this->m_NumberOfThreads )
  {

    double probSum = 0.0;
    for( int a=0; a < sampleBSize; a++)
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
    probSum /= sampleBSize;
    
    if( probSum > 0.0)
    {
      this->m_value[threadId] -= vcl_log( probSum );
    }

  }  // End of sample loop

  this->m_value[threadId] /= sampleASize;


  // Calculate the derivative
  // Loop over the pixel stacks
  for( int l=0; l<N; l++)
  {
    // Set the image for derivatives
    this->m_DerivativeCalcVector[threadId]->SetInputImage(this->m_ImageArray[l]);
    
    for (int i=sampleASize+threadId; i<sampleASize+sampleBSize; i += this->m_NumberOfThreads )
    {

      // compute W(v_l(x_i))
      double W_x_i = 0.0;
      for(unsigned int k=0; k<sampleASize; k++ )
      {
        //Compute d(x_i,x_k)
        double sumOfSquares = 0.0;
        for(unsigned int m=0; m<N; m++)
        {
          const double diff = this->m_Sample[i].imageValueArray[m] - this->m_Sample[k].imageValueArray[m];
          sumOfSquares += diff*diff;
        }

        W_x_i += this->m_KernelFunction->Evaluate( sqrt(sumOfSquares) / this->m_ImageStandardDeviation );
      }

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

      weight /= W_x_i;

      // Compute the gradient
      this->CalculateDerivatives(this->m_Sample[i].FixedImagePoint, deriv, l, threadId);

      //Copy the proper part of the derivative
      for (int n = 0; n < numberOfParameters; n++)
      {
        this->m_derivativeArray[threadId][l * numberOfParameters + n] += weight*deriv[n];
      }
    }  // End of sample loop B


    
    // Loop over the samples A
    for (int i=threadId; i<sampleASize; i += this->m_NumberOfThreads )
    {

      // compute W(v_l(x_i))
      double W_x_i = 0.0;
      for(unsigned int k=sampleASize; k<sampleASize+sampleBSize; k++ )
      {
        //Compute d(x_i,x_k)
        double sumOfSquares = 0.0;
        for(int m=0; m<N; m++)
        {
          const double diff = this->m_Sample[i].imageValueArray[m] - this->m_Sample[k].imageValueArray[m];
          sumOfSquares += diff*diff;
        }

        W_x_i += this->m_KernelFunction->Evaluate( sqrt(sumOfSquares) / this->m_ImageStandardDeviation );
      }

      // Sum over the second sample set
      double weight = 0.0;
      for (int j=sampleASize; i<sampleASize+sampleBSize; j++ )
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

      weight /= W_x_i;

      // Compute the gradient
      this->CalculateDerivatives(this->m_Sample[i].FixedImagePoint, deriv, l, threadId);

      //Copy the proper part of the derivative
      for (int n = 0; n < numberOfParameters; n++)
      {
        this->m_derivativeArray[threadId][l * numberOfParameters + n] += weight*deriv[n];
      }
    }  // End of sample loop B
    
  }
  //Add the regularization term
  if(this->m_Regularization )
  {
    double sumOfSquares = 0.0;

    typedef typename BSplineTransformType::RegionType RegionType;
    
    GradientPixelType gradientVoxel;
    int parametersIndex = 0;

    for (int i=threadId; i<this->m_NumberOfImages; i+= this->m_NumberOfThreads)
    {
      const RegionType region = this->m_BSplineTransformArray[i]->GetGridRegion();
      parametersIndex = 0;

      for(int j=0; j<MovingImageType::ImageDimension; j++)
      {

        typedef itk::ImageRegionConstIterator<GradientImageType> GradientIteratorType;
        typedef itk::ImageRegionIterator<BSplineParametersImageType> BSplineImageIteratorType;

        this->m_BSplineTransformArray[i]->Modified();
        this->m_BSplineGradientArray[i][j]->Modified();
        this->m_BSplineGradientArray[i][j]->Update();
        GradientIteratorType gradientIt( this->m_BSplineGradientArray[i][j]->GetOutput(), region );

        std::vector< BSplineImageIteratorType > imageIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          imageIterators[k] = BSplineImageIteratorType( this->m_BSplineGradientImagesArray[i][j][k], region );
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
          this->m_BSplineHessianArray[i][j][k]->Modified();
          this->m_BSplineHessianArray[i][j][k]->Update();
          hessianIterators[k] = GradientIteratorType( this->m_BSplineHessianArray[i][j][k]->GetOutput(), region );
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
            derivativeIterators[k][l] = BSplineImageIteratorType( this->m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
          }
        }
        std::vector< GradientIteratorType > hessianDerivativeIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          hessianDerivativeIterators[k] = GradientIteratorType( this->m_BSplineHessianArray[i][j][k]->GetOutput(), region );
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
            this->m_BSplineGradientUpdateImagesArray[k][l][threadId]->Modified();
            this->m_BSplineGradientUpdateArray[k][l][threadId]->Modified();
            this->m_BSplineGradientUpdateArray[k][l][threadId]->Update();
          }
        }

        // Extract the proper part of the gradient update arrays
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators2(MovingImageType::ImageDimension);
        
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators2[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators2[k][l] = BSplineImageIteratorType( this->m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
          }
        }
        std::vector< std::vector< GradientIteratorType > > gradientUpdateDerivativeIterators(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          gradientUpdateDerivativeIterators[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            gradientUpdateDerivativeIterators[k][l] = GradientIteratorType( this->m_BSplineGradientUpdateArray[k][l][threadId]->GetOutput(), region );
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
            this->m_BSplineGradientUpdateImagesArray[k][l][threadId]->Modified();
            this->m_BSplineGradientUpdateArray[k][l][threadId]->Modified();
            this->m_BSplineGradientUpdateArray[k][l][threadId]->Update();
          }
        }

        // Get the l'th component of the gradient images
        std::vector< std::vector< BSplineImageIteratorType > > derivativeIterators3(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          derivativeIterators3[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            derivativeIterators3[k][l] = BSplineImageIteratorType( this->m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
          }
        }
        std::vector< std::vector< GradientIteratorType > > gradientUpdateDerivativeIterators2(MovingImageType::ImageDimension);
        for(int k=0; k<MovingImageType::ImageDimension; k++)
        {
          gradientUpdateDerivativeIterators2[k].resize(MovingImageType::ImageDimension);
          for(int l=0; l<=k; l++)
          {
            gradientUpdateDerivativeIterators2[k][l] = GradientIteratorType( this->m_BSplineGradientUpdateArray[k][l][threadId]->GetOutput(), region );
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
            this->derivativeIterators4[k][l] = BSplineImageIteratorType( this->m_BSplineGradientUpdateImagesArray[k][l][threadId], region );
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
        BSplineImageIteratorType parametersUpdateIterators( this->m_BSplineGradientUpdateImagesArray[0][0][threadId], region );
        while ( !parametersUpdateIterators.IsAtEnd() )
        {
          this->m_derivativeArray[threadId][i * numberOfParameters + parametersIndex++] -= 2.0 * this->m_RegularizationFactor * parametersUpdateIterators.Get();
          ++parametersUpdateIterators;
        }

      }
    }
    this->m_value[threadId] += this->m_RegularizationFactor * sumOfSquares;



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
  //value /= (double) this->m_NumberOfSpatialSamples / 2 * this->m_ImageStandardDeviation * this->m_ImageStandardDeviation;
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



}        // end namespace itk


#endif
