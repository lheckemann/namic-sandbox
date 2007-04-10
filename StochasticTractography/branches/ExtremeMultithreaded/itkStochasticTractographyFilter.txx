#include "itkStochasticTractographyFilter.h"
#include "vnl/vnl_math.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sym_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_diag_matrix.h"
#include "vnl/algo/vnl_qr.h"
//#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_matrix_inverse.h"
//#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkSymmetricEigenAnalysis.h"
#include "vnl/vnl_transpose.h"
#include "itkVariableSizeMatrix.h"
#include "itkPathIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk{

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::StochasticTractographyFilter():
  m_TotalTracts(0),m_MaxTractLength(0),m_Gradients(NULL),m_bValues(NULL),
  m_SampleDirections(NULL), m_A(NULL), m_Aqr(NULL), m_LikelihoodCachePtr(NULL),
  m_MaxLikelihoodCacheSize(0), m_CurrentLikelihoodCacheElements(0), m_RandomSeed(0),
  m_ClockPtr(NULL), m_TotalDelegatedTracts(0) {
  this->m_SeedIndex[0]=0;
  this->m_SeedIndex[1]=0;
  this->m_SeedIndex[2]=0;
  this->m_MeasurementFrame.set_identity();
  this->SetNumberOfRequiredInputs(2); //Filter needs a DWI image and a Mask Image
  
  m_ClockPtr = RealTimeClock::New();
  //load in default sample directions
  this->LoadDefaultSampleDirections();
} 

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::~StochasticTractographyFilter(){
  delete this->m_A;
  delete this->m_Aqr;
} 

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::ProbabilisticallyInterpolate( vnl_random& randomgenerator, 
                      const PathType::ContinuousIndexType& cindex,
                      typename InputDWIImageType::IndexType& index){
                      
  for(int i=0; i<3; i++){
    if ((vcl_ceil(cindex[i]+vnl_math::eps)-cindex[i]) < randomgenerator.drand64())
       index[i]=(int)vcl_ceil(cindex[i]);
     else index[i]=(int)vcl_floor(cindex[i]);
  }
}

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::UpdateGradientDirections(void){
  //the gradient direction is transformed into IJK space
  //by moving into the image space and then to IJK space
  
  GradientDirectionContainerType::ConstPointer gradients_orig = this->m_Gradients;
  GradientDirectionContainerType::Pointer gradients_prime = GradientDirectionContainerType::New();
  unsigned int N = gradients_orig->Size();
  for(unsigned int i=0; i<N; i++){
    GradientDirectionContainerType::Element g_i = 
      this->m_MeasurementFrame *
      gradients_orig->GetElement(i);
    
    /** The correction to LPS space is not neccessary as of itk 3.2 **/
    //g_i[0] = -g_i[0];
    //g_i[1] = -g_i[1];
    g_i = this->GetInput()->GetDirection().GetInverse() * g_i;  
    gradients_prime->InsertElement(i, g_i);
  }
  this->m_Gradients=gradients_prime;
}

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::UpdateTensorModelFittingMatrices( void ){
  std::cout<<"UpdateTensorFittingMatrix\n";
  //estimate the parameters using linear LS estimation
  //using convention specified by Salvador
  //solve for Beta in: logPhi=X*Beta
  //number of rows of the matrix depends on the number of inputs,
  //i.e. the number of measurements of the voxel (n)
  unsigned int N = this->m_Gradients->Size();
  
  this->m_A = new vnl_matrix< double >(N, 7);
  vnl_matrix< double >& A = *(this->m_A);
  
  for(unsigned int j=0; j< N ; j++){
    GradientDirectionContainerType::Element g = m_Gradients->GetElement(j);
    const bValueType&  b_i = m_bValues->GetElement(j);
    
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

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::CalculateTensorModelParameters( const DWIVectorImageType::PixelType& dwivalues,
  vnl_diag_matrix<double>& W,
  TensorModelParamType& tensormodelparams){
  
  unsigned  int N = this->m_Gradients->Size();
  
  //setup const references for code clarity
  const vnl_matrix< double >& A = *(this->m_A);
  const vnl_qr< double >& Aqr = *(this->m_Aqr);
  
  //vnl_vector is used because the itk vector is limited in its methods and does not
  //contain an internal vnl class like VariableSizematrix
  //also itk_matrix has methods which are compatible with vnl_vectors
  vnl_vector< double > logPhi( N );
  
  for(unsigned int j=0; j< N ; j++){
    //fill up the logPhi vector using log(dwi) values
    logPhi.put(j, vcl_log(static_cast<double>(dwivalues[j]) + vnl_math::eps));
  }
  
  /** Find WLS estimate of the parameters of the Tensor model **/
  
  // First estimate W by LS estimation of the intensities
  //vnl_matrix< double > Q = Aqr.Q();
  //vnl_vector< double > QtB = Aqr.Q().transpose()*logPhi;
  //vnl_vector< double > QTB = Aqr.QtB(logPhi);
  //vnl_matrix< double > R = Aqr.R(); 
  W = A* vnl_qr< double >(Aqr.R()).solve(Aqr.QtB(logPhi));
  //W = A * Aqr.solve(logPhi);  
  for(vnl_diag_matrix< double >::iterator i = W.begin();i!=W.end(); i++){
    *i = vcl_exp( *i );
  }
  
  // Now solve for parameters using the estimated weighing matrix
  tensormodelparams = vnl_qr< double >((W*A).transpose()*W*A).solve(
    (W*A).transpose()*W*logPhi);
  int a;
  //tensormodelparams = vnl_qr< double >((W*A)).solve(W*logPhi);
}

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::CalculateConstrainedModelParameters( const TensorModelParamType& tensormodelparams,
    ConstrainedModelParamType& constrainedmodelparams){          
              
  vnl_sym_matrix< double > D( 3, 0 );
  double alpha =0;
  double beta=0;
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
  vnl_matrix_fixed< double, 3, 3 > S(0.0);
  vnl_vector_fixed< double, 3 > Lambda(0.0);
  SymmetricEigenAnalysis< vnl_sym_matrix< double >,
    vnl_vector_fixed< double, 3 >, vnl_matrix_fixed< double, 3, 3 > >
    eigensystem( 3 );
  eigensystem.ComputeEigenValuesAndVectors( D, Lambda, S );
  
  //need to take abs to get rid of negative eigenvalues
  alpha = (vcl_abs(Lambda[0]) + vcl_abs(Lambda[1])) / 2;
  beta = vcl_abs(Lambda[2]) - alpha;
  
  constrainedmodelparams[0] = tensormodelparams[0];
  constrainedmodelparams[1] = alpha;
  constrainedmodelparams[2] = beta;
  constrainedmodelparams[3] = S[2][0];
  constrainedmodelparams[4] = S[2][1];
  constrainedmodelparams[5] = S[2][2];                  
}
              
template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::CalculateNoiseFreeDWIFromConstrainedModel( const ConstrainedModelParamType& constrainedmodelparams,
    DWIVectorImageType::PixelType& noisefreedwi){
    
  unsigned int N = this->m_Gradients->Size();
  const double& z_0 = constrainedmodelparams[0];
  const double& alpha = constrainedmodelparams[1];
  const double& beta = constrainedmodelparams[2];
  TractOrientationContainerType::Element v_hat( constrainedmodelparams[3],
    constrainedmodelparams[4],
    constrainedmodelparams[5]);
  
  for(unsigned int i=0; i < N ; i++ ){
    const double& b_i = this->m_bValues->GetElement(i);
    const GradientDirectionContainerType::Element& g_i = 
      this->m_Gradients->GetElement(i);
    
    noisefreedwi.SetElement(i,
      vcl_exp(z_0-(alpha*b_i+beta*b_i*vnl_math_sqr(dot_product(g_i, v_hat)))));
  }     
}

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::CalculateResidualVariance( const DWIVectorImageType::PixelType& noisydwi,
    const DWIVectorImageType::PixelType& noisefreedwi,
    const vnl_diag_matrix< double >& W,
    const unsigned int numberofparameters,
    double& residualvariance){
    
  unsigned int N = this->m_Gradients->Size();
  
  residualvariance=0;
  
  /** Not sure if we should be taking difference of log or nonlog intensities **/
  /** residual variance is too low if we take the difference of log intensities **/
  /** perhaps using WLS will correct this problem **/
  for(unsigned int i=0; i<N; i++)
    residualvariance+=vnl_math_sqr(W(i,i) * (vcl_log(noisydwi[i]/noisefreedwi[i])));
  residualvariance/=(N-numberofparameters);
}
                                 
template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::CalculateLikelihood( const DWIVectorImageType::PixelType &dwipixel, 
    TractOrientationContainerType::ConstPointer orientations,
    ProbabilityDistributionImageType::PixelType& likelihood){
    
  unsigned int N = this->m_Gradients->Size();
  TensorModelParamType tensorparams( 0.0 );
  vnl_diag_matrix< double > W(N,0);
  ConstrainedModelParamType constrainedparams( 0.0 );
  DWIVectorImageType::PixelType noisefreedwi(N);
  double residualvariance=0;
  double jointlikelihood=1;
  
  CalculateTensorModelParameters( dwipixel, W, tensorparams );
  CalculateConstrainedModelParameters( tensorparams, constrainedparams );
  CalculateNoiseFreeDWIFromConstrainedModel( constrainedparams, noisefreedwi );
  CalculateResidualVariance( dwipixel, noisefreedwi, W, 6, residualvariance );
  
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
    
    jointlikelihood = 1.0;
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
}

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
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

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
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

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::SampleTractOrientation( vnl_random& randomgenerator, 
    const ProbabilityDistributionImageType::PixelType& posterior,
    TractOrientationContainerType::ConstPointer orientations,
    TractOrientationContainerType::Element& choosendirection ){
    
      double randomnum = randomgenerator.drand64();
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

//the seedindex is in continuous IJK coordinates
template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::StochasticTractGeneration( typename InputDWIImageType::ConstPointer dwiimagePtr,
  typename InputMaskImageType::ConstPointer maskimagePtr,
  typename InputDWIImageType::IndexType seedindex,
  unsigned long randomseed,
  PathType::Pointer tractPtr){
  
  PathType::ContinuousIndexType cindex_curr = seedindex;
  typename InputDWIImageType::IndexType index_curr = {{0,0,0}};
  ProbabilityDistributionImageType::PixelType 
      prior_curr(this->m_SampleDirections->Size()); 
  ProbabilityDistributionImageType::PixelType 
      posterior_curr(this->m_SampleDirections->Size());
  TractOrientationContainerType::Element v_curr(0,0,0);
  TractOrientationContainerType::Element v_prev(0,0,0);
  
  tractPtr->Initialize();
  vnl_random randomgenerator(randomseed);
  //std::cout<<randomgenerator.drand64()<<std::endl;
  
  for(unsigned int j=0; (j<this->m_MaxTractLength) &&
    (dwiimagePtr->GetLargestPossibleRegion().IsInside(cindex_curr));
    j++){
    tractPtr->AddVertex(cindex_curr);
    
    this->ProbabilisticallyInterpolate( randomgenerator, cindex_curr,
                                      index_curr );
    
    //End the tracking if pixel is not valid according to the mask image
    if(maskimagePtr->GetPixel( index_curr ) == 0 ){
      std::cout<<"Stopped Tracking: invalid voxel\n";
      break;
    }
    
    this->CalculatePrior( v_prev, this->m_SampleDirections, prior_curr);
    
    const ProbabilityDistributionImageType::PixelType&
      cachelikelihood_curr = this->AccessLikelihoodCache(index_curr);
                            
    if( cachelikelihood_curr.GetSize() != 0){
      //use the cached direction
      this->CalculatePosterior( cachelikelihood_curr, prior_curr, posterior_curr);
    }
    else{
      //do the likelihood calculation and discard
      //std::cout<<"Cache Miss!\n";
      ProbabilityDistributionImageType::PixelType 
        likelihood_curr_temp(this->m_SampleDirections->Size());

      this->CalculateLikelihood(static_cast< DWIVectorImageType::PixelType >(
        dwiimagePtr->GetPixel(index_curr)),
        this->m_SampleDirections,
        likelihood_curr_temp);
      this->CalculatePosterior( likelihood_curr_temp, prior_curr, posterior_curr);
    }
    this->SampleTractOrientation(randomgenerator, posterior_curr,
      this->m_SampleDirections, v_curr);
    
    //takes into account voxels of different sizes
    //converts from a step length of 1 mm to the corresponding length in IJK space
    const typename InputDWIImageType::SpacingType& spacing = dwiimagePtr->GetSpacing();
    cindex_curr[0]+=v_curr[0]/spacing[0];
    cindex_curr[1]+=v_curr[1]/spacing[1];
   cindex_curr[2]+=v_curr[2]/spacing[2];
//    cindex_curr[0]+=v_curr[0];
//    cindex_curr[1]+=v_curr[1];
 //   cindex_curr[2]+=v_curr[2];
    v_prev=v_curr;
  }
}
template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
void
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::GenerateData(){
  //preparations
  this->BeforeGenerateData();
  
  //allocate outputs
  this->AllocateOutputs();
  
  typename OutputConnectivityImageType::Pointer outputPtr = this->GetOutput();
  outputPtr->FillBuffer(0);

  //get a random seed
  this->m_RandomSeed = ((unsigned long) this->m_ClockPtr->GetTimeStamp())%10000;
  std::cout<<"RandomSeed: "<<this->m_RandomSeed<<std::endl;

  //setup the multithreader
  StochasticTractGenerationCallbackStruct data;
  data.Filter = this;
  this->GetMultiThreader()->SetSingleMethod( StochasticTractGenerationCallback,
    &data );
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  std::cout<<"Number of Threads: " << this->GetMultiThreader()->GetNumberOfThreads() << std::endl; 
  //start the multithreaded execution
  this->GetMultiThreader()->SingleMethodExecute();
  std::cout<< "CurrentLikelihoodCacheElements: " << 
    this->m_CurrentLikelihoodCacheElements << std::endl; 
}

// Callback routine used by the threading library. This routine just calls
// the ThreadedGenerateData method after setting the correct region for this
// thread. 
template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
ITK_THREAD_RETURN_TYPE 
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::StochasticTractGenerationCallback( void *arg )
{
  StochasticTractGenerationCallbackStruct* str=
    (StochasticTractGenerationCallbackStruct *)
      (((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  typename InputDWIImageType::ConstPointer inputDWIImagePtr = str->Filter->GetInput();
  typename InputMaskImageType::ConstPointer inputMaskImagePtr = str->Filter->GetMaskImageInput();
  typename OutputConnectivityImageType::Pointer outputPtr = str->Filter->GetOutput();
  
  typedef PathIterator< OutputConnectivityImageType, PathType > OutputPathIteratorType;
  unsigned int tractnumber=-1;
  PathType::Pointer tractPtr = PathType::New();
  
  while(str->Filter->ObtainTractNumber(tractnumber)){
    
    //generate the tract
    str->Filter->StochasticTractGeneration( inputDWIImagePtr,
      inputMaskImagePtr,
      str->Filter->GetSeedIndex(),
      str->Filter->m_RandomSeed*tractnumber,
      tractPtr);
      
    //write the tract to output image
    str->Filter->m_OutputImageMutex.Lock();
    OutputPathIteratorType outputtractIt( outputPtr,
        tractPtr );
    for(outputtractIt.GoToBegin(); !outputtractIt.IsAtEnd(); ++outputtractIt){
      /* there is an issue using outputtractIt.Value() */
      outputtractIt.Set(outputtractIt.Get()+1);
    }
//    std::cout<<tractnumber;
//      "CurrentLikelihoodCacheElements: " << 
//        str->Filter->m_CurrentLikelihoodCacheElements << std::endl;
    
    str->Filter->m_OutputImageMutex.Unlock();

  }
  
  return ITK_THREAD_RETURN_VALUE;
}

/** Thread Safe Function to check/update an entry in the likelihood cache **/
template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
ProbabilityDistributionImageType::PixelType&
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::AccessLikelihoodCache( typename InputDWIImageType::IndexType index ){
  this->m_LikelihoodCacheMutexImagePtr->GetPixel(index).Lock();
  
  ProbabilityDistributionImageType::PixelType& likelihood = 
    m_LikelihoodCachePtr->GetPixel( index );
  typename InputDWIImageType::ConstPointer inputDWIImagePtr = this->GetInput();
  
  if( likelihood.GetSize() !=0){
    //entry found in cache
    this->m_LikelihoodCacheMutexImagePtr->GetPixel(index).Unlock();
    return likelihood;
  }
  //we need to lock m_CurrentLikelihoodCacheElements as well but not crucial right now
  else if( this->m_CurrentLikelihoodCacheElements < this->m_MaxLikelihoodCacheElements ){
    //entry not found in cache but we have space to store it
    likelihood.SetSize(this->m_SampleDirections->Size());

    this->CalculateLikelihood(static_cast< DWIVectorImageType::PixelType >(
      inputDWIImagePtr->GetPixel(index)),
      this->m_SampleDirections,
      likelihood);
    this->m_CurrentLikelihoodCacheElements++;
    
    this->m_LikelihoodCacheMutexImagePtr->GetPixel(index).Unlock();
    return likelihood;
  }
  else{
    //entry not found in cache and no space to store it
    this->m_LikelihoodCacheMutexImagePtr->GetPixel(index).Unlock();
    return likelihood;
  }
  this->m_LikelihoodCacheMutexImagePtr->GetPixel(index).Unlock();
}

/** Thread Safe Function to obtain a tractnumber to start tracking **/
template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
bool
StochasticTractographyFilter< TInputDWIImage, TInputMaskImage, TOutputConnectivityImage >
::ObtainTractNumber(unsigned int& tractnumber){
  bool success = false;
  this->m_TotalDelegatedTractsMutex.Lock();
  if(this->m_TotalDelegatedTracts < this->m_TotalTracts){
    tractnumber = m_TotalDelegatedTracts;
    this->m_TotalDelegatedTracts++;
    success = true;
    //a tract was successfully delegated
  }
  else success = false; //all tracts have been delegated
  this->m_TotalDelegatedTractsMutex.Unlock();
  
  return success;
}

}
