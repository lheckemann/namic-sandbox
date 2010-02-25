/*=========================================================================

  Program:   Surface Registration Program
  Module:    $RCSfile: DeformableRegistrationMonitor.cxx,v $

  Copyright (c) Kitware Inc. 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "DeformableRegistrationMonitor.h"

/** Constructor */
template <class TPointSet>
DeformableRegistrationMonitor<TPointSet>
::DeformableRegistrationMonitor()
{
  this->StartObserver       = ObserverType::New();
  this->IterationObserver   = ObserverType::New();

  this->StartObserver->SetCallbackFunction( 
    this, & Self::StartVisualization );

  this->IterationObserver->SetCallbackFunction( 
    this, & Self::Update );
}

/** Destructor */
template <class TPointSet>
DeformableRegistrationMonitor<TPointSet>
::~DeformableRegistrationMonitor()
{
}


/** Set the objects to be observed: optimizer and transform */
template <class TPointSet>
void DeformableRegistrationMonitor<TPointSet>
::Observe( const ProcessObjectType * filter, const PointSetType * transform )
{
  this->ObservedFilter = filter;

  this->ObservedFilter->AddObserver( 
    itk::StartEvent(), this->StartObserver     );

  this->ObservedFilter->AddObserver( 
    itk::ProgressEvent(), this->IterationObserver );

  this->ObservedFilter->AddObserver( 
    itk::IterationEvent(), this->IterationObserver );
}

/** Callback for the StartEvent */
template <class TPointSet>
void DeformableRegistrationMonitor<TPointSet>
::StartVisualization()
{
  this->Superclass::StartVisualization();
}


/** Update the Visualization */
template <class TPointSet>
void DeformableRegistrationMonitor<TPointSet>
::Update()
{
  this->Superclass::Update();

  // HERE COPY destination points into FIXED mesh !!

  this->RefreshRendering();
}
