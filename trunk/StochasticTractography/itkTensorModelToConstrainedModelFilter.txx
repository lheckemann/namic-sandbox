#include "itkTensorModelToConstrainedModelFilter.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_symmetric_eigensystem.h"

namespace itk{
template< class TTensorParamImage,
        class TConstrainedParamImage >
void
TensorModelToConstrainedModelFilter< TTensorParamImage,
                TConstrainedParamImage >
::GenerateData( void ){
  typename TensorParamImageConstPointer inputPtr = this->GetInput();
  typename ConstrainedParamImagePointer outputPtr = this->GetOutput();

  ImageRegionConstIterator< TensorParamImageType > 
    inputIt(inputPtr, outputPtr->GetRequestedRegion());
  ImageRegionIterator< ConstrainedParamImageType > 
    constrainedparamimageIt(ouputPtr, outputPtr->GetRequestedRegion());
  
  vnl_matrix_fixed< double, 3, 3 > D;
  double alpha,beta;
  
  for(inputIt.GoToBegin(), outputIt.GoToBegin();
    !outputIt.IsAtEnd();
    ++outputIt, ++inputIt){
    //set the tensor model parameters into a Diffusion tensor
    D(0,0) = inputIt.Get()[1];
    D(0,1) = inputIt.Get()[4];
    D(0,2) = inputIt.Get()[5];
    D(1,0) = inputIt.Get()[4];
    D(1,1) = inputIt.Get()[2];
    D(1,2) = inputIt.Get()[6];
    D(2,0) = inputIt.Get()[5];
    D(2,1) = inputIt.Get()[6];
    D(2,2) = inputIt.Get()[3];
    
    //pass through the no gradient intensity Z0 and
    //calculate alpha, beta and v hat (the eigenvector 
    //associated with the largest eigenvalue)
    vnl_symmetric_eigensystem< double >(D) Deig;
    
    alpha = (Deig.get_eigenvalue(2) + Deig.get_eigenvalue(3)) / 2;
    beta = Deig.get_eigenvalue(1) - alpha;
    
    (outputIt.Value()).SetElement(0, inputIt.Get()[0]);
    (outputIt.Value()).SetElement(1, alpha);
    (outputIt.Value()).SetElement(2, beta);
    (outputIt.Value()).SetElement(3, (Deig.get_eigenvector(0))[0]);
    (outputIt.Value()).SetElement(4, (Deig.get_eigenvector(0))[1]);
    (outputIt.Value()).SetElement(5, (Deig.get_eigenvector(0))[2]);  
  }
};

}
