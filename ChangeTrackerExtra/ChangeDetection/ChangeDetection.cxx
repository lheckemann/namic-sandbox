#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMedianImageFilter.h"

#include "itkImage.h"

using namespace std;

typedef int      PixelType;
const   unsigned int      Dimension = 3;
typedef itk::Image< PixelType, Dimension >    ImageType;

typedef itk::ImageFileReader< ImageType >  ReaderType;
typedef itk::ImageFileWriter< ImageType >  WriterType;
typedef itk::HistogramMatchingImageFilter<ImageType,ImageType> HistogramMatchingType;
typedef itk::ThresholdImageFilter<ImageType> ThresholdType;
typedef itk::MinimumMaximumImageFilter<ImageType> MinMaxType;
typedef itk::SubtractImageFilter<ImageType,ImageType> SubtractType;
typedef itk::MedianImageFilter<ImageType,ImageType> MedianType;

int main( int argc, char ** argv )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile1  inputImageFile2 thresholdMin thresholdMax segmentationImage outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  ReaderType::Pointer reader0 = ReaderType::New();
  ReaderType::Pointer reader1 = ReaderType::New();

  const char * inputFilename0  = argv[1];
  const char * inputFilename1 = argv[2];
  unsigned int threshold0 = atoi(argv[3]);
  unsigned int threshold1 = atoi(argv[4]);

  reader0->SetFileName( inputFilename0 );
  reader1->SetFileName( inputFilename1 );

  // match the histogram of the second image to the first image
  HistogramMatchingType::Pointer histogramMatching = HistogramMatchingType::New();
  histogramMatching->SetSourceImage(reader1->GetOutput());
  histogramMatching->SetReferenceImage(reader0->GetOutput());
  
  // get intensity range for the input images
  MinMaxType::Pointer minmax0 = MinMaxType::New();
  MinMaxType::Pointer minmax1 = MinMaxType::New();
  minmax0->SetInput(reader0->GetOutput());
  minmax1->SetInput(histogramMatching->GetOutput());
  minmax0->Update();
  minmax1->Update();

  PixelType min0, min1, max0, max1;
  min0 = minmax0->GetMinimum();
  max0 = minmax0->GetMaximum();
  min1 = minmax1->GetMinimum();
  max1 = minmax1->GetMaximum();

  cout << "Image 0 range: " << (int)min0 << ", " << (int)max0 << std::endl;
  cout << "Image 1 range: " << (int)min1 << ", " << (int)max1 << std::endl;

  // set intensity of the first image below threshold0 to be threshold0
  // assume thresholds are defined with respect the the intensities observed
  // in the first image
  ThresholdType::Pointer threshold0min = ThresholdType::New();
  ThresholdType::Pointer threshold0max = ThresholdType::New();
  ThresholdType::Pointer threshold1min = ThresholdType::New();
  ThresholdType::Pointer threshold1max = ThresholdType::New();

  threshold0min->SetInput(reader0->GetOutput());
  threshold0min->SetOutsideValue((PixelType)threshold0);
  threshold0min->ThresholdBelow(threshold0);

  threshold0max->SetInput(threshold0min->GetOutput());
  threshold0max->ThresholdAbove((PixelType)threshold1);
  threshold0max->SetOutsideValue(threshold1);

  threshold1min->SetInput(histogramMatching->GetOutput());
  threshold1min->SetOutsideValue((PixelType)threshold0);
  threshold1min->ThresholdBelow(threshold0<min1?min1:threshold0);

  threshold1max->SetInput(threshold1min->GetOutput());
  threshold1max->SetOutsideValue((PixelType)threshold1);
  threshold1max->ThresholdAbove(threshold1>max1?max1:threshold1);
  
  ImageType::Pointer thImage0 = threshold0max->GetOutput();
  ImageType::Pointer thImage1 = threshold1max->GetOutput();

  // find the difference between the scans, and do median smoothing
  SubtractType::Pointer subtract = SubtractType::New();
  subtract->SetInput1(thImage0);
  subtract->SetInput2(thImage1);
  
  MedianType::Pointer median = MedianType::New();
  ImageType::SizeType radius;
  radius[0] = 1;
  radius[1] = 1;
  radius[2] = 1;
  median->SetInput(subtract->GetOutput());
  median->SetRadius(radius);


  WriterType::Pointer writer0 = WriterType::New();
  WriterType::Pointer writer1 = WriterType::New();

  writer0->SetFileName("tmp0.nrrd");
  writer1->SetFileName("tmp1.nrrd");

  writer0->SetInput(median->GetOutput());
  writer1->SetInput(threshold1max->GetOutput());

  try 
    { 
    writer0->Update(); 
    writer1->Update();
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return EXIT_FAILURE;
    } 

  return EXIT_SUCCESS;
}
