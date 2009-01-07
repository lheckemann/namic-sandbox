#ifndef __itkLevelSetDerivativesTerm_h_
#define __itkLevelSetDerivativesTerm_h_

#include "itkLevelSetTerm.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk {

template <class TImage>
class ITK_EXPORT LevelSetDerivativesTerm 
  : public LevelSetTerm<TImage>
{
public:
  typedef LevelSetDerivativesTerm Self;
  typedef LevelSetTerm<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(LevelSetDerivativesTerm, LevelSetTerm);

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

  struct GlobalDataStruct : public Superclass::GlobalDataStruct
  {
    ScalarValueType m_dx[itkGetStaticConstMacro(ImageDimension)];

    ScalarValueType m_dx_forward[itkGetStaticConstMacro(ImageDimension)];
    ScalarValueType m_dx_backward[itkGetStaticConstMacro(ImageDimension)];

    ScalarValueType m_GradMagSqr;
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
    unsigned int i;
    const ScalarValueType center_value  = neighborhood.GetCenterPixel();
    
    const NeighborhoodScalesType neighborhoodScales = this->ComputeNeighborhoodScales();
    
    GlobalDataStruct *gd = (GlobalDataStruct *)globalData;
  
    gd->m_GradMagSqr = 1.0e-6;
    for( i = 0 ; i < ImageDimension; i++)
      {
      const unsigned int positionA = static_cast<unsigned int>( this->m_Center + this->m_xStride[i]);
      const unsigned int positionB = static_cast<unsigned int>( this->m_Center - this->m_xStride[i]);
  
      gd->m_dx[i] = 0.5 * (neighborhood.GetPixel( positionA ) -
                           neighborhood.GetPixel( positionB ) ) * neighborhoodScales[i];
  
      gd->m_dx_forward[i]  = ( neighborhood.GetPixel( positionA ) - center_value ) * neighborhoodScales[i];
      gd->m_dx_backward[i] = ( center_value - neighborhood.GetPixel( positionB ) ) * neighborhoodScales[i];

      gd->m_GradMagSqr += gd->m_dx[i] * gd->m_dx[i];
      }

    return NumericTraits<PixelType>::Zero;
  }

  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const 
  { 
    return NumericTraits<TimeStepType>::max(); 
  }

protected:
  LevelSetDerivativesTerm() {}

  virtual ~LevelSetDerivativesTerm() {}

  void PrintSelf(std::ostream &s, Indent indent) const {};

private:
  LevelSetDerivativesTerm(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

}

#endif

