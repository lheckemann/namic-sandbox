#ifndef IMAGEPROCESSOR_H_
#define IMAGEPROCESSOR_H_

#include "itkImage.h"
#include "itkBinaryThresholdImageFilter.h"            // just for testing, TODO: remove when unnecessary
#include "itkLaplacianRecursiveGaussianImageFilter.h" // just for testing, TODO: remove when unnecessary
#include "itkImageFileWriter.h"                        // just for testing, TODO: remove when unnecessary
#include "itkHoughTransform2DLinesImageFilter.h"      // only used for first implementation of HoughFilter TODO: remove when unnecessary
#include "itkGradientMagnitudeImageFilter.h"          // only used for first implementation of HoughFilter TODO: remove when unnecessary
#include "itkThresholdImageFilter.h"                  // only used for first implementation of HoughFilter TODO: remove when unnecessary
#include "itkCannyEdgeDetectionImageFilter.h"          // only used for first implementation of CannyEdgeDetection TODO: remove when unnecessary
#include "itkSobelEdgeDetectionImageFilter.h"
#include "itkCastImageFilter.h"                       // used to cast char/short to float
#include "itkRescaleIntensityImageFilter.h"            // used to cast float back to char/short 
#include "itkSobelOperator.h"
#include "itkNeighborhoodIterator.h"

#define MAX 65335       // Maximum gray scale Intensity for FloatImageType and UShortImageType (=maximum of unsigned short)
#define MAXOUTPUT 600  // Maximum gray scale Intensity for the output image 

template <class InputPixelType>
class InvertIntensityFunctor
{
public: 
  InputPixelType operator()(InputPixelType input )
  {
    return MAX - input;
  }
};

class ImageProcessor  
{
public:
  typedef unsigned char                                                         UCharPixelType;
  typedef unsigned short                                                        UShortPixelType;
  typedef float                                                                 FloatPixelType;
  typedef itk::Image<UCharPixelType, 2>                                         UCharImageType;
  typedef itk::Image<UShortPixelType, 2>                                        UShortImageType;
  typedef itk::Image<FloatPixelType, 2>                                         FloatImageType;
  typedef itk::ImageRegionIterator<UCharImageType>                              UCharIteratorType;
  typedef itk::ImageRegionIterator<UShortImageType>                             UShortIteratorType;
  typedef itk::ImageRegionIterator<FloatImageType>                              FloatIteratorType;
  typedef itk::NeighborhoodIterator<FloatImageType>                             FloatNeighborhoodIteratorType;
  typedef itk::HoughTransform2DLinesImageFilter<FloatPixelType, FloatPixelType> HoughFilter; 
  typedef itk::RescaleIntensityImageFilter<FloatImageType, UCharImageType>      RescaleToUCharFilter;      
  typedef itk::RescaleIntensityImageFilter<FloatImageType, UShortImageType>     RescaleToUShortFilter;    
  typedef itk::RescaleIntensityImageFilter<UCharImageType, FloatImageType>      RescaleUCharToFloatFilter; 
  typedef itk::RescaleIntensityImageFilter<UShortImageType, FloatImageType>     RescaleUShortToFloatFilter;
  typedef itk::UnaryFunctorImageFilter<FloatImageType, FloatImageType,
               InvertIntensityFunctor< FloatImageType::PixelType> >             InverterType;

  ImageProcessor();
  //Conversion Functions
  void changeEndian(unsigned short* data);
  FloatImageType::Pointer  RescaleUCharToFloat(UCharImageType::Pointer inputImage);
  FloatImageType::Pointer  RescaleUShortToFloat(UShortImageType::Pointer inputImage);
  UCharImageType::Pointer  RescaleFloatToUChar(FloatImageType::Pointer inputImage);
  UShortImageType::Pointer RescaleFloatToUShort(FloatImageType::Pointer inputImage);
  //Access Functions
  void SetImage(void* pImage, int xSize, int ySize, int scalarSize, double spacing[3], double origin[3]); 
  void GetImage(void* pImage); 
  void Write(const char* filePath, int whichImage);
  //Filter Functions
  void PassOn() {mLocalOutputImage = mLocalInputImage;};
  void GradientMagnitude(bool inputTmp, bool outputTmp);
  void Threshold(bool inputTmp, bool outputTmp, int outsideValue, int threshBelow, int threshAbove);
  void HoughTransformation(bool inputTmp, bool outputTmp);
  void CannyEdgeDetection(bool inputTmp, bool outputTmp);         // not used anymore        
  void BinaryThreshold(bool inputTmp, bool outputTmp);           // not used anymore
  void LaplacianRecursiveGaussian(bool inputTmp, bool outputTmp);// not used anymore
  void SobelEdgeDetection(bool inputTmp, bool outputTmp);         // not used anymore
  //Neighborhood Functions
  void SobelFilter(bool inputTmp, bool outputTmp, int direction); // my own version of the sobel edge detection filter
  virtual ~ImageProcessor();
  
  private:
  // TODO: I still have to do exception handling for local images -> What happens if inputImage not set? if Filter fails to create an outputImage?
  FloatImageType::Pointer mLocalInputImage;
  FloatImageType::Pointer mLocalTmp1;
  FloatImageType::Pointer mLocalTmp2;
  FloatImageType::Pointer mLocalOutputImage;
  int mScalarSize;  // ScalarSize of the image 1:Char,UChar 2:Short,UShort
  int mWhichTmp;    // variable to decide which tmp buffer is currently in use: 
                    // 0 = none, 1 = mLocalTmp1, 2 = mLocalTmp2 
                    // Attention: Gets only set back to 0 when an image is written to mLocalOutputImage
};


#endif /*IMAGEPROCESSOR_H_*/
