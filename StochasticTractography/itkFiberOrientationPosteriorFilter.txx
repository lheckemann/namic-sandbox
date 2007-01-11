#include "itkFiberOrientationPosteriorFilter.h"

namespace itk{

template< class TFiberOrientationPriorImage, class TFiberOrientationLikelihoodImage,
        class TFiberOrientatonImage >
FiberOrientationPosteriorFilter< TFiberOrientationPriorImage, TFiberOrientationLikelihoodImage,
                      TFiberOrientationImage >
::FiberOrientationPosteriorFilter(){

}

template< class TFiberOrientationPriorImage, class TFiberOrientationLikelihoodImage,
        class TFiberOrientatonImage >
void
FiberOrientationPosteriorFilter< TFiberOrientationPriorImage, TFiberOrientationLikelihoodImage,
                      TFiberOrientationImage >
::GenerateData( void ){
  FOPImageConstPointer priorPtr = this->GetFOPImageInput();
  FOLImageConstPointer likelihoodPtr = this->GetFOLImageInput();
  FOImagePointer  outputPtr = this->GetOutput();
  
  typedef ImageRegionConstIterator<FOPImageType> FOPImageIteratorType;
  typedef ImageRegionConstIterator<FOLImageType> FOLImageIteratorType;
  typedef ImageRegionIterator<FOImageType> FOImageIteratorType;
  
  FOPImageIteratorType priorIt( priorPtr, outputPtr->GetRequestedRegion() );
  FOLImageIteratorType likelihoodIt( likelihoodPtr, outputPtr->GetRequestedRegion() );
  FOImageIteratorType outputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  for(outputIt.GoToBegin(), priorIt.GoToBegin(), likelihoodIt.GoToBegin(),
      !outputIt.IsAtEnd(),
      ++outputIt, ++priorIt, ++likelihoodIt){
    double sum=0;
    for(int i=0, i<(likelihoodIt.Value()).Size(), i++){
      sum+=(likelihoodIt.Value())[i]*(priorIt.Value())[i];
    }
    for(int i=0, i<(likelihoodIt.Value()).Size(), i++){
      (outputIt.Value()).InsertElement(i, 
        (likelihoodIt.Value())[i]*(priorIt.Value())[i]/sum);
    }
  }
}

}
