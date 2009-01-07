/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNarrowBandSolverLevelSetImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/05 13:59:36 $
  Version:   $Revision: 1.10 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNarrowBandSolverLevelSetImageFilter_h_
#define __itkNarrowBandSolverLevelSetImageFilter_h_

#include "itkNarrowBandImageFilterBase.h"
#include "itkFastChamferDistanceImageFilter.h"
#include "itkFastAnisotropicChamferDistanceImageFilter.h"
#include "itkIsoContourDistanceImageFilter.h"
#include "itkReinitializeLevelSetImageFilter.h"

namespace itk {

/**
 *
 * \class NarrowBandSolverLevelSetImageFilter
 * 
 * \brief A base class which provides a specialization of the NarrowBand solver for 
 * level set methods.
 *
 * \par OVERVIEW
 * This class is a specialization of the NarrowBandImageFilterBase class for level set
 * methods. It provides the method for reinitializing the distance function around
 * the zero level set within the narrow band. 
 * 
 * \par TEMPLATE PARAMETERS
 * There are two required template parameters for these filters, the type of the
 * InputImage and the type of the OutputImage. 
 *
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT NarrowBandSolverLevelSetImageFilter
  : public NarrowBandImageFilterBase<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs */
  typedef NarrowBandSolverLevelSetImageFilter Self;
  typedef NarrowBandImageFilterBase<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Inherited typedef from the superclass. */
  typedef typename Superclass::InputImageType  InputImageType;
  typedef typename Superclass::OutputImageType OutputImageType;
   
  /** Run-time type information (and related methods). */
  itkTypeMacro(NarrowBandSolverLevelSetImageFilter, NarrowBandImageFilterBase);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(OutputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  virtual ~NarrowBandSolverLevelSetImageFilter() {}
  NarrowBandSolverLevelSetImageFilter();

  virtual void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Tells the solver how to reinitialize the narrowband when the reinitialization
    * criterion meets */
  
  virtual void CreateNarrowBand ();

  /** Reinitialization filters **/ 
  /** Internal filter types used for reinitialization */ 
  typedef IsoContourDistanceImageFilter<OutputImageType,OutputImageType>
          IsoFilterType;
  typedef FastChamferDistanceImageFilter<OutputImageType,OutputImageType>
          ChamferFilterType; 
  typedef FastAnisotropicChamferDistanceImageFilter<OutputImageType,OutputImageType>
          AnisotropicChamferFilterType; 

  typename IsoFilterType::Pointer     m_IsoFilter;
  typename ChamferFilterType::Pointer m_ChamferFilter;
  typename AnisotropicChamferFilterType::Pointer m_AnisotropicChamferFilter;

private:
  NarrowBandSolverLevelSetImageFilter(const Self &); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNarrowBandSolverLevelSetImageFilter.txx"
#endif

#endif

