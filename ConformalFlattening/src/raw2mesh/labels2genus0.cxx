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
  std::cerr << "## CONVERT LABEL TO GENUS 0 MASK ##" << std::endl;
  std::cerr << "######################################" << std::endl;
  std::cerr << std::endl;

  itk::FileOutputWindow::SetInstance( itk::FileOutputWindow::New() );

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

  /* Extract labels of interest */
  std::cerr << "Extracting labels of interest..." << std::endl;
  InputImageType::Pointer inputImage = reader->GetOutput();
  IteratorType extractLabelIt( inputImage, inputImage->GetLargestPossibleRegion() );
  int count = 0;
  for ( extractLabelIt.GoToBegin(); !extractLabelIt.IsAtEnd(); ++extractLabelIt )
    {
    if( extractLabelIt.Get()==11 ||
        extractLabelIt.Get()==13
      )
      {
      extractLabelIt.Set( 1 );
      count++;
      }
    else
      {
      extractLabelIt.Set( 0 );
      }
    }
  std::cerr << "  A Total of " << count << " pixels were extracted." << std::endl;

  /* Threshold Image Filter: Force Image to Binary */
  std::cerr << "Threshold Image Filter: Force Image to Binary..." << std::endl;
  ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
  thresholdFilter->SetInput( inputImage );
  thresholdFilter->SetOutsideValue( 0 );
  thresholdFilter->SetInsideValue( 1 );
  thresholdFilter->SetLowerThreshold( 1 );
  thresholdFilter->Update();

  /* Connected Component Filter: Used to Remove Extraneous Regions */
  std::cerr << "Connected Component Filter: Used to Remove Extraneous Regions..." << std::endl;
  ConnectedComponentType::Pointer removeExtraComponentsFilter = ConnectedComponentType::New();
  removeExtraComponentsFilter->SetInput( thresholdFilter->GetOutput() );
  removeExtraComponentsFilter->SetFullyConnected( 0 );
  removeExtraComponentsFilter->Update();

  /* Relabel Filter: Used to Remove Extraneous Regions */
  std::cerr << "Relabel Filter: Used to Remove Extraneous Regions..." << std::endl;
  RelabelType::Pointer removeExtraRelabelFilter = RelabelType::New();
  removeExtraRelabelFilter->SetInput( removeExtraComponentsFilter->GetOutput() );
  removeExtraRelabelFilter->Update();
  std::cerr << "  Total # of connected components, before removing extraneous regions = " << removeExtraRelabelFilter->GetNumberOfObjects() << std::endl;

  /* Iterate through the Image to Remove Extraneous Regions */
  std::cerr << "Iterate through the Image to Remove Extraneous Regions... " << std::endl;
  InputImageType::Pointer outputImage = thresholdFilter->GetOutput();
  InputImageType::RegionType imageRegion = outputImage->GetLargestPossibleRegion();
  ConstIteratorType removeExtraIt( removeExtraRelabelFilter->GetOutput(), removeExtraRelabelFilter->GetOutput()->GetLargestPossibleRegion() );
  IteratorType outputIt( outputImage, imageRegion );
  count = 0;
  for ( removeExtraIt.GoToBegin(), outputIt.GoToBegin(); !removeExtraIt.IsAtEnd(); ++removeExtraIt, ++outputIt )
    {
    if( removeExtraIt.Get() > 1 )
      {
      outputIt.Set( 0 );
      count++;
      }
    }
  std::cerr << "  A Total of " << count << " pixels were filled." << std::endl;

  /* Invert Intensities: Before Filling Holes */
  std::cerr << "Invert Intensities: Before Filling Holes..." << std::endl;
  InvertFilterType::Pointer invertFilter = InvertFilterType::New();
  invertFilter->SetFunctor( invertFilter->GetFunctor() );
  invertFilter->SetInput( outputImage );
  invertFilter->SetMaximum( 1 );
  invertFilter->Update();

  /* Connected Component Filter: Used to Fill Holes */
  std::cerr << "Connected Component Filter: Used to Fill Holes... " << std::endl;
  ConnectedComponentType::Pointer fillHolesComponentsFilter = ConnectedComponentType::New();
  fillHolesComponentsFilter->SetInput( invertFilter->GetOutput() );
  fillHolesComponentsFilter->SetFullyConnected( 0 );
  fillHolesComponentsFilter->Update();

  /* Relabel Filter: Used to Fill Holes */
  std::cerr << "Relabel Filter: Used to Fill Holes... " << std::endl;
  RelabelType::Pointer fillHolesRelabelFilter = RelabelType::New();
  fillHolesRelabelFilter->SetInput( fillHolesComponentsFilter->GetOutput() );
  fillHolesRelabelFilter->Update();
  std::cerr << "  Total # of connected components, before filling holes = " << fillHolesRelabelFilter->GetNumberOfObjects() << std::endl;

  /* Iterate through the Image to Fill the Holes */
  std::cerr << "Iterate through the Image to Fill the Holes... " << std::endl;
  ConstIteratorType inputIt( fillHolesRelabelFilter->GetOutput(), fillHolesRelabelFilter->GetOutput()->GetLargestPossibleRegion() );
  count = 0;
  for ( inputIt.GoToBegin(), outputIt.GoToBegin(); !inputIt.IsAtEnd(); ++inputIt, ++outputIt )
    {
    if( inputIt.Get() > 1 )
      {
      outputIt.Set( 1 );
      count++;
      }
    }
  std::cerr << "  A Total of " << count << " pixels were filled." << std::endl;

  /* Write Output to File */
  std::cerr << "Writing Genus 0 Image to File... " << std::endl;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );
  writer->SetInput( outputImage );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  reader = 0;
  writer = 0;

  std::cerr << "SUCCESS!" << std::endl;

  return 1;
}
