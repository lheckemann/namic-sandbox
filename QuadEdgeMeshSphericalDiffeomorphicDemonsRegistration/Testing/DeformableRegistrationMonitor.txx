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
::Observe( ProcessObjectType * filter, const PointSetType * destinationPointSet )
{
  this->ObservedPointSet = destinationPointSet;

  if( this->ObservedFilter != filter )
    {
    this->RemovePreviousObservers();

    this->ObservedFilter = filter;

    unsigned long observerTag =
      this->ObservedFilter->AddObserver( 
        itk::StartEvent(), this->StartObserver     );
    
    this->ObserverTags.push_back( observerTag );

    observerTag = this->ObservedFilter->AddObserver( 
      itk::ProgressEvent(), this->IterationObserver );

    this->ObserverTags.push_back( observerTag );

    observerTag = this->ObservedFilter->AddObserver( 
      itk::IterationEvent(), this->IterationObserver );

    this->ObserverTags.push_back( observerTag );
    }
}


/** Set the objects to be observed: optimizer and transform */
template <class TPointSet>
void DeformableRegistrationMonitor<TPointSet>
::RemovePreviousObservers()
{
  if( this->ObservedFilter.IsNotNull() )
    {
    ObserverTagsArrayType::const_iterator observerTagItr = this->ObserverTags.begin();
    ObserverTagsArrayType::const_iterator observerTagEnd = this->ObserverTags.end();

    while( observerTagItr != observerTagEnd )
      {
      this->ObservedFilter->RemoveObserver( *observerTagItr );
      ++observerTagItr;
      }
    }
}


/** Callback for the StartEvent */
template <class TPointSet>
void DeformableRegistrationMonitor<TPointSet>
::StartVisualization()
{
  std::cout << "Starting Deformable Visualization" << std::endl;
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
