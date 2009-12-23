
#ifndef __itkSymRSHPowerImageFilter_h
#define __itkSymRSHPowerImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkSmartPointer.h"
#include "itkExtractImageFilterRegionCopier.h"
#include "itkNumericTraits.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

/** \class SymRSHPowerImageFilter
 * \brief
 *
 */
template <
class TInputImage,
class TOutputComponentType = float
>
class ITK_EXPORT SymRSHPowerImageFilter:
    public ImageToImageFilter
    <
      TInputImage,
      Image
        < Vector<TOutputComponentType, TInputImage::PixelType::NumberOfOrders >,
        TInputImage::ImageDimension>
    >
{
public:
  typedef Image
          < Vector<TOutputComponentType, TInputImage::PixelType::NumberOfOrders >,
              TInputImage::ImageDimension
            >                                               TOutputImage;

  /** Standard class typedefs. */
  typedef SymRSHPowerImageFilter                            Self;
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

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SymRSHPowerImageFilter, ImageToImageFilter);

  /** Flag for whether or not we want normalized odfs
   */
  itkSetMacro( Normalize, bool );
  itkBooleanMacro( Normalize );

protected:
  SymRSHPowerImageFilter();
  ~SymRSHPowerImageFilter() {};

  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

private:
  SymRSHPowerImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  //Should the power vector be normalized?
  bool        m_Normalize;
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSymRSHPowerImageFilter.txx"
#endif

#endif
