/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLevelSetBasedCellSegmentation.txx,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:31 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLevelSetBasedCellSegmentation_txx_
#define __itkLevelSetBasedCellSegmentation_txx_

#include "itkLevelSetBasedCellSegmentation.h"

namespace itk {


template <class TFeatureImage, class TMaskImage, class TOutputLabelImage>
LevelSetBasedCellSegmentation<TFeatureImage, TMaskImage, TOutputLabelImage>
::LevelSetBasedCellSegmentation()
{
}
 
template <class TFeatureImage, class TMaskImage, class TOutputLabelImage>
LevelSetBasedCellSegmentation<TFeatureImage, TMaskImage, TOutputLabelImage>
::~LevelSetBasedCellSegmentation()
{
}
 
template <class TFeatureImage, class TMaskImage, class TOutputLabelImage>
void
LevelSetBasedCellSegmentation<TFeatureImage, TMaskImage, TOutputLabelImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


template <class TFeatureImage, class TMaskImage, class TOutputLabelImage>
void
LevelSetBasedCellSegmentation<TFeatureImage, TMaskImage, TOutputLabelImage>
::GenerateData()
{
}

}// end namespace itk




#endif
