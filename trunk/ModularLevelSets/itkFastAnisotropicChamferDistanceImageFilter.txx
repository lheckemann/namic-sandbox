/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFastAnisotropicChamferDistanceImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2006/03/19 04:36:54 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkFastAnisotropicChamferDistanceImageFilter_txx
#define _itkFastAnisotropicChamferDistanceImageFilter_txx

#include <iostream>

#include "itkFastAnisotropicChamferDistanceImageFilter.h"
#include <itkNeighborhoodIterator.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>

namespace itk
{

template <class TInputImage,class TOutputImage>
FastAnisotropicChamferDistanceImageFilter<TInputImage,TOutputImage>
::FastAnisotropicChamferDistanceImageFilter()
{
  m_MaximumDistance = 10.0;
  m_NarrowBand = 0;
  m_UseImageSpacing = false;
}


template <class TInputImage,class TOutputImage>
void FastAnisotropicChamferDistanceImageFilter<TInputImage,TOutputImage>
::GenerateDataND() 
{
  const int SIGN_MASK = 1;
  const int INNER_MASK = 2;
  
  typename NeighborhoodIterator<TInputImage>::RadiusType r;
  bool in_bounds;
  
  r.Fill(1);
  NeighborhoodIterator<TInputImage> it(r, this->GetOutput(), m_RegionToProcess);
  
  const unsigned int  center_voxel = it.Size()/2;
  int i;
  unsigned int n;
  float val;
  PixelType  center_value;
  int neighbor_start,neighbor_end;
  BandNodeType node;
  
  float *cb_weights;
  cb_weights = new float[ it.Size() ];

  SpacingType spacing = this->GetInput()->GetSpacing();
  double min_spacing = NumericTraits<double>::max();
  for ( i=0; i<ImageDimension; i++)
    {
    min_spacing = spacing[i] < min_spacing ? spacing[i] : min_spacing;
    }

  if (!this->GetUseImageSpacing())
    {
    for ( i=0; i<ImageDimension; i++)
      {
      spacing[i] = 1.0;
      }
    min_spacing = 1.0;
    }

  //If the NarrowBand has been set, we update m_MaximumDistance using
  //narrowband TotalRadius plus a margin of 1 pixel.
  if ( m_NarrowBand.IsNotNull() )
    {
    m_MaximumDistance = (m_NarrowBand->GetTotalRadius() + 1) * min_spacing;
    }

  /** 1st Scan , using neighbors from center_voxel+1 to it.Size()-1 */

  /** Precomputing the neighbor city block weights */ 
  neighbor_start = center_voxel + 1;
  neighbor_end   = it.Size() - 1;

  for ( i = neighbor_start; i <= neighbor_end; i++)
    {
    cb_weights[i] = 0.0; 
    for( n = 0; n < ImageDimension; n++ ) 
      {
      cb_weights[i] += spacing[n] * spacing[n] * (it.GetOffset(i)[n] != 0);
      }
    cb_weights[i] = vcl_sqrt(cb_weights[i]);
    }
 
  /** Scan the image */
  for ( it.GoToBegin(); ! it.IsAtEnd(); ++it )
    {
    center_value = it.GetPixel(center_voxel);
    if (center_value>= m_MaximumDistance)
      {
      continue;
      }
    if (center_value<= -m_MaximumDistance)
      {
      continue;
      }
    /** Update Positive Distance */
    if (center_value>-min_spacing)
      {
      for (i = neighbor_start; i <= neighbor_end; i++)
        {
        val = center_value + cb_weights[i]; 
        if (val < it.GetPixel(i)) 
          {
          it.SetPixel(i,val,in_bounds);
          }
        }
      }
    /** Update Negative Distance */
    if (center_value<min_spacing)
      {
      for (i = neighbor_start; i <= neighbor_end; i++)
        {
        val = center_value - cb_weights[i]; 
        // Experiment an InlineGetPixel()
        if (val > it.GetPixel(i)) 
          {
          it.SetPixel(i,val,in_bounds);
          }
        }
      }
    }          
  
  /** 2nd Scan , using neighbors from 0 to center_voxel-1 */

  /*Clear the NarrowBand if it has been assigned */
  if (m_NarrowBand.IsNotNull()) 
    {
    m_NarrowBand->Clear();
    } 
  
  /** Precomputing the neighbor city block weights */ 
  neighbor_start = 0;
  neighbor_end   = center_voxel-1;
  
  for( i = neighbor_start; i <= neighbor_end; i++ )
    {
    cb_weights[i] = 0.0; 
    for (n=0; n < ImageDimension; n++)
      {
      cb_weights[i] += spacing[n] * spacing[n] * (it.GetOffset(i)[n] != 0);
      }
    cb_weights[i] = vcl_sqrt(cb_weights[i]);
    }
  
  /** Scan the image */
  for (it.GoToEnd(), --it; ! it.IsAtBegin(); --it)
    {
    center_value = it.GetPixel(center_voxel);
    if (center_value>= m_MaximumDistance)
      {
      continue;
      }
    if (center_value<= -m_MaximumDistance)
      {
      continue;
      }
    
    // Update the narrow band 
    if (m_NarrowBand.IsNotNull()) {
    if (fabs((float)center_value) <= m_NarrowBand->GetTotalRadius() * min_spacing) 
      {
      node.m_Index = it.GetIndex();
      //Check node state.
      node.m_NodeState = 0; 
      if (center_value>0)
        {
        node.m_NodeState += SIGN_MASK;
        }
      if (fabs((float)center_value) < m_NarrowBand->GetInnerRadius() * min_spacing) 
        {
        node.m_NodeState += INNER_MASK;
        }
      m_NarrowBand->PushBack(node);
      }
    }
    
    /** Update Positive Distance */
    if (center_value>-min_spacing)
      {
      for (i = neighbor_start; i <= neighbor_end; i++)
        {
        val = center_value + cb_weights[i]; 
        if (val < it.GetPixel(i)) 
          {
          it.SetPixel(i,val,in_bounds);
          }
        }
      }
    
    /** Update Negative Distance */
    if (center_value<min_spacing)
      {
      for (i = neighbor_start; i <= neighbor_end; i++)
        {
        val = center_value - cb_weights[i]; 
        if (val > it.GetPixel(i)) 
          {
          it.SetPixel(i,val,in_bounds);
          }
        }
      }
    }
  delete [] cb_weights;
}


template <class TInputImage,class TOutputImage>
void 
FastAnisotropicChamferDistanceImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{

  // Allocate the output image.
  typename TOutputImage::Pointer output = this->GetOutput();
  
  output->SetBufferedRegion(output->GetRequestedRegion());
  output->Allocate();
  
  ImageRegionIterator<TOutputImage>
    out(this->GetOutput(),this->GetInput()->GetRequestedRegion());
  ImageRegionConstIterator<TOutputImage>
    in( this->GetInput(), this->GetInput()->GetRequestedRegion());
  
  for(in.GoToBegin(),out.GoToBegin(); !in.IsAtEnd(); ++in,++out)
    {
    out.Set(in.Get());
    }
  
  m_RegionToProcess = this->GetInput()->GetRequestedRegion();

  this->GenerateDataND();

} // end GenerateData()

template <class TInputImage,class TOutputImage>
void 
FastAnisotropicChamferDistanceImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Maximal computed distance   : " << m_MaximumDistance << std::endl;
}

} // end namespace itk

#endif
