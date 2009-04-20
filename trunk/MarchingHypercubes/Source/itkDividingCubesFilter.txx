/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkDividingCubesFilter.txx,v $
Language:  C++
Date:      $Date: 2008-11-21 20:00:48 $
Version:   $Revision: 1.15 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDividingCubesFilter_txx
#define __itkDividingCubesFilter_txx


#include "itkDividingCubesFilter.h"
#include "itkNumericTraits.h"

namespace itk
{

// Constructor
template<class TInputImage, class TOutputMesh>
DividingCubesFilter<TInputImage,TOutputMesh>
::DividingCubesFilter()
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
}


// Destructor
template<class TInputImage, class TOutputMesh>
DividingCubesFilter<TInputImage,TOutputMesh>
::~DividingCubesFilter()
{
  
}


template<class TInputImage, class TOutputMesh>
void
DividingCubesFilter<TInputImage,TOutputMesh>
::SetInput(const InputImageType* image)
{
  this->ProcessObject::SetNthInput(0,
                                   const_cast< InputImageType * >( image ) );
}

/** Generate the data */
template<class TInputImage, class TOutputMesh>
void
DividingCubesFilter<TInputImage,TOutputMesh>
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
      this->SubdivideCell( cellRegionWalker );
      this->FindIntersectingSubcells( cellRegionWalker );
      }

    ++cellRegionWalker;

    progress.CompletedPixel();
    }
}


/** Compute gradients on the voxels contained in the neighborhood. */
template<class TInputImage, class TOutputMesh>
void
DividingCubesFilter<TInputImage,TOutputMesh>
::SubdivideCell( const NeighborhoodIteratorType & cellRegionWalker )
{
    
  const InputImageType * inputImage =
    static_cast<const InputImageType * >(this->ProcessObject::GetInput(0) );

  ImagePointType  outputOrigin;

  inputImage->TransformIndexToPhysicalPoint( cellRegionWalker.GetIndex(), outputOrigin );

  this->m_ResampleFilter->SetOutputOrigin( outputOrigin );

  this->m_ResampleFilter->Update();
}

/** Compute gradients on the voxels contained in the neighborhood. */
template<class TInputImage, class TOutputMesh>
void
DividingCubesFilter<TInputImage,TOutputMesh>
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


/** Find the cells of the supersample image that are intersected by the surface */
template<class TInputImage, class TOutputMesh>
void
DividingCubesFilter<TInputImage,TOutputMesh>
::FindIntersectingSubcells( const NeighborhoodIteratorType & cellRegionWalker )
{
  const InputImageType * subdividedImage = this->m_ResampleFilter->GetOutput();

  const InputImageType * inputImage =
    static_cast<const InputImageType * >(this->ProcessObject::GetInput(0) );

  PointsContainer    * pointsContainer  = this->GetOutput()->GetPoints();
  PointDataContainer * normalsContainer = this->GetOutput()->GetPointData();

  RegionType region = subdividedImage->GetLargestPossibleRegion();

  typename InputImageType::SizeType cellRadius;
  cellRadius.Fill(1);

  FaceCalculatorType bC;

  SizeType size = region.GetSize();

  // Reduce the region by one voxel in order to have the number of subcells
  for( unsigned int i = 0; i < InputImageDimension; i++ )
    {
    size[i]--;
    }

  region.SetSize( size );

  FaceListType faceList = bC( subdividedImage, region, cellRadius );

  // Process only the face that is fully contained inside the image
  FaceIterator internalFace = faceList.begin();

  NeighborhoodIteratorType subcellRegionWalker( cellRadius, subdividedImage, *internalFace);

  typename NeighborhoodIteratorType::OffsetType offset;
        
  offset[0] = 0;
  offset[1] = 0;
  offset[2] = 0;

  subcellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 0;
  offset[2] = 0;

  subcellRegionWalker.ActivateOffset(offset);

  offset[0] = 0;
  offset[1] = 1;
  offset[2] = 0;

  subcellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 1;
  offset[2] = 0;

  subcellRegionWalker.ActivateOffset(offset);

  offset[0] = 0;
  offset[1] = 0;
  offset[2] = 1;

  subcellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 0;
  offset[2] = 1;

  subcellRegionWalker.ActivateOffset(offset);

  offset[0] = 0;
  offset[1] = 1;
  offset[2] = 1;

  subcellRegionWalker.ActivateOffset(offset);

  offset[0] = 1;
  offset[1] = 1;
  offset[2] = 1;

  subcellRegionWalker.ActivateOffset(offset);

  subcellRegionWalker.GoToBegin();

  SpacingType subdividedSpacing = subdividedImage->GetSpacing();
  SpacingType inputSpacing      = inputImage->GetSpacing();

  ImagePointType cellOrigin = subdividedImage->GetOrigin();

  typedef typename NeighborhoodIteratorType::OffsetType     OffsetType; 

  SpacingType           factor;
  MeshPointType         point;
  MeshPixelType         normal;  // The output mesh stores normals as its Pixel type

  while ( ! subcellRegionWalker.IsAtEnd() )
    {
    if( this->IsSurfaceInside( subcellRegionWalker ) )
      {
      IndexType cornerIndex =  subcellRegionWalker.GetIndex();

      ContinuousIndexType subcellIndexCenter;

      for( unsigned int k = 0; k < InputImageDimension; k++ )
        {
        subcellIndexCenter[k] = cornerIndex[k] + 0.5;
        const double centerOfSubcell = ( subcellIndexCenter[k] * subdividedSpacing[k] );
        factor[k] = centerOfSubcell / inputSpacing[k];
        }

      // Ask the subdivided image to convert index coordinates to physical coordinates
      subdividedImage->TransformContinuousIndexToPhysicalPoint( subcellIndexCenter, point );

      // Insert the point in the output mesh.
      pointsContainer->InsertElement(  this->m_NumberOfPoints, point );

      normal.Fill(0.0);

      // Compute the gradient on each one of the pixels.
      //
      typename NeighborhoodIteratorType::ConstIterator pixelIterator = subcellRegionWalker.Begin();

      unsigned int vertexId = 0;
      
      while ( pixelIterator != subcellRegionWalker.End() )
        {
        OffsetType offset = pixelIterator.GetNeighborhoodOffset();

        double weight = 1.0;

        for( unsigned int k = 0; k < InputImageDimension; k++ )
          {
          if( offset[k] )
            {
            weight *= ( 1.0 - factor[k] );
            }
          else
            {
            weight *= factor[k];
            }
          }

        for( unsigned int j = 0; j < InputImageDimension; j++ )
          {
          normal[j] += weight * this->m_ListOfGradientsOnCell[vertexId][j];
          }
        ++vertexId;
        ++pixelIterator;
        }

      normal.Normalize();

      normalsContainer->InsertElement( this->m_NumberOfPoints, normal );

      this->m_NumberOfPoints++;
      }

    ++subcellRegionWalker;
    }
}

/** Check whether the 4^N region is cut by the iso-hyper-surface. */
template<class TInputImage, class TOutputMesh>
bool
DividingCubesFilter<TInputImage,TOutputMesh>
::IsSurfaceInside( const NeighborhoodIteratorType & cellRegionWalker )
{
  bool foundLargerValue  = false;
  bool foundSmallerValue = false;

  typename NeighborhoodIteratorType::ConstIterator pixelIterator = cellRegionWalker.Begin();

  // Check for pixel values above and below the iso-surface value.
  while( pixelIterator != cellRegionWalker.End() )
    {
    if( pixelIterator.Get() >= this->m_SurfaceValue )
      {
      foundLargerValue = true;
      }
    else
      {
      if( pixelIterator.Get() < this->m_SurfaceValue )
        {
        foundSmallerValue = true;
        }
      }
    ++pixelIterator;
    }

  const bool surfaceIsInside = foundSmallerValue && foundLargerValue;

  return surfaceIsInside;
}

/** PrintSelf */
template<class TInputImage, class TOutputMesh>
void
DividingCubesFilter<TInputImage,TOutputMesh>
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
