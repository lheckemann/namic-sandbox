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


#include "itkExpectationMaximizationImageClassification.h"

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


  const    unsigned int    Dimension = 2;
  typedef  float           PixelType;

  typedef itk::VectorImage< PixelType, Dimension >  InputImageType;


  typedef itk::ImageFileReader< InputImageType >     ImageReaderType;

  ImageReaderType::Pointer  reader  = ImageReaderType::New();
  
  reader->SetFileName(  argv[1] );
  
  reader->Update();


  typedef itk::Statistics::ExpectationMaximizationImageClassification< 
                                                         InputImageType 
                                                                        >   ClassifierType;

  ClassifierType::Pointer   EMClassifier = ClassifierType::New();


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

