/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkHybridProbeGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMatrix4x4.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWPushButton.h"
#include "vtkCornerAnnotation.h"

#include "vtkIGTPat2ImgRegistration.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkHybridProbeGUI );
vtkCxxRevisionMacro ( vtkHybridProbeGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkHybridProbeGUI::vtkHybridProbeGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkHybridProbeGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets

  this->EMTransformNodeSelector = NULL;
  this->OptTransformNodeSelector = NULL;

  this->EMTransformNode = NULL;
  this->OptTransformNode = NULL;

  this->RecordPointButton = NULL;
  this->ResetButton = NULL;
  this->PerformRegistrationButton = NULL;
  
  this->EMTempMatrix = vtkMatrix4x4::New();
  this->OptTempMatrix = vtkMatrix4x4::New();
  this->EMPositions = vtkMatrix4x4::New();
  this->OptPositions = vtkMatrix4x4::New();
  this->numberOfPoints = 0;

  this->firstClick = false;

  this->EMOptRegMatrix = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkHybridProbeGUI::~vtkHybridProbeGUI ( )
{

  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets

  if (this->EMTransformNodeSelector)
    {
    this->EMTransformNodeSelector->SetParent(NULL);
    this->EMTransformNodeSelector->Delete();
    }

  if (this->OptTransformNodeSelector)
    {
    this->OptTransformNodeSelector->SetParent(NULL);
    this->OptTransformNodeSelector->Delete();
    }

  if (this->ResetButton)
    {
    this->ResetButton->SetParent(NULL);
    this->ResetButton->Delete();
    }

  if (this->RecordPointButton)
    {
    this->RecordPointButton->SetParent(NULL);
    this->RecordPointButton->Delete();
    }

  if(this->PerformRegistrationButton)
    {
    this->PerformRegistrationButton->SetParent(NULL);
    this->PerformRegistrationButton->Delete();
    }
  
  if (this->EMTempMatrix)
    {
    this->EMTempMatrix->Delete();
    }

  if (this->OptTempMatrix)
    {
    this->OptTempMatrix->Delete();
    }
  
  if (this->EMPositions)
    {
    this->EMPositions->Delete();
    }

  if (this->OptPositions)
    {
    this->OptPositions->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "HybridProbeGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->EMTransformNodeSelector)
    {
    this->EMTransformNodeSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->OptTransformNodeSelector)
    {
    this->OptTransformNodeSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ResetButton)
    {
    this->ResetButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->RecordPointButton)
    {
    this->RecordPointButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->PerformRegistrationButton)
    {
    this->PerformRegistrationButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  this->EMTransformNodeSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->OptTransformNodeSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ResetButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->RecordPointButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->PerformRegistrationButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkHybridProbeGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkHybridProbeLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkHybridProbeGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (this->RecordPointButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->EMTransformNodeSelector && this->OptTransformNodeSelector)
      {
      this->EMTransformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->EMTransformNodeSelector->GetSelected());
      this->OptTransformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->OptTransformNodeSelector->GetSelected());

      if(this->EMTransformNode && this->OptTransformNode)
        {

          vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
          vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

          if(!this->firstClick)
            {
            // Convert int to string
            int numToDisplay = this->numberOfPoints+1;
            std::ostringstream oss;
            oss << numToDisplay;
            std::string RecordText = "Record Position " + oss.str();

            this->RecordPointButton->SetText(RecordText.c_str());
            this->RecordPointButton->SetBackgroundColor(color->SliceGUIYellow);
            this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIYellow);

            this->firstClick = true;
            }
          else
            {

          if(this->numberOfPoints >=0 && this->numberOfPoints < MAX_NUM_POINTS)
            {
            RecordPoints();

            // Convert int to string
            int numToDisplay = this->numberOfPoints+1;
            std::ostringstream oss;
            oss << numToDisplay;
            std::string RecordText = "Record Position " + oss.str();

            this->RecordPointButton->SetText(RecordText.c_str());
            this->RecordPointButton->SetBackgroundColor(color->SliceGUIYellow);
            this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIYellow);

            }
          else if(this->numberOfPoints == MAX_NUM_POINTS)
            {
            RecordPoints();

            // Convert int to string
            int numToDisplay = this->numberOfPoints;
            std::ostringstream oss;
            oss << numToDisplay;
            std::string RecordText = "Record Position " + oss.str();

            this->RecordPointButton->SetText(RecordText.c_str());
            this->RecordPointButton->SetBackgroundColor(color->White);
            this->RecordPointButton->SetActiveBackgroundColor(color->White);

            this->RecordPointButton->SetEnabled(0);

            this->PerformRegistrationButton->SetBackgroundColor(color->SliceGUIGreen);
            this->PerformRegistrationButton->SetActiveBackgroundColor(color->SliceGUIGreen);
            this->PerformRegistrationButton->SetEnabled(1);

            }
    }
        
        }
      }
    }

  if(this->ResetButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
      Reset();
    }

  if(this->EMTransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

    Reset();

    this->EMTransformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->EMTransformNodeSelector->GetSelected());
    if(!this->OptTransformNode && this->EMTransformNode)
      {
      this->RecordPointButton->SetBackgroundColor(color->SliceGUIYellow);
      this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIYellow);
      this->RecordPointButton->SetText("Select Optical Tracking System");
      }
    else if(!this->OptTransformNode && !this->EMTransformNode)
      {
      this->RecordPointButton->SetBackgroundColor(color->SliceGUIOrange);
      this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIOrange);
      this->RecordPointButton->SetText("Select Optical and EM Tracking Systems");
      }
    else if(this->OptTransformNode && this->EMTransformNode)
      {
      this->RecordPointButton->SetBackgroundColor(color->SliceGUIGreen);
      this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->RecordPointButton->SetText("Start Recording Points");
      }
    }


  if(this->OptTransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

    Reset();

    this->OptTransformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->OptTransformNodeSelector->GetSelected());
    if(!this->EMTransformNode && this->OptTransformNode)
      {
      this->RecordPointButton->SetBackgroundColor(color->SliceGUIYellow);
      this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIYellow);
      this->RecordPointButton->SetText("Select EM Tracking System");
      }
    else if(!this->EMTransformNode && !this->OptTransformNode)
      {
      this->RecordPointButton->SetBackgroundColor(color->SliceGUIOrange);
      this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIOrange);
      this->RecordPointButton->SetText("Select Optical and EM Tracking Systems");
      }
    else if(this->OptTransformNode && this->EMTransformNode)
      {
      this->RecordPointButton->SetBackgroundColor(color->SliceGUIGreen);
      this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->RecordPointButton->SetText("Start Recording Points");
      }
    }

  if (this->PerformRegistrationButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->numberOfPoints == 4)
      {
      PerformRegistration();
      }
    }

} 


void vtkHybridProbeGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkHybridProbeGUI *self = reinterpret_cast<vtkHybridProbeGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkHybridProbeGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkHybridProbeLogic::SafeDownCast(caller))
    {
    if (event == vtkHybridProbeLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "HybridProbe", "HybridProbe", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForSelectingProbes();

}


void vtkHybridProbeGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:HybridProbe</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "HybridProbe" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkHybridProbeGUI::BuildGUIForSelectingProbes()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("HybridProbe");
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();
  

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Optical / EM Probes Registration");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Selecting Probes");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  // -----------------------------------------
  // Selecting Probes

  vtkKWLabel* EMTrackingLabel = vtkKWLabel::New();
  EMTrackingLabel->SetParent(frame->GetFrame());
  EMTrackingLabel->Create();
  EMTrackingLabel->SetText("EM Transformation Node:");
  EMTrackingLabel->SetAnchorToWest();

  this->EMTransformNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->EMTransformNodeSelector->SetParent(frame->GetFrame());
  this->EMTransformNodeSelector->Create();
  this->EMTransformNodeSelector->SetNewNodeEnabled(0);
  this->EMTransformNodeSelector->SetNodeClass("vtkMRMLLinearTransformNode",NULL,NULL,NULL);
  this->EMTransformNodeSelector->SetMRMLScene(this->GetLogic()->GetMRMLScene());
  this->EMTransformNodeSelector->UpdateMenu();

  vtkKWLabel* OptTrackingLabel = vtkKWLabel::New();
  OptTrackingLabel->SetParent(frame->GetFrame());
  OptTrackingLabel->Create();
  OptTrackingLabel->SetText("Optical Transformation Node:");
  OptTrackingLabel->SetAnchorToWest();

  this->OptTransformNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->OptTransformNodeSelector->SetParent(frame->GetFrame());
  this->OptTransformNodeSelector->Create();
  this->OptTransformNodeSelector->SetNewNodeEnabled(0);
  this->OptTransformNodeSelector->SetNodeClass("vtkMRMLLinearTransformNode",NULL,NULL,NULL);
  this->OptTransformNodeSelector->SetMRMLScene(this->GetLogic()->GetMRMLScene());
  this->OptTransformNodeSelector->UpdateMenu();

  vtkKWFrame* RegistrationButtonFrame = vtkKWFrame::New();
  RegistrationButtonFrame->SetParent(frame->GetFrame());
  RegistrationButtonFrame->Create();

  this->RecordPointButton = vtkKWPushButton::New ( );
  this->RecordPointButton->SetParent ( RegistrationButtonFrame );
  this->RecordPointButton->Create ( );
  this->RecordPointButton->SetText ("Select Optical and EM Tracking Systems");
  this->RecordPointButton->SetWidth(35);
  this->RecordPointButton->SetBackgroundColor(color->SliceGUIOrange);
  this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIOrange);

  this->ResetButton = vtkKWPushButton::New();
  this->ResetButton->SetParent( RegistrationButtonFrame );
  this->ResetButton->Create();
  this->ResetButton->SetText ("Reset");
  this->ResetButton->SetWidth(15);
  this->ResetButton->SetBackgroundColor(color->MediumGrey);
  this->ResetButton->SetActiveBackgroundColor(color->MediumGrey);

  this->Script("pack %s %s -side left -fill x -padx 2 -pady 2",
         this->RecordPointButton->GetWidgetName(),
         this->ResetButton->GetWidgetName());

  this->PerformRegistrationButton = vtkKWPushButton::New();
  this->PerformRegistrationButton->SetParent( frame->GetFrame() );
  this->PerformRegistrationButton->Create();
  this->PerformRegistrationButton->SetText("Perform Registration");
  this->PerformRegistrationButton->SetBackgroundColor(color->White);
  this->PerformRegistrationButton->SetActiveBackgroundColor(color->White);
  this->PerformRegistrationButton->SetEnabled(0);

  this->Script("pack %s %s %s %s %s %s -side top -fill x -padx 2 -pady 2",
         EMTrackingLabel->GetWidgetName(),
         this->EMTransformNodeSelector->GetWidgetName(),
         OptTrackingLabel->GetWidgetName(),
         this->OptTransformNodeSelector->GetWidgetName(), 
               RegistrationButtonFrame->GetWidgetName(),
               this->PerformRegistrationButton->GetWidgetName());

  RegistrationButtonFrame->Delete();
  conBrowsFrame->Delete();
  frame->Delete();
  EMTrackingLabel->Delete();
  OptTrackingLabel->Delete();

}


//----------------------------------------------------------------------------
void vtkHybridProbeGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
void vtkHybridProbeGUI::RecordPoints()
{
  if(this->EMTransformNode && this->EMTempMatrix && this->EMPositions
     && this->OptTransformNode && this->OptTempMatrix && this->OptPositions)
    {
    this->EMTempMatrix->Identity();
    this->OptTempMatrix->Identity();

    this->EMTransformNode->GetMatrixTransformToWorld(this->EMTempMatrix);
    this->OptTransformNode->GetMatrixTransformToWorld(this->OptTempMatrix);

    // Fill EM Matrix
    this->EMPositions->SetElement(0, this->numberOfPoints, this->EMTempMatrix->GetElement(0,3));
    this->EMPositions->SetElement(1, this->numberOfPoints, this->EMTempMatrix->GetElement(1,3));
    this->EMPositions->SetElement(2, this->numberOfPoints, this->EMTempMatrix->GetElement(2,3));
    this->EMPositions->SetElement(3, this->numberOfPoints, 1);

    // Fill Optical Matrix
    this->OptPositions->SetElement(0, this->numberOfPoints, this->OptTempMatrix->GetElement(0,3));
    this->OptPositions->SetElement(1, this->numberOfPoints, this->OptTempMatrix->GetElement(1,3));
    this->OptPositions->SetElement(2, this->numberOfPoints, this->OptTempMatrix->GetElement(2,3));
    this->OptPositions->SetElement(3, this->numberOfPoints, 1);

    this->numberOfPoints++;
    }
}

void vtkHybridProbeGUI::PerformRegistration()
{
  vtkIGTPat2ImgRegistration* EMOptReg = vtkIGTPat2ImgRegistration::New();

  EMOptReg->SetNumberOfPoints(this->numberOfPoints);

  for(int i=0; i < this->numberOfPoints; i++)
    {
    int EMx = this->EMPositions->GetElement(0, i);
    int EMy = this->EMPositions->GetElement(1, i);
    int EMz = this->EMPositions->GetElement(2, i);

    int Optx = this->OptPositions->GetElement(0, i);
    int Opty = this->OptPositions->GetElement(1, i);
    int Optz = this->OptPositions->GetElement(2, i);

    // Need to invert X axis to register Aurora and Vicra

    EMOptReg->AddPoint(i, Optx, Opty, Optz, -EMx, EMy, EMz);
    }

  int error = EMOptReg->DoRegistration();

  if(!error)
    {
    this->EMOptRegMatrix = EMOptReg->GetLandmarkTransformMatrix();
    vtkMRMLLinearTransformNode* tempTransform = vtkMRMLLinearTransformNode::New();
    tempTransform->SetName("EMToOptRegistration");
    tempTransform->ApplyTransform(this->EMOptRegMatrix);
    this->GetLogic()->GetMRMLScene()->AddNode(tempTransform);
    tempTransform->Delete();
    }
  EMOptReg->Delete();
  
}

void vtkHybridProbeGUI::Reset()
{
  this->numberOfPoints = 0;
  this->firstClick = false;

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  if(this->RecordPointButton)
    {
      if(!this->EMTransformNode && this->OptTransformNode)
  {
    this->RecordPointButton->SetBackgroundColor(color->SliceGUIYellow);
    this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIYellow);
    this->RecordPointButton->SetText("Select EM Tracking System");
  }
      else if(this->EMTransformNode && !this->OptTransformNode)
  {
    this->RecordPointButton->SetBackgroundColor(color->SliceGUIYellow);
    this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIYellow);
    this->RecordPointButton->SetText("Select Optical Tracking System");
  }
      else if(!this->EMTransformNode && !this->OptTransformNode)
  {
    this->RecordPointButton->SetBackgroundColor(color->SliceGUIOrange);
    this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIOrange);
    this->RecordPointButton->SetText("Select Optical and EM Tracking Systems");
  }
      else if(this->EMTransformNode && this->OptTransformNode)
  {
    this->RecordPointButton->SetBackgroundColor(color->SliceGUIGreen);
    this->RecordPointButton->SetActiveBackgroundColor(color->SliceGUIGreen);
    this->RecordPointButton->SetText("Start Recording Points");
    this->RecordPointButton->SetEnabled(1);
  }
    }

  if(this->PerformRegistrationButton)
    {
      this->PerformRegistrationButton->SetBackgroundColor(color->White);
      this->PerformRegistrationButton->SetActiveBackgroundColor(color->White);
      this->PerformRegistrationButton->SetEnabled(0);
    }
}
