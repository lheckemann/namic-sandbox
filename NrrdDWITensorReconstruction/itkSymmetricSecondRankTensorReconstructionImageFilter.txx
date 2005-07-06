/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBinaryDilateImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSymmetricSecondRankTensorReconstructionImageFilter_txx
#define __itkSymmetricSecondRankTensorReconstructionImageFilter_txx

#include "itkSymmetricSecondRankTensorReconstructionImageFilter.h"

namespace itk {

template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::SymmetricSecondRankTensorReconstructionImageFilter()
{
  this->SetNumberOfRequiredInputs( 6 ); // At least 6 inputs are necessary
}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::~SymmetricSecondRankTensorReconstructionImageFilter()
{
  
}




template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::BeforeThreadedGenerateData()
{
  int numberOfThreads = this->GetNumberOfThreads();
}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::AfterThreadedGenerateData()
{
}


template< class TReferenceImagePixelType, 
          class TGradientImagePixelType, class TTensorPixelType >
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::ThreadedGenerateData()
{
}



template <class TImage>
void SymmetricSecondRankTensorReconstructionImageFilter< TReferenceImagePixelType,
  TGradientImagePixelType, TTensorPixelType >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "TensorBasisCoeffs: " << m_TensorBasisCoeffs << std::endl;
  os << indent << "GradientDirectionContainer: "
     << m_GradientDirectionContainer << std::endl;
  os << indent << "NumberOfGradientDirections: " << 
              m_NumberOfGradientDirections << std::endl;
  os << indent << "Threshold for reference B0 image: " << m_Threshold << std::endl;
}









}

#endif
