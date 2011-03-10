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
#include "vtkKWPushButton.h"
#include "vtkKWCheckButtonWithLabel.h"

#include "vtkProperty.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkCollection.h"
#include "vtkBoxWidget2.h"
#include "vtkBoxRepresentation.h"

#include "vtkCellPicker.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

#include "vtkKWMenuButtonWithLabel.h"
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
  this->CuttingPlane = vtkBoxWidget2::New();
  this->boxRepresentation = vtkBoxRepresentation::New();
  
  this->widgetPosition[0] = -50;
  this->widgetPosition[1] = 50;
  this->widgetPosition[2] = -50;
  this->widgetPosition[3] = 50;
  this->widgetPosition[4] = -2.5;
  this->widgetPosition[5] = 2.5;


  this->ModelToCutSelector = NULL;
  this->PerformCutButton = NULL;
  this->cutterThicknessSelector = NULL;
  this->cutterThickness = 5;


  this->StartSelectingModelParts = NULL;
  this->StopSelectingModelParts = NULL;
  this->SelectingModelParts = false;
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

  if (this->boxRepresentation)
    {
      //this->CuttingPlane->SetParent(NULL);
    this->boxRepresentation->Delete();
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

  if(this->cutterThicknessSelector)
    {
    this->cutterThicknessSelector->SetParent(NULL);
    this->cutterThicknessSelector->Delete();
    }

  if(this->StartSelectingModelParts)
    {
    this->StartSelectingModelParts->SetParent(NULL);
    this->StartSelectingModelParts->Delete();
    }

  if(this->StopSelectingModelParts)
    {
    this->StopSelectingModelParts->SetParent(NULL);
    this->StopSelectingModelParts->Delete();
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

  if(this->cutterThicknessSelector->GetWidget()->GetMenu())
    {
      this->cutterThicknessSelector->GetWidget()->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->StartSelectingModelParts)
    {
      this->StartSelectingModelParts
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->StopSelectingModelParts)
    {
      this->StopSelectingModelParts
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->GetApplicationGUI()->GetActiveRenderWindowInteractor())
    {
      this->GetApplicationGUI()->GetActiveRenderWindowInteractor()
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

  this->cutterThicknessSelector->GetWidget()->GetMenu()
    ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->StartSelectingModelParts
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->StopSelectingModelParts
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);  

  this->GetApplicationGUI()->GetActiveRenderWindowInteractor()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);


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
  // FIXME: Not working anymore when loading scene, need to import scene instead
  // -> TODO: Move AddObservers on start button event and removeObservers at the end of this function
  //   -> Not working, create a segmentation fault

  if(this->SelectingModelParts)
    {
      int* mousePosition = this->GetApplicationGUI()->GetActiveRenderWindowInteractor()->GetEventPosition();
      
      vtkCellPicker* cellPicker = vtkCellPicker::New();
      cellPicker->Pick(mousePosition[0],mousePosition[1],0,this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
      int cellIdPicked = cellPicker->GetCellId();  

      vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
      connectivityFilter->SetInput(this->GetLogic()->Getpart2()->GetModelDisplayNode()->GetPolyData());
      connectivityFilter->SetExtractionModeToCellSeededRegions();
      connectivityFilter->InitializeSeedList();
      connectivityFilter->AddSeed(cellIdPicked);
      connectivityFilter->Update();

      // Create new polydata 
      vtkPolyData* polyDataModel = vtkPolyData::New();
 
      // Create New vtkMRMLNode
      vtkMRMLModelNode* modelSelected = vtkMRMLModelNode::New();
 
      // Create New vtkMRMLModelDisplayNode
      vtkMRMLModelDisplayNode* dnodeS = vtkMRMLModelDisplayNode::New();
 
      // Add them to the scene 
      modelSelected->SetScene(this->GetMRMLScene());
      dnodeS->SetScene(this->GetMRMLScene());
 
      this->GetMRMLScene()->AddNode(dnodeS);
      this->GetMRMLScene()->AddNode(modelSelected);
 
      // Use new polydata model to be "ready" to receive data
      modelSelected->SetAndObservePolyData(polyDataModel);
      modelSelected->SetAndObserveDisplayNodeID(dnodeS->GetID());
 
      // Copy polydata to the new polydata model
      dnodeS->SetPolyData(connectivityFilter->GetOutput());
 
      // Clean
      polyDataModel->Delete();
      modelSelected->Delete();
      dnodeS->Delete();
 
      connectivityFilter->Delete();
      cellPicker->Delete();
    }

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
        if(this->boxRepresentation && this->CuttingPlane)
       {
          this->boxRepresentation->GetFaceProperty()->SetRepresentationToSurface();
          this->boxRepresentation->GetSelectedHandleProperty()->SetColor(0,0,1);
       this->boxRepresentation->BuildRepresentation();
 
          this->CuttingPlane->SetRepresentation(this->boxRepresentation);      

          this->CuttingPlane->SetDefaultRenderer(this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
          this->CuttingPlane->SetCurrentRenderer(this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
          this->CuttingPlane->SetInteractor(appGUI->GetActiveRenderWindowInteractor());
        
       // ????????????????
          // keep cutter in the same position when re-enabled ?
          this->CuttingPlane->GetRepresentation()->PlaceWidget(this->widgetPosition);

          this->CuttingPlane->On();
       }
        }
      else
     {
        // State Disabled -- Hide Plane
        this->CuttingPlane->Off();   
     }
  
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

    }


  // Cut button pressed  
  if (this->PerformCutButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
      // Clipping Slicer Model
      if(this->ModelToCutSelector && this->CuttingPlane)
     {

          // Data to clip
          vtkMRMLModelNode* model = reinterpret_cast<vtkMRMLModelNode*>(this->ModelToCutSelector->GetSelected());

          this->GetLogic()->ClipModelWithBox(model, this->CuttingPlane, this->GetApplicationGUI());

       // Select base if existing to prepare for next clipping and Hide original model


          vtkCollection* listmodel2 = this->GetMRMLScene()->GetNodesByName("Base");
          if(this->ModelToCutSelector && listmodel2->GetNumberOfItems() == 1)
         {
              vtkMRMLModelNode* basemodel = reinterpret_cast<vtkMRMLModelNode*>(listmodel2->GetItemAsObject(0));
              vtkMRMLModelNode* originalmodel = reinterpret_cast<vtkMRMLModelNode*>(this->ModelToCutSelector->GetSelected());
              originalmodel->GetModelDisplayNode()->SetVisibility(0); 
              this->ModelToCutSelector->SetSelected(basemodel);
         } 

       /*
          // [] FIXME: Use to move model, but without without axis + Segfault   
        vtkInteractorStyleTrackballActor* movingaxes = vtkInteractorStyleTrackballActor::New();
          movingaxes->SetDefaultRenderer(this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
          movingaxes->SetEnabled(1); 
        
          this->GetApplicationGUI()->GetActiveRenderWindowInteractor()->SetInteractorStyle(movingaxes);
       */

          // Delete
          listmodel2->Delete();
     }  
       
    }

  if(this->cutterThicknessSelector->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller)
     && event == vtkKWMenu::MenuItemInvokedEvent)
    {
      int selected_index = this->cutterThicknessSelector->GetWidget()->GetMenu()->GetIndexOfSelectedItem();
      if(!strcmp(this->cutterThicknessSelector->GetWidget()->GetMenu()->GetItemSelectedValue(selected_index),"5 mm"))
     {

          this->widgetPosition[4] = -2.5;
          this->widgetPosition[5] = 2.5;

          vtkTransform* boxTransform = vtkTransform::New();
          this->boxRepresentation->GetTransform(boxTransform);
          this->boxRepresentation->PlaceWidget(this->widgetPosition);
          this->boxRepresentation->SetTransform(boxTransform);

          this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

          boxTransform->Delete();

     }
      if(!strcmp(this->cutterThicknessSelector->GetWidget()->GetMenu()->GetItemSelectedValue(selected_index),"10 mm"))
     {
 
          this->widgetPosition[4] = -5;
          this->widgetPosition[5] = 5;

          vtkTransform* boxTransform = vtkTransform::New();
          this->boxRepresentation->GetTransform(boxTransform);
          this->boxRepresentation->PlaceWidget(this->widgetPosition); 
          this->boxRepresentation->SetTransform(boxTransform);

          this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

          boxTransform->Delete();
     }

    }

  if(this->StartSelectingModelParts == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
      this->SelectingModelParts = true;      
    }

  if(this->StopSelectingModelParts == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
      this->SelectingModelParts = false;
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

  vtkKWFrame *frame3 = vtkKWFrame::New();
  frame3->SetParent(frame->GetFrame());
  frame3->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",       frame3->GetWidgetName() );
  
  // Cutter thickness
  this->cutterThicknessSelector = vtkKWMenuButtonWithLabel::New();
  this->cutterThicknessSelector->SetParent(frame3);
  this->cutterThicknessSelector->Create();
  this->cutterThicknessSelector->SetLabelText("Cutter thickness:");
  this->cutterThicknessSelector->GetWidget()->GetMenu()->AddRadioButton("5 mm");
  this->cutterThicknessSelector->GetWidget()->GetMenu()->AddRadioButton("10 mm");
  this->cutterThicknessSelector->GetWidget()->GetMenu()->SelectItem(0);

  app->Script("pack %s -fill x -side top -padx 2 -pady 2",
              this->cutterThicknessSelector->GetWidgetName());

  vtkKWFrame *frame4 = vtkKWFrame::New();
  frame4->SetParent(frame->GetFrame());
  frame4->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",       frame4->GetWidgetName() );

  this->StartSelectingModelParts = vtkKWPushButton::New();
  this->StartSelectingModelParts->SetParent(frame4);
  this->StartSelectingModelParts->Create();
  this->StartSelectingModelParts->SetText("Start Selecting Parts");

  this->StopSelectingModelParts = vtkKWPushButton::New();
  this->StopSelectingModelParts->SetParent(frame4);
  this->StopSelectingModelParts->Create();
  this->StopSelectingModelParts->SetText("Stop Selecting Parts");

  app->Script("pack %s %s -fill x -side left -expand y -padx 2 -pady 2",
              this->StartSelectingModelParts->GetWidgetName(),
              this->StopSelectingModelParts->GetWidgetName());
  

  ModelToCutLabel->Delete();

  frame4->Delete();
  frame3->Delete();
  frame2->Delete();
  conBrowsFrame->Delete();
  frame->Delete();

}

//----------------------------------------------------------------------------
void vtkOsteoPlanGUI::UpdateAll()
{
}

