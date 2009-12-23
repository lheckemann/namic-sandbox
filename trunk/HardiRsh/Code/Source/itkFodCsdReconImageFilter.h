#ifndef __itkFodCsdReconImageFilter_h_
#define __itkFodCsdReconImageFilter_h_

#include "itkSymRealSphericalHarmonicRep.h"
#include "itkSymRshReconImageFilter.h"

namespace itk{

template<
          class TGradientImagePixelType,
          class TOutputPixelType = SymRealSphericalHarmonicRep< float, 4 >,
          unsigned int TImageDimension=3,
          class TResidualPercisionType = float
        >
class ITK_EXPORT FodCsdReconImageFilter :
  public SymRshReconImageFilter<  TGradientImagePixelType,
                                  TOutputPixelType, TImageDimension, TResidualPercisionType >

{
public:

  typedef FodCsdReconImageFilter                  Self;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

  typedef SymRshReconImageFilter<  TGradientImagePixelType,
                                  TOutputPixelType, TImageDimension, TResidualPercisionType >
                                                  Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(FodCsdReconImageFilter, SymRshReconImageFilter);

  /** pass through typdefs from superclass. */
  typedef typename Superclass::GradientPixelType                GradientPixelType;
  typedef typename Superclass::ResidualPixelType                ResidualPixelType;

  typedef typename Superclass::OutputImageType                  OutputImageType;
  typedef typename Superclass::OutputPixelType                  OutputPixelType;
  typedef typename Superclass::OutputImageRegionType            OutputImageRegionType;
  typedef typename Superclass::GradientImagesType               GradientImagesType;
  typedef typename Superclass::GradientDirectionContainerType   GradientDirectionContainerType;
  
  typedef typename Superclass::RshBasisMatrixType               RshBasisMatrixType;
  typedef typename Superclass::RshBasisMatrixInverseType        RshBasisMatrixInverseType;

  typedef vnl_vector< double >                                  VectorType;
  typedef typename OutputPixelType::BasisType::LmVector         LmVector;

  itkStaticConstMacro(NumberOfCoefficients,unsigned int, OutputPixelType::Dimension);

  itkSetMacro(ResponseRSH,VectorType);
  itkGetConstMacro(ResponseRSH,VectorType);

  itkSetMacro(RecipricalFilter,VectorType);
  itkGetConstMacro(RecipricalFilter,VectorType);

  itkSetMacro(NumConstraintGrads, unsigned int);
  itkGetConstMacro(NumConstraintGrads, unsigned int);

  itkSetMacro(Threshold, double);
  itkGetConstMacro(Threshold, double);

  itkSetMacro(Regulizer, double);
  itkGetConstMacro(Regulizer, double);

  itkSetMacro(UseCSD, bool);
  itkGetMacro(UseCSD, bool);
  itkBooleanMacro(UseCSD);

  /** Flag for whether or not we want normalized FODs
   */
  itkSetMacro( Normalize, bool );
  itkGetMacro( Normalize, bool );
  itkBooleanMacro( Normalize );

protected:
  FodCsdReconImageFilter();
  ~FodCsdReconImageFilter() {};

  /** overload the computeCoeffsFromSignal method. */
  virtual vnl_vector< double >  ComputeCoeffsFromSignal( vnl_vector< double >,ResidualPixelType& );
  
  /** Also Overload the ComputeRshBasis method */
  void ComputeRshBasis();

private:

  /** the fiber impulse response in the RSH space!*/
  VectorType                                m_ResponseRSH;

  /** 1 Over the Filter */
  VectorType                                m_RecipricalFilter;
  
  //Parameters contolling Positivity Constraint!
  /** Use Positivity Constratin */
  bool                                      m_UseCSD;  

  /** Number of gradients to use for Pos Constratint. Def 256*/
  unsigned int                              m_NumConstraintGrads;

  /** Number of Iterations to use. Defualt 50*/
  unsigned int                              m_NumIters;
  
  /** Regularizer used to weight positivity constraints Def 1.0*/
  double                                    m_Regulizer;

  /** thresh to determine positivty 0.1*/
  double                                    m_Threshold;

  /** basis for evaluating FOD at high res */
  RshBasisMatrixType                        m_HiResRshBasis;

  /** We should normalize the Fod after we compute it */
  bool                                      m_Normalize;

};

} //end itkNamespace


#include "itkFodCsdReconImageFilter.txx"

#endif
