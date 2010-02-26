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

#ifndef __DeformableRegistrationMonitor_h
#define __DeformableRegistrationMonitor_h

#include "RegistrationMonitor.h"
#include "itkProcessObject.h"

/** \class DeformableRegistrationMonitor 
 *  This class provides a VTK visualization pipeline configured for monitoring
 *  the progress of a deformable registration process. 
 */
template <class TPointSet>
class DeformableRegistrationMonitor : public RegistrationMonitor
{
public:
  
  typedef DeformableRegistrationMonitor  Self;
  typedef RegistrationMonitor        Superclass;

  typedef itk::ProcessObject         ProcessObjectType;

  typedef TPointSet                  PointSetType;

  DeformableRegistrationMonitor();
  virtual ~DeformableRegistrationMonitor();

  void Observe( 
    ProcessObjectType * filter, 
    const PointSetType * destinationPointSet );

protected:

  // These methods will only be called by the Observer
  virtual void Update();
  virtual void StartVisualization();

private:
  
  virtual void RemovePreviousObservers();

  typedef itk::SimpleMemberCommand< Self >  ObserverType;
  typedef std::vector< unsigned long >      ObserverTagsArrayType;

  typename ObserverType::Pointer          IterationObserver;
  typename ObserverType::Pointer          StartObserver;

  ProcessObjectType::Pointer              ObservedFilter;
  typename PointSetType::ConstPointer     ObservedPointSet;

  ObserverTagsArrayType                   ObserverTags;
};

#include "DeformableRegistrationMonitor.txx"

#endif
