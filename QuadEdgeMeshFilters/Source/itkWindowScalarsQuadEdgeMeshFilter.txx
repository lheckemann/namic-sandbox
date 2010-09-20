/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWindowScalarsQuadEdgeMeshFilter.h,v $
  Language:  C++
  Date:      $Date: 2010-05-25 16:26:05 $
  Version:   $Revision: 1.0 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWindowScalarsQuadEdgeMeshFilter_txx
#define __itkWindowScalarsQuadEdgeMeshFilter_txx

#include "itkWindowScalarsQuadEdgeMeshFilter.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"


namespace itk
{


template< class TInputMesh, class TOutputMesh  >
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::WindowScalarsQuadEdgeMeshFilter()
{
  this->SetNumberOfRequiredInputs( 1 );
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNumberOfOutputs( 1 );

  this->SetNthOutput( 0, OutputMeshType::New() );
  
  this->m_OutputMaximum = NumericTraits<OutputPixelType>::max();
  this->m_OutputMinimum = NumericTraits<OutputPixelType>::NonpositiveMin();
  
  this->m_InputMaximum = NumericTraits<InputPixelType>::min();
  this->m_InputMinimum = NumericTraits<InputPixelType>::max();
  
  this->m_WindowMaximum = NumericTraits<InputPixelType>::Zero;
  this->m_WindowMinimum = NumericTraits<InputPixelType>::Zero;
  
  this->m_Scale = 1.0;
  this->m_Shift = 0.0;
}


template< class TInputMesh, class TOutputMesh  >
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::~WindowScalarsQuadEdgeMeshFilter()
{
}


template< class TInputMesh, class TOutputMesh  >
void
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
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
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >::InputMeshType *
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::GetInputMesh() const
{
  Self * surrogate = const_cast< Self * >( this );
  const InputMeshType * inputMesh = 
    static_cast<const InputMeshType *>( surrogate->ProcessObject::GetInput(0) );

  return inputMesh;
}


template< class TInputMesh, class TOutputMesh  >
void 
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::SetWindowLevel(const InputPixelType& window, const InputPixelType& level) 
{

  double tmp1, tmp2;

  tmp1 = static_cast<double>(level)
    - (static_cast<double>(window)/2.0);
  if (tmp1 < NumericTraits<double>::NonpositiveMin())
    {
    tmp1 = 0.0;
    }

  tmp2 = static_cast<double>(level)
    + (static_cast<double>(window)/2.0);
  if (tmp2 > NumericTraits<double>::max())
    {
    tmp2 = NumericTraits<double>::max();
    }
  
  this->m_WindowMinimum = static_cast<InputPixelType>(tmp1);
  this->m_WindowMaximum = static_cast<InputPixelType>(tmp2);
}


template< class TInputMesh, class TOutputMesh  >
typename
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >::InputPixelType
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::GetWindow() const
{
  return this->m_WindowMaximum - this->m_WindowMinimum;
}


template< class TInputMesh, class TOutputMesh  >
typename 
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >::InputPixelType
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::GetLevel() const
{
  return (this->m_WindowMaximum + this->m_WindowMinimum) / 2;
}


template< class TInputMesh, class TOutputMesh  >
void
WindowScalarsQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
::GenerateData()
{

  // Copy the input mesh into the output mesh.
  this->CopyInputMeshToOutputMesh();
  
  OutputMeshType * outputMesh = this->GetOutput();
  
  const InputMeshType * inputMesh = this->GetInputMesh();
 
  //
  // Visit all nodes of the input mesh 
  //
  InputPointDataContainerConstPointer inputPointData = inputMesh->GetPointData(); 
  typedef typename InputPointDataContainer::ConstIterator  InputPointDataIterator;
  
  InputPointDataIterator inputPointDataItr = inputPointData->Begin();
  InputPointDataIterator inputPointDataEnd = inputPointData->End();
  
  while( inputPointDataItr != inputPointDataEnd )
    {
    if (inputPointDataItr.Value() > this->m_InputMaximum)
      {
      this->m_InputMaximum = inputPointDataItr.Value();
      }
      
    if (inputPointDataItr.Value() < this->m_InputMinimum)
      {
      this->m_InputMinimum = inputPointDataItr.Value();
      }
      
    ++inputPointDataItr;
    }
   
    
  //
  // Calculate Scale and Shift for linear transformation
  //
  this->m_Scale = 
    (static_cast<double>( this->m_OutputMaximum )
     - static_cast<double>( this->m_OutputMinimum )) /
    (static_cast<double>( this->m_WindowMaximum )
     - static_cast<double>( this->m_WindowMinimum ));

  this->m_Shift =
    static_cast<double>( this->m_OutputMinimum ) - 
    static_cast<double>( this->m_WindowMinimum ) * this->m_Scale;
    
    
  //
  // Visit all nodes of the output mesh 
  //
  OutputPointDataContainerPointer outputPointData = outputMesh->GetPointData(); 
  typedef typename OutputPointDataContainer::Iterator  outputPointDataIterator;
  
  outputPointDataIterator outputPointDataItr = outputPointData->Begin();
  outputPointDataIterator outputPointDataEnd = outputPointData->End();
  
  while( outputPointDataItr != outputPointDataEnd )
    {
    
    if ( outputPointDataItr.Value() < static_cast<OutputPixelType> (this->m_WindowMinimum) )
      {
      outputPointDataItr.Value() = static_cast<OutputPixelType> (this->m_WindowMinimum);
      }
    else if ( outputPointDataItr.Value() > static_cast<OutputPixelType> (this->m_WindowMaximum) )
      {
      outputPointDataItr.Value() = static_cast<OutputPixelType> (this->m_WindowMaximum);
      }
    
    const double newScalarValue = static_cast<double>( outputPointDataItr.Value() ) * this->m_Scale + this->m_Shift;
    outputPointDataItr.Value() = static_cast<OutputPixelType> (newScalarValue);
      
    ++outputPointDataItr;
    }

}

} // end namespace itk

#endif
