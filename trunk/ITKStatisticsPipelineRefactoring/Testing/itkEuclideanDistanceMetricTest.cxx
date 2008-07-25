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

  distance->SetMeasurementVectorSize( MeasurementVectorSize ); 

  if( distance->GetMeasurementVectorSize() != MeasurementVectorSize )
    {
    std::cerr << "GetMeasurementVectorSize() Failed !" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
