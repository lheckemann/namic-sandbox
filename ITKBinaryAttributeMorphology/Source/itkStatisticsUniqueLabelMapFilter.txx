/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkStatisticsUniqueLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkStatisticsUniqueLabelMapFilter_txx
#define __itkStatisticsUniqueLabelMapFilter_txx

#include "itkStatisticsUniqueLabelMapFilter.h"
#include "itkLabelMapUtilities.h"


namespace itk {

template <class TImage>
StatisticsUniqueLabelMapFilter<TImage>
::StatisticsUniqueLabelMapFilter()
{
  this->m_Attribute = LabelObjectType::MEAN;
}


template <class TImage>
void
StatisticsUniqueLabelMapFilter<TImage>
::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  switch( this->m_Attribute )
    {
    case LabelObjectType::MINIMUM:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::MinimumLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::MAXIMUM:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::MaximumLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::MEAN:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::MeanLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::SUM:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::SumLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::SIGMA:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::SigmaLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::VARIANCE:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::VarianceLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::MEDIAN:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::MedianLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::KURTOSIS:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::KurtosisLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::SKEWNESS:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::SkewnessLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::ELONGATION:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::ElongationLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    case LabelObjectType::FLATNESS:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::FlatnessLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), this->m_ReverseOrdering );
      break;
    default:
      Superclass::GenerateData();
      break;
    }
}


}// end namespace itk
#endif
