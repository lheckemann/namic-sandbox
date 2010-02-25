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
#ifndef __itkWarpQuadEdgeMeshFilter_txx
#define __itkWarpQuadEdgeMeshFilter_txx

#include "itkWarpQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkNumericTraitsVectorPixel.h"

namespace itk
{


template< class TInputMesh, class TVectorMesh, class TOutputMesh >
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
::WarpQuadEdgeMeshFilter()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );

  this->SetNthOutput( 0, OutputMeshType::New() );

  this->m_Interpolator = InterpolatorType::New();
  this->m_Interpolator->SetUseNearestNeighborInterpolationAsBackup(true);
}


template< class TInputMesh, class TVectorMesh, class TOutputMesh >
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
::~WarpQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TVectorMesh, class TOutputMesh >
void
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
::SetInputMesh( const InputMeshType * mesh )
{
  itkDebugMacro("setting input mesh to " << mesh);
  if( mesh != static_cast<const InputMeshType *>(this->ProcessObject::GetInput( 1 )) )
    {
    this->ProcessObject::SetNthInput(1, const_cast< InputMeshType *>( mesh ) );
    this->Modified();
    }
}


template< class TInputMesh, class TVectorMesh, class TOutputMesh >
const typename 
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >::InputMeshType *
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
::GetInputMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const InputMeshType * deformationMesh = 
    static_cast<const InputMeshType *>( surrogate->ProcessObject::GetInput(1) );
  return deformationMesh;
}


template< class TInputMesh, class TVectorMesh, class TOutputMesh >
void
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
::SetDeformationMesh( const VectorMeshType * mesh )
{
  itkDebugMacro("setting input deformation mesh to " << mesh);
  if( mesh != static_cast<const VectorMeshType *>(this->ProcessObject::GetInput( 2 )) )
    {
    this->ProcessObject::SetNthInput(2, const_cast< VectorMeshType *>( mesh ) );
    this->Modified();
    }
}


template< class TInputMesh, class TVectorMesh, class TOutputMesh >
const typename 
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
::GetDeformationMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const VectorMeshType * deformationMesh = 
    static_cast<const VectorMeshType *>( surrogate->ProcessObject::GetInput(2) );
  return deformationMesh;
}


template< class TInputMesh, class TVectorMesh, class TOutputMesh >
void
WarpQuadEdgeMeshFilter< TInputMesh, TVectorMesh, TOutputMesh >
::GenerateData()
{
  this->CopyInputMeshToOutputMesh();

  typedef typename OutputMeshType::PointsContainer   OutputPointsContainer;

  const VectorMeshType * deformationMesh = this->GetDeformationMesh();

  if( !deformationMesh )
    {
    itkExceptionMacro("deformation mesh is missing");
    }


  OutputMeshType outputMesh = this->GetOutput();

  typedef typename OutputMeshType::PointsContainer   OutputPointsContainerPointer;

  OutputPointsContainerPointer outputPoints = outputMesh->GetPoints();

  const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

  ProgressReporter progress(this, 0, numberOfPoints);

  this->m_Interpolator->SetInputMesh( deformationMesh );
  this->m_Interpolator->Initialize();

  typedef typename OutputPointsContainerPointer::ConstIterator   OutputPointIterator;

  OutputPointIterator outputPointItr = outputPoints->Begin();
  OutputPointIterator outputPointEnd = outputPoints->End();

  while( outputPointItr != outputPointEnd )
    {
    this->m_Interpolator->Evaluate( inputPoints, outputPointItr.Value(), evaluatedPoint );

    outputPointItr.Value().CastFrom( evaluatedPoint );

    progress.CompletedPixel();

    ++outputPointItr;
    ++referenceItr;
    }
}

} // end namespace itk

#endif
