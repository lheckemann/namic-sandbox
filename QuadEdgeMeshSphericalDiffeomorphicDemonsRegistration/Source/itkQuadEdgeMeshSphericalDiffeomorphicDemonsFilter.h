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


namespace itk
{
template< class TInputMesh, class TOutputMesh >
class QuadEdgeMeshSphericalDiffeomorphicDemonsFilter : 
    public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
{
public:
  typedef QuadEdgeMeshSphericalDiffeomorphicDemonsFilter                Self;
  typedef SmartPointer< Self >                                          Pointer;
  typedef SmartPointer< const Self >                                    ConstPointer;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >   Superclass;
  
  /** Input types. */
  typedef TInputMesh                              InputMeshType;
  typedef typename InputMeshType::Pointer         InputMeshPointer;
  typedef typename InputMeshType::ConstPointer    InputMeshConstPointer;
  typedef typename InputMeshType::CoordRepType    InputCoordRepType;
  typedef typename InputMeshType::PointType       InputPointType;
  typedef typename InputMeshType::PointIdentifier InputPointIdentifier;
  typedef typename InputMeshType::VectorType      InputVectorType;

  typedef typename InputMeshType::PointsContainerConstPointer
    InputPointsContainerConstPointer;
  typedef typename InputMeshType::PointsContainerConstIterator
    InputPointsContainerConstIterator;

  /** Output types. */
  typedef TOutputMesh                               OutputMeshType;
  typedef typename OutputMeshType::Pointer          OutputMeshPointer;
  typedef typename OutputMeshType::ConstPointer     OutputMeshConstPointer;
 
  
public:
  itkNewMacro( Self );
  itkTypeMacro( QuadEdgeMeshSphericalDiffeomorphicDemonsFilter, QuadEdgeMeshToQuadEdgeMeshFilter );
  
protected:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  ~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter();
  
  virtual void GenerateData( );
  
private:
  QuadEdgeMeshSphericalDiffeomorphicDemonsFilter( const Self& );
  void operator = ( const Self& );
};

}

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.txx"
#endif
