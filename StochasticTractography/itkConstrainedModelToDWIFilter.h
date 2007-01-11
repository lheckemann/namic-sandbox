#ifndef __itkConstrainedModelToDWIFilter_h__
#define __itkConstrainedModelToDWIFilter_h__

#include "vnl/vnl_vector_fixed.h"

namespace itk{
template< class TConstrainedParamImage,
        class TDWIVectorImage >
class ITK_EXPORT ConstrainedModelToDWIFilter :
  public ImageToImageFilter< TConstrainedParamImage,
                    TDWIVectorImage >{
public:
  typedef ConstrainedModelToDWIFilter Self;
  typedef ImageToImageFilter< TConstrainedParamImage,
                      TDWIVectorImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro(ConstrainedModelToDWIFilter, ImageToImageFilter);
  
  /** Convenient typedefs **/
  typedef TDWIVectorImage DWIVectorImageType;
  typedef DWIVectorImageType::ConstPointer DWIVectorImageConstPointer;
  typedef TConstrainedParamImage ConstrainedParamImageType;
  typedef ConstrainedParamImageType::Pointer ConstrainedParamImagePointer;
  
  typedef vnl_vector_fixed< double, 3 > GradientDirectionType;
  typedef VectorContainer< unsigned int, GradientDirectionType > GradientDirectionContainerType;
  typedef GradientDirectionContainerType::Pointer  GradientDirectionContainerPointer;
  typedef double bValueType;
  typedef VectorContainer< unsigned int, bValueType > bValueContainerType;
  typedef bValueContainerType::Pointer bValueContainerPointer; 

  void Updategradients(GradientDirectionContainerPointer gradients){
    this->Setgradients(gradients);
    UpdateCoefficientMatrix();
  }
  
  void Updatebvalues(bValueContainerPointer bvalues){
    this->Setbvalues(bvalues);
    UpdateCoefficientMatrix();
  }
  
protected:
  ConstrainedModelToDWIFilter();
  virtual ~ConstrainedModelToDWIFilter() {};

  /** Set/Get of gradient directions **/
  itkSetObjectMacro( gradients, GradientDirectionContainerType );
  itkGetObjectMacro( gradients, GradientDirectionContainerType );
  
  /** Set/Get bvalues **/
  itkSetObjectMacro( bvalues, bValueContainerType );
  itkGetObjectMacro( bvalues, bValueContainerType );
  
  /**Set/Get the number of gradient directions **/
  itkSetMacro( N, unsigned int );
  itkGetMacro( N, unsigned int );
  
  /** Updates the Coefficient Matrix (A) **/
  void UpdateCoefficientMatrix();
  
  bValueContainerPointer m_bvalues;
  GradientDirectionContainerPointer m_gradients;
  
  /** These are members of the class to avoid
    recalculating them everytime we call GenerateData **/
  vnl_matrix< double > A;
  unsigned int m_N;
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorModelFilter.txx"
#endif

#endif
