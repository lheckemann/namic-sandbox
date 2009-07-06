/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCopyAttributesLabelMapFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/28 19:59:05 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCopyAttributesLabelMapFilter_h
#define __itkCopyAttributesLabelMapFilter_h

#include "itkInPlaceLabelMapFilter.h"

namespace itk {
/** \class CopyAttributesLabelMapFilter
 * \brief Copy the attribute values from a reference LabelMap to the object of the input LabelMap
 *
 * Reference image is the input 1.
 *
 * This filter let the user get the attributes values back in the LabelMap after the conversion to
 * an image representation which doesn't support attributes - like itk::Image, with
 * itk::LabelMapToLabelImageFilter. The pipeline can be something like
 *
 * LM --- LabelMapToLabelImageFilter ---> I --- some transforms ---> I --- LabelImageToLabelMapFilter ---> LM --- CopyAttributesLabelMapFilter ---> LM
 *  |                                                                                                                |
 *  `----------------------------------------------------------------------------------------------------------------/
 *
 * TODO: make the filter support another type of LabelObject for the reference image. 
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \sa ShapeLabelObject, RelabelComponentImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */
template<class TImage >
class ITK_EXPORT CopyAttributesLabelMapFilter : 
    public InPlaceLabelMapFilter<TImage>
{
public:
  /** Standard class typedefs. */
  typedef CopyAttributesLabelMapFilter    Self;
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
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(CopyAttributesLabelMapFilter, 
               InPlaceLabelMapFilter);

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

protected:
  CopyAttributesLabelMapFilter();
  ~CopyAttributesLabelMapFilter() {};

  virtual void ThreadedGenerateData( LabelObjectType * labelObject );
  
private:
  CopyAttributesLabelMapFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCopyAttributesLabelMapFilter.txx"
#endif

#endif
