/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsKeepNObjectsLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkStatisticsKeepNObjectsLabelMapFilter_txx
#define __itkStatisticsKeepNObjectsLabelMapFilter_txx

#include "itkStatisticsKeepNObjectsLabelMapFilter.h"
#include "itkProgressReporter.h"


namespace itk {

template <class TImage>
StatisticsKeepNObjectsLabelMapFilter<TImage>
::StatisticsKeepNObjectsLabelMapFilter()
{
  this->m_Attribute = LabelObjectType::MEAN;
  // create the output image for the removed objects
  this->SetNumberOfRequiredOutputs(2);
  this->SetNthOutput( 1, static_cast<TImage*>(this->MakeOutput(1).GetPointer() ) );
}


template <class TImage>
void
StatisticsKeepNObjectsLabelMapFilter<TImage>
::GenerateData()
{
  switch( this->m_Attribute )
    {
    case LabelObjectType::MINIMUM:
      TemplatedGenerateData< typename Functor::MinimumLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::MAXIMUM:
      TemplatedGenerateData< typename Functor::MaximumLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::MEAN:
      TemplatedGenerateData< typename Functor::MeanLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::SUM:
      TemplatedGenerateData< typename Functor::SumLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::SIGMA:
      TemplatedGenerateData< typename Functor::SigmaLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::VARIANCE:
      TemplatedGenerateData< typename Functor::VarianceLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::MEDIAN:
      TemplatedGenerateData< typename Functor::MedianLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::KURTOSIS:
      TemplatedGenerateData< typename Functor::KurtosisLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::SKEWNESS:
      TemplatedGenerateData< typename Functor::SkewnessLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::ELONGATION:
      TemplatedGenerateData< typename Functor::ElongationLabelObjectAccessor< LabelObjectType > >();
      break;
    case LabelObjectType::FLATNESS:
      TemplatedGenerateData< typename Functor::FlatnessLabelObjectAccessor< LabelObjectType > >();
      break;
    default:
      Superclass::GenerateData();
      break;
    }
}


template <class TImage>
template <class TAttributeAccessor>
void
StatisticsKeepNObjectsLabelMapFilter<TImage>
::TemplatedGenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  ImageType * output = this->GetOutput();
  ImageType * output2 = this->GetOutput( 1 );

  // set the background value for the second output - this is not done in the superclasses
  output2->SetBackgroundValue( output->GetBackgroundValue() );

  typedef typename ImageType::LabelObjectContainerType LabelObjectContainerType;
  const LabelObjectContainerType & labelObjectContainer = output->GetLabelObjectContainer();
  typedef typename std::vector< typename LabelObjectType::Pointer > VectorType;

  ProgressReporter progress( this, 0, 2 * labelObjectContainer.size() );

  // get the label objects in a vector, so they can be sorted
  VectorType labelObjects;
  labelObjects.reserve( labelObjectContainer.size() );
  typename LabelObjectContainerType::const_iterator it = labelObjectContainer.begin();
   while(it != labelObjectContainer.end() )
    {
    labelObjects.push_back( it->second );
    progress.CompletedPixel();
    it++;
    }

  // instantiate the comparator and sort the vector
  if( this->m_NumberOfObjects < labelObjectContainer.size() )
    {
    typename VectorType::iterator end = labelObjects.begin() + this->m_NumberOfObjects;
    if( this->m_ReverseOrdering )
      {
      Functor::LabelObjectReverseComparator< LabelObjectType, TAttributeAccessor > comparator;
      std::nth_element( labelObjects.begin(), end, labelObjects.end(), comparator );
      }
    else
      {
      Functor::LabelObjectComparator< LabelObjectType, TAttributeAccessor > comparator;
      std::nth_element( labelObjects.begin(), end, labelObjects.end(), comparator );
      }
    progress.CompletedPixel();
  
    // and remove the last objects of the map
    typename VectorType::const_iterator it = end;
    while( it != labelObjects.end() )
      {
      output2->AddLabelObject( *it );
      output->RemoveLabelObject( *it );
      progress.CompletedPixel();
      it++;
      }
    }
}

}// end namespace itk
#endif