/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTestingMacros.h,v $
  Language:  C++
  Date:      $Date: 2009-05-09 17:40:20 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking.h"

#include "itkMacro.h"
#include "vtkPoints.h"

/** Constructor */
template <class TDeformationFilter,class TPointSet>
MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking<TDeformationFilter,TPointSet>
::MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking()
{
}

/** Destructor */
template <class TDeformationFilter,class TPointSet>
MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking<TDeformationFilter,TPointSet>
::~MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking()
{
}


