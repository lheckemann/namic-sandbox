#ifndef __itkLargestForegroundFilledMaskImageFilter_h
#define __itkLargestForegroundFilledMaskImageFilter_h

#include <itkImage.h>
#include <itkImageToImageFilter.h>
#include <itkNumericTraits.h>

namespace itk
{

/**
 * \author Hans J. Johnson
 *
 * This filter does a good job of finding a single largest connected
 * mask that separates the foreground object from the background.
 * It assumes that the corner voxels of the image belong to the backgound.
 * This filter was written for the purpose of finding the tissue
 * region of a brain image with no internal holes.
 *
 * The OtsuPercentileThreshold is used to define the range of values
 * where the percentage of voxels falls beetween
 * (0+OtsuPercentileThreshold) < "Intensities of Interest" < (1-OtsuPercentileThreshold).
 *
 * The ClosingSize specifies how many mm to dilate followed by
 * erode to fill holes that be present in the image.
 *
 * The image that is returned will be a binary image with foreground and background
 * values specified by the user (defaults to 1 and 0 respectively).
 *
 */
template <class TInputImage,class TOutputImage = TInputImage>
class ITK_EXPORT LargestForegroundFilledMaskImageFilter :
  public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Convenient typedefs for simplifying declarations. */
  typedef TInputImage                                           InputImageType;
  typedef typename InputImageType::ConstPointer                 InputImagePointer;
  typedef typename InputImageType::RegionType                   InputImageRegionType; 
  typedef typename InputImageType::PixelType                    InputPixelType;

  typedef TOutputImage                                          OutputImageType;
  typedef typename OutputImageType::Pointer                     OutputImagePointer;
  typedef typename OutputImageType::RegionType                  OutputImageRegionType;
  typedef typename OutputImageType::PixelType                   OutputPixelType;

  typedef LargestForegroundFilledMaskImageFilter                Self;
  typedef ImageToImageFilter< InputImageType, OutputImageType>  Superclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef Image<unsigned short,OutputImageType::ImageDimension> IntegerImageType;
  typedef typename IntegerImageType::PixelType                  IntegerPixelType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LargestForegroundFilledMaskImageFilter, ImageToImageFilter);

  /** set Otsu Threshold */
  itkSetMacro(OtsuPercentileThreshold,double);
  itkGetConstMacro(OtsuPercentileThreshold,double);
  itkSetMacro(ClosingSize,unsigned int);
  itkGetConstMacro(ClosingSize,unsigned int);
  itkSetMacro(InsideValue,IntegerPixelType);
  itkGetMacro(InsideValue,IntegerPixelType);
  itkSetMacro(OutsideValue,IntegerPixelType);
  itkGetMacro(OutsideValue,IntegerPixelType);
  itkSetMacro(ThresholdCorrectionFactor,double);
  itkGetConstMacro(ThresholdCorrectionFactor,double);

protected:
  LargestForegroundFilledMaskImageFilter();
  ~LargestForegroundFilledMaskImageFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual void GenerateData();
private:
  InputPixelType SetLowHigh(InputPixelType &low,
                            InputPixelType &high);
  void ImageMinMax(InputPixelType &min,InputPixelType &max);
  double m_OtsuPercentileThreshold;
  double m_ThresholdCorrectionFactor;
  unsigned int m_ClosingSize;
  IntegerPixelType m_InsideValue;
  IntegerPixelType m_OutsideValue;
};

} // end namespace itk

#if ITK_TEMPLATE_TXX
# include "itkLargestForegroundFilledMaskImageFilter.txx"
#endif

#endif

