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
#ifndef __itkNormalizeScalarsQuadEdgeMeshFilter_txx
#define __itkNormalizeScalarsQuadEdgeMeshFilter_txx

#include "itkNormalizeScalarsQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkNumericTraitsVectorPixel.h"

namespace itk
{


template< class TMesh >
NormalizeScalarsQuadEdgeMeshFilter< TMesh >
::NormalizeScalarsQuadEdgeMeshFilter()
{
  this->SetNumberOfRequiredInputs( 1 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );

  this->SetNthOutput( 0, OutputMeshType::New() );
}


template< class TMesh >
NormalizeScalarsQuadEdgeMeshFilter< TMesh >
::~NormalizeScalarsQuadEdgeMeshFilter()
{
}


template< class TMesh >
void
NormalizeScalarsQuadEdgeMeshFilter< TMesh >
::SetInputMesh( const InputMeshType * mesh )
{
  itkDebugMacro("setting input mesh to " << mesh);
  if( mesh != static_cast<const InputMeshType *>(this->ProcessObject::GetInput( 0 )) )
    {
    this->ProcessObject::SetNthInput(0, const_cast< InputMeshType *>( mesh ) );
    this->Modified();
    }
}


template< class TMesh >
const typename 
NormalizeScalarsQuadEdgeMeshFilter< TMesh >::InputMeshType *
NormalizeScalarsQuadEdgeMeshFilter< TMesh >
::GetInputMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const InputMeshType * inputMesh = 
    static_cast<const InputMeshType *>( surrogate->ProcessObject::GetInput(0) );

  for ( unsigned int i = 0; i < this->m_NumberOfIterations; i++ )
    {
    }

  return inputMesh;
}


template< class TMesh >
void
NormalizeScalarsQuadEdgeMeshFilter< TMesh >
::GenerateData()
{

  // Copy the input mesh into the output mesh.
  this->CopyInputMeshToOutputMesh();
  
  OutputMeshType * outputMesh = this->GetOutput();
  
  //
  // Visit all nodes of the Mesh 
  //


}

} // end namespace itk

#endif
