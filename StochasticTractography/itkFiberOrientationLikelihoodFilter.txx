#include "itkFiberOrientationLikelihoodFilter.h"
#include "vnl/vnl_math.h"

namespace itk{

template< class TDWIVectorImage, class TResidualVarianceImage, class TConstrainedParamImage,
      class TFiberOrientationLikelihoodImage >
FiberOrientationLikelihoodFilter< TDWIVectorImage, TResidualVarianceImage, TConstrainedParamImage,
                        TFiberOrientationLikelihoodImage >
::FiberOrientationLikelihoodFilter(){
  /** instantiate the filters **/
  m_cmtdwifilter = CMTDWIFilterType::New();
  m_revifilter = REVIFilterType::New();
  
  /** Setup the minipipeline **/
  m_cmtdwifilter->SetInput(m_revifilter->SetOutput());
}

template< class TDWIVectorImage, class TResidualVarianceImage, class TConstrainedParamImage,
      class TFiberOrientationLikelihoodImage >
void
FiberOrientationLikelihoodFilter< TDWIVectorImage, TResidualVarianceImage, TConstrainedParamImage,
                        TFiberOrientationLikelihoodImage >
::GenerateData( void ){
  /** Get Output and create associated iterator**/
  FOLImagePointer outputPtr = this->GetOutput();
  typedef ImageRegionIterator< FOLImageType > FOLImageIteratorType;
  FOLImageIteratorType outputIt( outputPtr, outputPtr->GetRequestedRegion() );
  
  /** Get Actual DWI Intensity and create associated iterator **/
  DWIVImageConstPointer actualdwiPtr = this->GetActualDWI();
  typedef ImageRegionConstIterator< DWIVImageType > DWIVImageIteratorType;
  DWIVImageIteratorType actualdwiIt( actualdwiPtr, outputPtr->GetRequestedRegion() );
  
  /** Get the residual variance input image and create associated iterator **/
  RVImageConstPointer residualvariancePtr = this->GetResidualVarianceInput();
  typedef RVPointerConstPointer< ResidualVarianceImageType > RVImageIteratorType;
  RVImageIteratorType residualvarianceIt( residualvariancePtr, 
                             outputPtr->GetRequestedRegion());
                                                        
  /** connect the constrained param input into the REVFilter **/
  m_revifilter->SetInput(this->GetConstrainedParamInput());
  
  
  
  /**Get Estimated Intensity Image from the CMTDWIFilter  and
    create associated iterator **/
  DWIVImageConstPointer estimatedDWIPtr = m_cmtdwiFilter->GetOutput();
  typedef ImageRegionConstIterator< DWIVImageType > DWIVImageIteratorType;
  DWIVImageIteratorType estimatedDWIIt(estimatedDWIPtr, outputPtr->GetRequestedRegion());


  for(outputIt.GoToBegin(), estimatedDWIIt.GoToBegin(), residualvarianceIt.GoToBegin(),
      actualdwiIt.GoToBegin(); 
      !outputIt.IsAtEnd();
      ++outputIt, ++estimatedDWIIt, ++residualvarianceIt, ++actualdwiIt ){
    
    for(int i=0; i < this->Getsampledirections.Size(); i++){
      /** Vary the entry corresponding to the estimated
        fiber orientation over the selected sample directions,
        while preserving the best estimate for the other parameters **/
      FiberOrientationType currentdir = (this->Getsampledirections())->GetElement(i);
      
      /** Incorporate the current sample direction into the secondary parameters **/
      m_revifilter->AddEntryReplacement(3, currentdir[0]);
      m_revifilter->AddEntryReplacement(4, currentdir[1]);
      m_revifilter->AddEntryReplacement(5, currentdir[2]);
      
      /** Update the constrainedtodwi filter to obtain the estimated
        intensity for this choice of fiber direction **/
      m_cmtdwifilter->Update();
      
      double jointlikelihood = 1.0;
      for(int j=0; j<(estimatedDWIIt.Value()).Size(); j++){
        /** Calculate the likelihood given the residualvariance,
          estimated intensity and the actual intensity (refer to Friman) **/
        jointlikelihood *= 
          ((estimatedDWIIt.Value())[j]*vnl_math::two_over_sqrtpi*vnl_math::sqrt1_2*0.5)*
            vcl_exp(-0.5*((estimatedDWIIt.Value())[j]/(residualvarianceIt.Value())[j])*
              vnl_math_sqr((actualdwiIt.Value())[j]-vcl_log((estimatedDWIIt.Value())[j])));
      }
      (outputIt.Value()).InsertElement(i, jointlikelihood);  
    }
  }
}

}
