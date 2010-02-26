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

#include "itkMacro.h"
#include "vtkPoints.h"

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
::Observe( const ProcessObjectType * filter, const PointSetType * destinationPointSet )
{
  this->ObservedFilter = filter;

  this->ObservedPointSet = destinationPointSet;

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

  typedef typename PointSetType::PointsContainer    PointsContainer;
  typedef typename PointsContainer::ConstIterator   PointsIterator;
  typedef typename PointSetType::PointType          PointType;

  const PointsContainer * displacedPoints = this->ObservedPointSet->GetPoints();

  if( !displacedPoints )
    {
    std::cerr << "displacedPoints = NULL" << std::endl;
    return;
    }

  vtkPoints * vtkpoints = this->GetFixedSurfacePoints();

  if( !vtkpoints )
    {
    std::cerr << "vtkpoints = NULL" << std::endl;
    return;
    }

  unsigned int numberOfPoints = 
    static_cast< unsigned int>( vtkpoints->GetNumberOfPoints() );

  if( numberOfPoints != displacedPoints->Size() )
    {
    std::cerr << "vtkPolyData numberOfPoints  = " << numberOfPoints << std::endl;
    std::cerr << "number of deformable points = " << displacedPoints->Size() << std::endl;
    itkGenericExceptionMacro("Fixed Mesh and Deformed Points have different size");
    } 

  PointsIterator pointItr = displacedPoints->Begin();
  PointsIterator pointEnd = displacedPoints->End();

  vtkFloatingPointType displacedPoint[3];

  int pointId = 0;

  while( pointItr != pointEnd )
    {
    const PointType & observedDisplacedPoint = pointItr.Value();

    displacedPoint[0] = observedDisplacedPoint[0];
    displacedPoint[1] = observedDisplacedPoint[1];
    displacedPoint[2] = observedDisplacedPoint[2];

    vtkpoints->SetPoint( pointId, displacedPoint );
    
    ++pointItr;
    ++pointId;
    }

  // FIXME : Do we need to disconnect the fixed mesh from its source ?
  // FIXME : Do we need to call Modified in the fixed mesh ?

  this->RefreshRendering();
}
