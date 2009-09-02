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
#ifndef __itkQuadEdgeMeshResamplingImageFilter_h
#define __itkQuadEdgeMeshResamplingImageFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"

namespace itk
{

/**
 * \class QuadEdgeMeshResamplingImageFilter
 * \brief This resamples the scalar values of one QuadEdgeMesh into another one via a user-provided Transform and Interpolator.
 *
 * \ingroup MeshFilters
 *
 */
template< class TInputMesh, class TOutputMesh >
class QuadEdgeMeshResamplingImageFilter :
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
{
public:
  typedef QuadEdgeMeshResamplingImageFilter             Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< 
    TInputMesh, TOutputMesh >                           Superclass;
  typedef SmartPointer< Self >                          Pointer;
  typedef SmartPointer< const Self >                    ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( QuadEdgeMeshResamplingImageFilter, QuadEdgeMeshToQuadEdgeMeshFilter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TInputMesh                                       InputMeshType;
  typedef typename InputMeshType::Pointer                  InputMeshPointer;
  typedef typename InputMeshType::PixelType                InputPixelType;
  typedef typename InputMeshType::PointDataContainer       InputPointDataContainer;

  typedef TOutputMesh                                        OutputMeshType;
  typedef typename OutputMeshType::Pointer                   OutputMeshPointer;
  typedef typename OutputMeshType::EdgeCellType              OutputEdgeCellType;
  typedef typename OutputMeshType::PolygonCellType           OutputPolygonCellType;
  typedef typename OutputMeshType::QEType                    OutputQEType;
  typedef typename OutputMeshType::PointIdentifier           OutputPointIdentifier;
  typedef typename OutputMeshType::PointType                 OutputPointType;
  typedef typename OutputPointType::VectorType               OutputVectorType;
  typedef typename OutputPointType::CoordRepType             OutputCoordType;
  typedef typename OutputMeshType::PointsContainer           OutputPointsContainer;
  typedef typename OutputMeshType::PointsContainerPointer    OutputPointsContainerPointer;
  typedef typename OutputMeshType::PointsContainerIterator   OutputPointsContainerIterator;
  typedef typename OutputMeshType::CellsContainerPointer     OutputCellsContainerPointer;
  typedef typename OutputMeshType::CellsContainerIterator    OutputCellsContainerIterator;
  typedef typename OutputMeshType::PointDataContainer        OutputPointDataContainer;
  typedef typename OutputMeshType::PointDataContainerPointer OutputPointDataContainerPointer;
  typedef typename OutputMeshType::PixelType                 OutputPixelType;

  itkStaticConstMacro( PointDimension, unsigned int, OutputMeshType::PointDimension );

protected:
  QuadEdgeMeshResamplingImageFilter();
  ~QuadEdgeMeshResamplingImageFilter();

  void GenerateData();

private:

  QuadEdgeMeshResamplingImageFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshResamplingImageFilter.txx"
#endif

#endif
