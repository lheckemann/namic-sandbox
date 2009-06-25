#include "gacSeg3.h"


int main( int argc, char *argv[] )
{
  if( argc < 6 )
    {
      std::cerr << "Missing Parameters " << std::endl;
      std::cerr << "Usage: " << argv[0];
      std::cerr << " inputImage  outputImage";
      std::cerr << " seedX seedY seedZ"<< std::endl;
      return 1;
    }


  int ctrX = atoi(argv[3]);
  int ctrY = atoi(argv[4]);
  int ctrZ = atoi(argv[5]);

  /* 
     10. read in the feature image: 0 contour slow region, 1, contour fast region
  */  
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );
  reader->Update();

  InternalImageType::Pointer inputImage = reader->GetOutput();



  /*
   * fast marching from seed point to get init level set
   */
  NodeContainer::Pointer seeds = NodeContainer::New();

  InternalImageType::IndexType  seedPosition;
  seedPosition[0] = ctrX;
  seedPosition[1] = ctrY;
  seedPosition[2] = ctrZ;

  const double seedValue = -5.0;

  NodeType node;  
  node.SetValue( seedValue );
  node.SetIndex( seedPosition );

  seeds->Initialize();
  seeds->InsertElement( 0, node );

  FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();
  fastMarching->SetTrialPoints(  seeds  );
  fastMarching->SetSpeedConstant( 1.0 );
  fastMarching->SetOutputSize( reader->GetOutput()->GetBufferedRegion().GetSize() );


  /*
   * GAC
   */
  GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
  //  const double advectionFactor = atof( argv[10] );
  geodesicActiveContour->SetPropagationScaling( 1.0 );
  geodesicActiveContour->SetCurvatureScaling( 1.0 );
  geodesicActiveContour->SetAdvectionScaling( 2.0 );

  geodesicActiveContour->SetMaximumRMSError( 0.02 );
  geodesicActiveContour->SetNumberOfIterations( 800 );

  geodesicActiveContour->SetInput(  fastMarching->GetOutput() );
  geodesicActiveContour->SetFeatureImage( inputImage );


  /*
   * threshold level set result 
   */
  ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
  thresholder->SetLowerThreshold( -1000.0 );
  thresholder->SetUpperThreshold(     0.0 );

  thresholder->SetOutsideValue(  0  );
  thresholder->SetInsideValue(  1 );


  /*
  * binary output
  */
  thresholder->SetInput( geodesicActiveContour->GetOutput() );
  writer->SetFileName( argv[2] );
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


  InternalWriterType::Pointer mapWriter = InternalWriterType::New();
  mapWriter->SetInput( fastMarching->GetOutput() );
  mapWriter->SetFileName("fastMarchingRslt.nrrd");
  mapWriter->Update();

  InternalWriterType::Pointer gacWriter = InternalWriterType::New();
  gacWriter->SetInput( geodesicActiveContour->GetOutput() );
  gacWriter->SetFileName("gacResult.nrrd");
  gacWriter->Update();
  


  return 0;
}



