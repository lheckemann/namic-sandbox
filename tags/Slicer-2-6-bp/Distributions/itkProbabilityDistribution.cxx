/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaussianDistributionFunction.txx,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:54:55 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkProbabilityDistribution.h"

namespace itk{ 
namespace Statistics{

double
ProbabilityDistribution
::EvaluatePDF(double x) const
{ 
  return 0.0;
}


double
ProbabilityDistribution
::EvaluateCDF(double x) const
{ 

  return 0.0;
}


double
ProbabilityDistribution
::EvaluateInverseCDF(double p) const
{ 
  return 0.0;
}


} // end namespace Statistics
} // end of namespace itk


