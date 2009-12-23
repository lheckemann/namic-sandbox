#ifndef __itkOdfReconImageFilter_h_
#define __itkOdfReconImageFilter_h_

#include "itkSymRealSphericalHarmonicRep.h"
#include "itkSymRshReconImageFilter.h"

namespace itk{

template< 
          class TGradientImagePixelType,
          class TOutputPixelType = SymRealSphericalHarmonicRep< float, 4 >,
          unsigned int TImageDimension=3,
          class TResidualPercisionType = float
        >
class ITK_EXPORT OdfReconImageFilter :
  public SymRshReconImageFilter<  TGradientImagePixelType,
                                  TOutputPixelType, TImageDimension, TResidualPercisionType >

{
public:

  typedef OdfReconImageFilter                  Self;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

  typedef SymRshReconImageFilter< TGradientImagePixelType,
                                  TOutputPixelType, TImageDimension, TResidualPercisionType >
                                                  Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(OdfReconImageFilter, SymRshReconImageFilter);

  /** pass through typdefs from superclass. */
  typedef typename Superclass::GradientPixelType                GradientPixelType;
  typedef typename Superclass::ResidualPixelType                ResidualPixelType;

  typedef typename Superclass::OutputImageType                  OutputImageType;
  typedef typename Superclass::OutputPixelType                  OutputPixelType;
  typedef typename Superclass::OutputImageRegionType            OutputImageRegionType;
  typedef typename Superclass::GradientImagesType               GradientImagesType;
  typedef typename Superclass::GradientDirectionContainerType   GradientDirectionContainerType;

  /** Flag for whether or not we want normalized odfs
   */
  itkSetMacro( Normalize, bool );
  itkBooleanMacro( Normalize );

protected:
  OdfReconImageFilter();
  ~OdfReconImageFilter() {};

  /** just overload the computeCoeffsFromSignal method. */
  virtual vnl_vector< double >  ComputeCoeffsFromSignal( vnl_vector< double >,ResidualPixelType& );

private:

  /** We should normalize the Odf after we compute it */
  bool                                              m_Normalize;

};

} //end itkNamespace


#include "itkOdfReconImageFilter.txx"

#endif
