#include __itkWeightedLeastSquaresDiffusionTensorFilter_h__
#define __itkWeightedLeastSquaresDiffusionTensorFilter_h__

#include "itkUnaryFunctorImageFilter.h"
#include "vnl/vnl_math.h"

namespace itk
{

namespace Functor {

template< class TInput, class TOutput >
class WeightedLeastSquaresDiffusionTensor{
public:
  /** Type for the Transformation Matrix for the gradient directions **/
  typedef vnl_matrix_fixed< double, 3, 3 > TransformMatrixType;
  
  /** Type for the sample directions **/
  typedef VectorContainer< unsigned int, vnl_vector_fixed< double, 3 > > 
    TractOrientationContainerType;
  
  /** Types for the Image-wide bValues **/
  typedef double bValueType;
  typedef VectorContainer< unsigned int, bValueType > bValueContainerType;
  
  /** Set/Get bvalues **/
  itkSetConstObjectMacro( bValues, bValueContainerType );
  itkGetConstObjectMacro( bValues, bValueContainerType ); 
  
  WeightedLeastSquaresDiffusionTensor():
    m_Gradients(NULL), m_bValues(NULL), m_A(NULL), m_Aqr(NULL){};
  ~WeightedLeastSquaresDiffusionTensor(){
    if(this->m_A) delete this->m_A;
    if(this->m_Aqr) delete this->m_Aqr;
  };
  bool operator!=( const WeightedLeastSquaresDiffusionTensor & ) const{
    return false;
  }
  bool operator==( const WeightedLeastSquaresDiffusionTensor & other ) const{
    return !(*this != other);
  }
  
  void SetScanParameters( GradientDirectionContainerType::Pointer gradients,
    bValueContainerType::Pointer bvalues ){
    this->m_Gradients = gradients;
    this->m_bValues = bvalues;
    
    unsigned int N = this->m_Gradients->Size();
    
    if(this->m_A!=NULL)
      delete this->m_A;
    this->m_A = new vnl_matrix< double >(N, 7); //potential memory leak here
    vnl_matrix< double >& A = *(this->m_A);
    
    for(unsigned int j=0; j< N ; j++){
      GradientDirectionContainerType::Element g = m_Gradients->GetElement(j);
      const bValueType&  b_i = m_bValues->GetElement(j);
      
      A(j,0)=1.0;
      A(j,1)=-1.0*b_i*(g[0]*g[0]);
      A(j,2)=-1.0*b_i*(g[1]*g[1]);
      A(j,3)=-1.0*b_i*(g[2]*g[2]);
      A(j,4)=-1.0*b_i*(2*g[0]*g[1]);
      A(j,5)=-1.0*b_i*(2*g[0]*g[2]);
      A(j,6)=-1.0*b_i*(2*g[1]*g[2]);
    }
    
    //Store a QR decomposition to quickly estimate
    //the weighting matrix for each voxel
    if(this->m_Aqr!=NULL)
      delete this->m_Aqr;
    this->m_Aqr = new vnl_qr< double >(A);  //potential memory leak here
  }

  TOutput operator()( const TInput & Y ){
      
    unsigned int N = this->m_Gradients->Size();
    TensorModelParamType tensormodelparams( 0.0 );
    vnl_diag_matrix< double > W(N,0);
    
    //setup const references for code clarity
    const vnl_matrix< double >& A = *(this->m_A);
    const vnl_qr< double >& Aqr = *(this->m_Aqr);
    
    //vnl_vector is used because the itk vector is limited in its methods and does not
    //contain an internal vnl class like VariableSizematrix
    //also itk_matrix has methods which are compatible with vnl_vectors
    vnl_vector< double > logPhi( N );
    
    for(unsigned int j=0; j< N ; j++){
      //fill up the logPhi vector using log(dwi) values
      logPhi.put(j, vcl_log(static_cast<double>(Y[j]) + vnl_math::eps));
    }
    
    /** Find WLS estimate of the parameters of the Tensor model **/
    
    // First estimate W by LS estimation of the intensities
    W = A* vnl_qr< double >(Aqr.R()).solve(Aqr.QtB(logPhi));
 
    for(vnl_diag_matrix< double >::iterator i = W.begin();i!=W.end(); i++){
      *i = vcl_exp( *i );
    }
    
    // Now solve for parameters using the estimated weighing matrix
    tensormodelparams = vnl_qr< double >((W*A).transpose()*W*A).solve(
      (W*A).transpose()*W*logPhi);
      
    return tensormodelparams; //maybe inefficient, results in an extra copy before reaching the image
  }
  
protected:
  // it is assumed that these gradient directions are in IJK image space
  GradientDirectionContainerType::ConstPointer m_Gradients;
  bValueContainerType::ConstPointer m_bValues;
  
  vnl_matrix< double >* m_A;
  vnl_qr< double >* m_Aqr
};

template < class TInputImage, class TOutputImage >
class WeightedLeastSquaresDiffusionTensorFilter :
  public UnaryFunctorImageFilter< TInputImage, TOutputImage,
    Functor::WeightedLeastSquaresDiffusionTensor< typename TInputImage::PixelType,
      typename TOutputImage::PixelType > >{
public:
  typedef WeightedLeastSquaresDiffusionTensorFilter Self;
  typedef UnaryFunctorImageFilter< TInputImage, TOutputImage,
    Functor::WeightedLeastSquaresDiffusionTensorFilter<
      typename TInputImage::PixelType,
      typename TOutputImage::PixelType >  Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro( Self );
  itkTypeMacro( WeightedLeastSquaresDiffusionTensorFilter, UnaryFunctorImageFilter );
  
protected:
  WeightedLeastSquaresDiffusionTensorFilter();
  virtual ~WeightedLeastSquaresDiffusionTensorFilter() {}
  virtual void BeforeThreadedGenerateData();
  void SetScanParameters( GradientDirectionContainerType::Pointer gradients,
    bValueContainerType::Pointer bvalues ){
    this->GetFunctor().SetScanParameters( gradients, bvalues );
  }
  
private:
  WeightedLeastSquaresDiffusionTensorFilter(const Self&); //purposely not implemented
  void operator=(const Self&);  //purposely not implemented
}

#endif
