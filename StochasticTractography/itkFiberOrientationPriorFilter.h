#ifndef __itkFiberOrientationPriorFilter_h__
#define __itkFiberOrientationPriorFilter_h__
#include "itkImageSource.h"

namespace itk{
template< class TFiberOrientationPriorImage >
class ITK_EXPORT FiberOrientationPriorFilter :
  public ImageSource< TFiberOrientationPriorImage >{
public:
  typedef FiberOrientationPriorImageFilter Self;
  typedef ImageSource< TFiberOrientationPriorImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro( FiberOrientationPriorFilter, ImageSource );
  
  typedef TFiberOrientationPriorImage FOPImageType;
  typedef FOPImageType::Pointer FOPImagePointer;
  
  typedef vnl_vector_fixed< double, 3 > FiberOrientationType;
  typedef VectorContainer< unsigned int, FiberDirectionType > FOContainerType;
  typedef FOContainer::Pointer FOContainerPointer;
  
  /** Set/Get the list of directions to sample **/
  itkSetObjectMacro( sampledirections, FiberOrientationContainerType );
  itkGetObjectMacro( sampledirections, FiberOrientationContainerType );
  
  /**Set/Get the prior orientation direction **/
  itkSetMacro( prevOrientation, FiberOrientationType );
  itkGetMacro( prevOrientation, FiberOrientationType ); 
  
  void GenerateData();
protected:
  FiberOrientation();
  virtual ~FiberOrientation();
  
  FiberOrientationType m_prevOrientation;
  FOContainerPointer m_sampledirections;
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFiberOrientationPriorFilter.txx"
#endif

#endif
