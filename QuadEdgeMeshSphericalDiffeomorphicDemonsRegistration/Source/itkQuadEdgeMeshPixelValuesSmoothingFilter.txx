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
#include "itkVersor.h"

namespace itk
{


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::QuadEdgeMeshPixelValuesSmoothingFilter()
{
  this->m_Lambda = 1.0;
  this->m_MaximumNumberOfIterations = 10;
  this->m_SphereCenter.Fill( 0.0 );
  this->m_SphereRadius = 1.0;
}


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::~QuadEdgeMeshPixelValuesSmoothingFilter()
{
}

template< class TInputMesh, class TOutputMesh >
void
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::ParalelTransport( 
    const OutputPointType src, const OutputPointType dst,
    const InputPixelType & inputPixelValue, 
    InputPixelType & transportedPixelValue ) const
{
  OutputVectorType vsrc = src - this->m_SphereCenter;
  OutputVectorType vdst = dst - this->m_SphereCenter;

  OutputVectorType axis = CrossProduct( vsrc, vdst );

  double sinus   = axis.GetNorm();
  double cosinus = vsrc * vdst;

  double angle = atan2( sinus, cosinus );
  
  typedef Versor< double > VersorType;

  VersorType versor;
  versor.Set( axis, angle );

  std::cout << "vsrc = " << vsrc << std::endl;
  std::cout << "vdst = " << vdst << std::endl;
  std::cout << "axis = " << axis << std::endl;
  std::cout << "angl = " << angle << std::endl;

  OutputVectorType vdst2 = versor.Transform( vsrc );
  std::cout << "vdst2 = " << vdst2 << std::endl;

  std::cout << std::endl << std::endl;

  transportedPixelValue = vdst2;
}

template< class TInputMesh, class TOutputMesh >
void
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::GenerateData()
{
std::cout << "QuadEdgeMeshPixelValuesSmoothingFilter::GenerateData() " << std::endl;
std::cout << "Number of Iterations " << this->m_MaximumNumberOfIterations << std::endl;

std::cout << "Input Mesh " << std::endl;
this->GetInput()->Print( std::cout );

  // Copy the input mesh into the output mesh.
  this->CopyInputMeshToOutputMesh();

  OutputMeshPointer outputMesh = this->GetOutput();

std::cout << "Mesh after copying " << std::endl;
  outputMesh->Print( std::cout );

  ProgressReporter progress(this, 0, this->m_MaximumNumberOfIterations);


  for( unsigned int iter = 0; iter < this->m_MaximumNumberOfIterations; ++iter )
    {
std::cout << " Smoothing Iteration " << iter << std::endl;
    
    //
    // Visit all nodes of the Mesh 
    //
    typedef typename OutputPointDataContainer::ConstIterator OutputPointDataIterator;

    OutputPointsContainerPointer points = outputMesh->GetPoints();
    OutputPointDataContainerPointer pointData = outputMesh->GetPointData();

    OutputPointDataIterator pixelIterator = outputMesh->GetPointData()->Begin();
    OutputPointDataIterator pixelEnd      = outputMesh->GetPointData()->End();

    typedef typename OutputPointsContainer::Iterator     PointIterator;
    PointIterator pointIterator = points->Begin();
    PointIterator pointEnd      = points->End();

    typedef typename OutputMeshType::QEPrimal    EdgeType;

    const unsigned int numberOfPoints = outputMesh->GetNumberOfPoints();

std::cout << "Output Mesh numberOfPoints " << numberOfPoints << std::endl;

    for( unsigned int pointId = 0; pointId < numberOfPoints; pointId++ )
      {
std::cout << "Smoothing point " << pointId << std::endl;
      EdgeType * edge1 = outputMesh->FindEdge( pointId );

      OutputPointIdentifier pointId2 = edge1->GetDestination();

      const OutputPointType & point0 = points->GetElement( pointId );
      const OutputPointType & point2 = points->GetElement( pointId2 );

      const OutputPixelType & pixelValue2 = pointData->GetElement( pointId2 );
      OutputPixelType transportedPixelValue;

      this->ParalelTransport( point2, point0, pixelValue2, transportedPixelValue );

      std::cout << "source      pixel = " << pixelValue2 << std::endl;
      std::cout << "destination pixel = " << transportedPixelValue << std::endl;

      }

    progress.CompletedPixel();  // potential exception thrown here
    }
}

} // end namespace itk

#endif
