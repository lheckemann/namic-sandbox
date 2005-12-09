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
#include "itkConstantBoundaryCondition.h"

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
/*
  this->SetNumberOfRequiredOutputs( 2 );
  this->SetNthOutput( 0, this->MakeOutput( 0 ) );
  this->SetNthOutput( 1, this->MakeOutput( 1 ) );
*/


  m_BinaryImage = InputImageType::New();

  this->m_Radius.Fill(1);
  this->m_Threshold = 0.005;

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





template <class TInputImage, class TOutputMesh>
void
LocalMaximumImageFilter< TInputImage, TOutputMesh>
::GenerateData()
{

std::cout << "starting generate data" << std::endl;
  OutputMeshPointer           mesh      = this->GetOutput();
  InputImageConstPointer      input     = this->GetInput(0);

  m_BinaryImage->SetRegions( input->GetLargestPossibleRegion() );
  m_BinaryImage->CopyInformation( input );
  m_BinaryImage->Allocate();

  PointsContainerPointer      points    = PointsContainer::New();
  PointDataContainerPointer   pointData = PointDataContainer::New();

  //Set background value for binary local maxima image
  OutputImageIterator it2( m_BinaryImage, input->GetRequestedRegion() );
  for (it2.GoToBegin(); !it2.IsAtEnd(); ++it2)
    {
    it2.Set(0);
    }


  unsigned int i;
  ConstantBoundaryCondition<InputImageType> cbc;
  cbc.SetConstant( NumericTraits<InputPixelType>::NonpositiveMin() );

  ConstNeighborhoodIterator<InputImageType> bit;
  ImageRegionIterator<InputImageType> it;


  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> bC;
  faceList = bC(input, input->GetRequestedRegion(), m_Radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

  // support progress methods/callbacks
  //ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());


  // Process each of the boundary faces.  These are N-d regions which border
  // the edge of the buffer.

  for (fit=faceList.begin(); fit != faceList.end(); ++fit)
    {
    bit = ConstNeighborhoodIterator<InputImageType>(m_Radius,
                                                    input, *fit);
    unsigned int neighborhoodSize = bit.Size();
    it = ImageRegionIterator<InputImageType>(m_BinaryImage, *fit);
    bit.OverrideBoundaryCondition(&cbc);
    bit.GoToBegin();

    while ( ! bit.IsAtEnd() )
      {
      bool isMaximum = true;
      InputPixelType centerValue = bit.GetCenterPixel();  //NumericTraits<InputRealType>::NonpositiveMin();
      for (i = 0; i < neighborhoodSize; ++i)
        {
        InputPixelType tmp = bit.GetPixel(i);
  // select only maxima that are larger than their neighborhood. If equal, discard as maximum ??.
  // Is this a good decision?
  // how to compare with center???
  if (tmp > centerValue)
    {
    isMaximum = false;
    break;
    }
        }

      // get the mean value
//      std::cout << isMaximum << "  " << centerValue << std::endl;
      if (isMaximum & (centerValue>m_Threshold))
        {
        InputIndexType maxIndex = it.GetIndex();
        PointType point;
        input->TransformIndexToPhysicalPoint( maxIndex , point );

        it.Set( static_cast<OutputPixelType>(1.0) );
        points->push_back( point );
        pointData->push_back( centerValue );
        }

      ++bit;
      ++it;
      //progress.CompletedPixel();
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
