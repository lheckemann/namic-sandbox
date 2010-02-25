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
#ifndef __itkWarpQuadEdgeMeshFilter_h
#define __itkWarpQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkLinearInterpolateDeformationFieldMeshFunction.h"

namespace itk
{

/**
 * \class WarpQuadEdgeMeshFilter
 * \brief This filter deforms the mesh of its first input by applying the
 * deformation field of the second input.
 *
 * This filter takes two meshes as inputs. The points of the first input mesh are deformed 
 * following the deformation vectors that are in the field data of the second
 * mesh. Both meshes are expected to be representing a Spherical geometry with
 * a zero genus topology.
 * 
 * The user must set explicitly the values of the sphere radius and center. Both meshes
 * are expected to have the same radius and center.
 *
 *
 * \ingroup MeshFilters
 *
 */
template< class TInputMesh, class TVectorMesh, class TOutputMesh >
class WarpQuadEdgeMeshFilter :
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
{
public:
  typedef WarpQuadEdgeMeshFilter               Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter<
    TInputMesh, TVectorMesh, TOutputMesh >     Superclass;
  typedef SmartPointer< Self >                 Pointer;
  typedef SmartPointer< const Self >           ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( WarpQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TInputMesh                                      InputMeshType;
  typedef TOutputMesh                                     OutputMeshType;

  typedef TVectorMesh                                     VectorMeshType;
  typedef typename VectorMeshType::PointDataContainer     DestinationPointsContainer;


  /** Interpolator typedef. */
  typedef LinearInterpolateDeformationFieldMeshFunction< 
    VectorMeshType, DestinationPointsContainer >          InterpolatorType;
  typedef typename InterpolatorType::Pointer              InterpolatorPointerType;


  /** Set/Get the mesh that will be deformed. */
  void SetInputMesh ( const FixedMeshType * mesh );
  const FixedMeshType * GetInputMesh( void ) const;

  /** Set/Get the mesh that carried the deformation field as pixel data. */
  void SetDeformationMesh ( const VectorMeshType * mesh );
  const VectorMeshType * GetDeformationield( void ) const;

  /** Set the interpolator function.  The default is a linear interpolator. */
  itkSetObjectMacro( Interpolator, InterpolatorType );

  /** Get a pointer to the interpolator function. */
  itkGetConstObjectMacro( Interpolator, InterpolatorType );


protected:
  WarpQuadEdgeMeshFilter();
  ~WarpQuadEdgeMeshFilter();

  void GenerateData();

private:

  WarpQuadEdgeMeshFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented


  InterpolatorPointerType  m_Interpolator;      // Image function for

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWarpQuadEdgeMeshFilter.txx"
#endif

#endif
