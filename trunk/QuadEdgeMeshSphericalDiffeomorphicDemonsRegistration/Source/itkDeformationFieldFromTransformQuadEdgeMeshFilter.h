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
#ifndef __itkDeformationFieldFromTransformQuadEdgeMeshFilter_h
#define __itkDeformationFieldFromTransformQuadEdgeMeshFilter_h

#include "itkMeshToMeshFilter.h"
#include "itkTransform.h"

namespace itk
{

/**
 * \class DeformationFieldFromTransformQuadEdgeMeshFilter
 * \brief Generate destination points from a Mesh and a Transform.
 *
 * This filter takes as input a Mesh and a Transform and produces as
 * output a point container with the list of Mesh points image resulting from
 * the Transform mapping.
 *
 * \ingroup MeshFilters
 *
 */
template< class TInputMesh, class TOutputMesh >
class DeformationFieldFromTransformQuadEdgeMeshFilter :
  public MeshToMeshFilter< TInputMesh, TOutputMesh >
{
public:
  typedef DeformationFieldFromTransformQuadEdgeMeshFilter     Self;
  typedef MeshToMeshFilter< 
    TInputMesh, TOutputMesh >                                 Superclass;
  typedef SmartPointer< Self >                                Pointer;
  typedef SmartPointer< const Self >                          ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( DeformationFieldFromTransformQuadEdgeMeshFilter, MeshToMeshFilter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TInputMesh                                       InputMeshType;
  typedef typename InputMeshType::Pointer                  InputMeshPointer;
  typedef typename InputMeshType::PixelType                InputPixelType;
  typedef typename InputMeshType::PointDataContainer       InputPointDataContainer;

  typedef TOutputMesh                                        OutputMeshType;
  typedef typename OutputMeshType::Pointer                   OutputMeshPointer;
  typedef typename OutputMeshType::ConstPointer              OutputMeshConstPointer;
  typedef typename OutputMeshType::PointIdentifier           OutputPointIdentifier;
  typedef typename OutputMeshType::PointType                 OutputPointType;
  typedef typename OutputPointType::VectorType               OutputVectorType;
  typedef typename OutputPointType::CoordRepType             OutputCoordType;
  typedef typename OutputMeshType::PointsContainer           OutputPointsContainer;
  typedef typename OutputMeshType::PixelType                 OutputPixelType;

  typedef typename OutputMeshType::PointsContainerConstPointer    OutputPointsContainerConstPointer;
  typedef typename OutputMeshType::PointsContainerPointer         OutputPointsContainerPointer;
  typedef typename OutputMeshType::PointsContainerIterator        OutputPointsContainerIterator;
  typedef typename OutputMeshType::PointsContainerConstIterator   OutputPointsContainerConstIterator;
  typedef typename OutputMeshType::PointDataContainer             OutputPointDataContainer;
  typedef typename OutputMeshType::PointDataContainerPointer      OutputPointDataContainerPointer;
  typedef typename OutputMeshType::PointDataContainerConstPointer OutputPointDataContainerConstPointer;

  itkStaticConstMacro( PointDimension, unsigned int, OutputMeshType::PointDimension );

  /** Transform typedef. */
  typedef Transform<double, 
    itkGetStaticConstMacro(PointDimension), 
    itkGetStaticConstMacro(PointDimension)>         TransformType;
  typedef typename TransformType::ConstPointer      TransformPointerType;

  /** Interpolator typedef. */
  typedef InterpolateMeshFunction< InputMeshType >  InterpolatorType;
  typedef typename InterpolatorType::Pointer        InterpolatorPointerType;


  /** Set the coordinate transformation.  Set the coordinate transform that
   * will map the points of the input mesh to points of the output PointSet. 
   * The points of the output PointSet are one-to-one the result of taking
   * points from the input Mesh and mapping them through the Transform.
   */
  itkSetConstObjectMacro( Transform, TransformType ); 

  /** Get a pointer to the coordinate transform. */
  itkGetConstObjectMacro( Transform, TransformType );


protected:
  DeformationFieldFromTransformQuadEdgeMeshFilter();
  ~DeformationFieldFromTransformQuadEdgeMeshFilter();

  void GenerateData();

private:

  DeformationFieldFromTransformQuadEdgeMeshFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  TransformPointerType     m_Transform;         // Coordinate transform to use

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDeformationFieldFromTransformQuadEdgeMeshFilter.txx"
#endif

#endif
