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
#ifndef __itkReplaceDestinationPointsQuadEdgeMeshFilter_txx
#define __itkReplaceDestinationPointsQuadEdgeMeshFilter_txx

#include "itkReplaceDestinationPointsQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkNumericTraitsVectorPixel.h"

namespace itk
{


template< class TInputMesh, class TInputPointSet, class TOutputMesh >
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >
::ReplaceDestinationPointsQuadEdgeMeshFilter()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );

  this->SetNthOutput( 0, OutputMeshType::New() );
}


template< class TInputMesh, class TInputPointSet, class TOutputMesh >
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >
::~ReplaceDestinationPointsQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TInputPointSet, class TOutputMesh >
void
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >
::SetInputMesh( const InputMeshType * mesh )
{
  itkDebugMacro("setting input Mesh to " << mesh);
  if( mesh != static_cast<const InputMeshType *>(this->ProcessObject::GetInput( 0 )) )
    {
    this->ProcessObject::SetNthInput(0, const_cast< InputMeshType *>( mesh ) );
    this->Modified();
    }
}


template< class TInputMesh, class TInputPointSet, class TOutputMesh >
const typename 
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >::InputMeshType *
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >
::GetInputMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const InputMeshType * referenceMesh = 
    static_cast<const InputMeshType *>( surrogate->ProcessObject::GetInput(0) );
  return referenceMesh;
}


template< class TInputMesh, class TInputPointSet, class TOutputMesh >
void
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >
::SetDestinationPoints( const InputPointSetType * destinationPointSet )
{
  itkDebugMacro("setting input ReferenceMesh to " << destinationPointSet);
  if( destinationPointSet != static_cast<const InputPointSetType *>(this->ProcessObject::GetInput( 1 )) )
    {
    this->ProcessObject::SetNthInput(1, const_cast< InputPointSetType *>( destinationPointSet ) );
    this->Modified();
    }
}


template< class TInputMesh, class TInputPointSet, class TOutputMesh >
const typename 
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >::InputPointSetType *
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >
::GetDestinationPoints() const
{
  Self * surrogate = const_cast< Self * >( this );
  const InputPointSetType * destinationPointSet = 
    static_cast<const InputPointSetType *>( surrogate->ProcessObject::GetInput(1) );
  return destinationPointSet;
}


template< class TInputMesh, class TInputPointSet, class TOutputMesh >
void
ReplaceDestinationPointsQuadEdgeMeshFilter< TInputMesh, TInputPointSet, TOutputMesh >
::GenerateData()
{
  this->CopyInputMeshToOutputMesh();

  const InputPointSetType * inputPointSet = this->GetDestinationPoints();

  if( !inputPointSet )
    {
    itkExceptionMacro("Input PointSet is missing");
    }

  typedef typename InputPointSetType::PointsContainer   DestinationPointsContainer;

  const DestinationPointsContainer * destinationPoints = inputPointSet->GetPoints();

  if( !destinationPoints )
    {
    itkExceptionMacro("Input PointSet has no points");
    }
 

  const unsigned int numberOfPoints = inputMesh->GetNumberOfPoints();

  if( destinationPoints->Size() != numberOfPoints )
    {
    itkExceptionMacro("The PointSet does not have the same number of points as the Mesh");
    }

  OutputMeshType * outputMesh = this->GetOutput();

  ProgressReporter progress(this, 0, numberOfPoints);

  typedef typename DestinationPointsContainer::ConstIterator   DestinationPointIterator;
  
  DestinationPointIterator destinationPointItr = destinationPoints->Begin();
  DestinationPointIterator destinationPointEnd = destinationPoints->End();

  typedef typename OutputMeshType::PointsContainer        OutputPointContainer;
  typedef typename OutputPointContainer::ConstIterator    OutputPointIterator;

  const OutputPointContainer * outputPoints = outputMesh->GetPoints();

  OutputPointIterator outputPointItr = outputPoints->Begin();

  typedef typename OutputMeshType::PointDataContainer    DisplacementVectorContainer;
  typedef typename DisplacementVectorContainer::Pointer  DisplacementVectorContainerPointer;
  typedef typename DisplacementVectorContainer::Iterator DisplacementVectorIterator;

  while( destinationPointItr != destinationPointEnd )
    {
    outputPointItr->SetPoint( destinationPointItr.Value() );
    ++outputPointItr;
    ++destinationPointItr;
    }
}

} // end namespace itk

#endif
