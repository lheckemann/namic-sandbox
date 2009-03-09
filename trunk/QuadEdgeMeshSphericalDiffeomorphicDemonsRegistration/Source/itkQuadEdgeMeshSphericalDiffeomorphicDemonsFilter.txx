/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ITKHeader.h,v $
  Language:  C++
  Date:      $Date: 2006-04-25 23:20:16 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx
#define __itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter_txx

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.txx"

namespace itk
{

template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TInputMesh, TOutputMesh >::
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );
  this->SetNthOutput( 0, OutputMeshType::New() );
}

template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TInputMesh, TOutputMesh >::
~QuadEdgeMeshSphericalDiffeomorphicDemonsFilter()
{
}

template< class TInputMesh, class TOutputMesh >
void
QuadEdgeMeshSphericalDiffeomorphicDemonsFilter< TInputMesh, TOutputMesh >::
GenerateData( )
{
  InputMeshPointer input = this->GetInput();

  OutputMeshPointer output0 = this->GetOutput( 0 );

  output0->SetCellsAllocationMethod(
      OutputMeshType::CellsAllocatedDynamicallyCellByCell );

}

}

#endif
