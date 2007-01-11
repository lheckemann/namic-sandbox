#ifndef __itkTensorModelToConstrainedModelFilter_h__
#define __itkTensorModelToConstrainedModelFilter_h__


namespace itk{
template< class TTensorParamImage,
        class TConstrainedParamImage>
class ITK_EXPORT TensorModelToTensorModelToConstrainedModelFilter :
  public ImageToImageFilter< TTensorParamImage,
                    TConstrainedParamImage >{
public:
  typedef TensorModelToConstrainedModelFilter Self;
  typedef ImageToImageFilter< TTensorParamImage,
                      TConstrainedParamImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  /** Convenient typedefs **/
  typedef TTensorParamImage TensorParamImageType;
  typedef TensorParamImageType::ConstPointer TensorParamImageConstPointer;
  typedef TConstrainedParamImage ConstrainedParamImageType;
  typedef ConstrainedParamImageType::Pointer ConstrainedParamImagePointer;

  itkNewMacro(Self);
  itkTypeMacro(TensorModelToConstrainedModelFilter, ImageToImageFilter);
  
protected:
  TensorModelToConstrainedModelFilter(){};
  virtual ~TensorModelToConstrainedModelFilter() {};
  
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorModelFilter.txx"
#endif

#endif
  
