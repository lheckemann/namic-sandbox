#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkListSampleToHistogramGenerator.h"
#include "itkScalarImageToListAdaptor.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkSubtractImageFilter.h"


#include "itkImage.h"

#define CHANGING_BAND 5

using namespace std;

typedef int      PixelType;
typedef float DTPixelType;
const   unsigned int      Dimension = 3;
typedef itk::Image< PixelType, Dimension >    ImageType;
typedef itk::Image< DTPixelType, Dimension >   DTImageType;

typedef itk::ImageFileReader< ImageType >  ReaderType;
typedef itk::ImageFileWriter< ImageType >  WriterType;
typedef itk::ImageFileWriter< DTImageType >  DTWriterType;
typedef itk::HistogramMatchingImageFilter<ImageType,ImageType> HistogramMatchingType;
typedef itk::ThresholdImageFilter<ImageType> ThresholdType;
typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> BinaryThresholdType;
typedef itk::BinaryThresholdImageFilter<DTImageType,DTImageType> DTBinaryThresholdType;
typedef itk::CastImageFilter<DTImageType,ImageType> CastType;
typedef itk::MinimumMaximumImageFilter<ImageType> MinMaxType;
typedef itk::SubtractImageFilter<ImageType,ImageType> SubtractType;
typedef itk::MedianImageFilter<ImageType,ImageType> MedianType;
typedef itk::MaskImageFilter<ImageType,ImageType,ImageType> MaskType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIterType;
typedef itk::ConnectedComponentImageFilter<ImageType,ImageType> ConnectedCompType;
typedef itk::RelabelComponentImageFilter<ImageType,ImageType> RelabelType;
typedef itk::SignedMaurerDistanceMapImageFilter<ImageType,DTImageType> DTType;
typedef itk::MedianImageFilter<ImageType,ImageType> MedianType;
typedef itk::SubtractImageFilter<ImageType,ImageType> SubtractType;

typedef int HistogramMeasurementType;
typedef itk::Statistics::ScalarImageToListAdaptor<ImageType> AdaptorType;
typedef itk::Statistics::ListSampleToHistogramGenerator<AdaptorType,HistogramMeasurementType> GeneratorType;

void SaveImage(ImageType::Pointer,const char*);
void SaveDTImage(DTImageType::Pointer,const char*);
DTImageType::Pointer DetectChanges(ImageType::Pointer,ImageType::Pointer,ImageType::Pointer,bool);

int main( int argc, char ** argv )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile1  inputImageFile2 segmentationImage outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  ReaderType::Pointer reader0 = ReaderType::New();
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer readerS = ReaderType::New();

  const char * inputFilename0  = argv[1];
  const char * inputFilename1 = argv[2];
  const char * segmFilename = argv[3];
  const char * outFilename = argv[4];

  reader0->SetFileName( inputFilename0 );
  reader1->SetFileName( inputFilename1 );
  readerS->SetFileName( segmFilename );

  // match the histogram of the second image to the first image
  HistogramMatchingType::Pointer histogramMatching = HistogramMatchingType::New();
  histogramMatching->SetSourceImage(reader1->GetOutput());
  histogramMatching->SetReferenceImage(reader0->GetOutput());

  MaskType::Pointer masker0 = MaskType::New();
  masker0->SetInput1(reader0->GetOutput());
  masker0->SetInput2(readerS->GetOutput());
  masker0->Update();
  
  // get intensity range for the input images within the segmented region
  ConstIterType image0It(masker0->GetOutput(), masker0->GetOutput()->GetLargestPossibleRegion());
  ConstIterType segmIt(readerS->GetOutput(), readerS->GetOutput()->GetLargestPossibleRegion());
  image0It.GoToBegin();
  segmIt.GoToBegin();
  PixelType min = 300, max = -300;
  
  for(;!image0It.IsAtEnd();++image0It,++segmIt){
    if(!segmIt.Get())
      continue;
    min = fmin(image0It.Get(),min);
    max = fmax(image0It.Get(),max);
  }

  MinMaxType::Pointer minmax0 = MinMaxType::New();
  minmax0->SetInput(masker0->GetOutput());
  minmax0->Update();
  std::cerr << "Thresholds are: " << min << ", " << max << std::endl;

  // create the mask for the second normalized image
  ThresholdType::Pointer thresh = ThresholdType::New();
  thresh->SetInput(histogramMatching->GetOutput());
  thresh->ThresholdOutside(min, max);
  thresh->Update();

  BinaryThresholdType::Pointer bthresh = BinaryThresholdType::New();
  bthresh->SetInput(thresh->GetOutput());
  bthresh->SetLowerThreshold(1);
  bthresh->SetUpperThreshold(max);

  ConnectedCompType::Pointer connected = ConnectedCompType::New();
  RelabelType::Pointer relabeler = RelabelType::New();
  connected->SetInput(bthresh->GetOutput());
  connected->SetFullyConnected(1);
  relabeler->SetInput(connected->GetOutput());
  relabeler->Update();

  BinaryThresholdType::Pointer relabelThresh = BinaryThresholdType::New();
  relabelThresh->SetInput(relabeler->GetOutput());
  relabelThresh->SetLowerThreshold(1);
  relabelThresh->SetUpperThreshold(1);
  relabelThresh->Update();

  MaskType::Pointer masker2 = MaskType::New();
  masker2->SetInput1(thresh->GetOutput());
  masker2->SetInput2(relabelThresh->GetOutput());
  masker2->Update();

  ImageType::Pointer image0ready, image1ready;
  image0ready = masker0->GetOutput();
  image1ready = masker2->GetOutput();

  std::cerr << "Before DT" << std::endl;

  DTType::Pointer dt = DTType::New();
  dt->SetInput(readerS->GetOutput());
  dt->SetSquaredDistance(0);
  dt->SetUseImageSpacing(1);
  dt->Update();

  std::cerr << "After DT" << std::endl;
  // get small number above 0 based on the isotropic spacing of the input (0
  // distance value corresponds to the outer layer of the pixels in the binary
  // input
  DTImageType::SpacingType dtSpacing = dt->GetOutput()->GetSpacing();

  DTBinaryThresholdType::Pointer growthThresh = DTBinaryThresholdType::New();
  growthThresh->SetInput(dt->GetOutput());
  growthThresh->SetLowerThreshold(dtSpacing[0]/3.);
  growthThresh->SetUpperThreshold(CHANGING_BAND);
  growthThresh->SetInsideValue(1);
  growthThresh->Update();


  DTBinaryThresholdType::Pointer shrinkThresh = DTBinaryThresholdType::New();
  shrinkThresh->SetInput(dt->GetOutput());
  shrinkThresh->SetUpperThreshold(0);
  shrinkThresh->SetLowerThreshold(-CHANGING_BAND);
  shrinkThresh->SetInsideValue(1);
  shrinkThresh->Update();

  CastType::Pointer cast0 = CastType::New(), cast1 = CastType::New();
  cast0->SetInput(growthThresh->GetOutput());
  cast1->SetInput(shrinkThresh->GetOutput());
  cast0->Update();
  cast1->Update();

  ImageType::Pointer shrinkMask = cast1->GetOutput();
  ImageType::Pointer growthMask = cast0->GetOutput();

  SaveDTImage(dt->GetOutput(), "dt.nrrd");
  SaveImage(cast0->GetOutput(), "image0.nrrd");
  SaveImage(cast1->GetOutput(), "image1.nrrd");

  // outer band, tumor corresponds to higher intensity, so growth will
  // correspond to negative values of the difference image
  DetectChanges(reader0->GetOutput(), reader1->GetOutput(), cast0->GetOutput(), 1);

  // inner band, tumor corresponds to higher intensity, so shrinkage will
  // correspond to positive values of the difference image
  DetectChanges(reader0->GetOutput(), reader1->GetOutput(), cast0->GetOutput(), 1);
  std::cout << "Images saved" << std::endl;

  return 0;

  /*

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

  */
  return EXIT_SUCCESS;
}

void SaveImage(ImageType::Pointer image,const char* name){
  WriterType::Pointer w = WriterType::New();
  w->SetFileName(name);
  w->SetInput(image);
  w->Update();
}

void SaveDTImage(DTImageType::Pointer image,const char* name){
  DTWriterType::Pointer w = DTWriterType::New();
  w->SetFileName(name);
  w->SetInput(image);
  w->Update();
}

// Take two images, the mask where the changes are expected, and flag whether
// changes are expected in the negative (1) or positive (0) side. 
// Return the image where intensity is between 0 and 1, corresponding to the
// likelihood of a pixel being the expected change.
DTImageType::Pointer DetectChanges(ImageType::Pointer tp0image, ImageType::Pointer tp1image, 
  ImageType::Pointer mask, bool positiveChange){

  // smooth the inputs
  MedianType::Pointer tp0med = MedianType::New(), tp1med = MedianType::New();
  ImageType::SizeType medRad;
  medRad.Fill(1);
  tp0med->SetRadius(medRad);
  tp1med->SetRadius(medRad);
  tp0med->SetInput(tp0image);
  tp1med->SetInput(tp1image);
  
  // find the difference
  SubtractType::Pointer sub = SubtractType::New();
  sub->SetInput1(tp0med->GetOutput());
  sub->SetInput2(tp1med->GetOutput());
  sub->Update();

  // calculate the histogram
  MinMaxType::Pointer minmax = MinMaxType::New();
  minmax->SetInput(sub->GetOutput());
  minmax->Update();
  std::cerr << "Min: " << minmax->GetMinimum() << ", Max: " << minmax->GetMaximum() << std::endl;

  typedef GeneratorType::HistogramType HistogramType;
  HistogramType::SizeType size;
  GeneratorType::Pointer generator = GeneratorType::New();
  AdaptorType::Pointer adaptor = AdaptorType::New();
  adaptor->SetImage(sub->GetOutput());
  
  
  generator->SetListSample(adaptor);
  size.Fill(minmax->GetMaximum()-minmax->GetMinimum());
  generator->SetNumberOfBins(size);
  generator->SetMarginalScale(1.);
  generator->Update();

  HistogramType::ConstPointer hist = generator->GetOutput();

  for(unsigned i=0;i<hist->Size();i++){
    std::cout << "Bin " << i << " bounds: " << hist->GetBinMin(0,i) << ":" 
      << hist->GetBinMax(0,i) << ", freq: " << hist->GetFrequency(i,0) << std::endl;
  }
  
  return NULL;
}
