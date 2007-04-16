/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageToCooccurrenceMatrixFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/04/20 20:31:22 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkScalarImageToCooccurrenceMatrixFilter_txx
#define __itkScalarImageToCooccurrenceMatrixFilter_txx

#include "itkScalarImageToCooccurrenceMatrixFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "vnl/vnl_math.h"

namespace itk {
namespace Statistics {
    
template< class TImageType, class THistogramFrequencyContainer >
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::
ScalarImageToCooccurrenceMatrixFilter()  
{

  this->SetNumberOfRequiredInputs(1);
  this->SetNumberOfRequiredOutputs(1);

  this->ProcessObject::SetNthOutput(0, this->MakeOutput(0).GetPointer() );

  //initialize parameters
  m_LowerBound.Fill(NumericTraits<PixelType>::min());
  m_UpperBound.Fill(NumericTraits<PixelType>::max() + 1);
  m_Min = NumericTraits<PixelType>::min();
  m_Max = NumericTraits<PixelType>::max();

  m_NumberOfBinsPerAxis = DefaultBinsPerAxis; 
  m_Normalize = false;
}

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >
::SetOffset( const OffsetType offset )
{
  OffsetVectorPointer offsetVector = OffsetVector::New();
  offsetVector->push_back(offset);
  this->SetOffsets(offsetVector);
}
   
template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >
::SetInput(const ImageType* image) 
{ 
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< ImageType* >( image ) );
}

template< class TImageType, class THistogramFrequencyContainer >
const TImageType* 
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >
::GetInput() const
{
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }
  
  return static_cast<const ImageType * >
    (this->ProcessObject::GetInput(0) );
}  

template< class TImageType, class THistogramFrequencyContainer >
const typename ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::HistogramType *
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >
::GetOutput() const
{
  const HistogramType * output = 
    static_cast< const HistogramType * >( this->ProcessObject::GetOutput(0));
  return output;
}

template< class TImageType, class THistogramFrequencyContainer >
typename ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::DataObjectPointer
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >
::MakeOutput(unsigned int itkNotUsed(idx))
{
  typename HistogramType::Pointer output = HistogramType::New();
  return static_cast< DataObject * >( output );
}


template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::
GenerateData( void )
{
  HistogramType * output = 
   static_cast< HistogramType * >( this->ProcessObject::GetOutput(0) );

  const ImageType *input = this->GetInput();
  // At this point input must be non-NULL because the ProcessObject 
  // checks the number of required input to be non-NULL pointers before
  // calling this GenerateData() method.

  // First, create an appropriate histogram with the right number of bins
  // and mins and maxes correct for the image type.
  typename HistogramType::SizeType size;
  size.Fill(m_NumberOfBinsPerAxis);
  output->Initialize(size, m_LowerBound, m_UpperBound);
  
  // Next, find the minimum radius that encloses all the offsets.
  unsigned int minRadius = 0;
  typename OffsetVector::ConstIterator offsets;
  for(offsets = m_Offsets->Begin(); offsets != m_Offsets->End(); offsets++)
    {
    for(unsigned int i = 0; i < offsets.Value().GetOffsetDimension(); i++)
      {
      unsigned int distance = vnl_math_abs(offsets.Value()[i]);
      if(distance > minRadius)
        {
        minRadius = distance;
        }
      }
    }
  
  RadiusType radius;
  radius.Fill(minRadius);
  
  // Now fill in the histogram
  this->FillHistogram(radius, input->GetRequestedRegion());
  
  // Normalizse the histogram if requested
  if(m_Normalize)
    {
    this->NormalizeHistogram();
    }
  
}

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::
FillHistogram(RadiusType radius, RegionType region)
{
  // Iterate over all of those pixels and offsets, adding each 
  // co-occurrence pair to the histogram

  const ImageType *input = this->GetInput();

  HistogramType * output = 
   static_cast< HistogramType * >( this->ProcessObject::GetOutput(0) );


  typedef ConstNeighborhoodIterator<ImageType> NeighborhoodIteratorType;
  NeighborhoodIteratorType neighborIt;
  neighborIt = NeighborhoodIteratorType(radius, input, region);

  for (neighborIt.GoToBegin(); !neighborIt.IsAtEnd(); ++neighborIt) 
    {
    const PixelType centerPixelIntensity = neighborIt.GetCenterPixel();
    if (centerPixelIntensity < m_Min || 
        centerPixelIntensity > m_Max)
      {
      continue; // don't put a pixel in the histogram if the value
                // is out-of-bounds.
      }
    
    typename OffsetVector::ConstIterator offsets;
    for(offsets = m_Offsets->Begin(); offsets != m_Offsets->End(); offsets++)
      {
      bool pixelInBounds;
      const PixelType pixelIntensity = 
        neighborIt.GetPixel(offsets.Value(), pixelInBounds);
      
      if (!pixelInBounds)
        {
        continue; // don't put a pixel in the histogram if it's out-of-bounds.
        }
      
      if (pixelIntensity < m_Min || 
          pixelIntensity > m_Max)
        {
        continue; // don't put a pixel in the histogram if the value
                  // is out-of-bounds.
        }
      
      // Now make both possible co-occurrence combinations and increment the
      // histogram with them.
      MeasurementVectorType cooccur;
      cooccur[0] = centerPixelIntensity;
      cooccur[1] = pixelIntensity;
      output->IncreaseFrequency(cooccur, 1);
      cooccur[1] = centerPixelIntensity;
      cooccur[0] = pixelIntensity;
      output->IncreaseFrequency(cooccur, 1);
      }
    }
}

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::
NormalizeHistogram( void )
{
  HistogramType * output = 
   static_cast< HistogramType * >( this->ProcessObject::GetOutput(0) );

  typename HistogramType::Iterator hit;
  typename HistogramType::FrequencyType totalFrequency = 
    output->GetTotalFrequency();
  
  for (hit = output->Begin(); hit != output->End(); ++hit)
    {
    hit.SetFrequency(hit.GetFrequency() / totalFrequency);
    }
}
    
template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::
SetPixelValueMinMax( PixelType min, PixelType max )
{
  itkDebugMacro("setting Min to " << min << "and Max to " << max);
  m_Min = min;
  m_Max = max;
  m_LowerBound.Fill(min);
  m_UpperBound.Fill(max + 1);
  this->Modified();
}

template< class TImageType, class THistogramFrequencyContainer >
void
ScalarImageToCooccurrenceMatrixFilter< TImageType,
THistogramFrequencyContainer >::
PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

} // end of namespace Statistics 
} // end of namespace itk 


#endif
