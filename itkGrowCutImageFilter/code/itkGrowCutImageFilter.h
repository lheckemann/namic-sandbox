/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplacianImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2007/09/19 17:24:12 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrowCutImageFilter_h
#define __itkGrowCutImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

namespace itk
{
/**
 * \class Grow Cut
 *
 * This filter implement the Grow Cut algorithm from the paper 
 * "Vezhnevets V., Konouchine V., 
 * 'Grow-Cut' - Interactive Multi-Label N-D Image Segmentation.
 * Graphicon-2005.
 *
 * 
 * 
 *
 * \par Inputs and Outputs
 * The input to this filter are an itk::Image of arbitrary dimension and a 
 * scalar-valued(integer valued) initial label map. The output is a scalar-
 * valued(integer valued) final label map of the type itk::Image. 
 *
 *
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * \sa NeighborhoodIterator
 *
 * */
template <class TInputImage, class TLabelMapImage>
class ITK_EXPORT GrowCutImageFilter : 
    public ImageToImageFilter< TInputImage, TLabelMapImage > 
{
public:
  /** Standard "Self" & Superclass typedef.   */
  typedef GrowCutImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TLabelMapImage > Superclass;

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  typedef typename TLabelMapImage::PixelType LabelMapPixelType;
  //  typedef typename TLabelMapImage::InternalPixelType LabelMapInternalPixelType;
  typedef typename TInputImage::PixelType InputPixelType;
  //  typedef typename TInputImage::InternalPixelType InputInternalPixelType;

  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(LabelMapDimension, unsigned int,
                      TLabelMapImage::ImageDimension);
  
  /** Image typedef support. */
  typedef TInputImage  InputImageType;
  typedef TLabelMapImage LabelMapImageType;
  typedef typename InputImageType::Pointer InputImagePointer;
  typedef typename LabelMapImageType::Pointer LabelMapImagePointer;

  /** Smart pointer typedef support.   */
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;


  
  /** Run-time type information (and related methods)  */
  itkTypeMacro(GrowCutImageFilter, ImageToImageFilter);
  
  /** Method for creation through the object factory.  */
  itkNewMacro(Self);


  /** Use the image spacing information in calculations. Use this option if you
   *  want derivatives in physical space. Default is UseImageSpacingOn. */
//   void SetUseImageSpacingOn()
//   { this->SetUseImageSpacing(true); }
  
  /** Ignore the image spacing. Use this option if you want derivatives in
      isotropic pixel space.  Default is UseImageSpacingOn. */
//   void SetUseImageSpacingOff()
//   { this->SetUseImageSpacing(false); }
  
  /** Set/Get whether or not the filter will use the spacing of the input
      image in its calculations */
//   itkSetMacro(UseImageSpacing, bool);
//   itkGetMacro(UseImageSpacing, bool);

  itkSetMacro(InitLabelMap, LabelMapImagePointer);
  itkGetMacro(InitLabelMap, LabelMapImagePointer);

// #ifdef ITK_USE_CONCEPT_CHECKING
//   /** Begin concept checking */
//   itkConceptMacro(SameDimensionCheck,
//     (Concept::SameDimension<InputImageDimension, ImageDimension>));
//   itkConceptMacro(InputPixelTypeIsFloatingPointCheck,
//     (Concept::IsFloatingPoint<InputPixelType>));
//   itkConceptMacro(OutputPixelTypeIsFloatingPointCheck,
//     (Concept::IsFloatingPoint<OutputPixelType>));
//   /** End concept checking */
// #endif

protected:
  GrowCutImageFilter()
    {
      m_InitLabelMap = 0;    
    }
  virtual ~GrowCutImageFilter()  {}

  /** Standard pipeline method. While this class does not implement a
   * ThreadedGenerateData(), its GenerateData() delegates all
   * calculations to an NeighborhoodOperatorImageFilter.  Since the
   * NeighborhoodOperatorImageFilter is multithreaded, this filter is
   * multithreaded by default.   */
  void GenerateData();
  void PrintSelf(std::ostream&, Indent) const;

private:
  GrowCutImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  LabelMapImagePointer m_InitLabelMap;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGrowCutImageFilter.txx"
#endif

#endif
