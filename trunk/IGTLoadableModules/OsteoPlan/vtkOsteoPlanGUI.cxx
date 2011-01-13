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

#include "vtkOsteoPlanGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWPushButton.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkBoxWidget.h"
#include "vtkBoxRepresentation.h"
#include "vtkProperty.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWPushButton.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"

#include "vtkPlanes.h"
#include "vtkClipPolyData.h"
#include "vtkPolyData.h"


#include "vtkCornerAnnotation.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkOsteoPlanGUI );
vtkCxxRevisionMacro ( vtkOsteoPlanGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkOsteoPlanGUI::vtkOsteoPlanGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkOsteoPlanGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->EnableCutter = NULL;
  
  //----------------------------------------------------------------
  // Plane widgets
  //  this->CuttingPlane = NULL;
  this->CuttingPlane = vtkBoxWidget::New();
  this->ModelToCutSelector = NULL;
  this->PerformCutButton = NULL;
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkOsteoPlanGUI::~vtkOsteoPlanGUI ( )
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

  if (this->EnableCutter)
    {
    this->EnableCutter->SetParent(NULL);
    this->EnableCutter->Delete();
    }


  if (this->CuttingPlane)
    {
      //this->CuttingPlane->SetParent(NULL);
    this->CuttingPlane->Delete();
    }

  if (this->ModelToCutSelector)
    {
    this->ModelToCutSelector->SetParent(NULL);
    this->ModelToCutSelector->Delete();
    }

  if(this->PerformCutButton)
    {
    this->PerformCutButton->SetParent(NULL);
    this->PerformCutButton->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::Enter()
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
void vtkOsteoPlanGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "OsteoPlanGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->EnableCutter)
    {
    this->EnableCutter
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ModelToCutSelector)
    {
    this->ModelToCutSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->PerformCutButton)
    {
    this->PerformCutButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::AddGUIObservers ( )
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

  this->EnableCutter->GetWidget()
    ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ModelToCutSelector->GetWidget()
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);


  this->PerformCutButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkOsteoPlanLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  // EnableCutter State Changed  
  if (this->EnableCutter->GetWidget() == vtkKWCheckButton::SafeDownCast(caller) 
      && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
      
      if(this->EnableCutter->GetWidget()->GetSelectedState() == 1)
     {
        // State Enabled -- Draw Plane
        
        vtkSlicerApplicationGUI *appGUI = vtkSlicerApplicationGUI::SafeDownCast (this->GetApplicationGUI() );      
        if (!appGUI)
          {
          vtkErrorMacro("ProcessGUIEvents: got Null SlicerApplicationGUI" );
          return;
          }

     
        // Initial Position
        double widgetPosition[6] = {-50,50,-50,50,-2,2};       
        this->CuttingPlane->SetInteractor(appGUI->GetActiveRenderWindowInteractor());
        this->CuttingPlane->PlaceWidget(widgetPosition);
        this->CuttingPlane->On();
        }
      else
     {
        // State Disabled -- Hide Plane
        this->CuttingPlane->Off();   
     }
    }


  // Cut button pressed  
  if (this->PerformCutButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
      // Clipping Slicer Model
      if(this->ModelToCutSelector && this->CuttingPlane)
     {  
       vtkClipPolyData* clipper = vtkClipPolyData::New();
          vtkMRMLModelNode* modOut = vtkMRMLModelNode::New();
          vtkMRMLModelDisplayNode* dnode = vtkMRMLModelDisplayNode::New();
          vtkPlanes* planes = vtkPlanes::New();
          this->CuttingPlane->GetPlanes(planes);
          
          vtkMRMLModelNode* model = reinterpret_cast<vtkMRMLModelNode*>(this->ModelToCutSelector->GetSelected());
          vtkPolyData* poly = model->GetPolyData();

          vtkPolyData* cpoly = vtkPolyData::New();
          cpoly->CopyStructure(poly);
 
          model->SetAndObservePolyData(cpoly);
          model->GetDisplayNode()->SetVisibility(0);

          clipper->SetClipFunction(planes);
          clipper->InsideOutOff();         
          clipper->SetInput(model->GetPolyData());
          clipper->Update();
          
          poly = clipper->GetOutput();

          dnode->SetColor(model->GetDisplayNode()->GetColor());
          this->GetMRMLScene()->AddNode(dnode);
          
          modOut->SetAndObservePolyData(poly);
          modOut->SetAndObserveDisplayNodeID(dnode->GetID());
          modOut->SetName("ClippedModel");
          this->GetMRMLScene()->AddNode(modOut);

          cpoly->Delete();
          dnode->Delete();
          clipper->Delete();
          planes->Delete();
          modOut->Delete();
     }  
     
    }
     


} 


void vtkOsteoPlanGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkOsteoPlanGUI *self = reinterpret_cast<vtkOsteoPlanGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkOsteoPlanGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkOsteoPlanLogic::SafeDownCast(caller))
    {
    if (event == vtkOsteoPlanLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessTimerEvents()
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
void vtkOsteoPlanGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "OsteoPlan", "OsteoPlan", NULL );

  BuildGUIForHelpFrame();
  BuildGUICutter();

}


void vtkOsteoPlanGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:OsteoPlan</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "OsteoPlan" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::BuildGUICutter()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("OsteoPlan");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Cutter Frame");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Enable Cutter");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );


  vtkKWFrame *frame2 = vtkKWFrame::New();
  frame2->SetParent(frame->GetFrame());
  frame2->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",       frame2->GetWidgetName() );


  // -----------------------------------------
  // Cutter

  this->EnableCutter = vtkKWCheckButtonWithLabel::New ( );
  this->EnableCutter->SetParent ( frame2 );
  this->EnableCutter->Create ( );
  this->EnableCutter->SetWidth (12);
  this->EnableCutter->SetLabelPositionToRight();
  this->EnableCutter->GetLabel()->SetText ("Enable Cutter");

  this->Script("pack %s -side top -padx 2 -pady 2", 
               this->EnableCutter->GetWidgetName());

  this->ModelToCutSelector = vtkSlicerNodeSelectorWidget::New();
  this->ModelToCutSelector->SetParent(frame2);
  this->ModelToCutSelector->Create();
  this->ModelToCutSelector->SetWidth(30);
  this->ModelToCutSelector->SetNewNodeEnabled(0);
  this->ModelToCutSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->ModelToCutSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->ModelToCutSelector->UpdateMenu();

  vtkKWLabel* ModelToCutLabel = vtkKWLabel::New();
  ModelToCutLabel->SetParent(frame2);
  ModelToCutLabel->Create();
  ModelToCutLabel->SetText("Model to cut");
  ModelToCutLabel->SetAnchorToWest();

  app->Script("pack %s %s -fill x -side top -padx 2 -pady 2",
               ModelToCutLabel->GetWidgetName(),
               this->ModelToCutSelector->GetWidgetName());

  this->PerformCutButton = vtkKWPushButton::New();
  this->PerformCutButton->SetParent(frame2);
  this->PerformCutButton->Create();
  this->PerformCutButton->SetText("Cut Model");

  app->Script("pack %s -fill x -side top -padx 2 -pady 2",
              this->PerformCutButton->GetWidgetName());


  ModelToCutLabel->Delete();

  frame2->Delete();
  conBrowsFrame->Delete();
  frame->Delete();

}

//----------------------------------------------------------------------------
void vtkOsteoPlanGUI::UpdateAll()
{
}

