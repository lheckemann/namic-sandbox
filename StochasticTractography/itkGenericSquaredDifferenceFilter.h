#ifndef __itkTensorModelFilter_h__
#define __itkTensorModelFilter_h__

#include "itkVectorContainer.h"

namespace itk{
//convention: first line is input class types,
//second line is output class types
template< class TDWIVectorImage,
        class TTensorModelParamImage >
class ITK_EXPORT TensorModelFilter :
  public ImageToImageFilter< TDWIVectorImage,
                    TTensorModelParamImage >{
public:
  /** Standard class typedefs. */
  typedef TensorModelFilter Self;
  typedef ImageToImageFilter< TDWIVectorImage,
                    TTensorImage > Superclass;
  typedef SmartPointer< Self >  Pointer;
  typedef SmartPointer< const Self >  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** For Type Information **/
  itkTypeMacro(TensorModelFilter, ImageToImageFilter );
  
  /** Some convenient typedefs **/
  typedef TDWIVectorImage DWIVectorImageType;
  typedef DWIVectorImageType::PixelType DWIVectorImagePixelType;
  typedef DWIVectorImageType::ConstPointer DWIVectorImageConstPointer;
  typedef TTensorModelParamImage TensorModelParamImageType;
  typedef TensorModelParamImage::Pointer TensorModelParamPointer;

  typedef vnl_vector_fixed< double, 3 > GradientDirectionType;
  typedef VectorContainer< unsigned int, GradientDirectionType > GradientDirectionContainerType;
  typedef GradientDirectionContainerType::Pointer  GradientDirectionContainerPointer;
  typedef double bValueType;
  typedef VectorContainer< unsigned int, bValueType > bValueContainerType;
  typedef bValueContainerType::Pointer bValueContainerPointer; 
  
  /** Set/Get of gradient directions **/
  itkSetObjectMacro( gradients, GradientDirectionContainerType );
  itkGetObjectMacro( gradients, GradientDirectionContainerType );
  
  /** Set/Get bvalues **/
  itkSetObjectMacro( bvalues, bValueContainerType );
  itkGetObjectMacro( bvalues, bValueContainerType);
  
protected:
  TensorModelFilter();
  virtual ~TensorModelFilter() {};
  
  //be sure to use itk's smart pointers with its factory constructed objects!
  //a segmentation fault will occur if you don't
  bValueContainerPointer m_bvalues;
  GradientDirectionContainerPointer m_gradients;
  
  void GenerateData ( void );

};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorModelFilter.txx"
#endif

#endif
