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
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"

#include <vector>
#include <algorithm>

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

  this->m_NumberOfIterations = 2;
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
  OutputPointDataContainer * pointData = outputMesh->GetPointData();

  typedef typename OutputPointDataContainer::Iterator  PointDataIterator;

  const unsigned int length = outputMesh->GetNumberOfPoints();

  std::cout << "Number of Points " << length << std::endl;

  typedef typename TMesh::PixelType  PixelType;

  typedef std::vector< PixelType >   VectorType;

  VectorType data;

  data.resize( length );

  for ( unsigned int i = 0; i < this->m_NumberOfIterations; i++ )
    {
    PointDataIterator pointItr = pointData->Begin();
    PointDataIterator pointEnd = pointData->End();

    while( pointItr != pointEnd )
      {
      data.push_back( pointItr.Value() );
      ++pointItr;
      }

    std::nth_element( data.begin(), data.begin()+length/2, data.end() );

    PixelType median = data[length/2];

    std::cout << "median = " << median << std::endl;

    pointItr = pointData->Begin();
    pointEnd = pointData->End();

    typedef typename NumericTraits< PixelType >::RealType PixelRealType;

    PixelRealType sum  = NumericTraits< PixelRealType >::Zero;
    PixelRealType sum2 = NumericTraits< PixelRealType >::Zero;

    while( pointItr != pointEnd )
      {
      PixelType centeredValue = pointItr.Value() - median;
      sum += centeredValue;
      sum2 += centeredValue * centeredValue;
      ++pointItr;
      }

    const PixelRealType mean = sum / length;
    const PixelRealType variance = ( sum2 - mean * mean ) / length;
    const PixelRealType standardDeviation = vcl_sqrt( variance );

    std::cout << "mean = " << mean << std::endl;
    std::cout << "variance = " << variance << std::endl;
    std::cout << "standardDeviation = " << standardDeviation << std::endl;

    pointItr = pointData->Begin();
    pointEnd = pointData->End();

    unsigned int countAbove = 0;
    unsigned int countBelow = 0;

    while( pointItr != pointEnd )
      {
      PixelRealType value = pointItr.Value();

      value /= standardDeviation;

      if( value > 3.0 )
        {
        countAbove++;
        }

      if( value < -3.0 )
        {
        countBelow++;
        }

      ++pointItr;
      }
 
    std::cout << "countAbove = " << countAbove << std::endl;
    std::cout << "countBelow = " << countBelow << std::endl;
    }
}

} // end namespace itk

#endif
