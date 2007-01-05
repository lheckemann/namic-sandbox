#include "itkImage.h"
#include "itkImageFileReader.h"
//#include "itkBinaryThresholdImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkPNGImageIO.h"
#include "itkImageSeriesWriter.h"

int main( int argc, char * argv [] )
{

  if( argc < 4 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: InputImage OutputImagePrefix OutputImageSuffix" << std::endl;
    return -1;
    }

  /* Read an image from file */
  std::cerr << "Reading image from file..." << std::endl;
  typedef itk::Image< unsigned char, 3 > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
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

  /* Rescale Input Image */
  std::cerr << "Rescaling input image..." << std::endl;
  typedef itk::RescaleIntensityImageFilter< ImageType, ImageType > RescaleFilterType;
  RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
  rescaleFilter->SetInput( reader->GetOutput() );
  rescaleFilter->SetOutputMinimum( 0 );
  rescaleFilter->SetOutputMaximum( 255 );
  rescaleFilter->Update();

//   std::cerr << "Threshold Image Filter: Rescale to 255..." << std::endl;
//   typedef itk::BinaryThresholdImageFilter< ImageType, ImageType > ThresholdFilterType;
//   ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
//   thresholdFilter->SetInput( reader->GetOutput() );
//   thresholdFilter->SetOutsideValue( 255 );
//   thresholdFilter->SetInsideValue( 0 );
//   thresholdFilter->SetLowerThreshold( 0.5 );
//   thresholdFilter->Update();

  /* Write an image series to file */
  std::cerr << "Writing image series to file..." << std::endl;
  typedef itk::Image< unsigned char, 2 > Image2DType;
  typedef itk::ImageSeriesWriter< ImageType, Image2DType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( rescaleFilter->GetOutput() );
  typedef itk::NumericSeriesFileNames NameGeneratorType;
  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  std::string format = argv[2];
  format += "%03d.";
  format += argv[3]; // filename extension
  nameGenerator->SetSeriesFormat( format.c_str() );
  ImageType::ConstPointer inputImage = rescaleFilter->GetOutput();
  ImageType::RegionType region = inputImage->GetLargestPossibleRegion();
  ImageType::IndexType start = region.GetIndex();
  ImageType::SizeType size = region.GetSize();  const unsigned int firstSlice = start[2];
  const unsigned int lastSlice = start[2] + size[2] - 1;
  nameGenerator->SetStartIndex( firstSlice );
  nameGenerator->SetEndIndex( lastSlice );
  nameGenerator->SetIncrementIndex( 1 );
  writer->SetFileNames( nameGenerator->GetFileNames() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown while reading the image" << std::endl;
    std::cerr << excp << std::endl;
    } 

  return EXIT_SUCCESS;
}
