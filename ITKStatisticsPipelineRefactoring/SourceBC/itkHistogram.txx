/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogram.txx,v $
  Language:  C++
  Date:      $Date: 2009-04-26 00:37:48 $
  Version:   $Revision: 1.53 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogram_txx
#define __itkHistogram_txx

#include "itkHistogram.h"

namespace itk { 
namespace Statistics {

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Histogram()
{
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
void
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Initialize(const SizeType &size)
{
  typename Superclass::SizeType newSize;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newSize[i] = size[i];
    }
  Superclass::Initialize(newSize);  
}

template< class TMeasurement, unsigned int VMeasurementVectorSize,
          class TFrequencyContainer>
void
Histogram<TMeasurement, VMeasurementVectorSize, TFrequencyContainer>
::Initialize(const SizeType &size, MeasurementVectorType& lowerBound,
             MeasurementVectorType& upperBound)
{
  typename Superclass::SizeType newSize;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newSize[i] = size[i];
    }
  typename Superclass::MeasurementVectorType newLowerBound;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newLowerBound[i] = lowerBound[i];
    }
  typename Superclass::MeasurementVectorType newUpperBound;
  for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
    {
    newUpperBound[i] = upperBound[i];
    }
  Superclass::Initialize(newSize, newLowerBound, newUpperBound);  
}

} // end of namespace Statistics 
} // end of namespace itk 

#endif
