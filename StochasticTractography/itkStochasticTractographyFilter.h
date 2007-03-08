#ifndef __itkStochasticTractographyFilter_h__
#define __itkStochasticTractographyFilter_h__

#include "itkImageToImageFilter.h"
#include "vnl/vnl_random.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix.h"
#include "itkArray.h"
#include "itkVectorContainer.h"
#include "itkPolyLineParametricPath.h"
#include "vnl/algo/vnl_qr.h"
#include "itkVariableLengthVector.h"
#include <vector>

namespace itk{

template< class TInputDWIImage, class TOutputConnectivityImage >
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
  itkGetConstObjectMacro( Gradients, GradientDirectionContainerType );;
  
  //overide the built in set input function
  //we need to create a new cache everytime we change the input image
  //but we need to preserve it when the input image is the same
  void SetInput( typename TInputDWIImage::Pointer dwiimage ){
    Superclass::SetInput( dwiimage );
    //update the likelihood cache
    this->m_LikelihoodCache = ProbabilityDistributionImageType::New();
    this->m_LikelihoodCache->CopyInformation( this->GetInput() );
    this->m_LikelihoodCache->SetBufferedRegion( this->GetInput()->GetBufferedRegion() );
    this->m_LikelihoodCache->SetRequestedRegion( this->GetInput()->GetRequestedRegion() );
    this->m_LikelihoodCache->Allocate();
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
  
  /**Types for Probability Distribution **/
  typedef Image< Array< double >, 3 > ProbabilityDistributionImageType;
  
  /** Path Types **/
  typedef PolyLineParametricPath< 3 > PathType;
  
  /** Instantiate a Probability Distribution Image for the Cache **/
  typename ProbabilityDistributionImageType::Pointer m_LikelihoodCache;
  
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
    TensorModelParamType& tensormodelparams);
  //these will be the same for every pixel in the image so
  //go ahead and do a QR decomposition to optimize the
  //LS fitting process for estimating the weighing matrix W
  //in this case we solve instead:
  //R*Beta = Q'logPhi
  vnl_matrix< double >* m_A;
  vnl_qr< double >* m_Aqr;
  vnl_diag_matrix< double >* m_W;         
  
  void CalculateConstrainedModelParameters( const TensorModelParamType& tensormodelparams,
    ConstrainedModelParamType& constrainedmodelparams);
  
  void CalculateNoiseFreeDWIFromConstrainedModel( const ConstrainedModelParamType& constrainedmodelparams,
    DWIVectorImageType::PixelType& noisefreedwi);
  
  void CalculateResidualVariance( const DWIVectorImageType::PixelType& noisydwi,
    const DWIVectorImageType::PixelType& noisefreedwi,
    const unsigned int dof,
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
    typename InputDWIImageType::IndexType seedindex,
    PathType::Pointer tractPtr);
                    
  unsigned int m_MaxTractLength;
  unsigned int m_TotalTracts;
  typename InputDWIImageType::IndexType m_SeedIndex;
  TractOrientationContainerType::ConstPointer m_SampleDirections;
  unsigned int m_MaxLikelihoodCacheSize;
  unsigned int m_CurrentLikelihoodCacheSize;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkStochasticTractographyFilter.txx"
#include "itkStochasticTractographyFilter_SD.txx"
#endif

#endif
