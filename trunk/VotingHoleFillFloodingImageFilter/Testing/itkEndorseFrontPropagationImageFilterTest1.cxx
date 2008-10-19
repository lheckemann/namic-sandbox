/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: VotingBinaryIterativeHoleFillingImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005-11-20 13:27:52 $
  Version:   $Revision: 1.12 $

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
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkEndorseFrontPropagationImageFilter.h"

int main( int argc, char * argv[] )
{
  if( argc < 8 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputMaskImageFile inputCreditImageFile outputMaskImageFile ";
    std::cerr << " radiusX radiusY majority maxNumberOfIterations" << std::endl;
    return EXIT_FAILURE;
    }


  typedef   unsigned char  MaskPixelType;
  typedef   signed short   CreditPixelType;

  typedef itk::Image< MaskPixelType, 2 >        MaskImageType;
  typedef itk::Image< CreditPixelType, 2 >      CreditImageType;


  typedef itk::ImageFileReader< MaskImageType >    MaskReaderType;
  typedef itk::ImageFileWriter< MaskImageType >    MaskWriterType;
  typedef itk::ImageFileReader< CreditImageType >  CreditReaderType;

  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  MaskWriterType::Pointer maskWriter = MaskWriterType::New();

  CreditReaderType::Pointer creditReader = CreditReaderType::New();

  maskReader->SetFileName( argv[1] );
  maskWriter->SetFileName( argv[3] );

  creditReader->SetFileName( argv[2] );

  typedef itk::EndorseFrontPropagationImageFilter< MaskImageType, CreditImageType, MaskImageType >  FilterType;

  FilterType::Pointer filter = FilterType::New();

  const unsigned int radiusX = atoi( argv[4] );
  const unsigned int radiusY = atoi( argv[5] );

  MaskImageType::SizeType indexRadius;
  
  indexRadius[0] = radiusX; // radius along x
  indexRadius[1] = radiusY; // radius along y

  filter->SetRadius( indexRadius );


  filter->SetBackgroundValue(   0 );
  filter->SetForegroundValue( 255 );

  const unsigned int majorityThreshold = atoi( argv[6] );

  filter->SetMajorityThreshold( majorityThreshold  );

  const unsigned int maximumNumberOfIterations = atoi( argv[7] );

  filter->SetMaximumNumberOfIterations( maximumNumberOfIterations  );


  filter->SetInput( maskReader->GetOutput() );
  filter->SetCreditInput( creditReader->GetOutput() );
  maskWriter->SetInput( filter->GetOutput() );
  maskWriter->Update();

  std::cout << "Iteration used = " << filter->GetCurrentIterationNumber()     << std::endl;
  std::cout << "Pixels changes = " << filter->GetTotalNumberOfPixelsChanged() << std::endl;

  return EXIT_SUCCESS;
}

