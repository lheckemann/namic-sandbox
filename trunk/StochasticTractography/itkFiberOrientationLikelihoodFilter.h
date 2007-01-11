#ifndef __itkFiberOrientationLikelihoodFilter_h__
#define __itkFiberOrientationLikelihoodFilter_h__
#include "itkImageToImageFilter.h"
#include "itkConstrainedModelToDWIFilter.h"
#include "itkReplaceEntryVectorImageFilter.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk{
template< class TDWIVectorImage, class TResidualVarianceImage, class TConstrainedParamImage,
      class TFiberOrientationLikelihoodImage >
class ITK_EXPORT FiberOrientationLikelihoodFilter :
  public ImageToImageFilter< TDWIVectorImage,
                    TFiberOrientationLikelihoodImage >{
public:
  typedef FiberOrientationLikelihoodFilter Self;
  typedef ImageToImageFilter< TDWIVectorImage,
                      TFiberOrientationLikelihoodImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro( FiberOrientationLikelihoodFilter, ImageToImageFilter );
  
  typedef TConstrainedParamImage CPImageType;
  typedef CPImageType::ConstPointer CPImageConstPointer;
  typedef TDWIVectorImage DWIVImageType;
  typedef DWIVImageType::ConstPointer DWIVImageConstPointer;
  typedef TResidualVarianceImageType RVImageType;
  typedef RVImageType::ConstPointer RVImageConstPointer;
  typedef TFiberOrientationLikelihoodImage FOLmageType;
  typedef FOLImage::Pointer FOLImagePointer;
  
  typedef vnl_vector_fixed< double, 3 > FiberOrientationType;
  typedef VectorContainer< unsigned int, FiberDirectionType > FOContainerType;
  typedef FOContainer::Pointer FOContainerPointer;
  
  /**Define functions to set/get the multiple inputs **/
  itkSetInputMacro(ConstrainedParam, CPImageType, 0);
  itkGetInputMacro(ConstrainedParam, CPImageType, 0);
  itkSetInputMacro(ResidualVariance, RVImageType, 1);
  itkGetInputMacro(ResidualVariance, RVImageType, 1);
  itkSetInputMacro(ActualDWI, DWIVImageType, 2);
  itkGetInputMacro(ActualDWI, DWIVImageType, 2);
  
  /** Set/Get the list of directions to sample **/
  itkSetObjectMacro( sampledirections, FiberOrientationContainerType );
  itkGetObjectMacro( sampledirections, FiberOrientationContainerType );
  
  void GenerateData();
protected:
  /** Filter implements a minipipeline as well as performs some calculations **/
  typedef ConstrainedModelToDWIFilter< ConstrainedParamImageType, DWIImageType >
    CMTDWIFilterType;
  typedef CMTDWIFilterType::Pointer CMTDWIFilterPointer;
  typedef ReplaceEntryVectorImageFilter< ConstrainedParamImageType, ConstrainedParamImageType >
    REVIFilterType;
  typedef REVIFilterType::Pointer REVIFilterPointer;
  
  FiberOrientationLikelihoodFilter();
  virtual ~FiberOrientationLikelihoodFilter();
  
  REVIFilterPointer m_reviFilter;
  CMTDWIFilterPointer m_cmtdwiFilter;
  FOContainerPointer m_sampledirections;
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFiberOrientationLikelihoodFilter.txx"
#endif

#endif
