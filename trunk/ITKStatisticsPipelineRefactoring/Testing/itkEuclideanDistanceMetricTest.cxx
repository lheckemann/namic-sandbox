/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/11 13:52:36 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkEuclideanDistanceMetric.h"

int itkEuclideanDistanceMetricTest(int, char* [] )
{
  const unsigned int MeasurementVectorSize = 3;

  typedef itk::Array< float  >  MeasurementVectorType;

  typedef itk::Statistics::EuclideanDistanceMetric< MeasurementVectorType >   DistanceMetricType;

  DistanceMetricType::Pointer distance = DistanceMetricType::New();

  std::cout << distance->GetNameOfClass() << std::endl;

  distance->Print(std::cout);

  MeasurementVectorType measurementNew;
  ::itk::Statistics::MeasurementVectorTraits::SetLength( measurementNew, 3);
  measurementNew[0] = 2.5;
  measurementNew[1] = 3.3;
  measurementNew[2] = 4.0;

  //Attempting to compute distance before setting a measurment vector should
  //throw an excpetion
  
  try
    {
    distance->Evaluate( measurementNew );
    std::cerr << "Attempting to compute distance w/o setting measurement vector"
                 "size, Exception should have been thrown" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception: " << excp << std::endl;
    }

  distance->SetMeasurementVectorSize( MeasurementVectorSize ); 

  if( distance->GetMeasurementVectorSize() != MeasurementVectorSize )
    {
    std::cerr << "GetMeasurementVectorSize() Failed !" << std::endl;
    return EXIT_FAILURE;
    }

  //Test if the distance computed is correct
  DistanceMetricType::OriginType origin;
  ::itk::Statistics::MeasurementVectorTraits::SetLength( origin, 3);
  origin[0] = 1.5;
  origin[1] = 2.3;
  origin[2] = 1.0;
  distance->SetOrigin( origin );

  MeasurementVectorType measurement;
  ::itk::Statistics::MeasurementVectorTraits::SetLength( measurement, 3);
  measurement[0] = 2.5;
  measurement[1] = 3.3;
  measurement[2] = 4.0;

  double trueValue = 3.31662;
  double distanceComputed = distance->Evaluate( measurement );
  const double tolerance = 0.001;

  if( fabs( distanceComputed - trueValue) > tolerance )
    {
    std::cerr << "Distance computed not correct: " << "truevalue= " << trueValue
              << "ComputedValue=" << distanceComputed << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
