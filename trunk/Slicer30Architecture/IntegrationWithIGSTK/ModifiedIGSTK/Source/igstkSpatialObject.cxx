/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkSpatialObject.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/19 23:02:01 $
  Version:   $Revision: 1.17 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkSpatialObject.h"
#include "igstkEvents.h"

namespace igstk
{ 


/** Constructor */
SpatialObject::SpatialObject():m_StateMachine(this)
{
  m_SpatialObject = NULL;
  m_TrackerTool = NULL;

  igstkAddInputMacro( SpatialObjectValid );
  igstkAddInputMacro( SpatialObjectNull );
  igstkAddInputMacro( ObjectValid );
  igstkAddInputMacro( ObjectNull );
  igstkAddInputMacro( TrackingEnabled );
  igstkAddInputMacro( TrackingLost );
  igstkAddInputMacro( TrackingRestored );
  igstkAddInputMacro( TrackingDisabled );
  igstkAddInputMacro( ManualTransform );
  igstkAddInputMacro( TrackerTransform );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( NonTracked  );
  igstkAddStateMacro( Tracked     );
  igstkAddStateMacro( TrackedLost );
  
  igstkAddTransitionMacro( Initial, ObjectValid, Initial,  AddObject );
  igstkAddTransitionMacro( Initial, ObjectValid, NonTracked,  AddObject );
  igstkAddTransitionMacro( NonTracked, ObjectValid, NonTracked,  AddObject );
  igstkAddTransitionMacro( Initial, ObjectNull, Initial,  ReportInvalidRequest );

  igstkAddTransitionMacro( Initial, SpatialObjectValid, NonTracked,  SetSpatialObject );
  igstkAddTransitionMacro( Initial, SpatialObjectNull, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackingEnabled, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackingLost, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackingRestored, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackingDisabled, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, ManualTransform, Initial,  ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, TrackerTransform, Initial,  ReportInvalidRequest );

  igstkAddTransitionMacro( NonTracked, SpatialObjectValid, NonTracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTracked, SpatialObjectNull, NonTracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTracked, TrackingEnabled, Tracked,  AttachToTrackerTool );
  igstkAddTransitionMacro( NonTracked, TrackingLost, NonTracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTracked, TrackingRestored, NonTracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( NonTracked, TrackingDisabled, NonTracked,  ReportTrackingDisabled );
  igstkAddTransitionMacro( NonTracked, ManualTransform, NonTracked,  SetTransform );
  igstkAddTransitionMacro( NonTracked, TrackerTransform, NonTracked,  ReportInvalidRequest );

  igstkAddTransitionMacro( Tracked, SpatialObjectValid, Tracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( Tracked, SpatialObjectNull, Tracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( Tracked, TrackingEnabled, Tracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( Tracked, TrackingLost, TrackedLost,  ReportTrackingLost );
  igstkAddTransitionMacro( Tracked, TrackingRestored, Tracked,  ReportInvalidRequest );
  igstkAddTransitionMacro( Tracked, TrackingDisabled, NonTracked, ReportTrackingDisabled  );
  igstkAddTransitionMacro( Tracked, ManualTransform, Tracked, ReportInvalidRequest  );
  igstkAddTransitionMacro( Tracked, TrackerTransform, Tracked, SetTransform  );

  igstkAddTransitionMacro( TrackedLost, SpatialObjectValid, TrackedLost,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLost, SpatialObjectNull, TrackedLost,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLost, TrackingEnabled, TrackedLost,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLost, TrackingLost, TrackedLost,  No );
  igstkAddTransitionMacro( TrackedLost, TrackingRestored, Tracked,  ReportTrackingRestored );
  igstkAddTransitionMacro( TrackedLost, TrackingDisabled, NonTracked,  ReportTrackingDisabled );
  igstkAddTransitionMacro( TrackedLost, ManualTransform, TrackedLost,  ReportInvalidRequest );
  igstkAddTransitionMacro( TrackedLost, TrackerTransform, TrackedLost,  ReportInvalidRequest );

  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();


  m_TrackerToolObserver = CommandType::New();
  m_TrackerToolObserver->SetCallbackFunction( this, &SpatialObject::TransformUpdateFromTrackerTool );
} 

/** Destructor */
SpatialObject::~SpatialObject()  
{
}


/** Print Self function */
void SpatialObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_SpatialObject )
    {
    os << indent << this->m_SpatialObject << std::endl;
    }
  os << indent << this->m_Transform << std::endl;
  if( this->m_TrackerTool )
    {
    os << indent << m_TrackerTool << std::endl;
    }

}

/** Request setting the ITK spatial object that provide internal functionalities. */
void SpatialObject::RequestSetSpatialObject( SpatialObjectType * spatialObject )
{  
  m_SpatialObjectToBeSet = spatialObject;

  if( m_SpatialObjectToBeSet.IsNull() )
    {
    m_StateMachine.PushInput( m_SpatialObjectNullInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_StateMachine.PushInput( m_SpatialObjectValidInput );
    m_StateMachine.ProcessInputs();
    }
}
  
 
/** Null operation for a State Machine transition */
void SpatialObject::NoProcessing()
{
}


/** Set the ITK spatial object that provide internal functionalities. This
 * method should only be called from the StateMachine */
void SpatialObject::SetSpatialObjectProcessing()
{
  m_SpatialObject = m_SpatialObjectToBeSet;
}

/** Request setting the ITK spatial object that provide internal functionalities. */
void SpatialObject::RequestAddObject(Self * object )
{
  m_ObjectToBeAdded = object;

  if( m_ObjectToBeAdded.IsNull() )
    {
    m_StateMachine.PushInput( m_ObjectNullInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_StateMachine.PushInput( m_ObjectValidInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Add an Object */
void SpatialObject::AddObjectProcessing()
{
  m_SpatialObject->AddSpatialObject(m_ObjectToBeAdded->GetSpatialObject());
  m_InternalObjectList.push_back(m_ObjectToBeAdded);
}

/** Return a child object given the id */
const SpatialObject::Self * SpatialObject::GetObject(unsigned long id) const
{
  if(id > m_InternalObjectList.size())
    {
    return NULL;
    }

  return m_InternalObjectList[id];
}


/** Return the internal pointer to the SpatialObject */
SpatialObject::SpatialObjectType * SpatialObject::GetSpatialObject()
{
  return m_SpatialObject;
}

/** Set the full Transform by a direct call. The state machine will take care
 * of only accepting this transform if the object is not currently being
 * tracked by a TrackerTool. */
void SpatialObject::RequestSetTransform(const Transform & transform )
{
  m_TransformToBeSet = transform;
  m_StateMachine.PushInput( m_ManualTransformInput );
  m_StateMachine.ProcessInputs();
}

/** Set the full Transform from a tracker. This call should only be invoked
 *  from the Callback of the observer that is listening to events from a 
 *  tracker.*/
void SpatialObject::RequestSetTrackedTransform(const Transform & transform )
{
  m_TransformToBeSet = transform;
  m_StateMachine.PushInput( m_TrackerTransformInput );
  m_StateMachine.ProcessInputs();
}


/** Set the full Transform. Only to be called by the State Machine. */
void SpatialObject::SetTransformProcessing()
{
  m_Transform = m_TransformToBeSet;
  if( m_SpatialObject.IsNotNull() )
    {
    Transform::VectorType translation = m_Transform.GetTranslation();
    m_SpatialObject->GetObjectToWorldTransform()->SetOffset( translation );
    Transform::VersorType rotation = m_Transform.GetRotation();
    Transform::VersorType::MatrixType matrix = rotation.GetMatrix();
    m_SpatialObject->GetObjectToWorldTransform()->SetMatrix( matrix );
    igstkLogMacro( DEBUG, " SpatialObject::SetTransform() T: " 
        << translation << " R: " << rotation << "\n" );
    }
}


/** Get Transform */
const Transform &
SpatialObject::GetTransform()  const
{
  return m_Transform;
}

/** Request the protocol for attaching to a tracker tool. This is a one-time
 * operation. Once a Spatial Object is attached to a tracker tool it is not
 * expected to get back to manual nor to be re-attached to a second tracker
 * tool. */
void SpatialObject::RequestAttachToTrackerTool(const TrackerTool * tool )
{
  m_TrackerToolToAttachTo = tool;
  m_StateMachine.PushInput( m_TrackingEnabledInput );
  m_StateMachine.ProcessInputs();
}

/** Make necessary connections to the TrackerTool */
void
SpatialObject::AttachToTrackerToolProcessing()
{
   m_TrackerTool = m_TrackerToolToAttachTo;
   m_TrackerTool->AddObserver( TransformModifiedEvent(), m_TrackerToolObserver ); 
}

/** Report that tracking is now enabled */
void
SpatialObject::ReportTrackingDisabledProcessing()
{
}

/** Report that tracking has been lost */
void
SpatialObject::ReportTrackingLostProcessing()
{
}

/** Report that tracking has been restored */
void
SpatialObject::ReportTrackingRestoredProcessing()
{
}

/** Report that an invalid or suspicious operation has been requested. This may
 * mean that an error condition has arised in one of the componenta that
 * interact with this SpatialObject. */
void
SpatialObject::ReportInvalidRequestProcessing()
{
}

/** Receive the Events from the Tracker Tool and use them for updating the
 * transform of this Spatial Object */
void
SpatialObject::TransformUpdateFromTrackerTool( const ::itk::EventObject & event  )
{
  const TransformModifiedEvent * transformEvent = 
    dynamic_cast< const TransformModifiedEvent * >( &event );

  if( transformEvent )
    {
    this->RequestSetTrackedTransform( transformEvent->Get() );
    }
}

} // end namespace igstk

