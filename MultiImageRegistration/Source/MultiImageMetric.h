/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCongealingMetric.h,v $
  Language:  C++
  Date:      $Date: 2004/12/21 22:47:26 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __MultiImageMetric_h
#define __MultiImageMetric_h

#include "itkImageBase.h"
#include "itkTransform.h"
#include "itkInterpolateImageFunction.h"
#include "itkVectorInterpolateImageFunction.h"
#include "itkSingleValuedCostFunction.h"
#include "itkExceptionObject.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkSpatialObject.h"
#include "itkMultiThreader.h"
#include "itkImageMaskSpatialObject.h"
#include "UserBSplineDeformableTransform.h"
#include "MutualInformationLinearInterpolateImageFunction.h"

#include <vector>
#include "UserMacro.h"

using std::vector;
    
namespace itk
{
  
/** \class CongealingMetric
 * \brief Computes similarity between regions of two images.
 *
 * This Class is templated over the type of the two input images.
 * It expects a Transform and an Interpolator to be plugged in.
 * This particular class is the base class for a hierarchy of 
 * similarity metrics.
 *
 * This class computes a value that measures the similarity 
 * between the Fixed image and the transformed Moving image.
 * The Interpolator is used to compute intensity values on 
 * non-grid positions resulting from mapping points through 
 * the Transform.
 * 
 *
 * \ingroup RegistrationMetrics
 *
 */

template <class TFixedImage> 
class ITK_EXPORT MultiImageMetric : public SingleValuedCostFunction 
{
public:
  /** Standard class typedefs. */
  typedef MultiImageMetric                Self;
  typedef SingleValuedCostFunction        Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Type used for representing point components  */
  typedef Superclass::ParametersValueType CoordinateRepresentationType;

  /** Run-time type information (and related methods). */
  itkTypeMacro( MultiImageMetric, SingleValuedCostFunction);

  /**  Type of the moving Image. */
  typedef TFixedImage                                TMovingImage;
  typedef TMovingImage                               MovingImageType;
  typedef typename TMovingImage::PixelType           MovingImagePixelType;
  typedef typename MovingImageType::ConstPointer     MovingImageConstPointer;

  /**  Type of the fixed Image. */
  typedef TFixedImage                                FixedImageType;
  typedef typename FixedImageType::ConstPointer      FixedImageConstPointer;
  typedef typename FixedImageType::RegionType        FixedImageRegionType;
  typedef std::vector<FixedImageConstPointer>        ImageConstPointerArray;

  /** Constants for the image dimensions */
  itkStaticConstMacro(MovingImageDimension, unsigned int,
                      TMovingImage::ImageDimension);

  /**  Type of the Transform Base class */
  typedef Transform<CoordinateRepresentationType, 
                    itkGetStaticConstMacro(MovingImageDimension),
                    itkGetStaticConstMacro(MovingImageDimension)> TransformType;

  typedef typename TransformType::Pointer            TransformPointer;
  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    OutputPointType;
  typedef typename TransformType::ParametersType     TransformParametersType;
  typedef typename TransformType::JacobianType       TransformJacobianType;
  typedef std::vector<TransformPointer>              TransformPointerArray;

  /**  Type of the Interpolator Base class */
  typedef InterpolateImageFunction<
                                 MovingImageType,
                                 CoordinateRepresentationType > InterpolatorType;

  typedef MutualInformationLinearInterpolateImageFunction<
                                 MovingImageType,
                                 CoordinateRepresentationType > MutualInformationInterpolatorType;
                                 
  /** Gaussian filter to compute the gradient of the Moving Image */
  typedef typename NumericTraits<MovingImagePixelType>::RealType RealType;
  typedef CovariantVector<MovingImagePixelType,itkGetStaticConstMacro(MovingImageDimension)> GradientPixelType;
  typedef Image<GradientPixelType,itkGetStaticConstMacro(MovingImageDimension)> GradientImageType;
  typedef typename GradientImageType::Pointer     GradientImagePointer;
  typedef std::vector<GradientImagePointer>   GradientImagePointerArray;
  typedef GradientRecursiveGaussianImageFilter< MovingImageType,GradientImageType > GradientImageFilterType;


  typedef typename InterpolatorType::Pointer         InterpolatorPointer;
  typedef std::vector<InterpolatorPointer>           InterpolatorPointerArray;

  typedef typename MutualInformationInterpolatorType::Pointer         MutualInformationInterpolatorPointer;
  typedef std::vector<MutualInformationInterpolatorPointer>           MutualInformationInterpolatorPointerArray;
  
  /** interpolator for gradient images */
  typedef VectorInterpolateImageFunction<
                                 GradientImageType,
                                 MovingImagePixelType > GradientInterpolatorType;
  typedef typename GradientInterpolatorType::Pointer         GradientInterpolatorPointer;
  typedef std::vector<GradientInterpolatorPointer>           GradientInterpolatorPointerArray;
  typedef typename GradientInterpolatorType::OutputType                   GradientOutputType;
  
  /**  Type for the mask of the fixed image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef ImageMaskSpatialObject< itkGetStaticConstMacro(MovingImageDimension)
                                             >       FixedImageMaskType;
  typedef typename  FixedImageMaskType::Pointer      FixedImageMaskPointer;

  /**  Type for the mask of the moving image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef ImageMaskSpatialObject< itkGetStaticConstMacro(MovingImageDimension)
                                             >      MovingImageMaskType;
  typedef typename  MovingImageMaskType::Pointer     MovingImageMaskPointer;
  typedef std::vector<MovingImageMaskPointer>              ImageMaskPointerArray;



  /**  Type of the measure. */
  typedef Superclass::MeasureType                    MeasureType;

  /**  Type of the derivative. */
  typedef Superclass::DerivativeType                 DerivativeType;

  /**  Type of the parameters. */
  typedef Superclass::ParametersType                 ParametersType;
  typedef std::vector<ParametersType>                ParametersArray;

  /** Connect i'th image to Image Array */
  UserSetConstObjectMacro( ImageArray, FixedImageType );

  /**Get the i'th Image */
  UserGetConstObjectMacro( ImageArray, FixedImageType );

  /**Connect the Transform Array */
  UserSetObjectMacro( TransformArray, TransformType );

  /** Get a pointer to the i'th Transform */
  UserGetConstObjectMacro( TransformArray, TransformType );
 
  /** Connect the i'th Interpolator. */
  UserSetObjectMacro( InterpolatorArray, InterpolatorType );

  /** Get a pointer to the i'th Interpolator.  */
  UserGetConstObjectMacro( InterpolatorArray, InterpolatorType );

  /** Connect the i'th Mutual Information Interpolator. */
  UserSetObjectMacro( MutualInformationInterpolatorArray, MutualInformationInterpolatorType );

  /** Get a pointer to the i'th Mutual Information Interpolator.  */
  UserGetConstObjectMacro( MutualInformationInterpolatorArray, MutualInformationInterpolatorType );
  
  /** Connect the i'th gradient Interpolator. */
  UserSetObjectMacro( GradientInterpolatorArray, GradientInterpolatorType );

  /** Get a pointer to the i'th gradient Interpolator.  */
  UserGetConstObjectMacro( GradientInterpolatorArray, GradientInterpolatorType );
  
  /** Get the number of pixels considered in the computation. */
  itkGetConstReferenceMacro( NumberOfPixelsCounted, unsigned long );

  /** Set the region over which the metric will be computed */
  itkSetMacro( FixedImageRegion, FixedImageRegionType );

  /** Get the region over which the metric will be computed */
  itkGetConstReferenceMacro( FixedImageRegion, FixedImageRegionType );
 
  /** Set/Get the i'th image mask. */
  UserSetObjectMacro( ImageMaskArray, MovingImageMaskType );
  UserGetConstObjectMacro( ImageMaskArray, MovingImageMaskType );

  typedef itk::UserBSplineDeformableTransform<double,   itkGetStaticConstMacro(MovingImageDimension), 3> BSplineTransformType;
  typedef typename BSplineTransformType::Pointer BSplineTransformTypePointer;
  
  /** Set/Get the i'th Bspline Transform Pointer. */
  UserSetObjectMacro( BSplineTransformArray, BSplineTransformType );
  UserGetConstObjectMacro( BSplineTransformArray, BSplineTransformType );
  
  /** Set/Get gradient computation. */
  itkSetMacro( ComputeGradient, bool);
  itkGetConstReferenceMacro( ComputeGradient, bool);
  itkBooleanMacro(ComputeGradient);

  /** Get Gradient Image. */
  UserSetObjectMacro( GradientImageArray, GradientImageType );
  UserGetObjectMacro( GradientImageArray, GradientImageType );

  /** Set the number of spatial samples. This is the number of image
   * samples used to calculate the joint probability distribution.
   * The number of spatial samples is clamped to be a minimum of 1.
   * Default value is 50. */
  itkSetMacro( NumberOfSpatialSamples, unsigned int);
  itkGetMacro( NumberOfSpatialSamples, unsigned int);
  
  /** Set the parameters defining the Transform. */
  void SetTransformParameters(const ParametersType & parameters ) const;

  /** Return the number of parameters required by the Transform */
  unsigned int GetNumberOfParameters(void) const 
   { return m_TransformArray[0]->GetNumberOfParameters()*m_NumberOfImages; }

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize(void) throw ( ExceptionObject );

  /** Set number of images in the class */
  virtual void SetNumberOfImages(int);

  /** Get number of images in the class */
  int GetNumberOfImages();

  /**  Get the value. Method that provides the infrastructure for supporting Multi-Threading. */
  MeasureType GetValue( const ParametersType& parameters ) const;

protected:
  MultiImageMetric();
  virtual ~MultiImageMetric() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  mutable unsigned long       m_NumberOfPixelsCounted;

  ImageConstPointerArray      m_ImageArray;

  mutable TransformPointerArray m_TransformArray;
  InterpolatorPointerArray    m_InterpolatorArray;
  MutualInformationInterpolatorPointerArray m_MutualInformationInterpolatorArray;
      
  GradientInterpolatorPointerArray m_GradientInterpolatorArray;
  
  bool                        m_ComputeGradient;
  bool                        m_CorrectInterpolationArtefact;
  //GradientImagePointer        m_GradientImage;
  GradientImagePointerArray   m_GradientImageArray;

  //mutable FixedImageMaskPointer   m_FixedImageMask;
  //mutable MovingImageMaskPointer  m_MovingImageMask;
  ImageMaskPointerArray   m_ImageMaskArray;

  /** Number of images */
  unsigned int m_NumberOfImages;

  /** number of total parameters */
  unsigned int numberOfParameters;

  /** Return the multithreader used by this class. */
  MultiThreader * GetMultiThreader() const
    {return m_Threader;}

  /** Methods added for supporting multi-threading */
  void GetThreadedValue( int threadID ) const;
  void BeforeGetThreadedValue(const ParametersType & parameters) const;
  void AfterGetThreadedValue( MeasureType & value, DerivativeType & derivative) const;


  MultiImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  FixedImageRegionType        m_FixedImageRegion;  
  
  /** Get/Set the number of threads to create when executing. */
  itkSetClampMacro( NumberOfThreads, int, 1, ITK_MAX_THREADS );
  itkGetConstReferenceMacro( NumberOfThreads, int );

  /** Support processing data in multiple threads. Used by subclasses
   * (e.g., ImageSource). */
  mutable MultiThreader::Pointer m_Threader;
  mutable int m_NumberOfThreads;

  /** Static function used as a "callback" by the MultiThreader.  The threading
   * library will call this routine for each thread, which will delegate the
   * control to ThreadedGenerateData(). */
  static ITK_THREAD_RETURN_TYPE ThreaderCallback( void *arg );

  /** Internal structure used for passing image data into the threading library */
  struct ThreadStruct
  {
   ConstPointer Metric;
  };

  // Bspline transform
  // Provided to optimize for bsplines
  mutable std::vector<BSplineTransformTypePointer> m_BSplineTransformArray;
  bool m_UserBsplineDefined;

  unsigned int                        m_NumberOfSpatialSamples;


};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "MultiImageMetric.cxx"
#endif

#endif



