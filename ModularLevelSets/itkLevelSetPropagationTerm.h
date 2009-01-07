#ifndef __itkLevelSetPropagationTerm_h_
#define __itkLevelSetPropagationTerm_h_

#include "itkLevelSetTerm.h"
#include "itkLevelSetDerivativesTerm.h"
#include "itkLinearInterpolateImageFunction.h"

namespace itk {

template <class TImage, class TSpeedImage>
class ITK_EXPORT LevelSetPropagationTerm 
  : public LevelSetTerm<TImage>
{
public:
  typedef LevelSetPropagationTerm Self;
  typedef LevelSetTerm<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(LevelSetPropagationTerm, LevelSetTerm);

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

  typedef typename ImageType::IndexType IndexType;
  typedef TSpeedImage SpeedImageType;
  typedef typename SpeedImageType::PixelType SpeedScalarType;

  typedef LinearInterpolateImageFunction<SpeedImageType> InterpolatorType;
  typedef typename InterpolatorType::ContinuousIndexType ContinuousIndexType;

  typedef typename LevelSetDerivativesTerm<ImageType>::GlobalDataType DerivativesGlobalDataType;

  struct GlobalDataStruct : public Superclass::GlobalDataStruct
  {
    GlobalDataStruct()
      {
      m_Speed = NumericTraits<ScalarValueType>::Zero;
      m_GradientMagnitude = NumericTraits<ScalarValueType>::Zero;
      }
    ScalarValueType m_Speed;
    ScalarValueType m_GradientMagnitude;
  };

  itkRegisterGlobalDataMacro;

  virtual const SpeedImageType *GetSpeedImage() const
  { return m_SpeedImage.GetPointer(); }
  virtual void SetSpeedImage(const SpeedImageType *f)
  { m_SpeedImage = f; }

  virtual void Initialize() 
  {
    Superclass::Initialize();
    m_Interpolator->SetInputImage(m_SpeedImage);
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
    if (dgd == NULL)
      {
      itkExceptionMacro("LevelSetDerivativesTerm is a dependency but its global data is not available.");
      }

    ScalarValueType speed = ZERO;

    IndexType idx = neighborhood.GetIndex();

    ContinuousIndexType cdx;
    for (unsigned i = 0; i < ImageDimension; ++i)
      {
      cdx[i] = static_cast<double>(idx[i]) - offset[i];
      }

    if (m_Interpolator->IsInsideBuffer(cdx))
      {
      speed = static_cast<ScalarValueType>(m_Interpolator->EvaluateAtContinuousIndex(cdx));
      }
    else 
      {
      speed = static_cast<ScalarValueType>(m_SpeedImage->GetPixel(idx));
      }

    gd->m_Speed = speed;

    gd->m_MaxChange = vnl_math_max(gd->m_MaxChange,vnl_math_abs(speed));

    ScalarValueType gradient_magnitude = ZERO;
   
    if (speed > ZERO)
      {
      for (unsigned int i = 0; i< ImageDimension; i++)
        {
        gradient_magnitude += vnl_math_sqr( vnl_math_max(dgd->m_dx_backward[i], ZERO) )
                            + vnl_math_sqr( vnl_math_min(dgd->m_dx_forward[i],  ZERO) );
        }
      }
    else
      {
      for (unsigned int i = 0; i< ImageDimension; i++)
        {
        gradient_magnitude += vnl_math_sqr( vnl_math_min(dgd->m_dx_backward[i], ZERO) )
                            + vnl_math_sqr( vnl_math_max(dgd->m_dx_forward[i],  ZERO) );
        }
      }
    gradient_magnitude = vcl_sqrt(gradient_magnitude);

    gd->m_GradientMagnitude = gradient_magnitude;

    ScalarValueType propagation_term = - this->m_Weight * speed * gradient_magnitude;

    return (PixelType)propagation_term;
  }

  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const 
  { 
    return NumericTraits<TimeStepType>::max(); 
  }

protected:
  LevelSetPropagationTerm() 
  { 
    this->AddDependency(LevelSetDerivativesTerm<ImageType>::New()); 
    m_Interpolator = InterpolatorType::New();
  }

  virtual ~LevelSetPropagationTerm() {}

  void PrintSelf(std::ostream &s, Indent indent) const {};

  typename SpeedImageType::ConstPointer m_SpeedImage;
  typename InterpolatorType::Pointer m_Interpolator;

private:
  LevelSetPropagationTerm(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

}

#endif

