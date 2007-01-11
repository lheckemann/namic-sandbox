#include "itkConstrainedModelToDWIFilter.h"
#include "vnl/vnl_math.h"

namespace itk{

template< class TConstrainedParamImage,
        class TDWIVectorImage >
ConstrainedModelToDWIFilter< TConstrainedParamImage,
                    TDWIVectorImage>
::ConstrainedModelToDWIFilter(){
  this->Setgradients(NULL);
  this->Setbvalues(NULL);
}

void UpdateCoefficientMatrix(){
  /** fill the matrices and vectors according to an expansion of the
    constrained model as described by friman
    log(\mathbf(z_i)) = [1 -b_i -b_i*g_x^2 -b_i*g_y^2 -b_i*g_z^2]*
                  [log(z_0) \alpha \beta*v_x^2 \beta*v_y^2 \beta*v_z^2]
    fill the A matrix holding gradients and bvalues **/
    
  this->SetN( (this->Getgradients())->Size() );
  
  A.set_size( N, 5 );
  for(int j=0; j< N ; j++){
    GradientDirectionType g = Getgradients()->GetElement(j);
    bValueType  b = GetbValues->GetElement(j);
    
    vnlA.put(j,0,1.0);
    vnlA.put(j,1,-b);
    vnlA.put(j,2,-b*(g[0]*g[0]));
    vnlA.put(j,3,-b*(g[1]*g[1]));
    vnlA.put(j,4,-b*(g[2]*g[2]));
  }
  
}

template< class TTensorParamImage,
        class TConstrainedParamImage >
void
ConstrainedModelToDWIFilter< TConstrainedParamImage,
                    TTensorParamImage>
::GenerateData( void ){
  ConstrainedParamImageConstPointer  inputPtr = this->GetInput();
  TensorParamImagePointer outputPtr = this->GetOutput();
  
  typedef ImageRegionConstIterator< ConstrainedParamImageType > ConstrainedParamImageIteratorType;
  ConstrainedParamImageIteratorType inputIt(inputPtr, outputPtr->GetRequestedRegion());
  
  typedef ImageRegionIterator< TensorParamImageType > TensorParamImageIteratorType;
  
  vnl_vector_fixed< double, 5 > q;
  vnl_vector< double > z;
  
  TensorParamImageIteratorType ouputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  for(inputIt.GoToBegin(), outputIt.GoToBegin();
    !outputIt.IsAtEnd();
    ++outputIt, ++inputIt){
    q[0]=inputIt.Value()[0];
    q[1]=inputIt.Value()[1];
    q[2]=inputIt.Value()[2]*inputIt.Value()[3]*inputIt.Value()[3];
    q[3]=inputIt.Value()[2]*inputIt.Value()[4]*inputIt.Value()[4];
    q[4]=inputIt.Value()[2]*inputIt.Value()[5]*inputIt.Value()[5];
    
    z=A*q;
    
    for(int i=0; i < this->GetN() ; i++ )
      (outputIt.Value()).SetElement(i, vcl_exp(z[i]));
  }
    
}

}
