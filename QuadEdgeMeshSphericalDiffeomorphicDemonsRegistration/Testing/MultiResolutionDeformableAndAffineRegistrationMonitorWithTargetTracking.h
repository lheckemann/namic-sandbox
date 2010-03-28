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

#ifndef __MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking_h
#define __MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking_h

#include "MultiResolutionDeformableAndAffineRegistrationMonitor.h"
#include <vector>

/** \class MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking 
 *  This class provides a VTK visualization pipeline configured for monitoring
 *  the progress of a deformable registration process. It take multiple resolution
 *  surfaces and use them for the visualization of the proper resolution level. 
 *  It also takes two surfaces as source and target and track the progress of the
 *  destination points from the source to the target with an RMS measure of distances.
 */
template <class TMultiResolutionDeformationFilter, class TPointSet>
class MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking : 
  public MultiResolutionDeformableAndAffineRegistrationMonitor<
    typename TMultiResolutionDeformationFilter::DeformationFilterType, TPointSet>
{
public:
  
  typedef MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking  Self;
  typedef MultiResolutionDeformableAndAffineRegistrationMonitor<
    typename TMultiResolutionDeformationFilter::DeformationFilterType, TPointSet> Superclass;

  MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking();
  virtual ~MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking();

private:

};

#include "MultiResolutionDeformableAndAffineRegistrationMonitorWithTargetTracking.txx"

#endif
