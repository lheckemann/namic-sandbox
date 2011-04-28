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
#include "vtkMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"

#include "vtkAssembly.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLLinearTransformNode.h"

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
  // Clipping Part
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
  // Moving Part

  this->displayMoverButton = NULL;

  //----------------------------------------------------------------
  // Placing markers

  this->modelSelector = NULL;
  this->placeMarkersButton = NULL;

  this->placeMarkerOn = false;
  this->ListOfModels = vtkCollection::New();
  this->ListOfFiducialLists = vtkCollection::New();

  //----------------------------------------------------------------
  // Back to Original Position

  this->modelBackOSelector = NULL;
  this->backOriginalButton = NULL;
  this->ParentTransformationNode = NULL;

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
      this->CuttingPlane->Delete();
    }

  if (this->boxRepresentation)
    {
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

  if(this->displayMoverButton)
    {
      this->displayMoverButton->SetParent(NULL);
      this->displayMoverButton->Delete();
    }

  if(this->modelSelector)
    {
      this->modelSelector->SetParent(NULL);
      this->modelSelector->Delete();
    }

  if(this->placeMarkersButton)
    {
      this->placeMarkersButton->SetParent(NULL);
      this->placeMarkersButton->Delete();
    }

  if(this->ListOfModels)
    {
      this->ListOfModels->Delete();
    }
 
  if(this->ListOfFiducialLists)
    {
      this->ListOfFiducialLists->Delete();
    }

  if(this->modelBackOSelector)
    {
      this->modelBackOSelector->SetParent(NULL);
      this->modelBackOSelector->Delete();
    }

  if(this->backOriginalButton)
    {
      this->backOriginalButton->SetParent(NULL);
      this->backOriginalButton->Delete();
    }

  // if(this->ParentTransformationNode)
  //   {
  //     this->ParentTransformationNode->Delete();
  //   }


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

  if(this->modelBackOSelector && this->modelBackOSelector->GetSelected())
    {
      // Refresh menu (in case node has been changed)
      this->modelBackOSelector->InvokeEvent(vtkSlicerNodeSelectorWidget::NodeSelectedEvent);
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

  if(this->displayMoverButton)
    {
      this->displayMoverButton
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->placeMarkersButton)
    {
      this->placeMarkersButton
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->modelSelector)
    {
      this->modelSelector
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->backOriginalButton)
    {
      this->backOriginalButton
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->modelBackOSelector)
    {
      this->modelBackOSelector
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
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent); 
 
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

  this->displayMoverButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);  

  this->placeMarkersButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);  

  this->modelSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->backOriginalButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);  

  this->modelBackOSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

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
      if(cellIdPicked != -1)
     {
       vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
       connectivityFilter->SetInput(this->GetLogic()->Getpart2()->GetModelDisplayNode()->GetPolyData());
       connectivityFilter->SetExtractionModeToCellSeededRegions();
       connectivityFilter->InitializeSeedList();
       connectivityFilter->AddSeed(cellIdPicked);
       connectivityFilter->Update();

       // Create new polydata 
       vtkPolyData* polyDataModel = vtkPolyData::New();
       polyDataModel->CopyStructure(this->GetLogic()->Getpart2()->GetModelDisplayNode()->GetPolyData());
 
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


     }
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
          if(model)
         {
           this->GetLogic()->ClipModelWithBox(model, this->CuttingPlane, this->GetApplicationGUI());
         }

          // Delete
          // listmodel2->Delete();
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

   

  if(this->displayMoverButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
      // Display Mover here
      vtkAssembly* modelMover = this->GetLogic()->CreateMover();

      this->GetLogic()->GetXActor()->AddObserver(vtkCommand::LeftButtonPressEvent,(vtkCommand *)this->GUICallbackCommand);
      this->GetLogic()->GetYActor()->AddObserver(vtkCommand::LeftButtonPressEvent,(vtkCommand *)this->GUICallbackCommand);
      this->GetLogic()->GetZActor()->AddObserver(vtkCommand::LeftButtonPressEvent,(vtkCommand *)this->GUICallbackCommand);
  
      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(modelMover);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

      modelMover->Delete();
    } 
 
  if(this->placeMarkersButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
      this->placeMarkerOn = !this->placeMarkerOn;
      AddPairModelFiducial();
    }

  if (this->modelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
      if (this->ListOfModels)
     {
       for(int i = 0; i < this->ListOfModels->GetNumberOfItems(); i++)
         {
           if(this->modelSelector->GetSelected() == this->ListOfModels->GetItemAsObject(i))
          {
            vtkMRMLFiducialListNode* correspondingFiducialList = reinterpret_cast<vtkMRMLFiducialListNode*>(this->ListOfFiducialLists->GetItemAsObject(i));
            this->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(correspondingFiducialList->GetID());
          }
           else
          {
            AddPairModelFiducial();
          }
         }
     }
    }


  if(this->backOriginalButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
      if(this->ParentTransformationNode && this->modelBackOSelector && this->modelBackOSelector->GetSelected())
     {
       vtkMRMLModelNode* BackOriginalModel = vtkMRMLModelNode::SafeDownCast(this->modelBackOSelector->GetSelected());
       if(BackOriginalModel != NULL)
         {
           BackModelToOriginalPosition(this->ParentTransformationNode, BackOriginalModel);
         }
     }
    }

  if (this->modelBackOSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
      vtkMRMLModelNode* BackOriginalModel = vtkMRMLModelNode::SafeDownCast(this->modelBackOSelector->GetSelected());
      if(BackOriginalModel != NULL)
     {
       // Check if parent transform is present
       vtkMRMLTransformNode *transformationNode = BackOriginalModel->GetParentTransformNode();
       if(transformationNode != NULL)
         {
           this->ParentTransformationNode = vtkMRMLLinearTransformNode::SafeDownCast(transformationNode);
         }
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

  if(this->MRMLScene == vtkMRMLScene::SafeDownCast(caller) 
     && event == vtkMRMLScene::NodeRemovedEvent)
    {
      // Check vtkCollections are still synchronized
      if(this->ListOfModels->GetNumberOfItems() == this->ListOfFiducialLists->GetNumberOfItems())
     {
       for(int i = 0; i < this->ListOfModels->GetNumberOfItems(); i++)
         {
           // Check which node has been removed;
           if(this->ListOfModels->GetItemAsObject(i) == callData || this->ListOfFiducialLists->GetItemAsObject(i) == callData)
          {
            this->ListOfModels->RemoveItem(i);
            this->ListOfFiducialLists->RemoveItem(i);
          }
         }
     }
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
  conBrowsFrame->SetLabelText("Clipping Frame");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Clipping frame

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

  // Cutter thickness
  this->cutterThicknessSelector = vtkKWMenuButtonWithLabel::New();
  this->cutterThicknessSelector->SetParent(frame2);
  this->cutterThicknessSelector->Create();
  this->cutterThicknessSelector->SetLabelText("Cutter thickness:");
  this->cutterThicknessSelector->GetWidget()->GetMenu()->AddRadioButton("5 mm");
  this->cutterThicknessSelector->GetWidget()->GetMenu()->AddRadioButton("10 mm");
  this->cutterThicknessSelector->GetWidget()->GetMenu()->SelectItem(0);

  // Enable / Disable
  this->EnableCutter = vtkKWCheckButtonWithLabel::New ( );
  this->EnableCutter->SetParent ( frame2 );
  this->EnableCutter->Create ( );
  this->EnableCutter->SetWidth (12);
  this->EnableCutter->SetLabelPositionToRight();
  this->EnableCutter->GetLabel()->SetText ("Enable Cutter");

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
            this->cutterThicknessSelector->GetWidgetName(),
            this->EnableCutter->GetWidgetName());


  vtkKWFrameWithLabel* ModelToCutFrame = vtkKWFrameWithLabel::New();
  ModelToCutFrame->SetParent(conBrowsFrame->GetFrame());
  ModelToCutFrame->Create();
  ModelToCutFrame->SetLabelText("Model to cut");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
           ModelToCutFrame->GetWidgetName() );


  this->ModelToCutSelector = vtkSlicerNodeSelectorWidget::New();
  this->ModelToCutSelector->SetParent(ModelToCutFrame->GetFrame());
  this->ModelToCutSelector->Create();
  this->ModelToCutSelector->SetWidth(30);
  this->ModelToCutSelector->SetNewNodeEnabled(0);
  this->ModelToCutSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->ModelToCutSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->ModelToCutSelector->UpdateMenu();

  app->Script("pack %s -fill x -side top -padx 2 -pady 2",
           this->ModelToCutSelector->GetWidgetName());

  this->PerformCutButton = vtkKWPushButton::New();
  this->PerformCutButton->SetParent(ModelToCutFrame->GetFrame());
  this->PerformCutButton->Create();
  this->PerformCutButton->SetText("Cut Model");

  app->Script("pack %s -fill x -side top -padx 2 -pady 2",
           this->PerformCutButton->GetWidgetName());
  
  vtkKWFrameWithLabel *ExtractingComponentsFrame = vtkKWFrameWithLabel::New();
  ExtractingComponentsFrame->SetParent(conBrowsFrame->GetFrame());
  ExtractingComponentsFrame->Create();
  ExtractingComponentsFrame->SetLabelText("Extracting Components");
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",       ExtractingComponentsFrame->GetWidgetName() );

  this->StartSelectingModelParts = vtkKWPushButton::New();
  this->StartSelectingModelParts->SetParent(ExtractingComponentsFrame->GetFrame());
  this->StartSelectingModelParts->Create();
  this->StartSelectingModelParts->SetText("Start Selecting Parts");

  this->StopSelectingModelParts = vtkKWPushButton::New();
  this->StopSelectingModelParts->SetParent(ExtractingComponentsFrame->GetFrame());
  this->StopSelectingModelParts->Create();
  this->StopSelectingModelParts->SetText("Stop Selecting Parts");

  app->Script("pack %s %s -fill x -side left -expand y -padx 2 -pady 2",
           this->StartSelectingModelParts->GetWidgetName(),
           this->StopSelectingModelParts->GetWidgetName());
  

  // -----------------------------------------
  // Moving Models frame

  vtkSlicerModuleCollapsibleFrame *moveModelsFrame = vtkSlicerModuleCollapsibleFrame::New();
  moveModelsFrame->SetParent(page);
  moveModelsFrame->Create();
  moveModelsFrame->SetLabelText("Moving models Frame");
  //moveModelsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            moveModelsFrame->GetWidgetName(), page->GetWidgetName());

  vtkKWFrameWithLabel *frame5 = vtkKWFrameWithLabel::New();
  frame5->SetParent(moveModelsFrame->GetFrame());
  frame5->Create();
  frame5->SetLabelText ("Move models");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
           frame5->GetWidgetName() );

  this->displayMoverButton = vtkKWPushButton::New();
  this->displayMoverButton->SetParent(frame5->GetFrame());
  this->displayMoverButton->Create();
  this->displayMoverButton->SetText("Display mover");

  app->Script("pack %s -fill x -side left -expand y -padx 2 -pady 2",
           this->displayMoverButton->GetWidgetName());

  // -----------------------------------------
  // Placing Markers Frame

  vtkSlicerModuleCollapsibleFrame *placeMarkersFrame = vtkSlicerModuleCollapsibleFrame::New();
  placeMarkersFrame->SetParent(page);
  placeMarkersFrame->Create();
  placeMarkersFrame->SetLabelText("Place Markers Frame");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            placeMarkersFrame->GetWidgetName(), page->GetWidgetName());


  vtkKWFrameWithLabel *frame6 = vtkKWFrameWithLabel::New();
  frame6->SetParent(placeMarkersFrame->GetFrame());
  frame6->Create();
  frame6->SetLabelText ("Place screws markers");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
           frame6->GetWidgetName() );

  this->modelSelector = vtkSlicerNodeSelectorWidget::New();
  this->modelSelector->SetParent(frame6->GetFrame());
  this->modelSelector->Create();
  this->modelSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->modelSelector->SetNewNodeEnabled(0);  
  this->modelSelector->SetMRMLScene(this->GetMRMLScene());
  this->modelSelector->UpdateMenu();

  this->placeMarkersButton = vtkKWPushButton::New();
  this->placeMarkersButton->SetParent(frame6->GetFrame());
  this->placeMarkersButton->Create();
  this->placeMarkersButton->SetText("Start Placing Markers");

  app->Script("pack %s %s -fill x -side top -expand y -padx 2 -pady 2",
           this->modelSelector->GetWidgetName(),
           this->placeMarkersButton->GetWidgetName());

  // -----------------------------------------
  // Back to original position Frame

  vtkSlicerModuleCollapsibleFrame *backOriginalFrame = vtkSlicerModuleCollapsibleFrame::New();
  backOriginalFrame->SetParent(page);
  backOriginalFrame->Create();
  backOriginalFrame->SetLabelText("Back to Original Position Frame");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
            backOriginalFrame->GetWidgetName(), page->GetWidgetName());


  vtkKWFrameWithLabel *frame7 = vtkKWFrameWithLabel::New();
  frame7->SetParent(backOriginalFrame->GetFrame());
  frame7->Create();
  frame7->SetLabelText ("Back to Original Position");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
           frame7->GetWidgetName() );

  this->modelBackOSelector = vtkSlicerNodeSelectorWidget::New();
  this->modelBackOSelector->SetParent(frame7->GetFrame());
  this->modelBackOSelector->Create();
  this->modelBackOSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->modelBackOSelector->SetNewNodeEnabled(0);  
  this->modelBackOSelector->SetMRMLScene(this->GetMRMLScene());
  this->modelBackOSelector->UpdateMenu();

  this->backOriginalButton = vtkKWPushButton::New();
  this->backOriginalButton->SetParent(frame7->GetFrame());
  this->backOriginalButton->Create();
  this->backOriginalButton->SetText("Back to Original Position");

  app->Script("pack %s %s -fill x -side top -expand y -padx 2 -pady 2",
           this->modelBackOSelector->GetWidgetName(),
           this->backOriginalButton->GetWidgetName());


  frame7->Delete(); 
  frame6->Delete();
  frame5->Delete();
  frame2->Delete();
  frame->Delete();
  ModelToCutFrame->Delete();
  ExtractingComponentsFrame->Delete();
  moveModelsFrame->Delete();
  conBrowsFrame->Delete();
  placeMarkersFrame->Delete();
  backOriginalFrame->Delete();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanGUI::UpdateAll()
{
}

void vtkOsteoPlanGUI::AddPairModelFiducial()
{
  if(this->modelSelector && this->modelSelector->GetSelected())
    {
      if(this->placeMarkerOn == true)
     {
       this->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);
       this->GetApplicationLogic()->GetInteractionNode()->SetSelected(1);
       this->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(1);

       this->placeMarkersButton->SetText("Stop Placing Markers");

       vtkMRMLModelNode* selectedModelNode = reinterpret_cast<vtkMRMLModelNode*>(this->modelSelector->GetSelected());
       this->modelNodeInsideCollection = false;
       int modelPosition = 0;

       for(int i = 0; i < this->ListOfModels->GetNumberOfItems();i++)
         {
           vtkMRMLModelNode* listModel = reinterpret_cast<vtkMRMLModelNode*>(this->ListOfModels->GetItemAsObject(i));
           if(!strcmp(selectedModelNode->GetID(),listModel->GetID()))
          {
            this->modelNodeInsideCollection = true;
            modelPosition = i;
          }
         }

       if(!this->modelNodeInsideCollection)
         {
           // Add Model to the List of models who have a fiducial list associated
           this->ListOfModels->AddItem(selectedModelNode);

           // Create the fiducial list with the name of the model
           vtkMRMLFiducialListNode* fiducialListConnectedToModel = vtkMRMLFiducialListNode::New();
           char fiducialListName[128];
           sprintf(fiducialListName,"%s-fiducialList",selectedModelNode->GetName());
           fiducialListConnectedToModel->SetName(fiducialListName);
            
           // Add Fiducial list to the list of fiducial list who have a model associated
           this->ListOfFiducialLists->AddItem(fiducialListConnectedToModel);

           // Add fiducial list to the scene and set it as active
           this->GetMRMLScene()->AddNode(fiducialListConnectedToModel);
           this->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(fiducialListConnectedToModel->GetID());
           fiducialListConnectedToModel->Delete();
         }
       else
         {
           // Set fiducial list corresponding to the model as active
           vtkMRMLFiducialListNode* fiducialListAlreadyConnectedToModel = reinterpret_cast<vtkMRMLFiducialListNode*>(this->ListOfFiducialLists->GetItemAsObject(modelPosition));
           this->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(fiducialListAlreadyConnectedToModel->GetID());
         }

     }
      else
     {
       this->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);
       this->GetApplicationLogic()->GetInteractionNode()->SetSelected(0);
       this->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(0);

       this->placeMarkersButton->SetText("Start Placing Markers");
     }
    }
 

}


void vtkOsteoPlanGUI::BackModelToOriginalPosition(vtkMRMLLinearTransformNode *ParentTransform, vtkMRMLModelNode* Model)
{
  // Get Tranformation Matrix
  vtkMatrix4x4* OriginalParentMatrix = ParentTransform->GetMatrixTransformToParent();
  
  // Create a Copy
  vtkMatrix4x4* CopiedParentMatrix = vtkMatrix4x4::New();
  CopiedParentMatrix->DeepCopy(OriginalParentMatrix);

  // Set Original to Identity
  OriginalParentMatrix->Identity();

  // Update Transform
  OriginalParentMatrix->Modified();

  // Invert Copied Matrix
  CopiedParentMatrix->Invert();

  // Create new transformation node
  vtkMRMLLinearTransformNode* FiducialTransform = vtkMRMLLinearTransformNode::New();
  FiducialTransform->SetAndObserveMatrixTransformToParent(CopiedParentMatrix);

  // Add it to the scene
  this->GetMRMLScene()->AddNode(FiducialTransform);

  // Update Transform Node
  FiducialTransform->Modified();
  
  // Check if model is in the list with fudicuals
  if(this->ListOfModels->GetNumberOfItems() == this->ListOfFiducialLists->GetNumberOfItems())
    {
      // Lists should be synchronized
      for(int i = 0; i < this->ListOfModels->GetNumberOfItems();i++)
     {
       vtkMRMLModelNode* listModel = vtkMRMLModelNode::SafeDownCast(this->ListOfModels->GetItemAsObject(i));
       if(listModel && listModel->GetID() == Model->GetID())
         {
           // The model is already in the list, which means fiducials should have been added on this model
           // Select the corresponding fiducial list
           vtkMRMLFiducialListNode* FiducialListModel = vtkMRMLFiducialListNode::SafeDownCast(this->ListOfFiducialLists->GetItemAsObject(i));
           if(FiducialListModel)
          {
            // Fiducial List found
            // Drop it under new transformation node (invert of the original)
            FiducialListModel->SetAndObserveTransformNodeID(FiducialTransform->GetID());
            FiducialListModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
            FiducialListModel->InvokeEvent(vtkMRMLScene::SceneEditedEvent);
          }
         }                                          
     }
    } 
   
  // Drop Inverted transformation under original transformation (Identity)
  FiducialTransform->SetAndObserveTransformNodeID(ParentTransform->GetID());

  // Update scene by invoking event
  FiducialTransform->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
  FiducialTransform->InvokeEvent(vtkMRMLScene::SceneEditedEvent);

  CopiedParentMatrix->Delete();
  FiducialTransform->Delete();
}
