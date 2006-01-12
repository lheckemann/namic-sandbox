/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkDICOMImageReaderTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/19 23:02:01 $
  Version:   $Revision: 1.4 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkDICOMImageReader.h"
#include "igstkImageSpatialObject.h"

#include "itkLogger.h"
#include "itkCommand.h"
#include "itkStdStreamLogOutput.h"

class DICOMImageModalityInformationCallback: public itk::Command
{
public:
  typedef DICOMImageModalityInformationCallback    Self;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::Command                      Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject< 
                                short, 
                                3                                
                                       > ImageSpatialObjectType;
  
 
  typedef igstk::DICOMModalityEvent DICOMModalityEventType;
  
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {

  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    if( DICOMModalityEventType().CheckEvent( &event ) )
      {
      const DICOMModalityEventType * modalityEvent = 
                   dynamic_cast< const DICOMModalityEventType *>( &event );
      std::cout << "Modality= " << modalityEvent->Get() << std::endl;
      }
      
  }
protected:
  DICOMImageModalityInformationCallback()   { };

private:
};

class DICOMImagePatientNameInformationCallback: public itk::Command
{
public:
  typedef DICOMImagePatientNameInformationCallback    Self;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::Command                      Superclass;
  itkNewMacro(Self);

  typedef igstk::ImageSpatialObject< 
                                short, 
                                3                                
                                       > ImageSpatialObjectType;
  
 
  typedef igstk::DICOMPatientNameEvent DICOMPatientNameEventType;
  
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {

  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    const DICOMPatientNameEventType * patientNameEvent = 
          dynamic_cast < const DICOMPatientNameEventType* > ( &event );    
    std::cerr << "PatientName= " << patientNameEvent->Get() << std::endl;
      
  }
protected:
  DICOMImagePatientNameInformationCallback()   { };

private:
};



int igstkDICOMImageReaderTest( int argc, char* argv[] )
{

  if(argc < 2)
    {
    std::cerr<<"Usage: "<<argv[0]<<"  DICOMSeries "<<std::endl;
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

  typedef short      PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension 
                                       > ImageSpatialObjectType;
  

  class myDicomTestReader : 
    public igstk::DICOMImageReader< ImageSpatialObjectType >
  {
    public:
       typedef myDicomTestReader  Self;
       typedef igstk::ImageReader< ImageSpatialObjectType > Superclass;

       igstkNewMacro( Self );

       void TestMe()
       {
       const ImageType * imageThatShallNotBeNamed = GetITKImage();
       if( imageThatShallNotBeNamed != NULL )
         {
         std::cout << "Test of private abstract method Passed" << std::endl;
         }
       }
    protected:
       myDicomTestReader() {}
       ~myDicomTestReader() {}
    private:
      typedef Superclass::ImageType ImageType;
      virtual const ImageType * GetITKImage() const { return NULL; }
  };

  typedef myDicomTestReader   ReaderType;


  ReaderType::Pointer   reader = ReaderType::New();
  reader->SetLogger( logger );

  if( reader->FileSuccessfullyRead() )
    {
    std::cerr << "FileSuccessfullyRead() is malfunctioning. It returned true ";
    std::cerr << "when the image has not even been read" << std::endl;
    return EXIT_FAILURE;
    }
      
  /* Read in a DICOM series */
  std::cout << "Reading the DICOM series : " << argv[1] <<std::endl;
  ReaderType::DirectoryNameType directoryName = argv[1];
  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();
  
 /* Add observer to listen to modality info */
  DICOMImageModalityInformationCallback::Pointer dimcb = DICOMImageModalityInformationCallback::New();
  reader->AddObserver( igstk::DICOMModalityEvent(), dimcb );
  reader->RequestModalityInformation(); 
 
  /* Add observer to listen to patient name  info */
  DICOMImagePatientNameInformationCallback::Pointer dipncb = DICOMImagePatientNameInformationCallback::New();
  reader->AddObserver( igstk::DICOMPatientNameEvent(), dipncb );
  reader->RequestPatientNameInformation(); 

  reader->Print( std::cout );

  /** Exercising the Unsafe GetPatientName() method */
  if( reader->FileSuccessfullyRead() )
    {
    ReaderType::DICOMInformationType  patientName = reader->GetPatientName();
    ReaderType::DICOMInformationType  patientID   = reader->GetPatientID();
    ReaderType::DICOMInformationType  modality    = reader->GetModality();
    
    std::cout << "Patient Name = " << patientName << std::endl;
    std::cout << "Patient ID = "   << patientID << std::endl;
    std::cout << "Modality = "     << modality << std::endl;
    }
  else
    {
    std::cout << "The file was not successfully read" << std::endl;
    return EXIT_FAILURE;
    }

  // Test Reader resetting function
  reader->RequestResetReader();

  // Purposely call RequestReadImage to check if the image can be read after reseting the reader..
  reader->RequestReadImage();

  
  return EXIT_SUCCESS;
}

