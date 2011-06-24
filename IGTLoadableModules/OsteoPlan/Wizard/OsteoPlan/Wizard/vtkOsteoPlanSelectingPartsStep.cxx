/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkOsteoPlanSelectingPartsStep.h"

#include "vtkObject.h"

#include "vtkOsteoPlanGUI.h"
#include "vtkOsteoPlanLogic.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkSlicerTheme.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
#include "vtkKWEntryWithLabel.h"

#include "vtkCellPicker.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyData.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkStringArray.h"

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanSelectingPartsStep);
vtkCxxRevisionMacro(vtkOsteoPlanSelectingPartsStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkOsteoPlanSelectingPartsStep::vtkOsteoPlanSelectingPartsStep()
{
  this->SetTitle("Select Parts");
  this->SetDescription("Select different parts to create a new model for each part");

  this->MainFrame=NULL;
  this->SelectFrame = NULL;
  this->InputModelLabel = NULL;  
  this->InputModelSelector = NULL;
  this->InputModel = NULL;
  this->SelectPartButton = NULL;

  this->SelectingPart = false;

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1;

  this->ProcessingCallback = false;

  this->ColorName = NULL;

  this->ColorNumber = 0;
}

//----------------------------------------------------------------------------
vtkOsteoPlanSelectingPartsStep::~vtkOsteoPlanSelectingPartsStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(SelectFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(InputModelLabel);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(InputModelSelector);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(SelectPartButton);

  if(this->ColorName)
    {
      this->ColorName->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWidget* parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  //-------------------------------------------------------
  //                     Select Parts Frame

  if(!this->SelectFrame)
    {
    this->SelectFrame = vtkKWFrameWithLabel::New();
    this->SelectFrame->SetParent( parent );
    this->SelectFrame->Create();
    this->SelectFrame->SetLabelText("Select Parts");
    }
  
  this->Script("pack %s -side top -fill x -anchor nw -padx 0 -pady 2",
           this->SelectFrame->GetWidgetName());

  if(!this->InputModelLabel)
    {
      this->InputModelLabel = vtkKWLabel::New();
    }
  if(!this->InputModelLabel->IsCreated())
    {
      this->InputModelLabel->SetParent(this->SelectFrame->GetFrame());
      this->InputModelLabel->Create();
      this->InputModelLabel->SetText("Input Model:");
      this->InputModelLabel->SetAnchorToWest();
    }

  if(!this->InputModelSelector)
    {
    this->InputModelSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->InputModelSelector->IsCreated())
    {
    this->InputModelSelector->SetParent( this->SelectFrame->GetFrame() );
    this->InputModelSelector->Create();
    this->InputModelSelector->SetNewNodeEnabled(0);
    this->InputModelSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
    this->InputModelSelector->SetMRMLScene(this->GetLogic()->GetMRMLScene());
    this->InputModelSelector->UpdateMenu();
    }

  if(!this->SelectPartButton)
    {
    this->SelectPartButton = vtkKWPushButton::New();
    }
  if(!this->SelectPartButton->IsCreated())
    {
    this->SelectPartButton->SetParent( this->SelectFrame->GetFrame() );
    this->SelectPartButton->Create();

    this->SelectPartButton->SetBackgroundColor(color->SliceGUIGreen);
    this->SelectPartButton->SetActiveBackgroundColor(color->SliceGUIGreen);
    this->SelectPartButton->SetText("Select Part");
    this->SelectPartButton->SetEnabled(1);
    }

  this->Script("pack %s %s %s -side top -fill x -padx 0 -pady 2",
         this->InputModelLabel->GetWidgetName(),
               this->InputModelSelector->GetWidgetName(),
         this->SelectPartButton->GetWidgetName());

  //-------------------------------------------------------
  // Show Original Model if existing

  if(this->InputModel)
    {
      this->InputModel->GetModelDisplayNode()->SetVisibility(1);
    }

  //-------------------------------------------------------
  // Colors
  if(!this->ColorName)
    {
      this->ColorName = vtkStringArray::New();
      this->ColorName->InsertNextValue("Blue");
      this->ColorName->InsertNextValue("Brown");
      this->ColorName->InsertNextValue("Green");
      this->ColorName->InsertNextValue("Ochre");
      this->ColorName->InsertNextValue("Orange");
      this->ColorName->InsertNextValue("Red");
      this->ColorName->InsertNextValue("Stone");

      this->colorId[0] = color->LightBlue;
      this->colorId[1] = color->LightBrown;
      this->colorId[2] = color->LightGreen;
      this->colorId[3] = color->LightOchre;
      this->colorId[4] = color->LightOrange;
      this->colorId[5] = color->LightRed;
      this->colorId[6] = color->LightStone;
    }

  //-------------------------------------------------------

  this->AddGUIObservers();
  
  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
  if(this->SelectingPart)
    {

      SelectClickedPart();

      vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
      vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

      this->SelectPartButton->SetBackgroundColor(color->SliceGUIGreen);
      this->SelectPartButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->SelectPartButton->SetText("Select Part");
      
      this->SelectingPart = false;
    }

}


//----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::ProcessGUIEvents(vtkObject *caller,
                                                      unsigned long event, void *callData)
{
  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    }
  else
    {
  if(this->InputModelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    this->InputModel = vtkMRMLModelNode::SafeDownCast(this->InputModelSelector->GetSelected());
    }

  if(this->SelectPartButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->InputModel)
      {
      vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
      vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

      this->SelectPartButton->SetBackgroundColor(color->SliceGUIYellow);
      this->SelectPartButton->SetActiveBackgroundColor(color->SliceGUIYellow);
      this->SelectPartButton->SetText("Selecting Part...");

      this->SelectingPart = true;
      }
    }
    }
}


//-----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::AddGUIObservers()
{
  this->RemoveGUIObservers();
 
 if(this->InputModelSelector)
    {
    this->InputModelSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->SelectPartButton)
    {
    this->SelectPartButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  this->GetGUI()->GetApplicationGUI()->GetActiveRenderWindowInteractor()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);


}
//-----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::RemoveGUIObservers()
{
  if(this->InputModelSelector)
    {
    this->InputModelSelector->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->SelectPartButton)
    {
    this->SelectPartButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

}

//--------------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::UpdateGUI()
{

}

//----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();

  // Hide Original Model when leaving step
  if(this->InputModel)
    {
      this->InputModel->GetModelDisplayNode()->SetVisibility(0);
    }
}

//----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::TearDownGUI()
{  
  RemoveGUIObservers();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanSelectingPartsStep::SelectClickedPart()
{  
  int* mousePosition = this->GetGUI()->GetApplicationGUI()->GetActiveRenderWindowInteractor()->GetEventPosition();
      
  vtkCellPicker* cellPicker = vtkCellPicker::New();
  cellPicker->Pick(mousePosition[0],mousePosition[1],0,this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
  int cellIdPicked = cellPicker->GetCellId();  
  if(cellIdPicked != -1)
    {
    vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
    connectivityFilter->SetInput(this->InputModel->GetModelDisplayNode()->GetPolyData());
    connectivityFilter->SetExtractionModeToCellSeededRegions();
    connectivityFilter->InitializeSeedList();
    connectivityFilter->AddSeed(cellIdPicked);
    connectivityFilter->Update();

    // Create new polydata 
    vtkPolyData* polyDataModel = vtkPolyData::New();
    polyDataModel->CopyStructure(this->InputModel->GetModelDisplayNode()->GetPolyData());

    // Create New vtkMRMLNode
    vtkMRMLModelNode* modelSelected = vtkMRMLModelNode::New();
 
    // Create New vtkMRMLModelDisplayNode
    vtkMRMLModelDisplayNode* dnodeS = vtkMRMLModelDisplayNode::New();
 
    // Add them to the scene 
    modelSelected->SetScene(this->GetGUI()->GetMRMLScene());
    modelSelected->SetName(this->ColorName->GetValue(this->ColorNumber%6).c_str());

    dnodeS->SetColor(this->colorId[this->ColorNumber%6]);
    dnodeS->SetScene(this->GetGUI()->GetMRMLScene());

    this->ColorNumber++;
 
    this->GetGUI()->GetMRMLScene()->AddNode(dnodeS);
    this->GetGUI()->GetMRMLScene()->AddNode(modelSelected);
 
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


