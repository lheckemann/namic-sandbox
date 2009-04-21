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
#ifndef __itkQuadEdgeMeshPixelValuesSmoothingFilter_h
#define __itkQuadEdgeMeshPixelValuesSmoothingFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"

namespace itk
{

/**
 * \class QuadEdgeMeshPixelValuesSmoothingFilter
 * \brief This filter smooths the pixel values associated with points.
 *
 * This filter was based on the filter provided by 
 * Arnaud Gelas, Alex Gouaillard and Sean Megason in their Insight Journal paper
 * http://hdl.handle.net/1926/1518
 * http://www.insight-journal.org/browse/publication/313
 *
 * The difference between this current filter and the one above is that this
 * filter smooths the values associated with the points (PointData) without
 * changing the actual positions of the points in space, while the filter above
 * smooths the point positions while leaving unchanged the pixel values
 * associated with the points.
 *
 */
template< class TInputMesh, class TOutputMesh >
class QuadEdgeMeshPixelValuesSmoothingFilter :
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
{
public:
  typedef QuadEdgeMeshPixelValuesSmoothingFilter        Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< 
    TInputMesh, TOutputMesh >                           Superclass;
  typedef SmartPointer< Self >                          Pointer;
  typedef SmartPointer< const Self >                    ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( QuadEdgeMeshPixelValuesSmoothingFilter, QuadEdgeMeshToQuadEdgeMeshFilter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TInputMesh                                       InputMeshType;
  typedef typename InputMeshType::Pointer                  InputMeshPointer;

  typedef TOutputMesh                                      OutputMeshType;
  typedef typename OutputMeshType::Pointer                 OutputMeshPointer;
  typedef typename OutputMeshType::EdgeCellType            OutputEdgeCellType;
  typedef typename OutputMeshType::PolygonCellType         OutputPolygonCellType;
  typedef typename OutputMeshType::QEType                  OutputQEType;
  typedef typename OutputMeshType::PointIdentifier         OutputPointIdentifier;
  typedef typename OutputMeshType::PointType               OutputPointType;
  typedef typename OutputPointType::VectorType             OutputVectorType;
  typedef typename OutputPointType::CoordRepType           OutputCoordType;
  typedef typename OutputMeshType::PointsContainer         OutputPointsContainer;
  typedef typename OutputMeshType::PointsContainerPointer  OutputPointsContainerPointer;
  typedef typename OutputMeshType::PointsContainerIterator OutputPointsContainerIterator;
  typedef typename OutputMeshType::CellsContainerPointer   OutputCellsContainerPointer;
  typedef typename OutputMeshType::CellsContainerIterator  OutputCellsContainerIterator;

  typedef typename NumericTraits< OutputCoordType >::RealType   RelaxationFactorType;

  itkStaticConstMacro( PointDimension, unsigned int, OutputMeshType::PointDimension );

  typedef MatrixCoefficients< OutputMeshType >                CoefficientsComputation;

  void SetCoefficientsMethod( CoefficientsComputation* iMethod )
    { m_CoefficientsMethod = iMethod; }

  itkSetMacro( MaximumNumberOfIterations, unsigned long );
  itkGetMacro( MaximumNumberOfIterations, unsigned long );

  itkSetMacro( RelaxationFactor, RelaxationFactorType );
  itkGetMacro( RelaxationFactor, RelaxationFactorType );

protected:
  QuadEdgeMeshPixelValuesSmoothingFilter();
  ~QuadEdgeMeshPixelValuesSmoothingFilter();

  CoefficientsComputation* m_CoefficientsMethod;

  void GenerateData();

private:

  QuadEdgeMeshPixelValuesSmoothingFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  unsigned long             m_MaximumNumberOfIterations;
  RelaxationFactorType      m_RelaxationFactor;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshPixelValuesSmoothingFilter.txx"
#endif

#endif
