#ifndef __itkLevelSetHessianTerm_h_
#define __itkLevelSetHessianTerm_h_

#include "itkLevelSetTerm.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk {

template <class TImage>
class ITK_EXPORT LevelSetHessianTerm 
  : public LevelSetTerm<TImage>
{
public:
  typedef LevelSetHessianTerm Self;
  typedef LevelSetTerm<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(LevelSetHessianTerm, LevelSetTerm);

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
    /** Hessian matrix */
    vnl_matrix_fixed<ScalarValueType,
                     itkGetStaticConstMacro(ImageDimension),
                     itkGetStaticConstMacro(ImageDimension)> m_dxy;
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
    unsigned int i, j;
    const ScalarValueType center_value  = neighborhood.GetCenterPixel();
    
    const NeighborhoodScalesType neighborhoodScales = this->ComputeNeighborhoodScales();
    
    GlobalDataStruct *gd = (GlobalDataStruct *)globalData;
  
    for( i = 0 ; i < ImageDimension; i++)
      {
      const unsigned int positionA = static_cast<unsigned int>( this->m_Center + this->m_xStride[i]);
      const unsigned int positionB = static_cast<unsigned int>( this->m_Center - this->m_xStride[i]);
  
      gd->m_dxy[i][i] = ( neighborhood.GetPixel( positionA )
                        + neighborhood.GetPixel( positionB ) - 2.0 * center_value )
                        * vnl_math_sqr(neighborhoodScales[i]) ;

      for( j = i+1; j < ImageDimension; j++ )
        {
        const unsigned int positionAa = static_cast<unsigned int>(
          this->m_Center - this->m_xStride[i] - this->m_xStride[j] );
        const unsigned int positionBa = static_cast<unsigned int>(
          this->m_Center - this->m_xStride[i] + this->m_xStride[j] );
        const unsigned int positionCa = static_cast<unsigned int>(
          this->m_Center + this->m_xStride[i] - this->m_xStride[j] );
        const unsigned int positionDa = static_cast<unsigned int>(
          this->m_Center + this->m_xStride[i] + this->m_xStride[j] );
  
        gd->m_dxy[i][j] = gd->m_dxy[j][i] = 0.25 * ( neighborhood.GetPixel( positionAa )
                                                   - neighborhood.GetPixel( positionBa )
                                                   - neighborhood.GetPixel( positionCa )
                                                   + neighborhood.GetPixel( positionDa ) )
                                            * neighborhoodScales[i] * neighborhoodScales[j] ;
        }
      }

    return NumericTraits<PixelType>::Zero;
  }

  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const 
  { 
    return NumericTraits<TimeStepType>::max(); 
  }

protected:
  LevelSetHessianTerm() 
  {
    this->m_Cached = true;
  }

  virtual ~LevelSetHessianTerm() {}

  void PrintSelf(std::ostream &s, Indent indent) const {};

private:
  LevelSetHessianTerm(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

}

#endif

