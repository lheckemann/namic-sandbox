/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkChangeLabelLabelMapFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/28 19:59:05 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkChangeLabelLabelMapFilter_h
#define __itkChangeLabelLabelMapFilter_h

#include "itkInPlaceLabelMapFilter.h"
#include <map>


namespace itk {
/** \class ChangeLabelLabelMapFilter
 * \brief Allows the Labels on a Label Map to be modified.
 *
 * This filter takes a label map as input and visits all existing labels, checking to see if they
 * differ from the input label map. If the labels are different, the filter updates the label. If the
 * label for a label object does not exist, the filter places a new label object in the label map.
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \sa ShapeLabelObject, RelabelComponentImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */
template<class TImage >
class ITK_EXPORT ChangeLabelLabelMapFilter : 
    public InPlaceLabelMapFilter<TImage>
{
public:
  /** Standard class typedefs. */
  typedef ChangeLabelLabelMapFilter     Self;
  typedef InPlaceLabelMapFilter<TImage> Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Some convenient typedefs. */
  typedef TImage                              ImageType;
  typedef typename ImageType::Pointer         ImagePointer;
  typedef typename ImageType::ConstPointer    ImageConstPointer;
  typedef typename ImageType::PixelType       PixelType;
  typedef typename ImageType::IndexType       IndexType;
  typedef typename ImageType::LabelObjectType LabelObjectType;
  
  /** ImageDimension constants */
  itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(ChangeLabelLabelMapFilter, InPlaceLabelMapFilter);
  
  typedef typename std::map< PixelType, PixelType > ChangeMapType;


#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
/*  itkConceptMacro(InputEqualityComparableCheck,
    (Concept::EqualityComparable<InputImagePixelType>));
  itkConceptMacro(IntConvertibleToInputCheck,
    (Concept::Convertible<int, InputImagePixelType>));
  itkConceptMacro(InputOStreamWritableCheck,
    (Concept::OStreamWritable<InputImagePixelType>));*/
  /** End concept checking */
#endif

  /**
   */
  void SetChangeMap( const ChangeMapType & changeMap )
    {
    if( m_ChangeMap != changeMap )
      {
      m_ChangeMap = changeMap;
      this->Modified();
      }
    }
  const ChangeMapType & GetChangeMap() const
    {
    return m_ChangeMap;
    }
  
  void SetChange( const PixelType & oldLabel, const PixelType & newLabel )
    {
    if( m_ChangeMap.find( oldLabel ) == m_ChangeMap.end() || m_ChangeMap[ oldLabel ] != newLabel )
      {
      m_ChangeMap[ oldLabel ] = newLabel;
      this->Modified();
      }
    }
  
  void ClearChangeMap()
    {
    if( !m_ChangeMap.empty() )
      {
      m_ChangeMap.clear();
      this->Modified();
      }
    }


protected:
  ChangeLabelLabelMapFilter();
  ~ChangeLabelLabelMapFilter() {};

  void GenerateData();

  template <class TAttributeAccessor> void TemplatedGenerateData();
  
  void PrintSelf(std::ostream& os, Indent indent) const;

  ChangeMapType          m_ChangeMap;

private:
  ChangeLabelLabelMapFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkChangeLabelLabelMapFilter.txx"
#endif

#endif
