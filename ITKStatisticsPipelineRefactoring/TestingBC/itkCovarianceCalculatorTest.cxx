/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCovarianceCalculatorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005-02-08 03:18:41 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToListAdaptor.h"
#include "itkCovarianceCalculator.h"
#include "itkRandomImageSource.h"
#include "itkImageRegionIterator.h"
#include "itkFixedArray.h"
#include "itkVector.h"
#include "itkMeanCalculator.h"

int itkCovarianceCalculatorTest(int, char* [] ) 
{
  std::cout << "CovarianceCalculator Test \n \n"; 
  bool pass = true;
  std::string whereFail = "" ;

  // Now generate an image
  enum { MeasurementVectorSize = 1 } ;
  typedef float MeasurementType ;
  typedef itk::FixedArray< MeasurementType, MeasurementVectorSize > 
    MeasurementVectorType ;
  typedef itk::Image< MeasurementVectorType, 3 > ImageType ;
  ImageType::Pointer image = ImageType::New() ;
  ImageType::RegionType region ;
  ImageType::SizeType size ;
  ImageType::IndexType index ;
  index.Fill(0) ;
  size.Fill(5) ;
  region.SetIndex(index) ;
  region.SetSize(size) ;
  
  image->SetLargestPossibleRegion(region) ;
  image->SetBufferedRegion(region) ;
  image->Allocate() ;

  typedef itk::ImageRegionIterator< ImageType > ImageIterator ;
  ImageIterator iter(image, region) ;

  unsigned int count = 0 ;
  MeasurementVectorType temp ;
  // fill the image
  while (!iter.IsAtEnd())
    {
    temp[0] = count ;
    iter.Set(temp) ;
    ++iter ;
    ++count ;
    }

  // creates an ImageToListAdaptor object
  typedef  itk::Statistics::ImageToListAdaptor< ImageType >
    ImageToListAdaptorType ;

  ImageToListAdaptorType::Pointer sample = ImageToListAdaptorType::New() ;
  sample->SetImage(image) ;

  typedef itk::Statistics::MeanCalculator< ImageToListAdaptorType > 
    MeanCalculatorType ;
  MeanCalculatorType::Pointer meanCalculator = MeanCalculatorType::New() ;
  meanCalculator->SetInputSample(sample.GetPointer()) ;
  meanCalculator->Update() ;
  MeanCalculatorType::OutputType* mean = meanCalculator->GetOutput() ;

  // calculates variance
  count = 0 ;
  double variance = 0.0 ;
  iter.GoToBegin() ;
  while (!iter.IsAtEnd())
    {
    double diff = iter.Get()[0] - float((*mean)[0]) ;
    variance += diff * diff ; 
    ++count ; 
    ++iter ;
    }
  variance /= static_cast< double>(count - 1) ;


  typedef itk::Statistics::CovarianceCalculator< ImageToListAdaptorType > 
    CalculatorType;

  CalculatorType::Pointer calculator = CalculatorType::New() ;
  
  calculator->SetInputSample(sample.GetPointer()) ;
  calculator->SetMean(meanCalculator->GetOutput()) ;
  calculator->Update() ;

  if (calculator->GetOutput()->GetVnlMatrix().get(0,0) != variance)
    {
    pass = false ;
    }
 
  // Testing one pass covariance calculation without a given mean
  calculator->SetMean(0) ;
  calculator->Update() ;

  if (calculator->GetOutput()->GetVnlMatrix().get(0,0) != variance)
    {
    pass = false ;
    }
  
  if ((*calculator->GetMean())[0] != (*meanCalculator->GetOutput())[0])
    {
    pass = false ;
    }

  if( !pass )
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



