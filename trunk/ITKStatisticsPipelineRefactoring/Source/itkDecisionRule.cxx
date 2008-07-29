/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDecisionRule.cxx,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:29:05 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkDecisionRule.h"

namespace itk
{

DecisionRule
::DecisionRule() 
{
}

DecisionRule
::~DecisionRule()
{
}

void
DecisionRule
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
}// end PrintSelf

} // namespace itk

