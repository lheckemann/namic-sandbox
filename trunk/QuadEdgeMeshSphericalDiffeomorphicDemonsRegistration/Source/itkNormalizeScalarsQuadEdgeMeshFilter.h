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
#ifndef __itkNormalizeScalarsQuadEdgeMeshFilter_h
#define __itkNormalizeScalarsQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"


namespace itk
{

/**
 * \class NormalizeScalarsQuadEdgeMeshFilter
 * \brief This filter normalizes the scalar values of a mesh.
 * 
 * The output mesh will have the same geometry.
 *
 * \ingroup MeshFilters
 *
 */
template< class TInputMesh, class TReferenceMesh, class TDeformationField >
class NormalizeScalarsQuadEdgeMeshFilter :
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TInputMesh >
{
public:
  typedef NormalizeScalarsQuadEdgeMeshFilter                            Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TInputMesh >    Superclass;
  typedef SmartPointer< Self >                                          Pointer;
  typedef SmartPointer< const Self >                                    ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( NormalizeScalarsQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TInputMesh                                                         InputMeshType;
  typedef TInputMesh                                                         OutputMeshType;
  typedef typename OutputMeshType::PointDataContainer                        OutputPointDataContainer;
  typedef typename OutputMeshType::PointDataContainerPointer                 OutputPointDataContainerPointer;

  /** Set/Get the mesh that will be deformed. */
  void SetInputMesh ( const InputMeshType * mesh );
  const InputMeshType * GetInputMesh( void ) const;

protected:
  NormalizeScalarsQuadEdgeMeshFilter();
  ~NormalizeScalarsQuadEdgeMeshFilter();

  void GenerateData();

private:

  NormalizeScalarsQuadEdgeMeshFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNormalizeScalarsQuadEdgeMeshFilter.txx"
#endif

#endif
