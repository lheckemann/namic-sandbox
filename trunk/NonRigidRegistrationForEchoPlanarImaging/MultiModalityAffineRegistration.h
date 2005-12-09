
#ifndef __MultiModalityAffineRegistration_h
#define __MultiModalityAffineRegistration_h


#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkNormalizeImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkAffineTransform.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkImageFileWriter.h"

#include "itkImageMaskSpatialObject.h"

#include "itkTimeProbesCollectorBase.h"

#include "CommandIterationUpdate.h"
#include "CommandStopUpdate.h"


namespace itk
{

/** \class MultiModalityAffineRegistration
 * \brief Performs rigid registration at Multi-Resolution for same modality images.
 *
 */
class MultiModalityAffineRegistration : public ::itk::Object 
{
public:

  /** Standard class typedefs */
  typedef MultiModalityAffineRegistration    Self;
  typedef ::itk::Object                      Superclass;
  typedef ::itk::SmartPointer<Self>          Pointer;
  typedef ::itk::SmartPointer<const Self>    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiModalityAffineRegistration, ::itk::Object);



  itkStaticConstMacro( Dimension, unsigned int, 3 );

  typedef  float               FixedPixelType;
  typedef  signed short        MovingPixelType;
  typedef  float               InternalPixelType;
  
  typedef itk::Image< FixedPixelType,    Dimension >  FixedImageType;
  typedef itk::Image< MovingPixelType,   Dimension >  MovingImageType;
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;

  typedef  unsigned char       MaskPixelType;

  typedef itk::Image< MaskPixelType, Dimension >     MaskImageType;

  typedef itk::NormalizeImageFilter< 
                           FixedImageType,
                           InternalImageType >       FixedNormalizeFilterType;

  typedef itk::NormalizeImageFilter< 
                           MovingImageType,
                           InternalImageType >       MovingNormalizeFilterType;

  typedef itk::ImageFileWriter< InternalImageType >  ResampledNormalizedWriter;                           
  typedef itk::ImageFileWriter< MovingImageType >    ResampledWriter;                           

  typedef itk::ResampleImageFilter< 
                           InternalImageType, 
                           InternalImageType >       ResampleFilterType;

  typedef itk::ResampleImageFilter< 
                           MovingImageType, 
                           MovingImageType >         ResampleNativeFilterType;
  
  typedef itk::AffineTransform< double >             TransformType;
 
  typedef TransformType::ParametersType              ParametersType;

  typedef itk::RegularStepGradientDescentOptimizer              OptimizerType;

  typedef itk::LinearInterpolateImageFunction< 
                                InternalImageType,
                                double             > LinearInterpolatorType;
  typedef itk::LinearInterpolateImageFunction< 
                                MovingImageType,
                                double    >          NativeLinearInterpolatorType;

  typedef itk::NearestNeighborInterpolateImageFunction< 
                                InternalImageType,
                                double             > NearestInterpolatorType;

  typedef itk::MattesMutualInformationImageToImageMetric< 
                                InternalImageType, 
                                InternalImageType >  MutualInformationMetricType;


  typedef MutualInformationMetricType                MetricType;                                          
                                          

  typedef OptimizerType::ScalesType                  OptimizerScalesType;


  typedef itk::ImageRegistrationMethod< 
                                   InternalImageType, 
                                   InternalImageType    > RegistrationType;


  typedef itk::ImageMaskSpatialObject< Dimension >  SpatialObjectMaskType;

  typedef CommandIterationUpdate            IterationObserverType;
  typedef CommandStopUpdate                 StopObserverType;

  typedef itk::TimeProbesCollectorBase      TimerType;


  // Enums for defining different levels of registration quality.
  typedef enum {
    Low,
    High
  } QualityLevelType;

protected:

  MultiModalityAffineRegistration();
  ~MultiModalityAffineRegistration();


public:

  void SetFixedImage(  const FixedImageType * image );
  void SetMovingImage( const MovingImageType * image );

  void SetFixedImageMask( const MaskImageType * imageMask );
  void SetMovingImageMask( const MaskImageType * imageMask );

  void SetInitialTransform( const TransformType * transform );

  void InitializeRegistration();

  void RegisterCurrentResolutionLevel();

  const ParametersType & GetParameters() const;

  const TransformType * GetTransform() const;

  void ReportTimersOn();
  void ReportTimersOff();

  void SetQualityLevel( QualityLevelType level );

protected:  
  
  void UpdateInputs();

  void PrepareLevel( unsigned int factor );

  
private:

  FixedNormalizeFilterType::Pointer     m_FixedNormalizer;
  MovingNormalizeFilterType::Pointer    m_MovingNormalizer;

  ResampleFilterType::Pointer           m_FixedResampler;
  ResampleFilterType::Pointer           m_MovingResampler;
  ResampleNativeFilterType::Pointer     m_MovingNativeResampler;

  ResampledNormalizedWriter::Pointer    m_FixedWriter;
  ResampledNormalizedWriter::Pointer    m_MovingWriter;
  ResampledWriter::Pointer              m_MovingNativeWriter;

  TransformType::Pointer                m_Transform;
  OptimizerType::Pointer                m_Optimizer;

  LinearInterpolatorType::Pointer       m_LinearInterpolator;
  NearestInterpolatorType::Pointer      m_NearestInterpolator;
  NativeLinearInterpolatorType::Pointer m_NativeLinearInterpolator;

  MetricType::Pointer                   m_Metric;

  RegistrationType::Pointer             m_RegistrationMethod;

  FixedImageType::ConstPointer          m_FixedImage;
  MovingImageType::ConstPointer         m_MovingImage;

  MovingPixelType                       m_BackgroundLevel;

  IterationObserverType::Pointer        m_IterationObserver;
  StopObserverType::Pointer             m_StopObserver;

  TimerType                             m_Timer;

  SpatialObjectMaskType::Pointer        m_FixedMask;
  SpatialObjectMaskType::Pointer        m_MovingMask;

  unsigned int                          m_Level;
  
  bool                                  m_ReportTimers;

  QualityLevelType                      m_QualityLevel;

  std::vector<unsigned int>             m_LevelFactor;
};



}  // end of itk namespace

#endif

