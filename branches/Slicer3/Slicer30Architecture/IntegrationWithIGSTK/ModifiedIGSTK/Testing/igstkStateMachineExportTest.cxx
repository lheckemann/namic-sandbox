/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkStateMachineExportTest.cxx,v $
  Language:  C++
  Date:      $Date: 2006/01/07 00:01:12 $
  Version:   $Revision: 1.12 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
 //Warning about: identifier was truncated to '255' characters in the debug
 //information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

/**
 *  This file generates the State Machine diagram of all the calsses by
 *  invoking their respective Export method.
 */

#include <fstream>

#include "igstkConfigure.h"

#include "igstkCylinderObjectRepresentation.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkPulseGenerator.h"
#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkSpatialObject.h"
#include "igstkSpatialObjectReader.h"
#include "igstkCTImageReader.h"
#include "igstkMRImageReader.h"
#include "igstkLandmark3DRegistration.h"
#include "igstkImageSpatialObject.h"
#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageReader.h"
#include "igstkDICOMImageReader.h"
#include "igstkCTImageSpatialObject.h"
#include "igstkCTImageSpatialObjectRepresentation.h"
#include "igstkMRImageSpatialObject.h"
#include "igstkMRImageSpatialObjectRepresentation.h"

#if IGSTK_USE_FLTK
#include "igstkView2D.h"
#include "igstkView3D.h"
#endif

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

namespace igstk 
{
  
template<class ClassType>
void ExportStateMachineDescription( 
              const ClassType * instance, 
              const std::string & outputDirectory, bool skipLoops )
  {
  std::string filename = outputDirectory+"/";
  filename = filename + "igstk";
  filename = filename + instance->GetNameOfClass();


  std::string dotfilename = filename + ".dot";
  std::ofstream dotOutputFile;
  dotOutputFile.open( dotfilename.c_str() );
  if( dotOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  instance->ExportStateMachineDescription( dotOutputFile, skipLoops );
  dotOutputFile.close();


  std::string ltsfilename = filename + ".lts";
  std::ofstream ltsOutputFile;
  ltsOutputFile.open( ltsfilename.c_str() );
  if( ltsOutputFile.fail() )
    {
    std::cerr << "Problem opening the file " << filename << std::endl;
    itk::ExceptionObject excp;
    excp.SetDescription("Problem opening file");
    throw excp;
    }
  instance->ExportStateMachineDescriptionToLTS( ltsOutputFile, skipLoops );
  ltsOutputFile.close();

  }


} // end namespace igstk


// This is for classes that use SmartPointers
#define igstkTestExportStateMachine1( type, outputDirectory, skipLoops ) \
  { \
  type::Pointer instance = type::New(); \
  igstk::ExportStateMachineDescription( instance.GetPointer(), outputDirectory, skipLoops ); \
  }

// This is for classes that do not use SmartPointers and have a default constructor
#define igstkTestExportStateMachine2( type, outputDirectory, skipLoops ) \
  { \
  type * instance = new type; \
  igstk::ExportStateMachineDescription( instance, outputDirectory, skipLoops ); \
  delete instance; \
  }


#define igstkDeclareSurrogateClass( surrogate, type ) \
class surrogate : public type \
  {  \
public:      \
    typedef surrogate                      Self;    \
    typedef itk::Object                    Superclass;    \
    typedef itk::SmartPointer<Self>        Pointer;       \
    igstkTypeMacro( surrogate, type );   \
    igstkNewMacro( Self );      \
  };    \


namespace igstk
{
  typedef ImageSpatialObject<float,3>                    ImageSpatialObjectType;
  typedef ImageReader< ImageSpatialObjectType >          ImageReaderType;
  typedef DICOMImageReader< ImageSpatialObjectType >     DICOMImageReaderType;
  typedef ImageSpatialObjectRepresentation< 
                            ImageSpatialObjectType >     ImageSpatialObjectRepresentationType;
  typedef SpatialObjectReader<3,float>                   SpatialObjectReaderType;

  igstkDeclareSurrogateClass( SpatialObjectSurrogate, SpatialObject );
  igstkDeclareSurrogateClass( ImageSpatialObjectSurrogate, ImageSpatialObjectType );
  igstkDeclareSurrogateClass( DICOMImageReaderSurrogate, DICOMImageReaderType );
}


int main( int argc, char * argv [] )
{
  
  std::string outputDirectory;

  if( argc > 1 )
    {
    outputDirectory = argv[1];
    }

  bool skipLoops = true;

  if( argc > 2 )
    {
    skipLoops = atoi( argv[2] );
    }

  std::cout << "Output directory = " << outputDirectory << std::endl;
  std::cout << "Skip Loops option = " << skipLoops << std::endl;
  

  // This is for classes that use SmartPointers
  igstkTestExportStateMachine1( igstk::CylinderObjectRepresentation, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::EllipsoidObjectRepresentation, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::EllipsoidObject, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::CylinderObject, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::PulseGenerator, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::Tracker, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::TrackerTool, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::Landmark3DRegistration, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::CTImageReader, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::MRImageReader, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::MRImageSpatialObject, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::MRImageSpatialObjectRepresentation, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::CTImageSpatialObject, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::CTImageSpatialObjectRepresentation, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ImageSpatialObjectRepresentationType, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::SpatialObjectReaderType, outputDirectory, skipLoops );


#if IGSTK_USE_FLTK
  // The View classes don't use SmartPointer and don't have a default constructor.
  igstk::View2D view2D(0,0, 100, 100, "dummy view for testing");
  igstk::ExportStateMachineDescription( &view2D, outputDirectory, skipLoops ); 

  igstk::View3D view3D(0,0, 100, 100, "dummy view for testing");
  igstk::ExportStateMachineDescription( &view3D, outputDirectory, skipLoops ); 
#endif


  // Exporting Abstract classes by creating derived surrogates for them.
#ifdef IGSTK_BUILD_SHARED_LIBS
  igstkTestExportStateMachine1( igstk::SpatialObjectSurrogate, outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::ImageSpatialObjectSurrogate , outputDirectory, skipLoops );
  igstkTestExportStateMachine1( igstk::DICOMImageReaderSurrogate, outputDirectory, skipLoops );
#endif


  // Export the state diagrams for the Serial Communication classes according
  // to the current platform.
#ifdef WIN32
  igstkTestExportStateMachine1( igstk::SerialCommunicationForWindows, outputDirectory, skipLoops );
#else
  igstkTestExportStateMachine1( igstk::SerialCommunicationForPosix, outputDirectory, skipLoops );
#endif


  return EXIT_SUCCESS;
}

