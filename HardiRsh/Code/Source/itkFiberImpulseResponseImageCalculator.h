
#ifndef __itkFiberImpulseResponseImageCalculator_h
#define __itkFiberImpulseResponseImageCalculator_h

#include "itkObject.h"
#include "itkObjectFactory.h"

#include "itkVectorImage.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkVectorContainer.h"
#include "itkSpatialObject.h"

#include "itkRealSymSphericalHarmonicBasis.h"
#include "itkSymRealSphericalHarmonicRep.h"
#include "itkDiffusionTensor3D.h"


namespace itk
{

/** \class FiberImpulseResponceImageCalculator
 * This calculator computes the fiber impulse response function from
 * a Dti image 
 *
 * \ingroup Operators
 */
template
<
  class TGradientImagePixelType,
  typename TBasisType=itk::RealSymSphericalHarmonicBasis< 4 >
>
class ITK_EXPORT FiberImpulseResponseImageCalculator : public Object 
{
public:
  /** Standard class typedefs. */
  typedef FiberImpulseResponseImageCalculator Self;
  typedef Object                        Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FiberImpulseResponseImageCalculator, Object);


  typedef TGradientImagePixelType                     GradientPixelType;
  typedef VectorImage< GradientPixelType, 3 >         GradientImagesType;
  typedef typename GradientImagesType::ConstPointer   GradImageConstPointer;
  typedef typename GradientImagesType::PixelType      GradientVectorType;

  
  /** Type definition for the input image index type. */
  typedef typename GradientImagesType::IndexType    IndexType;
  
  /** Type definition for the input image region type. */
  typedef typename GradientImagesType::RegionType   RegionType;
  
  /** Holds each magnetic field gradient used to acquire one DWImage */
  typedef vnl_vector_fixed< double, 3 >             GradientDirectionType;

  /** Container to hold gradient directions of the 'n' DW measurements */
  typedef VectorContainer< unsigned int,
          GradientDirectionType >                   GradientDirectionContainerType;

  typedef GradientDirectionContainerType::ConstPointer
                                                    GradientDirectionContainerPointer;
  
  /**  Type for the mask of the fixed image.
   *    Only pixels that are "inside"
   *    this mask will be considered for the computation of the metric
   **/
  typedef SpatialObject< 3 >                        ImageMaskType;
  typedef typename ImageMaskType::ConstPointer      ImageMaskPointer;

  /** Basis Typedef. */
  typedef TBasisType                                BasisType;
          
  typedef SymRealSphericalHarmonicRep< double, BasisType::MaxOrder >
                                                    RshPixelType;
  
  /** Tensor typedefs. */  
  typedef itk::DiffusionTensor3D<double>                DtiType;

  typedef vnl_vector<double>                        VectorType;
  typedef vnl_matrix< double >                      MatrixType;

  void Compute(void);


  /** Set/Get the image mask. */
  itkSetConstObjectMacro( ImageMask, ImageMaskType );

  void SetGradientImage( GradientDirectionContainerType *,
                                             const GradientImagesType *image);
  
  itkSetMacro(BValue,double);
  itkGetConstMacro(BValue,double);
  
  itkGetConstMacro(RespRSH,VectorType);
  
protected:
  FiberImpulseResponseImageCalculator();
  virtual ~FiberImpulseResponseImageCalculator() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  FiberImpulseResponseImageCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MatrixType InitDtiSolver( GradientDirectionContainerPointer );
  MatrixType InitRshSolver( GradientDirectionContainerPointer );
  
  GradientDirectionContainerPointer     RotateGradientContainer( MatrixType ) const;

  GradImageConstPointer                                 m_GradientImage;

  /** container to hold gradient directions */
  GradientDirectionContainerType::ConstPointer          m_GradientDirectionContainer;
  
  vnl_vector<double>                                    m_RespRSH;
  
  /** Image Mask */
  mutable ImageMaskPointer                              m_ImageMask;

  std::vector<unsigned int>                             m_Gradientind;
  std::vector<unsigned int>                             m_Baselineind;
  unsigned int                                          m_NumGradients;
  unsigned int                                          m_NumBaselines;
  
  double                                                m_BValue;
  
};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFiberImpulseResponseImageCalculator.txx"
#endif

#endif /* __itkFiberImpulseResponseImageCalculator_h */
