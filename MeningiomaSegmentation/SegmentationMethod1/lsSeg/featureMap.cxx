#include "featureMap.h"


int main( int argc, char *argv[] )
{
  if( 5 != argc )
    {
      std::cerr << "Missing Parameters " << std::endl;
      std::cerr << "Usage: " << argv[0];
      std::cerr << " inputImage  outputImage"  << std::endl;
      return 1;
    }

  /* 
   * read in the image
   */  
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );
  reader->Update();

  InternalImageType::Pointer inputImage = reader->GetOutput();


  /*
   * smooth input image
   */
  SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  smoothing->SetInput( inputImage );
  smoothing->SetTimeStep( 0.05 );
  smoothing->SetNumberOfIterations(  5 );
  smoothing->SetConductanceParameter( 9.0 );

  /*
   * get gradient magnitude map
   */
  GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  gradientMagnitude->SetSigma( 1.0 );

  /*
   * sigmoid transform
   */
  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  sigmoid->SetOutputMinimum(  0.0  );
  sigmoid->SetOutputMaximum(  1.0  );
  const double alpha =  -1;
  const double beta  =  20;
//   const double alpha =  atof( argv[3] );
//   const double beta  =  atof( argv[4] );

  sigmoid->SetAlpha( alpha );
  sigmoid->SetBeta(  beta  );


  /* write out the sigmoid result*/
  writer->SetFileName( argv[2] );
  writer->SetInput( sigmoid->GetOutput() );
  
  try
    {
      writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }


  return 0;
}
