/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkChangeLabelLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkChangeLabelLabelMapFilter_txx
#define __itkChangeLabelLabelMapFilter_txx

#include "itkChangeLabelLabelMapFilter.h"
#include "itkProgressReporter.h"
#include <deque>


namespace itk {

template <class TImage>
ChangeLabelLabelMapFilter<TImage>
::ChangeLabelLabelMapFilter()
{
}


template <class TImage>
void
ChangeLabelLabelMapFilter<TImage>
::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  ImageType * output = this->GetOutput();

  typedef typename std::deque< typename LabelObjectType::Pointer > VectorType;

  ProgressReporter progress( this, 0, 1 );
  // TODO: report the progress

  // first remove the ones to change and store them elsewhere to process them later
  VectorType labelObjects;
  for( typename ChangeMapType::iterator it = m_ChangeMap.begin();
    it != m_ChangeMap.end();
    it++ )
    {
    if( output->HasLabel( it->first ) && it->first != output->GetBackgroundValue() )
      {
      labelObjects.push_back( output->GetLabelObject( it->first ) );
      output->RemoveLabel( it->first );
      }
    // progress.CompletedPixel();
    }
    
  // then change the label of the background if needed
  if( m_ChangeMap.find( output->GetBackgroundValue() ) != m_ChangeMap.end()
    && m_ChangeMap[ output->GetBackgroundValue() ] !=  output->GetBackgroundValue() )
    {
    PixelType label = m_ChangeMap[ output->GetBackgroundValue() ];
    if( output->HasLabel( label ) )
      {
      // we must have a background - remove that object
      output->RemoveLabel( label );
      }
    output->SetBackgroundValue( label );
    }

  // and put back the objects in the map, with the updated label
  for( typename VectorType::iterator it = labelObjects.begin();
    it != labelObjects.end();
    it++ )
    {
    LabelObjectType * lo = *it;
    PixelType label = m_ChangeMap[ lo->GetLabel() ];
    if( label == output->GetBackgroundValue() )
      {
      // just don't put that object in the label map - it is no in the background
      }
    else if( output->HasLabel( label ) )
      {
      // add the content of the label object to the one already there
      LabelObjectType * mainLo = output->GetLabelObject( label );
      typename LabelObjectType::LineContainerType::const_iterator lit;
      typename LabelObjectType::LineContainerType & lineContainer = lo->GetLineContainer();
    
      for( lit = lineContainer.begin(); lit != lineContainer.end(); lit++ )
        {
        mainLo->AddLine( *lit );
        }
      // be sure to have the lines well organized
      mainLo->Optimize();
      }
    else
      {
      // just put the label object in the label map with the new label
      lo->SetLabel( label );
      output->AddLabelObject( lo );
      }
    
    // go to the next label object
    // progress.CompletedPixel();
    }
}


template <class TImage>
void
ChangeLabelLabelMapFilter<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  // TODO: print the change map?

}

}// end namespace itk
#endif
