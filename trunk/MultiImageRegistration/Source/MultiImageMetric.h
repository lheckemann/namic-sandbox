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
#include "itkSingleValuedCostFunction.h"
#include "itkExceptionObject.h"
#include "itkSpatialObject.h"
#include "itkMultiThreader.h"
#include "itkImageMaskSpatialObject.h"

// Project specific headers
#include "BSplineDeformableTransformOpt.h"
#include "UserMacro.h"

#include <vector>
    
namespace itk
{
  
/** \class MultiImageMetric
 * \brief Computes similarity between regions of a group of images
 *
 * This Class is templated over the type of an input image.
 * It expects an array of  Transforms and an array of
 * Interpolators to be plugged in.
 * This particular class is the base class for a hierarchy of 
 * similarity metrics.
 *
 * This class computes a value that measures the similarity 
 * within a group of images.
 * The Interpolator is used to compute intensity values on 
 * non-grid positions resulting from mapping points through 
 * the Transform.
 * 
 *
 * \ingroup RegistrationMetrics
 *
 */

template <class TImage> 
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

  /**  Type of an input Image. */
  typedef TImage                                     ImageType;
  typedef typename ImageType::PixelType              PixelType;
  typedef typename ImageType::ConstPointer           ImageConstPointer;

  /**  Type of the Image Region */
  typedef typename ImageType::RegionType        ImageRegionType;
  typedef std::vector<ImageConstPointer>        ImageConstPointerArray;

  /** Constants for the image dimensions */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      ImageType::ImageDimension);

  /**  Type of the Transform Base class */
  typedef Transform<CoordinateRepresentationType, 
                    itkGetStaticConstMacro(ImageDimension),
                    itkGetStaticConstMacro(ImageDimension)> TransformType;

  typedef typename TransformType::Pointer            TransformPointer;
  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    OutputPointType;
  typedef typename TransformType::ParametersType     TransformParametersType;
  typedef typename TransformType::JacobianType       TransformJacobianType;
  typedef std::vector<TransformPointer>              TransformPointerArray;

  /**  Type of the Interpolator Base class */
  typedef InterpolateImageFunction<
                                 ImageType,
                                 CoordinateRepresentationType > InterpolatorType;

  typedef typename InterpolatorType::Pointer         InterpolatorPointer;
  typedef std::vector<InterpolatorPointer>           InterpolatorPointerArray;

  /** Gaussian filter to compute the gradient of the Moving Image */
  typedef typename NumericTraits<PixelType>::RealType RealType;
  typedef CovariantVector<PixelType,itkGetStaticConstMacro(ImageDimension)> GradientPixelType;
  typedef Image<GradientPixelType,itkGetStaticConstMacro(ImageDimension)> GradientImageType;
  typedef typename GradientImageType::Pointer     GradientImagePointer;
  typedef std::vector<GradientImagePointer>   GradientImagePointerArray;



  /**  Type for the mask of the fixed image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef ImageMaskSpatialObject< itkGetStaticConstMacro(ImageDimension)
                                             >       ImageMaskType;
  typedef typename  ImageMaskType::Pointer           ImageMaskPointer;
  typedef std::vector<ImageMaskPointer>              ImageMaskPointerArray;
  

  /**  Type of the measure. */
  typedef Superclass::MeasureType                    MeasureType;

  /**  Type of the derivative. */
  typedef Superclass::DerivativeType                 DerivativeType;

  /**  Type of the parameters. */
  typedef Superclass::ParametersType                 ParametersType;
  typedef std::vector<ParametersType>                ParametersArray;

  /** Connect i'th image to Image Array */
  UserSetConstObjectMacro( ImageArray, ImageType );

  /**Get the i'th Image */
  UserGetConstObjectMacro( ImageArray, ImageType );

  /**Connect the Transform Array */
  UserSetObjectMacro( TransformArray, TransformType );

  /** Get a pointer to the i'th Transform */
  UserGetConstObjectMacro( TransformArray, TransformType );
 
  /** Connect the i'th Interpolator. */
  UserSetObjectMacro( InterpolatorArray, InterpolatorType );

  /** Get a pointer to the i'th Interpolator.  */
  UserGetConstObjectMacro( InterpolatorArray, InterpolatorType );

  /** Get the number of pixels considered in the computation. */
  itkGetConstReferenceMacro( NumberOfPixelsCounted, unsigned long );

  /** Set the region over which the metric will be computed */
  itkSetMacro( FixedImageRegion, ImageRegionType );

  /** Get the region over which the metric will be computed */
  itkGetConstReferenceMacro( FixedImageRegion, ImageRegionType );
 
  /** Set/Get the i'th image mask. */
  UserSetObjectMacro( ImageMaskArray, ImageMaskType );
  UserGetConstObjectMacro( ImageMaskArray, ImageMaskType );

  typedef itk::BSplineDeformableTransformOpt<double,   itkGetStaticConstMacro(ImageDimension), 3> BSplineTransformType;
  typedef typename BSplineTransformType::Pointer BSplineTransformTypePointer;
  
  /** Set/Get the i'th Bspline Transform Pointer. 
  *   For B-Spline optimization B-Spline pointers should
  *   be defined separately      */
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
   * samples used to calculate the cost function.
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

  /** Finalize the Metric by making sure that there is no
  *  memory leak     */
  virtual void Finalize(void);
  
  /** Set number of images in the class */
  virtual void SetNumberOfImages(int);

  /** Get number of images in the class */
  int GetNumberOfImages();

  /**  Get the value. Method that provides the infrastructure for supporting Multi-Threading. */
  MeasureType GetValue( const ParametersType& parameters ) const;

  
protected:
  MultiImageMetric();
  virtual ~MultiImageMetric();
  void PrintSelf(std::ostream& os, Indent indent) const;

  mutable unsigned long       m_NumberOfPixelsCounted;

  ImageConstPointerArray      m_ImageArray;

  mutable TransformPointerArray m_TransformArray;
  InterpolatorPointerArray    m_InterpolatorArray;


  bool                        m_ComputeGradient;
  GradientImagePointerArray   m_GradientImageArray;
  
  ImageMaskPointerArray       m_ImageMaskArray;

  unsigned int                m_NumberOfImages;

  unsigned int                m_NumberOfParameters;

  ImageRegionType             m_FixedImageRegion;

  /** Return the multithreader used by this class. */
  MultiThreader * GetMultiThreader() const
    {return m_Threader;}

  /** Methods added for supporting multi-threading */
  void GetThreadedValue( int threadID ) const;
  void BeforeGetThreadedValue(const ParametersType & parameters) const;
  void AfterGetThreadedValue( MeasureType & value, DerivativeType & derivative) const;


  MultiImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  
  /** Get/Set the number of threads to create when executing. */
  itkSetClampMacro( NumberOfThreads, unsigned int, 1, ITK_MAX_THREADS );
  itkGetConstReferenceMacro( NumberOfThreads, unsigned int );

  /** Support processing data in multiple threads. Used by subclasses
   * (e.g., ImageSource). */
  mutable MultiThreader::Pointer m_Threader;
  unsigned int m_NumberOfThreads;

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
  bool m_BsplineDefined;

  unsigned long int                                m_NumberOfSpatialSamples;

  /** A spatial sample consists of the fixed domain point, the fixed image value
   *   at that point, and the corresponding moving image value. */
  typedef typename TransformType::InputPointType        FixedImagePointType;
  typedef typename TransformType::OutputPointType       MovingImagePointType;

  /* A single sample to hold a point and an intensity value array */
  class SpatialSample
  {
    public:
      SpatialSample(){}
      ~SpatialSample(){};

      FixedImagePointType              FixedImagePoint;
      Array< float >                   imageValueArray;
  };
  mutable std::vector<SpatialSample>              m_Sample;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "MultiImageMetric.txx"
#endif

#endif



