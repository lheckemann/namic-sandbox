#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkImage.h"
#include "itkFastMarchingImageFilter.h"
#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"


int main( int argc, char *argv[] )
{
  if( argc < 9 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage  inputLabelImage outputImage";
    std::cerr << " Sigma SigmoidAlpha SigmoidBeta ";
    std::cerr << " curvatureScaling propagationScaling" << std::endl;
    return 1;
    }

  typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 3;
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;
  typedef unsigned char                            OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::BinaryThresholdImageFilter< InternalImageType, OutputImageType >
    ThresholdingFilterType;
  typedef itk::SignedMaurerDistanceMapImageFilter<InternalImageType,InternalImageType> DistanceFilterType;

  ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
                        
  thresholder->SetLowerThreshold( -1000.0 );
  thresholder->SetUpperThreshold(     0.0 );

  thresholder->SetOutsideValue(  0  );
  thresholder->SetInsideValue(  255 );

  typedef  itk::ImageFileReader< InternalImageType > ReaderType;
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  ReaderType::Pointer maskReader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );
  maskReader->SetFileName( argv[2] );
  writer->SetFileName( argv[3] );
  const double sigma = atof( argv[4] );
  const double alpha =  atof( argv[5] );
  const double beta  =  atof( argv[6] );
  const double curvatureScaling   = atof( argv[ 7 ] ); 
  const double propagationScaling = atof( argv[ 8 ] ); 


  typedef itk::RescaleIntensityImageFilter<InternalImageType, OutputImageType>
    CastFilterType;

  typedef   itk::CurvatureAnisotropicDiffusionImageFilter< 
                               InternalImageType, 
                               InternalImageType >  SmoothingFilterType;

  SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();

  typedef   itk::GradientMagnitudeRecursiveGaussianImageFilter< 
                               InternalImageType, 
                               InternalImageType >  GradientFilterType;

  typedef   itk::SigmoidImageFilter<
                               InternalImageType, 
                               InternalImageType >  SigmoidFilterType;

  GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();


  sigmoid->SetOutputMinimum(  0.0  );
  sigmoid->SetOutputMaximum(  1.0  );

  DistanceFilterType::Pointer dtFilter = DistanceFilterType::New();
  dtFilter->SetInput(maskReader->GetOutput());
  dtFilter->SetSquaredDistance(0);

  typedef  itk::ShapeDetectionLevelSetImageFilter< InternalImageType, 
                              InternalImageType >    ShapeDetectionFilterType;
  ShapeDetectionFilterType::Pointer 
    shapeDetection = ShapeDetectionFilterType::New();

  smoothing->SetInput( reader->GetOutput() );
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  
  shapeDetection->SetInput( dtFilter->GetOutput() );
  shapeDetection->SetFeatureImage( sigmoid->GetOutput() );

  thresholder->SetInput( shapeDetection->GetOutput() );

  writer->SetInput( thresholder->GetOutput() );

  smoothing->SetTimeStep( 0.06 );
  smoothing->SetNumberOfIterations(  5 );
  smoothing->SetConductanceParameter( 9.0 );

  gradientMagnitude->SetSigma(  sigma  );

  sigmoid->SetAlpha( alpha );
  sigmoid->SetBeta(  beta  );

  CastFilterType::Pointer caster1 = CastFilterType::New();
  CastFilterType::Pointer caster2 = CastFilterType::New();
  CastFilterType::Pointer caster3 = CastFilterType::New();

  WriterType::Pointer writer1 = WriterType::New();
  WriterType::Pointer writer2 = WriterType::New();
  WriterType::Pointer writer3 = WriterType::New();

  caster1->SetInput( smoothing->GetOutput() );
  writer1->SetInput( caster1->GetOutput() );
  writer1->SetFileName("Smoothing.nrrd");
  caster1->SetOutputMinimum(   0 );
  caster1->SetOutputMaximum( 255 );
  writer1->Update();

  caster2->SetInput( gradientMagnitude->GetOutput() );
  writer2->SetInput( caster2->GetOutput() );
  writer2->SetFileName("GradientMagnitude.nrrd");
  caster2->SetOutputMinimum(   0 );
  caster2->SetOutputMaximum( 255 );
  writer2->Update();

  caster3->SetInput( sigmoid->GetOutput() );
  writer3->SetInput( caster3->GetOutput() );
  writer3->SetFileName("Sigmoid.nrrd");
  caster3->SetOutputMinimum(   0 );
  caster3->SetOutputMaximum( 255 );
  writer3->Update();

  shapeDetection->SetPropagationScaling(  propagationScaling );
  shapeDetection->SetCurvatureScaling( curvatureScaling ); 

  shapeDetection->SetMaximumRMSError( 0.02 );
  shapeDetection->SetNumberOfIterations( 800 );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  std::cout << std::endl;
  std::cout << "Max. no. iterations: " << shapeDetection->GetNumberOfIterations() << std::endl;
  std::cout << "Max. RMS error: " << shapeDetection->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "No. elpased iterations: " << shapeDetection->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << shapeDetection->GetRMSChange() << std::endl;

  typedef itk::ImageFileWriter< InternalImageType > InternalWriterType;

  InternalWriterType::Pointer speedWriter = InternalWriterType::New();
  speedWriter->SetInput( sigmoid->GetOutput() );
  speedWriter->SetFileName("ShapeDetectionLevelSetFilterOutput3.mha");
  speedWriter->Update();

  InternalWriterType::Pointer gradientWriter = InternalWriterType::New();
  gradientWriter->SetInput( gradientMagnitude->GetOutput() );
  gradientWriter->SetFileName("ShapeDetectionLevelSetFilterOutput2.mha");
  gradientWriter->Update();

  return 0;
}
