/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsPrintTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/24 15:16:21 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkFixedArray.h"
#include "itkImage.h"
#include "itkPointSet.h"

// #include "itkSample.h"   // abstract class
int itkStatisticsPrintTest(int , char* [])
{
  typedef float MeasurementType ;
  typedef float FrequencyType ;
  typedef itk::FixedArray< MeasurementType, 2 > MeasurementVectorType ;
  typedef itk::Image< MeasurementVectorType, 3 > ImageType ;
  typedef itk::PointSet< MeasurementType > PointSetType ;

  return EXIT_SUCCESS;
}
