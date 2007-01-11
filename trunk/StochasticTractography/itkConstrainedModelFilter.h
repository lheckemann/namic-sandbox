#ifndef __itkConstrainedModelFilter_h__
#define __itkConstrainedModelFilter_h__

#include "vnl/vnl_vector_fixed.h"
#include "itkTensorModelFilter.h"
#include "itkTensorModelToConstrainedModelFilter.h"

namespace itk{
template< class TDWIVectorImage,
        class TConstrainedParamImage>
class ITK_EXPORT ConstrainedModelFilter :
  public ImageToImageFilter< TDWIVectorImage,
                    TConstrainedParamImage >{
public:
  typedef ConstrainedModelFilter Self;
  typedef ImageToImageFilter< TDWIVectorImage,
                      TConstrainedParamImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro(ConstrainedModelFilter, ImageToImageFilter);
  
  /** Convenient typedefs **/
  typedef TDWIVectorImage DWIVectorImageType;
  typedef TConstrainedParamImage ConstrainedParamImageType;
  typedef Image< double, 3 > ResidualImageType;
  typedef Image< double, 3 > EstimatedImageType;

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
  /** Image Typedefs for internal filters **/
  typedef VectorImage< double, 3 > TensorModelParamImageType;
  
  /** Filters used in this composite filter **/
  typedef TensorModelFilter< DWIVectorImageType, TensorModelParamImageType > 
    TensorModelFilterType;
  typedef TensorModelFilterType::Pointer TensorModelFilterPointer;
  typedef TensorModelToConstrainedModelFilter< TensorModelParamImageType, 
                                ConstrainedParamImageType >
    TensorModelToConstrainedModelFilterType;
  typedef TensorModelToConstrainedModelFilter::Pointer
    TensorModelToConstrainedModelFilterPointer;
      
  ConstrainedModelFilter();
  virtual ~ConstrainedModelFilter() {};
  
  TensorModelFilterPointer m_tensormodelfilter;
  TensorModelToConstrainedModelFilterPointer m_tensormodeltoconstrainedmodelfilter;
  
  bValueContainerPointer m_bvalues;
  GradientDirectionContainerPointer m_gradients;
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorModelFilter.txx"
#endif

#endif
  
