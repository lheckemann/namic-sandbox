/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkShiftScalarsQuadEdgeMeshFilter.h,v $
  Language:  C++
  Date:      $Date: 2010-09-17 16:26:05 $
  Version:   $Revision: 1.0 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShiftScalarsQuadEdgeMeshFilter_txx
#define __itkShiftScalarsQuadEdgeMeshFilter_txx

#include "itkShiftScalarsQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"


namespace itk
{


template< class TInputMesh, class TOutputMesh  >
ShiftScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::ShiftScalarsQuadEdgeMeshFilter()
{
  this->SetNumberOfRequiredInputs( 1 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );

  this->SetNthOutput( 0, OutputMeshType::New() );
  
  this->m_Scale = 1.0;
  this->m_Shift = 0.0;
}


template< class TInputMesh, class TOutputMesh  >
ShiftScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::~ShiftScalarsQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TOutputMesh  >
void
ShiftScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::SetInputMesh( const InputMeshType * mesh )
{
  itkDebugMacro("setting input mesh to " << mesh);
  if( mesh != static_cast<const InputMeshType *>(this->ProcessObject::GetInput( 0 )) )
    {
    this->ProcessObject::SetNthInput(0, const_cast< InputMeshType *>( mesh ) );
    this->Modified();
    }
}


template< class TInputMesh, class TOutputMesh  >
const typename 
ShiftScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >::InputMeshType *
ShiftScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::GetInputMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const InputMeshType * inputMesh = 
    static_cast<const InputMeshType *>( surrogate->ProcessObject::GetInput(0) );

  return inputMesh;
}


template< class TInputMesh, class TOutputMesh  >
void
ShiftScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::GenerateData()
{

  // Copy the input mesh into the output mesh.
  this->CopyInputMeshToOutputMesh();
  
  OutputMeshType * outputMesh = this->GetOutput();
  
  const InputMeshType * inputMesh = this->GetInputMesh();
    
    
  //
  // Visit all nodes of the input mesh and output mesh
  //
  InputPointDataContainerConstPointer inputPointData = inputMesh->GetPointData(); 
  typedef typename InputPointDataContainer::ConstIterator  InputPointDataIterator;
  
  InputPointDataIterator inputPointDataItr = inputPointData->Begin();
  InputPointDataIterator inputPointDataEnd = inputPointData->End();
  
  OutputPointDataContainerPointer outputPointData = outputMesh->GetPointData(); 
  typedef typename OutputPointDataContainer::Iterator  outputPointDataIterator;
  
  outputPointDataIterator outputPointDataItr = outputPointData->Begin();
  outputPointDataIterator outputPointDataEnd = outputPointData->End();
  
  double scalarValue;
  
  while( outputPointDataItr != outputPointDataEnd )
    {
    
    scalarValue = ( static_cast<double> (inputPointDataItr.Value()) + this->m_Shift ) * this->m_Scale;
    if (scalarValue < NumericTraits<OutputPixelType>::NonpositiveMin())
      {
      outputPointDataItr.Value() = NumericTraits<OutputPixelType>::NonpositiveMin();
      }
    else if (scalarValue > NumericTraits<OutputPixelType>::max())
      {
      outputPointDataItr.Value() = NumericTraits<OutputPixelType>::max();
      }
    else
      {
      outputPointDataItr.Value() = static_cast<OutputPixelType>( scalarValue );
      }
      
    ++outputPointDataItr;
    ++inputPointDataItr;

    }

}

} // end namespace itk

#endif
