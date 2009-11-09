#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkGridImageSource.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkBSplineKernelFunction.h"
#include "itkOrientedImage.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkNotImageFilter.h"

#include "itkPluginUtilities.h"
#include "ImageGridGeneratorCLP.h"

namespace {

int main(int argc, char* arv[])
{

  PARSE_ARGS;

  typedef    float       InputPixelType;
  typedef    char       OutputPixelType;

  typedef itk::Image< InputPixelType,  3 >   InputImageType;
  typedef itk::Image< OutputPixelType, 3 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType;

  typedef itk::BinaryThresholdImageFilter<InputImageType,OutputImageType> ThresholdType;
  typedef itk::NotImageFilter<OutputImageType,OutputImageType> NotType;

  // Instantiate the filter
  typedef itk::GridImageSource<OutputImageType> GridSourceType;
  typedef itk::ImageFileReader<InputImageType> ReaderType;

  typename GridSourceType::Pointer gridImage = GridSourceType::New();
  typename ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName(inputImage.c_str());
  reader->Update();

  typename InputImageType::Pointer inImage = reader->GetOutput();

  double scale = 255.0;
  typename ImageType::SizeType size;
  typename ImageType::PointType origin;
  typename ImageType::SpacingType spacing;
  typename GridSourceType::ArrayType gridSpacing; 
  typename GridSourceType::ArrayType gridOffset; 
  typename GridSourceType::ArrayType sigma; 
  typename GridSourceType::BoolArrayType which; 

  // Specify image parameters
  origin.Fill( 0.0 );
  size.Fill( 128 );
  spacing.Fill( 1.0 );
  
  // Specify grid parameters
  gridSpacing.Fill( 16.0 );
  gridOffset.Fill( 0.0 );
  sigma.Fill( 1 );
  which.Fill( true );

  spacing = inImage->GetSpacing();
  sigma[0] = spacing[0];
  sigma[1] = spacing[1];
  sigma[2] = spacing[2];

  size = inImage->GetLargestPossibleRegion().GetSize();
  gridSpacing[0] = size[0]*spacing[0]/10.;
  gridSpacing[1] = size[1]*spacing[1]/10.;
  gridSpacing[2] = size[2]*spacing[2]/10.;

  // Specify 0th order B-spline function (Box function)
  typedef itk::BSplineKernelFunction<0> KernelType;
  KernelType::Pointer kernel = KernelType::New();
  
  // Set parameters
  gridImage->SetKernelFunction( kernel );

  // Set parameters
  gridImage->SetSpacing( inImage->GetSpacing() );
  gridImage->SetOrigin( inImage->GetOrigin() );
  gridImage->SetSize( inImage->GetLargestPossibleRegion().GetSize() );
  gridImage->SetGridSpacing( gridSpacing );
  gridImage->SetGridOffset( gridOffset );
  gridImage->SetWhichDimensions( which );
  gridImage->SetSigma( sigma );
  gridImage->SetScale( scale );

  
  try
    {
    gridImage->Update();
    }
  catch (itk::ExceptionObject & err)
    { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
    return EXIT_FAILURE;
    }

  typename ImageType::Pointer outImage = gridImage->GetOutput();

  typename NotType::Pointer notf = NotType::New();
  notf->SetInput(gridImage->GetOutput());
  notf->Update();

  outImage = notf->GetOutput();
  outImage->SetDirection(inImage->GetDirection());

  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImage.c_str() );
  writer->SetInput( outImage );
  writer->Update();
  
  return EXIT_SUCCESS;
}

} // end of anonymous namespace
