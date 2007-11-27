/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ScalarImageMarkovRandomField1.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 18:23:31 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkImage.h"
#include "itkVector.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkMRFImageFilter.h"
#include "itkDistanceToCentroidMembershipFunction.h"
#include "itkMinimumDecisionRule.h"
#include "itkImageClassifierBase.h"

int main( int argc, char * argv [] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0];
    std::cerr << " inputVectorImage inputLabeledImage";
    std::cerr << " outputLabeledImage numberOfIterations";
    std::cerr << " smoothingFactor numberOfClasses";
    std::cerr << " mean1 mean2 ... meanN " << std::endl;
    return EXIT_FAILURE;
    }

  const char * inputImageFileName      = argv[1];
  const char * inputLabelImageFileName = argv[2];
  const char * outputImageFileName     = argv[3];

  const unsigned int numberOfIterations = atoi( argv[4] );
  const double       smoothingFactor    = atof( argv[5] );
  const unsigned int numberOfClasses    = atoi( argv[6] );

  const unsigned int numberOfArgumentsBeforeMeans = 7;

  const unsigned int NumberOfComponents = 3;

  if( static_cast<unsigned int>(argc) <
      numberOfClasses * NumberOfComponents + numberOfArgumentsBeforeMeans )
    {
    std::cerr << "Error: " << std::endl;
    std::cerr << numberOfClasses << " classes has been specified ";
    std::cerr << "but no enough means have been provided in the command ";
    std::cerr << "line arguments " << std::endl;
    return EXIT_FAILURE;
    }


  typedef unsigned char       PixelComponentType;
  const unsigned int          Dimension = 2;

  typedef itk::Vector<PixelComponentType,NumberOfComponents>  PixelType;

  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImageFileName );

  
  typedef unsigned char       LabelPixelType;

  typedef itk::Image<LabelPixelType, Dimension > LabelImageType;

  typedef itk::ImageFileReader< LabelImageType > LabelReaderType;
  LabelReaderType::Pointer labelReader = LabelReaderType::New();
  labelReader->SetFileName( inputLabelImageFileName );


  typedef itk::MRFImageFilter< ImageType, LabelImageType > MRFFilterType;

  MRFFilterType::Pointer mrfFilter = MRFFilterType::New();

  mrfFilter->SetInput(  reader->GetOutput() );

  mrfFilter->SetNumberOfClasses( numberOfClasses );
  mrfFilter->SetMaximumNumberOfIterations( numberOfIterations );
  mrfFilter->SetSmoothingFactor( smoothingFactor );
  mrfFilter->SetErrorTolerance( 1e-7 );


  typedef itk::ImageClassifierBase< 
                              ImageType,
                              LabelImageType >   SupervisedClassifierType;

  SupervisedClassifierType::Pointer classifier = 
                                         SupervisedClassifierType::New();


  typedef itk::MinimumDecisionRule DecisionRuleType;

  DecisionRuleType::Pointer  classifierDecisionRule = DecisionRuleType::New();

  classifier->SetDecisionRule( classifierDecisionRule.GetPointer() );


  typedef itk::Statistics::DistanceToCentroidMembershipFunction< 
                                                    PixelType > 
                                                       MembershipFunctionType;

  typedef MembershipFunctionType::Pointer MembershipFunctionPointer;


  double meanDistance = 0;
  vnl_vector<double> centroid(NumberOfComponents); 
  for( unsigned int i=0; i < numberOfClasses; i++ )
    {
    MembershipFunctionPointer membershipFunction = MembershipFunctionType::New();

    for( unsigned int j=0; j < NumberOfComponents; j++ )
      {
      centroid[j] = atof( argv[j+i*NumberOfComponents+numberOfArgumentsBeforeMeans] ); 
      }

    membershipFunction->SetCentroid( centroid );

    classifier->AddMembershipFunction( membershipFunction );
    meanDistance += static_cast< double > (centroid[0]);
    }
  meanDistance /= numberOfClasses;


  mrfFilter->SetNeighborhoodRadius( 1 );

  std::vector< double > weights;
  weights.push_back(1.5);
  weights.push_back(2.0);
  weights.push_back(1.5);
  weights.push_back(2.0);
  weights.push_back(0.0); // This is the central pixel
  weights.push_back(2.0);
  weights.push_back(1.5);
  weights.push_back(2.0);
  weights.push_back(1.5);

    
  double totalWeight = 0;
  for(std::vector< double >::const_iterator wcIt = weights.begin(); 
      wcIt != weights.end(); ++wcIt )
    {
    totalWeight += *wcIt;
    }
 
  for(std::vector< double >::iterator wIt = weights.begin(); 
      wIt != weights.end(); wIt++ )
    {
    *wIt = static_cast< double > ( (*wIt) * meanDistance / (2 * totalWeight));
    }

  mrfFilter->SetMRFNeighborhoodWeight( weights );
  mrfFilter->SetClassifier( classifier );

  typedef MRFFilterType::OutputImageType  OutputImageType;

  typedef itk::ImageFileWriter< OutputImageType > WriterType;

  WriterType::Pointer writer = WriterType::New();
  
  writer->SetInput( mrfFilter->GetOutput() );

  writer->SetFileName( outputImageFileName );


  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encountered while writing ";
    std::cerr << " image file : " << argv[2] << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Number of Iterations : ";
  std::cout << mrfFilter->GetNumberOfIterations() << std::endl;
  std::cout << std::endl;

  std::cout << "Stopping condition: " << std::endl;

  switch( mrfFilter->GetStopCondition() )
    {
    case 1:
      std::cout << "  (1) Maximum number of iterations " << std::endl;
      break;
    case 2:
      std::cout << "  (2) Error tolerance:  "  << std::endl;
      break;
    }

  return EXIT_SUCCESS;
  
}


