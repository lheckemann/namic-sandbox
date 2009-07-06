/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPackLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPackLabelMapFilter_txx
#define __itkPackLabelMapFilter_txx

#include "itkPackLabelMapFilter.h"
#include "itkProgressReporter.h"


namespace itk {

template <class TImage>
void
PackLabelMapFilter<TImage>
::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  ImageType * output = this->GetOutput();

  // get the label objects
  typedef typename ImageType::LabelObjectContainerType LabelObjectContainerType;
  LabelObjectContainerType labelObjects = output->GetLabelObjectContainer();

  ProgressReporter progress( this, 0, labelObjects.size() );

  // and put back the objects in the map
  output->ClearLabels();
  for( typename LabelObjectContainerType::iterator it = labelObjects.begin();
    it != labelObjects.end();
    it++ )
    {
    output->PushLabelObject( it->second );
    
    // go to the next label
    progress.CompletedPixel();
    }
}

}// end namespace itk
#endif
