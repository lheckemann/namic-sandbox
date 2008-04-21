/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBSplineDeformableTransformInitializer.h,v $
  Language:  C++
  Date:      $Date: 2007/04/11 18:10:06 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkBSplineDeformableTransformInitializer_h
#define __itkBSplineDeformableTransformInitializer_h

#include "itkObject.h"
#include "itkObjectFactory.h"

#include <iostream>

namespace itk
{

/** \class BSplineDeformableTransformInitializer
 * \brief BSplineDeformableTransformInitializer is a helper class intended to
 * initialize the grid parameters of a BSplineDeformableTransform based on the
 * parameters of an image.
 *
 * In the context of image registration, the image to be used are reference will
 * be the fixed image. The BSpline grid will use the fixed image as a base for
 * computing the grid spacing, orientation and origin, among other things.
 * 
 * 
 * \ingroup Transforms
 */
template < class TTransform, class TImage > 
class ITK_EXPORT BSplineDeformableTransformInitializer : public Object
{
public:
  /** Standard class typedefs. */
  typedef BSplineDeformableTransformInitializer     Self;
  typedef Object                                    Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;
    
  /** New macro for creation of through a Smart Pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( BSplineDeformableTransformInitializer, Object );

  /** Type of the transform to initialize */
  typedef TTransform                        TransformType;
  typedef typename TransformType::Pointer   TransformPointer;

  /** Dimension of parameters. */
  itkStaticConstMacro(SpaceDimension, unsigned int, 
                      TransformType::InputSpaceDimension);

  /** Image Types to use in the initialization of the transform */
  typedef   TImage                             ImageType;
  typedef   typename ImageType::ConstPointer   ImagePointer;

  
  /** Set the transform to be initialized */
  itkSetObjectMacro( Transform,   TransformType   );

  /** Set the fixed image used in the registration process */
  itkSetConstObjectMacro( Image,  ImageType  );


  /** Initialize the transform using data from the images */
  virtual void InitializeTransform() const;

protected:
  BSplineDeformableTransformInitializer();
  ~BSplineDeformableTransformInitializer(){};

  void PrintSelf(std::ostream &os, Indent indent) const;

private:
  BSplineDeformableTransformInitializer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  TransformPointer    m_Transform;

  ImagePointer        m_Image;

}; //class BSplineDeformableTransformInitializer


}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBSplineDeformableTransformInitializer.txx"
#endif

#endif /* __itkBSplineDeformableTransformInitializer_h */
