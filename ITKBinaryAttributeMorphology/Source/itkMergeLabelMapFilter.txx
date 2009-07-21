/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMergeLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMergeLabelMapFilter_txx
#define __itkMergeLabelMapFilter_txx

#include "itkMergeLabelMapFilter.h"
#include "itkProgressReporter.h"
#include <deque>


namespace itk {

template <class TImage>
MergeLabelMapFilter<TImage>
::MergeLabelMapFilter()
{
  m_Method = KEEP;
}

template <class TImage>
void
MergeLabelMapFilter<TImage>
::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  ImageType * output = this->GetOutput();

  ProgressReporter progress( this, 0, 1 );
  // TODO: really report the progress

  KeepMethod();
  StrictMethod();
  AggregateMethod();
  PackMethod();
} 

template <class TImage>
void
MergeLabelMapFilter<TImage>
::KeepMethod()
{
}
 
  typedef typename ImageType::LabelObjectContainerType LabelObjectContainerType;

  if( m_Method == KEEP )
    {
    typedef typename std::deque< typename LabelObjectType::Pointer > VectorType;
    VectorType labelObjects;
    for( unsigned int i=1; i < this->GetNumberOfInputs(); i++ )
      {
            
      const LabelObjectContainerType & otherLabelObjects = this->GetInput(i)->GetLabelObjectContainer();
      typename LabelObjectContainerType::const_iterator it2 = otherLabelObjects.begin();
      while( it2 != otherLabelObjects.end() )
      {
        {
        const LabelObjectType * lo = it2->second;
        typename LabelObjectType::Pointer newLo = LabelObjectType::New();
        newLo->CopyAllFrom( lo );
        
        if( ! output->HasLabel( newLo->GetLabel() ) )
          {
          // we can keep the label
          output->AddLabelObject( newLo );
          }
        else
          {
          // store the label object to read it later with another label
          labelObjects.push_back( newLo );
          }
        
        // go to the next label
        progress.CompletedPixel();
        }
        it2++;
      }
      
    // add the other label objects, with a different label
    for( typename VectorType::iterator it = labelObjects.begin();
      it != labelObjects.end();
      it++ )
      {
      output->PushLabelObject( *it );
      }
    }
  else if( m_Method == STRICT )
    {
    for( unsigned int i=1; i<this->GetNumberOfInputs(); i++ )
      {
      const LabelObjectContainerType & otherLabelObjects = this->GetInput(i)->GetLabelObjectContainer();
      for( typename LabelObjectContainerType::const_iterator it2 = otherLabelObjects.begin();
        it2 != otherLabelObjects.end();
        it2++ )
        {
        const LabelObjectType * lo = it2->second;
        typename LabelObjectType::Pointer newLo = LabelObjectType::New();
        newLo->CopyAllFrom( lo );
        
        if( ! output->HasLabel( newLo->GetLabel() ) )
          {
          // we can keep the label
          output->AddLabelObject( newLo );
          }
        else
          {
          itkExceptionMacro(<< "Label " 
            << static_cast< typename itk::NumericTraits< PixelType >::PrintType >( newLo->GetLabel() )
            << " from input " << i
            << " is already in use.");
          }
        
        // go to the next label
        progress.CompletedPixel();
        }
      }
    }
  else if( m_Method == AGGREGATE )
    {
    for( unsigned int i=1; i<this->GetNumberOfInputs(); i++ )
      {
      const LabelObjectContainerType & otherLabelObjects = this->GetInput(i)->GetLabelObjectContainer();
      for( typename LabelObjectContainerType::const_iterator it2 = otherLabelObjects.begin();
        it2 != otherLabelObjects.end();
        it2++ )
        {
        const LabelObjectType * lo = it2->second;
        
        if( ! output->HasLabel( lo->GetLabel() ) )
          {
          // we can keep the label
          typename LabelObjectType::Pointer newLo = LabelObjectType::New();
          newLo->CopyAllFrom( lo );
          output->AddLabelObject( newLo );
          }
        else if( lo->GetLabel() == output->GetBackgroundValue() )
          {
          // just do nothing, so the label object will be in the background
          }
        else
          {
          // add the lines of that object to the one already in the output
          LabelObjectType * mainLo = output->GetLabelObject( lo->GetLabel() );
          typename LabelObjectType::LineContainerType::const_iterator lit;
          const typename LabelObjectType::LineContainerType & lineContainer = lo->GetLineContainer();
        
          for( lit = lineContainer.begin(); lit != lineContainer.end(); lit++ )
            {
            mainLo->AddLine( *lit );
            }
          // be sure to have the lines well organized
          mainLo->Optimize();
          }
        
        // go to the next label
        progress.CompletedPixel();
        }
      }
    }
  else if( m_Method == PACK )
    {
    // get the label objects of the first input
    LabelObjectContainerType labelObjects = output->GetLabelObjectContainer();

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

    // now, the next images
    for( unsigned int i=1; i<this->GetNumberOfInputs(); i++ )
      {
      const LabelObjectContainerType & otherLabelObjects = this->GetInput(i)->GetLabelObjectContainer();
      for( typename LabelObjectContainerType::const_iterator it2 = otherLabelObjects.begin();
        it2 != otherLabelObjects.end();
        it2++ )
        {
        const LabelObjectType * lo = it2->second;
        typename LabelObjectType::Pointer newLo = LabelObjectType::New();
        newLo->CopyAllFrom( lo );
        output->PushLabelObject( newLo );
        
        // go to the next label
        progress.CompletedPixel();
        }
      }
    }
  else
    {
    itkExceptionMacro(<< "No such method: " << m_Method);
    }
}


template <class TImage>
void
MergeLabelMapFilter<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Method: "  << m_Method << std::endl;
}

}// end namespace itk
#endif
