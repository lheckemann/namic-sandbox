#ifndef __itkLevelSetThresholdTerm_h_
#define __itkLevelSetThresholdTerm_h_

#include "itkLevelSetPropagationTerm.h"
#include "itkLinearInterpolateImageFunction.h"

namespace itk {

template <class TImage, class TSpeedImage>
class ITK_EXPORT LevelSetThresholdTerm 
  : public LevelSetPropagationTerm<TImage,TSpeedImage>
{
public:
  typedef LevelSetThresholdTerm Self;
  typedef LevelSetPropagationTerm<TImage,TSpeedImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(LevelSetThresholdTerm, LevelSetPropagationTerm);

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

  typedef typename Superclass::SpeedScalarType SpeedScalarType;

  struct GlobalDataStruct : public Superclass::GlobalDataStruct
  {
  };

  itkRegisterGlobalDataMacro;

  void SetUpperThreshold(SpeedScalarType f)
  { m_UpperThreshold = f; }
  SpeedScalarType GetUpperThreshold() const
  { return m_UpperThreshold; }
  void SetLowerThreshold(SpeedScalarType f)
  { m_LowerThreshold = f; }
  SpeedScalarType GetLowerThreshold() const
  { return m_LowerThreshold; }

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
    Superclass::ComputeUpdate(neighborhood,globalData,offset);

    GlobalDataType *gd = (GlobalDataType *)globalData;

    ScalarValueType upper_threshold = static_cast<ScalarValueType>(m_UpperThreshold);
    ScalarValueType lower_threshold = static_cast<ScalarValueType>(m_LowerThreshold);
    ScalarValueType mid = ( (upper_threshold - lower_threshold) / 2.0 ) + lower_threshold;

    if (gd->m_Speed < mid)
      {
      gd->m_Speed = gd->m_Speed - lower_threshold;
      }
    else
      {
      gd->m_Speed = upper_threshold - gd->m_Speed;
      }

    gd->m_MaxChange = vnl_math_max(gd->m_MaxChange,vnl_math_abs(gd->m_Speed));

    ScalarValueType threshold_term = - this->m_Weight * gd->m_Speed * gd->m_GradientMagnitude;

    return (PixelType)threshold_term;
  }

  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const 
  { 
    return NumericTraits<TimeStepType>::max(); 
  }

protected:
  LevelSetThresholdTerm() 
  { 
  }

  virtual ~LevelSetThresholdTerm() {}

  void PrintSelf(std::ostream &s, Indent indent) const {};

  SpeedScalarType m_UpperThreshold;
  SpeedScalarType m_LowerThreshold;

private:
  LevelSetThresholdTerm(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

}

#endif

