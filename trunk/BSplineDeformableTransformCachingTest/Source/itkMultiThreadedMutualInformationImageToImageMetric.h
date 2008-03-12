/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMultiThreadedMutualInformationImageToImageMetric.h,v $
  Language:  C++
  Date:      $Date: 2007/12/20 22:17:30 $
  Version:   $Revision: 1.43 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMultiThreadedMutualInformationImageToImageMetric_h
#define __itkMultiThreadedMutualInformationImageToImageMetric_h

#include "itkImageToImageMetric.h"
#include "itkCovariantVector.h"
#include "itkPoint.h"

#include "itkIndex.h"
#include "itkKernelFunction.h"
#include "itkCentralDifferenceImageFunction.h"
#include "itkSparseVector.h"

namespace itk
{

/** \class MultiThreadedMutualInformationImageToImageMetric
 * \brief Computes the mutual information between two images to be registered
 *
 * MultiThreadedMutualInformationImageToImageMetric computes the mutual information
 * between a fixed and moving image to be registered.
 *
 * This class is templated over the FixedImage type and the MovingImage type.
 *
 * The fixed and moving images are set via methods SetFixedImage() and
 * SetMovingImage(). This metric makes use of user specified Transform and
 * Interpolator. The Transform is used to map points from the fixed image to
 * the moving image domain. The Interpolator is used to evaluate the image
 * intensity at user specified geometric points in the moving image.
 * The Transform and Interpolator are set via methods SetTransform() and
 * SetInterpolator().
 *
 * \warning This metric assumes that the moving image has already been
 * connected to the interpolator outside of this class. 
 *
 * The method GetValue() computes of the mutual information
 * while method GetValueAndDerivative() computes
 * both the mutual information and its derivatives with respect to the
 * transform parameters.
 *
 * The calculations are based on the method of Viola and Wells
 * where the probability density distributions are estimated using
 * Parzen windows.
 *
 * By default a Gaussian kernel is used in the density estimation.
 * Other option include Cauchy and spline-based. A user can specify
 * the kernel passing in a pointer a KernelFunction using the
 * SetKernelFunction() method.
 *
 * Mutual information is estimated using two sample sets: one to calculate
 * the singular and joint pdf's and one to calculate the entropy
 * integral. By default 50 samples points are used in each set.
 * Other values can be set via the SetNumberOfSpatialSamples() method.
 *
 * Quality of the density estimate depends on the choice of the
 * kernel's standard deviation. Optimal choice will depend on the images.
 * It is can be shown that around the optimal variance, the mutual
 * information estimate is relatively insensitive to small changes
 * of the standard deviation. In our experiments, we have found that a
 * standard deviation of 0.4 works well for images normalized to have a mean
 * of zero and standard deviation of 1.0.
 * The variance can be set via methods SetFixedImageStandardDeviation()
 * and SetMovingImageStandardDeviation().
 *
 * Implementaton of this class is based on:
 * Viola, P. and Wells III, W. (1997).
 * "Alignment by Maximization of Mutual Information"
 * International Journal of Computer Vision, 24(2):137-154
 *
 * \sa KernelFunction
 * \sa GaussianKernelFunction
 *
 * \ingroup RegistrationMetrics
 */
template <class TFixedImage,class TMovingImage >
class ITK_EXPORT MultiThreadedMutualInformationImageToImageMetric :
    public ImageToImageMetric< TFixedImage, TMovingImage >
{
public:

  /** Standard class typedefs. */
  typedef MultiThreadedMutualInformationImageToImageMetric  Self;
  typedef ImageToImageMetric< TFixedImage, TMovingImage > Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiThreadedMutualInformationImageToImageMetric, ImageToImageMetric);

  /** Types inherited from Superclass. */
  typedef typename Superclass::TransformType            TransformType;
  typedef typename Superclass::TransformPointer         TransformPointer;
  typedef typename Superclass::TransformJacobianType    TransformJacobianType;
  typedef typename Superclass::InterpolatorType         InterpolatorType;
  typedef typename Superclass::MeasureType              MeasureType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename Superclass::ParametersType           ParametersType;
  typedef typename Superclass::FixedImageType           FixedImageType;
  typedef typename Superclass::MovingImageType          MovingImageType;
  typedef typename Superclass::FixedImageConstPointer   FixedImageConstPointer;
  typedef typename Superclass::MovingImageConstPointer  MovingImageCosntPointer;

  /** Index and Point typedef support. */
  typedef typename FixedImageType::IndexType            FixedImageIndexType;
  typedef typename FixedImageIndexType::IndexValueType  FixedImageIndexValueType;
  typedef typename MovingImageType::IndexType           MovingImageIndexType;
  typedef typename TransformType::InputPointType        FixedImagePointType;
  typedef typename TransformType::OutputPointType       MovingImagePointType;

  /** Enum of the moving image dimension. */
  itkStaticConstMacro(MovingImageDimension, unsigned int,
                      MovingImageType::ImageDimension);

  /** Get the derivatives of the match measure. */
  void GetDerivative( 
    const ParametersType& parameters,
    DerivativeType & Derivative ) const;

  /**  Get the value. */
  MeasureType GetValue( const ParametersType& parameters ) const; // threaded
  MeasureType GetValueDefault( const ParametersType& parameters ) const; // previous implementation

  /**  Get the value and derivatives for single valued optimizers. */
  void GetValueAndDerivative( const ParametersType& parameters, 
                              MeasureType& Value, DerivativeType& Derivative ) const;

  void GetValueAndDerivativeDefault( const ParametersType& parameters, 
                                     MeasureType& Value, DerivativeType& Derivative ) const;


  /** Set the number of spatial samples. This is the number of image
   * samples used to calculate the joint probability distribution.
   * The number of spatial samples is clamped to be a minimum of 1.
   * Default value is 50. */
  void SetNumberOfSpatialSamples( unsigned int num );

  /** Get the number of spatial samples. */
  itkGetConstReferenceMacro( NumberOfSpatialSamples, unsigned int );

  /** Set/Get the moving image intensitiy standard deviation. This defines
   * the kernel bandwidth used in the joint probability distribution
   * calculation. Default value is 0.4 which works well for image intensities
   * normalized to a mean of 0 and standard deviation of 1.0.  
   * Value is clamped to be always greater than zero. */
  itkSetClampMacro( MovingImageStandardDeviation, double, 
                    NumericTraits<double>::NonpositiveMin(), NumericTraits<double>::max() );
  itkGetConstReferenceMacro( MovingImageStandardDeviation, double );

  /** Set/Get the fixed image intensitiy standard deviation. This defines
   * the kernel bandwidth used in the joint probability distribution
   * calculation. Default value is 0.4 which works well for image intensities
   * normalized to a mean of 0 and standard deviation of 1.0.  
   * Value is clamped to be always greater than zero. */
  itkSetClampMacro( FixedImageStandardDeviation, double,
                    NumericTraits<double>::NonpositiveMin(), NumericTraits<double>::max() );
  itkGetMacro( FixedImageStandardDeviation, double );

  /** Set/Get the kernel function. This is used to calculate the joint
   * probability distribution. Default is the GaussianKernelFunction. */
  itkSetObjectMacro( KernelFunction, KernelFunction );
  itkGetObjectMacro( KernelFunction, KernelFunction );

  /** Reinitialize the seed of the random number generator that selects the
   * sample of pixels used for estimating the image histograms and the joint
   * histogram. By nature, this metric is not deterministic, since at each run
   * it may select a different set of pixels. By initializing the random number
   * generator seed to the same value you can restore determinism. On the other
   * hand, calling the method ReinitializeSeed() without arguments will use the
   * clock from your machine in order to have a very random initialization of
   * the seed. This will indeed increase the non-deterministic behavior of the
   * metric. */
  void ReinitializeSeed();
  void ReinitializeSeed(int);

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize( void ) throw ( ExceptionObject );

  /** Initialize the components related to supporting multiple threads */
  virtual void MultiThreadingInitialize( void ) throw ( ExceptionObject );

protected:
  MultiThreadedMutualInformationImageToImageMetric();
  virtual ~MultiThreadedMutualInformationImageToImageMetric() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  MultiThreadedMutualInformationImageToImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  /** A spatial sample consists of the fixed domain point, the fixed image value
   *   at that point, and the corresponding moving image value. */
  class SpatialSample
  {
  public:
    SpatialSample():FixedImageValue(0.0),MovingImageValue(0.0)
    { FixedImagePointValue.Fill( 0.0 ); MovingImagePointValue.Fill( 0.0 ); }
    ~SpatialSample(){};

    FixedImagePointType              FixedImagePointValue;
    double                           FixedImageValue;
    double                           MovingImageValue;
    MovingImagePointType             MovingImagePointValue;
  };

  /** SpatialSampleContainer typedef support. */
  typedef std::vector<SpatialSample>  SpatialSampleContainer;
  typedef typename SpatialSampleContainer::iterator         SamplesIterator;
  typedef typename SpatialSampleContainer::const_iterator   SamplesConstIterator;

  /** Container to store sample set  A - used to approximate the probability
   * density function (pdf). */
  mutable SpatialSampleContainer      m_SampleA;

  /** Container to store sample set  B - used to approximate the mutual
   * information value. */
  mutable SpatialSampleContainer      m_SampleB;

  unsigned int                        m_NumberOfSpatialSamples;
  double                              m_MovingImageStandardDeviation;
  double                              m_FixedImageStandardDeviation;
  typename KernelFunction::Pointer    m_KernelFunction;
  double                              m_MinProbability;

  /** Uniformly select samples from the fixed image buffer.
   * \warning Note that this method has a different signature than the one in
   * the base OptImageToImageMetric and therefore they are not intended to
   * provide polymorphism. That is, this function is not overriding the one in
   * the base class. */
  virtual void SampleFixedImageDomain( SpatialSampleContainer& samples ) const;

  /**
   * Calculate the intensity derivatives at a point
   */
  void CalculateDerivatives( const FixedImagePointType& , DerivativeType& ) const;

  typedef typename Superclass::CoordinateRepresentationType  
  CoordinateRepresentationType;
  typedef CentralDifferenceImageFunction< MovingImageType, 
                                          CoordinateRepresentationType > DerivativeFunctionType;

  typename DerivativeFunctionType::Pointer  m_DerivativeCalculator;

  bool             m_ReseedIterator;
  int              m_RandomSeed;
  

  /** Multithreading stuff starts here - FIXME - cleanup. */
  typedef itk::MultiThreader               MultiThreaderType;

  struct MultiThreaderParameterType
    {
    Self                   * metric;
    };

  MultiThreaderType::Pointer               m_Threader;
  MultiThreaderParameterType               m_ThreaderParameter;

  void GetValueMultiThreadedInternal( unsigned int threadID ) const;

  typedef std::vector<double>             PartialResultsType;

  mutable std::vector<PartialResultsType>           m_SumFixedPartialAResults;
  mutable std::vector<PartialResultsType>           m_SumMovingPartialAResults;
  mutable std::vector<PartialResultsType>           m_SumJointPartialAResults;

  mutable std::vector<unsigned long>                m_SampleAVisitCount;

  typename SpatialSampleContainer::const_iterator*  m_SampleAStartIterators;
  typename SpatialSampleContainer::const_iterator*  m_SampleBStartIterators;
  typename SpatialSampleContainer::const_iterator*  m_SampleAEndIterators;
  typename SpatialSampleContainer::const_iterator*  m_SampleBEndIterators;

  std::vector<unsigned long>::iterator*             m_SampleAVisitCountStartIterators;
  std::vector<unsigned long>::iterator*             m_SampleAVisitCountEndIterators;

  unsigned int m_NumberOfThreads;

  static ITK_THREAD_RETURN_TYPE GetValueThreadCallback( void * arg );
  void GetValueThreadedInitiate( void ) const;

  void ClearPartialResults() const;

  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativePhase1ThreadCallback( void * arg );
  void GetValueAndDerivativePhase1ThreadedInitiate( void ) const;  

  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativePhase2ThreadCallback( void * arg );
  void GetValueAndDerivativePhase2ThreadedInitiate( void ) const;  

  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativePhase3ThreadCallback( void * arg );
  void GetValueAndDerivativePhase3ThreadedInitiate( void ) const;  

public:
  MeasureType GetValueMultiThreaded( const ParametersType& parameters ) const;

  void GetValueAndDerivativeThreaded( const ParametersType& parameters, 
                                     MeasureType& Value, DerivativeType& Derivative ) const;

  void GetValueAndDerivativeThreaded2( const ParametersType& parameters, 
                                     MeasureType& Value, DerivativeType& Derivative ) const;

private:
  void CalculateDerivativesThreaded( const FixedImagePointType& ,
                                     const MovingImagePointType& mappedPoint,
                                     DerivativeType& derivative,
                                     unsigned int threadID ) const;

  void CalculateDerivativesThreadedBSplineSparse( const FixedImagePointType& ,
                                            const MovingImagePointType& mappedPoint,
                                            SparseVector< double > & ) const;
  void CalculateDerivativesThreadedDefaultTransform( const FixedImagePointType& ,
                                                     const MovingImagePointType& mappedPoint,
                                                     DerivativeType& ) const;

  /*
  bool m_TransformParametersHaveLocalInfluence;
  */
  mutable bool m_TransformIsBSpline;

  // Bspline optimization
  // Get nonzero indexex
  int m_NumberOfWeights;
  //mutable Array<unsigned long> m_JacobianIndexes;
  long unsigned int m_NumberOfParametersPerdimension;

  ParametersType m_Indexes; // Holds nonzeros indexes of Bspline derivatives

  void SetupSampleThreadIterators() const;
  double CombineASampleResults();
  void GetValueAndDerivativeMultiThreadedInternalPhase1( unsigned int threadID ) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase1Combine( MeasureType& measure ) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase2( unsigned int threadID ) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase2Combine( DerivativeType& derivative ) const;

  // DerivativeType* m_Derivative;

  /*
  typedef std::vector< DerivativeType > DerivativePartialResultsType;
  mutable DerivativePartialResultsType m_DerivativePartialResult;
  mutable DerivativePartialResultsType m_DerivativeBPartialResult;
  mutable DerivativePartialResultsType m_TotalWeightPartialResult;
  */

  typedef std::vector< double > WeightPartialResultType;
  mutable std::vector< WeightPartialResultType > m_TotalWeightBSamplePartialResult;

  void SetupDerivativePartialResults() const;

  typedef std::vector< DerivativeType > DerivativePartialResultsType; 
  mutable DerivativePartialResultsType m_ThreadDerivatives;

  void GetValueAndDerivativeMultiThreadedInternalPhase3(unsigned int thread) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase3Combine( DerivativeType& derivative ) const;

  typedef std::vector< SamplesConstIterator > ThreadSampleIteratorContainerType;
  mutable ThreadSampleIteratorContainerType m_SampleBPhase3StartIterators;
  mutable ThreadSampleIteratorContainerType m_SampleBPhase3EndIterators;

  mutable unsigned int m_NumberOfParameters;

  void SetupPhase3BSampleIterators() const;

  bool ValidatePartialResultSizes() const;

  void SetupThreadTransforms() const;
  void SynchronizeTransforms() const;

  //mutable TransformType** m_ThreaderTransform;
  mutable TransformPointer* m_TransformArray;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiThreadedMutualInformationImageToImageMetric.txx"
#endif

#endif


