/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNodeScalarGradientCalculator.h,v $
  Language:  C++
  Date:      $Date: 2008-05-14 09:26:05 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNodeScalarGradientCalculator_h
#define __itkNodeScalarGradientCalculator_h

#include "itkFunctionBase.h"
#include "itkCovariantVector.h"
#include "itkMesh.h"

namespace itk
{

/** \class NodeScalarGradientCalculator
 * Estimates Gradient of a scalar function at at the node of a mesh.
 
 * \brief 
 *
 * \sa NodeScalarGradientCalculator
 * \ingroup MeshFunctions 
 * 
 * */
template <class TInputMesh, class TScalar = double>
class ITK_EXPORT NodeScalarGradientCalculator :
  public FunctionBase< typename TInputMesh::PointIdentifier,
    CovariantVector< 
      typename NumericTraits< TScalar >::RealType,
      ::itk::GetMeshDimension<TInputMesh>::PointDimension > >
{
public:
  
  typedef typename NumericTraits< TScalar >::RealType       RealScalarType;
  typedef CovariantVector< RealScalarType, 3 >              CovariantType;

  /** Standard class typedefs. */
  typedef NodeScalarGradientCalculator                      Self;
  typedef FunctionBase< 
    typename TInputMesh::PointIdentifier, 
    CovariantType > Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NodeScalarGradientCalculator, FunctionBase);

  /** Dimension underlying input mesh. */
  itkStaticConstMacro(MeshDimension, unsigned int, 
    ::itk::GetMeshDimension<TInputMesh>::PointDimension );

  /** Point typedef support. */
  typedef typename TInputMesh::PointType                  PointType;

  /** Set the input mesh. */
  virtual void SetInputMesh( const TInputMesh * ptr );

  /** Get the input mesh. */
  const TInputMesh * GetInputMesh() const;

  typedef typename Superclass::OutputType                 OutputType;
  typedef typename Superclass::InputType                  InputType;

  /** Evaluate at the specified input position */
  virtual OutputType Evaluate( const InputType& input ) const { return OutputType(); }

protected:
  NodeScalarGradientCalculator();
  ~NodeScalarGradientCalculator();

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  NodeScalarGradientCalculator( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typename TInputMesh::ConstPointer    m_Mesh;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNodeScalarGradientCalculator.txx"
#endif

#endif
