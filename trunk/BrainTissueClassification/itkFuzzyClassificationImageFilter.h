/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFuzzyClassificationImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2008-10-18 16:11:15 $
  Version:   $Revision: 1.23 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFuzzyClassificationImageFilter_h
#define __itkFuzzyClassificationImageFilter_h

#include "itkInPlaceImageFilter.h"

#include "itkConceptChecking.h"

namespace itk
{

/** \class FuzzyClassificationImageFilter
 * \brief Set image values to a user-specified value if they are below, 
 * above, or between simple threshold values.
 *
 * FuzzyClassificationImageFilter sets image values to a user-specified "outside"
 * value (by default, "black") if the image values are below, above, or
 * between simple threshold values. 
 *
 * The pixels must support the operators >= and <=.
 * 
 * \ingroup IntensityImageFilters Multithreaded
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT FuzzyClassificationImageFilter : public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FuzzyClassificationImageFilter                Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Some additional typedefs.  */
  typedef TInputImage                           InputImageType;
  typedef typename InputImageType::ConstPointer InputImagePointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename InputImageType::PixelType    InputImagePixelType;

  /** Some additional typedefs.  */
  typedef TOutputImage                          OutputImageType;
  typedef typename OutputImageType::Pointer     OutputImagePointer;
  typedef typename OutputImageType::RegionType  OutputImageRegionType;
  typedef typename OutputImageType::PixelType   OutputImagePixelType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(FuzzyClassificationImageFilter, ImageToImageFilter);

  /** Typedef to describe the type of pixel. */
  typedef typename TInputImage::PixelType PixelType;
    
  /** Set/Get methods for number of classes */
  itkSetMacro(NumberOfClasses, int);
  itkGetMacro(NumberOfClasses, int);

  /** Set/Get methods for bias correction option */
  itkSetMacro(BiasCorrectionOption, int);
  itkGetMacro(BiasCorrectionOption, int);

  /** Set/Get the Bias field. */
  itkSetObjectMacro( BiasField,  OutputImageType );
  itkGetObjectMacro( BiasField,  OutputImageType );

  /** Set/Get the Image Mask. */
  itkSetObjectMacro( ImageMask,  InputImageType );

  virtual void GenerateData();

protected:
  FuzzyClassificationImageFilter();
  ~FuzzyClassificationImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  FuzzyClassificationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_NumberOfClasses;
  int m_BiasCorrectionOption;
  OutputImagePointer m_BiasField;
  InputImagePointer m_ImageMask;
  
};

  
} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFuzzyClassificationImageFilter.txx"
#endif
  
#endif
