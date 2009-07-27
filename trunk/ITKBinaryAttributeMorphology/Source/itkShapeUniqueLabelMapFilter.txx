/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkShapeUniqueLabelMapFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:09:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShapeUniqueLabelMapFilter_txx
#define __itkShapeUniqueLabelMapFilter_txx

#include "itkShapeUniqueLabelMapFilter.h"
#include "itkLabelMapUtilities.h"


namespace itk {

template <class TImage>
ShapeUniqueLabelMapFilter<TImage>
::ShapeUniqueLabelMapFilter()
{
  m_ReverseOrdering = false;
  m_Attribute = LabelObjectType::SIZE;
}


template <class TImage>
void
ShapeUniqueLabelMapFilter<TImage>
::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  switch( m_Attribute )
    {
    case LabelObjectType::LABEL:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::LabelLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::SIZE:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::SizeLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::PHYSICAL_SIZE:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::PhysicalSizeLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::SIZE_REGION_RATIO:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::SizeRegionRatioLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::REGION_ELONGATION:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::RegionElongationLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::SIZE_ON_BORDER:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::SizeOnBorderLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::PHYSICAL_SIZE_ON_BORDER:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::PhysicalSizeOnBorderLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::FERET_DIAMETER:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::FeretDiameterLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::BINARY_ELONGATION:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::BinaryElongationLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::PERIMETER:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::PerimeterLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::ROUNDNESS:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::RoundnessLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::EQUIVALENT_RADIUS:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::EquivalentRadiusLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::EQUIVALENT_PERIMETER:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::EquivalentPerimeterLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    case LabelObjectType::BINARY_FLATNESS:
      LabelMapUtilities::UniqueGenerateData< Self, typename Functor::BinaryFlatnessLabelObjectAccessor< LabelObjectType > >( this, this->GetLabelMap(), m_ReverseOrdering );
      break;
    default:
      itkExceptionMacro(<< "Unknown attribute type");
      break;
    }
}


template <class TImage>
void
ShapeUniqueLabelMapFilter<TImage>
::PrintSelf( std::ostream &os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "ReverseOrdering: "  << m_ReverseOrdering << std::endl;
  os << indent << "Attribute: "  << LabelObjectType::GetNameFromAttribute( m_Attribute ) << " (" << m_Attribute << ")" << std::endl;
}

}// end namespace itk
#endif
