/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPointLocator2.txx,v $
  Language:  C++
  Date:      $Date: 2006-03-19 04:36:59 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkPointLocator2_txx
#define _itkPointLocator2_txx
#include "itkPointLocator2.h"

namespace itk
{

template <class TMesh>
PointLocator2<TMesh>
::PointLocator2()
{
}

template <class TMesh>
PointLocator2<TMesh>
::~PointLocator2()
{
}

/**
 * Print out internals
 */
template <class TMesh>
void
PointLocator2<TMesh>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk

#endif
