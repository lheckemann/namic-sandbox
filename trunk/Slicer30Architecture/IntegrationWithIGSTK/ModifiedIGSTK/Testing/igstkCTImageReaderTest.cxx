/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkCTImageReaderTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 02:16:49 $
  Version:   $Revision: 1.1 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkCTImageReader.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkCTImageReaderTest( int argc, char* argv[] )
{

  if( argc < 2 )
    {
    std::cerr<<"Usage: "<<argv[0]<<"  CTImage "<<std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout<<"Reading CT image : "<<argv[1]<<std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  
  reader->Print( std::cout );

  try
    {
    reader->RequestReadImage();
    }
  catch( ... )
    {
    std::cerr << "ERROR: An exception was thrown while reading the CT dataset" << std::endl;
    std::cerr << "This should not have happened. The State Machine should have" << std::endl;
    std::cerr << "catched that exception and converted it into a SM Input " << std::endl;
    return EXIT_FAILURE;
    }

  igstk::CTImageSpatialObject::ConstPointer ctImage = reader->GetOutput();

  // Details of the loaded image
  ctImage->Print( std::cout );
  
  return EXIT_SUCCESS;
}

