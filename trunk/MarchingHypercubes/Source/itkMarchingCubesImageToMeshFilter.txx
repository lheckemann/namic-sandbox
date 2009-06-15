/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkMarchingCubesImageToMeshFilter.txx,v $
Language:  C++
Date:      $Date: 2008-11-21 20:00:48 $
Version:   $Revision: 1.15 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMarchingCubesImageToMeshFilter_txx
#define __itkMarchingCubesImageToMeshFilter_txx


#include "itkMarchingCubesImageToMeshFilter.h"
#include "itkNumericTraits.h"
//sophie's include:


//#include <iostream.h>

namespace itk
{

// Constructor
template<class TInputImage, class TOutputMesh>
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::MarchingCubesImageToMeshFilter()
{
  this->SetNumberOfRequiredInputs(1);

  this->m_PointLocator       = PointLocatorType::New();

  this->m_SurfaceValue = 
    ( NumericTraits< InputPixelRealType >::max() - 
      NumericTraits< InputPixelRealType >::NonpositiveMin() ) / 2.0;

  this->m_GradientCalculator = GradientCalculatorType::New();
  this->m_ResampleFilter     = ResampleFilterType::New();
  this->m_Interpolator       = InterpolatorType::New();

  this->m_ResampleFilter->SetInterpolator( this->m_Interpolator );

  this->m_SubdivideFactors.Fill( 1 );
  
  this->m_GradientCalculator->UseImageDirectionOn();

  this->m_NumberOfPoints = NumericTraits< PointIdentifier >::Zero;
  this->m_NumberOfCells = NumericTraits< CellIdentifier >::Zero;

  this->m_EdgeIndexToVertexIndex[0] = VertexPairType( 0, 0 );
  
  this->m_EdgeIndexToVertexIndex[1] = VertexPairType( 0, 1 );
  this->m_EdgeIndexToVertexIndex[2] = VertexPairType( 0, 2 );
  this->m_EdgeIndexToVertexIndex[3] = VertexPairType( 1, 3 );
  this->m_EdgeIndexToVertexIndex[4] = VertexPairType( 2, 3 );

  this->m_EdgeIndexToVertexIndex[5] = VertexPairType( 0, 4 );
  this->m_EdgeIndexToVertexIndex[6] = VertexPairType( 1, 5 );
  this->m_EdgeIndexToVertexIndex[7] = VertexPairType( 2, 6 );
  this->m_EdgeIndexToVertexIndex[8] = VertexPairType( 3, 7 );

  this->m_EdgeIndexToVertexIndex[ 9] = VertexPairType( 4, 5 );
  this->m_EdgeIndexToVertexIndex[10] = VertexPairType( 4, 6 );
  this->m_EdgeIndexToVertexIndex[11] = VertexPairType( 5, 7 );
  this->m_EdgeIndexToVertexIndex[12] = VertexPairType( 6, 7 );
/*
  this->m_EdgeIndexToVertexIndex[1] = VertexPairType( 1, 2 );
  this->m_EdgeIndexToVertexIndex[2] = VertexPairType( 1, 3 );
  this->m_EdgeIndexToVertexIndex[3] = VertexPairType( 2, 4 );
  this->m_EdgeIndexToVertexIndex[4] = VertexPairType( 3, 4 );

  this->m_EdgeIndexToVertexIndex[5] = VertexPairType( 1, 5 );
  this->m_EdgeIndexToVertexIndex[6] = VertexPairType( 2, 6 );
  this->m_EdgeIndexToVertexIndex[7] = VertexPairType( 3, 7 );
  this->m_EdgeIndexToVertexIndex[8] = VertexPairType( 4, 8 );

  this->m_EdgeIndexToVertexIndex[ 9] = VertexPairType( 5, 6 );
  this->m_EdgeIndexToVertexIndex[10] = VertexPairType( 5, 7 );
  this->m_EdgeIndexToVertexIndex[11] = VertexPairType( 6, 8 );
  this->m_EdgeIndexToVertexIndex[12] = VertexPairType( 7, 8 );
*/


  // Populate the look up table here:
  ListOfTrianglesType marchingCubesLookupTable[256]=
     {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,1,2,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,6,3,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,6,3,5,3,2,0,0,0,0,0,0,0,0,0},
    {2,4,7,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,4,7,1,7,5,0,0,0,0,0,0,0,0,0},
    {2,4,7,3,1,6,0,0,0,0,0,0,0,0,0},
    {3,4,7,3,7,5,3,5,6,0,0,0,0,0,0},
    {3,8,4,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,5,4,3,8,0,0,0,0,0,0,0,0,0},
    {6,8,4,6,4,1,0,0,0,0,0,0,0,0,0},
    {4,2,5,4,5,6,4,6,8,0,0,0,0,0,0},
    {2,3,8,2,8,7,0,0,0,0,0,0,0,0,0},
    {1,3,8,1,8,7,1,7,5,0,0,0,0,0,0},
    {2,1,6,2,6,8,2,8,7,0,0,0,0,0,0},
    {7,5,6,7,6,8,0,0,0,0,0,0,0,0,0},
    {10,9,5,0,0,0,0,0,0,0,0,0,0,0,0},
    {9,1,2,9,2,10,0,0,0,0,0,0,0,0,0},
    {9,5,10,1,6,3,0,0,0,0,0,0,0,0,0},
    {9,6,3,9,3,2,9,2,10,0,0,0,0,0,0},
    {5,10,9,7,2,4,0,0,0,0,0,0,0,0,0},
    {7,10,9,7,9,1,7,1,4,0,0,0,0,0,0},
    {9,5,10,1,6,3,7,2,4,0,0,0,0,0,0},
    {9,6,3,9,3,4,9,4,10,4,7,10,0,0,0},
    {8,4,3,5,10,9,0,0,0,0,0,0,0,0,0},
    {4,3,8,1,2,10,1,10,9,0,0,0,0,0,0},
    {9,5,10,1,6,8,1,8,4,0,0,0,0,0,0},
    {9,6,8,9,8,4,9,4,10,4,2,10,0,0,0},
    {5,10,9,7,2,3,7,3,8,0,0,0,0,0,0},
    {7,10,9,7,9,1,7,1,8,1,3,8,0,0,0},
    {2,1,6,2,6,8,2,8,7,9,5,10,0,0,0},
    {9,6,8,9,8,7,9,7,10,0,0,0,0,0,0},
    {9,11,6,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,1,2,6,9,11,0,0,0,0,0,0,0,0,0},
    {1,9,11,1,11,3,0,0,0,0,0,0,0,0,0},
    {5,9,11,5,11,3,5,3,2,0,0,0,0,0,0},
    {11,6,9,2,4,7,0,0,0,0,0,0,0,0,0},
    {6,9,11,5,1,4,5,4,7,0,0,0,0,0,0},
    {2,4,7,3,1,9,3,9,11,0,0,0,0,0,0},
    {5,9,11,5,11,3,5,3,7,3,4,7,0,0,0},
    {3,8,4,11,6,9,0,0,0,0,0,0,0,0,0},
    {5,1,2,6,9,11,4,3,8,0,0,0,0,0,0},
    {11,8,4,11,4,1,11,1,9,0,0,0,0,0,0},
    {5,9,11,5,11,8,5,8,2,8,4,2,0,0,0},
    {11,6,9,3,8,7,3,7,2,0,0,0,0,0,0},
    {1,3,8,1,8,7,1,7,5,11,6,9,0,0,0},
    {11,8,7,11,7,2,11,2,9,2,1,9,0,0,0},
    {11,8,7,11,7,5,11,5,9,0,0,0,0,0,0},
    {10,11,6,10,6,5,0,0,0,0,0,0,0,0,0},
    {6,1,2,6,2,10,6,10,11,0,0,0,0,0,0},
    {1,5,10,1,10,11,1,11,3,0,0,0,0,0,0},
    {2,10,11,2,11,3,0,0,0,0,0,0,0,0,0},
    {7,2,4,5,10,11,5,11,6,0,0,0,0,0,0},
    {6,1,4,6,4,7,6,7,11,7,10,11,0,0,0},
    {1,5,10,1,10,11,1,11,3,7,2,4,0,0,0},
    {7,10,11,7,11,3,7,3,4,0,0,0,0,0,0},
    {3,8,4,11,6,5,11,5,10,0,0,0,0,0,0},
    {6,1,2,6,2,10,6,10,11,4,3,8,0,0,0},
    {1,5,10,1,10,11,1,11,4,11,8,4,0,0,0},
    {4,2,10,4,10,11,4,11,8,0,0,0,0,0,0},
    {6,5,10,6,10,11,2,3,8,2,8,7,0,0,0},
    {1,3,8,1,8,7,1,7,6,7,10,6,10,11,6},
    {1,5,10,1,10,11,1,11,2,11,8,2,8,7,2},
    {8,7,10,8,10,11,0,0,0,0,0,0,0,0,0},
    {10,7,12,0,0,0,0,0,0,0,0,0,0,0,0},
    {10,7,12,2,5,1,0,0,0,0,0,0,0,0,0},
    {3,1,6,10,7,12,0,0,0,0,0,0,0,0,0},
    {10,7,12,2,5,6,2,6,3,0,0,0,0,0,0},
    {4,12,10,4,10,2,0,0,0,0,0,0,0,0,0},
    {10,5,1,10,1,4,10,4,12,0,0,0,0,0,0},
    {3,1,6,2,4,12,2,12,10,0,0,0,0,0,0},
    {3,4,12,3,12,10,3,10,6,10,5,6,0,0,0},
    {7,12,10,8,4,3,0,0,0,0,0,0,0,0,0},
    {1,2,5,4,3,8,10,7,12,0,0,0,0,0,0},
    {7,12,10,8,4,1,8,1,6,0,0,0,0,0,0},
    {4,2,5,4,5,6,4,6,8,10,7,12,0,0,0},
    {8,12,10,8,10,2,8,2,3,0,0,0,0,0,0},
    {1,3,8,1,8,12,1,12,5,12,10,5,0,0,0},
    {2,1,6,2,6,8,2,8,10,8,12,10,0,0,0},
    {10,5,6,10,6,8,10,8,12,0,0,0,0,0,0},
    {5,7,12,5,12,9,0,0,0,0,0,0,0,0,0},
    {2,7,12,2,12,9,2,9,1,0,0,0,0,0,0},
    {1,6,3,9,5,7,9,7,12,0,0,0,0,0,0},
    {2,7,12,2,12,9,2,9,3,9,6,3,0,0,0},
    {5,2,4,5,4,12,5,12,9,0,0,0,0,0,0},
    {12,9,1,12,1,4,0,0,0,0,0,0,0,0,0},
    {5,2,4,5,4,12,5,12,9,3,1,6,0,0,0},
    {3,4,12,3,12,9,3,9,6,0,0,0,0,0,0},
    {8,4,3,7,12,9,7,9,5,0,0,0,0,0,0},
    {2,7,12,2,12,9,2,9,1,8,4,3,0,0,0},
    {4,1,6,4,6,8,5,7,12,5,12,9,0,0,0},
    {2,7,12,2,12,9,2,9,4,9,6,4,6,8,4},
    {8,12,9,8,9,5,8,5,3,5,2,3,0,0,0},
    {8,12,9,8,9,1,8,1,3,0,0,0,0,0,0},
    {2,1,6,2,6,8,2,8,5,8,12,5,12,9,5},
    {12,9,6,12,6,8,0,0,0,0,0,0,0,0,0},
    {12,10,7,9,11,6,0,0,0,0,0,0,0,0,0},
    {12,10,7,9,11,6,2,5,1,0,0,0,0,0,0},
    {12,10,7,9,11,3,9,3,1,0,0,0,0,0,0},
    {5,9,11,5,11,3,5,3,2,12,10,7,0,0,0},
    {9,11,6,12,10,2,12,2,4,0,0,0,0,0,0},
    {10,5,1,10,1,4,10,4,12,6,9,11,0,0,0},
    {1,9,11,1,11,3,10,2,4,10,4,12,0,0,0},
    {5,9,11,5,11,3,5,3,10,3,4,10,4,12,10},
    {3,8,4,11,6,9,7,12,10,0,0,0,0,0,0},
    {2,5,1,10,7,12,9,11,6,3,8,4,0,0,0},
    {11,8,4,11,4,1,11,1,9,7,12,10,0,0,0},
    {11,8,4,11,4,2,11,2,9,2,5,9,10,7,12},
    {8,12,10,8,10,2,8,2,3,9,11,6,0,0,0},
    {8,12,10,8,10,5,8,5,3,5,1,3,6,9,11},
    {8,12,10,8,10,2,8,2,11,2,1,11,1,9,11},
    {8,12,10,8,10,5,8,5,11,5,9,11,0,0,0},
    {12,11,6,12,6,5,12,5,7,0,0,0,0,0,0},
    {12,11,6,12,6,1,12,1,7,1,2,7,0,0,0},
    {12,11,3,12,3,1,12,1,7,1,5,7,0,0,0},
    {12,11,3,12,3,2,12,2,7,0,0,0,0,0,0},
    {5,2,4,5,4,12,5,12,6,12,11,6,0,0,0},
    {6,1,4,6,4,12,6,12,11,0,0,0,0,0,0},
    {5,2,4,5,4,12,5,12,1,12,11,1,11,3,1},
    {4,12,11,4,11,3,0,0,0,0,0,0,0,0,0},
    {12,11,6,12,6,5,12,5,7,3,8,4,0,0,0},
    {2,7,12,2,12,11,2,11,1,11,6,1,3,8,4},
    {11,8,4,11,4,1,11,1,12,1,5,12,5,7,12},
    {2,7,12,2,12,11,2,11,4,11,8,4,0,0,0},
    {12,11,6,12,6,5,12,5,8,5,2,8,2,3,8},
    {12,11,6,12,6,1,12,1,8,1,3,8,0,0,0},
    {1,5,2,8,12,11,0,0,0,0,0,0,0,0,0},
    {8,12,11,0,0,0,0,0,0,0,0,0,0,0,0},
    {8,11,12,0,0,0,0,0,0,0,0,0,0,0,0},
    {2,5,1,11,12,8,0,0,0,0,0,0,0,0,0},
    {6,3,1,8,11,12,0,0,0,0,0,0,0,0,0},
    {8,11,12,6,3,2,6,2,5,0,0,0,0,0,0},
    {4,7,2,12,8,11,0,0,0,0,0,0,0,0,0},
    {12,8,11,4,7,5,4,5,1,0,0,0,0,0,0},
    {6,3,1,8,11,12,2,4,7,0,0,0,0,0,0},
    {3,4,7,3,7,5,3,5,6,12,8,11,0,0,0},
    {3,11,12,3,12,4,0,0,0,0,0,0,0,0,0},
    {1,2,5,4,3,11,4,11,12,0,0,0,0,0,0},
    {6,11,12,6,12,4,6,4,1,0,0,0,0,0,0},
    {4,2,5,4,5,6,4,6,12,6,11,12,0,0,0},
    {12,7,2,12,2,3,12,3,11,0,0,0,0,0,0},
    {1,3,11,1,11,12,1,12,5,12,7,5,0,0,0},
    {6,11,12,6,12,7,6,7,1,7,2,1,0,0,0},
    {12,7,5,12,5,6,12,6,11,0,0,0,0,0,0},
    {10,9,5,11,12,8,0,0,0,0,0,0,0,0,0},
    {11,12,8,10,9,1,10,1,2,0,0,0,0,0,0},
    {10,9,5,11,12,8,1,6,3,0,0,0,0,0,0},
    {9,6,3,9,3,2,9,2,10,8,11,12,0,0,0},
    {5,10,9,7,2,4,11,12,8,0,0,0,0,0,0},
    {7,10,9,7,9,1,7,1,4,11,12,8,0,0,0},
    {11,12,8,10,9,5,7,2,4,3,1,6,0,0,0},
    {3,4,7,3,7,10,3,10,6,10,9,6,11,12,8},
    {10,9,5,11,12,4,11,4,3,0,0,0,0,0,0},
    {12,4,3,12,3,11,2,10,9,2,9,1,0,0,0},
    {6,11,12,6,12,4,6,4,1,10,9,5,0,0,0},
    {6,11,12,6,12,4,6,4,9,4,2,9,2,10,9},
    {12,7,2,12,2,3,12,3,11,5,10,9,0,0,0},
    {7,10,9,7,9,1,7,1,12,1,3,12,3,11,12},
    {6,11,12,6,12,7,6,7,1,7,2,1,5,10,9},
    {7,10,9,7,9,6,7,6,12,6,11,12,0,0,0},
    {8,6,9,8,9,12,0,0,0,0,0,0,0,0,0},
    {5,1,2,6,9,12,6,12,8,0,0,0,0,0,0},
    {8,3,1,8,1,9,8,9,12,0,0,0,0,0,0},
    {5,9,12,5,12,8,5,8,2,8,3,2,0,0,0},
    {4,7,2,12,8,6,12,6,9,0,0,0,0,0,0},
    {7,5,1,7,1,4,9,12,8,9,8,6,0,0,0},
    {8,3,1,8,1,9,8,9,12,2,4,7,0,0,0},
    {3,4,7,3,7,5,3,5,8,5,9,8,9,12,8},
    {3,6,9,3,9,12,3,12,4,0,0,0,0,0,0},
    {3,6,9,3,9,12,3,12,4,5,1,2,0,0,0},
    {4,1,9,4,9,12,0,0,0,0,0,0,0,0,0},
    {5,9,12,5,12,4,5,4,2,0,0,0,0,0,0},
    {3,6,9,3,9,12,3,12,2,12,7,2,0,0,0},
    {3,6,9,3,9,12,3,12,1,12,7,1,7,5,1},
    {2,1,9,2,9,12,2,12,7,0,0,0,0,0,0},
    {7,5,9,7,9,12,0,0,0,0,0,0,0,0,0},
    {10,12,8,10,8,6,10,6,5,0,0,0,0,0,0},
    {10,12,8,10,8,6,10,6,2,6,1,2,0,0,0},
    {10,12,8,10,8,3,10,3,5,3,1,5,0,0,0},
    {8,3,2,8,2,10,8,10,12,0,0,0,0,0,0},
    {10,12,8,10,8,6,10,6,5,4,7,2,0,0,0},
    {10,12,8,10,8,6,10,6,7,6,1,7,1,4,7},
    {10,12,8,10,8,3,10,3,5,3,1,5,2,4,7},
    {3,4,7,3,7,10,3,10,8,10,12,8,0,0,0},
    {3,6,5,3,5,10,3,10,4,10,12,4,0,0,0},
    {6,1,2,6,2,10,6,10,3,10,12,3,12,4,3},
    {10,12,4,10,4,1,10,1,5,0,0,0,0,0,0},
    {12,4,2,12,2,10,0,0,0,0,0,0,0,0,0},
    {12,7,2,12,2,3,12,3,10,3,6,10,6,5,10},
    {7,10,12,3,6,1,0,0,0,0,0,0,0,0,0},
    {12,7,2,12,2,1,12,1,10,1,5,10,0,0,0},
    {12,7,10,0,0,0,0,0,0,0,0,0,0,0,0},
    {7,8,11,7,11,10,0,0,0,0,0,0,0,0,0},
    {2,5,1,10,7,8,10,8,11,0,0,0,0,0,0},
    {6,3,1,8,11,10,8,10,7,0,0,0,0,0,0},
    {11,10,7,11,7,8,5,6,3,5,3,2,0,0,0},
    {4,8,11,4,11,10,4,10,2,0,0,0,0,0,0},
    {10,5,1,10,1,4,10,4,11,4,8,11,0,0,0},
    {4,8,11,4,11,10,4,10,2,6,3,1,0,0,0},
    {4,8,11,4,11,10,4,10,3,10,5,3,5,6,3},
    {7,4,3,7,3,11,7,11,10,0,0,0,0,0,0},
    {7,4,3,7,3,11,7,11,10,1,2,5,0,0,0},
    {6,11,10,6,10,7,6,7,1,7,4,1,0,0,0},
    {4,2,5,4,5,6,4,6,7,6,11,7,11,10,7},
    {10,2,3,10,3,11,0,0,0,0,0,0,0,0,0},
    {1,3,11,1,11,10,1,10,5,0,0,0,0,0,0},
    {6,11,10,6,10,2,6,2,1,0,0,0,0,0,0},
    {11,10,5,11,5,6,0,0,0,0,0,0,0,0,0},
    {11,9,5,11,5,7,11,7,8,0,0,0,0,0,0},
    {2,7,8,2,8,11,2,11,1,11,9,1,0,0,0},
    {11,9,5,11,5,7,11,7,8,1,6,3,0,0,0},
    {9,6,3,9,3,2,9,2,11,2,7,11,7,8,11},
    {5,2,4,5,4,8,5,8,9,8,11,9,0,0,0},
    {11,9,1,11,1,4,11,4,8,0,0,0,0,0,0},
    {11,9,5,11,5,2,11,2,8,2,4,8,3,1,6},
    {4,8,11,4,11,9,4,9,3,9,6,3,0,0,0},
    {7,4,3,7,3,11,7,11,5,11,9,5,0,0,0},
    {7,4,3,7,3,11,7,11,2,11,9,2,9,1,2},
    {11,9,5,11,5,7,11,7,6,7,4,6,4,1,6},
    {4,2,7,11,9,6,0,0,0,0,0,0,0,0,0},
    {5,2,3,5,3,11,5,11,9,0,0,0,0,0,0},
    {9,1,3,9,3,11,0,0,0,0,0,0,0,0,0},
    {11,9,5,11,5,2,11,2,6,2,1,6,0,0,0},
    {11,9,6,0,0,0,0,0,0,0,0,0,0,0,0},
    {9,10,7,9,7,8,9,8,6,0,0,0,0,0,0},
    {9,10,7,9,7,8,9,8,6,2,5,1,0,0,0},
    {8,3,1,8,1,9,8,9,7,9,10,7,0,0,0},
    {9,10,7,9,7,8,9,8,5,8,3,5,3,2,5},
    {4,8,6,4,6,9,4,9,2,9,10,2,0,0,0},
    {10,5,1,10,1,4,10,4,9,4,8,9,8,6,9},
    {8,3,1,8,1,9,8,9,4,9,10,4,10,2,4},
    {8,3,4,10,5,9,0,0,0,0,0,0,0,0,0},
    {3,6,9,3,9,10,3,10,4,10,7,4,0,0,0},
    {3,6,9,3,9,10,3,10,4,10,7,4,2,5,1},
    {7,4,1,7,1,9,7,9,10,0,0,0,0,0,0},
    {4,2,5,4,5,9,4,9,7,9,10,7,0,0,0},
    {9,10,2,9,2,3,9,3,6,0,0,0,0,0,0},
    {10,5,1,10,1,3,10,3,9,3,6,9,0,0,0},
    {10,2,1,10,1,9,0,0,0,0,0,0,0,0,0},
    {10,5,9,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,7,8,5,8,6,0,0,0,0,0,0,0,0,0},
    {2,7,8,2,8,6,2,6,1,0,0,0,0,0,0},
    {1,5,7,1,7,8,1,8,3,0,0,0,0,0,0},
    {3,2,7,3,7,8,0,0,0,0,0,0,0,0,0},
    {4,8,6,4,6,5,4,5,2,0,0,0,0,0,0},
    {8,6,1,8,1,4,0,0,0,0,0,0,0,0,0},
    {5,2,4,5,4,8,5,8,1,8,3,1,0,0,0},
    {3,4,8,0,0,0,0,0,0,0,0,0,0,0,0},
    {3,6,5,3,5,7,3,7,4,0,0,0,0,0,0},
    {6,1,2,6,2,7,6,7,3,7,4,3,0,0,0},
    {5,7,4,5,4,1,0,0,0,0,0,0,0,0,0},
    {4,2,7,0,0,0,0,0,0,0,0,0,0,0,0},
    {6,5,2,6,2,3,0,0,0,0,0,0,0,0,0},
    {3,6,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}; 
      
  for(int i = 0 ; i < 256 ; i++ )
    {
    this->m_CubeConfigurationCodeToListOfTriangle[i] = marchingCubesLookupTable[i];
    }
      
}

// Destructor
template<class TInputImage, class TOutputMesh>
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::~MarchingCubesImageToMeshFilter()
{
  
}

template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::SetInput(const InputImageType* image)
{
  this->ProcessObject::SetNthInput(0,
                                   const_cast< InputImageType * >( image ) );
}

/** Generate the data */
template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::GenerateData()
{

  const InputImageType * inputImage =
    static_cast<const InputImageType * >(this->ProcessObject::GetInput(0) );

  this->m_ResampleFilter->SetInput( inputImage );
  this->m_ResampleFilter->SetOutputDirection( inputImage->GetDirection() );

  //
  // Prepare Output PointSet
  //
  OutputMeshType * outputPointSet = this->GetOutput();

  PointsContainerPointer    pointsContainer   = PointsContainer::New();
  PointDataContainerPointer normalsContainer  = PointDataContainer::New();

  pointsContainer->Initialize();
  normalsContainer->Initialize();

  this->m_NumberOfPoints = NumericTraits< PointIdentifier >::Zero;
  this->m_NumberOfCells = NumericTraits< CellIdentifier >::Zero;

  outputPointSet->SetPoints( pointsContainer );
  outputPointSet->SetPointData( normalsContainer );


  RegionType subdivisionRegion;
  SizeType   subdivisionSize;

  SpacingType inputSpacing = inputImage->GetSpacing();
  SpacingType subdividedSpacing;

  for( unsigned int k = 0; k < InputImageDimension; k++ )
    {
    subdivisionSize[k] = this->m_SubdivideFactors[k] + 1;
    if( this->m_SubdivideFactors[k] > 0 )
      {
      subdividedSpacing[k] = inputSpacing[k] / this->m_SubdivideFactors[k];
      }
    else
      {
      itkExceptionMacro("Subdivision Factor not larger than zero");
      }
    }

  this->m_ResampleFilter->SetOutputSpacing( subdividedSpacing );
  this->m_ResampleFilter->SetSize( subdivisionSize );

  typename InputImageType::RegionType region = inputImage->GetLargestPossibleRegion();

  typename InputImageType::SizeType centralDifferenceRadius;
  centralDifferenceRadius.Fill(2);

  typename InputImageType::SizeType cellRadius;
  cellRadius.Fill(1);

  FaceCalculatorType bC;

  FaceListType faceList = bC( inputImage, region, centralDifferenceRadius );

  // support progress methods/callbacks
  ProgressReporter progress(this, 0, region.GetNumberOfPixels());

  // Process only the face that is fully contained inside the image
  FaceIterator internalFace = faceList.begin();

  NeighborhoodIteratorType cellRegionWalker( cellRadius, inputImage, *internalFace);

  cellRegionWalker.GoToBegin();

  typename NeighborhoodIteratorType::OffsetType offset;
        
  offset[0] = 0;
  offset[1] = 0;
  offset[2] = 0;

  cellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 0;
  offset[2] = 0;

  cellRegionWalker.ActivateOffset(offset);

  offset[0] = 0;
  offset[1] = 1;
  offset[2] = 0;

  cellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 1;
  offset[2] = 0;

  cellRegionWalker.ActivateOffset(offset);

  offset[0] = 0;
  offset[1] = 0;
  offset[2] = 1;

  cellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 0;
  offset[2] = 1;

  cellRegionWalker.ActivateOffset(offset);

  offset[0] = 0;
  offset[1] = 1;
  offset[2] = 1;

  cellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 1;
  offset[2] = 1;

  cellRegionWalker.ActivateOffset(offset);

  this->m_GradientCalculator->SetInputImage( inputImage );

  this->m_ListOfGradientsOnCell.resize( static_cast<int>( vcl_pow( (float)2, InputImageDimension) ) );
  
  while ( ! cellRegionWalker.IsAtEnd() )
    {
    if( this->IsSurfaceInside( cellRegionWalker ) )
      {
      this->GenerateTriangles( cellRegionWalker );
      this->ComputeCentralDifferences( cellRegionWalker );
      }

    ++cellRegionWalker;

    progress.CompletedPixel();
    }
}


/** Compute gradients on the voxels contained in the neighborhood. */
template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::GenerateTriangles( const NeighborhoodIteratorType & cellRegionWalker )
{
   const ListOfTrianglesType & listOfTrianglesAsEdges = 
     this->m_CubeConfigurationCodeToListOfTriangle[ this->m_TableIndex ];

   const unsigned int maximumNumberOfTrianglesPerCube = 5;
   const unsigned int numberOfEdgesPerTriangle = 3;
   const unsigned int numberOfEdgesPerConfiguration = 
     numberOfEdgesPerTriangle * maximumNumberOfTrianglesPerCube;

   DirectedPointType point1;
   DirectedPointType point2; 
   DirectedPointType point3; 

   for( unsigned int i = 0; i < numberOfEdgesPerConfiguration; i += 3 )
     {
     if( listOfTrianglesAsEdges.Triangle[i] == 0 )
       {
       break;
       }

     const VertexTypeId edgeId1 = listOfTrianglesAsEdges.Triangle[ i ];
     const VertexTypeId edgeId2 = listOfTrianglesAsEdges.Triangle[i+1];
     const VertexTypeId edgeId3 = listOfTrianglesAsEdges.Triangle[i+2];

     const VertexPairType & edge1 = this->m_EdgeIndexToVertexIndex[edgeId1];
     const VertexPairType & edge2 = this->m_EdgeIndexToVertexIndex[edgeId2];
     const VertexPairType & edge3 = this->m_EdgeIndexToVertexIndex[edgeId3];

     // NOTE: This will generate duplicate coincident points.
     this->InterpolateEdge( edge1, cellRegionWalker, point1 );
     this->InterpolateEdge( edge2, cellRegionWalker, point2 );
     this->InterpolateEdge( edge3, cellRegionWalker, point3 );

     this->AddTriangleToOutputMesh( point1, point2, point3 );
     }
}

/** Insert a triangle in the output mesh.
 *  TODO: Think about how to avoid coincident points. */
template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::AddTriangleToOutputMesh(
  const DirectedPointType & directedPoint1,
  const DirectedPointType & directedPoint2,
  const DirectedPointType & directedPoint3 )
{
  OutputMeshType     * mesh = this->GetOutput();
  PointsContainer    * pointsContainer  = mesh->GetPoints();
  PointDataContainer * pointDataContainer  = mesh->GetPointData();
  
  typename TriangleType::CellAutoPointer cellpointer;
  cellpointer.TakeOwnership( new TriangleType );

  // TODO  Insert here use of the Point locator.

  // Insert the point in the output mesh.
  pointsContainer->InsertElement(  this->m_NumberOfPoints, directedPoint1.point );
  pointDataContainer->InsertElement(  this->m_NumberOfPoints, directedPoint1.gradient );
  cellpointer->SetPointId( 0, this->m_NumberOfPoints++ );

  pointsContainer->InsertElement(  this->m_NumberOfPoints, directedPoint2.point ); 
  pointDataContainer->InsertElement(  this->m_NumberOfPoints, directedPoint2.gradient );
  cellpointer->SetPointId( 1, this->m_NumberOfPoints++ );

  pointsContainer->InsertElement(  this->m_NumberOfPoints, directedPoint3.point );
  pointDataContainer->InsertElement(  this->m_NumberOfPoints, directedPoint3.gradient );
  cellpointer->SetPointId( 2, this->m_NumberOfPoints++ );

  mesh->SetCell( this->m_NumberOfCells++, cellpointer );
}

/** Compute gradients on the voxels contained in the neighborhood. */
template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::InterpolateEdge( const VertexPairType & vertexPair, 
  const NeighborhoodIteratorType & cellRegionWalker,
  DirectedPointType & outputDirectedPoint )
{
  //Sophie's code (verified)  

  unsigned int v1 = vertexPair.Vertex1;
  unsigned int v2 = vertexPair.Vertex2;

  IndexType index1 = m_IndexTypeArray[v1];
  IndexType index2 = m_IndexTypeArray[v2];

  const InputImageType * inputImage = 
  static_cast< const InputImageType * > ( this->ProcessObject::GetInput(0) );

  typename InputImageType::PointType point1;
  typename InputImageType::PointType point2;
  
  inputImage->TransformIndexToPhysicalPoint( index1,point1 );
  inputImage->TransformIndexToPhysicalPoint( index2,point2 );

  //typename tells compiler its type
  typedef typename InputImageType::PixelType PixelType;
  typedef typename NumericTraits< PixelType >::RealType RealPixelType;

  //static_cast = typecasting in templates
  RealPixelType intensity1 = static_cast< RealPixelType >( m_PixelTypeArray[v1] );
  RealPixelType intensity2 = static_cast< RealPixelType >( m_PixelTypeArray[v2] );

  //does the computation for Linear Interpolation for the middle pt coords
  double alpha = ( m_SurfaceValue - intensity1 ) / ( intensity2 - intensity1 );
  typename InputImageType::PointType Q;

  Q.SetToBarycentricCombination( point2, point1, alpha );  
  
  outputDirectedPoint.point = Q;
   // TODO Here:
   // * Get the two vertex physical coordinates
   // * Compute Linear interpolation for the middle point coordinates
   // * Interpolate gradients from the m_ListOfGradientsOnCell.
}

/** Compute gradients on the voxels contained in the neighborhood. */
template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::ComputeCentralDifferences( const NeighborhoodIteratorType & cellRegionWalker )
{
  typename NeighborhoodIteratorType::ConstIterator pixelIterator = cellRegionWalker.Begin();

  IndexType  cornerIndex = cellRegionWalker.GetIndex();
  IndexType  index;

  // Compute the gradient on each one of the pixels.
  unsigned int vertexId = 0;
  while( pixelIterator != cellRegionWalker.End() )
    {
    index = cornerIndex + pixelIterator.GetNeighborhoodOffset();
    this->m_ListOfGradientsOnCell[vertexId++] = this->m_GradientCalculator->EvaluateAtIndex( index );
    ++pixelIterator;
    }
}


/** Check whether the 4^N region is cut by the iso-hyper-surface. */
template<class TInputImage, class TOutputMesh>
bool
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::IsSurfaceInside( const NeighborhoodIteratorType & cellRegionWalker )
{
  bool foundLargerValue  = false;
  bool foundSmallerValue = false;

  TableIndexType tableIndex = itk::NumericTraits< TableIndexType >::Zero;
  TableIndexType currentNode = 1;

  typedef typename InputImageType::PixelType PixelType;
  typedef typename NumericTraits< PixelType >::RealType RealPixelType;
 
  
  typename NeighborhoodIteratorType::ConstIterator pixelIterator = cellRegionWalker.Begin();

  //Sophie's counter
  int i = 0;  
  IndexType cornerIndex = cellRegionWalker.GetIndex();
  
  // Check for pixel values above and below the iso-surface value.
  while( pixelIterator != cellRegionWalker.End() )
    {
    //sophie lines
    this->m_PixelTypeArray[i] = pixelIterator.Get();
    this->m_IndexTypeArray[i] = cornerIndex + pixelIterator.GetNeighborhoodOffset();
    
    i++;
    //end sophie code
    
    if( pixelIterator.Get() >= this->m_SurfaceValue )
      {
      tableIndex |= currentNode;
      foundLargerValue = true;
      }
    else
      {
      if( pixelIterator.Get() < this->m_SurfaceValue )
        {
        foundSmallerValue = true;
        }
      }
    currentNode <<= 1;
 
    ++pixelIterator;
    }
    
  this->m_TableIndex = tableIndex;

  const bool surfaceIsInside = foundSmallerValue && foundLargerValue;

  return surfaceIsInside;
}

/** PrintSelf */
template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent
     << "ObjectValue: "
     << static_cast<typename NumericTraits<InputPixelRealType>::PrintType>(m_SurfaceValue)
     << std::endl;
}

} /** end namespace itk. */

#endif
