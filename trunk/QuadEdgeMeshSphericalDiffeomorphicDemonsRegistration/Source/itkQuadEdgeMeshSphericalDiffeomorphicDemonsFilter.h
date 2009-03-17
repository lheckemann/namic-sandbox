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
  typedef typename  MovingMeshType::ConstPointer    MovingMeshConstPointer;

  /** Output types. */
  typedef TOutputMesh                               OutputMeshType;
  typedef typename  OutputMeshType::Pointer         OutputMeshPointer;
 
  /** Set/Get the Fixed mesh. */
  void SetFixedMesh( const FixedMeshType * fixedMesh );
  itkGetConstObjectMacro( FixedMesh, FixedMeshType ); 

  /** Set/Get the Moving mesh. */
  void SetMovingMesh( const MovingMeshType * movingMesh );
  itkGetConstObjectMacro( MovingMesh, MovingMeshType );


protected:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  ~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  
  virtual void GenerateData( );
  
private:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter( const Self& );
  void operator = ( const Self& );

  void AllocateOutputMesh();

  MovingMeshConstPointer          m_MovingMesh;
  FixedMeshConstPointer           m_FixedMesh;

  typedef Vector< float, 3 >                                    VectorType;
  typedef TriangleBasisSystem< VectorType, 2 >                  BasisSystemType;
  typedef typename TFixedMesh::PointIdentifier                  PointIdentifier;
  typedef VectorContainer< PointIdentifier, BasisSystemType >   BasisSystemContainerType;
  typedef typename BasisSystemContainerType::Pointer            BasisSystemContainerPointer;

  /** This is the Array of "Cn" matrices presented in equation 3.14 in the
   * paper. */
  BasisSystemContainerPointer     m_BasisSystemAtNode; 
};

}

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.txx"
#endif
