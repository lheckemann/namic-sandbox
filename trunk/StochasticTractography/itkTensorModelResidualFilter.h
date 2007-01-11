/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStochasticTractographyFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/27 17:01:06 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTensorModelResidualFilter_h_
#define __itkTensorModelResidualFilter_h_

#include "itkImageToImageFilter.h"

namespace itk{
template< class TTensorPixelType=double,
        class TResidualPixelType=double>
class ITK_EXPORT TensorModelResidualFilter:
  public ImageToImageFilter< Image< DiffusionTensor3D< TTensorPixelType >, 3> 
                    Image< TResidualPixelType, 3 > >
{
public:
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorModelResidualFilter.txx"
#endif
