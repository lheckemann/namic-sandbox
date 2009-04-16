/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNodeVectorJacobianCalculator.h,v $
  Language:  C++
  Date:      $Date: 2008-05-14 09:26:05 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNodeVectorJacobianCalculator_h
#define __itkNodeVectorJacobianCalculator_h

#include "itkFunctionBase.h"
#include "itkMatrix.h"
#include "itkMesh.h"
#include "itkTriangleBasisSystem.h"
#include "itkTriangleListBasisSystemCalculator.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkTriangleHelper.h"

namespace itk
{

/** \class NodeVectorJacobianCalculator
 * Estimates Jacobian of a vector function at at the node of a mesh.
 * 
 * \brief 
 *
 * \sa NodeVectorJacobianCalculator
 * \ingroup MeshFunctions 
 * 
 * */
template <class TInputMesh, class TVectorContainer>
class ITK_EXPORT NodeVectorJacobianCalculator :
  public FunctionBase< typename TInputMesh::PointIdentifier,
    Matrix< 
     typename TVectorContainer::Element::RealType, 
      ::itk::GetMeshDimension<TInputMesh>::PointDimension > ,
      ::itk::GetMeshDimension<TInputMesh>::PointDimension > >
{
public:
  /** Standard class typedefs. */
  typedef NodeVectorJacobianCalculator                         Self;
  typedef FunctionBase< typename TInputMesh::PointIdentifier,
    Matrix< 
     typename TVectorContainer::Element::RealType, 
      ::itk::GetMeshDimension<TInputMesh>::PointDimension >,
      ::itk::GetMeshDimension<TInputMesh>::PointDimension > >  Superclass;
  typedef SmartPointer<Self>                                   Pointer;
  typedef SmartPointer<const Self>                             ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NodeVectorJacobianCalculator, FunctionBase);

  /** Dimension underlying input mesh. */
  itkStaticConstMacro(MeshDimension, unsigned int, 
    ::itk::GetMeshDimension<TInputMesh>::PointDimension );

  /** Point typedef support. */
  typedef TInputMesh                                                        InputMeshType;
  typedef typename InputMeshType::PointType                                 PointType;
  typedef typename PointType::VectorType                                    VectorType;
  typedef typename InputMeshType::VectorContainer                           VectorContainer;
  typedef typename VectorContainer::Iterator                                VectorIterator;
  typedef typename InputMeshType::PixelType                                 PixelType;
  typedef typename InputMeshType::PointsContainer                           PointsContainer;
  typedef typename PointsContainer::ConstIterator                           PointIterator;

  typedef TriangleBasisSystem< VectorType, 2 >                              TriangleBasisSystemType;
  typedef typename InputMeshType::CellIdentifier                            CellIdentifier;
  typedef VectorContainer<CellIdentifier, TriangleBasisSystemType>          BasisSystemListType;
  typedef typename BasisSystemListType::ConstIterator                       BasisSystemListIterator;

  typedef typename InputMeshType::CellType                                  CellType;
  typedef typename InputMeshType::CellTraits                                CellTraits;
  typedef typename InputMeshType::CellsContainerIterator                    CellsContainerIterator;
  typedef typename CellTraits::PointIdIterator                              PointIdIterator;

  typedef TriangleHelper< PointType >                                       TriangleType;
  typedef typename TriangleType::CoordRepType                               AreaType;

  typedef TriangleListBasisSystemCalculator< InputMeshType, TriangleBasisSystemType >
    TriangleListBasisSystemCalculatorType;


  typedef LinearInterpolateMeshFunction<InputMeshType>                      InterpolatorType;

  typedef PointLocator2< TInputMesh >                                       PointLocatorType;
  typedef typename PointLocatorType::Pointer                                PointLocatorPointer;
  typedef typename InterpolatorType::PointIdentifier                        PointIdentifier;

  typedef typename InterpolatorType::RealType                               RealType;
  typedef typename InterpolatorType::JacobianType                           JacobianType;
  typedef VectorContainer<CellIdentifier, JacobianType>                     JacobianListType;
      
  typedef typename PointType::CoordRepType                                  CoordRepType;
  typedef VectorContainer<PointIdentifier, CoordRepType>                    CoordRepListType; 

  /** Set/Get the input mesh. */
  itkSetConstObjectMacro( InputMesh, InputMeshType );
  itkGetConstObjectMacro( InputMesh, InputMeshType );

  /** Set/Get the input mesh. */
  itkSetConstObjectMacro( DataContainer, TVectorContainer );
  itkGetConstObjectMacro( DataContainer, TVectorContainer );

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
  virtual OutputType Evaluate( const InputType& input) const;

protected:
  NodeVectorJacobianCalculator();
  ~NodeVectorJacobianCalculator();

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  NodeVectorJacobianCalculator( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typename InputMeshType::ConstPointer                         m_InputMesh;
  typename TVectorContainer::ConstPointer                      m_VectorContainer;
  typename BasisSystemListType::ConstPointer                   m_BasisSystemList;
  typename JacobianListType::Pointer                           m_JacobianList;
  typename CoordRepListType::Pointer                           m_PointAreaAccumulatorList; 
  typename JacobianListType::Pointer                           m_PointJacobianAccumulatorList; 

  /** Check that all necessary inputs are connected. */
  virtual void VerifyInputs( void ) const;

  /** Initialize internal variables. */
  virtual void Initialize( void );

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNodeVectorJacobianCalculator.txx"
#endif

#endif
