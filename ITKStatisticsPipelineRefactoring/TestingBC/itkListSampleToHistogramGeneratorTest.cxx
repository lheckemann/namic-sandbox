/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkListSampleToHistogramGeneratorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005-08-24 15:16:15 $
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
#include "itkVector.h"
#include "itkListSample.h"
#include "itkListSampleToHistogramGenerator.h"

int itkListSampleToHistogramGeneratorTest(int, char* [] ) 
{
  std::cout << "ListSampleToHistogramGenerator Test \n \n"; 
  bool pass = true;
  std::string whereFail = "" ;

  const unsigned int MeasurementVectorLength = 2;
  typedef int MeasurementType ;
  typedef itk::Vector< MeasurementType , MeasurementVectorLength > MeasurementVectorType ;
  typedef itk::Statistics::ListSample< MeasurementVectorType > ListSampleType ;

  ListSampleType::Pointer sample = ListSampleType::New();
  sample->SetMeasurementVectorSize( MeasurementVectorLength );

  MeasurementVectorType mv ;
  for ( unsigned int i = 1 ; i < 6 ; i++ )
    {
      for (unsigned int j = 0 ; j < MeasurementVectorLength ; j++ )
        {
          mv[j] = ( MeasurementType ) i ;
        }
      for ( unsigned int j = 0 ; j < i ; j++ )
        {
          sample->PushBack(mv) ;
        }
    }

  typedef float HistogramMeasurementType ;
  typedef itk::Statistics::ListSampleToHistogramGenerator< ListSampleType, 
            HistogramMeasurementType, itk::Statistics::DenseFrequencyContainer, 
            MeasurementVectorLength  > GeneratorType;
  GeneratorType::Pointer generator = GeneratorType::New() ;

  GeneratorType::HistogramType::SizeType size ;
  size.Fill(5) ;

  generator->SetListSample( sample ) ;
  generator->SetNumberOfBins(size) ;
  generator->Update() ;

  GeneratorType::HistogramType::ConstPointer histogram = generator->GetOutput() ;

  GeneratorType::HistogramType::IndexType index ;
  
  index.Fill(0) ;

  for ( unsigned int i = 0 ; i < 5 ; i++ )
    {
      index[0] = i ;
      for ( unsigned int j = 0 ; j < 5 ; j++ )
        {
          index[1] = j ;
          
          if ( i == j && histogram->GetFrequency(index) != (i + 1.0) )
            {
              pass = false ;
              whereFail = " this class" ;
            }
        }
    }
          
  if( !pass )
    {
      std::cout << "Test failed in " << whereFail << "." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


}



