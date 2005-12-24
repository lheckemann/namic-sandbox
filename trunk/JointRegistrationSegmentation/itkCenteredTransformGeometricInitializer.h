/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCenteredTransformGeometricInitializer.h,v $
  Language:  C++
  Date:      $Date: 2004/04/26 02:17:29 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkCenteredTransformGeometricInitializer_h
#define __itkCenteredTransformGeometricInitializer_h

#include "itkObject.h"
#include "itkObjectFactory.h"

#include <iostream>

namespace itk
{

/** \brief CenteredTransformGeometricInitializer is a helper class intended to
 * initialize the center of rotation and the translation of Transforms having
 * the center of rotation among their parameters.
 * 
 * This class is connected to the fixed image, moving image and transform
 * involved in the registration. Two modes of operation are possible:
 * 
 * - Geometrical, 
 * - Center of mass
 *
 * In the first mode, the geometrical center of the moving image is passed as
 * initial center of rotation to the transform and the vector from the center
 * of the  fixed image to the center of the moving image is passed as the
 * initial translation. This mode basically assumes that the anatomical objects
 * to be registered are centered in their respective images. Hence the best
 * initial guess for the registration is the one that superimposes those two
 * centers.
 *
 * In the second mode, the moments of gray level values are computed
 * for both images. The center of mass of the moving image is then
 * used as center of rotation. The vector between the two centers of
 * mass is passes as the initial translation to the transform. This
 * second approach assumes that the moments of the anatomical objects
 * are similar for both images and hence the best initial guess for
 * registration is to superimpose both mass centers.  Note that this
 * assumption will probably not hold in multi-modality registration.
 * 
 * \ingroup Transforms
 */
template < class TTransform,     
           class TFixedImage,
           class TMovingImage > 
class ITK_EXPORT CenteredTransformGeometricInitializer : 
            public Object
{
public:
  /** Standard class typedefs. */
  typedef CenteredTransformGeometricInitializer     Self;
  typedef Object                           Superclass;
  typedef SmartPointer<Self>               Pointer;
  typedef SmartPointer<const Self>         ConstPointer;
    
  /** New macro for creation of through a Smart Pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( CenteredTransformGeometricInitializer, Object );

  /** Type of the transform to initialize */
  typedef TTransform                        TransformType;
  typedef typename TransformType::Pointer   TransformPointer;

  /** Dimension of parameters. */
  itkStaticConstMacro(SpaceDimension, unsigned int, TransformType::SpaceDimension);
  itkStaticConstMacro(InputSpaceDimension, unsigned int, TransformType::InputSpaceDimension);
  itkStaticConstMacro(OutputSpaceDimension, unsigned int, TransformType::OutputSpaceDimension);

  
  /** Image Types to use in the initialization of the transform */
  typedef   TFixedImage              FixedImageType;
  typedef   TMovingImage             MovingImageType;

  typedef   typename FixedImageType::ConstPointer   FixedImagePointer;
  typedef   typename MovingImageType::ConstPointer  MovingImagePointer;

  /** Offset type. */
  typedef typename TransformType::OffsetType  OffsetType;

  /** Point type. */
  typedef typename TransformType::InputPointType   InputPointType;
  
  /** Vector type. */
  typedef typename TransformType::OutputVectorType  OutputVectorType;
  
  /** Set the transform to be initialized */
  itkSetObjectMacro( Transform,   TransformType   );

  /** Set the fixed image used in the registration process */
  itkSetConstObjectMacro( FixedImage,  FixedImageType  );

  /** Set the moving image used in the registration process */
  itkSetConstObjectMacro( MovingImage, MovingImageType );


  /** Initialize the transform using data from the images */
  virtual void InitializeTransform() const;


protected:
  CenteredTransformGeometricInitializer();
  ~CenteredTransformGeometricInitializer(){};

  void PrintSelf(std::ostream &os, Indent indent) const;

private:
  CenteredTransformGeometricInitializer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  TransformPointer    m_Transform;

  FixedImagePointer   m_FixedImage;

  MovingImagePointer  m_MovingImage;

}; //class CenteredTransformGeometricInitializer


}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCenteredTransformGeometricInitializer.txx"
#endif

#endif /* __itkCenteredTransformGeometricInitializer_h */
