/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLocalMaximumImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:51 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkLocalMaximumImageFilter_txx
#define _itkLocalMaximumImageFilter_txx
#include "itkLocalMaximumImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"

#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

#include "itkNumericTraits.h"


namespace itk
{

template <class TInputImage, class TOutputMesh>
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::LocalMaximumImageFilter()
{

  // Modify superclass default values, can be overridden by subclasses
  this->SetNumberOfRequiredInputs(1);

  PointDataContainerPointer  pointData  = PointDataContainer::New();
  OutputMeshPointer mesh = this->GetOutput();
  mesh->SetPointData( pointData.GetPointer() );

  this->SetNumberOfRequiredOutputs( 1 );
/*
  this->SetNthOutput( 0, this->MakeOutput( 0 ) );
  this->SetNthOutput( 1, this->MakeOutput( 1 ) );
*/


  m_BinaryImage = InputImageType::New();

  this->m_Radius.Fill(1);
  this->m_Threshold = 1.275;
//  this->m_Threshold2 = 0.5;

}


/**
 *   Make Ouput
 * \todo Verify that MakeOutput is createing the right type of objects
 *  this could be the cause of the reinterpret_cast bug in this class
 *
template <class TInputImage, TOutputMesh>
DataObject::Pointer
LocalMaximumImageFilter<TInputImage,TOutputMesh>
::MakeOutput(unsigned int idx)
{
  DataObject::Pointer output;
  switch( idx )
    {
    case 0:
      output = (OutputMeshType::New()).GetPointer();
      break;
    case 1:
      output = (InputImageType::New()).GetPointer();
      break;
    }
  return output.GetPointer();
}
*/



/**
 *
 */
template <class TInputImage, class TOutputMesh>
void
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::GenerateOutputInformation()
{
}


/*******************************************************
 *
 * get the output image
 *
 *****************************************************/
template <class TInputImage, class TOutputMesh>
typename LocalMaximumImageFilter< TInputImage, TOutputMesh>::InputImageType *
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::GetLocalMaximaImage()
  {
  return this->m_BinaryImage;
  }



/**
 *
 */
template <class TInputImage, class TOutputMesh>
void
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::SetInput( const InputImageType * inputImage )
{

  // This const_cast is needed due to the lack of
  // const-correctness in the ProcessObject.
  this->SetNthInput( 0,
            const_cast< InputImageType * >( inputImage ) );

}

/*
template <class TInputImage, class TOutputMesh>
void
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::SetInput2( const InputImageType * inputImage )
{

  // This const_cast is needed due to the lack of
  // const-correctness in the ProcessObject.
  this->SetNthInput( 1,
            const_cast< InputImageType * >( inputImage ) );

}
*/




template <class TInputImage, class TOutputMesh>
void
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::GenerateData()
{



  OutputMeshPointer           mesh      = this->GetOutput();
  InputImageConstPointer      image     = this->GetInput(0);
//  InputImageConstPointer      image2     = this->GetInput(1);

  m_BinaryImage->SetRegions( image->GetLargestPossibleRegion() );
  m_BinaryImage->CopyInformation( image );
  m_BinaryImage->Allocate();

  PointsContainerPointer      points    = PointsContainer::New();
  PointDataContainerPointer   pointData = PointDataContainer::New();

  OutputImageIterator it2( m_BinaryImage, image->GetRequestedRegion() );
  for (it2.GoToBegin(); !it2.IsAtEnd(); ++it2)
  {
  it2.Set(0);
  }



  int numberofwindows[InputImageDimension];
  InputSizeType regionSize;
  InputIndexType regionIndex;
    for (int k = 0; k<InputImageDimension; k++)
      {
      regionSize[k] = 2*m_Radius[k] + 1;
      regionIndex[k] = m_Radius[k];
      numberofwindows[k] = (image->GetRequestedRegion().GetSize()[k])/regionSize[k];
      }
  InputImageRegionType activeRegion;
    activeRegion.SetSize( regionSize );
    activeRegion.SetIndex( regionIndex );





  if (InputImageDimension==2)
    {
    for (int k=0; k<numberofwindows[0]; k++)
      {
      for (int j = 0; j<numberofwindows[1]; j++)
        {
  regionIndex[0] = regionSize[0]*k;
  regionIndex[1] = regionSize[1]*j;
  activeRegion.SetIndex( regionIndex );


        InputImageIterator  it(image, activeRegion);
  PointType point;
  InputPixelType maximum = NumericTraits<InputPixelType>::min();
  InputIndexType maxIndex;
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    if (it.Get() > maximum)
      {
      maximum = it.Get();
      maxIndex = it.GetIndex();
      image->TransformIndexToPhysicalPoint( maxIndex , point );
      }
    }
  if (maximum>m_Threshold)
    {
    if (IsLocalMaximum(maxIndex,maximum,image))
     {
     m_BinaryImage->SetPixel(maxIndex, 255.0);
     points->push_back( point );
     pointData->push_back( maximum );
     }
    }
  }
      }
    }
  else
    {


    for (int k=0; k<numberofwindows[0]; k++)
      {
      for (int j = 0; j<numberofwindows[1]; j++)
        {
      for (int l = 0; l<numberofwindows[2]; l++)
        {
  regionIndex[0] = regionSize[0]*k;
  regionIndex[1] = regionSize[1]*j;
  regionIndex[2] = regionSize[2]*l;
  activeRegion.SetIndex( regionIndex );


        InputImageIterator  it(image, activeRegion);
  PointType point;
  it.GoToBegin();
  InputPixelType maximum = it.Value();
  InputIndexType maxIndex;
  for (; !it.IsAtEnd(); ++it)
    {
    if (it.Value() >= maximum)
      {
      maximum  = it.Value();
      maxIndex = it.GetIndex();
      image->TransformIndexToPhysicalPoint( maxIndex , point );
      }
    }

  if (maximum > m_Threshold)
    {

    if (IsLocalMaximum(maxIndex,maximum,image))
      {
      m_BinaryImage->SetPixel(maxIndex, 255.0);
      points->push_back( point );
      pointData->push_back( maximum );
      }
    }
  }
      }
    }
  }


  mesh->SetPoints( points );
  mesh->SetPointData( pointData );

  // This indicates that the current BufferedRegion is equal to the
  // requested region. This action prevents useless rexecutions of
  // the pipeline.
  mesh->SetBufferedRegion( mesh->GetRequestedRegion() );


}


/**
 * is local Maximum
 */
template <class TInputImage, class TOutputMesh>
inline bool
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::IsLocalMaximum( const InputIndexType ind, const InputPixelType maximum, const InputImageType * image)
{
  InputImageRegionType neighborhood;
  InputSizeType size;
  InputIndexType ind2;
  for (unsigned int k=0; k<InputImageDimension; k++)
   {
   ind2[k]=ind[k]-1;
   size[k]=1;
   }
  neighborhood.SetIndex( ind2 );
  neighborhood.SetSize( size );

  InputImageIterator  it(image, neighborhood);

  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
  {
  if (it.Get() > maximum)
    {
    return false;
    }
  }
  return true;
}

/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutputMesh>
void
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::PrintSelf(
  std::ostream& os,
  Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Radius: " << m_Radius << std::endl;

}

} // end namespace itk

#endif
