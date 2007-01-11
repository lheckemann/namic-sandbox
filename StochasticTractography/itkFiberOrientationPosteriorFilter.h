#ifndef __itkFiberOrientationPosteriorFilter_h__
#endif __itkFiberOrientationPosteriorFilter_h__

#include "itkImageToImageFilter.h"

namespace itk{
template< class TFiberOrientationPriorImage, class TFiberOrientationLikelihoodImage,
        class TFiberOrientationImage >
class ITK_EXPORT FiberOrientationPosteriorFilter :
  public ImageToImageFilter< TFiberOrientationPriorImage,
                    TFiberOrientationImage >{
public:
  typedef FiberOrientationPosteriorFilter Self;
  typedef ImageToImageFilter< TFiberOrientationPriorImage,
                      TFiberOrientationImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro( FiberOrientationPosteriorFilter, ImageToImageFilter );
  
  typedef TFiberOrientationPriorImage FOPImageType;
  typedef FOPImageType::ConstPointer FOPImageConstPointer;
  typedef TFiberOrientationLikelihoodImage FOLImageType;
  typedef FOLImageType::ConstPointer FOLImageConstPointer;
  typedef TFiberOrientationImage FOImageType;
  typedef FOImageType::Pointer FOImagePointer;
  
  /** Define functions to set/get the multiple input **/
  itkSetInputMacro(FOPImage, FOPImageType, 0);
  itkGetInputMacro(FOPImage, FOPImageType, 0);
  itkSetInputMacro(FOLImage, FOLImageType, 1);
  itkGetInputMacro(FOLImage, FOLImageType, 1);
  
   void GenerateData();
protected:
  FiberOrientationPosterior();
  virtual ~FiberOrientationPosterior();

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFiberOrientationPosteriorFilter.h"
#endif

#endif
