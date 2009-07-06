/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCopyAttributesLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCopyAttributesLabelMapFilter_txx
#define __itkCopyAttributesLabelMapFilter_txx

#include "itkCopyAttributesLabelMapFilter.h"


namespace itk {

template <class TImage>
CopyAttributesLabelMapFilter<TImage>
::CopyAttributesLabelMapFilter()
{
  this->SetNumberOfRequiredInputs(2);
}


template <class TImage>
void
CopyAttributesLabelMapFilter<TImage>
::ThreadedGenerateData( LabelObjectType * labelObject )
{
  assert( this->GetInput(1) != NULL );
  
  const LabelObjectType * reference = this->GetInput(1)->GetLabelObject( labelObject->GetLabel() );
  labelObject->CopyAttributesFrom( reference );

}

}// end namespace itk
#endif
