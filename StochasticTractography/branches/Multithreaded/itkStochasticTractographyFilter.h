#ifndef __itkStochasticTractographyFilter_h__
#define __itkStochasticTractographyFilter_h__

#include "itkImageToImageFilter.h"
#include "vnl/vnl_random.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"
#include "itkArray.h"
#include "itkVectorContainer.h"
#include "vnl/algo/vnl_qr.h"
#include "itkVariableLengthVector.h"
#include "itkSlowPolyLineParametricPath.h"
#include "itkSimpleFastMutexLock.h"
#include <vector>

namespace itk{

/**Types for Probability Distribution **/
typedef Image< Array< double >, 3 > ProbabilityDistributionImageType;

template< class TInputDWIImage, class TInputMaskImage, class TOutputConnectivityImage >
class ITK_EXPORT StochasticTractographyFilter :
  public ImageToImageFilter< TInputDWIImage,
                    TOutputConnectivityImage >{
public:
  typedef StochasticTractographyFilter Self;
  typedef ImageToImageFilter< TInputDWIImage,
    TOutputConnectivityImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro( StochasticTractographyFilter,
            ImageToImageFilter );
  
  /** Types for the DWI Input Image **/
  typedef TInputDWIImage InputDWIImageType;
  
  /** Types for the Connectivity Output Image**/
  typedef TOutputConnectivityImage OutputConnectivityImageType;
  
  /** Types for the Mask Image **/
  typedef TInputMaskImage InputMaskImageType;
  
  /** Types for the Image-wide Magnetic Field Gradient Directions **/
  typedef VectorContainer< unsigned int, vnl_vector_fixed< double, 3 > >
    GradientDirectionContainerType;
  
  /** Types for the Image-wide bValues **/
  typedef double bValueType;
  typedef VectorContainer< unsigned int, bValueType > bValueContainerType;
   
  /** Types for the Measurement Frame of the Gradients **/
  typedef vnl_matrix_fixed< double, 3, 3 > MeasurementFrameType;

  /** Type for the sample directions **/
  typedef VectorContainer< unsigned int, vnl_vector_fixed< double, 3 > > 
    TractOrientationContainerType;
  
  /** the number of Tracts to generate **/
  itkSetMacro( TotalTracts, unsigned int);
  itkGetMacro( TotalTracts, unsigned int);
  
  /** the maximum length of Tract **/
  itkSetMacro( MaxTractLength, unsigned int );
  itkGetMacro( MaxTractLength, unsigned int );
  
  /** Set/Get bvalues **/
  itkSetConstObjectMacro( bValues, bValueContainerType );
  itkGetConstObjectMacro( bValues, bValueContainerType );
  
  /** Set/Get of gradient directions **/
  itkSetConstObjectMacro( Gradients, GradientDirectionContainerType );
  itkGetConstObjectMacro( Gradients, GradientDirectionContainerType );
  
  /** Set/Get the Mask Input image **/
  // If the user does not set one, the filter will
  // create a default one that specifies the
  // whole image as valid
  // 0 indicates an invalid region
  // values > 0 indicate valid regions
  itkSetInputMacro(MaskImage, InputMaskImageType, 1);
  itkGetInputMacro(MaskImage, InputMaskImageType, 1);
  
  //overide the built in set input function
  //we need to create a new cache everytime we change the input image
  //but we need to preserve it when the input image is the same
  void SetInput( typename InputDWIImageType::Pointer dwiimagePtr ){
    Superclass::SetInput( dwiimagePtr );
    //update the likelihood cache
    this->m_LikelihoodCachePtr = ProbabilityDistributionImageType::New();
    this->m_LikelihoodCachePtr->CopyInformation( this->GetInput() );
    this->m_LikelihoodCachePtr->SetBufferedRegion( this->GetInput()->GetBufferedRegion() );
    this->m_LikelihoodCachePtr->SetRequestedRegion( this->GetInput()->GetRequestedRegion() );
    this->m_LikelihoodCachePtr->Allocate();
    this->m_CurrentLikelihoodCacheSize = 0;
  }
  
  /** Set/Get the seed index **/
  itkSetMacro( SeedIndex, typename InputDWIImageType::IndexType );
  itkGetMacro( SeedIndex, typename InputDWIImageType::IndexType );
  
  /** Set/Get the list of directions to sample **/
  itkSetConstObjectMacro( SampleDirections, TractOrientationContainerType );
  itkGetConstObjectMacro( SampleDirections, TractOrientationContainerType );
  
  /** Set/Get the Measurement Frame **/
  itkSetMacro( MeasurementFrame, MeasurementFrameType );
  itkGetMacro( MeasurementFrame, MeasurementFrameType );
  
  /** Set/Get the Maximum Likelihood Cache Size, the max num. of cached voxels **/
  itkSetMacro( MaxLikelihoodCacheSize, unsigned int );
  itkGetMacro( MaxLikelihoodCacheSize, unsigned int );
            
  /** Get the current Likelihood Cache Size, i.e. the total unique cached pixels **/
  itkGetMacro( CurrentLikelihoodCacheSize, unsigned int );
  void GenerateData();
  
protected:
  /** Convenience Types used only inside the filter **/
    
  /**Types for the parameters of the Tensor Model **/
  typedef vnl_vector_fixed< double, 7 > TensorModelParamType;
  
  /**Types for the parameters of the Constrained Model **/
  typedef vnl_vector_fixed< double, 6 > ConstrainedModelParamType;
  
  /**Type to hold generated DWI values**/
  typedef Image< VariableLengthVector< double >, 3 > DWIVectorImageType;
  
  /** Path Types **/
  typedef SlowPolyLineParametricPath< 3 > PathType;
  
  /**Types for Probability Distribution **/
  typedef Image< Array< double >, 3 > ProbabilityDistributionImageType;
  
  StochasticTractographyFilter();
  virtual ~StochasticTractographyFilter();
  
  /** Classwide Data members **/
  GradientDirectionContainerType::ConstPointer m_Gradients;
  bValueContainerType::ConstPointer m_bValues;
  MeasurementFrameType m_MeasurementFrame;
  
  /** Preparatory Steps (optimizations) before running algorithm **/
  void BeforeGenerateData(void){
    this->UpdateGradientDirections();
    this->UpdateTensorModelFittingMatrices();
    this->m_TotalDelegatedTracts = 0;
  }
  
  /** Load the default Sample Directions**/
  void LoadDefaultSampleDirections( void );
  
  /** Primary steps in the algorithm **/
  void ProbabilisticallyInterpolate( vnl_random& randomgenerator, 
      const PathType::ContinuousIndexType& cindex,
      typename InputDWIImageType::IndexType& index);
                      
  /** Functions and data related to fitting the tensor model at each pixel **/
  void UpdateGradientDirections(void);
  void UpdateTensorModelFittingMatrices( void );
  void CalculateTensorModelParameters( const DWIVectorImageType::PixelType& dwivalues,
    vnl_diag_matrix<double>& W,
    TensorModelParamType& tensormodelparams);
  //these will be the same for every pixel in the image so
  //go ahead and do a QR decomposition to optimize the
  //LS fitting process for estimating the weighing matrix W
  //in this case we solve instead:
  //R*Beta = Q'logPhi
  vnl_matrix< double >* m_A;
  vnl_qr< double >* m_Aqr;    
  
  void CalculateConstrainedModelParameters( const TensorModelParamType& tensormodelparams,
    ConstrainedModelParamType& constrainedmodelparams);
  
  void CalculateNoiseFreeDWIFromConstrainedModel( const ConstrainedModelParamType& constrainedmodelparams,
    DWIVectorImageType::PixelType& noisefreedwi);
  
  void CalculateResidualVariance( const DWIVectorImageType::PixelType& noisydwi,
    const DWIVectorImageType::PixelType& noisefreedwi,
    const vnl_diag_matrix< double >& W,
    const unsigned int numberofparameters,
    double& residualvariance);
  
  void CalculateLikelihood( const DWIVectorImageType::PixelType &dwipixel, 
    TractOrientationContainerType::ConstPointer orientations,
    ProbabilityDistributionImageType::PixelType& likelihood);
  
  void CalculatePrior( TractOrientationContainerType::Element v_prev, 
    TractOrientationContainerType::ConstPointer orientations,
    ProbabilityDistributionImageType::PixelType& prior );
  
  void CalculatePosterior( const ProbabilityDistributionImageType::PixelType& likelihood,
    const ProbabilityDistributionImageType::PixelType& prior,
    ProbabilityDistributionImageType::PixelType& posterior);       
  
  void SampleTractOrientation( vnl_random& randomgenerator, 
    const ProbabilityDistributionImageType::PixelType& posterior,
    TractOrientationContainerType::ConstPointer orientations,
    TractOrientationContainerType::Element& choosendirection );
  
  void StochasticTractGeneration( typename InputDWIImageType::ConstPointer dwiimagePtr,
    typename InputMaskImageType::ConstPointer maskimagePtr,
    typename InputDWIImageType::IndexType seedindex,
    unsigned int tractnumber,
    PathType::Pointer tractPtr );
  
  /** This function is called by the multithreader **/
  static ITK_THREAD_RETURN_TYPE StochasticTractGenerationCallback( void *arg );
  
  struct StochasticTractGenerationCallbackStruct{
    Pointer Filter;
  };
  /** Thread Safe Function to check/update an entry in the likelihood cache **/
  ProbabilityDistributionImageType::PixelType& 
    AccessLikelihoodCache( typename InputDWIImageType::IndexType index );
  /** Thread Safe Function to obtain a tractnumber to start tracking **/
  bool ObtainTractNumber(unsigned int& tractnumber);
  
  ProbabilityDistributionImageType::Pointer m_LikelihoodCachePtr;
  unsigned int m_MaxTractLength;
  unsigned int m_TotalTracts;
  typename InputDWIImageType::IndexType m_SeedIndex;
  TractOrientationContainerType::ConstPointer m_SampleDirections;
  unsigned int m_MaxLikelihoodCacheSize;
  unsigned int m_CurrentLikelihoodCacheSize;
  SimpleFastMutexLock m_LikelihoodCacheMutex;
  
  unsigned int m_TotalDelegatedTracts;
  SimpleFastMutexLock m_TotalDelegatedTractsMutex;
  
  SimpleFastMutexLock m_OutputImageMutex;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkStochasticTractographyFilter.txx"
#include "itkStochasticTractographyFilter_SD.txx"
#endif

#endif