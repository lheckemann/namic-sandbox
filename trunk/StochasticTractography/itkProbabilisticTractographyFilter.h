#ifndef __itkProbabilisticTractographyFilter_h__
#define __itkProbabilisticTractographyFilter_h__

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

template< class TInputDWIImage, class TInputROIImage, class TOutputConnectivityImage >
class ITK_EXPORT ProbabilisticTractographyFilter :
  public ImageToImageFilter< TInputDWIImage,
                    TOutputConnectivityImage >{
public:
  typedef ProbabilisticTractographyFilter Self;
  typedef ImageToImageFilter< TInputDWIImage,
                      TOutputConnectivityImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro( ProbabilisticTractographyFilter,
            ImageToImageFilter );
  
  /** Types for the DWI Input Image **/
  typedef TInputDWIImage InputDWIImageType;
  
  /** Type for the ROI Input Image **/
  typedef TInputROIImage InputROIImageType;
  
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

  typedef VectorContainer< unsigned int, vnl_vector_fixed< double, 3 > > 
    TractOrientationContainerType;
  
  /** Set/Get Inputs **/
  itkSetInputMacro(DWIImage, InputDWIImageType, 0);
  itkGetInputMacro(DWIImage, InputDWIImageType, 0);
  
  itkSetInputMacro(ROIImage, InputROIImageType, 1);
  itkGetInputMacro(ROIImage, InputROIImageType, 1);
  
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
  
  /** Set/Get the list of directions to sample **/
  itkSetConstObjectMacro( SampleDirections, TractOrientationContainerType );
  itkGetConstObjectMacro( SampleDirections, TractOrientationContainerType );
  
  /** Set/Get the Measurement Frame **/
  itkSetMacro( MeasurementFrame, MeasurementFrameType );
  itkGetMacro( MeasurementFrame, MeasurementFrameType );
              
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
  
  ProbabilisticTractographyFilter();
  virtual ~ProbabilisticTractographyFilter();
  
  /** Classwide Data members **/
  GradientDirectionContainerType::ConstPointer m_Gradients;
  bValueContainerType::ConstPointer m_bValues;
  MeasurementFrameType m_MeasurementFrame;
  
  /** Preparatory Steps (optimizations) before running algorithm **/
  void BeforeGenerateData(void){
    this->UpdateGradientDirections();
    this->UpdateTensorModelFittingMatrices();
    this->m_LikelihoodCache = ProbabilityDistributionImageType::New();
    this->m_LikelihoodCache->CopyInformation( this->GetDWIImageInput() );
      this->m_LikelihoodCache->SetBufferedRegion( this->GetDWIImageInput()->GetBufferedRegion() );
      this->m_LikelihoodCache->SetRequestedRegion( this->GetDWIImageInput()->GetRequestedRegion() );
    this->m_LikelihoodCache->Allocate();
  }
    
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
    PathType::ContinuousIndexType seedpoint,
    PathType::Pointer tractPtr);
                    
  unsigned int m_MaxTractLength;
  unsigned int m_TotalTracts;
  TractOrientationContainerType::ConstPointer m_SampleDirections;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkProbabilisticTractographyFilter.txx"
#endif

#endif
