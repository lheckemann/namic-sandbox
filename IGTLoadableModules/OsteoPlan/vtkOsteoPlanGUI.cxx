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
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

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
          // Get Planes from vtkBoxWidget  
          vtkPlanes* planes = vtkPlanes::New();
          this->CuttingPlane->GetPlanes(planes);

          // Data to clip
          vtkMRMLModelNode* model = reinterpret_cast<vtkMRMLModelNode*>(this->ModelToCutSelector->GetSelected());
       
          // Set Clipper 1
          vtkClipPolyData* clipper = vtkClipPolyData::New();
          clipper->SetInput(model->GetPolyData());
          clipper->SetClipFunction(planes);
          clipper->GenerateClipScalarsOn();
          clipper->GenerateClippedOutputOn();
          clipper->InsideOutOn();
      
          // Set Clipper 2
          vtkClipPolyData* clipper2 = vtkClipPolyData::New();
          clipper2->SetInput(model->GetPolyData());
          clipper2->SetClipFunction(planes);
          clipper2->GenerateClipScalarsOn();
          clipper2->GenerateClippedOutputOn();
          clipper2->InsideOutOff();
          
          // Set Mapper 1     
          vtkPolyDataMapper* clipMapper = vtkPolyDataMapper::New();
          clipMapper->SetInputConnection(clipper->GetOutputPort());
          clipMapper->ScalarVisibilityOff();
          
          // Set Mapper 2
          vtkPolyDataMapper* clipMapper2 = vtkPolyDataMapper::New();
          clipMapper2->SetInputConnection(clipper2->GetOutputPort());
          clipMapper2->ScalarVisibilityOff();
          
          // Set Actor 1
          vtkActor* clipActor = vtkActor::New();
          clipActor->SetMapper(clipMapper);
          clipActor->GetProperty()->SetColor(1,0,0);

       // Set Actor 2
          vtkActor* clipActor2 = vtkActor::New();
          clipActor2->SetMapper(clipMapper2);
          clipActor2->GetProperty()->SetColor(0,0,1);

          // Add to MRML Scene
          vtkMRMLModelNode* part1 = vtkMRMLModelNode::New();
          vtkMRMLModelNode* part2 = vtkMRMLModelNode::New();
          vtkMRMLModelDisplayNode* dnode1 = vtkMRMLModelDisplayNode::New();
          vtkMRMLModelDisplayNode* dnode2 = vtkMRMLModelDisplayNode::New();      

          // - Model 1
          dnode1->SetColor(clipActor->GetProperty()->GetColor());
          this->GetMRMLScene()->AddNode(dnode1);
          part1->SetAndObservePolyData(clipper->GetOutput());
          part1->SetAndObserveDisplayNodeID(dnode1->GetID());
          this->GetMRMLScene()->AddNode(part1);

          // - Base
          // -- Check Base does not exist
          vtkCollection* listmodel2 = this->GetMRMLScene()->GetNodesByName("Base");
          if(listmodel2->GetNumberOfItems() > 0)
         {
           // --- Delete All "Base" Model
           for(int i=0; i < listmodel2->GetNumberOfItems(); i++)
          {
            vtkMRMLNode* basemodel = reinterpret_cast<vtkMRMLNode*>(listmodel2->GetItemAsObject(i));
                  this->GetMRMLScene()->RemoveNode(basemodel);
          }
         }
          listmodel2->Delete();

       // -- Draw (or redraw if deleted) "Base" Model
          dnode2->SetColor(clipActor2->GetProperty()->GetColor());
          this->GetMRMLScene()->AddNode(dnode2);
          part2->SetAndObservePolyData(clipper2->GetOutput());         
          part2->SetAndObserveDisplayNodeID(dnode2->GetID());
          part2->SetName("Base");
          this->GetMRMLScene()->AddNode(part2);

       // Select base if existing to prepare for next clipping and Hide original model
          listmodel2 = this->GetMRMLScene()->GetNodesByName("Base");
          if(this->ModelToCutSelector && listmodel2->GetNumberOfItems() == 1)
         {
              vtkMRMLModelNode* basemodel = reinterpret_cast<vtkMRMLModelNode*>(listmodel2->GetItemAsObject(0));
              vtkMRMLModelNode* originalmodel = reinterpret_cast<vtkMRMLModelNode*>(this->ModelToCutSelector->GetSelected());
              originalmodel->GetModelDisplayNode()->SetVisibility(0); 
              this->ModelToCutSelector->SetSelected(basemodel);
         } 
 
          // Delete ******************** 
          planes->Delete();
          clipper->Delete();
          clipper2->Delete();
          clipMapper->Delete();
          clipMapper2->Delete();
          clipActor->Delete();
          clipActor2->Delete();
          dnode1->Delete();
          dnode2->Delete();
          part1->Delete();
          part2->Delete();
          listmodel2->Delete();

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

