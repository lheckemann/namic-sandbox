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
#include "itkTriangleBasisSystem.h"
#include "itkTriangleListBasisSystemCalculator.h"

namespace itk
{

/** \class NodeScalarGradientCalculator
 * Estimates Gradient of a scalar function at at the node of a mesh.
 * 
 * \brief 
 *
 * \sa NodeScalarGradientCalculator
 * \ingroup MeshFunctions 
 * 
 * */
template <class TInputMesh, class TPointDataContainer = double>
class ITK_EXPORT NodeScalarGradientCalculator :
  public FunctionBase< typename TInputMesh::PointIdentifier,
    CovariantVector< 
      typename NumericTraits< typename TPointDataContainer::Element >::RealType,
      ::itk::GetMeshDimension<TInputMesh>::PointDimension > >
{
public:
  /** Standard class typedefs. */
  typedef NodeScalarGradientCalculator                         Self;
  typedef FunctionBase< typename TInputMesh::PointIdentifier,
    CovariantVector< 
      typename NumericTraits< typename TPointDataContainer::Element >::RealType,
      ::itk::GetMeshDimension<TInputMesh>::PointDimension > >  Superclass;
  typedef SmartPointer<Self>                                   Pointer;
  typedef SmartPointer<const Self>                             ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NodeScalarGradientCalculator, FunctionBase);

  /** Dimension underlying input mesh. */
  itkStaticConstMacro(MeshDimension, unsigned int, 
    ::itk::GetMeshDimension<TInputMesh>::PointDimension );

  /** Point typedef support. */
  typedef TInputMesh                                                        InputMeshType;
  typedef typename InputMeshType::PointType                                 PointType;
  typedef typename PointType::VectorType                                    VectorType;

  typedef TriangleBasisSystem< VectorType, 2 >                              TriangleBasisSystemType;
  typedef typename InputMeshType::CellIdentifier                            CellIdentifier;
  typedef VectorContainer<CellIdentifier, TriangleBasisSystemType>          BasisSystemListType;
  typedef typename BasisSystemListType::ConstIterator                       BasisSystemListIterator;

  typedef TriangleListBasisSystemCalculator< 
    InputMeshType, TriangleBasisSystemType >          TriangleListBasisSystemCalculatorType;

  /** Set/Get the input mesh. */
  itkSetConstObjectMacro( InputMesh, InputMeshType );
  itkGetConstObjectMacro( InputMesh, InputMeshType );

  /** Set/Get the input mesh. */
  itkSetConstObjectMacro( DataContainer, TPointDataContainer );
  itkGetConstObjectMacro( DataContainer, TPointDataContainer );

  /** Definition of input type and output type. The input type is actually a
   * point identifier, while the output type is a gradient of the scalar values. */
  typedef typename Superclass::OutputType                 OutputType;
  typedef typename Superclass::InputType                  InputType;

  /** Compute the values at every node. */
  virtual void Compute();  

  /** Set/Get list of basis systems at every cell. */
  itkSetConstObjectMacro( BasisSystemList, BasisSystemListType );
  itkGetConstObjectMacro( BasisSystemList, BasisSystemListType );

  /** Evaluate at the specified input position */
  virtual OutputType Evaluate( const InputType& input ) const;

protected:
  NodeScalarGradientCalculator();
  ~NodeScalarGradientCalculator();

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  NodeScalarGradientCalculator( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typename InputMeshType::ConstPointer                         m_InputMesh;
  typename TPointDataContainer::ConstPointer                   m_DataContainer;
  typename BasisSystemListType::ConstPointer                   m_BasisSystemList;

  /** Check that all necessary inputs are connected. */
  virtual void Initialize( void ) const;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNodeScalarGradientCalculator.txx"
#endif

#endif
