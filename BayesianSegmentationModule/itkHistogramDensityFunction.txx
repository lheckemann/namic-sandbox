/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramDensityFunction.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/15 01:00:46 $
  Version:   $Revision: 1.20 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogramDensityFunction_txx
#define __itkHistogramDensityFunction_txx

#include "itkHistogramDensityFunction.h"

namespace itk{ 
namespace Statistics{

template < class TMeasurementVector >
HistogramDensityFunction< TMeasurementVector >
::HistogramDensityFunction()
{
}

template < class TMeasurementVector >
HistogramDensityFunction< TMeasurementVector >
::~HistogramDensityFunction()
{
}

template < class TMeasurementVector >
void  
HistogramDensityFunction< TMeasurementVector >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


template < class TMeasurementVector >
inline double
HistogramDensityFunction< TMeasurementVector >
::Evaluate(const MeasurementVectorType &measurement) const
{ 
   double probability = 0.0;
   return probability;
}
  

} // end namespace Statistics
} // end of namespace itk



#endif
