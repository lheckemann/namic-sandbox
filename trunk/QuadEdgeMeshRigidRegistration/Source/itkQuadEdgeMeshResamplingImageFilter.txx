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
#ifndef __itkQuadEdgeMeshResamplingImageFilter_txx
#define __itkQuadEdgeMeshResamplingImageFilter_txx

#include "itkQuadEdgeMeshResamplingImageFilter.h"
#include "itkProgressReporter.h"
#include "itkVersor.h"
#include "itkNumericTraitsVectorPixel.h"

namespace itk
{


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshResamplingImageFilter< TInputMesh, TOutputMesh >
::QuadEdgeMeshResamplingImageFilter()
{
}


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshResamplingImageFilter< TInputMesh, TOutputMesh >
::~QuadEdgeMeshResamplingImageFilter()
{
}


template< class TInputMesh, class TOutputMesh >
void
QuadEdgeMeshResamplingImageFilter< TInputMesh, TOutputMesh >
::GenerateData()
{
  // Copy the input mesh into the output mesh.
  this->CopyInputMeshToOutputMesh();

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


  ProgressReporter progress(this, 0, this->m_MaximumNumberOfIterations);

  const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

  std::cout << "Output Mesh numberOfPoints " << numberOfPoints << std::endl;

}

} // end namespace itk

#endif
