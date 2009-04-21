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

namespace itk
{


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::QuadEdgeMeshPixelValuesSmoothingFilter()
{
  this->m_CoefficientsMethod = 0;
  this->m_MaximumNumberOfIterations = 1;
  this->m_RelaxationFactor = static_cast< RelaxationFactorType >( 1.0 );
}


template< class TInputMesh, class TOutputMesh >
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::~QuadEdgeMeshPixelValuesSmoothingFilter()
{
}


template< class TInputMesh, class TOutputMesh >
void
QuadEdgeMeshPixelValuesSmoothingFilter< TInputMesh, TOutputMesh >
::GenerateData()
{
  OutputMeshPointer mesh;

  OutputPointsContainerPointer temp = OutputPointsContainer::New();

  temp->Reserve( this->GetInput()->GetNumberOfPoints() );

  OutputPointsContainerPointer points;
  OutputPointsContainerIterator it;
  OutputPointType p, q, r;
  OutputVectorType v;
  OutputCoordType coeff, sum_coeff, den;
  OutputQEType* qe;
  OutputQEType* qe_it;

  if( this->m_MaximumNumberOfIterations == 0 )
    {
    Superclass::GenerateData();
    }
  else
    {
    mesh = this->GetInput();
    }

  for( unsigned int iter = 0; iter < m_MaximumNumberOfIterations; ++iter )
    {
    }
}

} // end namespace itk

#endif
