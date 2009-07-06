/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAttributeUniqueLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAttributeUniqueLabelMapFilter_txx
#define __itkAttributeUniqueLabelMapFilter_txx

#include "itkAttributeUniqueLabelMapFilter.h"
#include "itkProgressReporter.h"
#include "itkLabelMapUtilities.h"


namespace itk {

template <class TImage, class TAttributeAccessor>
AttributeUniqueLabelMapFilter<TImage, TAttributeAccessor>
::AttributeUniqueLabelMapFilter()
{
  m_ReverseOrdering = false;
}


template <class TImage, class TAttributeAccessor>
void
AttributeUniqueLabelMapFilter<TImage, TAttributeAccessor>
::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  LabelMapUtilities::UniqueGenerateData<Self, TAttributeAccessor>( this, this->GetLabelMap(), m_ReverseOrdering );
}


template <class TImage, class TAttributeAccessor>
void
AttributeUniqueLabelMapFilter<TImage, TAttributeAccessor>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "ReverseOrdering: "  << m_ReverseOrdering << std::endl;
}

}// end namespace itk
#endif
