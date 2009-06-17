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
#ifndef __itkQuadEdgeMeshPixelValuesSmoothingFilter_txx
#define __itkQuadEdgeMeshPixelValuesSmoothingFilter_txx

#include "itkQuadEdgeMeshPixelValuesSmoothingFilter.h"
#include "itkProgressReporter.h"

namespace itk
{


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::QuadEdgeMeshPixelValuesSmoothingFilter()
{
  this->m_Lambda = 1.0;
  this->m_MaximumNumberOfIterations = 10;
  this->m_SphereCenter.Fill( 0.0 );
}


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::~QuadEdgeMeshPixelValuesSmoothingFilter()
{
}

template< class TInputMesh, class TOutputMesh >
void
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::ParalelTransport( const InputPixelType & inputPixelValue, 
    InputPixelType & transportedPixelValue ) const
{
  // FIXME: temporary implementation, to be replaced with real parallel
  // transport.
  transportedPixelValue = inputPixelValue;

  this->m_SphereCenter;

}

template< class TInputMesh, class TOutputMesh >
void
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::GenerateData()
{
  OutputMeshPointer outputMesh = this->GetOutput();

  if( this->m_MaximumNumberOfIterations == 0 )
    {
    // Copy the input mesh into the output mesh.
    this->Superclass::GenerateData();
    }


  ProgressReporter progress(this, 0, this->m_MaximumNumberOfIterations);


  for( unsigned int iter = 0; iter < this->m_MaximumNumberOfIterations; ++iter )
    {
    
    //
    // Visit all nodes of the Mesh 
    //
    typedef typename OutputPointDataContainer::ConstIterator OutputPointDataIterator;
    OutputPointDataIterator pixelIterator = outputMesh->GetPointData()->Begin();
    OutputPointDataIterator pixelEnd      = outputMesh->GetPointData()->End();

    typedef typename OutputPointsContainer::Iterator     PointIterator;
    PointIterator pointIterator = outputMesh->GetPoints()->Begin();
    PointIterator pointEnd      = outputMesh->GetPoints()->End();

    typedef typename OutputMeshType::QEPrimal    EdgeType;

    const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

    for( unsigned int pointId = 0; pointId < numberOfPoints; pointId++ )
      {
      EdgeType * edge1 = outputMesh->FindEdge( pointId );
      }

    while( pixelIterator != pixelEnd  && pointIterator != pointEnd ) 
      {
     
      //
      //  Parallel transport all its neigbors
      // 
      pointIterator.Value() = pointIterator.Value() + pixelIterator.Value();
      ++pixelIterator;
      ++pointIterator;
      }

    progress.CompletedPixel();  // potential exception thrown here
    }
}

} // end namespace itk

#endif
