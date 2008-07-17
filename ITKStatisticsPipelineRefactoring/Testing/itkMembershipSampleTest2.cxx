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

#include <iostream>
#include "itkMembershipSample.h"
#include "itkArray.h"
#include "itkListSample.h"

int itkMembershipSampleTest2(int, char* [] )
{

  const unsigned int MeasurementVectorSize = 29;

  typedef itk::Array< float >  MeasurementVectorType;

  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::MembershipSample< SampleType >   MembershipSampleType;


  MeasurementVectorType vector;

  vector.SetSize( MeasurementVectorSize );


  return EXIT_SUCCESS;
}
