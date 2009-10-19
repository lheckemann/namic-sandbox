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

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vtkMatrix4x4.h"

//------------------------------------------------------------------------

vtkStandardNewMacro(vtkPivotCalibration);
vtkCxxRevisionMacro(vtkPivotCalibration, "$Revision: 1.0 $");
const double vtkPivotCalibration::DEFAULT_SINGULAR_VALUE_THRESHOLD = 1e-1;


//-------------------------------------------------------------------------

vtkPivotCalibration::vtkPivotCalibration()
{
  //initialize the error flags and final calibration transform
  this->bInitializeError = true;
  this->bComputationError = true;
  this->CalibrationTransform = vtkMatrix4x4::New();
  this->m_SingularValueThreshold = DEFAULT_SINGULAR_VALUE_THRESHOLD;
}

//------------------------------------------------------------------------
vtkPivotCalibration::~vtkPivotCalibration()
{
  //Destroy contents of vector
  this->m_Transforms.clear();
}

//------------------------------------------------------------------------
void vtkPivotCalibration::Initialize( unsigned int n,vtkMRMLNode* node )
{
  //Initialize the transform data node and number of transforms
  if (node != NULL)
    {
    this->transformNode = static_cast<vtkMRMLLinearTransformNode*> (node);
    this->m_RequiredNumberOfTransformations = n;
    this->m_Transforms.clear();
    bInitializeError = false;
    std::cerr << "Calibration successfully initialized." << std::endl;
    }
  else
    {
    std::cerr << "Tool transform node does not exist" << std::endl;
    bInitializeError = true;
    }
}

//--------------------------------------------------------------------------
int vtkPivotCalibration::AcquireTransform()
{
  if (!bInitializeError)
    {
        //Check to see if the calibration vector is already full
    if( (this->m_Transforms.size() == this->m_RequiredNumberOfTransformations) && (this->bComputationError) )
      {
      //got all the transformations we need for calibration
      std::cerr << "Size of transforms vector: " << this->m_Transforms.size() << std::endl;
      std::cerr << "Finished filling the vector" << std::endl;
      this->transformNode = NULL;

      //compute calibration
      this->ResetCalibration();
      this->ComputeCalibration();

      //Delete the matrices allocated in memory in the vector
      std::vector<vtkMatrix4x4*>::const_iterator it;
      for( it = this->m_Transforms.begin(); it != this->m_Transforms.end(); it++)
        {
        (*it)->Delete();
        }
        this->m_Transforms.clear();

      //check if the calibration computation failed
      if( this->bComputationError == false )
        return 1;
      else
        return 0;
      }
    else  //transform was updated, we need to retrieve it
      {
      vtkMatrix4x4* mat = vtkMatrix4x4::New();
      this->transformNode->GetMatrixTransformToWorld(mat);
      this->m_Transforms.push_back(mat);
      std::cerr << "Number of transforms stacked: " << this->m_Transforms.size() << std::endl;
      return 0;
      }
    }
  else
    {
    std::cerr << "The calibration did not initialize correctly" << std::endl;
    return 0;
    }
}

//---------------------------------------------------------------------
void vtkPivotCalibration::ResetCalibration()
{
  //Reset all calibration variables
  this->CalibrationTransform->Zero();
  this->m_RMSE = 0;
  this->m_Translation[0] = 0; this->m_Translation[1] = 0; this->m_Translation[2] = 0;
  this->m_PivotPoint[0] = 0; this->m_PivotPoint[1] = 0; this->m_PivotPoint[2] = 0;
  this->bComputationError = true;
  std::cerr << "Calibration Reset successfully" << std::endl;
}

//------------------------------------------------------------------------
void vtkPivotCalibration::ComputeCalibration()
{
  if(this->m_Transforms.empty())
    {
    bComputationError = true;
    return;
    }

  std::cerr << "Starting computation" << std::endl;
  unsigned int rows = 3*this->m_Transforms.size();
  unsigned int columns = 4;

  vnl_matrix< double > A( rows, columns ), minusI( 3, 3, 0 ), R(3,1); //minusI is initialized with ones
  vnl_vector< double > b(rows), x(columns), t(3), r(3);
  minusI( 0, 0 ) = -1;
  minusI( 1, 1 ) = -1;
  minusI( 2, 2 ) = -1;

  //do the computation and set the internal variables
  std::vector<vtkMatrix4x4*>::const_iterator it, transformsEnd = this->m_Transforms.end();
  unsigned int currentRow;
  for( currentRow = 0, it = this->m_Transforms.begin();
       it != transformsEnd;
       it++, currentRow += 3 )
    {
    //Build the vector of independent coefficients
    for (int i = 0; i < 3; i++)
      {
      t(i) = (*it)->GetElement(i, 3);
      std::cerr << (*it)->GetElement(i, 3) << std::endl;
      }
    t *= -1;
    b.update( t, currentRow );
    //Build the matrix with the equation coefficients
    for (int i = 0; i < 3; i++)
      {
      //for (int j = 0; j < 3; j++)
      R(i, 0) = (*it)->GetElement(i, 2);
      }
    A.update(R, currentRow, 0);
    A.update( minusI, currentRow, 1 );
    }

  std::cerr << "Matrix A:" << A << std::endl;
  std::cerr << "Vector b:" << b << std::endl;

  //solve the equations
  vnl_svd<double> svdA(A);

  svdA.zero_out_absolute( this->m_SingularValueThreshold );

  //there is a solution only if rank(A)=4 (columns are linearly
  //independent)
  if( svdA.rank() < 4 )
    {
    bComputationError = true;
    }
  else
    {
    x = svdA.solve( b );

    //set the RMSE
    this->m_RMSE = ( A * x - b ).rms();

    //set the transformation
    this->CalibrationTransform->Identity();
    this->m_Translation[0] = 0.0;
    this->m_Translation[1] = 0.0;
    this->m_Translation[2] = x[0];
    this->CalibrationTransform->SetElement(2,3,m_Translation[2]);

    //set the pivot point
    this->m_PivotPoint[0] = x[1];
    this->m_PivotPoint[1] = x[2];
    this->m_PivotPoint[2] = x[3];

    //Print out solution to screen
    std::cerr << "Pivot point location: " << this->m_PivotPoint << std::endl;
    std::cerr << "Offset from tip to sensor: " << x[0] << std::endl;
    std::cerr << "Root mean squared error: " << this->m_RMSE << std::endl;

    bComputationError = false;
    }
}



//--------------------------------------------------------------------------------
void vtkPivotCalibration::SetSingularValueThreshold( double threshold )
{
  this->m_SingularValueThreshold = threshold;
}

//--------------------------------------------------------------------------------


/*void
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
