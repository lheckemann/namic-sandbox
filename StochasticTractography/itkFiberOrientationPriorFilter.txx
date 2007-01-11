#include "itkFiberOrientationPriorFilter.h"

namespace itk{
template< class TFiberOrientationPriorImage >
FiberOrientationPriorFilter< TFiberOrientationPriorImage >
::FiberOrientationPriorFilter(){
  this->Setsampledirections( NULL );
  this->SetprevOrientation( FiberOrientationType(0,0,0) );
}

template< class TFiberOrientationPriorImage >
void
FiberOrientationPriorFilter< TFiberOrientationPriorImage >
::GenerateData( void ){
  FOPImagePointer outputPtr = this->GetOutput();
  typedef ImageRegionIterator< FOPImageType > FOPImageIteratorType;
  FOPImageIteratorType outputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  for( outputIt.GoToBegin(); !outputIt.IsAtEnd(); ++outputIt ){

    for(int i=0; i < this->Getsampleddirections.Size(); i++){
      if((this->GetprevOrientation()).squared_magnitude()==0){
        (outputIt.Value()).InsertElement(i,1.0);
      }
      else{
        (outputIt.Value()).InsertElement(i, 
          dot_product((this->Getsampledirections()).GetElement(i),
          this->GetprevOrientation()));
      }
    }  
  }
}

}
