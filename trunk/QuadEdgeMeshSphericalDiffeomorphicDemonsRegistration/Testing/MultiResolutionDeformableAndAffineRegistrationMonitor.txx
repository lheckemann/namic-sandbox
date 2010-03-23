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

#include "MultiResolutionDeformableAndAffineRegistrationMonitor.h"

#include "itkMacro.h"
#include "vtkPoints.h"

/** Constructor */
template <class TPointSet>
MultiResolutionDeformableAndAffineRegistrationMonitor<TPointSet>
::MultiResolutionDeformableAndAffineRegistrationMonitor()
{
}

/** Destructor */
template <class TPointSet>
MultiResolutionDeformableAndAffineRegistrationMonitor<TPointSet>
::~MultiResolutionDeformableAndAffineRegistrationMonitor()
{
}

/** Set Number of Resolution Levels */
template <class TPointSet>
void
MultiResolutionDeformableAndAffineRegistrationMonitor<TPointSet>
::SetNumberOfResolutionLevels( unsigned int number )
{
  this->m_FixedSurfaces.resize( number );
  this->m_MovingSurfaces.resize( number );
}

/** Set Surface At a Given Resolution. */
template <class TPointSet>
void
MultiResolutionDeformableAndAffineRegistrationMonitor<TPointSet>
::SetFixedSurface(unsigned int level, vtkPolyData * surface )
{
  this->m_FixedSurfaces[level] = surface;
}

/** Set Surface At a Given Resolution. */
template <class TPointSet>
void
MultiResolutionDeformableAndAffineRegistrationMonitor<TPointSet>
::SetMovingSurface(unsigned int level, vtkPolyData * surface )
{
  this->m_MovingSurfaces[level] = surface;
}

/** Set Surface At a Given Resolution. */
template <class TPointSet>
void
MultiResolutionDeformableAndAffineRegistrationMonitor<TPointSet>
::SetResolutionLevel(unsigned int level)
{
  this->SetFixedSurface(  this->m_FixedSurfaces[level]  );
  this->SetMovingSurface( this->m_MovingSurfaces[level] );
}

