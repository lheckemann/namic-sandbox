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
::GenerateData()
{
  // Copy the input mesh into the output mesh.
  // FIXME this->CopyReferenceMeshToOutputMesh();

  OutputMeshPointer outputMesh = this->GetOutput();

  //
  // Visit all nodes of the Mesh 
  //
  typedef typename OutputPointDataContainer::ConstIterator OutputPointDataIterator;

  OutputPointsContainerPointer points = outputMesh->GetPoints();

  if( points.IsNull() )
    {
    itkExceptionMacro("Mesh has NULL Points");
    }

  const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

  ProgressReporter progress(this, 0, numberOfPoints);

  std::cout << "Output Mesh numberOfPoints " << numberOfPoints << std::endl;

  typedef typename OutputMeshType::PointsContainer::ConstIterator    PointIterator;
  typedef typename OutputMeshType::PointDataContainer::Iterator      PointDataIterator;

  PointIterator pointItr = outputMesh->GetPoints()->Begin();
  PointIterator pointEnd = outputMesh->GetPoints()->End();

  typedef typename TransformType::InputPointType     InputPointType;
  typedef typename TransformType::OutputPointType    MappedPointType;

  OutputPointType  inputPoint;
  OutputPointType  pointToEvaluate;

  while( pointItr != pointEnd )
    {
    inputPoint.CastFrom( pointItr.Value() );

    MappedPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    pointToEvaluate.CastFrom( transformedPoint );

    progress.CompletedPixel();

    ++pointItr;
    }

}


} // end namespace itk

#endif
