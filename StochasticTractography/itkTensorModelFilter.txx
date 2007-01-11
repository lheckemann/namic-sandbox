#include "itkTensorModelFilter.h"
#include "itkVariableSizeMatrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_math.h"
#include "vnl/algo/vnl_qr.h"
#include "vnl/algo/vnl_svd.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkVariableLengthVector.h"
#include <iostream>

namespace itk{

//Constructor
template< class TDWIVectorImage, class TDiffusionTensorImage >
TensorModelFilter< TDWIVectorImage, TDiffusionTensorImage >
::TensorModelFilter(){
  //don't need to allocate the VectorContainers 
  //since they should be already allocated outside of the class
  //the memory will automatically handled since they are itk smart classes
  this->SetbValues( NULL );
  this->SetGradients( NULL );
}

template< class TDWIVectorImage, class TDiffusionTensorImage >
void
TensorModelFilter< TDWIVectorImage, TDiffusionTensorImage >
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

template< class TDWIVectorImage, class TDiffusionTensorImage >
void
TensorModelFilter< TDWIVectorImage, TDiffusionTensorImage >
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
    GradientDirectionContainerType::Element g = (this->GetGradients())->GetElement(j);
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

template< class TDWIVectorImage, class TDiffusionTensorImage >
void
TensorModelFilter< TDWIVectorImage, TDiffusionTensorImage >
::CalculateTensorModelParameters( const VariableLengthVector< double >& dwivalues,
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
//This is the main part the filter.  Does a LS estimation of the tensor parameters from
//the DWI measurements
//Get the Residual Image
template< class TDWIVectorImage, class TDiffusionTensorImage >
void
TensorModelFilter< TDWIVectorImage, TDiffusionTensorImage >
::GenerateData( void ){
  //preparations
  this->BeforeGenerateData();
  
  //allocate outputs
  this->AllocateOutputs();
  
  //Get the DWI data
  //we should only get the images once, casting may be expensive
  //create a pointer to the input images
  DWIVectorImageConstPointer inputPtr = this->GetInput();
  
  //Get Output Image
  //?? If the filter can modify the image returned by GetTensor/Residual Image can
  //  other filters do likewise?  It seems downstream filters should only have readonly
  // access to upstream filter outputs
  DTImagePointer outputPtr = this->GetOutput();
  
  //create an iterator to iterate over the region of the DWI Vector image requested
  //from the output 
   typedef ImageRegionConstIterator< DWIVectorImageType > DWIVectorImageIteratorType;
    DWIVectorImageIteratorType inputIt( inputPtr, outputPtr->GetRequestedRegion() ); 

  //Create iterator for tensor image
  typedef ImageRegionIterator< DTImageType > DTImageIteratorType;
  DTImageIteratorType outputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  for(inputIt.GoToBegin(), outputIt.GoToBegin();
    !outputIt.IsAtEnd();
    ++outputIt, ++inputIt){
    //A data type that holds all 7 parameters of the diffusion tensor model
    TensorModelParamType q;
    
    this->CalculateTensorModelParameters( static_cast< VariableLengthVector< double > >(
                                inputIt.Get()) + vnl_math::eps,
                              q);
    
    //get a reference to the current tensor image pixel
    DTImagePixelType& D =  outputIt.Value();
    
    //set the tensor model parameters into a Diffusion tensor
    //we lose the intensity of the b0 image in the process
    D(0,0) = q[1];
    D(0,1) = q[4];
    D(0,2) = q[5];
    D(1,0) = q[4];
    D(1,1) = q[2];
    D(1,2) = q[6];
    D(2,0) = q[5];
    D(2,1) = q[6];
    D(2,2) = q[3];
  }
}

}
