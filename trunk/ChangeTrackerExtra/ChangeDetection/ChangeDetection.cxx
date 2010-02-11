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
#include "itkImageRegionConstIterator.h"
#include "itkAddImageFilter.h"


#include "itkImage.h"

#define CHANGING_BAND 5
#define SENSITIVITY .99

using namespace std;

typedef int   PixelType;
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
typedef itk::BinaryThresholdImageFilter<DTImageType,ImageType> DTBinaryThresholdType;
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
typedef itk::AddImageFilter<ImageType,ImageType> AddType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIterType;

typedef int HistogramMeasurementType;
typedef itk::Statistics::ScalarImageToListAdaptor<ImageType> AdaptorType;
typedef itk::Statistics::ListSampleToHistogramGenerator<AdaptorType,HistogramMeasurementType> GeneratorType;

void SaveImage(ImageType::Pointer,const char*);
void SaveDTImage(DTImageType::Pointer,const char*);
DTImageType::Pointer DetectChanges(ImageType::Pointer,ImageType::Pointer,ImageType::Pointer,bool);
void CalculateRegionHistograms(ImageType::Pointer diff, std::vector<float> &, std::vector<float> &,  std::vector<float> &, std::vector<float> &);

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

  // calculate masks for stable, growth and shrink regions
  ImageType::Pointer growthMask, shrinkMask, stableMask, diffRegion;
  
  DTType::Pointer dt = DTType::New();
  dt->SetInput(readerS->GetOutput());
  dt->SetSquaredDistance(0);
  dt->SetUseImageSpacing(0);  // the image is isotropic
  dt->Update();

  DTBinaryThresholdType::Pointer thresh = DTBinaryThresholdType::New();
  thresh->SetInput(dt->GetOutput());
  thresh->SetLowerThreshold(-CHANGING_BAND);
  thresh->SetUpperThreshold(CHANGING_BAND);
  thresh->SetInsideValue(1);
  thresh->Update();
  diffRegion = thresh->GetOutput();

  DTBinaryThresholdType::Pointer stableThresh = DTBinaryThresholdType::New();
  stableThresh->SetInput(dt->GetOutput());
  stableThresh->SetUpperThreshold(-CHANGING_BAND*2-0.01);
  stableThresh->SetLowerThreshold(-CHANGING_BAND-100.);
  stableThresh->SetInsideValue(1);
  stableThresh->Update();
  stableMask = stableThresh->GetOutput();

  // smooth the inputs
  MedianType::Pointer tp0med = MedianType::New(), tp1med = MedianType::New();
  ImageType::SizeType medRad;
  medRad.Fill(1);
  tp0med->SetRadius(medRad);
  tp1med->SetRadius(medRad);
  tp0med->SetInput(reader0->GetOutput());
  tp1med->SetInput(reader1->GetOutput());
 
  // find the difference
  SubtractType::Pointer sub = SubtractType::New();
  sub->SetInput1(tp1med->GetOutput());
  sub->SetInput2(tp0med->GetOutput());
  sub->Update();

  // mask the analysis region
  MaskType::Pointer maskerChange = MaskType::New();
  maskerChange->SetInput1(sub->GetOutput());
  maskerChange->SetInput2(diffRegion);
  maskerChange->Update();

  // mask the stable region
  MaskType::Pointer maskerStable = MaskType::New();
  maskerStable->SetInput1(sub->GetOutput());
  maskerStable->SetInput2(stableMask);
  maskerStable->Update();

  ImageType::Pointer diff = maskerChange->GetOutput();
  ImageType::Pointer stable = maskerStable->GetOutput();
  SaveImage(diff,"diff.nrrd");
  SaveImage(stable,"stable.nrrd");
 
  std::vector<float> cumHistPosDiff;
  std::vector<float> cumHistNegDiff;
  std::vector<float> histPosDiff;
  std::vector<float> histNegDiff;

  std::vector<float> cumHistPosStable;
  std::vector<float> cumHistNegStable;
  std::vector<float> histPosStable;
  std::vector<float> histNegStable;

  CalculateRegionHistograms(diff, histPosDiff, cumHistPosDiff, histNegDiff, cumHistNegDiff);
  CalculateRegionHistograms(stable, histPosStable, cumHistPosStable, histNegStable, cumHistNegStable);

  std::cout << "Stable region bounds: -" << histNegStable.size() << " to " << histPosStable.size() << std::endl;
  std::cout << "Changing region bounds: -" << histNegDiff.size() << " to " << histPosDiff.size() << std::endl;

  BinaryThresholdType::Pointer growthThresh = BinaryThresholdType::New();
  BinaryThresholdType::Pointer shrinkThresh = BinaryThresholdType::New();

  float cutoffThresh = histNegStable.size()>histPosStable.size() ? histNegStable.size() : histPosStable.size();
  
  growthThresh->SetInput(diff);
  growthThresh->SetLowerThreshold(cutoffThresh);
  growthThresh->SetUpperThreshold(histPosDiff.size());
  growthThresh->SetInsideValue(14);
  growthThresh->SetOutsideValue(0);

  shrinkThresh->SetInput(diff);
  shrinkThresh->SetUpperThreshold(-cutoffThresh);
  shrinkThresh->SetLowerThreshold(-1.*float(histNegDiff.size()));
  shrinkThresh->SetInsideValue(12);
  shrinkThresh->SetOutsideValue(0);

  AddType::Pointer adder = AddType::New();
  adder->SetInput1(growthThresh->GetOutput());
  adder->SetInput2(shrinkThresh->GetOutput());
  adder->Update();
  
  float negCnt = 0, posCnt = 0;
  ConstIterType rit(adder->GetOutput(),adder->GetOutput()->GetLargestPossibleRegion());
  for(rit.GoToBegin();!rit.IsAtEnd();++rit){
    if(rit.Get()==12)
      negCnt++;
    if(rit.Get()==14)
      posCnt++;
  }
  ImageType::SpacingType spacing = diff->GetSpacing();
  std::cout << "Input image spacing: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;
  std::cout << "AltGrowth: " << posCnt*spacing[0]*spacing[1]*spacing[2] << std::endl;
  std::cout << "AltShrink: " << negCnt*spacing[0]*spacing[1]*spacing[2] << std::endl;

  SaveImage(adder->GetOutput(), "alt_result.nrrd");

  /*
  DetectChanges(reader0->GetOutput(),reader1->GetOutput(),diffRegion,true);

  std::cerr << "After DT" << std::endl;
  // get small number above 0 based on the isotropic spacing of the input (0
  // distance value corresponds to the outer layer of the pixels in the binary
  // input
  DTImageType::SpacingType dtSpacing = dt->GetOutput()->GetSpacing();

  DTBinaryThresholdType::Pointer growthThresh = DTBinaryThresholdType::New();
  growthThresh->SetInput(dt->GetOutput());
  growthThresh->SetLowerThreshold(0.01);
  growthThresh->SetUpperThreshold(CHANGING_BAND);
  growthThresh->SetInsideValue(1);
  growthThresh->Update();
  growthMask = growthThresh->GetOutput();

  DTBinaryThresholdType::Pointer shrinkThresh = DTBinaryThresholdType::New();
  shrinkThresh->SetInput(dt->GetOutput());
  shrinkThresh->SetUpperThreshold(0.01);
  shrinkThresh->SetLowerThreshold(-CHANGING_BAND);
  shrinkThresh->SetInsideValue(1);
  shrinkThresh->Update();
  shrinkMask = shrinkThresh->GetOutput();

  DTBinaryThresholdType::Pointer stableThresh = DTBinaryThresholdType::New();
  stableThresh->SetInput(dt->GetOutput());
  stableThresh->SetUpperThreshold(-CHANGING_BAND-0.01);
  stableThresh->SetLowerThreshold(-CHANGING_BAND-100.);
  stableThresh->SetInsideValue(1);
  stableThresh->Update();
  stableMask = stableThresh->GetOutput();
 
  SaveImage(growthMask,"growthMask.nrrd");
  SaveImage(shrinkMask,"shrinkMask.nrrd");
  SaveImage(stableMask,"stableMask.nrrd");

  
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

/*
template<TImage>
ImageType::Pointer ThresholdBetween(TImage::Pointer image,int low, int high){
  itk::BinaryThresholdImageFilter<TImage,ImageType> ThresholdType;
  ThresholdType::Pointer thresh = ThresholdType::New();
  thresh->SetInput(image);
  thresh->SetLowerThreshold(low);
  thresh->SetUpperThreshold(high);
  thresh->Update();
  return thresh->GetOutput();
}*/

void CalculateRegionHistograms(ImageType::Pointer diff, std::vector<float> &histPos, std::vector<float> &cumHistPos,
  std::vector<float> &histNeg, std::vector<float> &cumHistNeg){
  // calculate the histogram
  MinMaxType::Pointer minmax = MinMaxType::New();
  minmax->SetInput(diff);
  minmax->Update();

  std::cerr << "Min: " << minmax->GetMinimum() << ", Max: " << minmax->GetMaximum() << std::endl;
  cumHistPos.resize(minmax->GetMaximum()+1);
  cumHistNeg.resize(abs(minmax->GetMinimum())+1);
  histPos.resize(minmax->GetMaximum()+1);
  histNeg.resize(abs(minmax->GetMinimum())+1);
//  std::vector<float> cumHistPos(minmax->GetMaximum()+1,0);
//  std::vector<float> cumHistNeg(abs(minmax->GetMinimum())+1,0);
//  std::vector<float> histPos(minmax->GetMaximum()+1,0);
//  std::vector<float> histNeg(abs(minmax->GetMinimum())+1,0);
  float posCnt = 0, negCnt = 0, posCntCum = 0, negCntCum = 0;
  unsigned i;

  ConstIterType sit(diff,diff->GetLargestPossibleRegion());
  for(sit.GoToBegin();!sit.IsAtEnd();++sit){
    float val = sit.Get();
    if(!val)
      continue;
    if(val<0){
      histNeg[abs(val)]++;
      negCnt++;
    }
    if(val>0){
      histPos[val]++;
      posCnt++;
    }
  }

  int posThresh = 0, negThresh = 0;
  for(i=0;i<histPos.size();i++){
    posCntCum += histPos[i];
    cumHistPos[i] = posCntCum/posCnt;
//    std::cout << i << " " << histPos[i] << " " << cumHistPos[i] << std::endl;
    if(cumHistPos[i] >= SENSITIVITY && !posThresh){
      posThresh = i;
    }
  }
  for(i=0;i<histNeg.size();i++){
    negCntCum += histNeg[i];
    cumHistNeg[i]= negCntCum/negCnt;
//    std::cout << i << " " << histNeg[i] << " " << cumHistNeg[i] << std::endl;
    if(cumHistNeg[i] >= SENSITIVITY && !negThresh){
      negThresh = -i;
    }
  }

  /*
  std::cout << "PosThresh: " << posThresh << std::endl << "NegThresh: " << negThresh << std::endl;

  std::ofstream phf("pos_cum_hist.txt");
  std::ofstream nhf("neg_cum_hist.txt");
  for(i=0;i<cumHistPos.size();i++){
    phf << i << " " << cumHistPos[i] << std::endl;
  }
  for(i=0;i<cumHistNeg.size();i++){
    nhf << i << " " << cumHistNeg[i] << std::endl;
  }

  BinaryThresholdType::Pointer growthThresh = BinaryThresholdType::New();
  BinaryThresholdType::Pointer shrinkThresh = BinaryThresholdType::New();
  
  growthThresh->SetInput(diff);
  growthThresh->SetLowerThreshold(posThresh);
  growthThresh->SetUpperThreshold(minmax->GetMaximum());
  growthThresh->SetInsideValue(14);
  growthThresh->SetOutsideValue(0);

  shrinkThresh->SetInput(diff);
  shrinkThresh->SetUpperThreshold(negThresh);
  shrinkThresh->SetLowerThreshold(minmax->GetMinimum());
  shrinkThresh->SetInsideValue(12);
  shrinkThresh->SetOutsideValue(0);

  AddType::Pointer adder = AddType::New();
  adder->SetInput1(growthThresh->GetOutput());
  adder->SetInput2(shrinkThresh->GetOutput());
  adder->Update();
  
  negCnt = 0; posCnt = 0;
  ConstIterType rit(adder->GetOutput(),adder->GetOutput()->GetLargestPossibleRegion());
  for(rit.GoToBegin();!rit.IsAtEnd();++rit){
    if(rit.Get()==12)
      negCnt++;
    if(rit.Get()==14)
      posCnt++;
  }
  ImageType::SpacingType spacing = diff->GetSpacing();
  std::cout << "Input image spacing: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;
  std::cout << "AltGrowth: " << posCnt*spacing[0]*spacing[1]*spacing[2] << std::endl;
  std::cout << "AltShrink: " << negCnt*spacing[0]*spacing[1]*spacing[2] << std::endl;

  SaveImage(adder->GetOutput(), "alt_result.nrrd");

//    std::cout << "Bin " << i << " bounds: " << hist->GetBinMin(0,i) << ":" 
//      << hist->GetBinMax(0,i) << ", freq: " << hist->GetFrequency(i,0) << std::endl;
  */
 
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
  sub->SetInput1(tp1med->GetOutput());
  sub->SetInput2(tp0med->GetOutput());
  sub->Update();

  // mask the analysis region
  MaskType::Pointer masker = MaskType::New();
  masker->SetInput1(sub->GetOutput());
  masker->SetInput2(mask);
  masker->Update();

  ImageType::Pointer diff = masker->GetOutput();
  SaveImage(diff,"diff.nrrd");
 
  // calculate the histogram
  MinMaxType::Pointer minmax = MinMaxType::New();
  minmax->SetInput(diff);
  minmax->Update();

  std::cerr << "Min: " << minmax->GetMinimum() << ", Max: " << minmax->GetMaximum() << std::endl;
  std::vector<float> cumHistPos(minmax->GetMaximum()+1,0);
  std::vector<float> cumHistNeg(abs(minmax->GetMinimum())+1,0);
  std::vector<float> histPos(minmax->GetMaximum()+1,0);
  std::vector<float> histNeg(abs(minmax->GetMinimum())+1,0);

  CalculateRegionHistograms(diff, histPos, cumHistPos, histNeg, cumHistNeg);

  float posCnt = 0, negCnt = 0, posCntCum = 0, negCntCum = 0;
  unsigned i;

  ConstIterType sit(diff,diff->GetLargestPossibleRegion());
  for(sit.GoToBegin();!sit.IsAtEnd();++sit){
    float val = sit.Get();
    if(!val)
      continue;
    if(val<0){
      histNeg[abs(val)]++;
      negCnt++;
    }
    if(val>0){
      histPos[val]++;
      posCnt++;
    }
  }

  int posThresh = 0, negThresh = 0;
  for(i=0;i<histPos.size();i++){
    posCntCum += histPos[i];
    cumHistPos[i] = posCntCum/posCnt;
//    std::cout << i << " " << histPos[i] << " " << cumHistPos[i] << std::endl;
    if(cumHistPos[i] >= SENSITIVITY && !posThresh){
      posThresh = i;
    }
  }
  for(i=0;i<histNeg.size();i++){
    negCntCum += histNeg[i];
    cumHistNeg[i]= negCntCum/negCnt;
//    std::cout << i << " " << histNeg[i] << " " << cumHistNeg[i] << std::endl;
    if(cumHistNeg[i] >= SENSITIVITY && !negThresh){
      negThresh = -i;
    }
  }

  std::cout << "PosThresh: " << posThresh << std::endl << "NegThresh: " << negThresh << std::endl;

  std::ofstream phf("pos_cum_hist.txt");
  std::ofstream nhf("neg_cum_hist.txt");
  for(i=0;i<cumHistPos.size();i++){
    phf << i << " " << cumHistPos[i] << std::endl;
  }
  for(i=0;i<cumHistNeg.size();i++){
    nhf << i << " " << cumHistNeg[i] << std::endl;
  }

  BinaryThresholdType::Pointer growthThresh = BinaryThresholdType::New();
  BinaryThresholdType::Pointer shrinkThresh = BinaryThresholdType::New();
  
  growthThresh->SetInput(diff);
  growthThresh->SetLowerThreshold(posThresh);
  growthThresh->SetUpperThreshold(minmax->GetMaximum());
  growthThresh->SetInsideValue(14);
  growthThresh->SetOutsideValue(0);

  shrinkThresh->SetInput(diff);
  shrinkThresh->SetUpperThreshold(negThresh);
  shrinkThresh->SetLowerThreshold(minmax->GetMinimum());
  shrinkThresh->SetInsideValue(12);
  shrinkThresh->SetOutsideValue(0);

  AddType::Pointer adder = AddType::New();
  adder->SetInput1(growthThresh->GetOutput());
  adder->SetInput2(shrinkThresh->GetOutput());
  adder->Update();
  
  negCnt = 0; posCnt = 0;
  ConstIterType rit(adder->GetOutput(),adder->GetOutput()->GetLargestPossibleRegion());
  for(rit.GoToBegin();!rit.IsAtEnd();++rit){
    if(rit.Get()==12)
      negCnt++;
    if(rit.Get()==14)
      posCnt++;
  }
  ImageType::SpacingType spacing = diff->GetSpacing();
  std::cout << "Input image spacing: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;
  std::cout << "AltGrowth: " << posCnt*spacing[0]*spacing[1]*spacing[2] << std::endl;
  std::cout << "AltShrink: " << negCnt*spacing[0]*spacing[1]*spacing[2] << std::endl;

  SaveImage(adder->GetOutput(), "alt_result.nrrd");

//    std::cout << "Bin " << i << " bounds: " << hist->GetBinMin(0,i) << ":" 
//      << hist->GetBinMax(0,i) << ", freq: " << hist->GetFrequency(i,0) << std::endl;
  
  
  return NULL;
}
