/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkPointSetToDistanceFilter.txx,v $
Language:  C++
Date:      $Date: 2004/12/25 14:45:45 $
Version:   $Revision: 1.10 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTubeSpatialObjectToDistanceFilter_txx
#define __itkTubeSpatialObjectToDistanceFilter_txx

#include <algorithm>
#include <math.h>

#include "itkTubeSpatialObjectToDistanceFilter.h"
#include "itkImageBase.h"

#include "itkTubeSpatialObject.h"
#include "itkDTITubeSpatialObject.h"



namespace itk
{

template<class TInputTubeSpatialObjectGroup, class TOutputImage>
TubeSpatialObjectToDistanceFilter<TInputTubeSpatialObjectGroup,TOutputImage>::TubeSpatialObjectToDistanceFilter()
{
  //this->SetRequiredNumberOfInputs(1);
}

template<class TInputTubeSpatialObjectGroup, class TOutputImage>
void
TubeSpatialObjectToDistanceFilter<TInputTubeSpatialObjectGroup,TOutputImage>::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

/** Set the Input TInputTubeSpatialObjectGroup */
template <class TInputTubeSpatialObjectGroup, class TOutputImage>
void 
TubeSpatialObjectToDistanceFilter<TInputTubeSpatialObjectGroup,TOutputImage>
::SetInput(const TInputTubeSpatialObjectGroup *input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< TInputTubeSpatialObjectGroup * >( input ) );
}

/** Set the Input TubeSpatialObject */
template <class TInputTubeSpatialObjectGroup, class TOutputImage>
void 
TubeSpatialObjectToDistanceFilter<TInputTubeSpatialObjectGroup,TOutputImage>
::SetInput(unsigned int idx, const TInputTubeSpatialObjectGroup *input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(idx, 
                                   const_cast< TInputTubeSpatialObjectGroup * >( input ) );
}

template <class TInputTubeSpatialObjectGroup, class TOutputImage>
const TInputTubeSpatialObjectGroup*
TubeSpatialObjectToDistanceFilter<TInputTubeSpatialObjectGroup,TOutputImage>
::GetInput(unsigned int idx)
{
  return static_cast< const TInputTubeSpatialObjectGroup * >
    (this->ProcessObject::GetInput(idx));
}

template <class TInputTubeSpatialObjectGroup, class TOutputImage>
const TInputTubeSpatialObjectGroup*
TubeSpatialObjectToDistanceFilter<TInputTubeSpatialObjectGroup,TOutputImage>
::GetInput()
{
  return this->GetInput(0);
}

template<class TInputTubeSpatialObjectGroup, class TOutputImage>
void
TubeSpatialObjectToDistanceFilter<TInputTubeSpatialObjectGroup,TOutputImage>::GenerateData()
{
  
  typedef typename TInputTubeSpatialObjectGroup::ComponentType GroupListType;
  const GroupListType input  = this->GetInput()->Get();
  OutputImagePointer outputImage = this->GetOutput();

  typename OutputImageType::RegionType region;

  typename OutputImageType::SizeType size;
  unsigned int np = input.size();
  size[0] = np;
  size[1] = np;

  region.SetSize(size);

  outputImage->SetRegions(region);

  outputImage->Allocate();
  outputImage->FillBuffer(0);
  

  const int Dim = GetImageDimension<TOutputImage>::ImageDimension;
  typename GroupListType::const_iterator iterA;
  typename GroupListType::const_iterator iterB;

  // For each fiber
  for( iterA = input.begin(); iterA != input.end(); ++iterA) 
    {
    // Compare pairwise to all other fibers
    for( iterB = input.begin(); iterB != input.end(); ++iterB) 
      {
      // Loop over all points in A and find closest point in B
      // This type is typically a std::vector of some TubeSpatialObjectPoint<3>
        
      if(iterA != iterB)
      {
      // FIXME: Nasty cast/pointer hack
      SpatialObject<3>::Pointer soA = *iterA;
      SpatialObject<3>::Pointer soB = *iterB;
      DTITubeSpatialObject<3>* tubeA = static_cast<DTITubeSpatialObject<3>* >(soA.GetPointer());
      DTITubeSpatialObject<3>* tubeB = static_cast<DTITubeSpatialObject<3>* >(soB.GetPointer());
      //DTITubeSpatialObject<3>::PointListType ptsA = tubeA->GetPoints();
      //DTITubeSpatialObject<3>::PointListType ptsB = tubeB->GetPoints();
      
      typedef typename OutputImageType::PixelType DistanceType;
      DistanceType sumDist = 0;
      DistanceType maxDist = 0;
      typedef DTITubeSpatialObject<3>::PointListType::const_iterator PointIterator;
      for(PointIterator i = tubeA->GetPoints().begin(); i != tubeA->GetPoints().end(); ++i)
        {
        DistanceType minPointDist = std::numeric_limits<DistanceType>::max();

        for(PointIterator j = tubeB->GetPoints().begin(); j != tubeB->GetPoints().end(); ++j)
          {
          DistanceType dist = 0;
          for(unsigned int d = 0; d < 3; ++d)
            {
            double x = (i->GetPosition()[d]) - (j->GetPosition()[d]);
            dist += x*x;
            }
          dist = sqrt(dist);
          if(dist < minPointDist) {minPointDist = dist;}

          } // end point B iteration

        sumDist += minPointDist;
#ifdef _MSC_VER
        maxDist = std::_cpp_max(maxDist,minPointDist);
#else
        maxDist = std::max(maxDist,minPointDist);
#endif
        } /// end point A iteration

      typename OutputImageType::IndexType idx;
      idx[0] = std::distance(input.begin(),iterA);
      idx[1] = std::distance(input.begin(),iterB);
      outputImage->SetPixel(idx,sumDist / (typename OutputImageType::PixelType)(tubeA->GetNumberOfPoints()));
      //outputImage->SetPixel(idx,maxDist);
      } // end B != A test
      } // fiber B iteration
    } // fiber A iteration
 
}

} // end namespace itk

#endif
