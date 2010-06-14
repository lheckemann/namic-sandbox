#include "vtkPerkStationInsertStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationSecondaryMonitor.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWApplication.h"
#include "vtkKWCheckButton.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWLoadSaveButton.h"

#if defined(USE_NDIOAPI)
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkNDITracker.h"
#endif

#include <stdio.h>


  // Definition of plan list columns.
enum
  {
  COL_NAME = 0,
  COL_ER,
  COL_EA,
  COL_ES,
  COL_TR,
  COL_TA,
  COL_TS,
  COL_COUNT
  };
static const char* COL_LABELS[ COL_COUNT ] = { "Name", "ER", "EA", "ES", "TR", "TA", "TS" };
static const int COL_WIDTHS[ COL_COUNT ] = { 10, 5, 5, 5, 5, 5, 5 };


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationInsertStep);
vtkCxxRevisionMacro(vtkPerkStationInsertStep, "$Revision: 1.1 $");


//----------------------------------------------------------------------------
vtkPerkStationInsertStep::vtkPerkStationInsertStep()
{
  this->SetName("3/5. Insert");
  this->SetDescription("Do the needle insertion");
  
  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationInsertStep::WizardGUICallback);
  
  
  this->PlanList = NULL;
  this->PlanListFrame = NULL;
  
  
  this->ProcessingCallback = false; 
  this->TimerProcessing = false;
  
  
  this->TrackerTimerId = NULL;
}


//----------------------------------------------------------------------------
vtkPerkStationInsertStep::~vtkPerkStationInsertStep()
{
}


//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  // in clinical mode
  this->SetName("3/4. Insert");
  this->GetGUI()->GetWizardWidget()->Update();
  
  this->SetDescription("Select plan and insert needle.");
  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  
  
    // Plan list.
  
  
    // Create a frame for the plan list.
  
  if ( ! this->PlanListFrame )
    {
    this->PlanListFrame = vtkKWFrame::New();
    }
  if ( ! this->PlanListFrame->IsCreated() )
    {
    this->PlanListFrame->SetParent( parent );
    this->PlanListFrame->Create();
    }
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanListFrame->GetWidgetName() );
  
  
    // Create the plan list.
  
  if ( ! this->PlanList )
    {
    this->PlanList = vtkKWMultiColumnListWithScrollbars::New();
    this->PlanList->SetParent( this->PlanListFrame );
    this->PlanList->Create();
    this->PlanList->SetHeight( 1 );
    this->PlanList->GetWidget()->SetSelectionTypeToRow();
    this->PlanList->GetWidget()->SetSelectionBackgroundColor( 1, 0, 0 );
    this->PlanList->GetWidget()->MovableRowsOff();
    this->PlanList->GetWidget()->MovableColumnsOff();
    
      // Create the columns.
    
    for ( int col = 0; col < COL_COUNT; ++ col )
      {
      this->PlanList->GetWidget()->AddColumn( COL_LABELS[ col ] );
      this->PlanList->GetWidget()->SetColumnWidth( col, COL_WIDTHS[ col ] );
      this->PlanList->GetWidget()->SetColumnAlignmentToLeft( col );
      }
    }
  
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanList->GetWidgetName());
  
  
    // TO DO: install callbacks
  this->InstallCallbacks();

  this->AddGUIObservers();

  // TO DO: populate controls wherever needed
  this->PopulateControls();
  this->UpdateGUI();

}

//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


void
vtkPerkStationInsertStep
::OnMultiColumnListSelectionChanged()
{
  int numRows = this->PlanList->GetWidget()->GetNumberOfSelectedRows();
  
  if ( numRows != 1 ) return;
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
  int rowIndex = this->PlanList->GetWidget()->GetIndexOfFirstSelectedRow();
  vtkPerkStationPlan* plan = moduleNode->GetPlanAtIndex( rowIndex );
  
  moduleNode->SetCurrentPlanIndex( rowIndex );
  
  
  moduleNode->GetPlanMRMLFiducialListNode()->RemoveAllFiducials();
  
  double point[ 3 ];
  
  plan->GetEntryPointRAS( point );
  int ind = moduleNode->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ( point[ 0 ], point[ 1 ], point[ 2 ], 0 );
  moduleNode->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText( ind, "Entry" );
  moduleNode->SetPlanEntryPoint( point );
  
  plan->GetTargetPointRAS( point );
  ind = moduleNode->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ( point[ 0 ], point[ 1 ], point[ 2 ], 0 );
  moduleNode->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText( ind, "Target" );
  moduleNode->SetPlanTargetPoint( point );
  
  moduleNode->SetCurrentSliceOffset( point[ 2 ] );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic()->SetSliceOffset(
    moduleNode->GetCurrentSliceOffset() );
  
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
}


//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::InstallCallbacks()
{
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
    vtkPerkStationInsertStep *self = reinterpret_cast<vtkPerkStationInsertStep *>(clientData);
    if (self) { self->ProcessGUIEvents(caller, event, callData); }
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Planning")!=0)
      return;

  
  if (this->ProcessingCallback)
    {
    return;
    }

  this->ProcessingCallback = true;

  this->ProcessingCallback = false;
}


//-------------------------------------------------------------------------------------------
void vtkPerkStationInsertStep::ConnectTrackerCallback(bool value)
{
  #if defined(USE_NDIOAPI)
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  if (value)
    {
    this->TrackerStatusMsg->SetText("Connecting to tracker...");
    if(this->Tracker->Probe() != 1)
      {
      this->TrackerStatusMsg->SetText("Could not connect to tracker");
      return;
      } 

    this->TrackerStatusMsg->SetText("Connected to tracker");
    this->Tracker->SetReferenceTool(mrmlNode->GetReferenceBodyToolPort());
    this->Tracker->StartTracking();
    this->TrackerStatusMsg->SetText("Tracking...");
    // calculate trackerToRAS matrix from the two registration matrices
    // and apply it to tracker class as a calibration matrix
    // the resulting reading from the tracker would then be directly in RAS coordinates
    vtkMatrix4x4 *trackerToImageRASMatrix = vtkMatrix4x4::New();
    //update trackerToRAS
    vtkMatrix4x4::Multiply4x4(mrmlNode->GetPhantomToImageRASMatrix(), mrmlNode->GetTrackerToPhantomMatrix(), trackerToImageRASMatrix);
    this->Tracker->SetWorldCalibrationMatrix(trackerToImageRASMatrix);

    // apply tool tip offset which is retrieved from GUI
    vtkTrackerTool *tool = this->Tracker->GetTool(mrmlNode->GetNeedleToolPort());
    vtkMatrix4x4 *toolCalibrationMatrix = tool->GetBuffer()->GetToolCalibrationMatrix();
    for(unsigned int i=0;i<3;i++)
      toolCalibrationMatrix->SetElement(i, 3, this->ToolTipOffset->GetWidget(i)->GetValueAsDouble());

    // disable tool tip offset entry widget
    for(unsigned int i=0;i<3;i++)
      this->ToolTipOffset->GetWidget(i)->SetEnabled(0);

    // start the callback timer
    // inititate timer call
     const char *tmpId = vtkKWTkUtilities::CreateTimerHandler(this->GetApplication()->GetMainInterp(), 50, this, "TrackerTimerEvent");
     this->TrackerTimerId = new char[strlen(tmpId)+1];
     strcpy(this->TrackerTimerId, tmpId);
    }
  else
    {
    if (this->Tracker->IsTracking())
      { 
      this->Tracker->StopTracking();
      this->TrackerStatusMsg->SetText("Tracking stopped");
      this->GetGUI()->GetSecondaryMonitor()->RemoveOverlayRealTimeNeedleTip();

      // enable tool tip offset entry widget
      for(unsigned int i=0;i<3;i++)
        this->ToolTipOffset->GetWidget(i)->SetEnabled(1);
      }
      if (this->TrackerTimerId)
      {
      vtkKWTkUtilities::CancelTimerHandler(vtkKWApplication::GetMainInterp(), this->TrackerTimerId); 
      }
    }


  #endif(USE_NDIOAPI)
}


void
vtkPerkStationInsertStep
::UpdateGUI()
{
  vtkMRMLPerkStationModuleNode* mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if ( ! mrmlNode ) return;
  
  
    // Update plan list.
  
  if ( this->PlanList == NULL || this->PlanList->GetWidget() == NULL ) return;
  
  int numPlans = mrmlNode->GetNumberOfPlans();
  
  bool deleteFlag = true;
  if ( numPlans != this->PlanList->GetWidget()->GetNumberOfRows() )
    {
    this->PlanList->GetWidget()->DeleteAllRows();
    }
  else
    {
    deleteFlag = false;
    }
  
  
  const int PRECISION_DIGITS = 1;
  
  double planEntry[ 3 ];
  double planTarget[ 3 ];
  double validationEntry[ 3 ];
  double validationTarget[ 3 ];
  
  for ( int row = 0; row < numPlans; ++ row )
    {
    vtkPerkStationPlan* plan = mrmlNode->GetPlanAtIndex( row );
    
    if ( deleteFlag )
      {
      this->PlanList->GetWidget()->AddRow();
      }
    
    plan->GetEntryPointRAS( planEntry );
    plan->GetTargetPointRAS( planTarget );
    plan->GetValidationEntryPointRAS( validationEntry );
    plan->GetValidationTargetPointRAS( validationTarget );
    
    if ( planEntry == NULL || planTarget == NULL )
      {
      vtkErrorMacro( "ERROR: No plan points in plan" );
      }
    
    vtkKWMultiColumnList* colList = this->PlanList->GetWidget();
    if ( deleteFlag || plan->GetName().compare( this->PlanList->GetWidget()->GetCellText( row, COL_NAME ) ) != 0 )
      {
      this->PlanList->GetWidget()->SetCellText( row, COL_NAME, plan->GetName().c_str() );
      for ( int i = 0; i < 3; ++ i )
        {
        std::ostrstream os;
        os << std::setiosflags( ios::fixed | ios::showpoint ) << std::setprecision( PRECISION_DIGITS );
        os << planEntry[ i ] << std::ends;
        colList->SetCellText( row, COL_ER + i, os.str() );
        os.rdbuf()->freeze();
        }
      for ( int i = 0; i < 3; ++ i )
        {
        std::ostrstream os;
        os << std::setiosflags( ios::fixed | ios::showpoint ) << std::setprecision( PRECISION_DIGITS );
        os << planTarget[ i ] << std::ends;
        colList->SetCellText( row, COL_ER + 3 + i, os.str() );
        os.rdbuf()->freeze();
        }
      }
    }
}
  


//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::TrackerTimerEvent()
{
  #if defined(USE_NDIOAPI)

  if (this->TimerProcessing)
    return;

  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  
  // sort of locking mechanism
  this->TimerProcessing = true;

  // read the marix/transform from the tracker
  // update on gui text
  // update on gui, in 3D view the needle tip actor
  // update on gui, in 3D the needle line actor

  
  this->Tracker->Update();
  int numOfTools = this->Tracker->GetNumberOfTools();

  vtkTrackerTool *tool = this->Tracker->GetTool(mrmlNode->GetNeedleToolPort());
  

  if( !(tool->IsMissing() || tool->IsOutOfView() || tool->IsOutOfVolume()))
    {
    //vtkTransform *toolTransform = vtkTransform::New();
    //toolTransform = tool->GetTransform();
    //vtkMatrix4x4 *toolMatrix = vtkMatrix4x4::New();
    //tool->GetBuffer()->GetUncalibratedMatrix(toolMatrix,0);

    vtkMatrix4x4 *toolCalMatrix = vtkMatrix4x4::New();
    tool->GetBuffer()->GetMatrix(toolCalMatrix, 0);
    double *elements;
    double timestamp = tool->GetBuffer()->GetTimeStamp(0);

    double rasPt[4] = {toolCalMatrix->GetElement(0,3), toolCalMatrix->GetElement(1,3), toolCalMatrix->GetElement(2,3), 1};
    //double inPt[4] = {toolMatrix->GetElement(0,3), toolMatrix->GetElement(1,3), toolMatrix->GetElement(2,3), 1};
    //double pointInPhantomSpace[4];
    //mrmlNode->GetTrackerToPhantomMatrix()->MultiplyPoint(inPt, pointInPhantomSpace);
    //double pointInImageRAS[4];
    //mrmlNode->GetPhantomToImageRASMatrix()->MultiplyPoint(pointInPhantomSpace, pointInImageRAS);
     
    this->NeedleTipPosition->GetWidget(0)->SetValueAsDouble(rasPt[0]);
    this->NeedleTipPosition->GetWidget(1)->SetValueAsDouble(rasPt[1]);
    this->NeedleTipPosition->GetWidget(2)->SetValueAsDouble(rasPt[2]);

    // update visual overlay on secondary monitor
    this->GetGUI()->GetSecondaryMonitor()->OverlayRealTimeNeedleTip(rasPt, toolCalMatrix);

    if (this->LogToFile)
      {
      elements = *(toolCalMatrix->Element);   
      this->WriteTrackingRecordToFile(timestamp,elements);
      }

    }

  

  this->TimerProcessing = false;

  const char *tmpId = vtkKWTkUtilities::CreateTimerHandler(this->GetApplication()->GetMainInterp(), 50, this, "TrackerTimerEvent");
  this->TrackerTimerId = new char[strlen(tmpId)+1];
  strcpy(this->TrackerTimerId, tmpId);
 
  #endif(USE_NDIOAPI)
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::WriteTrackingRecordToFile(double timestamp, const double *matrix)
{
  #if defined(USE_NDIOAPI)

  fprintf(this->InsertionLogFile,"%14.3f ",timestamp);  
  fprintf(this->InsertionLogFile,"%8.2f %8.2f %8.2f ",
          matrix[4*0+3],matrix[4*1+3],matrix[4*2+3]);
  
  for (int i = 0; i < 3; i++)
    {
    fprintf(this->InsertionLogFile,"%15.13f %15.13f %15.13f",
            matrix[4*i],matrix[4*i+1],matrix[4*i+2]);
    if (i < 2)
      {
      fprintf(this->InsertionLogFile," ");
      }
    else
      {
      fprintf(this->InsertionLogFile,";\n");
      }
    }

  #endif(USE_NDIOAPI)
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::PopulateControls()
{

}

//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::ResetControls()
{
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::Reset()
{ 
  this->ResetControls();
}


//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::AddGUIObservers()
{
  this->RemoveGUIObservers();
  
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetSelectionChangedCommand(
      this, "OnMultiColumnListSelectionChanged" );
    }
  
}


//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::RemoveGUIObservers()
{
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetSelectionChangedCommand( this, "" );
    }
}


//---------------------------------------------------------------------------
void vtkPerkStationInsertStep::LogFileSaveButtonCallback()
{
#if defined(USE_NDIOAPI)
  this->InsertionLogFile = fopen(this->InsertionLogFileName.c_str(),"w");   
  if (this->InsertionLogFile == 0)
    {
    this->LogFileLoadMsg->SetText("Could not open file for writing...");
    return;
    }
  this->StartStopLoggingToFileCheckButton->SetEnabled(1);
#endif // USE_NDIOAPI
}
//---------------------------------------------------------------------------
void vtkPerkStationInsertStep::LogFileCheckButtonCallback(bool state)
{ 
  #if defined(USE_NDIOAPI)

  if (state)
    {    
    this->LogToFile = true;
    this->StartStopLoggingToFileCheckButton->SetText("Logging..press to stop");
    }
  else
    {
    this->LogToFile = false;
    this->StartStopLoggingToFileCheckButton->SetText("Start logging");
    if (this->InsertionLogFile)
      {
      fclose(this->InsertionLogFile);
      }    
    }

  #endif(USE_NDIOAPI)
}
//---------------------------------------------------------------------------
void vtkPerkStationInsertStep::Validate()
{
  this->Superclass::Validate();

   // reset the overlay needle guide both in sliceviewer and in secondary monitor
 // this->GetGUI()->GetSecondaryMonitor()->RemoveOverlayNeedleGuide();

  this->GetGUI()->GetSecondaryMonitor()->RemoveDepthPerceptionLines();

  this->GetGUI()->GetSecondaryMonitor()->RemoveOverlayRealTimeNeedleTip();

  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }
}
