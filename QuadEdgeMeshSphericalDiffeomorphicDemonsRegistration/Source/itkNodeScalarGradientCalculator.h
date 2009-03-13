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

#include "itkMeshFunction.h"

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
template <class TInputMesh, class TCoordRep = double>
class ITK_EXPORT NodeScalarGradientCalculator :
  public MeshFunction< TInputMesh, TCoordRep >
{
public:
  /** Standard class typedefs. */
  typedef NodeScalarGradientCalculator                      Self;
  typedef MeshFunction<TInputMesh, TCoordRep>               Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NodeScalarGradientCalculator, MeshFunction);

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

  /** Type used for representing point components  */
  typedef Superclass::ParametersValueType CoordinateRepresentationType;


   /** Constants for the pointset dimensions */
  itkStaticConstMacro(MeshDimension, unsigned int,
                      TMesh::PointDimension);
  
  typedef typename MeshType::ConstPointer     MeshConstPointer;
  typedef typename MeshType::PointsContainer::ConstIterator        PointIterator;
  typedef typename MeshType::PointDataContainer::ConstIterator     PointDataIterator;


  /**  Type of the Gradient Calculator Base class */
  typedef MeshFunction<
    MovingMeshType,
    CoordinateRepresentationType >                   NodeScalarGradientCalculatorType;
  typedef typename NodeScalarGradientCalculatorType::Pointer         NodeScalarGradientCalculatorPointer;
  typedef typename NodeScalarGradientCalculatorType::RealType        RealDataType;

  typedef SmartPointer<RealDataType>                 ScalarDataPointer;
  typedef itk::CovariantVector< RealType, MeshDimension >   GradientType;

  typedef typename NodeScalarGradientCalculatorType::GradientType    GradientDataType;

  typedef SmartPointer<RealDataType>                 RealDataPointer;
  typedef SmartPointer<GradientDataType>             GradientDataPointer;

  /** 
   * Compute gradient of the mesh at a point position.
   * Returns the gradient of the mesh intensity at a specified node. The
   * mesh cell is located based on proximity to the point to be evaluated.
   *
   *
   */
  virtual OutputType Evaluate( unsigned int index ) const;  

  /** 
   * Set the fixed mesh. 
   * The method needs this to evaluate values of the warped moving mesh at
   * node points of the fixed mesh. */
  void SetMesh( const MeshType * fixedMesh )
    {
    this->m_Mesh = fixedMesh;
    }


  /** 
   * Set the scalar array, according to which to compute the gradient. 
   * The method needs these values of the warped moving mesh, to then evaluate
   * node points of the fixed mesh. */
  void SetScalarArray( RealDataPointer inScalarArray); 

protected:
  NodeScalarGradientCalculator();
  ~NodeScalarGradientCalculator();

  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef typename Superclass::InstanceIdentifierVectorType InstanceIdentifierVectorType;

  virtual bool ComputeWeights( const PointType & point,
    const InstanceIdentifierVectorType & pointIds ) const;

  virtual void FindTriangle( const PointType& point, InstanceIdentifierVectorType & pointIds ) const;

private:
  NodeScalarGradientCalculator( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typedef typename PointType::VectorType             VectorType;

  MeshConstPointer             m_FixedMesh;
  RealDataPointer              m_ScalarArray;
  GradientDataPointer          m_GradientArray; 
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNodeScalarGradientCalculator.txx"
#endif

#endif
