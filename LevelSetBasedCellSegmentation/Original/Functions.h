// All the relevant ITK includes
#include "itkAbsImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkAffineTransform.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkCastImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkFastChamferDistanceImageFilter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkImageToListAdaptor.h"
#include "itkIsoContourDistanceImageFilter.h"
#include "itkListSample.h"
#include "itkMaskNegatedImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkVector.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "Npcf.h"
#include "TwoPCF.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <fstream>
#include "string.h"
using namespace std;

// This is a generic function to perform mathematical operations at a pixel level.
// Input1, Input2 are volumes. Flag = 0 -> Addition, Flag = 1 -> Subtraction, else Multiplication.
template<class InternalImageType> itk::SmartPointer<InternalImageType> AddProcedure
(itk::SmartPointer<InternalImageType> Input1, itk::SmartPointer<InternalImageType> Input2, int flag)
{
  typedef typename itk::AddImageFilter<InternalImageType,InternalImageType,InternalImageType> AddFilterType;
  typedef typename itk::SubtractImageFilter<InternalImageType,InternalImageType,InternalImageType> SubFilterType;
  typedef typename itk::MultiplyImageFilter<InternalImageType,InternalImageType,InternalImageType> MulFilterType;

  typename AddFilterType::Pointer Addfilter = AddFilterType::New();
  typename SubFilterType::Pointer Subfilter = SubFilterType::New();
  typename MulFilterType::Pointer Mulfilter = MulFilterType::New();

  if (flag == 0){
    Addfilter->SetInput(0,Input1);
    Addfilter->SetInput(1,Input2);
    Addfilter->Update();
    return (Addfilter->GetOutput());
  }
  else if(flag==1){
    Subfilter->SetInput(0,Input1);
    Subfilter->SetInput(1,Input2);
    Subfilter->Update();
    return (Subfilter->GetOutput());
  }
  else{
    Mulfilter->SetInput(0,Input1);
    Mulfilter->SetInput(1,Input2);
    Mulfilter->Update();
    return (Mulfilter->GetOutput());
  }
}

// This function determines the voxel locations (or seeds) interior to a cells/nuclei. The level-set is initialized based on these seeds.
// The input volume is a distance-field volume. fg is the cell foreground mask. sample is a list where all the seed locations are stored.
template<class InternalImageType, class SampleType> void ImageSeeds
(itk::SmartPointer<InternalImageType> input, itk::SmartPointer<InternalImageType> fg, itk::SmartPointer<SampleType> sample) {
  
  typedef  typename itk::DiscreteGaussianImageFilter<InternalImageType,InternalImageType> GaussianFilterType;
  typedef typename itk::BinaryBallStructuringElement<typename InternalImageType::PixelType,3> StructuringElementType;
  typedef typename itk::GrayscaleDilateImageFilter<InternalImageType,InternalImageType,StructuringElementType> DilateFilterType;
  typedef typename itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;

  // Smooth the distance field to remove out small islands from being reported as seeds.
  typename GaussianFilterType::Pointer smoother = GaussianFilterType::New();
  smoother->SetInput(input);
  smoother->SetVariance(2.0);
  smoother->SetMaximumKernelWidth(4);
  smoother->Update();

  typename InternalImageType::Pointer smoothImage = smoother->GetOutput();

  // Specify the dilation radius of the structuring mask. It correlated with the minimum separation between the nuclei.
  StructuringElementType structuringElement;
  structuringElement.SetRadius(1);
  structuringElement.CreateStructuringElement();

  // Dilate the input volume
    typename DilateFilterType::Pointer grayscaleDilate = DilateFilterType::New();
  grayscaleDilate->SetKernel(structuringElement);
  grayscaleDilate->SetInput(smoothImage);
  grayscaleDilate->Update();

  typename InternalImageType::Pointer dilateImage = grayscaleDilate->GetOutput();
  typename InternalImageType::IndexType idx;
  typename InternalImageType::PixelType pixelSmooth,pixelDilate,pixelMask;
  // Iterate over the smooth distance field
  IteratorType It(smoothImage,smoothImage->GetRequestedRegion());

  typename SampleType::MeasurementVectorType mv;
  for (It.GoToBegin();!It.IsAtEnd();++It){
    idx = It.GetIndex();
    pixelSmooth = smoothImage->GetPixel(idx);
    pixelDilate = dilateImage->GetPixel(idx);
    pixelMask = fg->GetPixel(idx);

    // If the pixel after dilation has the same value prior to dilation, it is a local maxima!
    // Make sure it lies in the cell foreground.
    if ((pixelSmooth == pixelDilate) && (pixelMask == 0)) {
      typename SampleType::MeasurementVectorType mv;
      mv[0] = idx[0];
      mv[1] = idx[1];
      mv[2] = idx[2];
      sample->PushBack(mv);
    }
  }
}

// This function computes the combined information from distance fields (dist) and the sigmoid of the gradient field (grad).
// Neck regions and high gradients are responsible for splitting the nuclei.
// t is a user-defined parameter that controls how much you depend on gradients or distance-field cues.
template<class InternalImageType> itk::SmartPointer<InternalImageType> SpeedFun
(itk::SmartPointer<InternalImageType> grad, itk::SmartPointer<InternalImageType> dist, float t)
{
  typedef typename itk::RescaleIntensityImageFilter<InternalImageType,InternalImageType> RescaleFilterType;

  // Rescale the images to have a maximum of 255*t
  typename RescaleFilterType::Pointer rescalerA = RescaleFilterType::New();
  rescalerA->SetOutputMinimum(0);
  rescalerA->SetOutputMaximum(255*t);
  rescalerA->SetInput(grad);

  // Rescale to a maximum of 255*(1-t)
  typename RescaleFilterType::Pointer rescalerB = RescaleFilterType::New();
  rescalerB->SetOutputMinimum(0);
  rescalerB->SetOutputMaximum(255*(1-t));
  rescalerB->SetInput(dist);

  // Note that 0 means additions
  typename InternalImageType::Pointer speed = AddProcedure<InternalImageType>(rescalerA->GetOutput(),rescalerB->GetOutput(),0);
  return(speed);
}

// Compute the distance field of the binary volume - mask
// The chamfer filter is very efficient compared to the DanielsonDistanceMapFilter. Specify the maximum distance that needs to be computed. 
// Everything above that value is retained at default maximum value.
// flag - determines if you need to compute the signed/unsigned distance field to a contour.
template<class InternalImageType> itk::SmartPointer<InternalImageType> DistanceMap
(itk::SmartPointer<InternalImageType> mask, int flag)
{
  typedef  typename itk::DiscreteGaussianImageFilter<InternalImageType,InternalImageType> GaussianFilterType;
  typedef typename itk::IsoContourDistanceImageFilter<InternalImageType,InternalImageType> IsoContourFilterType;
  typedef typename itk::FastChamferDistanceImageFilter<InternalImageType,InternalImageType> ChamferFilterType;
  typedef typename itk::AbsImageFilter<InternalImageType,InternalImageType> AbsFilterType;

  // Smooth the binary mask
  typename GaussianFilterType::Pointer smoother = GaussianFilterType::New();
  smoother->SetInput(mask);
  smoother->SetVariance(1.0);
  smoother->SetMaximumKernelWidth(3);
  smoother->Update();

  // An iso value of 128 after smoothing gives you the edge of the foreground
  typename IsoContourFilterType::Pointer isoContour = IsoContourFilterType::New();
  isoContour->SetInput(smoother->GetOutput());
  isoContour->SetLevelSetValue(128);
  isoContour->SetFarValue(50);
  isoContour->NarrowBandingOff();
  isoContour->Update();
   
  typename ChamferFilterType::Pointer chamferDist = ChamferFilterType::New();
  chamferDist->SetInput(isoContour->GetOutput());
  chamferDist->SetMaximumDistance(255);
  chamferDist->Update();

  // If flag is set not set, then run it through an absolute value filter
  if (flag==0){
    typename AbsFilterType::Pointer absFilter = AbsFilterType::New();
    absFilter->SetInput(chamferDist->GetOutput());
    absFilter->Update();
    return (absFilter->GetOutput());
  }else{
    return (chamferDist->GetOutput());
  }
}

template<class InternalImageType> itk::SmartPointer<InternalImageType> GAC
(itk::SmartPointer<InternalImageType> currDist, typename InternalImageType::IndexType seedLoc, const double seedValue,
 const double propagationScaling, const double curvatureScaling, const double advectionScaling,
 const double maxRMSChange, int NumOfCells)
{
  typedef typename itk::GeodesicActiveContourLevelSetImageFilter<InternalImageType,InternalImageType> GeodesicActiveContourFilterType;
  typedef typename itk::BinaryThresholdImageFilter<InternalImageType,InternalImageType> ThresholdingFilterType;
  typedef typename itk::ImageRegionIterator<InternalImageType> IteratorType;
  typedef typename itk::ImageRegionIteratorWithIndex<InternalImageType> IndexIteratorType;
  
  typename InternalImageType::IndexType idx;

  typename InternalImageType::Pointer levelSet = InternalImageType::New();
  levelSet->SetRegions(currDist->GetLargestPossibleRegion().GetSize());
  levelSet->Allocate();

  IndexIteratorType It(levelSet,levelSet->GetLargestPossibleRegion());
  for(It.GoToBegin();!It.IsAtEnd();++It){
    idx = It.GetIndex();
    float pixValue = sqrt((float)(
      (idx[0]-seedLoc[0])*(idx[0]-seedLoc[0]) + 
      (idx[1]-seedLoc[1])*(idx[1]-seedLoc[1]) +
      (idx[2]-seedLoc[2])*(idx[2]-seedLoc[2])
      )) + seedValue;
    It.Set(pixValue);
  }

  typename GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
  geodesicActiveContour->SetPropagationScaling(propagationScaling);
  geodesicActiveContour->SetCurvatureScaling(curvatureScaling);
  geodesicActiveContour->SetAdvectionScaling(advectionScaling);
  geodesicActiveContour->SetMaximumRMSError(maxRMSChange);
  geodesicActiveContour->SetNumberOfIterations(400);
  geodesicActiveContour->SetInput(levelSet);
  geodesicActiveContour->SetFeatureImage(currDist);
  geodesicActiveContour->Update();

  typename ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();              
  thresholder->SetLowerThreshold(-1000.0);
  thresholder->SetUpperThreshold(0.0);
  thresholder->SetOutsideValue(0);
  thresholder->SetInsideValue(NumOfCells+1);
  thresholder->SetInput(geodesicActiveContour->GetOutput());
  thresholder->Update();

  std::cout << std::endl;
  std::cout << "Max. no. iterations: " << geodesicActiveContour->GetNumberOfIterations() << std::endl;
  std::cout << "Max. RMS error: " << geodesicActiveContour->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "No. elapsed iterations: " << geodesicActiveContour->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << geodesicActiveContour->GetRMSChange() << std::endl;

  return(thresholder->GetOutput());
}

// This function performs thresholding. input - volume, thresh_low - lower threshold
// thresh_high - higher threshold, value - set for the exterior region.
// Note that the inside region is always set to 0. Can be changed in the future.
template<class InternalImageType> itk::SmartPointer<InternalImageType> ForegroundExtraction
(itk::SmartPointer<InternalImageType> input_volume, float thresh_low, float thresh_high, float value)
{
  typedef typename itk::BinaryThresholdImageFilter<InternalImageType,InternalImageType> ThresholdingFilterType;

  typename ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
  thresholder->SetLowerThreshold(thresh_low);
  thresholder->SetUpperThreshold(thresh_high);
  thresholder->SetOutsideValue(value);
  thresholder->SetInsideValue(0);
  thresholder->SetInput(input_volume);
  thresholder->Update();

  return (thresholder->GetOutput());
}


// Performs anisotropic diffusion filtering based on image gradients. Input is the volume, iterations and conductance.
template<class InternalImageType> itk::SmartPointer<InternalImageType> ImageGradientSpeed
(itk::SmartPointer<InternalImageType> input,int iter, double cond) {
  typedef typename itk::GradientAnisotropicDiffusionImageFilter<InternalImageType,InternalImageType> SmoothingFilterType;

  typename SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  smoothing->SetInput(input);
  smoothing->SetTimeStep(0.05);
  smoothing->SetNumberOfIterations(iter);
  smoothing->SetConductanceParameter(cond);
  smoothing->Update();

  return(smoothing->GetOutput());
}

// Performs curvature anisotropic diffusion filtering. Input is the volume, iterations and conductance.
template<class InternalImageType> itk::SmartPointer<InternalImageType> ImageCurvatureSpeed
(itk::SmartPointer<InternalImageType> input,int iter, double cond) {

  typedef typename itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType,InternalImageType> SmoothingFilterType;

  typename SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  smoothing->SetInput(input);
  smoothing->SetTimeStep(0.05);
  smoothing->SetNumberOfIterations(iter);
  smoothing->SetConductanceParameter(cond);
  smoothing->Update();

  return(smoothing->GetOutput());
}

//Computes the Pearson's Correlation coefficient. Check Wikipedia for the formula.
template<class InternalImageType> float PearsonCorrelation
(itk::SmartPointer<InternalImageType> input1, itk::SmartPointer<InternalImageType> input2) {

  typedef typename itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;

  int n =0;
  float x=0,y=0,xy=0,x2=0,y2=0;
  IteratorType inputIt1(input1,input1->GetLargestPossibleRegion());
  IteratorType inputIt2(input2,input2->GetLargestPossibleRegion());
  for (inputIt1.GoToBegin(),inputIt2.GoToBegin(); !inputIt1.IsAtEnd();++inputIt1,++inputIt2){
    n++;
    typename InternalImageType::PixelType p1 = inputIt1.Get(), p2 = inputIt2.Get();
    x = x + p1;
    y = y + p2;
    xy = xy + p1*p2;
    x2 = x2 + p1*p1;
    y2 = y2 + p2*p2;
  }
  float c, d = (sqrt(n*x2 - x*x)*sqrt(n*y2 - y*y));
  if (d==0){
    c = 0;
  }else{
    c = ((n*xy)-(x*y))/d;
  }
  return (c*127 + 128);

}

// Initialize the Gaussian blob as a convolution filter. Input is the filter size and the spacing - Anisotropic Gaussian
template<class InternalImageType> itk::SmartPointer<InternalImageType> InitializeBlob
(typename InternalImageType::RegionType::SizeType regionSize, typename InternalImageType::SpacingType spacing) {

  typedef typename itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
  
  const float sigma = 6.0; //You need to find this automatically

  typename InternalImageType::RegionType blobRegion;
  typename InternalImageType::IndexType blobStart;
  typename InternalImageType::IndexType blobOrigin;
  for(unsigned int i=0;i<3;i++){
    blobOrigin[i] = (regionSize[i]-1)/2;
    blobStart[i] = 0;
  }
  blobRegion.SetSize(regionSize);
  blobRegion.SetIndex(blobStart);

  typename InternalImageType::Pointer blob = InternalImageType::New();
  blob->SetRegions(blobRegion);
  blob->Allocate();
  blob->SetSpacing(spacing);

  IteratorType blobIt(blob,blob->GetLargestPossibleRegion());
  for (blobIt.GoToBegin(); !blobIt.IsAtEnd();++blobIt){
    typename InternalImageType::IndexType idx = blobIt.GetIndex();
    float dx[3];
    for(unsigned int i=0;i<3;i++){
      dx[i] = (idx[i]-blobOrigin[i])*spacing[i];
    }
    float p = exp(-(pow(dx[0],2)+ pow(dx[1],2) + pow(dx[2],2))/(2*pow(sigma,2)));
    blobIt.Set(p);
  }
  return(blob);
}

// Computes the Gaussian correlation of the input volume. Implemented as a adaptive Gaussian. Not an entriely clean concept. But works.
template<class InternalImageType> itk::SmartPointer<InternalImageType> GaussCorrelation
(itk::SmartPointer<InternalImageType> input) {
  
  typedef typename itk::RegionOfInterestImageFilter<InternalImageType,InternalImageType > ROIFilterType;
  typedef typename itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
  
  typename InternalImageType::RegionType::SizeType regionSize;
  regionSize[0] = 9;  regionSize[1] = 9;  regionSize[2] = 5;

  typename InternalImageType::SizeType inputSize = input->GetLargestPossibleRegion().GetSize();

  // Define a sub-image region to run the windowing on
  typename InternalImageType::RegionType::IndexType start;
  typename InternalImageType::RegionType::SizeType size;
  for(unsigned int i=0;i<3;i++){
    start[i] = (regionSize[i]-1)/2;  
    size[i] = inputSize[i] - regionSize[i] + 1;
  }

  typename InternalImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);

  typename InternalImageType::Pointer output = InternalImageType::New();
  output->SetRegions(input->GetLargestPossibleRegion());
  output->SetSpacing(input->GetSpacing());
  output->SetOrigin(input->GetOrigin());
  output->Allocate();

  //Generate window regions
  typename ROIFilterType::Pointer ROIfilter = ROIFilterType::New();
  ROIfilter->SetInput(input);

  //Set window region size and start locations 
  typename InternalImageType::RegionType windowRegion;
  windowRegion.SetSize(regionSize);

  typename InternalImageType::SpacingType spacing = input->GetSpacing();
  // Initialize Gaussian blob
  typename InternalImageType::Pointer blob = InitializeBlob<InternalImageType>(regionSize,spacing);

  IteratorType inputIt(input,region);

  int count = 0;
  for (inputIt.GoToBegin(); !inputIt.IsAtEnd();++inputIt){
    if (count%1000000 == 0)
      std::cerr << count << std::endl;
    count++;
    typename InternalImageType::IndexType idx = inputIt.GetIndex();
    typename InternalImageType::IndexType windowStart;
    for(unsigned int i=0;i<3;i++){
      windowStart[i] = idx[i]-(regionSize[i]-1)/2;
    }
    windowRegion.SetIndex(windowStart);

    ROIfilter->SetRegionOfInterest(windowRegion);
    ROIfilter->Update();

    typename InternalImageType::Pointer windowImage = ROIfilter->GetOutput();  

    float c = PearsonCorrelation<InternalImageType>(blob,windowImage);
    typename InternalImageType::PixelType pixel = c;
    output->SetPixel(idx,pixel);
  }
  return(output);
}

// Perform upsampling in the z-direction of a volume to always make the voxel ratio 1:1:1. Can be generalized in the future
// based on upsampling ratios provided by the user.
// Uses B-Spline Interpolation.
template<class InternalImageType> itk::SmartPointer<InternalImageType> Resample
(itk::SmartPointer<InternalImageType> input) {
  
  typedef typename itk::ResampleImageFilter<InternalImageType,InternalImageType> ResampleVolumeFilter;
  typedef typename itk::BSplineInterpolateImageFunction<InternalImageType> Interpolator;

  typename InternalImageType::SizeType size = input->GetLargestPossibleRegion().GetSize();
  typename InternalImageType::SpacingType spacing = input->GetSpacing();
  typename InternalImageType::PointType origin = input->GetOrigin();

  // get new size, spacing and origin of input image
  typename InternalImageType::SpacingType nspacing;
  typename InternalImageType::SizeType nsize;
  for(unsigned int i=0;i<3;i++){
    nspacing[i] = 1;
    nsize[i] =  size[i]*(unsigned int)spacing[i];
    std::cerr << nsize[i] << std::endl;
  }

    // create the resample filter, transform and interpolator
    typename itk::AffineTransform<double,3>::Pointer transform = itk::AffineTransform<double,3>::New();
    transform->SetIdentity();
  
  typename Interpolator::Pointer interp = Interpolator::New();
  interp->SetSplineOrder(1);

  typename ResampleVolumeFilter::Pointer resamp_filter = ResampleVolumeFilter::New();
    resamp_filter->SetTransform(transform);
    resamp_filter->SetInterpolator(interp);
    resamp_filter->SetInput(input);
    resamp_filter->SetSize(nsize);
    resamp_filter->SetOutputOrigin(origin);
    resamp_filter->SetOutputSpacing(nspacing);
    resamp_filter->SetDefaultPixelValue(0); //doesn't matter
    resamp_filter->Update();

  return(resamp_filter->GetOutput());

}

// Read in a 3D volume as a series of 2D images. Need to specify the first file number, last file number and the file format (../%d.png)
template<class InternalImageType> itk::SmartPointer<InternalImageType> ReadIn
(const unsigned int first_file, const unsigned int last_file, char *input_file_name) {
  
  typedef typename itk::NumericSeriesFileNames NameGeneratorType;
  typedef typename itk::ImageSeriesReader<InternalImageType> ImageSeriesReaderType;

  typename NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  nameGenerator->SetSeriesFormat(input_file_name);
  nameGenerator->SetStartIndex(first_file);
  nameGenerator->SetEndIndex(last_file);
  nameGenerator->SetIncrementIndex(1);

  // set the file names
  typename ImageSeriesReaderType::Pointer reader = ImageSeriesReaderType::New();
  reader->SetFileNames(nameGenerator->GetFileNames());   
  reader->Update();

  return(reader->GetOutput());
}

// Write out a 3D volume as a series of 2D images. Need to specify the first file number, last file number and the file format (../%d.png)
template<class IOImageType> void WriteOut
(itk::SmartPointer<IOImageType> input, const unsigned int first_file, 
 const unsigned int last_file, char *output_file_name) {
  
  typedef typename itk::Image<unsigned char,2> WriteImageType;
  typedef typename itk::NumericSeriesFileNames NameGeneratorType;
  typedef typename itk::ImageSeriesWriter<IOImageType, WriteImageType> SeriesWriterType;

  typename NameGeneratorType::Pointer nameGeneratorOutput = NameGeneratorType::New();
    nameGeneratorOutput->SetSeriesFormat(output_file_name);
    nameGeneratorOutput->SetStartIndex(first_file);
    nameGeneratorOutput->SetEndIndex(last_file);
  nameGeneratorOutput->SetIncrementIndex(1);

    // set the file names for output 1
  typename SeriesWriterType::Pointer series_writer = SeriesWriterType::New();
  series_writer->SetInput(input);
    series_writer->SetFileNames(nameGeneratorOutput->GetFileNames());   
    series_writer->Update();
}


Npcf::Npcf(int initLength, int initRegionSize, int initSampleSize, int NumOfMat){
length = initLength;
regionSize = initRegionSize;
sampleSize = initSampleSize;
numOfMat = NumOfMat;
}

TwoPCF::TwoPCF(int initRow, int initColumn, int initSlice, int NumOfMat){
  row = initRow;
  column = initColumn;
  slice = initSlice;
  
  eval = new float *[NumOfMat];
  for(int i=0;i<NumOfMat;i++){
    eval[i]=new float [NumOfMat];
    memset(eval[i],0,NumOfMat*sizeof(float));
  }
}

template<class InternalImageType,class TwoPCF, class Npcf> void TwoPointCorrelationEvaluation
(itk::SmartPointer<InternalImageType> inputImage, float n[3], TwoPCF _TwoPCF, Npcf _Npcf)
{
  typename InternalImageType::IndexType p1,p2;
  int m1,m2;

  float theta,phi;
  srand (time(NULL));

  int RegionSize = _Npcf.getRegionSize();
  int SampleSize = _Npcf.getSampleSize();
  int Length = _Npcf.getLength();

  for (int i=0;i<SampleSize;i++) {
    do {
      p1[0] = (long int)rand()%RegionSize;
      p1[1] = (long int)rand()%RegionSize;
      p1[2] = (long int)rand()%RegionSize;

      theta = ((float)rand()/RAND_MAX)*2*3.1415;
      phi = ((float)rand()/RAND_MAX)*3.1415 - 3.1415/2;

      p2[0] = (long int)floor(p1[0] + Length*cos(phi)*cos(theta));
      p2[1] = (long int)floor(p1[1] + Length*cos(phi)*sin(theta));
      p2[2] = (long int)floor(p1[2] + Length*sin(phi));
    } while (!((p2[0]>0)&&(p2[0]<RegionSize)&&(p2[1]>0)&&(p2[1]<RegionSize)&&(p2[2]>0)&&(p2[2]<RegionSize)));

    m1 = (int)inputImage->GetPixel(p1);
    m2 = (int)inputImage->GetPixel(p2);
    _TwoPCF.incrEval(m1,m2);
  }
}


void EulerTransformation(float alpha, float beta, float gamma, float T[3][3]){
  T[0][0] = cos(alpha)*cos(beta)*cos(gamma) - sin(alpha)*sin(gamma);
  T[0][1] = sin(alpha)*cos(beta)*cos(gamma) + cos(alpha)*sin(gamma);
  T[0][2] = - sin(beta)*cos(gamma);

  T[1][0] = -cos(alpha)*cos(beta)*sin(gamma) - sin(alpha)*cos(gamma);
  T[1][1] = -sin(alpha)*cos(beta)*sin(gamma) + cos(alpha)*cos(gamma);
  T[1][2] = sin(beta)*sin(gamma);

  T[2][0] = cos(alpha)*sin(beta);
  T[2][1] = sin(alpha)*sin(beta);
  T[2][2] = cos(beta);
}
