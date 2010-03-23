/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVectorImageToImageMetric.h,v $
  Language:  C++
  Date:      $Date: 2007/11/12 20:00:37 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVectorImageToImageMetric_h
#define __itkVectorImageToImageMetric_h

// First make sure that the configuration is available.
// This line can be removed once the optimized versions
// gets integrated into the main directories.
#include "itkConfigure.h"

#include "itkImageBase.h"
#include "itkTransform.h"
#include "itkVectorInterpolateImageFunction.h"
#include "itkSingleValuedCostFunction.h"
#include "itkExceptionObject.h"
#include "itkSpatialObject.h"

namespace itk
{
  
/** \class VectorImageToImageMetric
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

template <class TFixedImage,  class TMovingImage> 
class ITK_EXPORT VectorImageToImageMetric : public SingleValuedCostFunction 
{
public:
  /** Standard class typedefs. */
  typedef VectorImageToImageMetric           Self;
  typedef SingleValuedCostFunction     Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;

  /** Type used for representing point components  */
  typedef typename Superclass::ParametersValueType CoordinateRepresentationType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorImageToImageMetric, SingleValuedCostFunction);

  /**  Type of the moving Image. */
  typedef TMovingImage                               MovingImageType;
  typedef typename TMovingImage::PixelType           MovingImagePixelType;
  typedef typename MovingImageType::ConstPointer     MovingImageConstPointer;

  /**  Type of the fixed Image. */
  typedef TFixedImage                                FixedImageType;
  typedef typename FixedImageType::ConstPointer      FixedImageConstPointer;
  typedef typename FixedImageType::RegionType        FixedImageRegionType;

  /** Constants for the image dimensions */
  itkStaticConstMacro(MovingImageDimension, 
                      unsigned int,
                      TMovingImage::ImageDimension);
  itkStaticConstMacro(FixedImageDimension, 
                      unsigned int,
                      TFixedImage::ImageDimension);
  
  /**  Type of the Transform Base class */
  typedef Transform<CoordinateRepresentationType, 
                    itkGetStaticConstMacro(MovingImageDimension),
                    itkGetStaticConstMacro(FixedImageDimension)> 
                                                     TransformType;

  typedef typename TransformType::Pointer            TransformPointer;
  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    OutputPointType;
  typedef typename TransformType::ParametersType     TransformParametersType;
  typedef typename TransformType::JacobianType       TransformJacobianType;

  /**  Type of the Interpolator Base class */
  typedef VectorInterpolateImageFunction<
    MovingImageType,
    CoordinateRepresentationType > InterpolatorType;


  /** Gaussian filter to compute the gradient of the Moving Image */
  typedef typename NumericTraits<MovingImagePixelType>::RealType 
                                                     RealType;

  typedef typename InterpolatorType::Pointer         InterpolatorPointer;


  /**  Type for the mask of the fixed image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef SpatialObject< itkGetStaticConstMacro(FixedImageDimension) >
                                                     FixedImageMaskType;
  typedef typename  FixedImageMaskType::Pointer      FixedImageMaskPointer;

  /**  Type for the mask of the moving image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef SpatialObject< itkGetStaticConstMacro(MovingImageDimension) >
                                                     MovingImageMaskType;
  typedef typename  MovingImageMaskType::Pointer     MovingImageMaskPointer;


  /**  Type of the measure. */
  typedef typename Superclass::MeasureType                    MeasureType;

  /**  Type of the derivative. */
  typedef typename Superclass::DerivativeType                 DerivativeType;

  /**  Type of the parameters. */
  typedef typename Superclass::ParametersType                 ParametersType;

  /** Connect the Fixed Image.  */
  itkSetConstObjectMacro( FixedImage, FixedImageType );

  /** Get the Fixed Image. */
  itkGetConstObjectMacro( FixedImage, FixedImageType );

  /** Connect the Moving Image.  */
  itkSetConstObjectMacro( MovingImage, MovingImageType );

  /** Get the Moving Image. */
  itkGetConstObjectMacro( MovingImage, MovingImageType );

  /** Connect the Transform. */
  itkSetObjectMacro( Transform, TransformType );

  /** Get a pointer to the Transform.  */
  itkGetConstObjectMacro( Transform, TransformType );
 
  /** Connect the Interpolator. */
  itkSetObjectMacro( Interpolator, InterpolatorType );

  /** Get a pointer to the Interpolator.  */
  itkGetConstObjectMacro( Interpolator, InterpolatorType );

  /** Get the number of pixels considered in the computation. */
  itkGetConstReferenceMacro( NumberOfPixelsCounted, unsigned long );

  /** Set the region over which the metric will be computed */
  itkSetMacro( FixedImageRegion, FixedImageRegionType );

  /** Get the region over which the metric will be computed */
  itkGetConstReferenceMacro( FixedImageRegion, FixedImageRegionType );
 
  /** Set/Get the moving image mask. */
  itkSetObjectMacro( MovingImageMask, MovingImageMaskType );
  itkGetConstObjectMacro( MovingImageMask, MovingImageMaskType );

  /** Set/Get the fixed image mask. */
  itkSetObjectMacro( FixedImageMask, FixedImageMaskType );
  itkGetConstObjectMacro( FixedImageMask, FixedImageMaskType );

  /** Set the parameters defining the Transform. */
  void SetTransformParameters( const ParametersType & parameters ) const;

  /** Return the number of parameters required by the Transform */
  unsigned int GetNumberOfParameters(void) const 
    { return m_Transform->GetNumberOfParameters(); }

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize(void) throw ( ExceptionObject );

protected:
  VectorImageToImageMetric();
  virtual ~VectorImageToImageMetric();
  void PrintSelf(std::ostream& os, Indent indent) const;

  mutable unsigned long       m_NumberOfPixelsCounted;

  FixedImageConstPointer      m_FixedImage;
  MovingImageConstPointer     m_MovingImage;

  mutable TransformPointer    m_Transform;
  InterpolatorPointer         m_Interpolator;

  mutable FixedImageMaskPointer   m_FixedImageMask;
  mutable MovingImageMaskPointer  m_MovingImageMask;

private:
  VectorImageToImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  FixedImageRegionType        m_FixedImageRegion;  

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVectorImageToImageMetric.txx"
#endif

#endif