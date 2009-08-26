#ifndef itkFindeCenterOfBrainFilter_txx
#define itkFindeCenterOfBrainFilter_txx
#include <itkImageToImageFilter.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>
#include "itkLargestForegroundFilledMaskImageFilter.h"

namespace itk
{
template <class TInputImage>
class ITK_EXPORT FindCenterOfBrainFilter :
public ImageToImageFilter<TInputImage,TInputImage>
{
public:
  typedef FindCenterOfBrainFilter  Self;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  itkNewMacro(Self);
  itkTypeMacro(FindCenterOfBrain,LightObject);

  typedef TInputImage                      ImageType;
  typedef typename ImageType::Pointer      InputImagePointer;
  typedef typename ImageType::PixelType    PixelType;
  typedef typename ImageType::PointType    PointType;
  typedef typename ImageType::SizeType     SizeType;
  typedef typename ImageType::SpacingType  SpacingType;
  typedef typename ImageType::IndexType    IndexType;
  typedef typename itk::ImageRegionIteratorWithIndex<ImageType>
                                           ImageIteratorType;
  typedef typename itk::ImageRegionConstIteratorWithIndex<ImageType>
                                           ImageConstIteratorType;
  typedef LargestForegroundFilledMaskImageFilter<ImageType>
                                           LFFMaskFilterType;
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension );
  
  itkSetMacro(Maximize,bool);
  itkGetMacro(Maximize,bool);
  itkSetMacro(Axis,unsigned int);
  itkGetMacro(Axis,unsigned int);
  itkSetMacro(OtsuPercentileThreshold,double);
  itkGetMacro(OtsuPercentileThreshold,double);
  itkSetMacro(ClosingSize,unsigned int);
  itkGetMacro(ClosingSize,unsigned int);
  itkSetMacro(HeadSizeLimit,double);
  itkGetMacro(HeadSizeLimit,double);
  itkGetMacro(CenterOfBrain,PointType);
protected:
  FindCenterOfBrainFilter();
  ~FindCenterOfBrainFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;
  void AllocateOutputs();
  virtual void GenerateData();
private:  
  bool m_Maximize;
  unsigned int m_Axis;
  double m_OtsuPercentileThreshold;
  unsigned int m_ClosingSize;
  double m_HeadSizeLimit;
  PixelType m_BackgroundValue;
  PointType m_CenterOfBrain;
};

}

#if ITK_TEMPLATE_TXX
# include "itkFindCenterOfBrainFilter.txx"
#endif

#endif // itkFindeCenterOfBrainFilter_txx
