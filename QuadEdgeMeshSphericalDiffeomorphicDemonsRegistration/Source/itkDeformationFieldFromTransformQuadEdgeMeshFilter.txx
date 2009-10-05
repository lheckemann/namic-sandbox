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
#ifndef __itkDeformationFieldFromTransformQuadEdgeMeshFilter_txx
#define __itkDeformationFieldFromTransformQuadEdgeMeshFilter_txx

#include "itkDeformationFieldFromTransformQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkVersor.h"
#include "itkNumericTraitsVectorPixel.h"

namespace itk
{


template< class TInputMesh, class TOutputMesh >
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::DeformationFieldFromTransformQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TOutputMesh >
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::~DeformationFieldFromTransformQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TOutputMesh >
void
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::SetReferenceMesh( const TOutputMesh * mesh )
{
  itkDebugMacro("setting input ReferenceMesh to " << mesh);
  if( mesh != static_cast<const TOutputMesh *>(this->ProcessObject::GetInput( 1 )) )
    {
    this->ProcessObject::SetNthInput(1, const_cast< TOutputMesh *>( mesh ) );
    this->Modified();
    }
}


template< class TInputMesh, class TOutputMesh >
const typename DeformationFieldFromTransformQuadEdgeMeshFilter<TInputMesh,TOutputMesh>::OutputMeshType *
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::GetReferenceMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const OutputMeshType * referenceMesh = 
    static_cast<const OutputMeshType *>(surrogate->ProcessObject::GetInput(1));
  return referenceMesh;
}


template< class TInputMesh, class TOutputMesh >
void
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::GenerateData()
{
  // Copy the input mesh into the output mesh.
  this->CopyReferenceMeshToOutputMesh();

  OutputMeshPointer outputMesh = this->GetOutput();

  //
  // Visit all nodes of the Mesh 
  //
  typedef typename OutputPointDataContainer::ConstIterator OutputPointDataIterator;

  OutputPointsContainerPointer points = outputMesh->GetPoints();

  if( points.IsNull() )
    {
    itkExceptionMacro("Mesh has NULL PointData");
    }

  OutputPointDataContainerPointer pointData = outputMesh->GetPointData();

  if( pointData.IsNull() )
    {
    itkExceptionMacro("Output Mesh has NULL PointData");
    }

  const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

  ProgressReporter progress(this, 0, numberOfPoints);

  std::cout << "Output Mesh numberOfPoints " << numberOfPoints << std::endl;

  typedef typename OutputMeshType::PointsContainer::ConstIterator    PointIterator;
  typedef typename OutputMeshType::PointDataContainer::Iterator      PointDataIterator;

  PointIterator pointItr = outputMesh->GetPoints()->Begin();
  PointIterator pointEnd = outputMesh->GetPoints()->End();

  PointDataIterator pointDataItr = outputMesh->GetPointData()->Begin();
  PointDataIterator pointDataEnd = outputMesh->GetPointData()->End();

  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    MappedPointType;

  OutputPointType  inputPoint;
  OutputPointType  pointToEvaluate;

  while( pointItr != pointEnd && pointDataItr != pointDataEnd )
    {
    inputPoint.CastFrom( pointItr.Value() );

    MappedPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    pointToEvaluate.CastFrom( transformedPoint );
    pointDataItr.Value() = this->m_Interpolator->Evaluate( pointToEvaluate );

    progress.CompletedPixel();

    ++pointItr;
    ++pointDataItr;
    }

}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMesh()
{
  this->CopyReferenceMeshToOutputMeshGeometry();
  this->CopyReferenceMeshToOutputMeshFieldData();
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshGeometry()
{
  this->CopyReferenceMeshToOutputMeshPoints();
  this->CopyReferenceMeshToOutputMeshEdgeCells();
  this->CopyReferenceMeshToOutputMeshCells();
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshFieldData()
{
  this->CopyReferenceMeshToOutputMeshPointData();
  this->CopyReferenceMeshToOutputMeshCellData();
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshPoints()
{
  const OutputMeshType * in = this->GetReferenceMesh();
  OutputMeshType * out = this->GetOutput();

  CopyMeshToMeshPoints( in, out );
}


// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshEdgeCells()
{
  const OutputMeshType * in = this->GetReferenceMesh();
  OutputMeshType * out = this->GetOutput();

  CopyMeshToMeshEdgeCells( in, out );
}


// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshCells()
{
  const OutputMeshType * in = this->GetReferenceMesh();
  OutputMeshType * out = this->GetOutput();

  CopyMeshToMeshCells( in, out );
}


// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshPointData()
{
  const OutputMeshType * in = this->GetReferenceMesh();
  OutputMeshType * out = this->GetOutput();

  CopyMeshToMeshPointData( in, out );
}


// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void 
DeformationFieldFromTransformQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CopyReferenceMeshToOutputMeshCellData()
{
  const OutputMeshType * in = this->GetReferenceMesh();
  OutputMeshType * out = this->GetOutput();

  CopyMeshToMeshCellData( in, out );
}

} // end namespace itk

#endif
