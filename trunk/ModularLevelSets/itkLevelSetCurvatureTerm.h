#ifndef __itkLevelSetCurvatureTerm_h_
#define __itkLevelSetCurvatureTerm_h_

#include "itkLevelSetTerm.h"
#include "itkLevelSetDerivativesTerm.h"
#include "itkLevelSetHessianTerm.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk {

template <class TImage>
class ITK_EXPORT LevelSetCurvatureTerm 
  : public LevelSetTerm<TImage>
{
public:
  typedef LevelSetCurvatureTerm Self;
  typedef LevelSetTerm<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(LevelSetCurvatureTerm, LevelSetTerm);

  itkStaticConstMacro(ImageDimension, unsigned int, Superclass::ImageDimension);
  
  typedef double TimeStepType;
  typedef typename Superclass::ImageType  ImageType;
  typedef typename Superclass::PixelType  PixelType;
  typedef                      PixelType  ScalarValueType;
  typedef typename Superclass::PixelRealType  PixelRealType;
  typedef typename Superclass::RadiusType RadiusType;
  typedef typename Superclass::NeighborhoodType NeighborhoodType;
  typedef typename Superclass::NeighborhoodScalesType NeighborhoodScalesType;
  typedef typename Superclass::FloatOffsetType FloatOffsetType;

  typedef typename LevelSetDerivativesTerm<ImageType>::GlobalDataType DerivativesGlobalDataType;
  typedef typename LevelSetHessianTerm<ImageType>::GlobalDataType HessianGlobalDataType;

  struct GlobalDataStruct : public Superclass::GlobalDataStruct
  {
  };

  itkRegisterGlobalDataMacro;

  virtual void Initialize() 
  {
    Superclass::Initialize();
  }

  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                  void *globalData,
                                  const FloatOffsetType& offset) 
  { 
    const ScalarValueType ZERO = NumericTraits<ScalarValueType>::Zero;

    if (this->m_Weight == ZERO)
      {
      return (PixelType)ZERO;
      }

    GlobalDataType *gd = (GlobalDataType *)globalData;

    DerivativesGlobalDataType *dgd = (DerivativesGlobalDataType *)gd->m_DependencyGlobalDataMap["LevelSetDerivativesTerm"];
    HessianGlobalDataType *hgd = (HessianGlobalDataType *)gd->m_DependencyGlobalDataMap["LevelSetHessianTerm"];

    if (dgd == NULL)
      {
      itkExceptionMacro("LevelSetDerivativesTerm is a dependency but its global data is not available.");
      }
 
    if (hgd == NULL)
      {
      itkExceptionMacro("LevelSetHessianTerm is a dependency but its global data is not available.");
      }
 
    ScalarValueType curvature_term = ZERO;
    unsigned int i, j;
  
    for (i = 0; i < ImageDimension; i++)
      {
      for(j = 0; j < ImageDimension; j++)
        {
        if(j != i)
          {
          curvature_term -= dgd->m_dx[i] * dgd->m_dx[j] * hgd->m_dxy[i][j];
          curvature_term += hgd->m_dxy[j][j] * dgd->m_dx[i] * dgd->m_dx[i];
          }
        }
      }

    curvature_term /= dgd->m_GradMagSqr;
  
    curvature_term *= this->m_Weight;

    gd->m_MaxChange = vnl_math_max(gd->m_MaxChange,vnl_math_abs(curvature_term));

    return (PixelType)curvature_term;
  }

  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const 
  { 
    return NumericTraits<TimeStepType>::max(); 
  }

protected:
  LevelSetCurvatureTerm() 
  { 
    this->AddDependency(LevelSetDerivativesTerm<ImageType>::New()); 
    this->AddDependency(LevelSetHessianTerm<ImageType>::New()); 
  }

  virtual ~LevelSetCurvatureTerm() {}

  void PrintSelf(std::ostream &s, Indent indent) const {};

private:
  LevelSetCurvatureTerm(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

}

#endif

