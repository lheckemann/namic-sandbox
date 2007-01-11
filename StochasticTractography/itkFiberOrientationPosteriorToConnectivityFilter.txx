#include "itkFiberOrientationPosteriorToConnectivityFilter.h"
#include "vnl_math.h"

namespace itk{

template< class TFiberOrientationPosteriorImage, class TConnectivityImage >
FiberOrientationPosteriorToConnectivityFilter< TFiberOrientationPosteriorImage,
                                TConnectivityImage >
::FiberOrientationPosteriorToConnectivityFilter(){
  /**Set the default number of fibers **/
  this->SetTotalTracts( 1000 );
  this->SetMaxTractLength( 500 );
}

template< class TFiberOrientationPosteriorImage, class TConnectivityImage >
FOPImageIndexType
FiberOrientationPosteriorToConnectivityFilter< TFiberOrientationPosteriorImage,
                                TConnectivityImage >
::ProbabilisticallyInterpolate( vnl_random& randomgenerator, 
                      PathContinuousIndexType cindex){
  FOPImageIndexType index;
  for(int i=0; i<3; i++){
    if ((vcl_ceil(cindex[i]+vnl_math::eps)-cindex[i]) < randomgenerator.drand32())
       index[i]=vcl_ceil(cindex[i]);
     else index[i]=vcl_floor(cindex[i]);
  }
  
  return index;
}

template< class TFiberOrientationPosteriorImage, class TConnectivityImage >
FiberOrientationType
FiberOrientationPosteriorToConnectivityFilter< TFiberOrientationPosteriorImage,
                                TConnectivityImage >
::SampleFiberOrientation( vnl_random& randomgenerator, 
                  FOPImagePixelType pixel, 
                  FOContainerPointer orientations ){
      double randomnum = randomgenerator.drand32();
      int i=0;
      double cumsum=0;
      
      while(cumsum < randomnum){
        cumsum+=pixel.GetElement(i);
        i++;
      }
      return rv[i-1];
}

template< class TFiberOrientationPosteriorImage, class TConnectivityImage >
void
FiberOrientationPosteriorToConnectivityFilter< TFiberOrientationPosteriorImage,
                                TConnectivityImage >
::GenerateData(){
  FOPImagePointer inputPtr = GetInput();
  CImagePointer outputPtr = GetOutput();
  
  typedef ImageRegionIterator< CImageType > CImageIteratorType;
  typedef ImageRegionConstIterator< FOPImageType > FOPImageIteratorType;
  typedef PathIterator< FOPImageType, PathType > FOPImagePathIteratorType;
  
  CImageIteratorType inputIt( inputPtr, outputPtr->GetRequestedRegion() );
  FOPImageIteratorType outputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  PathContinuousIndexType cindex_curr;
  FOPImagePointType point_curr;
  FOPImageIndexType index_curr;
  FOPImagePixelType pixel_curr;
  FiberOrientationType v_curr, v_prev;
  
  PathType tract_curr = PathType::New();
  
  for(int i=0; i<this->GetTotalFibers(); i++){
    vnl_random randomgenerator;
    
    tract_curr->Initialize();
    point_curr=this->GetSeedPoint();
    inputPtr->TransformPhysicalPointToContinuousIndex( randomgenerator, 
      point_curr, cindex_curr );
    tract_curr->AddVertex(cindex_curr);
    
    for(int j=0; j<this->GetMaxTractLength(); j++){
      index_curr = this->ProbabilisticallyInterpolate( randomgenerator, cindex_curr );
      v_curr=this->SampleFiberOrientation(inputPtr->GetPixel( index_curr ),
                               this->GetSampleDirections());
      cindex_curr[0]+=v_curr[0];
      cindex_curr[1]+=v_curr[1];
      cindex_curr[2]+=v_curr[2];
      
      tract_curr->AddVertex(cindex_curr);
      
      //break when we are out of the volume
    }
    FOPImagePathIteratorType tractIt( outputPtr, tract_curr );
    
    for(tractIt.GoToBegin(); !tractIt.IsAtEnd(); ++tractIt){
      (tractIt.Value())++;
    }
  }
}

}
