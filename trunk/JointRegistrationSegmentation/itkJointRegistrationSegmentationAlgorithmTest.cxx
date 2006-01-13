/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration9.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/31 14:14:16 $
  Version:   $Revision: 1.29 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkJointRegistrationSegmentationAlgorithm.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"



int main( int argc, char *argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << "   inputImageFile  numberOfIterations" << std::endl;
    return 1;
    }


  const    unsigned int    Dimension = 3;
  typedef  float           PixelType;

  typedef itk::VectorImage< PixelType, Dimension >  InputImageType;


  typedef itk::ImageFileReader< InputImageType >     ImageReaderType;

  ImageReaderType::Pointer  reader  = ImageReaderType::New();
  
  reader->SetFileName(  argv[1] );
  
  reader->Update();


  typedef float PriorsPixelComponentType;
  typedef float WeightsPixelComponentType;

  typedef itk::Statistics::JointRegistrationSegmentationAlgorithm< 
                                                           InputImageType, 
                                                           PriorsPixelComponentType,
                                                           WeightsPixelComponentType
                                                                        >   ClassifierType;

  ClassifierType::Pointer   EMClassifier = ClassifierType::New();


  typedef  ClassifierType::GaussianDensityFunctionType      GaussianDensityFunctionType;


  GaussianDensityFunctionType::Pointer  gaussian1  = GaussianDensityFunctionType::New();
  GaussianDensityFunctionType::Pointer  gaussian2  = GaussianDensityFunctionType::New();
  GaussianDensityFunctionType::Pointer  gaussian3  = GaussianDensityFunctionType::New();
  GaussianDensityFunctionType::Pointer  gaussian4  = GaussianDensityFunctionType::New();

  const double proportionClass1 = 0.4;
  const double proportionClass2 = 0.8;
  const double proportionClass3 = 0.9;
  const double proportionClass4 = 0.3;
  
  EMClassifier->AddIntensityDistributionDensity( gaussian1, proportionClass1 );
  EMClassifier->AddIntensityDistributionDensity( gaussian2, proportionClass2 );
  EMClassifier->AddIntensityDistributionDensity( gaussian3, proportionClass3 );
  EMClassifier->AddIntensityDistributionDensity( gaussian4, proportionClass4 );


  EMClassifier->SetObservations( reader->GetOutput() );


  try 
    { 
    EMClassifier->Update();
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 


  return 0;
}

