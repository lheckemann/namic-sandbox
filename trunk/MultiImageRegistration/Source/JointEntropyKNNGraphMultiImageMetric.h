/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMutualInformationCongealingMetric.h,v $
  Language:  C++
  Date:      $Date: 2005/10/03 15:18:45 $
  Version:   $Revision: 1.39 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __JointEntropyKNNGraphMultiImageMetric_h
#define __JointEntropyKNNGraphMultiImageMetric_h

#include "itkCovariantVector.h"
#include "itkPoint.h"

#include "itkIndex.h"
#include "itkKernelFunction.h"
#include "itkCentralDifferenceImageFunction.h"
    
#include "itkGradientImageFilter.h"
#include "itkImageRandomNonRepeatingConstIteratorWithIndex.h"

#include "itkImageRegionIterator.h"

//user defined headers
#include <vector>
#include "MultiImageMetric.h"


// ANN is provided with Lesser GNU Public License (LGPL)
// check if that is a problem    
#include <ANN/ANN.h>             // ANN declarations

    
using namespace std;

namespace itk
{

/** \class ParzenWindowEntropyImageToImageMetric
 * \brief Computes the mutual information between two images to be registered
 *
 * MutualInformationImageToImageMetric computes the mutual information
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
template <class TFixedImage>
class ITK_EXPORT JointEntropyKNNGraphMultiImageMetric :
    public MultiImageMetric< TFixedImage>
{
public:

  /** Standard class typedefs. */
  typedef JointEntropyKNNGraphMultiImageMetric  Self;
  typedef MultiImageMetric< TFixedImage > Superclass;
  //typedef CongealingMetric< TFixedImage, TFixedImage > Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(JointEntropyKNNGraphMultiImageMetric, MultiImageMetric);

  /** Types inherited from Superclass. */
  typedef typename Superclass::TransformType            TransformType;
  typedef typename Superclass::TransformPointer         TransformPointer;
  typedef typename Superclass::TransformJacobianType    TransformJacobianType;
  typedef typename Superclass::InterpolatorType         InterpolatorType;
  typedef typename Superclass::MeasureType              MeasureType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename Superclass::ParametersType           ParametersType;
  typedef typename Superclass::ParametersArray          ParametersArray;
  typedef typename Superclass::FixedImageType           FixedImageType;
  typedef typename Superclass::MovingImageType          MovingImageType;
  typedef typename Superclass::FixedImageConstPointer   FixedImageConstPointer;
  typedef typename Superclass::MovingImageConstPointer  MovingImageCosntPointer;
  typedef typename Superclass::GradientImageType        GradientImageType;
  typedef typename Superclass::GradientImagePointer     GradientImagePointer;
  typedef typename Superclass::GradientPixelType        GradientPixelType;
  typedef typename Superclass::MovingImagePixelType     ImagePixelType;
  typedef typename Superclass::RealType        RealType;
  typedef typename Superclass::GradientOutputType     GradientOutputType;

  
  struct ThreadStruct
  {
    ConstPointer Metric;
  };
  //typedef vector<FixedImageType> FixedImageTypeArray;
  //typedef vector<FixedImageConstPointer> ImageConstPointerArray;

  /** Index and Point typedef support. */
  typedef typename FixedImageType::IndexType            FixedImageIndexType;
  typedef typename FixedImageIndexType::IndexValueType  FixedImageIndexValueType;
  typedef typename MovingImageType::IndexType           MovingImageIndexType;
  typedef typename TransformType::InputPointType        FixedImagePointType;
  typedef typename TransformType::OutputPointType       MovingImagePointType;

  /** PixelType */
  //typedef typename FixedImageType::PixelType            PixelType;


  /** Enum of the moving image dimension. */
  itkStaticConstMacro(MovingImageDimension, unsigned int,
                      MovingImageType::ImageDimension);

  /** Get the derivatives of the match measure. */
  void GetDerivative( 
    const ParametersType& parameters,
    DerivativeType & Derivative ) const;

  /**  Get the value. */
  MeasureType GetValue( const ParametersType& parameters ) const;
  /** Methods added for supporting multi-threading GetValue */
  void GetThreadedValue( int threadID ) const;
  void BeforeGetThreadedValue(const ParametersType & parameters) const;
  MeasureType AfterGetThreadedValue() const;

  /**  Get the value and derivatives for single valued optimizers. */
  void GetValueAndDerivative( const ParametersType& parameters, 
                              MeasureType& Value, DerivativeType& Derivative ) const;


  /** Methods added for supporting multi-threading GetValueAndDerivative */
  void GetThreadedValueAndDerivative( int threadID ) const;
  void BeforeGetThreadedValueAndDerivative(const ParametersType & parameters) const;
  void AfterGetThreadedValueAndDerivative(MeasureType & value,
                             DerivativeType & derivative) const;
  

  /** Set/Get the moving image intensitiy standard deviation. This defines
   * the kernel bandwidth used in the joint probability distribution
   * calculation. Default value is 0.4 which works well for image intensities
   * normalized to a mean of 0 and standard deviation of 1.0.  
   * Value is clamped to be always greater than zero. */
  itkSetClampMacro( ImageStandardDeviation, double,
                    NumericTraits<double>::NonpositiveMin(), NumericTraits<double>::max() );
  itkGetConstReferenceMacro( ImageStandardDeviation, double );

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize(void) throw ( ExceptionObject );

  /** Finalize the Metric by making sure that there is no
   *  memory leak     */
  virtual void Finalize(void);
  
  /** Define the bspline tranform type for regularization
     For Regularization BsplineTransfromPointer must be explicitly
     provided */
  typedef itk::UserBSplineDeformableTransform<double,   itkGetStaticConstMacro(MovingImageDimension), 3> BSplineTransformType;
  typedef typename BSplineTransformType::Pointer BSplineTransformTypePointer;
  
  typedef typename BSplineTransformType::ImageType BSplineParametersImageType;
  typedef typename BSplineParametersImageType::Pointer BSplineParametersImagePointer;
  
  typedef itk::GradientImageFilter<BSplineParametersImageType, RealType, RealType> GradientFilterType;
  typedef typename GradientFilterType::Pointer GradientFilterTypePointer;


  /** Set/Get the regularization factor */
  itkSetMacro( RegularizationFactor, RealType );
  itkGetMacro( RegularizationFactor, RealType );

  /** Set/Get the number of fixed images */
  itkSetMacro( NumberOfFixedImages, unsigned int );
  itkGetMacro( NumberOfFixedImages, unsigned int );
  
  /** Turn regularization on/off: Default off (true=on) */
  itkSetMacro( Regularization, bool );
  /** Get regularization on/off */
  itkGetMacro( Regularization, bool );

  /** Set/Get the regularization factor */
  itkSetMacro( NumberOfNearestNeigbors, unsigned int );
  itkGetMacro( NumberOfNearestNeigbors, unsigned int );

  /** Set/Get the regularization factor */
  itkSetMacro( ErrorBound, double );
  itkGetMacro( ErrorBound, double );

protected:
  JointEntropyKNNGraphMultiImageMetric();
  virtual ~JointEntropyKNNGraphMultiImageMetric();
  void PrintSelf(std::ostream& os, Indent indent) const;

  JointEntropyKNNGraphMultiImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  
  /** SpatialSampleContainer typedef support. */
  typedef typename Superclass::SpatialSample SpatialSample;
  typedef SpatialSample *  SpatialSampleContainer;
  static ITK_THREAD_RETURN_TYPE ThreaderCallbackGetValueAndDerivative( void *arg );
  static ITK_THREAD_RETURN_TYPE ThreaderCallbackGetValue( void *arg );


  double                              m_ImageStandardDeviation;
  typename KernelFunction::Pointer    m_KernelFunction;
  double                              m_MinProbability;

  /** Uniformly select samples from the fixed image buffer. */
  void GetSamples() const;

  /** Add the derivative update to the current images parameters at a given point and image derivative*/
  typedef CovariantVector < double, MovingImageDimension > CovarientType;
  void UpdateImageParameters( DerivativeType & inputDerivative, const int& index, const RealType& weight, const int& imageNumber, const int& threadID) const;

  typedef typename Superclass::CoordinateRepresentationType  CoordinateRepresentationType;
  typedef CentralDifferenceImageFunction< MovingImageType,   CoordinateRepresentationType > DerivativeFunctionType;


  bool             m_ReseedIterator;
  int              m_RandomSeed;
  
  mutable Array< RealType >   m_value;
  mutable std::vector< std::vector< typename DerivativeFunctionType::Pointer > > m_DerivativeCalculator;
  mutable std::vector< std::vector<DerivativeType> > m_DerivativesArray;
  int m_NumberOfThreads;

  // Bspline optimization
  ParametersType indexes; // Holds nonzeros indexes of Bspline derivatives

  mutable std::vector< ParametersType > m_TransformParametersArray;

  bool m_Regularization;
  double m_RegularizationFactor;
  
  bool m_UseMask;
  unsigned int m_NumberOfFixedImages;

  typedef ImageRandomNonRepeatingConstIteratorWithIndex < FixedImageType > NonRepeatingRandomIterator;
  mutable NonRepeatingRandomIterator* randIter;

  // Get nonzero indexex
  int numberOfWeights;
  mutable unsigned int* bsplineIndexes;
  long unsigned int numbersPerdimension;


  /** KNN related parameters */
  unsigned int m_NumberOfNearestNeigbors;
  double m_ErrorBound;
  
  mutable ANNdistArray*      dists;               // near neighbor distances
  mutable ANNidxArray*       nnIdx;

  mutable ANNkd_tree*       kdTree;              // search structure
  mutable ANNpointArray     dataPts;          // data points

  mutable MovingImagePointType**            m_MappedPointsArray;
  mutable FixedImagePointType*               m_FixedImagePointArray;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "JointEntropyKNNGraphMultiImageMetric.cxx"
#endif

#endif

