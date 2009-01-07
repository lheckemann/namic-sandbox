/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNarrowBandSolverLevelSetImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/21 22:47:27 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNarrowBandSolverLevelSetImageFilter_txx_
#define __itkNarrowBandSolverLevelSetImageFilter_txx_

#include "itkNarrowBandSolverLevelSetImageFilter.h"
#include "stdio.h"

namespace itk {

template <class TInputImage, class TOutputImage>
void
NarrowBandSolverLevelSetImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

template <class TInputImage, class TOutputImage>
NarrowBandSolverLevelSetImageFilter<TInputImage, TOutputImage>
::NarrowBandSolverLevelSetImageFilter()
{
  m_IsoFilter = IsoFilterType::New();
  m_ChamferFilter = ChamferFilterType::New(); 
  m_AnisotropicChamferFilter = AnisotropicChamferFilterType::New(); 
}

template <class TInputImage, class TOutputImage>
void
NarrowBandSolverLevelSetImageFilter<TInputImage, TOutputImage>
::CreateNarrowBand()
{
  typename OutputImageType::Pointer levelset = this->GetOutput();

  if (!this->m_NarrowBand->Empty())
    {
    m_IsoFilter->SetNarrowBand(this->m_NarrowBand.GetPointer());
    m_IsoFilter->NarrowBandingOn(); //Maybe we should check that the NarrowBand exits.
    }
  else
    {
    m_IsoFilter->NarrowBandingOff();
    }  
  
  m_IsoFilter->SetUseImageSpacing(this->GetUseImageSpacing());
  m_IsoFilter->SetFarValue(this->m_NarrowBand->GetTotalRadius()+1);
  m_IsoFilter->SetInput(levelset);
  m_IsoFilter->Update();

  if (!this->GetUseImageSpacing())
    {
    m_ChamferFilter->SetInput(m_IsoFilter->GetOutput());
    m_ChamferFilter->SetMaximumDistance(this->m_NarrowBand->GetTotalRadius()+1);
    m_ChamferFilter->SetNarrowBand(this->m_NarrowBand.GetPointer());
    m_ChamferFilter->Update();
  
    this->GraftOutput(m_ChamferFilter->GetOutput());
    }
  else
    {
    m_AnisotropicChamferFilter->SetInput(m_IsoFilter->GetOutput());
    m_AnisotropicChamferFilter->SetMaximumDistance(this->m_NarrowBand->GetTotalRadius()+1);
    m_AnisotropicChamferFilter->SetNarrowBand(this->m_NarrowBand.GetPointer());
    m_AnisotropicChamferFilter->SetUseImageSpacing(this->GetUseImageSpacing());
    m_AnisotropicChamferFilter->Update();
  
    this->GraftOutput(m_AnisotropicChamferFilter->GetOutput());
    }

  m_IsoFilter->SetInput(NULL);
  m_ChamferFilter->SetInput(NULL);
  m_AnisotropicChamferFilter->SetInput(NULL);
}

} // end namespace itk

#endif
