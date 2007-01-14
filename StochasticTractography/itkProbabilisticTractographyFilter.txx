#include "itkProbabilisticTractographyFilter.h"
#include "vnl/vnl_math.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_diag_matrix.h"
#include "vnl/algo/vnl_qr.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_transpose.h"
#include "itkVariableSizeMatrix.h"
#include "itkPathIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk{

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::ProbabilisticTractographyFilter(){

  this->SetTotalTracts( 50 );
  this->SetMaxTractLength( 100 );
  this->SetGradients(NULL);
  this->SetbValues(NULL);
  this->SetSampleDirections(NULL);
  this->m_A=NULL;
  this->m_Aqr=NULL;
  this->m_W=NULL;
  this->m_LikelihoodCache = NULL;
} 

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::~ProbabilisticTractographyFilter(){
  delete this->m_A;
  delete this->m_Aqr;
  delete this->m_W;
} 

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::ProbabilisticallyInterpolate( vnl_random& randomgenerator, 
                      const PathType::ContinuousIndexType& cindex,
                      typename InputDWIImageType::IndexType& index){
                      
  for(int i=0; i<3; i++){
    if ((vcl_ceil(cindex[i]+vnl_math::eps)-cindex[i]) < randomgenerator.drand32())
       index[i]=(int)vcl_ceil(cindex[i]);
     else index[i]=(int)vcl_floor(cindex[i]);
  }
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::UpdateGradientDirections(void){
  //the gradient direction is transformed into IJK space
  //by moving into RAS space and then to LPS space then to IJK space
  
  GradientDirectionContainerType::ConstPointer gradients_orig = this->GetGradients();
  GradientDirectionContainerType::Pointer gradients_prime = GradientDirectionContainerType::New();
  unsigned int N = gradients_orig->Size();
  for(unsigned int i=0; i<N; i++){
    GradientDirectionContainerType::Element g_i = 
      this->GetMeasurementFrame() *
      gradients_orig->GetElement(i);
    
    g_i[0] = -g_i[0];
    g_i[1] = -g_i[1];
    g_i = this->GetInput()->GetDirection().GetInverse() * g_i;  
    gradients_prime->InsertElement(i, g_i);
  }
  this->SetGradients(gradients_prime);
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::UpdateTensorModelFittingMatrices( void ){

  //estimate the parameters using linear LS estimation
  //using convention specified by Salvador
  //solve for Beta in: logPhi=X*Beta
  //number of rows of the matrix depends on the number of inputs,
  //i.e. the number of measurements of the voxel (n)
  unsigned int N = (this->GetGradients())->Size();
  
  this->m_A = new vnl_matrix< double >(N, 7);
  vnl_matrix< double >& A = *(this->m_A);
  
  for(unsigned int j=0; j< N ; j++){
    GradientDirectionContainerType::Element g = GetGradients()->GetElement(j);
    const bValueType&  b_i = GetbValues()->GetElement(j);
    
    A(j,0)=1.0;
    A(j,1)=-1*b_i*(g[0]*g[0]);
    A(j,2)=-1*b_i*(g[1]*g[1]);
    A(j,3)=-1*b_i*(g[2]*g[2]);
    A(j,4)=-1*b_i*(2*g[0]*g[1]);
    A(j,5)=-1*b_i*(2*g[0]*g[2]);
    A(j,6)=-1*b_i*(2*g[1]*g[2]);
  }
  
  //Store a QR decomposition to quickly estimate
  //the weighing matrix for each voxel
  this->m_Aqr = new vnl_qr< double >(A);
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::CalculateTensorModelParameters( const DWIVectorImageType::PixelType& dwivalues,
  TensorModelParamType& tensormodelparams){
  
  unsigned  int N = (this->GetGradients())->Size();
  
  //setup references for code clarity
  vnl_matrix< double >& A = *(this->m_A);
  vnl_qr< double >& Aqr = *(this->m_Aqr);
  
  //vnl_vector is used because the itk vector is limited in its methods and does not
  //contain an internal vnl class like VariableSizematrix
  //also itk_matrix has methods which are compatible with vnl_vectors
  vnl_vector< double > logPhi( N );
  
  for(unsigned int j=0; j< N ; j++){
    //fill up the logPhi vector using log(dwi) values
    //add eps to prevent vcl_log from returning infinte values
    logPhi.put(j, vcl_log((double) dwivalues[j]));
  }
  
  // Find WLS estimate of the parameters of the Tensor model
  
  // First estimate W by LS estimation of the intensities
  this->m_W = new vnl_diag_matrix< double >(A * Aqr.solve(logPhi));
  
  //setup a reference for code clarity
  vnl_diag_matrix< double >& W = *(this->m_W);
  
  for(vnl_diag_matrix< double >::iterator i = W.begin();
      i!=W.end(); i++){
    *i = vcl_exp( *i );
  }
  
  // Now solve for parameters using the estimated weighing matrix
  tensormodelparams = vnl_svd< double >((W*A)).solve(W*logPhi);
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::CalculateConstrainedModelParameters( const TensorModelParamType& tensormodelparams,
    ConstrainedModelParamType& constrainedmodelparams){          
              
  vnl_matrix_fixed< double, 3, 3 > D;
  double alpha, beta;
  //set the tensor model parameters into a Diffusion tensor
  D(0,0) = tensormodelparams[1];
  D(0,1) = tensormodelparams[4];
  D(0,2) = tensormodelparams[5];
  D(1,0) = tensormodelparams[4];
  D(1,1) = tensormodelparams[2];
  D(1,2) = tensormodelparams[6];
  D(2,0) = tensormodelparams[5];
  D(2,1) = tensormodelparams[6];
  D(2,2) = tensormodelparams[3];
  
  //pass through the no gradient intensity Z_0 and
  //calculate alpha, beta and v hat (the eigenvector 
  //associated with the largest eigenvalue)
  vnl_symmetric_eigensystem< double > Deig(D);
  
  //need to take abs to get rid of negative eigenvalues
  alpha = (vcl_abs(Deig.get_eigenvalue(0)) + vcl_abs(Deig.get_eigenvalue(1))) / 2;
  beta = vcl_abs(Deig.get_eigenvalue(2)) - alpha;
  
  constrainedmodelparams[0] = tensormodelparams[0];
  constrainedmodelparams[1] = alpha;
  constrainedmodelparams[2] = beta;
  constrainedmodelparams[3] = (Deig.get_eigenvector(2))[0];
  constrainedmodelparams[4] = (Deig.get_eigenvector(2))[1];
  constrainedmodelparams[5] = (Deig.get_eigenvector(2))[2];                  
}
              
template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::CalculateNoiseFreeDWIFromConstrainedModel( const ConstrainedModelParamType& constrainedmodelparams,
    DWIVectorImageType::PixelType& noisefreedwi){
    
  unsigned int N = (this->GetGradients())->Size();
  const double& z_0 = constrainedmodelparams[0];
  const double& alpha = constrainedmodelparams[1];
  const double& beta = constrainedmodelparams[2];
  TractOrientationContainerType::Element v_hat( constrainedmodelparams[3],
    constrainedmodelparams[4],
    constrainedmodelparams[5]);
  
  for(unsigned int i=0; i < N ; i++ ){
    const double& b_i = (this->GetbValues())->GetElement(i);
    const GradientDirectionContainerType::Element& g_i = 
      (this->GetGradients())->GetElement(i);
    
    noisefreedwi.SetElement(i,
      vcl_exp(z_0-(alpha*b_i+beta*b_i*vnl_math_sqr(dot_product(g_i, v_hat)))));
  }     
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::CalculateResidualVariance( const DWIVectorImageType::PixelType& noisydwi,
    const DWIVectorImageType::PixelType& noisefreedwi,
    const unsigned int dof,
    double& residualvariance){
    
  unsigned int N = (this->GetGradients())->Size();
  vnl_diag_matrix< double >& W = *(this->m_W);
  
  residualvariance=0;
  
  /** Not sure if we should be taking difference of log or nonlog intensities **/
  /** residual variance is too low if we take the difference of log intensities **/
  /** perhaps using WLS will correct this problem **/
  for(unsigned int i=0; i<N; i++)
    residualvariance+=vnl_math_sqr(W(i,i) * (vcl_log(noisydwi[i]/noisefreedwi[i])));
  residualvariance/=(N-dof);
}
                                 
template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::CalculateLikelihood( const DWIVectorImageType::PixelType &dwipixel, 
    TractOrientationContainerType::ConstPointer orientations,
    ProbabilityDistributionImageType::PixelType& likelihood){
    
  unsigned int N = (this->GetGradients())->Size();
  TensorModelParamType tensorparams;
  ConstrainedModelParamType constrainedparams;
  DWIVectorImageType::PixelType noisefreedwi(N);
  double residualvariance;
  
  CalculateTensorModelParameters( dwipixel, tensorparams );
  CalculateConstrainedModelParameters( tensorparams, constrainedparams );
  CalculateNoiseFreeDWIFromConstrainedModel( constrainedparams, noisefreedwi );
  CalculateResidualVariance( dwipixel, noisefreedwi, 5, residualvariance );
  
  for(unsigned int i=0; i < orientations->Size(); i++){
    /** Vary the entry corresponding to the estimated
      Tract orientation over the selected sample directions,
      while preserving the best estimate for the other parameters **/
    TractOrientationContainerType::Element currentdir = orientations->GetElement(i);
    
    /** Incorporate the current sample direction into the secondary parameters **/
    constrainedparams[3]=currentdir[0];
    constrainedparams[4]=currentdir[1];
    constrainedparams[5]=currentdir[2];
    
    /** Obtain the estimated
      intensity for this choice of Tract direction **/
    CalculateNoiseFreeDWIFromConstrainedModel(constrainedparams, noisefreedwi);
    
    double jointlikelihood = 1.0;
    for(unsigned int j=0; j<N; j++){
      /** Calculate the likelihood given the residualvariance,
        estimated intensity and the actual intensity (refer to Friman) **/
      jointlikelihood *= 
        (noisefreedwi[j]/vcl_sqrt(2*vnl_math::pi*residualvariance))*
          vcl_exp(-vnl_math_sqr(noisefreedwi[j]*vcl_log(dwipixel[j]/noisefreedwi[j]))/
                (2*residualvariance));
    }
    likelihood[i]=jointlikelihood;  
  }
  double a = 1;
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::CalculatePrior( TractOrientationContainerType::Element v_prev, 
    TractOrientationContainerType::ConstPointer orientations,
    ProbabilityDistributionImageType::PixelType& prior ){
    
  const double gamma = 1;
          
  for(unsigned int i=0; i < orientations->Size(); i++){
    if(v_prev.squared_magnitude()==0){
      prior[i]=1.0;
    }
    else{
      prior[i] = dot_product(orientations->GetElement(i),v_prev);;
      if(prior[i]<0){
        prior[i]=0;
      }
      else{
        prior[i]=vcl_pow(prior[i],gamma);
      }
    }
  }
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::CalculatePosterior( const ProbabilityDistributionImageType::PixelType& likelihood,
    const ProbabilityDistributionImageType::PixelType& prior,
    ProbabilityDistributionImageType::PixelType& posterior){
    
    double sum=0;
    for(unsigned int i=0; i<likelihood.Size(); i++){
      sum+=likelihood[i]*prior[i];
    }
    for(unsigned int i=0; i<likelihood.Size(); i++){
      posterior[i] = (likelihood[i]*prior[i])/sum;
    }
}

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::SampleTractOrientation( vnl_random& randomgenerator, 
    const ProbabilityDistributionImageType::PixelType& posterior,
    TractOrientationContainerType::ConstPointer orientations,
    TractOrientationContainerType::Element& choosendirection ){
    
      double randomnum = randomgenerator.drand32();
      int i=0;
      double cumsum=0;
      
      //will crash in the unlikely case that 0 was choosen as the randomnum
      while(cumsum < randomnum){
        cumsum+=posterior[i];
        i++;
      }
      choosendirection = orientations->GetElement(i-1);
      
      //std::cout<< "cumsum: " << cumsum<<std::endl;
      //std::cout<<"selected orientation:( " << (i-1) 
      //  <<") "<<choosendirection<< std::endl;
}

//the seedpoint is in continuous IJK coordinates
template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::StochasticTractGeneration( typename InputDWIImageType::ConstPointer dwiimagePtr,
  PathType::ContinuousIndexType seedpoint,
  PathType::Pointer tractPtr){
  
  PathType::ContinuousIndexType cindex_curr;
  typename InputDWIImageType::IndexType index_curr;
  
  vnl_random randomgenerator;
  TractOrientationContainerType::Element v_curr(0,0,0);
  TractOrientationContainerType::Element v_prev(0,0,0);
  
  tractPtr->Initialize();
  
  cindex_curr = seedpoint;
  for(unsigned int j=0; (j<this->GetMaxTractLength()) &&
    (dwiimagePtr->GetLargestPossibleRegion().IsInside(cindex_curr)); j++){

    ProbabilityDistributionImageType::PixelType 
      prior_curr(this->GetSampleDirections()->Size()); 
    ProbabilityDistributionImageType::PixelType 
      posterior_curr(this->GetSampleDirections()->Size());
      
    tractPtr->AddVertex(cindex_curr);
    
    this->ProbabilisticallyInterpolate( randomgenerator, cindex_curr,
                                      index_curr );
    
    
    ProbabilityDistributionImageType::PixelType&
      likelihood_curr = m_LikelihoodCache->GetPixel( index_curr );  
                            
    if( likelihood_curr.GetSize() == 0){
      //std::cout<<"Cache Miss!\n";
      likelihood_curr.SetSize(this->GetSampleDirections()->Size());
      
      this->CalculateLikelihood(static_cast< DWIVectorImageType::PixelType >(
        dwiimagePtr->GetPixel(index_curr)) + vnl_math::eps,
        this->GetSampleDirections(),
        likelihood_curr);
    }

    this->CalculatePrior( v_prev, this->GetSampleDirections(), prior_curr);
    this->CalculatePosterior( likelihood_curr, prior_curr, posterior_curr);
    this->SampleTractOrientation(randomgenerator, posterior_curr,
                          this->GetSampleDirections(), v_curr);
          
    cindex_curr[0]+=v_curr[0];
    cindex_curr[1]+=v_curr[1];
    cindex_curr[2]+=v_curr[2];
    v_prev=v_curr;
  }
}
template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
void
ProbabilisticTractographyFilter< TInputDWIImage, TInputROIImage,
                      TOutputConnectivityImage >
::GenerateData(){
  typedef ProbabilityDistributionImageType PDImageType;
  //preparations
  this->BeforeGenerateData();
  
  //allocate outputs
  this->AllocateOutputs();
  
  typename InputDWIImageType::ConstPointer inputDWIImagePtr = this->GetDWIImageInput();
  typename InputROIImageType::ConstPointer inputROIImagePtr = this->GetROIImageInput();
  typename OutputConnectivityImageType::Pointer outputPtr = this->GetOutput();
  
  outputPtr->FillBuffer(0);
  typedef ImageRegionConstIterator< InputDWIImageType > InputDWIImageIteratorType;
  typedef ImageRegionConstIterator< InputROIImageType > InputROIImageIteratorType;
  typedef ImageRegionIterator< OutputConnectivityImageType > OutputIteratorType;
  typedef PathIterator< OutputConnectivityImageType, PathType > OutputPathIteratorType;
  
  InputROIImageIteratorType inputROIImageIt( inputROIImagePtr, 
    outputPtr->GetRequestedRegion() );
  OutputIteratorType outputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  PathType::Pointer tractPtr = PathType::New();
  unsigned int voxelnum = 0;
  
  for(inputROIImageIt.GoToBegin(); !inputROIImageIt.IsAtEnd(); ++inputROIImageIt){
    if(inputROIImageIt.Get() > 0 && inputROIImageIt.Get() < 100){
      std::cout << "PixelIndex: "<< inputROIImageIt.GetIndex() << std::endl;
      for(unsigned int i=0; i<this->GetTotalTracts(); i++){
        StochasticTractGeneration(inputDWIImagePtr,
          static_cast< PathType::ContinuousIndexType >(inputROIImageIt.GetIndex()),
          tractPtr);
      
        OutputPathIteratorType outputtractIt( outputPtr, tractPtr );

        for(outputtractIt.GoToBegin(); !outputtractIt.IsAtEnd(); ++outputtractIt){
          /* there is an issue using outputtractIt.Value() */
          outputtractIt.Set(outputtractIt.Get()+1);
        }
        //std::cout<<"Tract: "<<i<<" complete."<<std::endl;
        //for(outputIt.GoToBegin(); !outputIt.IsAtEnd(); ++outputIt){
        //  outputIt.Set(outputIt.Get()/this->GetTotalTracts());
        //}
      }
      std::cout<<"----------Voxel: "<<voxelnum++<<" complete\n";  
    }
  }
}

}
