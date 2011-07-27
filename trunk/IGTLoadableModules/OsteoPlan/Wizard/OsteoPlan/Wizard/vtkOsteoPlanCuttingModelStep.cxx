/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkOsteoPlanCuttingModelStep.h"

#include "vtkObject.h"

#include "vtkOsteoPlanGUI.h"
#include "vtkOsteoPlanLogic.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkSlicerTheme.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWPushButton.h"

#include "vtkBoxWidget2.h"
#include "vtkBoxRepresentation.h"
#include "vtkProperty.h"
#include "vtkPlanes.h"
#include "vtkPlane.h"
#include "vtkClipPolyData.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkPolyDataMapper.h"
#include "vtkAppendPolyData.h"
#include "vtkLoopSubdivisionFilter.h"

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanCuttingModelStep);
vtkCxxRevisionMacro(vtkOsteoPlanCuttingModelStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkOsteoPlanCuttingModelStep::vtkOsteoPlanCuttingModelStep()
{
  this->SetTitle("Cut");
  this->SetDescription("Use cutter to cut jaw.");

  this->MainFrame=NULL;
  
  this->TitleBackgroundColor[0] = 0.68;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1;

  this->ProcessingCallback = false;

  //-- -- -- -- -- -- -- -- -- -- -- -- -- --
  //               Display Frame            
  //-- -- -- -- -- -- -- -- -- -- -- -- -- --
  this->DisplayFrame = NULL;

  this->TogglePlaneButton = NULL;

  this->CuttingPlane = NULL;
  this->CuttingPlaneRepresentation = NULL;
  this->CutterAlreadyCreatedOnce = false;

  this->NextDisplayCutterStatus = false;

  //-- -- -- -- -- -- -- -- -- -- -- -- -- --
  //               Cut Frame            
  //-- -- -- -- -- -- -- -- -- -- -- -- -- --

  this->CutFrame = NULL;

  this->ModelToCutSelector = NULL;
  this->ModelToCut = NULL;

  this->ApplyCutButton = NULL;

  this->ModelSelected = false;
}

//----------------------------------------------------------------------------
vtkOsteoPlanCuttingModelStep::~vtkOsteoPlanCuttingModelStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(DisplayFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TogglePlaneButton);

  if(this->CuttingPlane)
    {
    this->CuttingPlane->Delete();
    this->CuttingPlane = NULL;
    }

  if(this->CuttingPlaneRepresentation)
    {
    this->CuttingPlaneRepresentation->Delete();
    this->CuttingPlaneRepresentation = NULL;
    }

  DELETE_IF_NULL_WITH_SETPARENT_NULL(CutFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ModelToCutSelector);
 
  if(this->ApplyCutButton)
    {
    this->ApplyCutButton->Delete();
    this->ApplyCutButton = NULL;
    }

}

//----------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();


  vtkKWWidget* parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  //-------------------------------------------------------
  //                     Display Frame

  if(!this->DisplayFrame)
    {
    this->DisplayFrame = vtkKWFrameWithLabel::New();
    this->DisplayFrame->SetParent( parent );
    this->DisplayFrame->Create();
    this->DisplayFrame->SetLabelText("Display cutter");
    }
  
  this->Script("pack %s -side top -fill x -anchor nw -padx 0 -pady 2",
           this->DisplayFrame->GetWidgetName());
  
  if(!this->TogglePlaneButton)
    {
    this->TogglePlaneButton = vtkKWPushButton::New();
    }
  if(!this->TogglePlaneButton->IsCreated())
    {
    this->TogglePlaneButton->SetParent(this->DisplayFrame->GetFrame());
    this->TogglePlaneButton->Create();
    this->TogglePlaneButton->SetBackgroundColor(color->SliceGUIGreen);
    this->TogglePlaneButton->SetActiveBackgroundColor(color->SliceGUIGreen);
    this->TogglePlaneButton->SetText("Show cutter");
    }
   
  this->Script("pack %s -side top -fill x -padx 0 -pady 2",
           this->TogglePlaneButton->GetWidgetName());

  //-------------------------------------------------------


  //-------------------------------------------------------
  //-------------------------------------------------------
  //                       Cut Frame 
  if(!this->CutFrame)
    {
    this->CutFrame = vtkKWFrameWithLabel::New();
    this->CutFrame->SetParent( parent );
    this->CutFrame->Create();
    this->CutFrame->SetLabelText("Apply cut");
    }

  this->Script("pack %s -side top -fill x -anchor nw -padx 0 -pady 2",
           this->CutFrame->GetWidgetName());

  if(!this->ModelToCutSelector)
    {
    this->ModelToCutSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->ModelToCutSelector->IsCreated())
    {
    this->ModelToCutSelector->SetParent(this->CutFrame->GetFrame());
    this->ModelToCutSelector->Create();
    this->ModelToCutSelector->SetNewNodeEnabled(0);
    this->ModelToCutSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
    this->ModelToCutSelector->SetMRMLScene(this->GetLogic()->GetMRMLScene());
    this->ModelToCutSelector->UpdateMenu();
    }


  if(!this->ApplyCutButton)
    {
    this->ApplyCutButton = vtkKWPushButton::New();
    }
  if(!this->ApplyCutButton->IsCreated())
    {
    this->ApplyCutButton->SetParent(this->CutFrame->GetFrame());
    this->ApplyCutButton->Create();
    this->ApplyCutButton->SetBackgroundColor(color->SliceGUIGreen);
    this->ApplyCutButton->SetActiveBackgroundColor(color->SliceGUIGreen);
    this->ApplyCutButton->SetText("Select Model");
    }
   
  this->Script("pack %s %s -side top -fill x -padx 0 -pady 2",
           this->ModelToCutSelector->GetWidgetName(),
               this->ApplyCutButton->GetWidgetName());

  //-------------------------------------------------------

  CreateCutter();

  this->CutterAlreadyCreatedOnce = true;

  this->AddGUIObservers();
  
  UpdateGUI();

}

//----------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::ProcessGUIEvents(vtkObject *caller,
                                                    unsigned long event, void *callData)
{
  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  if(this->TogglePlaneButton == vtkKWPushButton::SafeDownCast(caller) &&
     event == vtkKWPushButton::InvokedEvent)
    {

    if(!this->NextDisplayCutterStatus)
      {
      // Change button
      this->TogglePlaneButton->SetBackgroundColor(color->LightestRed);
      this->TogglePlaneButton->SetActiveBackgroundColor(color->LightestRed);
      this->TogglePlaneButton->SetText("Hide cutter");

      // Show cutter
      this->CuttingPlane->On();
      this->CuttingPlane->GetRepresentation()->SetVisibility(1);
      this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();

      // Update status
      this->NextDisplayCutterStatus = true;
      }
    else
      {
      // Change button
      this->TogglePlaneButton->SetBackgroundColor(color->SliceGUIGreen);
      this->TogglePlaneButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->TogglePlaneButton->SetText("Show cutter");

      // Hide cutter
      this->CuttingPlane->Off();
      this->CuttingPlane->GetRepresentation()->SetVisibility(0);
      this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();

      // Update status
      this->NextDisplayCutterStatus = false;
      }
    }


  if(this->ModelToCutSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
     event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    this->ModelToCut = vtkMRMLModelNode::SafeDownCast(this->ModelToCutSelector->GetSelected());
    if(this->ModelToCut)
      {
      std::string name;
      name = this->ModelToCut->GetName();
      std::string buttonname = "Apply Cut to " + name;
      this->ApplyCutButton->SetText(buttonname.c_str());

      this->ModelSelected = true;
      }
    }

  if(this->ApplyCutButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    
    // Check if model is selected and cutter is displayed
    if(this->ModelToCut && this->ModelSelected && this->CuttingPlane && this->NextDisplayCutterStatus)
      {
      this->ApplyCutButton->SetBackgroundColor(color->SliceGUIYellow);
      this->ApplyCutButton->SetActiveBackgroundColor(color->SliceGUIYellow);
      this->ApplyCutButton->SetText("Processing, please wait...");
      
      ClipModel(this->ModelToCut, this->CuttingPlane);

      std::string name;
      name = this->ModelToCut->GetName();
      std::string buttonname = "Apply Cut to " + name;

      this->ApplyCutButton->SetBackgroundColor(color->SliceGUIGreen);
      this->ApplyCutButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->ApplyCutButton->SetText(buttonname.c_str());

      this->GetGUI()->GetWizardWidget()->GetWizardWorkflow()->AttemptToGoToNextStep();
      }
    }


}


//-----------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if(this->TogglePlaneButton)
    {
    this->TogglePlaneButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->ModelToCutSelector)
    {
    this->ModelToCutSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->ApplyCutButton)
    {
    this->ApplyCutButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

}
//-----------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::RemoveGUIObservers()
{
  if(this->TogglePlaneButton)
    {
    this->TogglePlaneButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->ModelToCutSelector)
    {
    this->ModelToCutSelector->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->ApplyCutButton)
    {
    this->ApplyCutButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
}

//--------------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::UpdateGUI()
{

}

//----------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();

  if(this->CuttingPlane)
    {
      // Hide cutter when leaving step
      vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
      vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

      this->CuttingPlane->Off();
      this->CuttingPlane->GetRepresentation()->SetVisibility(0);
      this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();

      this->TogglePlaneButton->SetBackgroundColor(color->SliceGUIGreen);
      this->TogglePlaneButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->TogglePlaneButton->SetText("Show cutter");

      this->NextDisplayCutterStatus = false;
    }
}

//----------------------------------------------------------------------------
void vtkOsteoPlanCuttingModelStep::TearDownGUI()
{  
  RemoveGUIObservers();
}

void vtkOsteoPlanCuttingModelStep::CreateCutter()
{
  if(!this->CuttingPlane)
    {
    this->CuttingPlane = vtkBoxWidget2::New();
    if(!this->CuttingPlaneRepresentation)
      {
      this->CuttingPlaneRepresentation = vtkBoxRepresentation::New();
      }
    }

  if(this->CuttingPlane && this->CuttingPlaneRepresentation && !this->CutterAlreadyCreatedOnce)
    {
    this->CuttingPlaneRepresentation->GetFaceProperty()->SetRepresentationToSurface();
    this->CuttingPlaneRepresentation->GetSelectedHandleProperty()->SetColor(0,0,1);
    this->CuttingPlaneRepresentation->BuildRepresentation();

    this->CuttingPlane->SetRepresentation(this->CuttingPlaneRepresentation);

    this->CuttingPlane->SetDefaultRenderer(this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
    this->CuttingPlane->SetCurrentRenderer(this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
    this->CuttingPlane->SetInteractor(this->GetGUI()->GetApplicationGUI()->GetActiveRenderWindowInteractor());
 
    double PlanePosition[6] = {-50,50,-50,50,0,1};
    this->CuttingPlane->GetRepresentation()->PlaceWidget(PlanePosition);
    this->CuttingPlane->GetRepresentation()->SetVisibility(0);
    this->CuttingPlane->On();
    }
}




// Note:
// If not pre-clipping, polygons bigger than the box are not clipped

// Method:
// We first clip with the top plane to cut in two pieces (polygons intersecting with the planes are cutted)
// Then we paste (append) these two parts to recreate the model (but with polygons cutted on the intersection line)
// We do the same thing with second plane
// We apply a subdivision filter to reduce polygons size
// And we finally apply final cutting

// Options:
// firstAppend and secondAppend could be displayed to easily understand the process (switch for wireframe mode on Slicer once process is over)

void vtkOsteoPlanCuttingModelStep::ClipModel(vtkMRMLModelNode* model, vtkBoxWidget2* cuttingBox)
{
  // Get Planes from vtkBoxWidget  
  vtkPlanes* planes = vtkPlanes::New();
  vtkBoxRepresentation* boxRepresentation = reinterpret_cast<vtkBoxRepresentation*>(cuttingBox->GetRepresentation());
  boxRepresentation->GetPlanes(planes);

  // Get Planes to pre-clip
  vtkPlane* bottomPlane = vtkPlane::New();
  planes->GetPlane(4, bottomPlane);
  vtkPlane* topPlane = vtkPlane::New();
  planes->GetPlane(5, topPlane);

  // Pre-clip with the first plane
  vtkClipPolyData* polyCutter = vtkClipPolyData::New();
  polyCutter->GenerateClippedOutputOn();
  polyCutter->GenerateClipScalarsOn();
  polyCutter->SetClipFunction(bottomPlane);
  polyCutter->SetInput(model->GetPolyData());

  vtkAppendPolyData* firstAppend = vtkAppendPolyData::New();
  firstAppend->AddInput(polyCutter->GetOutput());
  firstAppend->AddInput(polyCutter->GetClippedOutput());
 
  vtkClipPolyData* polyCutter2 = vtkClipPolyData::New();
  polyCutter2->GenerateClippedOutputOn();
  polyCutter2->GenerateClipScalarsOn();
  polyCutter2->SetClipFunction(topPlane);
  polyCutter2->SetInput(firstAppend->GetOutput());
    
  vtkAppendPolyData* secondAppend = vtkAppendPolyData::New();
  secondAppend->AddInput(polyCutter2->GetOutput());
  secondAppend->AddInput(polyCutter2->GetClippedOutput());
   
  vtkLoopSubdivisionFilter* subdividePolygons = vtkLoopSubdivisionFilter::New();
  subdividePolygons->SetInput(secondAppend->GetOutput());
  subdividePolygons->SetNumberOfSubdivisions(1);
  subdividePolygons->GetOutput()->Squeeze();
      
  vtkClipPolyData* realCut = vtkClipPolyData::New();
  realCut->GenerateClippedOutputOn();
  realCut->SetClipFunction(planes);
  realCut->SetInput(subdividePolygons->GetOutput());
  
  // Model 1
  vtkMRMLModelNode* part1 = vtkMRMLModelNode::New();
  part1->SetScene(this->GetLogic()->GetMRMLScene());
  part1->SetName("Inside");
  part1->SetAndObservePolyData(realCut->GetClippedOutput());             // Replace realCut->GetClippedOutput() by firstAppend->GetOutput() to visualize firstAppend
  part1->SetModifiedSinceRead(1);
  part1->GetPolyData()->Squeeze();
  this->GetLogic()->GetMRMLScene()->AddNode(part1);

  vtkMRMLModelDisplayNode* dnode1 = vtkMRMLModelDisplayNode::New();
  dnode1->SetPolyData(part1->GetPolyData());
  dnode1->SetColor(0.85,0.85,0.85);
  dnode1->SetBackfaceCulling(1);
  dnode1->SetVisibility(0);
  this->GetLogic()->GetMRMLScene()->AddNode(dnode1);

  part1->SetAndObserveDisplayNodeID(dnode1->GetID());

  // Model 2  
  vtkMRMLModelNode* part2 = vtkMRMLModelNode::New();
  part2->SetScene(this->GetLogic()->GetMRMLScene());
  part2->SetName("Outside");
  part2->SetAndObservePolyData(realCut->GetOutput());       // Replace realCut->GetClippedOutput() by secondAppend->GetOutput() to visualize secondAppend
  part2->SetModifiedSinceRead(1);
  part2->GetPolyData()->Squeeze();
  this->GetLogic()->GetMRMLScene()->AddNode(part2);

  vtkMRMLModelDisplayNode* dnode2 = vtkMRMLModelDisplayNode::New();      
  dnode2->SetPolyData(part2->GetPolyData());
  dnode2->SetColor(0.15,0.15,0.15);
  dnode2->SetBackfaceCulling(1);
  dnode2->SetVisibility(1);
  this->GetLogic()->GetMRMLScene()->AddNode(dnode2);
  
  part2->SetAndObserveDisplayNodeID(dnode2->GetID());
  
  // Hide original model
  model->GetModelDisplayNode()->SetVisibility(0);

  // Delete
  subdividePolygons->Delete();  
  bottomPlane->Delete();
  topPlane->Delete();
  polyCutter->Delete();
  polyCutter2->Delete();
  firstAppend->Delete();
  secondAppend->Delete();
  realCut->Delete();
  planes->Delete();
  dnode1->Delete();
  dnode2->Delete();
  part1->Delete();
  part2->Delete();

}

