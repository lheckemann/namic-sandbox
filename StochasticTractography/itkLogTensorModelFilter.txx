#include "itkLogTensorModelFilter.h"
#include "itkVariableSizeMatrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"
#include <iostream>

namespace itk{

//Constructor
template< class TDWIVectorImage,
        class TTensorModelParamImage >
LogTensorModelFilter< TDWIVectorImage,
              TTensorModelParamImage >
::LogTensorModelFilter(){
  //don't need to allocate the VectorContainers 
  //since they should be already allocated outside of the class
  //the memory will automatically handled since they are itk smart classes
  this->bValueContainer =  NULL;
  this->gradientDirectionContainer = NULL;
}

//This is the main part the filter.  Does a LS  estimation of the tensor parameters from
//the DWI measurements
//Get the Residual Image
template< class TDWIVectorImage,
        class TTensorModelParamImage >
void
LogTensorModelFilter< TDWIVectorImage,
              TTensorModelParamImage >
::GenerateData( void ){
  //Get the DWI data
  //we should only get the images once, casting may be expensive
  //create a pointer to the input images
  DWIVectorImageConstPointer inputPtr = this->GetInput();
  
  //Get Output Image
  //?? If the filter can modify the image returned by GetTensor/Residual Image can
  //  other filters do likewise?  It seems downstream filters should only have readonly
  // access to upstream filter outputs
  TensorModelParamImagePointer outputPtr = this->GetOutput();
  
  //create an iterator to iterate over the region of the DWI Vector image requested
  //from the output 
   typedef ImageRegionConstIterator< DWIVectorImageType > DWIVectorImageIteratorType;
    DWIVectorImageIteratorType inputIt( inputPtr, outputPtr->GetRequestedRegion() ); 

  //Create iterator for tensor image
  typedef ImageRegionIterator<TensorModelParamImageType> TensorModelParamImageIteratorType;
  TensorModelParamImageIteratorType 
    outputIt( outputPtr, outputPtr->GetRequestedRegion() );

  //estimate the parameters using linear LS estimation
  //using convention specified by Salvador
  //solve for Beta in: logPhi=X*Beta
  //number of rows of the matrix depends on the number of inputs,
  //i.e. the number of measurements of the voxel (n)
  int N = inputPtr->GetVectorLength();
  
  VariableSizeMatrix< double >  itkA( N , 7 );
  //extract out the vnl_matrix so that we have access to more matrix operations
  VariableSizeMatrix< double >::InternalMatrixType vnlA = itkA.GetVnlMatrix();
  
  //vnl_vector is used because the itk vector is limited in its methods and does not
  //contain an internal vnl class like VariableSizematrix
  //also itk_matrix has methods which are compatible with vnl_vectors
  vnl_vector< DWIVectorImagePixelType > logPhi( N );
  
  //vnl_vector_fixed used to hold the Beta vector which contains the Least Squares
  //fitted parameters for the tensor model
  //The 7 elements are the 7 parameters of the tensor model
  vnl_vector_fixed< double, 7 > Beta;
  
  //a vnl_vector_fixed used to hold the difference between the actual intensity
  //and the intensity predicted by the fitted model
  vnl_vector< double > e( N );
  
  //iterate through every pixel of vector image
  //reset the input and output image iterators to the beginning
  inputIt.GoToBegin();
  outputIt.GoToBegin();
  
  //fill the matrices and vectors according to Salvador pg 146
  //fill the A matrix holding gradients and bvalues
  
  for(int j=0; j< N ; j++){
    typename GradientDirectionType g = Getgradients()->GetElement(j);
    typename bValueType  b = GetbValues->GetElement(j);
    
    vnlA.put(j,0,1.0);
    vnlA.put(j,1,-b*(g[0]*g[0]));
    vnlA.put(j,2,-b*(g[1]*g[1]));
    vnlA.put(j,3,-b*(g[2]*g[2]));
    vnlA.put(j,4,-b*(2*g[0]*g[1]));
    vnlA.put(j,5,-b*(2*g[0]*g[2]));
    vnlA.put(j,6,-b*(2*g[1]*g[2]));
  }
  
  //these will be the same for every pixel in the image so
  //go ahead and do a QR decomposition to optimize the
  //least squares fitting process
  //in this case we solve instead:
  //R*Beta = Q'logPhi
  vnl_matrix< double > Q( N, 7 );
  vnl_matrix< double > R( 7, 7 );
  vnl_qr< double >(vnlA).extract_q_and_r( &Q, &R );
  vnl_matrix_inverse< double > Rinv(R);
  vnl_transpose< double > Qtranspose(Q);
  
  while(!outputIt.IsAtEnd()){
    //set the logPhi vector using log(dwi) values at the current pixel
    //notice the vector directly references the data in the image, no copying
    //fortunately, we will not be modifying the vector in this operation.
    //we must use Value() to access the pixel to get a reference to the raw data
    logPhi.set( (inputIt.Value()).GetDataPointer() );
    
    // Find LS estimate of Beta
    // A'A Beta = A'logPhi
    // Beta = (A'*A) A'logPhi
    // actually Beta hat since it is the estimate of the parameters of Beta
    
    //An optimization can be performed if we find the QR decomposition of A
    //before performing least squares estimation
    //The QR decomposition can be stored for use to solve all other voxels
    Beta = Rinv*Qtranspose*logPhi;
    
    (outputIt.Value()).SetElement(0, Beta[0]);
    (outputIt.Value()).SetElement(1, Beta[1]);
    (outputIt.Value()).SetElement(2, Beta[2]);
    (outputIt.Value()).SetElement(3, Beta[3]);
    (outputIt.Value()).SetElement(4, Beta[4]);
    (outputIt.Value()).SetElement(5, Beta[5]);
    (outputIt.Value()).SetElement(6, Beta[6]);
    
    ++outputIt;
    ++inputIt;
  }
}

}
