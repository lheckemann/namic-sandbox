/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSymmetricSpaceTensorAnisotropicDiffusionFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/12/09 12:15:42 $
  Version:   $Revision: 1.28 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricSpaceTensorAnisotropicDiffusionFilter_txx_
#define __itkSymmetricSpaceTensorAnisotropicDiffusionFilter_txx_

#include "itkSymmetricSpaceTensorAnisotropicDiffusionFilter.h"

namespace itk{

template <class TInputImage, class TOutputImage>
void
SymmetricSpaceTensorAnisotropicDiffusionFilter<TInputImage, TOutputImage>
::ThreadedApplyUpdate(TimeStepType dt, const ThreadRegionType &regionToProcess,
                      int threadId)
{
  ImageRegionIterator<UpdateBufferType> u(this->GetUpdateBuffer(),regionToProcess);
  ImageRegionIterator<OutputImageType>  o(this->GetOutput(), regionToProcess);

  u = u.Begin();
  o = o.Begin();

  TensorType refTensor,updatedTensor,deltaTensor;
  PixelType updatedValue,originalValue,deltaValue;
  while ( !u.IsAtEnd() )
    {

    originalValue=o.Value();
    deltaValue= static_cast<PixelType>(u.Value() * dt);
   
    for(int i=0;i<6;i++)
      {
      refTensor[i]=originalValue[i];
      deltaTensor[i]=deltaValue[i];
      }
    SymmetricSpaceTensorGeometry<float, 3> * ssTensorGeometry;
    ssTensorGeometry = new SymmetricSpaceTensorGeometry<float, 3>;
    updatedTensor=ssTensorGeometry->ExpMap(refTensor,deltaTensor);

    for(int i=0;i<6;i++)
        updatedValue[i]=updatedTensor[i];

    o.Value()=updatedValue ;  // no adaptor support here
    ++o;
    ++u;
    }
}

}//end namespace itk

#endif
