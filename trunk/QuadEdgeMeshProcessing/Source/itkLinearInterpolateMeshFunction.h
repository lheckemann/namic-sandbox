/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLinearInterpolateMeshFunction.h,v $
  Language:  C++
  Date:      $Date: 2008-05-14 09:26:05 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLinearInterpolateMeshFunction_h
#define __itkLinearInterpolateMeshFunction_h

#include "itkInterpolateMeshFunction.h"

namespace itk
{

/** \class LinearInterpolateMeshFunction
 * \brief Performs linear interpolation in the cell closest to the evaluated point.
 *
 * This class will first locate the cell that is closest to the evaluated
 * point, and then will compute on it the output value using linear
 * interpolation among the values at the points of the cell.
 *
 * \sa VectorLinearInterpolateMeshFunction
 * \ingroup MeshFunctions MeshInterpolators
 * 
 * */
template <class TInputMesh, class TCoordRep = double>
class ITK_EXPORT LinearInterpolateMeshFunction :
  public InterpolateMeshFunction< TInputMesh, TCoordRep >
{
public:
  /** Standard class typedefs. */
  typedef LinearInterpolateMeshFunction                     Self;
  typedef InterpolateMeshFunction<TInputMesh, TCoordRep>    Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LinearInterpolateMeshFunction, MeshFunction);

  /** OutputType typedef support. */
  typedef typename Superclass::OutputType OutputType;

  /** InputMeshType typedef support. */
  typedef typename Superclass::InputMeshType InputMeshType;
  
  /** Dimension underlying input mesh. */
  itkStaticConstMacro(MeshDimension, unsigned int, Superclass::MeshDimension);

  /** Point typedef support. */
  typedef typename Superclass::PointType                  PointType;
  typedef typename Superclass::PointIdentifier            PointIdentifier;
  typedef typename Superclass::CellIdentifier             CellIdentifier;

  /** RealType typedef support. */
  typedef typename TInputMesh::PixelType                  PixelType;
  typedef typename Superclass::RealType                   RealType;
  typedef typename Superclass::DerivativeType             DerivativeType;


  /** Interpolate the mesh at a point position
   *
   * Returns the interpolated mesh intensity at a specified point position. The
   * mesh cell is located based on proximity to the point to be evaluated.
   *
   * FIXME: What to do if the point is far from the Mesh ?
   *
   **/
  virtual OutputType Evaluate( const PointType& point ) const;

  virtual void EvaluateDerivative( const PointType& point, DerivativeType & derivative ) const;

protected:
  LinearInterpolateMeshFunction();
  ~LinearInterpolateMeshFunction();

  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef typename Superclass::InstanceIdentifierVectorType InstanceIdentifierVectorType;

  virtual bool ComputeWeights( const PointType & point,
    const InstanceIdentifierVectorType & pointIds ) const;

  virtual void FindTriangle( const PointType& point, InstanceIdentifierVectorType & pointIds ) const;

private:
  LinearInterpolateMeshFunction( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typedef typename PointType::VectorType             VectorType;

  mutable VectorType  m_V12;
  mutable VectorType  m_V32;

  mutable VectorType  m_U12;
  mutable VectorType  m_U32;

  mutable RealType m_InterpolationWeights[MeshDimension];
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLinearInterpolateMeshFunction.txx"
#endif

#endif
