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
#include "itkBSplineDeformableTransform.h"

#ifdef WIN32
#include <hash_map>
#else
#include "itk_hash_map.h"
#endif

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
  typedef ImageToImageMetric< TFixedImage, TMovingImage >   Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

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
  MeasureType GetValue( const ParametersType& parameters ) const;

  /**  Get the value and derivatives for single valued optimizers. */
  void GetValueAndDerivative( const ParametersType& parameters, 
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
  virtual ~MultiThreadedMutualInformationImageToImageMetric();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  MultiThreadedMutualInformationImageToImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** \class SpatialSample - private class to store sample information.
   *   A spatial sample consists of the fixed domain point, the fixed image value
   *   at that point, and the corresponding moving image value. */
  class SpatialSample
    {
    public:
    SpatialSample():FixedImageValue(0.0),MovingImageValue(0.0)
    { FixedImagePointValue.Fill( 0.0 ); MovingImagePointValue.Fill( 0.0 ); FixedImageLinearOffset = 0;}
    ~SpatialSample(){};
    // FIXME -- Check offset type
    long                             FixedImageLinearOffset;
    FixedImagePointType              FixedImagePointValue;
    double                           FixedImageValue;
    double                           MovingImageValue;
    MovingImagePointType             MovingImagePointValue;
    };

  /** SpatialSampleContainer typedef support. */
  typedef std::vector<SpatialSample>                        SpatialSampleContainer;
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

  typedef typename Superclass::CoordinateRepresentationType  CoordinateRepresentationType;
  typedef CentralDifferenceImageFunction< MovingImageType, 
                                          CoordinateRepresentationType > DerivativeFunctionType;

  typename DerivativeFunctionType::Pointer  m_DerivativeCalculator;

  bool             m_ReseedIterator;
  int              m_RandomSeed;
  

  /* Multithreading stuff starts here. */
  /* FIXME - CLEANUP */

  typedef itk::MultiThreader                MultiThreaderType;
  typedef std::vector<double>               PartialResultsType;
  typedef std::vector<PartialResultsType>   PartialResultsContainerType;

  typedef std::vector< double >                      WeightPartialResultType;
  typedef WeightPartialResultType::iterator          WeightPartialResultIterator;
  typedef WeightPartialResultType::const_iterator    WeightPartialResultConstIterator;
  typedef std::vector< WeightPartialResultIterator > WeightIteratorContainerType;

  // Sparse derivative
  typedef unsigned long                                                     SparseDerivativeIndexType;
  typedef typename DerivativeType::ValueType                                SparseDerivativeValueType;
  typedef std::pair< SparseDerivativeIndexType, SparseDerivativeValueType > SparseDerivativeEntryType;
  typedef std::vector< SparseDerivativeEntryType >                          SparseDerivativeType;

  // FIXME - Should this be unsigned, what type is the linear offset for itk::Image?
  typedef long FixedImageLinearOffsetType; 

  // Hash map -- element access is fast. Stored in no particular order.
#ifdef WIN32
  typedef stdext::hash_map< FixedImageLinearOffsetType, 
                           DerivativeType > DerivativeMapType;

  typedef stdext::hash_map< FixedImageLinearOffsetType,
                           SparseDerivativeType > SparseDerivativeMapType;

#else
  typedef itk::hash_map< FixedImageLinearOffsetType, 
                         DerivativeType,
                         itk::hash< FixedImageLinearOffsetType > > DerivativeMapType;

  typedef itk::hash_map< FixedImageLinearOffsetType,
                         SparseDerivativeType, 
                         itk::hash< FixedImageLinearOffsetType > > SparseDerivativeMapType;
#endif

  typedef std::vector< DerivativeType > DerivativePartialResultsType; 

  /** FIXME -- hardcoded spline order*/
  typedef BSplineDeformableTransform< 
                            CoordinateRepresentationType,
                            ::itk::GetImageDimension<FixedImageType>::ImageDimension,
                            3 >                                                       BSplineTransformType;

  typedef typename SparseDerivativeType::const_iterator SparseDerivativeIterator;
  typedef std::vector< SamplesConstIterator > ThreadSampleIteratorContainerType;

  struct MultiThreaderParameterType
  {
  Self                   * metric;
  };

  // --------------- Class Variables --------------------------------------

  MultiThreaderType::Pointer                        m_Threader;
  MultiThreaderParameterType                        m_ThreaderParameter;

  mutable PartialResultsContainerType               m_SumFixedPartialAResults;
  mutable PartialResultsContainerType               m_SumMovingPartialAResults;
  mutable PartialResultsContainerType               m_SumJointPartialAResults;

  typename SpatialSampleContainer::const_iterator*  m_SampleAStartIterators;
  typename SpatialSampleContainer::const_iterator*  m_SampleBStartIterators;
  typename SpatialSampleContainer::const_iterator*  m_SampleAEndIterators;
  typename SpatialSampleContainer::const_iterator*  m_SampleBEndIterators;

  unsigned int                                      m_NumberOfThreads;

  mutable bool                                      m_TransformIsBSpline;
  // Bspline optimization
  // Get nonzero indexes
  int                                               m_NumberOfWeights;
  long unsigned int                                 m_NumberOfParametersPerdimension;

  mutable std::vector< WeightPartialResultType >    m_TotalWeightBSamplePartialResult;
  mutable WeightIteratorContainerType               m_TotalWeightPhase3StartIterators;
  mutable WeightIteratorContainerType               m_TotalWeightPhase3EndIterators;

  mutable unsigned int                              m_NumberOfParameters;

  ParametersType                                    m_Indexes; // Holds nonzeros indexes of Bspline derivatives

  mutable TransformPointer*                         m_TransformArray;

  mutable DerivativeMapType                         m_DerivativeMap;
  mutable SparseDerivativeMapType                   m_SparseDerivativeMap;
  mutable DerivativePartialResultsType              m_ThreadDerivatives;

  /** Size of derivative cache in bytes. */
  unsigned long                                     m_DerivativeCacheSize;

  mutable ThreadSampleIteratorContainerType         m_SampleBPhase3StartIterators;
  mutable ThreadSampleIteratorContainerType         m_SampleBPhase3EndIterators;

  //------------------- Functions -----------------------------

  MeasureType GetValueDefault( const ParametersType& parameters ) const; // previous implementation
  MeasureType GetValueMultiThreaded( const ParametersType& parameters ) const;
  void GetValueMultiThreadedInternal( unsigned int threadID ) const;
  void GetValueThreadedInitiate( void ) const;
  double CombineASampleResults();

  void ClearPartialResults() const;

  void GetValueAndDerivativePhase1ThreadedInitiate( void ) const;  
  void GetValueAndDerivativePhase2ThreadedInitiate( void ) const;  
  void GetValueAndDerivativePhase3ThreadedInitiate( void ) const;  

  static ITK_THREAD_RETURN_TYPE GetValueThreadCallback( void * arg );
  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativePhase1ThreadCallback( void * arg );
  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativePhase2ThreadCallback( void * arg );
  static ITK_THREAD_RETURN_TYPE GetValueAndDerivativePhase3ThreadCallback( void * arg );

  void GetValueAndDerivativeDefault( const ParametersType& parameters, 
                                     MeasureType& Value, DerivativeType& Derivative ) const;

  void GetValueAndDerivativeThreaded2( const ParametersType& parameters, 
                                     MeasureType& Value, DerivativeType& Derivative ) const;

  void CalculateDerivativesThreaded( const FixedImagePointType& ,
                                     const MovingImagePointType& mappedPoint,
                                     DerivativeType& derivative,
                                     unsigned int threadID ) const;


  void SetupSampleThreadIterators() const;
  void SetupDerivativePartialResults() const;
  void SetupPhase3BSampleIterators() const;
  bool ValidatePartialResultSizes() const;
  void SetupThreadTransforms() const;
  void SynchronizeTransforms() const;

  void GetValueAndDerivativeMultiThreadedInternalPhase1( unsigned int threadID ) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase1Combine( MeasureType& measure ) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase2( unsigned int threadID ) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase2Combine( DerivativeType& derivative ) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase3(unsigned int thread) const;
  void GetValueAndDerivativeMultiThreadedInternalPhase3Combine( DerivativeType& derivative ) const;

  /** Cache derivatives for the A samples used in the 
   *  inner loop calculations. */
  void CacheSampleADerivatives() const;
  void CacheSparseSampleADerivatives() const;

  /** Calculate a sparse derivative */
  void CalculateDerivativesThreadedSparse( const FixedImagePointType& ,
                                           const MovingImagePointType& mappedPoint,
                                           SparseDerivativeType& derivative,
                                           unsigned int threadID ) const;

  /** Update threadDerivative with the derivative at sample. */
  inline void UpdateDerivative( DerivativeType& threadDerivative, 
                                const SpatialSample& sample, 
                                double weight, 
                                unsigned int threadID ) const;

  /** Update threadDerivative with a sparse derivative at sample. */
  inline void UpdateDerivativeSparse( DerivativeType& threadDerivative,
                                      const SpatialSample& sample,
                                      double weight,
                                      unsigned int threadID ) const;

  inline void FastDerivativeSubtractWithWeight( DerivativeType& in, 
                                                const DerivativeType& subtrahend, 
                                                const double subtractWeight ) const
    {
    double* inData = in.data_block();
    const double* subtrahendData = subtrahend.data_block();
    const unsigned int inSize = in.size();
    if (inSize != subtrahend.size() )
      {
      std::cerr << "FastDerivativeSubtractWithWeight -- sizes don't match!" << std::endl;
      }

    for (unsigned int i = 0; i < inSize; i++ )
      {
      inData[i] -= (subtrahendData[i] * subtractWeight);
      }
    }

  inline void FastDerivativeAddWithWeight( DerivativeType& in, 
                                           const DerivativeType& addend, 
                                           const double addWeight ) const
    {
    double* inData = in.data_block();
    const double* addendData = addend.data_block();
    const unsigned int inSize = in.size();
    if (inSize != addend.size() )
      {
      std::cerr << "FastDerivativeAddWithWeight -- sizes don't match!" << std::endl;
      }

    for (unsigned int i = 0; i < inSize; i++ )
      {
      inData[i] += (addendData[i] * addWeight);
      }
    }

  inline void FastSparseDerivativeSubtractWithWeight( DerivativeType& in, 
                                                      const SparseDerivativeType& subtrahend, 
                                                      const double subtractWeight ) const
    {
    double* inData = in.data_block();
 
    for ( SparseDerivativeIterator siter = subtrahend.begin();
                                   siter != subtrahend.end();
                                   ++siter )
      {
      inData[ (*siter).first ] -= ( (*siter).second * subtractWeight );
      }
    }

  inline void FastDerivativeAdd( DerivativeType& in, 
                                 const DerivativeType& addend ) const
    {
    double* inData = in.data_block();
    const double* addendData = addend.data_block();
    const unsigned int inSize = in.size();
    if (inSize != addend.size() )
      {
      std::cerr << "FastDerivativeAdd -- sizes don't match!" << std::endl;
      }

    for (unsigned int i = 0; i < inSize; i++ )
      {
      inData[i] += addendData[i];
      }
    }

  inline void FastSparseDerivativeAddWithWeight( DerivativeType& in, 
                                                 const SparseDerivativeType& addend, 
                                                 const double weight ) const
    {
    double* inData = in.data_block();
 
    for ( SparseDerivativeIterator siter = addend.begin();
                                   siter != addend.end();
                                   ++siter )
      {
      inData[ (*siter).first ] += ( (*siter).second * weight );
      }
    }

  bool ValidateSparseDerivativeMap() const;
  bool ValidateSamples() const;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiThreadedMutualInformationImageToImageMetric.txx"
#endif

#endif