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


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationInsertStep);
vtkCxxRevisionMacro(vtkPerkStationInsertStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkPerkStationInsertStep::vtkPerkStationInsertStep()
{
  this->SetName("3/5. Insert");
  this->SetDescription("Do the needle insertion");
  
  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationInsertStep::WizardGUICallback);

  this->LoadTrackerConfigFrame = NULL;
  this->LoadTrackerConfigFileButton = NULL;
  this->TrackerConfigFileLoadMsg = NULL;

  this->TrackerConnectionFrame = NULL;
  this->ConnectTrackerCheckButton = NULL;
  this->TrackerStatusMsg = NULL;

  this->NeedleToolFrame = NULL;
  this->NeedleTipPositionFrame = NULL;
  this->NeedleTipPositionLabel = NULL;
  this->NeedleTipPosition = NULL;

  this->ToolTipOffsetFrame = NULL;
  this->ToolTipOffsetLabel = NULL;
  this->ToolTipOffset = NULL;

  this->LoggingFrame = NULL;
  this->StartStopLoggingToFileCheckButton = NULL;
  this->LogFileLoadMsg = NULL;
  this->LogFileButton = NULL;

  
  this->ProcessingCallback = false; 
  this->TimerProcessing = false;
  this->LogToFile = false;

  #if defined(USE_NDIOAPI)
  this->Tracker = vtkNDITracker::New();
  this->InsertionLogFile = NULL;
  #endif


  this->TrackerTimerId = NULL;

}

//----------------------------------------------------------------------------
vtkPerkStationInsertStep::~vtkPerkStationInsertStep()
{
  if(this->LoadTrackerConfigFrame)
    {
    this->LoadTrackerConfigFrame->Delete();
    this->LoadTrackerConfigFrame = NULL;
    }
  if(this->LoadTrackerConfigFileButton)
    {
    this->LoadTrackerConfigFileButton->Delete();
    this->LoadTrackerConfigFileButton = NULL;
    }
  if(this->TrackerConfigFileLoadMsg)
    {
    this->TrackerConfigFileLoadMsg->Delete();
    this->TrackerConfigFileLoadMsg = NULL;
    }
  if(this->TrackerConnectionFrame)
    {
    this->TrackerConnectionFrame->Delete();
    this->TrackerConnectionFrame = NULL;
    }
  if(this->TrackerStatusMsg)
    {
    this->TrackerStatusMsg->Delete();
    this->TrackerStatusMsg = NULL;
    }
  if(this->ConnectTrackerCheckButton)
    {
    this->ConnectTrackerCheckButton->Delete();
    this->ConnectTrackerCheckButton = NULL;
    }
  if(this->NeedleToolFrame)
    {
    this->NeedleToolFrame->Delete();
    this->NeedleToolFrame = NULL;
    }
  if(this->NeedleTipPositionFrame)
    {
    this->NeedleTipPositionFrame->Delete();
    this->NeedleTipPositionFrame = NULL;
    }
  if(this->NeedleTipPositionLabel)
    {
    this->NeedleTipPositionLabel->Delete();
    this->NeedleTipPositionLabel = NULL;
    }  
  if(this->NeedleTipPosition)
    {
    this->NeedleTipPosition->DeleteAllWidgets();
    this->NeedleTipPosition = NULL;
    }
  if(this->ToolTipOffsetFrame)
    {
    this->ToolTipOffsetFrame->Delete();
    this->ToolTipOffsetFrame = NULL;
    }
  if(this->ToolTipOffsetLabel)
    {
    this->ToolTipOffsetLabel->Delete();
    this->ToolTipOffsetLabel = NULL;
    }  
  if(this->ToolTipOffset)
    {
    this->ToolTipOffset->DeleteAllWidgets();
    this->ToolTipOffset = NULL;
    }
  if(this->LoggingFrame)
    {
    this->LoggingFrame->Delete();
    this->LoggingFrame = NULL;
    }
  if(this->StartStopLoggingToFileCheckButton)
    {
    this->StartStopLoggingToFileCheckButton->Delete();
    this->StartStopLoggingToFileCheckButton = NULL;
    }
  if(this->LogFileLoadMsg)
    {
    this->LogFileLoadMsg->Delete();
    this->LogFileLoadMsg = NULL;
    }
  if(this->LogFileButton)
    {
    this->LogFileButton->Delete();
    this->LogFileButton = NULL;
    }
  
  #if defined(USE_NDIOAPI)
  this->Tracker->Delete();
  #endif
  
  if (this->TrackerTimerId)
    {
    vtkKWTkUtilities::CancelTimerHandler(vtkKWApplication::GetMainInterp(), this->TrackerTimerId); 
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  switch (this->GetGUI()->GetMode())     
    {

    case vtkPerkStationModuleGUI::ModeId::Training:

      this->SetName("3/5. Insert");
      this->GetGUI()->GetWizardWidget()->Update();
      break;

    case vtkPerkStationModuleGUI::ModeId::Clinical:
       
      // in clinical mode
      this->SetName("3/4. Insert");
      this->GetGUI()->GetWizardWidget()->Update();
      break;
    }

  
  this->SetDescription("Do the needle insertion");

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);

  vtkKWWidget *parent = wizard_widget->GetClientArea();


  // load registration file components

  if(!this->LoadTrackerConfigFrame)
    {
    this->LoadTrackerConfigFrame = vtkKWFrameWithLabel::New();
    }
  if(!this->LoadTrackerConfigFrame->IsCreated())
    {
    this->LoadTrackerConfigFrame->SetParent(parent);
    this->LoadTrackerConfigFrame->Create(); 
    }
   this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->LoadTrackerConfigFrame->GetWidgetName());


  if(!this->TrackerConfigFileLoadMsg)
    {
    this->TrackerConfigFileLoadMsg = vtkKWLabel::New();
    }
  if(!this->TrackerConfigFileLoadMsg->IsCreated())
    {
    this->TrackerConfigFileLoadMsg->SetParent(this->LoadTrackerConfigFrame->GetFrame());
    this->TrackerConfigFileLoadMsg->Create();
    this->TrackerConfigFileLoadMsg->SetText("Tracker config file not loaded yet");
    }
   this->Script("pack %s -side left -anchor nw -fill x -padx 0 -pady 2", 
                        this->TrackerConfigFileLoadMsg->GetWidgetName());
  
    // create the load file dialog button
   if (!this->LoadTrackerConfigFileButton)
    {
    this->LoadTrackerConfigFileButton = vtkKWLoadSaveButton::New();
    }
    if (!this->LoadTrackerConfigFileButton->IsCreated())
    {
    this->LoadTrackerConfigFileButton->SetParent(this->LoadTrackerConfigFrame->GetFrame());
    this->LoadTrackerConfigFileButton->Create();
    this->LoadTrackerConfigFileButton->SetBorderWidth(2);
    this->LoadTrackerConfigFileButton->SetReliefToRaised();       
    this->LoadTrackerConfigFileButton->SetHighlightThickness(2);
    this->LoadTrackerConfigFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadTrackerConfigFileButton->SetActiveBackgroundColor(1,1,1);
    this->LoadTrackerConfigFileButton->SetText("Load registration");
    this->LoadTrackerConfigFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LoadTrackerConfigFileButton->SetBalloonHelpString("Load tracker config file");
    this->LoadTrackerConfigFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadTrackerConfigFileButton->TrimPathFromFileNameOn();
    this->LoadTrackerConfigFileButton->SetMaximumFileNameLength(256);
    this->LoadTrackerConfigFileButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode
    this->LoadTrackerConfigFileButton->GetLoadSaveDialog()->SetFileTypes("{{CFG File} {.cfg}} {{All Files} {*.*}}");      
    }
    this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                        this->LoadTrackerConfigFileButton->GetWidgetName());


  if(!this->TrackerConnectionFrame)
    {
    this->TrackerConnectionFrame = vtkKWFrameWithLabel::New();
    }
  if(!this->TrackerConnectionFrame->IsCreated())
    {
    this->TrackerConnectionFrame->SetParent(parent);
    this->TrackerConnectionFrame->Create();
    //this->LoadRegistrationFrame->SetE
    }
   this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->TrackerConnectionFrame->GetWidgetName());



  // Connect to tracker check button : this will try to establish connection in callback, and receive data in another callback

  if (!this->ConnectTrackerCheckButton)
    {
    this->ConnectTrackerCheckButton = vtkKWCheckButtonWithLabel::New();
    }
  if (!this->ConnectTrackerCheckButton->IsCreated())
    {
    this->ConnectTrackerCheckButton->SetParent(this->TrackerConnectionFrame->GetFrame());
    this->ConnectTrackerCheckButton->Create();
    this->ConnectTrackerCheckButton->GetLabel()->SetBackgroundColor(0.7,0.7,0.7);
    this->ConnectTrackerCheckButton->SetLabelText("Connect to tracker:");
    this->ConnectTrackerCheckButton->SetHeight(4);
    }
 
  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->ConnectTrackerCheckButton->GetWidgetName());

  
  if(!this->TrackerStatusMsg)
    {
    this->TrackerStatusMsg = vtkKWLabel::New();
    }
  if(!this->TrackerStatusMsg->IsCreated())
    {
    this->TrackerStatusMsg->SetParent(this->TrackerConnectionFrame->GetFrame());
    this->TrackerStatusMsg->Create();
    this->TrackerStatusMsg->SetText("");
    }
   this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->TrackerStatusMsg->GetWidgetName());
  
  /*this->Script("grid %s -column 0 -row 0 -sticky nw -padx 2 -pady 2", 
               this->ConnectTrackerCheckButton->GetWidgetName());*/
  
  
  if (!this->NeedleToolFrame)
    {
    this->NeedleToolFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->NeedleToolFrame->IsCreated())
    {
    this->NeedleToolFrame->SetParent(parent);
    this->NeedleToolFrame->Create();
    this->NeedleToolFrame->SetLabelText("Needle tool info");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
               this->NeedleToolFrame->GetWidgetName());

  
  if (!this->NeedleTipPositionFrame)
    {
    this->NeedleTipPositionFrame = vtkKWFrame::New();
    }
  if (!this->NeedleTipPositionFrame->IsCreated())
    {
    this->NeedleTipPositionFrame->SetParent(this->NeedleToolFrame->GetFrame());
    this->NeedleTipPositionFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
               this->NeedleTipPositionFrame->GetWidgetName());


  // label

  if (!this->NeedleTipPositionLabel)
    { 
    this->NeedleTipPositionLabel = vtkKWLabel::New();
    }
  if (!this->NeedleTipPositionLabel->IsCreated())
    {
    this->NeedleTipPositionLabel->SetParent(this->NeedleTipPositionFrame);
    this->NeedleTipPositionLabel->Create();
    this->NeedleTipPositionLabel->SetText("Needle tip position: ");
    this->NeedleTipPositionLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }

  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->NeedleTipPositionLabel->GetWidgetName());
  
  // Needle tip position: will get populated/updated in a callback, which receives data from tracker
 
  if (!this->NeedleTipPosition)
    {
    this->NeedleTipPosition =  vtkKWEntrySet::New();    
    }
  if (!this->NeedleTipPosition->IsCreated())
    {
    this->NeedleTipPosition->SetParent(this->NeedleTipPositionFrame);
    this->NeedleTipPosition->Create();
    this->NeedleTipPosition->SetBorderWidth(2);
    this->NeedleTipPosition->SetReliefToGroove();
    this->NeedleTipPosition->SetPackHorizontally(1);
    this->NeedleTipPosition->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->NeedleTipPosition->AddWidget(id);     
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }
  this->Script("pack %s -side top  -anchor nw -padx 2 -pady 2", 
                this->NeedleTipPosition->GetWidgetName());
  
  // frame for Tool tip offset
  
  if (!this->ToolTipOffsetFrame)
    {
    this->ToolTipOffsetFrame = vtkKWFrame::New();
    }
  if (!this->ToolTipOffsetFrame->IsCreated())
    {
    this->ToolTipOffsetFrame->SetParent(this->NeedleToolFrame->GetFrame());
    this->ToolTipOffsetFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
               this->ToolTipOffsetFrame->GetWidgetName());

  // label for Tool tip offset

  if (!this->ToolTipOffsetLabel)
    { 
    this->ToolTipOffsetLabel = vtkKWLabel::New();
    }
  if (!this->ToolTipOffsetLabel->IsCreated())
    {
    this->ToolTipOffsetLabel->SetParent(this->ToolTipOffsetFrame);
    this->ToolTipOffsetLabel->Create();
    this->ToolTipOffsetLabel->SetText("Tool tip offset: ");
    this->ToolTipOffsetLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script("pack %s -side left -anchor nw -padx 0 -pady 2", 
                this->ToolTipOffsetLabel->GetWidgetName());

  // Tool tip offset: will get populated when the configuration file is loaded
 
  if (!this->ToolTipOffset)
    {
    this->ToolTipOffset =  vtkKWEntrySet::New();    
    }
  if (!this->ToolTipOffset->IsCreated())
    {
    this->ToolTipOffset->SetParent(this->ToolTipOffsetFrame);
    this->ToolTipOffset->Create();
    this->ToolTipOffset->SetBorderWidth(2);
    this->ToolTipOffset->SetReliefToGroove();
    this->ToolTipOffset->SetPackHorizontally(1);
    this->ToolTipOffset->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->ToolTipOffset->AddWidget(id);     
      entry->SetWidth(7);
      entry->ReadOnlyOn();      
      entry->SetValueAsDouble(0);
      }
    }
  this->Script("pack %s -side top  -anchor nw -padx 2 -pady 2", 
                this->ToolTipOffset->GetWidgetName());


  // logging to file components
  
  // frame
  if (!this->LoggingFrame)
    {
    this->LoggingFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->LoggingFrame->IsCreated())
    {
    this->LoggingFrame->SetParent(parent);
    this->LoggingFrame->Create();
    this->LoggingFrame->SetLabelText("Logging..");
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
               this->LoggingFrame->GetWidgetName());

  // msg label
  if(!this->LogFileLoadMsg)
    {
    this->LogFileLoadMsg = vtkKWLabel::New();
    }
  if(!this->LogFileLoadMsg->IsCreated())
    {
    this->LogFileLoadMsg->SetParent(this->LoggingFrame->GetFrame());
    this->LogFileLoadMsg->Create();
    this->LogFileLoadMsg->SetText("No log file chosen to be written to..");
    }
   this->Script("pack %s -side left -anchor nw -fill x -padx 0 -pady 2", 
                        this->LogFileLoadMsg->GetWidgetName()); 

  // save file button
  if (!this->LogFileButton)
    {
    this->LogFileButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LogFileButton->IsCreated())
    {
    this->LogFileButton->SetParent(this->LoggingFrame->GetFrame());
    this->LogFileButton->Create();
    this->LogFileButton->SetBorderWidth(2);
    this->LogFileButton->SetReliefToRaised();       
    this->LogFileButton->SetHighlightThickness(2);
    this->LogFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LogFileButton->SetActiveBackgroundColor(1,1,1);
    this->LogFileButton->SetText("Log file");
    this->LogFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LogFileButton->SetBalloonHelpString("Load log file to write to");
    this->LogFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LogFileButton->TrimPathFromFileNameOn();
    this->LogFileButton->SetMaximumFileNameLength(256);
    this->LogFileButton->GetLoadSaveDialog()->SaveDialogOn(); // save mode  
    this->LogFileButton->GetLoadSaveDialog()->SetFileTypes("{{CFG File} {.cfg}} {{All Files} {*.*}}");      
    }
  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                        this->LogFileButton->GetWidgetName());

  // start/stop logging toggle button
  if (!this->StartStopLoggingToFileCheckButton)
    {
    this->StartStopLoggingToFileCheckButton = vtkKWCheckButton::New();
    }
  if (!this->StartStopLoggingToFileCheckButton->IsCreated())
    {
    this->StartStopLoggingToFileCheckButton->SetParent(this->LoggingFrame->GetFrame());
    this->StartStopLoggingToFileCheckButton->Create();
    this->StartStopLoggingToFileCheckButton->SetText("Start logging");  
    this->StartStopLoggingToFileCheckButton->SetCompoundModeToLeft();
    this->StartStopLoggingToFileCheckButton->SetBorderWidth(2);
    this->StartStopLoggingToFileCheckButton->SetReliefToSunken();
    this->StartStopLoggingToFileCheckButton->SetOffReliefToRaised();
    this->StartStopLoggingToFileCheckButton->SetHighlightThickness(2);
    this->StartStopLoggingToFileCheckButton->IndicatorVisibilityOff();    
    this->StartStopLoggingToFileCheckButton->SetBackgroundColor(0.85,0.85,0.85);
    this->StartStopLoggingToFileCheckButton->SetActiveBackgroundColor(1,1,1);
    this->StartStopLoggingToFileCheckButton->IndicatorVisibilityOff();  
    this->StartStopLoggingToFileCheckButton->SetEnabled(0);
    }
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 2", 
                        this->StartStopLoggingToFileCheckButton->GetWidgetName());


  // TO DO: install callbacks
  this->InstallCallbacks();

  this->AddGUIObservers();

  // TO DO: populate controls wherever needed
  this->PopulateControls();

//  if (this->GetGUI()->GetSecondaryMonitor()->GetDepthLinesInitialized())
  //  {
    this->LogTimer->StartTimer();
//  }

}

//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
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

  // load configdialog button
  if (this->LoadTrackerConfigFileButton && this->LoadTrackerConfigFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    const char *fileName = this->LoadTrackerConfigFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      //this->CalibFilePath = std::string(this->LoadCalibrationFileButton->GetLoadSaveDialog()->GetLastPath());
      // indicates ok has been pressed with a file name
      this->ConfigFileName = std::string(fileName);

      // call the callback function
      this->LoadConfigButtonCallback();   
      
      }
    
    // reset the file browse button text
    //this->LoadTrackerConfigFileButton->SetText ("Load new config");
   
    }
  // check button
  if (this->ConnectTrackerCheckButton && this->ConnectTrackerCheckButton->GetWidget()== vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
    // vertical flip button selected state changed
    this->ConnectTrackerCallback(bool(this->ConnectTrackerCheckButton->GetWidget()->GetSelectedState()));
    }

  
  // log file dialog button
  if (this->LogFileButton && this->LogFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    const char *fileName = this->LogFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      //this->CalibFilePath = std::string(this->LoadCalibrationFileButton->GetLoadSaveDialog()->GetLastPath());
      // indicates ok has been pressed with a file name
      this->InsertionLogFileName = std::string(fileName);

      // call the callback function
      this->LogFileSaveButtonCallback();   
      
      }
    
    }
  // check button
  if (this->StartStopLoggingToFileCheckButton && this->StartStopLoggingToFileCheckButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
    this->LogFileCheckButtonCallback(bool(this->StartStopLoggingToFileCheckButton->GetSelectedState()));
    }

  

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
      }
    if (this->TrackerTimerId)
      {
      vtkKWTkUtilities::CancelTimerHandler(vtkKWApplication::GetMainInterp(), this->TrackerTimerId); 
      }
    }


  #endif(USE_NDIOAPI)
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

//-----------------------------------------------------------------------------
void vtkPerkStationInsertStep::LoadInsertion(istream &file)
{
 
}

//-----------------------------------------------------------------------------
void vtkPerkStationInsertStep::SaveInsertion(ostream& of)
{
 
}


//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

   // load config file button
  if (this->LoadTrackerConfigFileButton)
    {
    this->LoadTrackerConfigFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->ConnectTrackerCheckButton)
    {
    this->ConnectTrackerCheckButton->GetWidget()->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
  if (this->LogFileButton)
    {
    this->LogFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->StartStopLoggingToFileCheckButton)
    {
    this->StartStopLoggingToFileCheckButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
  
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::RemoveGUIObservers()
{
  // load reset components
  if (this->LoadTrackerConfigFileButton)
    {
    this->LoadTrackerConfigFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->ConnectTrackerCheckButton)
    {
    this->ConnectTrackerCheckButton->GetWidget()->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
   if (this->LogFileButton)
    {
    this->LogFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->StartStopLoggingToFileCheckButton)
    {
    this->StartStopLoggingToFileCheckButton->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::LoadConfigButtonCallback()
{
    //if (this->CalibFileName.empty() || this->CalibFilePath.empty())
      //return;

    //std::string fileNameWithPath = this->CalibFilePath+"/"+this->CalibFileName;
    std::string fileNameWithPath = this->ConfigFileName;
    ifstream file(fileNameWithPath.c_str());    
    bool fileRead = this->GetGUI()->GetLogic()->ReadConfigFile(file);
    file.close();
    if (fileRead){
        this->TrackerConfigFileLoadMsg->SetText("Config file read correctly");
        this->UpdateToolTipOffset();
    }
    else
        this->TrackerConfigFileLoadMsg->SetText("Config file could not be read");
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

  // stop the log timer
  this->LogTimer->StopTimer();

  // log the time in mrml node
  mrmlNode->SetTimeSpentOnInsertStep(this->LogTimer->GetElapsedTime());
 

}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::UpdateToolTipOffset()
{
  #if defined(USE_NDIOAPI)
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (mrmlNode)
  {
  vtkTrackerTool *tool = this->Tracker->GetTool(mrmlNode->GetNeedleToolPort());
  double *tool_tip_offset = mrmlNode->GetToolTipOffset();
  vtkMatrix4x4 *toolCalibrationMatrix = tool->GetBuffer()->GetToolCalibrationMatrix();
  for(unsigned int i=0;i<3;i++){
    toolCalibrationMatrix->SetElement(i, 3, tool_tip_offset[i]);
    this->ToolTipOffset->GetWidget(i)->SetValueAsDouble(tool_tip_offset[i]);
  }
  }
  #endif
}
