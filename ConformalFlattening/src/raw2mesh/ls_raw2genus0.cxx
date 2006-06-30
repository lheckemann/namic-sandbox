#include "ls_raw2genus0.h"

int main( int argc, char *argv[] )
{
  /* Check for correct number of command-line arguments */
  if( argc < 2 )
    {
    std::cerr << "Usage arguments: InputImage OutputImage" << std::endl;
    return EXIT_FAILURE;
    }
  
  std::cerr << "######################################" << std::endl;
  std::cerr << "## CONVERT RAW MASK TO GENUS 0 MASK ##" << std::endl;
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

  /* Threshold Image Filter: Force Image to Binary */
  std::cerr << "Threshold Image Filter: Force Image to Binary..." << std::endl;
  ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
  thresholdFilter->SetInput( reader->GetOutput() );
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
  ThresholdFilterType::OutputImageType::Pointer outputImage = thresholdFilter->GetOutput();
  ThresholdFilterType::OutputImageType::RegionType imageRegion = outputImage->GetLargestPossibleRegion();
  ConstIteratorType removeExtraIt( removeExtraRelabelFilter->GetOutput(), removeExtraRelabelFilter->GetOutput()->GetLargestPossibleRegion() );
  IteratorType outputIt( outputImage, imageRegion );
  int count = 0;
  for ( removeExtraIt.GoToBegin(), outputIt.GoToBegin(); !removeExtraIt.IsAtEnd(); ++removeExtraIt, ++outputIt )
    {
    if( removeExtraIt.Get() > 1 )
      {
      outputIt.Set( 0 );
      count++;
      }
    }
  std::cerr << "  A Total of " << count << " pixels were filled." << std::endl;

  /* Smoothing Filter */
  std::cerr << "Smoothing Filter... " << std::endl;
  SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  smoothing->SetInput( outputImage );
  smoothing->SetTimeStep( 0.125 );
  smoothing->SetNumberOfIterations(  5 );
  smoothing->SetConductanceParameter( 9.0 );
  smoothing->Update();

  /* Gradient Filter */
  std::cerr << "Gradient Filter... " << std::endl;
  GradientFilterType::Pointer gradientMagnitude = GradientFilterType::New();
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  gradientMagnitude->SetSigma( 1.0 );
  gradientMagnitude->Update();

  /* Sigmoid Filter */
  std::cerr << "Sigmoid Filter... " << std::endl;
  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
  sigmoid->SetOutputMinimum(  0.0  );
  sigmoid->SetOutputMaximum(  1.0  );
  sigmoid->SetAlpha( 1.0 );
  sigmoid->SetBeta( 1.0 );
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  sigmoid->Update();

  /* Fast Marching Filter */
  std::cerr << "Fast Marching... " << std::endl;
  const double initialDistance = 50;
  const double seedValue = - initialDistance;
  FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();
  NodeContainer::Pointer seeds = NodeContainer::New();
  InputImageType::IndexType  seedPosition;
  seedPosition[0] = 150;
  seedPosition[1] = 150;
  seedPosition[2] = 150;
  NodeType node;
  node.SetValue( seedValue );
  node.SetIndex( seedPosition );
  seeds->Initialize();
  seeds->InsertElement( 0, node );
  fastMarching->SetTrialPoints(  seeds  );
  fastMarching->SetSpeedConstant( 1.0 );
  fastMarching->SetOutputSize( reader->GetOutput()->GetBufferedRegion().GetSize() );

  /* TPGAC Filter */
  std::cerr << "TPGAC... " << std::endl;
  TPGACFilterType::Pointer tpgac = TPGACFilterType::New();
  tpgac->SetPropagationScaling( 1.0 );
  tpgac->SetCurvatureScaling( 1.0 );
  tpgac->SetAdvectionScaling( 1.0 );
  tpgac->SetMaximumRMSError( 0.02 );
  tpgac->SetNumberOfIterations( 800 );
  tpgac->SetInput( fastMarching->GetOutput() );
  tpgac->SetFeatureImage( sigmoid->GetOutput() );

  /* Threshold Filter */
  std::cerr << "Threshold... " << std::endl;
  ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
  thresholder->SetInput( tpgac->GetOutput() );
  thresholder->SetLowerThreshold( -1000.0 );
  thresholder->SetUpperThreshold(     0.0 );
  thresholder->SetOutsideValue(  0  );
  thresholder->SetInsideValue(  255 );

  /* Write Image to File */
  std::cerr << "Write Image to File... " << std::endl;
  CastFilterType::Pointer caster1 = CastFilterType::New();
  CastFilterType::Pointer caster2 = CastFilterType::New();
  CastFilterType::Pointer caster3 = CastFilterType::New();
  CastFilterType::Pointer caster4 = CastFilterType::New();

  WriterType::Pointer writer = WriterType::New();
  WriterType::Pointer writer1 = WriterType::New();
  WriterType::Pointer writer2 = WriterType::New();
  WriterType::Pointer writer3 = WriterType::New();
  WriterType::Pointer writer4 = WriterType::New();

  caster1->SetInput( smoothing->GetOutput() );
  writer1->SetInput( caster1->GetOutput() );
  writer1->SetFileName("TPGACImageFilterOutput1.png");
  caster1->SetOutputMinimum(   0 );
  caster1->SetOutputMaximum( 255 );
  writer1->Update();

  caster2->SetInput( gradientMagnitude->GetOutput() );
  writer2->SetInput( caster2->GetOutput() );
  writer2->SetFileName("TPGACImageFilterOutput2.png");
  caster2->SetOutputMinimum(   0 );
  caster2->SetOutputMaximum( 255 );
  writer2->Update();

  caster3->SetInput( sigmoid->GetOutput() );
  writer3->SetInput( caster3->GetOutput() );
  writer3->SetFileName("TPGACImageFilterOutput3.png");
  caster3->SetOutputMinimum(   0 );
  caster3->SetOutputMaximum( 255 );
  writer3->Update();

  caster4->SetInput( fastMarching->GetOutput() );
  writer4->SetInput( caster4->GetOutput() );
  writer4->SetFileName("TPGACImageFilterOutput4.png");
  caster4->SetOutputMinimum(   0 );
  caster4->SetOutputMaximum( 255 );
  writer4->Update();

  writer->SetInput( thresholder->GetOutput() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  // Print out some useful information 
  std::cout << std::endl;
  std::cout << "Max. no. iterations: " << tpgac->GetNumberOfIterations() << std::endl;
  std::cout << "Max. RMS error: " << tpgac->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "No. elpased iterations: " << tpgac->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << tpgac->GetRMSChange() << std::endl;

  WriterType::Pointer mapWriter = WriterType::New();
  mapWriter->SetInput( fastMarching->GetOutput() );
  mapWriter->SetFileName("TPGACImageFilterOutput4.mha");
  mapWriter->Update();

  WriterType::Pointer speedWriter = WriterType::New();
  speedWriter->SetInput( sigmoid->GetOutput() );
  speedWriter->SetFileName("TPGACImageFilterOutput3.mha");
  speedWriter->Update();

  WriterType::Pointer gradientWriter = WriterType::New();
  gradientWriter->SetInput( gradientMagnitude->GetOutput() );
  gradientWriter->SetFileName("TPGACImageFilterOutput2.mha");
  gradientWriter->Update();

  return 0;
}
