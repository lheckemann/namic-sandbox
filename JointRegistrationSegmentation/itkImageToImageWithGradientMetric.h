/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToImageWithGradientMetric.h,v $
  Language:  C++
  Date:      $Date: 2004/12/21 22:47:26 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToImageWithGradientMetric_h
#define __itkImageToImageWithGradientMetric_h

#include "itkImageToImageMetric.h"
#include "itkGradientRecursiveGaussianImageFilter.h"

namespace itk
{
  
/** \class ImageToImageWithGradientMetric
 *
 * \brief Computes similarity between regions of two images and provide
 * gradient of the moving image.
 *
 * This class precomputes the gradient of the moving image in order to offer it
 * to subclasses that can use it for computing the derivatives of the Metric as
 * a chain rule combining the image Gradient with the Jacobian of the
 * Transform.
 * 
 *
 * \ingroup RegistrationMetrics
 *
 */

template <class TFixedImage,  class TMovingImage> 
class ITK_EXPORT ImageToImageWithGradientMetric : 
                  public ImageToImageMetric< TFixedImage, TMovingImage > 
{
public:
  /** Standard class typedefs. */
  typedef ImageToImageWithGradientMetric                  Self;
  typedef ImageToImageMetric<TFixedImage, TMovingImage>   Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  /** Type used for representing point components  */
  typedef Superclass::ParametersValueType CoordinateRepresentationType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToImageWithGradientMetric, ImageToImageMetric);

  /**  Type of the moving Image. */
  typedef typename Superclass::MovingImageType          MovingImageType;
  typedef typename Superclass::MovingImagePixelType     MovingImagePixelType;
  typedef typename Superclass::MovingImageConstPointer  MovingImageConstPointer;

  /**  Type of the fixed Image. */
  typedef typename Superclass::FixedImageType           FixedImageType;
  typedef typename Superclass::FixedImageConstPointer   FixedImageConstPointer;
  typedef typename Superclass::FixedImageRegionType     FixedImageRegionType;

  /** Constants for the image dimensions */
  itkStaticConstMacro(MovingImageDimension, unsigned int,
                      TMovingImage::ImageDimension);
  itkStaticConstMacro(FixedImageDimension, unsigned int,
                      TFixedImage::ImageDimension);
  
  /**  Type of the Transform Base class */
  typedef typename Superclass::TransformType            TransformType;
  typedef typename Superclass::TransformPointer         TransformPointer;
  typedef typename Superclass::InputPointType           InputPointType;
  typedef typename Superclass::OutputPointType          OutputPointType;
  typedef typename Superclass::TransformParametersType  TransformParametersType;
  typedef typename Superclass::TransformJacobianType    TransformJacobianType;

  /**  Type of the Interpolator Base class */
  typedef typename Superclass::InterpolatorType         InterpolatorType;


  /** Gaussian filter to compute the gradient of the Moving Image */
  typedef typename NumericTraits<MovingImagePixelType>::RealType RealType;
  typedef CovariantVector<RealType,
                          itkGetStaticConstMacro(MovingImageDimension)> GradientPixelType;
  typedef Image<GradientPixelType,
                itkGetStaticConstMacro(MovingImageDimension)> GradientImageType;
  typedef SmartPointer<GradientImageType>     GradientImagePointer;
  typedef GradientRecursiveGaussianImageFilter< MovingImageType,
                                                GradientImageType >
  GradientImageFilterType;  
  typedef typename GradientImageFilterType::Pointer GradientImageFilterPointer;


  /** Type of the interpolator taken from the superclass */
  typedef typename Superclass::InterpolatorPointer    InterpolatorPointer;


  /**  Type for the mask of the fixed image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef typename Superclass::FixedImageMaskType      FixedImageMaskType;
  typedef typename Superclass::FixedImageMaskPointer   FixedImageMaskPointer;


  /**  Type for the mask of the moving image. Only pixels that are "inside"
       this mask will be considered for the computation of the metric */
  typedef typename Superclass::MovingImageMaskType      MovingImageMaskType;
  typedef typename Superclass::MovingImageMaskPointer   MovingImageMaskPointer;



  /**  Type of the measure. */
  typedef Superclass::MeasureType                    MeasureType;

  /**  Type of the derivative. */
  typedef Superclass::DerivativeType                 DerivativeType;

  /**  Type of the parameters. */
  typedef Superclass::ParametersType                 ParametersType;


  /** Set/Get gradient computation. */
  itkSetMacro( ComputeGradient, bool);
  itkGetConstReferenceMacro( ComputeGradient, bool);
  itkBooleanMacro(ComputeGradient);

  /** Get Gradient Image. */
  itkGetConstObjectMacro( GradientImage, GradientImageType );


protected:
  ImageToImageWithGradientMetric();
  virtual ~ImageToImageWithGradientMetric() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  bool                        m_ComputeGradient;
  GradientImagePointer        m_GradientImage;

private:
  ImageToImageWithGradientMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToImageWithGradientMetric.txx"
#endif

#endif



