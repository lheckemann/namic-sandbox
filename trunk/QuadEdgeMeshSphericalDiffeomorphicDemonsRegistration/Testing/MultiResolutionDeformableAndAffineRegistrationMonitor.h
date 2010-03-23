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

#ifndef __MultiResolutionDeformableAndAffineRegistrationMonitor_h
#define __MultiResolutionDeformableAndAffineRegistrationMonitor_h

#include "DeformableAndAffineRegistrationMonitor.h"
#include <vector>

/** \class MultiResolutionDeformableAndAffineRegistrationMonitor 
 *  This class provides a VTK visualization pipeline configured for monitoring
 *  the progress of a deformable registration process. It take multiple resolution
 *  surfaces and use them for the visualization of the proper resolution level. 
 */
template <class TPointSet>
class MultiResolutionDeformableAndAffineRegistrationMonitor : public DeformableAndAffineRegistrationMonitor<TPointSet>
{
public:
  
  typedef MultiResolutionDeformableAndAffineRegistrationMonitor  Self;
  typedef DeformableAndAffineRegistrationMonitor<TPointSet>      Superclass;

  MultiResolutionDeformableAndAffineRegistrationMonitor();
  virtual ~MultiResolutionDeformableAndAffineRegistrationMonitor();

private:
  typedef std::vector< vtkPolyData * >     PolyDataArray;

  PolyDataArray    m_FixedSurfaces;
  PolyDataArray    m_MovingSurfaces;
};

#include "MultiResolutionDeformableAndAffineRegistrationMonitor.txx"

#endif
