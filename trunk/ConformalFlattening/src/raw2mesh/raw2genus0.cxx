#include "raw2genus0.h"

int main(int argc, char *argv[])
{
  /* Check for right number of command-line arguments */
  if( argc < 2 )
    {
    std::cerr << "Usage arguments: InputImage OutputImage" << std::endl;
    return EXIT_FAILURE;
    }
  
  std::cerr << "######################################" << std::endl;
  std::cerr << "## CONVERT RAW MASK TO GENUS 0 MASK ##" << std::endl;
  std::cerr << "######################################" << std::endl;
  std::cerr << std::endl;

  /* Readin the Skull-Stripped Binary Image from File */
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  /* Connected Component Filter: Used to Remove Extraneous Regions */
  std::cerr << "Removing Extraneous Regions... " << std::endl;
  ConnectedComponentType::Pointer removeExtraComponentsFilter = ConnectedComponentType::New();
  removeExtraComponentsFilter->SetInput( reader->GetOutput() );
  removeExtraComponentsFilter->SetFullyConnected( 0 );
  std::cerr << "  GetFullyConnected = " << removeExtraComponentsFilter->GetFullyConnected() << std::endl;

  /* Relabel Filter: Used to Remove Extraneous Regions */
  RelabelType::Pointer removeExtraRelabelFilter = RelabelType::New();
  removeExtraRelabelFilter->SetInput( removeExtraComponentsFilter->GetOutput() );
  removeExtraRelabelFilter->InPlaceOn();
  removeExtraRelabelFilter->Update();
  std::cerr << "  Total # of connected components, before removing extraneous regions = " << removeExtraRelabelFilter->GetNumberOfObjects() << std::endl;

  /* Connected Component Filter: Used to Fill Holes */
  std::cerr << "Filling Holes... " << std::endl;
  ConnectedComponentType::Pointer fillHolesComponentsFilter = ConnectedComponentType::New();
  fillHolesComponentsFilter->SetInput( removeExtraComponentsFilter->GetOutput() );
  fillHolesComponentsFilter->SetFullyConnected( 0 );
  std::cerr << "  GetFullyConnected = " << fillHolesComponentsFilter->GetFullyConnected() << std::endl;

  /* Relabel Filter: Used to Fill Holes */
  RelabelType::Pointer fillHolesRelabelFilter = RelabelType::New();
  fillHolesRelabelFilter->SetInput( fillHolesComponentsFilter->GetOutput() );
  fillHolesRelabelFilter->InPlaceOn();
  fillHolesRelabelFilter->Update();
  std::cerr << "  Total # of connected components, before filling holes = " << fillHolesRelabelFilter->GetNumberOfObjects() << std::endl;

  /* Fill the Holes */
  RelabelType::OutputImageType::Pointer outputImage = fillHolesRelabelFilter->GetOutput();
  InputImageType::RegionType region = fillHolesRelabelFilter->GetOutput()->GetLargestPossibleRegion();
  ConstIteratorType inputIt( fillHolesRelabelFilter->GetOutput(), region );
  IteratorType outputIt( outputImage, region );
  for( inputIt.GoToBegin(), outputIt.GoToBegin(); !inputIt.IsAtEnd(); ++inputIt, ++outputIt )
    {
    if( inputIt.Get() > 0 )
      outputIt.Set( 1 );
    else
      outputIt.Set( 0 );
    }

  /* Write Relabeled Image to File (to help with debugging) */
  std::cerr << "Writing Genus 0 Image to File... " << std::endl;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );
  writer->SetInput( outputImage );
  writer->Update();
  writer = 0;

  return 1;
}
