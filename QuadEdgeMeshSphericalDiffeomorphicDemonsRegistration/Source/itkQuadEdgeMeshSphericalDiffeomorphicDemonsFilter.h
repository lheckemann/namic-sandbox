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

#ifndef __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h
#define __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkTriangleBasisSystemCalculator.h"
#include "itkTriangleBasisSystem.h"
#include "itkVectorContainer.h"
#include "itkVector.h"


namespace itk
{
template< class TFixedMesh, class TMovingMesh, class TOutputMesh >
class QuadEdgeMeshSphericalDiffeomorphicDemonsFilter : 
    public QuadEdgeMeshToQuadEdgeMeshFilter< TFixedMesh, TOutputMesh >
{
public:
  typedef QuadEdgeMeshSphericalDiffeomorphicDemonsFilter                Self;
  typedef SmartPointer< Self >                                          Pointer;
  typedef SmartPointer< const Self >                                    ConstPointer;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TFixedMesh, TOutputMesh >   Superclass;
  
  /** Method that instantiates a new object */
  itkNewMacro( Self );

  /** Method that provides the name of the class as a string as well as the
   * name of the parent class. */
  itkTypeMacro( QuadEdgeMeshSphericalDiffeomorphicDemonsFilter, QuadEdgeMeshToQuadEdgeMeshFilter );
  
  /** Input types. */
  typedef TFixedMesh                                FixedMeshType;
  typedef TMovingMesh                               MovingMeshType;
  typedef typename  FixedMeshType::ConstPointer     FixedMeshConstPointer;
  typedef typename  FixedMeshType::PointType        PointType;
  typedef typename  MovingMeshType::ConstPointer    MovingMeshConstPointer;

  /** Output types. */
  typedef TOutputMesh                               OutputMeshType;
  typedef typename  OutputMeshType::Pointer         OutputMeshPointer;
 
  /** Declaration of internal types, some of which are exposed for monitoring purposes */
  typedef typename PointType::VectorType                        VectorType;
  typedef TriangleBasisSystem< VectorType, 2 >                  BasisSystemType;
  typedef typename TFixedMesh::PointIdentifier                  PointIdentifier;
  typedef VectorContainer< PointIdentifier, BasisSystemType >   BasisSystemContainerType;
  typedef typename BasisSystemContainerType::Pointer            BasisSystemContainerPointer;
  typedef VectorContainer< PointIdentifier, PointType >         DestinationPointContainerType;
  typedef typename DestinationPointContainerType::Pointer       DestinationPointContainerPointer;

  /** Set/Get the Fixed mesh. */
  void SetFixedMesh( const FixedMeshType * fixedMesh );
  itkGetConstObjectMacro( FixedMesh, FixedMeshType ); 

  /** Set/Get the Moving mesh. */
  void SetMovingMesh( const MovingMeshType * movingMesh );
  itkGetConstObjectMacro( MovingMesh, MovingMeshType );

  /** Returns the array of local coordinates systems at every node of the fixed
   * mesh. This array is only valid after a call to Update() has completed
   * successfully. */
  itkGetConstObjectMacro( BasisSystemAtNode, BasisSystemContainerType );

  /** Returns the array of destination points resulting from applying the
   * deformation field to all nodes of the Fixed Mesh. The content of this array
   * is only valid after the first iteration of the filter execution has been
   * completed. It can be used for tracking the progress of the filter. */
  itkGetConstObjectMacro( DestinationPoints, DestinationPointContainerType );

protected:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  ~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  
  virtual void GenerateData( );
  
private:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter( const Self& );
  void operator = ( const Self& );

  void AllocateOutputMesh();
  void AllocateInternalArrays();

  void ComputeBasisSystemAtEveryNode();

  MovingMeshConstPointer          m_MovingMesh;
  FixedMeshConstPointer           m_FixedMesh;

  /** This is the Array of "Qn" matrices 
   *  presented in equation 3.14 in the paper. */
  BasisSystemContainerPointer     m_BasisSystemAtNode; 

  /** Array containing the destination coordinates of every node in the Fixed
   * Mesh.  This array represents both the deformation field c(xn) and its
   * smoothed version, the field s(xn) as defined in.  */
  DestinationPointContainerPointer   m_DestinationPoints;
};

}

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.txx"
#endif
