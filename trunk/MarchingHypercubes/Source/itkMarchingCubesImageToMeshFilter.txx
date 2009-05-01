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

namespace itk
{

// Constructor
template<class TInputImage, class TOutputMesh>
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::MarchingCubesImageToMeshFilter()
{
  this->SetNumberOfRequiredInputs(1);

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

  this->m_EdgeIndexToVertexIndex[0] = VertexPairType( 0, 0 );

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
      this->ComputeCentralDifferences( cellRegionWalker );
      this->InterpolateEdges( cellRegionWalker );
      }

    ++cellRegionWalker;

    progress.CompletedPixel();
    }
}


/** Compute gradients on the voxels contained in the neighborhood. */
template<class TInputImage, class TOutputMesh>
void
MarchingCubesImageToMeshFilter<TInputImage,TOutputMesh>
::InterpolateEdges( const NeighborhoodIteratorType & cellRegionWalker )
{
    
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

  typename NeighborhoodIteratorType::ConstIterator pixelIterator = cellRegionWalker.Begin();

  // Check for pixel values above and below the iso-surface value.
  while( pixelIterator != cellRegionWalker.End() )
    {
    if( pixelIterator.Get() >= this->m_SurfaceValue )
      {
      tableIndex |= 1;
      foundLargerValue = true;
      }
    else
      {
      if( pixelIterator.Get() < this->m_SurfaceValue )
        {
        foundSmallerValue = true;
        }
      }
    tableIndex <<= 1;
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
