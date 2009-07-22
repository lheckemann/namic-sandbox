/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMergeLabelMapFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/28 19:59:05 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMergeLabelMapFilter_h
#define __itkMergeLabelMapFilter_h

#include "itkInPlaceLabelMapFilter.h"

namespace itk {
/** \class MergeLabelMapFilter
 * \brief TODO
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/1926/584  or 
 * http://www.insight-journal.org/browse/publication/176
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \sa ShapeLabelObject, RelabelComponentImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */
template<class TImage >
class ITK_EXPORT MergeLabelMapFilter : 
    public InPlaceLabelMapFilter<TImage>
{
public:
  /** Standard class typedefs. */
  typedef MergeLabelMapFilter           Self;
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
  itkTypeMacro(MergeLabelMapFilter, InPlaceLabelMapFilter);

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

  enum {
    KEEP = 0,
    AGGREGATE = 1,
    PACK = 2,
    STRICT = 3,
  } MethodChoice;

  itkSetMacro( Method, int );
  itkGetConstReferenceMacro( Method, int );

protected:
  MergeLabelMapFilter();
  ~MergeLabelMapFilter() {};

  void GenerateData();

  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef typename ImageType::LabelObjectContainerType    LabelObjectContainerType;
  typedef typename LabelObjectType::LineContainerType     LineContainerType;
  typedef typename LineContainerType::const_iterator      LineContainerIterator;

  MethodChoice         m_Method;

private:
  MergeLabelMapFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  void MergeWithKeep();
  void MergeWithAggregate();
  void MergeWithPack();
  void MergeWithStrict();

}; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMergeLabelMapFilter.txx"
#endif

#endif
