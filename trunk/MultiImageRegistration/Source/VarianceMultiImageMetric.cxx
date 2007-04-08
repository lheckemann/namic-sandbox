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

  //Initialize the derivative to zero
  this->m_derivativeArray[threadId].Fill(0.0);

  
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
      double weight = 2.0 / static_cast<double>(this->m_NumberOfImages) *
                      (this->m_Sample[a].imageValueArray[i] - mean);
      
      //Get the derivative at this pixel
      this->m_DerivativeCalcVector[threadId]->SetInputImage(this->m_ImageArray[i]);
      this->CalculateDerivatives (this->m_Sample[a].FixedImagePoint, deriv, i, threadId);

      //copy the properpart of the derivative
      for (int j = 0;j < this->m_TransformArray[i]->GetNumberOfParameters (); j++)
      {
        this->m_derivativeArray[threadId][i * numberOfParameters + j] += weight*deriv[j];
      }
    }
  } // End of sample Loop
  this->m_value[threadId] = measure;

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





}        // end namespace itk


#endif
