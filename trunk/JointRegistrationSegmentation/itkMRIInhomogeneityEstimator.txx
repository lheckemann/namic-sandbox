/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageRegistrationMethod2.h,v $
  Language:  C++
  Date:      $Date: 2005/11/01 21:57:22 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMRIInhomogeneityEstimator_txx
#define __itkMRIInhomogeneityEstimator_txx

#include "itkMRIInhomogeneityEstimator.h"

namespace itk {

namespace Statistics {


template < class TInputImage >
MRIInhomogeneityEstimator< TInputImage >
::MRIInhomogeneityEstimator() :
    m_NumberOfClasses( 0 ),
    m_StructureIntensityDistributionContainer( NULL )
{
  this->SetNumberOfRequiredInputs(2);
  this->SetNumberOfRequiredOutputs(1);
}

template < class TInputImage >
MRIInhomogeneityEstimator< TInputImage >
::~MRIInhomogeneityEstimator()
{
}

template < class TInputImage >
void
MRIInhomogeneityEstimator< TInputImage >
::SetMembershipImage(const MembershipImageType* image) 
{ 
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(1, 
                                   const_cast< MembershipImageType* >( image ) );
}

template < class TInputImage >
const MRIInhomogeneityEstimator< class TInputImage >::MembershipImageType*
MRIInhomogeneityEstimator< TInputImage >
::GetMembershipImage() const
{
  if (this->GetNumberOfInputs() < 2)
    {
    return 0;
    }
  
  return static_cast<const MembershipImageType * >
    (this->ProcessObject::GetInput(1) );
}

template < class TInputImage >
void
MRIInhomogeneityEstimator< TInputImage >
::GenerateData()
{
  InputImageType *input = this->GetInput();
  MembershipImageType *weights = this->GetMembershipImage();
  
  if (!input || !weights) 
    {
    return;
    }
  
  if (this->m_StructureIntensityDistributionContainer && 
      (this->m_StructureIntensityDistributionContainer->Size() !=
        this->GetInput()->GetVectorLength() 
       || this->m_StructureIntensityDistributionContainer->Size() !=
        this->GetMembershipImage()->GetVectorLength() ))
    {
    itkExceptionMacro( << "Number of channels in input must be equal to number "
        << "of gaussian functions supplied");
    }

  this->m_NumberOfClasses = this->m_StructureIntensityDistributionContainer->Size();

  typedef itk::ImageRegionConstIterator< InputImageType > ConstInputIteratorType;
  typedef itk::ImageRegionConstIterator< MembershipImageType > ConstMembershipIteratorType;
  typedef itk::ImageRegionConstIterator< OutputImageType > OutputImageIteratorType;

  // Iterator over the input image 
  ConstInputIteratorType  cIit( input, input->GetBufferedRegion() );
  ConstMembershipIteratorType cWit( weights, weights->GetBufferedRegion() );
  OutputImageIteratorType cOit( output, output->GetBufferedRegion() );
  
  cIit.GoToBegin();
  cWit.GoToBegin();
  cOit.GoToBegin();
  
  while (!cIit.IsAtEnd())
    {
    MembershipImageType::PixelType membershipPixel = cWit.Get();
    InputImageType::PixelType inputPixel = cIit.Get();
    
    for (unsigned int classIdx = 0; classIdx < this->m_NumberOfClasses; classIdx++)
      {
      cOit.Set(membershipPixel * 
       m_StructureIntensityDistributionContainer->GetElement(
         classIdx)->GetCovariance() * (inputPixel - 
           m_StructureIntensityDistributionContainer->GetElement(classIdx)->GetMean()));
      }
    ++cOit;
    ++cWit;
    ++cIit;
    }
      

}

template < class TInputImage >
void
MRIInhomogeneityEstimator< TInputImage >
::SetIntensityDistributions( StructureIntensityDistributionType 
                                                * densityFunctionContainer )
{
  this->m_NumberOfClasses = densityFunctionContainer->Size();
  this->m_StructureIntensityDistributionContainer = densityFunctionContainer;
  this->Modified();
}

/** Give reasonable defaults to the structure intensity distributions */
template < class TInputImage >
void
MRIInhomogeneityEstimator< TInputImage >
::InitializeStructureIntensityDistributions()
{
  // TODO: Initialize the gaussians from KMeans or something...
  //
  // Typedefs for the KMeans filter, Covariance calculator...
  typedef ScalarImageKmeansImageFilter< InputImageType > KMeansFilterType;
  typedef typename KMeansFilterType::OutputImageType  KMeansOutputImageType;
  typedef ImageRegionConstIterator< 
                  KMeansOutputImageType >             ConstKMeansIteratorType;
  typedef Array< double >                             CovarianceArrayType;
  typedef Array< double >                             ClassCountArrayType;
  typedef Statistics::GaussianDensityFunction< 
          MeasurementVectorType >                     GaussianMembershipFunctionType;
  typedef VectorContainer< unsigned short, ITK_TYPENAME 
    GaussianMembershipFunctionType::MeanType* >       MeanEstimatorsContainerType;
  typedef VectorContainer< unsigned short, ITK_TYPENAME 
    GaussianMembershipFunctionType::CovarianceType* > CovarianceEstimatorsContainerType;

  
  // Run k means to get the means from the input image
  typename KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
  kmeansFilter->SetInput( this->GetInput() );
  kmeansFilter->SetUseNonContiguousLabels( false );

  for( unsigned k=0; k < m_NumberOfClasses; k++ )
    {
    const double userProvidedInitialMean = k;  
    //TODO: Choose more reasonable defaults for specifying the initial means
    //to the KMeans filter. We could also add this as an option of the filter.
    kmeansFilter->AddClassWithInitialMean( userProvidedInitialMean );
    }

  try
    {
    kmeansFilter->Update();
    }
  catch( ExceptionObject& err )
    {
    // Pass exception to caller
    throw err;
    }

  typename KMeansFilterType::ParametersType 
          estimatedMeans = kmeansFilter->GetFinalMeans(); // mean of each class

  // find class covariances from the kmeans output to initialize the gaussian
  // density functions.
  ConstKMeansIteratorType itrKMeansImage( kmeansFilter->GetOutput(), 
                      kmeansFilter->GetOutput()->GetBufferedRegion() );
  CovarianceArrayType sumsOfSquares( m_NumberOfClasses );        // sum of the square intensities for each class
  CovarianceArrayType sums( m_NumberOfClasses );                 // sum of the intensities for each class
  ClassCountArrayType classCount( m_NumberOfClasses );           // m_Number of pixels belonging to each class
  CovarianceArrayType estimatedCovariances( m_NumberOfClasses ); // covariance of each class

  // initialize the arrays
  sumsOfSquares.Fill( 0.0 );
  sums.Fill( 0.0 );
  classCount.Fill( 0 );

  const InputImageType *                inputImage = this->GetInput();
  typename InputImageType::RegionType   imageRegion  = inputImage->GetLargestPossibleRegion();
  InputImageIteratorType                itrInputImage( inputImage, imageRegion );

  itrInputImage.GoToBegin();
  itrKMeansImage.GoToBegin();

  // find sumsOfSquares, sums, and classCount by indexing using the kmeans output labelmap
  while( !itrInputImage.IsAtEnd() )
    {
    sumsOfSquares[(unsigned int)itrKMeansImage.Get()] 
      = sumsOfSquares[(unsigned int)itrKMeansImage.Get()] +
        itrInputImage.Get() * itrInputImage.Get();
    sums[(unsigned int)itrKMeansImage.Get()] 
      = sums[(unsigned int)itrKMeansImage.Get()] + itrInputImage.Get();
    ++classCount[(unsigned int)itrKMeansImage.Get()];
    ++itrInputImage;
    ++itrKMeansImage;
    }

  // calculate the class covariances using the sumsOfSquares, sums, and classCount information
  itkDebugMacro( << "Estimated parameters after Kmeans filter" );
  for ( unsigned int i = 0; i < m_NumberOfClasses; ++i )
    {
    estimatedCovariances[i] =
      (sumsOfSquares[i] / classCount[i]) -
      ((sums[i] * sums[i]) / (classCount[i] * classCount[i]));
    if ( estimatedCovariances[i] < 0.0000001 )  // set lower limit for covariance
      {
      estimatedCovariances[i] = 0.0000001;
      };
    itkDebugMacro( << "cluster[" << i << "]-- " );
    itkDebugMacro( << " estimated mean : " << estimatedMeans[i] );
    itkDebugMacro( << " estimated covariance : " << estimatedCovariances[i] );
    }

  // Create gaussian membership functions.
  typename MeanEstimatorsContainerType::Pointer meanEstimatorsContainer =
                                       MeanEstimatorsContainerType::New();
  typename CovarianceEstimatorsContainerType::Pointer covarianceEstimatorsContainer =
                                       CovarianceEstimatorsContainerType::New();
  meanEstimatorsContainer->Reserve( m_NumberOfClasses );
  covarianceEstimatorsContainer->Reserve( m_NumberOfClasses );

  m_MembershipFunctionContainer = MembershipFunctionContainerType::New();
  m_MembershipFunctionContainer->Initialize(); // Clear elements
  for ( unsigned int i = 0; i < m_NumberOfClasses; ++i )
    {
    meanEstimatorsContainer->InsertElement( i, 
         new typename GaussianMembershipFunctionType::MeanType(1) );
    covarianceEstimatorsContainer->
      InsertElement( i, new typename GaussianMembershipFunctionType::CovarianceType() );
    typename GaussianMembershipFunctionType::MeanType*       meanEstimators = 
             const_cast< ITK_TYPENAME GaussianMembershipFunctionType::MeanType * >
                           (meanEstimatorsContainer->GetElement(i));
    typename GaussianMembershipFunctionType::CovarianceType* covarianceEstimators = 
              const_cast< ITK_TYPENAME GaussianMembershipFunctionType::CovarianceType * >
              (covarianceEstimatorsContainer->GetElement(i));
    meanEstimators->SetSize(1);
    covarianceEstimators->SetSize( 1, 1 );

    meanEstimators->Fill( estimatedMeans[i] );
    covarianceEstimators->Fill( estimatedCovariances[i] );
    typename GaussianMembershipFunctionType::Pointer gaussianDensityFunction
                                       = GaussianMembershipFunctionType::New();
    gaussianDensityFunction->SetMean( meanEstimatorsContainer->GetElement( i ) );
    gaussianDensityFunction->SetCovariance( covarianceEstimatorsContainer->GetElement( i ) );
 
    m_MembershipFunctionContainer->InsertElement(i, 
            dynamic_cast< MembershipFunctionType * >( gaussianDensityFunction.GetPointer() ) );
    }
}


}

}


#endif

