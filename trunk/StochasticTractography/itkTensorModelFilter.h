#ifndef __itkTensorModelFilter_h__
#define __itkTensorModelFilter_h__

#include "itkImageToImageFilter.h"
#include "itkVectorContainer.h"
#include "itkVariableLengthVector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_qr.h"
#include "vnl/vnl_diag_matrix.h"

namespace itk{
//convention: first line is input class types,
//second line is output class types
template< class TDWIVectorImage, class TDiffusionTensorImage>
class ITK_EXPORT TensorModelFilter :
  public ImageToImageFilter< TDWIVectorImage,
                    TDiffusionTensorImage >{
public:
  /** Standard class typedefs. */
  typedef TensorModelFilter Self;
  typedef ImageToImageFilter< TDWIVectorImage,
                    TDiffusionTensorImage > Superclass;
  typedef SmartPointer< Self >  Pointer;
  typedef SmartPointer< const Self >  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** For Type Information **/
  itkTypeMacro(TensorModelFilter, ImageToImageFilter );
  
  /** Types for the DWI Input Image **/
  typedef TDWIVectorImage DWIVectorImageType;
  typedef typename TDWIVectorImage::IndexType DWIVectorImageIndexType;
  typedef typename DWIVectorImageType::PixelType DWIVectorImagePixelType;
  typedef typename DWIVectorImageType::PointType DWIVectorImagePointType;
  typedef typename DWIVectorImageType::ConstPointer DWIVectorImageConstPointer;
  
  /** Types for the Diffusion Tensor Output **/
  typedef TDiffusionTensorImage DTImageType;
  typedef typename DTImageType::Pointer DTImagePointer;
  typedef typename DTImageType::PixelType DTImagePixelType;

  /**Types for the parameters of the Tensor Model **/
  typedef vnl_vector_fixed< double, 7 > TensorModelParamType;
  
  /** Types for the Image-wide Magnetic Field Gradient Directions **/
  typedef VectorContainer< unsigned int, vnl_vector_fixed< double, 3 > >
    GradientDirectionContainerType;
  
  /** Types for the Image-wide bValues **/
  typedef double bValueType;
  typedef VectorContainer< unsigned int, bValueType > bValueContainerType;
  typedef bValueContainerType::Pointer bValueContainerPointer;
  
  /** Types for the Measurement Frame of the Gradients **/
  typedef vnl_matrix_fixed< double, 3, 3 > MeasurementFrameType;
  
  /** Set/Get bvalues **/
  itkSetConstObjectMacro( bValues, bValueContainerType );
  itkGetConstObjectMacro( bValues, bValueContainerType );
  
  /** Set/Get of gradient directions **/
  itkSetConstObjectMacro( Gradients, GradientDirectionContainerType );
  itkGetConstObjectMacro( Gradients, GradientDirectionContainerType );
  
  /** Set/Get the Measurement Frame **/
  itkSetMacro( MeasurementFrame, MeasurementFrameType );
  itkGetMacro( MeasurementFrame, MeasurementFrameType );
protected:
  //be sure to use itk's smart pointers with its factory constructed objects!
  //a segmentation fault will occur if you don't
  /** Classwide Data members **/
  GradientDirectionContainerType::ConstPointer m_Gradients;
  bValueContainerType::ConstPointer m_bValues;
  MeasurementFrameType m_MeasurementFrame;
  
  //these will be the same for every pixel in the image so
  //go ahead and do a QR decomposition to optimize the
  //least squares fitting process
  //in this case we solve instead:
  //R*Beta = Q'logPhi
  vnl_matrix< double >* m_A;
  vnl_qr< double >* m_Aqr;
  vnl_diag_matrix< double >* m_W;
  
  TensorModelFilter();
  virtual ~TensorModelFilter() {};
  
  /** Preparatory Steps (optimizations) before running algorithm **/
  void BeforeGenerateData(void){
    this->UpdateGradientDirections();
    this->UpdateTensorModelFittingMatrices();
  }
  
  /** Functions and data related to fitting the tensor model at each pixel **/
  void UpdateGradientDirections(void);
  void UpdateTensorModelFittingMatrices( void );
  void CalculateTensorModelParameters( const VariableLengthVector< double >& dwivalues,
                             TensorModelParamType& tensormodelparams);
  
  void GenerateData ( void );

};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorModelFilter.txx"
#endif

#endif
