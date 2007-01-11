#include "itkFiberOrientationProbabilityToConnectivityFilter.h"
#include "vnl_math.h"

namespace itk{

template< class TFiberOrientationLikelihoodImage, class TConnectivityImage >
FiberOrientationProbabilityToConnectivityFilter< TFiberOrientationLikelihoodImage,
                                TConnectivityImage >
::FiberOrientationProbabilityToConnectivityFilter(){
  /**Set the default number of fibers **/
  this->SetTotalTracts( 1000 );
  this->SetMaxTractLength( 500 );
}

template< class TFiberOrientationLikelihoodImage, class TConnectivityImage >
FOLImageIndexType
FiberOrientationProbabilityToConnectivityFilter< TFiberOrientationLikelihoodImage,
                                TConnectivityImage >
::ProbabilisticallyInterpolate( vnl_random& randomgenerator, 
                      PathContinuousIndexType cindex){
  FOLImageIndexType index;
  for(int i=0; i<3; i++){
    if ((vcl_ceil(cindex[i]+vnl_math::eps)-cindex[i]) < randomgenerator.drand32())
       index[i]=vcl_ceil(cindex[i]);
     else index[i]=vcl_floor(cindex[i]);
  }
  
  return index;
}

template< class TFiberOrientationLikelihoodImage, class TConnectivityImage >
void
FiberOrientationProbabilityToConnectivityFilter< TFiberOrientationLikelihoodImage,
                                TConnectivityImage >
::CalculatePrior( vnl_vector v_prev, 
            FOContainerPointer orientations,
            FOLImagePixelType& prior ){
  FOLImagePixelType prior;
  
  for(int i=0; i < orientations.Size(); i++){
    if(v_prev.squared_magnitude()==0){
      prior.InsertElement(i,1.0);
    }
    else{
      prior.InsertElement(i, dot_product(orientations.GetElement(i),v_prev);
    }
  }
}

template< class TFiberOrientationLikelihoodImage, class TConnectivityImage >
void
FiberOrientationProbabilityToConnectivityFilter< TFiberOrientationLikelihoodImage,
                                TConnectivityImage >
::CalculatePosterior( const FOLImagePixelType& likelihood,
               const FOLImagePixelType& prior
               FOLImagePixelType& posterior){
    FOLImagePixelType posterior;
    
    double sum=0;
    for(int i=0, i<likelihood.Size(), i++){
      sum+=likelihood[i]*prior[i];
    }
    for(int i=0, i<likelihood.Size(), i++){
      posterior.InsertElement(i, likelihood[i]*prior[i]/sum);
    }
}

template< class TFiberOrientationLikelihoodImage, class TConnectivityImage >
FiberOrientationType
FiberOrientationProbabilityToConnectivityFilter< TFiberOrientationLikelihoodImage,
                                TConnectivityImage >
::SampleFiberOrientation( vnl_random& randomgenerator, 
                  const FOLImagePixelType& posterior,
                  FOContainerPointer orientations ){
      double randomnum = randomgenerator.drand32();
      int i=0;
      double cumsum=0;
      
      while(cumsum < (randomnum + vnl_math::eps)){
        cumsum+=posterior.GetElement(i);
        i++;
      }
      return orientations[i-1];
}

template< class TFiberOrientationLikelihoodImage, class TConnectivityImage >
void
FiberOrientationProbabilityToConnectivityFilter< TFiberOrientationLikelihoodImage,
                                TConnectivityImage >
::GenerateData(){
  FOLImagePointer inputPtr = GetInput();
  CImagePointer outputPtr = GetOutput();
  
  outputPtr->FillBuffer(0);
  typedef ImageRegionIterator< CImageType > CImageIteratorType;
  typedef ImageRegionConstIterator< FOLImageType > FOLImageIteratorType;
  typedef PathIterator< FOLImageType, PathType > FOLImagePathIteratorType;
  
  CImageIteratorType inputIt( inputPtr, outputPtr->GetRequestedRegion() );
  FOLImageIteratorType outputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  PathContinuousIndexType cindex_curr;
  FOLImagePointType point_curr;
  FOLImageIndexType index_curr;


  
  PathType tract_curr = PathType::New();
  
  for(int i=0; i<this->GetTotalFibers(); i++){
    vnl_random randomgenerator;
    FiberOrientationType v_curr(0,0,0);
    FiberOrientationType v_prev(0,0,0);
    
    tract_curr->Initialize();
    point_curr=this->GetSeedPoint();
    inputPtr->TransformPhysicalPointToContinuousIndex( randomgenerator, 
      point_curr, cindex_curr );
    
    for(int j=0; (j<this->GetMaxTractLength()) &&
      (!inputPtr->GetLargestPossibleRegion().IsInside(cindex_curr)); j++){
      FOLImagePixelType prior_curr, posterior_curr;
      
      tract_curr->AddVertex(cindex_curr);
      
      index_curr = this->ProbabilisticallyInterpolate( randomgenerator, cindex_curr );
      this->CalculatePrior( v_prev, this->GetSampleDirections(), prior_curr);
      this->CalculatePosterior( inputPtr->GetPixel( index_curr ), posterior_curr);
      v_curr=this->SampleFiberOrientation(randomgenerator, posterior_curr,
                                this->GetSampleDirections());
            
      cindex_curr[0]+=v_curr[0];
      cindex_curr[1]+=v_curr[1];
      cindex_curr[2]+=v_curr[2];
      v_prev=v_curr;

    }
    FOLImagePathIteratorType tractIt( outputPtr, tract_curr );

    for(tractIt.GoToBegin(); !tractIt.IsAtEnd(); ++tractIt){
      (tractIt.Value())++;
    }
    
    for(outputIt.GoToBegin(); outputIt.IsAtEnd(); ++outputIt){
      (outputIt.Value())/=this->GetTotalFibers();
    }
  }
}

}
