#ifndef __CenterOfBrain_txx__
#define __CenterOfBrain_txx__

/*
 * Copyright (c) 2009, Hans J. Johnson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Nathan Kline Institute nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */



/*** ITK Headers ***/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "CenterOfBrain.h"

#include "itkOrientedImage.h"
#include "itkCastImageFilter.h"


typedef itk::OrientedImage<short,3> SImageType;


//TODO: This function needs to be cleaned up.  It should not compute the otsu 
//TODO: threasholding, but rather, it should have been given
//TODO: An image that was previously otsu threasholded.
//TODO: This filter slowly evolved from another filter, and should be cleaned so
//TODO: that it only uses the items that are necessary for success.
extern SImageType::PointType FindCenterOfBrainBasedOnTopOfHead( SImageType::Pointer &volOrig,
                               bool maximize, unsigned int axis,
                               double otsuPercentileThreshold, unsigned int closingSize,
                               double headSizeLimit, SImageType::PixelType BackgroundFillValue );


//////////////////////////////////////////////////////////////////////////
//  This is a lightweight wrapper for FindCenterOfBrainBasedOnTopOfHead.

template <class TImageType>
typename TImageType::PointType GetCenterOfBrain(typename TImageType::Pointer any_volume)
{
  // NOTE:   This lets us deal with vastly different sized heads,
  // note:   or where the field of view is not centered in the brain
  // note:   or where there is a massive amount of neck.
  
  // First we have to cast a general volume into a short voxel volume, due to 
  // exploitation of image histograms within FindCenterOfBrainBasedOnTopOfHead.
  
  typedef itk::CastImageFilter<TImageType, SImageType> CastImageFilter;
  typename CastImageFilter::Pointer castFilter = CastImageFilter::New();
  castFilter->SetInput( any_volume );
  castFilter->Update( );
  typename SImageType::Pointer volume = castFilter->GetOutput();
  
  // Find center of the head.
  // 
  typename SImageType::PointType   CenterOfMass = 
    FindCenterOfBrainBasedOnTopOfHead( volume,
                                 1, 2,
                                 0.01, 7,
                                 1000, 0 );
    // IN AN LPS SYSTEM, the INFERIOR/SUPERIOR should be the center of physical space, rather than the
    // center of mass because the INFERIOR/SUPERIOR direction depends so much on the size of the neck.

  return  CenterOfMass;
}

//
//////////////////////////////////////////////////////////////////////////////////////////////////

#endif                          /* __CenterOfBrain_txx__ */
