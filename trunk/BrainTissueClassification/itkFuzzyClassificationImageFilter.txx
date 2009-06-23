/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFuzzyClassificationImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-18 16:11:15 $
  Version:   $Revision: 1.30 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFuzzyClassificationImageFilter_txx
#define __itkFuzzyClassificationImageFilter_txx

#include "itkFuzzyClassificationImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"
#include "itkObjectFactory.h"
#include "itkProgressReporter.h"

namespace itk
{

/**
 *
 */
template <class TInputImage, class TOutputImage>
FuzzyClassificationImageFilter<TInputImage, TOutputImage>
::FuzzyClassificationImageFilter()
{
  m_NumberOfClasses = 3;
  m_BiasCorrectionOption = 0;
  m_ImageMask = NULL;
}


/**
 *
 */
template <class TInputImage, class TOutputImage>
void
FuzzyClassificationImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Number Of Classes: "
     << m_NumberOfClasses
     << std::endl;
  os << indent << "Bias Correction Option: "
     << m_BiasCorrectionOption
     << std::endl;
}


/**
 *
 */
template <class TInputImage, class TOutputImage>
void 
FuzzyClassificationImageFilter<TInputImage, TOutputImage>
::GenerateData( )
{
  itkDebugMacro(<<"Actually executing");

  // Get the input and output pointers
  InputImagePointer  inputPtr  = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput(0);

}

} // end namespace itk

#endif
