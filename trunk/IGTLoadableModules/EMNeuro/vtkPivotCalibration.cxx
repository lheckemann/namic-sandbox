/*=========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================*/

#include "vtkPivotCalibration.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPivotCalibration);
vtkCxxRevisionMacro(vtkPivotCalibration, "$Revision: 1.0 $");

//-------------------------------------------------------------------------

vtkPivotCalibration::vtkPivotCalibration()
{
  //instantiate the class that performs the pivot calibration computation
  //this->m_PivotCalibrationAlgorithm = PivotCalibrationAlgorithm::New();
  std::cerr << "Got to the constructor" << std::endl;
  //this->bInitializeError = true;
  //this->bComputationError = true;
}

//------------------------------------------------------------------------
vtkPivotCalibration::~vtkPivotCalibration()
{

}

//------------------------------------------------------------------------
/*void
vtkPivotCalibration::Initialize( unsigned int n,
                                    vtkMRMLNode* node )
{
  //Initialize the transform data node and number of transforms
  if (node != NULL)
    {
    this->transformNode = node;
    this->m_RequiredNumberOfTransformations = n;
    this->m_Transforms.clear();
    //this->m_PivotCalibrationAlgorithm->RequestResetCalibration();
    bInitializeError = false;
    std::cerr << "Calibration successfully initialized." << std::endl;
    }
  else
    {
    std::cerr << "Tool transform node does not exist" << std::endl;
    bInitializeError = true;
    }
}*/

//--------------------------------------------------------------------------
/*void
vtkPivotCalibration::AcquireTransform()
{
//Make sure initialized is true
  if( this->m_Transforms.size() == this->m_RequiredNumberOfTransformations )
    {
    //got all the transformations we need for calibration
    // Instead of removing the observer, we set the callback function to empty
    // because that the tracker is running on a separate thread, when the
    // tracker update event evoke the observer callback, it will crash the
    // application if the observer is being removed by another thread.Thus it is
    // safer to  set the observer callback to an empty function
    //this->m_TrackerTool->RemoveObserver( this->m_AcquireTransformObserverID );
    /*this->m_TransformAcquiredObserver->SetCallbackFunction(this,
                                             & PivotCalibration::EmptyCallBack);
    //this->m_TrackerTool->RemoveObserver( this->m_TransformToTrackerObserverID );

    this->InvokeEvent( DataAcquisitionEndEvent() );
    //actually perform the calibration
    this->m_PivotCalibrationAlgorithm->RequestResetCalibration();
    this->m_PivotCalibrationAlgorithm->RequestAddTransforms(this->m_Transforms);
    this->m_PivotCalibrationAlgorithm->RequestComputeCalibration();
    //check if the calibration computation failed
    if( this->m_ErrorObserver->ErrorOccured() )
      {
      this->m_ErrorObserver->GetErrorMessage(
                                          this->m_ReasonForCalibrationFailure );
      this->m_ErrorObserver->ClearError();
      igstkPushInputMacro( Failed );
      }
    else
      {
      igstkPushInputMacro( Succeeded );
      }
    this->m_StateMachine.ProcessInputs();
    }
  else  //transform was updated, we need to retrieve it
    {
    //this->m_Transforms.push_back(this->transformNode->GetTransformToParent());
    }
}

//--------------------------------------------------------------------------------

void
vtkPivotCalibration::RequestComputeCalibration()
{
  //Start the computation of the calibration
  //Make sure error flag is false
  this->m_Transforms.clear();
  //this->m_ReasonForCalibrationFailure.clear();
}

void
vtkPivotCalibration::RequestCalibrationTransform()
{
  //Output transform
  //Make sure computation error is off
}

void
vtkPivotCalibration::RequestPivotPoint()
{
  //Output pivot point
  //Make sure computation error is off
}

void
vtkPivotCalibration::RequestCalibrationRMSE()
{
  //Output RMS error
  //Make sure computation error is off
}

/*void
PivotCalibration::ComputeCalibrationProcessing()
{
  this->m_Transforms.clear();
  this->m_ReasonForCalibrationFailure.clear();
  this->InvokeEvent( DataAcquisitionStartEvent() );
  this->m_TrackerTool->AddObserver( igstk::TrackerToolTransformUpdateEvent(),
                                            this->m_TransformAcquiredObserver );

  this->m_TransformAcquiredObserver->SetCallbackFunction( this,
                             &PivotCalibration::AcquireTransformsAndCalibrate );

  this->m_TransformToTrackerObserverID =
      this->m_TrackerTool->AddObserver( CoordinateSystemTransformToEvent() ,
                                                    this->m_TransformObserver );

}


void
PivotCalibration::AcquireTransformsAndCalibrate(itk::Object *caller,
                                                const itk::EventObject & event)
{
  //got all the transformations we need for calibration
  if( this->m_Transforms.size() == this->m_RequiredNumberOfTransformations )
    {
    // Instead of removing the observer, we set the callback function to empty
    // because that the tracker is running on a separate thread, when the
    // tracker update event evoke the observer callback, it will crash the
    // application if the observer is being removed by another thread.Thus it is
    // safer to  set the observer callback to an empty function
    //this->m_TrackerTool->RemoveObserver( this->m_AcquireTransformObserverID );
    this->m_TransformAcquiredObserver->SetCallbackFunction(this,
                                             & PivotCalibration::EmptyCallBack);
    this->m_TrackerTool->RemoveObserver( this->m_TransformToTrackerObserverID );



    this->InvokeEvent( DataAcquisitionEndEvent() );
    //actually perform the calibration
    this->m_PivotCalibrationAlgorithm->RequestResetCalibration();
    this->m_PivotCalibrationAlgorithm->RequestAddTransforms(this->m_Transforms);
    this->m_PivotCalibrationAlgorithm->RequestComputeCalibration();
    //check if the calibration computation failed
    if( this->m_ErrorObserver->ErrorOccured() )
      {
      this->m_ErrorObserver->GetErrorMessage(
                                          this->m_ReasonForCalibrationFailure );
      this->m_ErrorObserver->ClearError();
      igstkPushInputMacro( Failed );
      }
    else
      {
      igstkPushInputMacro( Succeeded );
      }
    this->m_StateMachine.ProcessInputs();
    }
  else  //transform was updated, we need to retrieve it
    {
    this->m_TrackerTool->RequestGetTransformToParent();
    if( this->m_TransformObserver->GotTransformToTracker() )
      {
      this->m_Transforms.push_back(
        (this->m_TransformObserver->GetTransformToTracker()).GetTransform() );
      DataAcquisitionEvent evt;
      evt.Set( (double)this->m_Transforms.size()/
                (double)(this->m_RequiredNumberOfTransformations) );
      this->InvokeEvent( evt );
      }
    }
}

void
PivotCalibration::ReportCalibrationComputationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "ReportCalibrationComputationSuccessProcessing called...\n");
  this->InvokeEvent( CalibrationSuccessEvent() );
}

void
PivotCalibration::ReportCalibrationComputationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "ReportCalibrationComputationFailureProcessing called...\n");
  CalibrationFailureEvent evt;
  evt.Set( this->m_ReasonForCalibrationFailure );
  this->InvokeEvent( evt );
}

void
PivotCalibration::GetTransformProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "GetTransformProcessing called...\n");
  this->m_PivotCalibrationAlgorithm->RequestCalibrationTransform();
  if( this->m_GetCalibrationTransformObserver->GotCalibrationTransform() )
    {
    CoordinateSystemTransformToEvent  event;
    event.Set(
      this->m_GetCalibrationTransformObserver->GetCalibrationTransform() );
    this->InvokeEvent(  event );
    }
}

void
PivotCalibration::GetPivotPointProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "GetPivotPointProcessing called...\n");
  //the events generated by the
  this->m_PivotCalibrationAlgorithm->RequestPivotPoint();
  if( this->m_GetPivotPointObserver->GotPivotPoint() )
    {
    PointEvent evt;
    evt.Set( this->m_GetPivotPointObserver->GetPivotPoint() );
    this->InvokeEvent( evt );
    }
}

void
PivotCalibration::GetRMSEProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "GetTransformRMSEProcessing called...\n");
  //the events generated by the
  this->m_PivotCalibrationAlgorithm->RequestCalibrationRMSE();
  if( this->m_GetCalibrationRMSEObserver->GotCalibrationRMSE() )
    {
    DoubleTypeEvent evt;
    evt.Set( this->m_GetCalibrationRMSEObserver->GetCalibrationRMSE() );
    this->InvokeEvent( evt );
    }
}

void
PivotCalibration::PrintSelf( std::ostream& os,
                                itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Tool : " << std::endl;
  os << indent << this->m_TrackerTool << std::endl;
  os << indent << "Required number of transformations: " << std::endl;
  os << indent << m_RequiredNumberOfTransformations << std::endl;
}

PivotCalibration::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
{
                           //calibration errors
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>(
   (igstk::PivotCalibrationAlgorithm::CalibrationFailureEvent()).GetEventName(),
                         "Pivot Calibration Algorithm: computation failed." ) );
}

void
PivotCalibration::ErrorObserver::Execute(const itk::Object *caller,
                          const itk::EventObject & event) throw (std::exception)
{
  std::map<std::string,std::string>::iterator it;
  std::string className = event.GetEventName();
  it = this->m_ErrorEvent2ErrorMessage.find(className);

  this->m_ErrorOccured = true;
  this->m_ErrorMessage = (*it).second;
}

void
PivotCalibration::ErrorObserver::Execute(itk::Object *caller,
                          const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}*/
