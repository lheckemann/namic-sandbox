/*******************************************************************************
Program:   Demons Registration using ITK
Id:        $Id$
Language:  C++
*******************************************************************************/

#include <iostream>
#include <cstdlib>

// ITK IO includes
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// ITK Registration includes
#include "itkMultiResolutionPDEDeformableRegistration.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkDemonsRegistrationFilter.h" 
#include "itkHistogramMatchingImageFilter.h" 
#include "itkCastImageFilter.h" 
#include "itkWarpImageFilter.h" 
#include "itkLinearInterpolateImageFunction.h"

unsigned int RmsCounter = 0;
double MaxRmsE[4] = {0.8,  0.75,  0.4, 0.2};

//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.

//
  class CommandIterationUpdate : public itk::Command 
  {
  public:
    typedef  CommandIterationUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
  protected:
    CommandIterationUpdate() {};

     // define ITK short-hand types
  //const unsigned int Dimension = 3; 
  typedef short PixelType;
  typedef float InternalPixelType; 
  typedef itk::Image< PixelType, 3 > ImageType;
  typedef itk::Image< InternalPixelType, 3 > InternalImageType;
    typedef itk::Vector< float, 3 > VectorPixelType; 
  typedef itk::Image< VectorPixelType, 3 > DeformationFieldType; 
  typedef itk::DemonsRegistrationFilter< InternalImageType, 
    InternalImageType, DeformationFieldType> RegistrationFilterType;

  public:

    void Execute(const itk::Object *caller, const itk::EventObject & event)
      {
      std::cout << "Warning: The const Execute method shouldn't be called" << std::endl;
      } 

    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
         RegistrationFilterType * filter = 
          dynamic_cast<  RegistrationFilterType * >( caller );

         if( !(itk::IterationEvent().CheckEvent( &event )) )
          {
          return;
          }
        if(filter)
          {
          filter->SetMaximumRMSError(MaxRmsE[RmsCounter]);
          std::cout << filter->GetMetric() <<  "  RMS Change: " << filter->GetRMSChange() << std::endl;
           
           std::cout << "Level Tolerance=  "<<filter->GetMaximumRMSError ()<<std::endl;
      }

  }
};
  
// define ITK short-hand types
      
       

class CommandResolutionLevelUpdate : public itk::Command 
  {
  public:
    typedef  CommandResolutionLevelUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
     
  protected:
    CommandResolutionLevelUpdate() {};

    
  
  public:

    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
      Execute( (const itk::Object *)caller, event);
      };
    void Execute(const itk::Object * object, const itk::EventObject & event)
      {
      std::cout << "----------------------------------" << std::endl;
      RmsCounter = RmsCounter + 1;
      
          
      std::cout << "----------------------------------" << std::endl;
      
      }
  };



//using namespace std;
int main( int argc, char * argv [] )
{
  
  // Verify the number of parameters in the command line
  if( argc != 5 )
    {
    std::clog << "usage: " << std::endl 
    std::clog << argv[0] << " target source output deformation" << std::endl;
    return EXIT_FAILURE;
    }
  
  // define ITK short-hand types
  const unsigned int Dimension = 3; 
  typedef short PixelType;
  typedef float InternalPixelType; 
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;
  typedef itk::CastImageFilter< ImageType, InternalImageType > ImageCasterType;
     
    


  // setup input file readers
  typedef itk::ImageFileReader< ImageType >  ReaderType;
  ReaderType::Pointer targetReader = ReaderType::New();
  targetReader->SetFileName( argv[1] );
  targetReader->Update();
  //targetReader->GetOutput()->Print(std::cout);
  ReaderType::Pointer sourceReader = ReaderType::New();
  sourceReader->SetFileName( argv[2] );
  sourceReader->Update();
  //sourceReader->GetOutput()->Print(std::cout);

  // cast target and source to float
  ImageCasterType::Pointer targetImageCaster = ImageCasterType::New(); 
  ImageCasterType::Pointer sourceImageCaster = ImageCasterType::New(); 
  targetImageCaster->SetInput( targetReader->GetOutput() ); 
  sourceImageCaster->SetInput( sourceReader->GetOutput() );
  
  // match the histograms between source and target
  typedef itk::HistogramMatchingImageFilter< InternalImageType, InternalImageType > MatchingFilterType; 
  MatchingFilterType::Pointer matcher = MatchingFilterType::New();
  matcher->SetInput( sourceImageCaster->GetOutput() ); 
  matcher->SetReferenceImage( targetImageCaster->GetOutput() );
  matcher->SetNumberOfHistogramLevels( 1024 ); 
  matcher->SetNumberOfMatchPoints( 7 );
  matcher->ThresholdAtMeanIntensityOn();

  // setup the deformation field and filter
  typedef itk::Vector< float, Dimension > VectorPixelType; 
  typedef itk::Image< VectorPixelType, Dimension > DeformationFieldType; 
  typedef itk::DemonsRegistrationFilter< InternalImageType, 
    InternalImageType, DeformationFieldType> RegistrationFilterType; 
  RegistrationFilterType::Pointer filter = RegistrationFilterType::New();
  filter->SetStandardDeviations( 1.0 );
   
  

    // Create the Command observer and register it with the registration filter.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  filter->AddObserver( itk::IterationEvent(), observer );
  

  // use multiresolution scheme
  typedef itk::MultiResolutionPDEDeformableRegistration
    < InternalImageType, InternalImageType, DeformationFieldType >
    MultiResRegistrationFilterType;
  MultiResRegistrationFilterType::Pointer 
    multires = MultiResRegistrationFilterType::New();
  multires->SetRegistrationFilter( filter );
  multires->SetNumberOfLevels( 4 );
  multires->SetFixedImage( targetImageCaster->GetOutput() );
  multires->SetMovingImage( matcher->GetOutput() );
  unsigned int nIterations[4] = {40, 40, 32, 32 };
  multires->SetNumberOfIterations( nIterations );
    // Create the Command observer and register it with the registration filter.
  //
  CommandResolutionLevelUpdate::Pointer levelobserver = CommandResolutionLevelUpdate::New();
  multires->AddObserver( itk::IterationEvent(), levelobserver );
  
  // apply the registration filter
  multires->Update();

  // compute the output (warped) image
  typedef itk::WarpImageFilter< ImageType, ImageType, DeformationFieldType > WarperType;
  typedef itk::LinearInterpolateImageFunction< ImageType, double > InterpolatorType; 
  WarperType::Pointer warper = WarperType::New(); 
  InterpolatorType::Pointer interpolator = InterpolatorType::New(); 
  ImageType::Pointer targetImage = targetReader->GetOutput(); 
  warper->SetInput( sourceReader->GetOutput() ); 
  warper->SetInterpolator( interpolator ); 
  warper->SetOutputSpacing( targetImage->GetSpacing() ); 
  warper->SetOutputOrigin( targetImage->GetOrigin() );
  warper->SetDeformationField( multires->GetOutput() );

  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( warper->GetOutput() );
  writer->Update();

  // write the deformation field
  typedef itk::ImageFileWriter< DeformationFieldType >  DeformationWriterType;
  DeformationWriterType::Pointer defwriter = DeformationWriterType::New();
  defwriter->SetFileName( argv[4] );
  defwriter->SetInput( multires->GetOutput() );
  defwriter->Update();

  return EXIT_SUCCESS;
}



