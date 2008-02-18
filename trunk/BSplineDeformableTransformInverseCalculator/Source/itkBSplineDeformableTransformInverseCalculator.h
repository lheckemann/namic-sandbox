/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBSplineDeformableTransformInverseCalculator.h,v $
  Language:  C++
  Date:      $Date: 2007/04/11 18:10:06 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkBSplineDeformableTransformInverseCalculator_h
#define __itkBSplineDeformableTransformInverseCalculator_h

#include "itkObject.h"
#include "itkObjectFactory.h"


namespace itk
{

/** \class BSplineDeformableTransformInverseCalculator
 * \brief  Computes a exact solution (when possible) for the inverse of a BSplineDeformableTransform.
 * 
 * Note that the possibility for computing the inverse of a BSplineDeformableTransform
 * is restricted by the overlap between the BSpline grid of the destination space and
 * the mapped BSpline grid from the source space.
 * 
 * 
 * \ingroup Transforms
 */
template < class TDirectBSplineDeformableTransform,
           class TInverseBSplineDeformableTransform > 
class ITK_EXPORT BSplineDeformableTransformInverseCalculator : public Object
{
public:
  /** Standard class typedefs. */
  typedef BSplineDeformableTransformInverseCalculator     Self;
  typedef Object                                          Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;
    
  /** New macro for creation of through a Smart Pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( BSplineDeformableTransformInverseCalculator, Object );

  /** Type of the source and destination transforms */
  typedef TDirectBSplineDeformableTransform    DirectTransformType;
  typedef TInverseBSplineDeformableTransform   InverseTransformType;

  /** Dimension of parameters. */
  itkStaticConstMacro(DirectSpaceDimension, unsigned int, 
                      DirectTransformType::SpaceDimension);
  itkStaticConstMacro(InverseSpaceDimension, unsigned int, 
                      InverseTransformType::SpaceDimension);

  // FIXME: Add concept checking for making sure that these two dimensions are
  // the same.

  typedef typename DirectTransformType::ConstPointer  DirectTransformPointer;
  typedef typename InverseTransformType::Pointer      InverseTransformPointer;

 /** Point type. */
  typedef typename DirectTransformType::InputPointType  DirectPointType;
  
  /** Vector type. */
  typedef typename DirectTransformType::OutputVectorType  OutputVectorType;
  
  /** Set the transform for which we want to compute the inverse */
  itkSetObjectMacro( DirectTransform,  DirectTransformType );

  /** Set the transform that will receive the inverse. Note that this transform
   * MUST have already initialized all its fixed parameters. Including, its
   * spacing, origin, orientation, and grid size. */
  itkSetObjectMacro( InverseTransform,  InverseTransformType );

  /** Compute the Inverse */
  virtual void Compute() const;

protected:
  BSplineDeformableTransformInverseCalculator();
  ~BSplineDeformableTransformInverseCalculator(){};

  void PrintSelf(std::ostream &os, Indent indent) const;

private:
  BSplineDeformableTransformInverseCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  DirectTransformPointer    m_DirectTransform;
  InverseTransformPointer   m_InverseTransform;

}; //class BSplineDeformableTransformInverseCalculator


}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBSplineDeformableTransformInverseCalculator.txx"
#endif

#endif /* __itkBSplineDeformableTransformInverseCalculator_h */
