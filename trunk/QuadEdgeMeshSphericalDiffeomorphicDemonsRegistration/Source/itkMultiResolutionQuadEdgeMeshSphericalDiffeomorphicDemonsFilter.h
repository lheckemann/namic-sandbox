/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ITKHeader.h,v $
  Language:  C++
  Date:      $Date: 2006-04-25 23:20:16 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkMultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h
#define __itkMultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.h"

#include "itkVersorTransformOptimizer.h"
#include "itkVersorTransform.h"

namespace itk
{
template< class TMesh >
class MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter : 
    public QuadEdgeMeshToQuadEdgeMeshFilter< TMesh, TMesh >
{
public:
  typedef MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter Self;
  typedef SmartPointer< Self >                                          Pointer;
  typedef SmartPointer< const Self >                                    ConstPointer;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TMesh, TMesh >              Superclass;
  
  /** Method that instantiates a new object */
  itkNewMacro( Self );

  /** Method that provides the name of the class as a string as well as the
   * name of the parent class. */
  itkTypeMacro( 
    MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter, QuadEdgeMeshToQuadEdgeMeshFilter );
  
  /** Using a unique mesh type. */
  typedef  TMesh                                  MeshType;
  typedef typename  MeshType::PointType           PointType;

  /** Set/Get the Fixed mesh. */
  void SetFixedMesh( const MeshType * fixedMesh );

  /** Set/Get the Moving mesh. */
  void SetMovingMesh( const MeshType * movingMesh );

  /** Set Sphere Center.  The implementation of this filter assumes that the
   * Mesh surface has a spherical geometry (not only spherical topology). With
   * this method you can specify the coordinates of the center of the sphere
   * represented by the Mesh. This will be used in the computation of parallel
   * transport for vector values associated with nodes.
   */
  itkSetMacro( SphereCenter, PointType );
  itkGetConstMacro( SphereCenter, PointType );

  /** Set Sphere Radius.  The implementation of this filter assumes that the
   * Mesh surface has a spherical geometry (not only spherical topology). With
   * this method you can specify the radius of the sphere. This will be used in
   * the computation of parallel transport for vector values associated
   * with nodes.
   */
  itkSetMacro( SphereRadius, double );
  itkGetConstMacro( SphereRadius, double );

  /**  Create the Output of the proper type for that output number */
  DataObject::Pointer MakeOutput(unsigned int idx);

  typedef VersorTransform< double >  TransformType;

  itkGetConstObjectMacro( RigidTransform, TransformType );

  typedef VersorTransformOptimizer     RigidOptimizerType;

  itkGetObjectMacro( RigidOptimizer, RigidOptimizerType );

  typedef QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< 
    MeshType, MeshType, MeshType >  DemonsFilterType;

  typedef typename DemonsFilterType::DestinationPointSetType    DestinationPointSetType;


protected:
  MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  ~MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  virtual void GenerateData( );
  
private:
  MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter( const Self& );
  void operator = ( const Self& );

  /** Perform rigid registration between two meshes at the current resolution level. */
  void ComputeRigidRegistration();

  /** Center of spherical mesh. We assume that both the Fixed and
   * Moving meshes have spherical geometry and that they share the same
   * center and radius. */
  PointType       m_SphereCenter;

  /** Radius of spherical mesh. We assume that both the Fixed and
   * Moving meshes have spherical geometry and that they share the same
   * center and radius. */
  double          m_SphereRadius;

  unsigned int    m_CurrentResolutionLevel;

  unsigned int    m_NumberOfResolutionLevels;

  typename TransformType::Pointer  m_RigidTransform;

  typename RigidOptimizerType::Pointer  m_RigidOptimizer;

  typename MeshType::Pointer   m_CurrentLevelFixedMesh;
  typename MeshType::Pointer   m_CurrentLevelMovingMesh;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.txx"
#endif

#endif
