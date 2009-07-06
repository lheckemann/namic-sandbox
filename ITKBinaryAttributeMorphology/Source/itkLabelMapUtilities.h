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
#ifndef __itkLabelMapUtilities_h
#define __itkLabelMapUtilities_h

namespace itk {
namespace LabelMapUtilities {

template<class TLabelObject >
struct LineOfLabelObject
  {
  typedef TLabelObject                       LabelObjectType;
  typedef typename LabelObjectType::LineType LineType;
  
  LineOfLabelObject( const LineType line, LabelObjectType * lo )
    {
    this->line = line;
    this->labelObject = lo;
    }
  LineType          line;
  LabelObjectType * labelObject;
  };

template<class TLabelObject >
class LineOfLabelObjectComparator
  {
  public:
    typedef LineOfLabelObject< TLabelObject> LineOfLabelObjectType;
    itkStaticConstMacro(ImageDimension, unsigned int, TLabelObject::ImageDimension);
    bool operator()( const LineOfLabelObjectType & lla, const LineOfLabelObjectType & llb )
      {
      for( int i=ImageDimension-1; i>=0; i-- )
        {
        if( lla.line.GetIndex()[i] > llb.line.GetIndex()[i] )
          {
          return true;
          }
        else if( lla.line.GetIndex()[i] < llb.line.GetIndex()[i] )
          {
          return false;
          }
        }
      return false;
      }
  };


template<class TFilter, class TAttributeAccessor>
void UniqueGenerateData( TFilter * self, typename TFilter::ImageType * labelMap, bool reverseOrder );

}
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabelMapUtilities.txx"
#endif

#endif
