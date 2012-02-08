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
#include "vtkDecimatePro.h"
#include "vtkTriangleFilter.h"

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

  this->MainFrame          = NULL;
  this->SelectFrame        = NULL;
  this->InputModelLabel    = NULL;
  this->InputModelSelector = NULL;
  this->InputModel         = NULL;
  this->SelectPartButton   = NULL;

  this->SelectingPart           = false;

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1;

  this->ProcessingCallback = false;

  this->ColorName   = NULL;
  this->ColorNumber = 0;

  this->NumberOfPartsSelected = 0;
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

  vtkKWWidget           *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication  *app    = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor        *color  = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  // Select Parts Frame

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

  // Select black model if existing
  vtkCollection *BlackModel = this->GetGUI()->GetMRMLScene()->GetNodesByName("Black");
  if(this->InputModelSelector)
    {
    this->InputModelSelector->SetSelected(vtkMRMLModelNode::SafeDownCast(BlackModel->GetItemAsObject(BlackModel->GetNumberOfItems()-1)));
    this->InputModelSelector->InvokeEvent(vtkSlicerNodeSelectorWidget::NodeSelectedEvent);
    this->InputModel = vtkMRMLModelNode::SafeDownCast(this->InputModelSelector->GetSelected());
    }
  BlackModel->Delete();

  //-------------------------------------------------------
  // Show Original Model if existing

  if(this->InputModel)
    {
    this->InputModel->GetModelDisplayNode()->SetVisibility(1);
    }


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
    // Select part of the model clicked (see below)
    SelectClickedPart();

    vtkSlicerApplication *app   = vtkSlicerApplication::SafeDownCast(this->GetApplication());
    vtkSlicerColor       *color = app->GetSlicerTheme()->GetSlicerColors();

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

    //--------------------------------------------------
    // Model to select selected

    if(this->InputModelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
       && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
      {
      this->InputModel = vtkMRMLModelNode::SafeDownCast(this->InputModelSelector->GetSelected());
      }


    //--------------------------------------------------
    // Select Button

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
  if(this->InputModel && this->NumberOfPartsSelected)
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
// SelectClikedPart:
//  - Detect position click
//  - Use vtkPolyDataConnectivityFilter to select all the part connected to clicked spot
//  - Reduce mesh (has been increased when clipping, but slowing down system if too many meshes)
//  - Create a new model with this part
//  - Apply a color to this model and add it to the scene
void vtkOsteoPlanSelectingPartsStep::SelectClickedPart()
{
  char buf[32] = "Selecting Part...";
  this->GetGUI()->GetApplicationGUI()->SetExternalProgress(buf, 0.0);

  int* mousePosition = this->GetGUI()->GetApplicationGUI()->GetActiveRenderWindowInteractor()->GetEventPosition();

  vtkCellPicker* cellPicker = vtkCellPicker::New();
  cellPicker->Pick(mousePosition[0],mousePosition[1],0,this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
  int cellIdPicked = cellPicker->GetCellId();
  if(cellIdPicked != -1)
    {

    vtkTriangleFilter* triangleFilter = vtkTriangleFilter::New();
    triangleFilter->SetInput(this->InputModel->GetModelDisplayNode()->GetPolyData());

    this->GetGUI()->GetApplicationGUI()->SetExternalProgress(buf, 0.10);
    vtkPolyDataConnectivityFilter* connectivityFilter = vtkPolyDataConnectivityFilter::New();
    connectivityFilter->SetInput(triangleFilter->GetOutput());
    connectivityFilter->SetExtractionModeToCellSeededRegions();
    connectivityFilter->InitializeSeedList();
    connectivityFilter->AddSeed(cellIdPicked);
    connectivityFilter->Update();

    this->GetGUI()->GetApplicationGUI()->SetExternalProgress(buf, 0.70);

    // Reduce number of triangles (after inscreasing to cut) to increase Slicer speed
    vtkDecimatePro* decimateMesh = vtkDecimatePro::New();
    decimateMesh->SetInput((vtkDataObject*)connectivityFilter->GetOutput());
    decimateMesh->PreserveTopologyOn();
    decimateMesh->SetTargetReduction(0.3);

    this->GetGUI()->GetApplicationGUI()->SetExternalProgress(buf, 0.90);

    // Create New vtkMRMLNode
    vtkMRMLModelNode* modelSelected = vtkMRMLModelNode::New();
    modelSelected->SetScene(this->GetGUI()->GetMRMLScene());
    modelSelected->SetName(this->ColorName->GetValue(this->ColorNumber%6).c_str());
    modelSelected->SetAndObservePolyData(decimateMesh->GetOutput());
    modelSelected->SetModifiedSinceRead(1);
    modelSelected->GetPolyData()->Squeeze();
    this->GetGUI()->GetMRMLScene()->AddNode(modelSelected);

    // Create New vtkMRMLModelDisplayNode
    vtkMRMLModelDisplayNode* dnodeS = vtkMRMLModelDisplayNode::New();
    dnodeS->SetPolyData(modelSelected->GetPolyData());
    dnodeS->SetColor(this->colorId[this->ColorNumber%6]);
    dnodeS->SetVisibility(1);
    this->GetGUI()->GetMRMLScene()->AddNode(dnodeS);

    modelSelected->SetAndObserveDisplayNodeID(dnodeS->GetID());

    this->ColorNumber++;

    this->GetGUI()->GetApplicationGUI()->SetExternalProgress(buf, 1.0);

    // Clean
    triangleFilter->Delete();
    decimateMesh->Delete();
    modelSelected->Delete();
    dnodeS->Delete();
    connectivityFilter->Delete();

    this->NumberOfPartsSelected++;
    }
  cellPicker->Delete();
}


