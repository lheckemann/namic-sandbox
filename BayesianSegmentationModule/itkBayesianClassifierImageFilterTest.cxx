/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierImageFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/03/09 20:58:34 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkBayesianClassifierImageFilter.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkGaussianDensityFunction.h"


//int itkBayesianClassifierImageFilterTest(int argc, char* argv[] )
int main(int argc, char* argv[] )
{

  if( argc < 3 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile outputImageFile numberOfClasses" << std::endl;
    return EXIT_FAILURE;
    }

  // INPUT PARAMETERS
  char * rawDataFileName = argv[1];
  char * labelMapFileName = argv[2];
  float timeStep = 0.1; // USER VARIABLE (DEFAULT = 0.1)
  

  // SETUP READER
  const unsigned int Dimension = 2;
  typedef unsigned char InputComponentType;
  typedef itk::Vector<InputComponentType, 3> InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >    InputImageType;
  typedef itk::ImageFileReader< InputImageType >     ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( rawDataFileName );


  // SETUP WRITER
  typedef unsigned long OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;
  typedef itk::ImageFileWriter< OutputImageType >    WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( labelMapFileName );


  // SETUP FILTER
  typedef itk::BayesianClassifierImageFilter< 
                                 InputImageType,
                                 OutputImageType >  ClassifierFilterType;

  ClassifierFilterType::Pointer filter = ClassifierFilterType::New();
  filter->SetInput( reader->GetOutput() );


  // SET FILTER'S PRIOR PARAMETERS
  // do nothing here to default to uniform priors

  // SET FILTER'S DATA PARAMETERS
  typedef ClassifierFilterType::MeasurementVectorType MeasurementVectorType;
  typedef itk::Statistics::GaussianDensityFunction<
                                 MeasurementVectorType >    MembershipFunctionType;

  MembershipFunctionType::Pointer gaussian1 =  MembershipFunctionType::New();
  MembershipFunctionType::Pointer gaussian2 =  MembershipFunctionType::New();
  MembershipFunctionType::Pointer gaussian3 =  MembershipFunctionType::New();
  MembershipFunctionType::Pointer gaussian4 =  MembershipFunctionType::New();

  filter->AddMembershipFunction( gaussian1 );
  filter->AddMembershipFunction( gaussian2 );
  filter->AddMembershipFunction( gaussian3 );
  filter->AddMembershipFunction( gaussian4 );


  // SET FILTER'S SMOOTHING PARAMETERS
  // do nothing here to use default smoothing parameters

  // EXECUTE THE FILTER
  filter->Update();


  // WRITE LABELMAP TO FILE
  // need to write GetOutput method
//   writer->SetInput( filter->GetOutput() );
//   writer->SetFileName( labelMapFileName );
//   try
//     {
//     writer->Update();
//     }
//   catch( itk::ExceptionObject & excp )
//     {
//     std::cerr << "Problem encoutered while writing image file : "
//       << argv[2] << std::endl;
//     std::cerr << excp << std::endl;
//     return EXIT_FAILURE;
//     }


  // TESTING PRINT
  filter->Print( std::cout );
  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
