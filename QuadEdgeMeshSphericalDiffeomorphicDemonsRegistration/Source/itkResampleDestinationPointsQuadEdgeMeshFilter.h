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
#ifndef __itkResampleDestinationPointsQuadEdgeMeshFilter_h
#define __itkResampleDestinationPointsQuadEdgeMeshFilter_h

#include "itkMeshToMeshFilter.h"
#include "itkLinearInterpolateDeformationFieldMeshFunction.h"
#include "itkTransform.h"

namespace itk
{

/**
 * \class ResampleDestinationPointsQuadEdgeMeshFilter
 * \brief This resamples the scalar values of one QuadEdgeMesh into another one
 * via a user-provided Transform and Interpolator.
 *
 * \ingroup MeshFilters
 *
 */
template< class TInputMesh, class TReferenceMesh, class TOutputMesh >
class ResampleDestinationPointsQuadEdgeMeshFilter :
  public MeshToMeshFilter< TInputMesh, TOutputMesh >
{
public:
  typedef ResampleDestinationPointsQuadEdgeMeshFilter                    Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< 
    TInputMesh, TOutputMesh >                           Superclass;
  typedef SmartPointer< Self >                          Pointer;
  typedef SmartPointer< const Self >                    ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( ResampleDestinationPointsQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TInputMesh                                       InputMeshType;
  typedef typename InputMeshType::Pointer                  InputMeshPointer;

  typedef TReferenceMesh                                     ReferenceMeshType;

  typedef TOutputMesh                                        OutputMeshType;
  typedef typename OutputMeshType::Pointer                   OutputMeshPointer;
  typedef typename OutputMeshType::ConstPointer              OutputMeshConstPointer;
  typedef typename OutputMeshType::PointIdentifier           OutputPointIdentifier;
  typedef typename OutputMeshType::PointType                 OutputPointType;
  typedef typename OutputPointType::VectorType               OutputVectorType;
  typedef typename OutputPointType::CoordRepType             OutputCoordType;
  typedef typename OutputMeshType::PointsContainer           OutputPointsContainer;

  typedef typename OutputMeshType::PointsContainerConstPointer    OutputPointsContainerConstPointer;
  typedef typename OutputMeshType::PointsContainerPointer         OutputPointsContainerPointer;
  typedef typename OutputMeshType::PointsContainerIterator        OutputPointsContainerIterator;
  typedef typename OutputMeshType::PointsContainerConstIterator   OutputPointsContainerConstIterator;

  itkStaticConstMacro( PointDimension, unsigned int, OutputMeshType::PointDimension );

  /** Transform typedef. */
  typedef Transform<double, 
    itkGetStaticConstMacro(PointDimension), 
    itkGetStaticConstMacro(PointDimension)>         TransformType;
  typedef typename TransformType::ConstPointer      TransformPointerType;

  /** Interpolator typedef. */
  typedef LinearInterpolateDeformationFieldMeshFunction< 
    ReferenceMeshType, InputMeshType >                    InterpolatorType;
  typedef typename InterpolatorType::Pointer              InterpolatorPointerType;


  /** Set Mesh whose grid will define the geometry and topology of the output Mesh.
   *  In a registration scenario, this will typically be the Fixed mesh. */
  void SetReferenceMesh ( const ReferenceMeshType * mesh );
  const ReferenceMeshType * GetReferenceMesh( void ) const;

  /** Set the coordinate transformation.
   * Set the coordinate transform to use for resampling.  Note that this must
   * be in physical coordinates and it is the output-to-input transform, NOT
   * the input-to-output transform that you might naively expect.  By default
   * the filter uses an Identity transform. You must provide a different
   * transform here, before attempting to run the filter, if you do not want to
   * use the default Identity transform. */
  itkSetConstObjectMacro( Transform, TransformType ); 

  /** Get a pointer to the coordinate transform. */
  itkGetConstObjectMacro( Transform, TransformType );

  /** Set the interpolator function.  The default is
   * itk::LinearInterpolateMeshFunction<InputMeshType, TInterpolatorPrecisionType>. Some
   * other options are itk::NearestNeighborInterpolateMeshFunction
   * (useful for binary masks and other images with a small number of
   * possible pixel values), and itk::BSplineInterpolateMeshFunction
   * (which provides a higher order of interpolation).  */
  itkSetObjectMacro( Interpolator, InterpolatorType );

  /** Get a pointer to the interpolator function. */
  itkGetConstObjectMacro( Interpolator, InterpolatorType );


protected:
  ResampleDestinationPointsQuadEdgeMeshFilter();
  ~ResampleDestinationPointsQuadEdgeMeshFilter();

  void GenerateData();

private:

  ResampleDestinationPointsQuadEdgeMeshFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented


  TransformPointerType     m_Transform;         // Coordinate transform to use
  InterpolatorPointerType  m_Interpolator;      // Image function for

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkResampleDestinationPointsQuadEdgeMeshFilter.txx"
#endif

#endif
