
#ifndef __itkRSHDistanceMapImageFilter_h
#define __itkRSHDistanceMapImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkSmartPointer.h"
#include "itkExtractImageFilterRegionCopier.h"
#include "itkNumericTraits.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

template <
class TInputImage,
class TOutputComponentType = float
>
class ITK_EXPORT RSHDistanceMapImageFilter:
    public ImageToImageFilter
    <TInputImage, Image< TOutputComponentType, TInputImage::ImageDimension> >
{
public:
  typedef Image< TOutputComponentType, TInputImage::ImageDimension>
                                                            TOutputImage;

  /** Standard class typedefs. */
  typedef RSHDistanceMapImageFilter                         Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>      Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Image typedefs. */
  typedef typename Superclass::OutputImageType              OutputImageType;
  typedef typename Superclass::InputImageType               InputImageType;
  typedef typename OutputImageType::RegionType              OutputImageRegionType;
  typedef typename InputImageType::PixelType                InputPixelType;
  typedef typename OutputImageType::PixelType               OutputPixelType;
  typedef typename InputImageType::ConstPointer             InputImageConstPointer;
  typedef typename OutputImageType::Pointer                 OutputImagePointer;

  /** point and index typedefs */
  typedef typename InputImageType::PointType                InputImagePointType;
  typedef typename InputImageType::IndexType                InputImageIndexType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(RSHDistanceMapImageFilter, ImageToImageFilter);

  //ENUM to decide with metric we want to use
  typedef enum
    {
    FullL2Metric = 1,
    RotationInvarientMetric
    } MetricTypeEnumeration;

  itkSetMacro( Metric, MetricTypeEnumeration );
  itkGetMacro( Metric, MetricTypeEnumeration );

  //itkSetMacro( SeedIndex, InputImageIndexType );
  virtual void SetSeedIndex (const InputImageIndexType _arg)
  {
    itkDebugMacro("setting SeedIndex to " << _arg);
    if (this->m_SeedIndex != _arg)
      {
      this->m_SeedIndex = _arg;
      this->m_IndexSet = true;
      this->Modified();
      }
  } 

  
  itkGetMacro( SeedIndex, InputImageIndexType );

protected:
  RSHDistanceMapImageFilter();
  ~RSHDistanceMapImageFilter() {};

  void BeforeThreadedGenerateData( );
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

private:
  RSHDistanceMapImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  //Which metric should we use
  MetricTypeEnumeration                                           m_Metric;
  InputImageIndexType                                             m_SeedIndex;
  bool                                                            m_IndexSet;
  
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRSHDistanceMapImageFilter.txx"
#endif

#endif
