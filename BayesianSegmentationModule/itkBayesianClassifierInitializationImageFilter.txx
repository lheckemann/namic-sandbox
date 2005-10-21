/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierInitializationImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/03/08 20:29:20 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkBayesianClassifierInitializationImageFilter_txx
#define _itkBayesianClassifierInitializationImageFilter_txx

#include "itkBayesianClassifierInitializationImageFilter.h"

namespace itk
{

/**
 *
 */
template <class TInputImage, class TOutputImage>
BayesianClassifierInitializationImageFilter<TInputImage, TOutputImage>
::BayesianClassifierInitializationImageFilter()
{
  /*
   * Set 2 outputs.  Output1 will denote the prior probabilities.
   * Output2 will denote the membership probabilities.
   */
  SetNumberOfOutputs(2);
}

/**
 *
 */
template <class TInputImage, class TOutputImage>
void 
BayesianClassifierInitializationImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

/** 
 * This function will create Output1, the vector image of uniform prior probabilites,
 * and Output2, the vector image of membership probabilities.  The membership probabilities
 * are generated via the Gaussian density function, which is initialized with statistics
 * learned via the Kmeans classification.
 */
template <class TInputImage, class TOutputImage>
void 
BayesianClassifierInitializationImageFilter<TInputImage, TOutputImage>
::GenerateData()
{

  // temporary things
  unsigned int numberOfClasses = 3;  

  /*
   * Setup the input image
   */
  const InputImageType *                inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetBufferedRegion();
  typename InputImageType::SpacingType  imageSpacing = inputImage->GetSpacing();
  typename InputImageType::PointType    imageOrigin  = inputImage->GetOrigin();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );

  /*
   * Create a vector image of uniform prior probabilities
   */
  PriorImagePointer priors = PriorImageType::New();
  priors->SetRegions( imageRegion );
  priors->SetSpacing( imageSpacing );
  priors->SetOrigin( imageOrigin );
  priors->SetVectorLength( numberOfClasses );
  priors->Allocate();
  PriorImageIteratorType itrPriorImage( priors, imageRegion );
  PriorPixelType priorPixel( numberOfClasses );

  const double priorProbability = (double)1 / numberOfClasses;
  for ( unsigned int i = 0; i < numberOfClasses; ++i )
    {
    priorPixel[i] = priorProbability;
    }
  itrPriorImage.GoToBegin();
  while( !itrPriorImage.IsAtEnd() )
    {
    itrPriorImage.Set( priorPixel );
    ++itrPriorImage;
    }

  /*
   * Perform Kmeans classification to initialize the gaussian density function
   */
  // find class means via kmeans classification
  const unsigned int useNonContiguousLabels = false;
  KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
  kmeansFilter->SetInput( inputImage );
  kmeansFilter->SetUseNonContiguousLabels( useNonContiguousLabels );

  for( unsigned k=0; k < numberOfClasses; k++ )
    {
    const double userProvidedInitialMean = k;
    kmeansFilter->AddClassWithInitialMean( userProvidedInitialMean );
    }
  kmeansFilter->Update();
  KMeansFilterType::ParametersType estimatedMeans = kmeansFilter->GetFinalMeans(); // mean of each class
  numberOfClasses = estimatedMeans.Size(); // the kmeans filter may return less classes than initially requested.

  // find class covariances from the kmeans output
  ConstKMeansIteratorType itrKMeansImage( kmeansFilter->GetOutput(), imageRegion );
  CovarianceArrayType sumsOfSquares( numberOfClasses );        // sum of the square intensities for each class
  CovarianceArrayType sums( numberOfClasses );                 // sum of the intensities for each class
  CovarianceArrayType classCount( numberOfClasses );           // number of pixels belonging to each class
  CovarianceArrayType estimatedCovariances( numberOfClasses ); // covariance of each class

  for ( unsigned int i = 0; i < numberOfClasses; ++i )  // initialize the arrays
    {
    sumsOfSquares[i] = 0;
    sums[i] = 0;
    classCount[i] = 0;
    }

  itrInputImage.GoToBegin();
  itrKMeansImage.GoToBegin();
  // find sumsOfSquares, sums, and classCount by indexing using the kmeans output labelmap
  while( !itrInputImage.IsAtEnd() )
    {
    sumsOfSquares[(int)itrKMeansImage.Get()] = sumsOfSquares[(int)itrKMeansImage.Get()] +
      itrInputImage.Get() * itrInputImage.Get();
    sums[(int)itrKMeansImage.Get()] = sums[(int)itrKMeansImage.Get()] +
      itrInputImage.Get();
    classCount[(int)itrKMeansImage.Get()] = classCount[(int)itrKMeansImage.Get()] + 1;
    ++itrInputImage;
    ++itrKMeansImage;
    }

  // calculate the class covariances using the sumsOfSquares, sums, and classCount information
  for ( unsigned int i = 0; i < numberOfClasses; ++i )
    {
    estimatedCovariances[i] =
      (sumsOfSquares[i] / classCount[i]) -
      ((sums[i] * sums[i]) / (classCount[i] * classCount[i]));
    if ( estimatedCovariances[i] < 0.0000001 )  // set lower limit for covariance
      {
      estimatedCovariances[i] = 0.0000001;
      };
    std::cout << "cluster[" << i << "]-- " << std::endl;
    std::cout << " estimated mean : " << estimatedMeans[i] << std::endl;
    std::cout << " estimated covariance : " << estimatedCovariances[i] << std::endl;
    }


  /*
   * Create a vector image of membership probabilites using the gaussian density function
   */
  // create gaussian membership functions
  MeanEstimatorsContainerType::Pointer       meanEstimatorsContainer =
                                               MeanEstimatorsContainerType::New();
  CovarianceEstimatorsContainerType::Pointer covarianceEstimatorsContainer =
                                               CovarianceEstimatorsContainerType::New();
  meanEstimatorsContainer->Reserve( numberOfClasses );
  covarianceEstimatorsContainer->Reserve( numberOfClasses );
  for ( unsigned int i = 0; i < numberOfClasses; ++i )
    {
    meanEstimatorsContainer->InsertElement( i, new MembershipFunctionType::MeanType(1) );
    covarianceEstimatorsContainer->
                             InsertElement( i, new MembershipFunctionType::CovarianceType() );
    MembershipFunctionType::MeanType*       meanEstimators = 
                                      const_cast< MembershipFunctionType::MeanType * >
                                      (meanEstimatorsContainer->GetElement(i));
    MembershipFunctionType::CovarianceType* covarianceEstimators = 
                                      const_cast< MembershipFunctionType::CovarianceType * >
                                      (covarianceEstimatorsContainer->GetElement(i));
    meanEstimators->SetSize(1);
    covarianceEstimators->SetSize( 1, 1 );

    meanEstimators->Fill( estimatedMeans[i] );
    covarianceEstimators->Fill( estimatedCovariances[i] );
    MembershipFunctionContainer.push_back( MembershipFunctionType::New() );
    MembershipFunctionContainer[i]->SetMean( meanEstimatorsContainer->GetElement( i ) );
    MembershipFunctionContainer[i]->SetCovariance( covarianceEstimatorsContainer->GetElement( i ) );
    }

  // create vector image of membership probabilities
  MembershipImagePointer data = MembershipImageType::New();
  data->SetRegions( imageRegion );
  data->SetOrigin( imageOrigin );
  data->SetSpacing( imageSpacing );
  data->SetVectorLength( numberOfClasses );
  data->Allocate();
  MembershipImageIteratorType itrDataImage( data, imageRegion );
  MembershipPixelType membershipPixel( numberOfClasses );
  MeasurementVectorType mv;
  
  for ( unsigned int i = 0; i < numberOfClasses; ++i )
    {
    membershipPixel[i] = 0.0;
    }
  itrDataImage.GoToBegin();
  while( !itrDataImage.IsAtEnd() )
    {
    itrDataImage.Set( membershipPixel );
    ++itrDataImage;
    }

  itrDataImage.GoToBegin();
  itrInputImage.GoToBegin();
  while ( !itrDataImage.IsAtEnd() )
    {
    mv = itrInputImage.Get();
    for ( unsigned int i = 0; i < numberOfClasses; i++ )
      {
      membershipPixel[i] = MembershipFunctionContainer[i]->Evaluate( mv );
      }
    itrDataImage.Set( membershipPixel );
    ++itrInputImage;
    ++itrDataImage;
    }

/*  this->GetOutput(0)->SetRequestedRegion( .. );
  this->GetOutput(1)->SetRequestedRegion( .. );
  AllocateOutputs();

  OutputImageType::Pointer output1 = this->GetOutput(0);
  OutputImageType::Pointer output2 = this->GetOutput(1);
*/
}
} // end namespace itk

#endif
