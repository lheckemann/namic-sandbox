#ifndef __itkSymRshReconImageFilter_h_
#define __itkSymRshReconImageFilter_h_

#include "itkImageToImageFilter.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_svd.h"
#include "itkVectorContainer.h"
#include "itkVectorImage.h"
#include "itkSpatialObject.h"

#include "itkSymRealSphericalHarmonicRep.h"

namespace itk{

template< class TGradientImagePixelType,
          class TOutputPixelType = SymRealSphericalHarmonicRep< float, 4 >,
          unsigned int TImageDimension=3,
          class TResidualPercisionType = float
          >
class ITK_EXPORT SymRshReconImageFilter :
  public ImageToImageFilter< Image< TGradientImagePixelType, TImageDimension >,
                             Image< TOutputPixelType, TImageDimension > >
{

public:

  typedef SymRshReconImageFilter                  Self;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

  typedef ImageToImageFilter< Image< TGradientImagePixelType, TImageDimension >,
                              Image< TOutputPixelType, TImageDimension > >
                                                  Superclass;

   /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(SymRshReconImageFilter, ImageToImageFilter);

  /** Typedefs for Pixel and Image Types. */
  typedef TGradientImagePixelType                   GradientPixelType;

  typedef typename Superclass::OutputImageType      OutputImageType;
  typedef typename OutputImageType::PixelType       OutputPixelType;

  typedef VectorImage< TResidualPercisionType, 3 >  ResidualImageType;
  typedef typename ResidualImageType::PixelType     ResidualPixelType;
  
  itkStaticConstMacro(NumberOfCoefficients,unsigned int, OutputPixelType::Dimension);
  itkStaticConstMacro(NumberOfOrders,unsigned int, OutputPixelType::NumberOfOrders);
  itkStaticConstMacro(MaxOrder,unsigned int, OutputPixelType::MaxOrder);

  /** Reference image data,  This image is aquired in the absence
   * of a diffusion sensitizing field gradient */
  typedef typename Superclass::InputImageType      ReferenceImageType;

  typedef typename Superclass::OutputImageRegionType
                                                   OutputImageRegionType;

  /** An alternative typedef defining one (of the many) gradient images.
   * It will be assumed that the vectorImage has the same dimension as the
   * Reference image and a vector length parameter of \c n (number of
   * gradient directions)*/
  typedef VectorImage< GradientPixelType, 3 >      GradientImagesType;

  typedef vnl_matrix< double >                     RshBasisMatrixType;
  typedef vnl_matrix< double >                     RshBasisMatrixInverseType;
  typedef std::vector< RshBasisMatrixInverseType > PartialRshMatrixInverseType;
  typedef std::vector< RshBasisMatrixType >        PartialRshMatrixType;


  /** Holds each magnetic field gradient used to acquire one DWImage */
  typedef vnl_vector_fixed< double, 3 >            GradientDirectionType;

  /** Container to hold gradient directions of the 'n' DW measurements */
  typedef VectorContainer< unsigned int,
          GradientDirectionType >                  GradientDirectionContainerType;

  /**  Type for the mask of the fixed image.
   *    Only pixels that are "inside"
   *    this mask will be considered for the computation of the metric
   **/
  typedef SpatialObject< TImageDimension >          ImageMaskType;
  typedef typename ImageMaskType::Pointer           ImageMaskPointer;

  /** Set/Get the image mask. */
  itkSetObjectMacro( ImageMask, ImageMaskType );
  itkGetConstObjectMacro( ImageMask, ImageMaskType );

  /**
   * The image here is a VectorImage. The user is expected to pass the
   * gradient directions in a container. The ith element of the container
   * corresponds to the gradient direction of the ith component image the
   * VectorImage.  For the baseline image, a vector of all zeros
   * should be set.*/
  void SetGradientImage( GradientDirectionContainerType *,
                                             const GradientImagesType *image);

  itkGetConstObjectMacro( GradientDirectionContainer ,GradientDirectionContainerType);
  
  /** Get/set comput rsiduals flag. */
  itkSetMacro( CalculateResidualImage, bool );
  itkGetConstMacro( CalculateResidualImage, bool );
  itkBooleanMacro( CalculateResidualImage );

  /** Get the Residual image . */
  itkGetConstObjectMacro( ResidualImage, ResidualImageType );

  /** Get/set lambda. */
  itkSetMacro( BeltramiLambda, double );
  itkGetMacro( BeltramiLambda, double );

protected:
  SymRshReconImageFilter();
  ~SymRshReconImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual void ComputeRshBasis();
  virtual vnl_vector< double > ComputeCoeffsFromSignal( vnl_vector< double >, ResidualPixelType&  );

  void BeforeThreadedGenerateData();
  void ThreadedGenerateData( const
      OutputImageRegionType &outputRegionForThread, int);

  /* Real Spherical harmonic basis coeffs */
  RshBasisMatrixType                                m_RshBasis;
  RshBasisMatrixInverseType                         m_RshBasisPseudoInverse;

  /** Get/set AllowSuperResolution.
   * Only sub classes should be able to set this!
   **/
  itkSetMacro( AllowSuperResolution, bool );
  itkGetConstMacro( AllowSuperResolution, bool );
  itkBooleanMacro(AllowSuperResolution);
  
  /**Variables for computing residue */
  typename ResidualImageType::Pointer               m_ResidualImage;
  bool                                              m_CalculateResidualImage;

private:

  /** container to hold gradient directions */
  GradientDirectionContainerType::Pointer           m_GradientDirectionContainer;

  /** Number of gradient measurements */
  unsigned int                                      m_NumberOfGradientDirections;

  /** Number of baseline images */
  unsigned int                                      m_NumberOfBaselineImages;

  /** Image Mask */
  mutable ImageMaskPointer                          m_ImageMask;

  bool                                              m_AllowSuperResolution;
  
  /** Lambda for Controlling Beltrami Regularization */
  double                                            m_BeltramiLambda;

};

}

#include "itkSymRshReconImageFilter.txx"

#endif
