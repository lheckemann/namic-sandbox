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

#include "vtkSurfaceTextureGUI.h"
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

#include "vtkCornerAnnotation.h"

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkImageData.h>
#include <vtkImageReader.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkDataSetAttributes.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkTubeFilter.h>

#include <vtkPolyDataNormals.h>
//#include <vtkDensifyPolyData.h>
#include <vtkCleanPolyData.h>

#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>


#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkKWRange.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSurfaceTextureGUI );
vtkCxxRevisionMacro ( vtkSurfaceTextureGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkSurfaceTextureGUI::vtkSurfaceTextureGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSurfaceTextureGUI::DataCallback);

  this->PointValue   = NULL;
  this->NeedModelUpdate = 1;

  this->Lower   = 0.0; 
  this->Upper   = 1.0; 
  this->Step    = 0.5; 
  this->Window  = 3000;
  this->Level   = 1000;

  this->PolyDataNormals = NULL;
  this->PolyData = NULL;
  this->Mapper   = NULL;
  this->Actor    = NULL;
  this->Renderer = NULL;
  
  //----------------------------------------------------------------
  // GUI widgets
  this->TestButton11 = NULL;
  this->TestButton12 = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;

  this->ModelSelectorWidget = NULL;
  this->VolumeSelectorWidget = NULL;

  this->WindowLevelRange = NULL;
  this->OuterLayerRange = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkSurfaceTextureGUI::~vtkSurfaceTextureGUI ( )
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

  if (this->TestButton11)
    {
    this->TestButton11->SetParent(NULL);
    this->TestButton11->Delete();
    }

  if (this->TestButton12)
    {
    this->TestButton12->SetParent(NULL);
    this->TestButton12->Delete();
    }

  if (this->TestButton21)
    {
    this->TestButton21->SetParent(NULL);
    this->TestButton21->Delete();
    }

  if (this->TestButton22)
    {
    this->TestButton22->SetParent(NULL);
    this->TestButton22->Delete();
    }

  if (this->ModelSelectorWidget)
    {
    this->ModelSelectorWidget->SetParent(NULL);
    this->ModelSelectorWidget->Delete();
    }
  if (this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget->SetParent(NULL);
    this->VolumeSelectorWidget->Delete();
    }
  if ( this->WindowLevelRange ) 
    {
    this->WindowLevelRange->SetParent(NULL);
    this->WindowLevelRange->Delete();
    this->WindowLevelRange = NULL;
    }
  if (this->OuterLayerRange)
    {
    this->OuterLayerRange->SetParent(NULL);
    this->OuterLayerRange->Delete();
    this->OuterLayerRange = NULL;
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::Enter()
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
void vtkSurfaceTextureGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "SurfaceTextureGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->TestButton11)
    {
    this->TestButton11
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton12)
    {
    this->TestButton12
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton21)
    {
    this->TestButton21
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton22)
    {
    this->TestButton22
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  if (this->ModelSelectorWidget)
    {
    this->ModelSelectorWidget
      ->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                        (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget
      ->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                        (vtkCommand *)this->GUICallbackCommand );
    }


  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::AddGUIObservers ( )
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

  this->TestButton11
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton12
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton21
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton22
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ModelSelectorWidget
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                  (vtkCommand *)this->GUICallbackCommand );
  this->VolumeSelectorWidget
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                  (vtkCommand *)this->GUICallbackCommand );

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkSurfaceTextureLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (this->TestButton11 == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    this->UpdateTexture();
    std::cerr << "TestButton11 is pressed." << std::endl;
    }
  else if (this->TestButton12 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton12 is pressed." << std::endl;
    }
  else if (this->TestButton21 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton21 is pressed." << std::endl;
    }
  else if (this->TestButton22 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton22 is pressed." << std::endl;
    }
  else if (this->ModelSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    vtkMRMLModelNode* node = 
      vtkMRMLModelNode::SafeDownCast(this->ModelSelectorWidget->GetSelected());
    if (node)
      {
      this->ModelNodeID = node->GetID();
      this->NeedModelUpdate = 1;
      }
    }
  else if (this->VolumeSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    vtkMRMLScalarVolumeNode* node = 
      vtkMRMLScalarVolumeNode::SafeDownCast(this->VolumeSelectorWidget->GetSelected());
    if (node)
      {
      this->VolumeNodeID = node->GetID();
      this->NeedModelUpdate = 1;
      }
    }
} 


void vtkSurfaceTextureGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkSurfaceTextureGUI *self = reinterpret_cast<vtkSurfaceTextureGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSurfaceTextureGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkSurfaceTextureLogic::SafeDownCast(caller))
    {
    if (event == vtkSurfaceTextureLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::ProcessTimerEvents()
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
void vtkSurfaceTextureGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "SurfaceTexture", "SurfaceTexture", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();
  BuildGUIForTestFrame2();

}


void vtkSurfaceTextureGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:SurfaceTexture</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "SurfaceTexture" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("SurfaceTexture");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 1");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Test child frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  // -----------------------------------------
  // Test push button

  this->ModelSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->ModelSelectorWidget->SetParent(frame->GetFrame());
  this->ModelSelectorWidget->Create();
  this->ModelSelectorWidget->SetNodeClass("vtkMRMLModelNode", NULL,
                                                  NULL, "Model");
  this->ModelSelectorWidget->SetNewNodeEnabled(1);
  this->ModelSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->ModelSelectorWidget->SetBorderWidth(2);
  this->ModelSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->ModelSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->ModelSelectorWidget->SetLabelText( "Model: ");
  this->ModelSelectorWidget->SetBalloonHelpString("Select a Model from the current scene.");

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ModelSelectorWidget->GetWidgetName());

  this->VolumeSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->VolumeSelectorWidget->SetParent(frame->GetFrame());
  this->VolumeSelectorWidget->Create();
  this->VolumeSelectorWidget->SetNodeClass("vtkMRMLScalarVolumeNode", NULL,
                                                  NULL, "Volume");
  this->VolumeSelectorWidget->SetNewNodeEnabled(1);
  this->VolumeSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->VolumeSelectorWidget->SetBorderWidth(2);
  this->VolumeSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->VolumeSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->VolumeSelectorWidget->SetLabelText( "Volume: ");
  this->VolumeSelectorWidget->SetBalloonHelpString("Select a volume from the current scene.");

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->VolumeSelectorWidget->GetWidgetName());

  this->WindowLevelRange = vtkKWRange::New();
  this->WindowLevelRange->SetParent(frame->GetFrame());
  this->WindowLevelRange->Create();
  this->WindowLevelRange->SymmetricalInteractionOn();
  this->WindowLevelRange->EntriesVisibilityOn ();
  this->WindowLevelRange->SetCommand(this, "ProcessWindowLevelCommand");
  this->WindowLevelRange->SetWholeRange(0, 5000);
  this->WindowLevelRange->SetRange(this->Level - this->Window/2, this->Level + this->Window/2);
  //this->WindowLevelRange->SetStartCommand(this, "ProcessWindowLevelStartCommand");
  //this->WindowLevelRange->SetEndCommand(this, "ProcessWindowLevelEndCommand");
  this->WindowLevelRange->SetEntriesCommand(this, "ProcessWindowLevelCommand");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->WindowLevelRange->GetWidgetName());

  this->OuterLayerRange = vtkKWRange::New();
  this->OuterLayerRange->SetParent(frame->GetFrame());
  this->OuterLayerRange->Create();
  this->OuterLayerRange->SymmetricalInteractionOn();
  this->OuterLayerRange->EntriesVisibilityOn ();
  //this->OuterLayerRange->SetCommand(this, "ProcessOuterLayerCommand");
  this->OuterLayerRange->SetWholeRange(-20.0, 20.0);
  this->OuterLayerRange->SetRange(0.0, 1.0);
  //this->OuterLayerRange->SetStartCommand(this, "ProcessOuterLayerStartCommand");
  this->OuterLayerRange->SetEndCommand(this, "ProcessOuterLayerCommand");
  this->OuterLayerRange->SetEntriesCommand(this, "ProcessOuterLayerCommand");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->OuterLayerRange->GetWidgetName());

  this->TestButton11 = vtkKWPushButton::New ( );
  this->TestButton11->SetParent ( frame->GetFrame() );
  this->TestButton11->Create ( );
  this->TestButton11->SetText ("Test 11");
  this->TestButton11->SetWidth (12);

  this->TestButton12 = vtkKWPushButton::New ( );
  this->TestButton12->SetParent ( frame->GetFrame() );
  this->TestButton12->Create ( );
  this->TestButton12->SetText ("Tset 12");
  this->TestButton12->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->TestButton11->GetWidgetName(),
               this->TestButton12->GetWidgetName());

  conBrowsFrame->Delete();
  frame->Delete();

}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("SurfaceTexture");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 2");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Test child frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  
  // -----------------------------------------
  // Test push button

  this->TestButton21 = vtkKWPushButton::New ( );
  this->TestButton21->SetParent ( frame->GetFrame() );
  this->TestButton21->Create ( );
  this->TestButton21->SetText ("Test 21");
  this->TestButton21->SetWidth (12);

  this->TestButton22 = vtkKWPushButton::New ( );
  this->TestButton22->SetParent ( frame->GetFrame() );
  this->TestButton22->Create ( );
  this->TestButton22->SetText ("Tset 22");
  this->TestButton22->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->TestButton21->GetWidgetName(),
               this->TestButton22->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
}


//----------------------------------------------------------------------------
void vtkSurfaceTextureGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
void vtkSurfaceTextureGUI::ProcessWindowLevelCommand(double min, double max)
{
  this->Window = max-min;
  this->Level  = (max+min)/2;
  UpdateTexture();
}

//----------------------------------------------------------------------------
void vtkSurfaceTextureGUI::ProcessOuterLayerCommand(double min, double max)
{
  this->Lower = min;
  this->Upper = max;
  this->NeedModelUpdate = 1;
  UpdateTexture();
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureGUI::UpdateTexture()
{

  vtkMRMLScene* scene  = this->GetMRMLScene();

  if (!scene)
    {
    std::cerr << "MRML scene node was not found." << std::endl;
    return;
    }

  vtkMRMLScalarVolumeNode* vnode
    = vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(this->VolumeNodeID.c_str()));
  if (!vnode)
    {
    std::cerr << "Volume node was not found." << std::endl;
    return;
    }

  vtkMRMLModelNode* mnode
    = vtkMRMLModelNode::SafeDownCast(scene->GetNodeByID(this->ModelNodeID.c_str()));
  if (!mnode)
    {
    std::cerr << "Volume node was not found." << std::endl;
    return;
    }

  vtkPolyData* poly = mnode->GetPolyData();

  // Calculate normals
  if (!this->PolyDataNormals)
    {
    this->PolyDataNormals = vtkPolyDataNormals::New();
    }

  //polyDataNormals->SetInputConnection(reader->GetOutputPort());
  PolyDataNormals->SetInput(poly);
  PolyDataNormals->SplittingOn();
  PolyDataNormals->SetFeatureAngle(170.0);  // Need to be examined
  PolyDataNormals->Update();
  
  this->PolyData = this->PolyDataNormals->GetOutput();
  int n = this->PolyData->GetNumberOfPoints();

  vtkSmartPointer<vtkUnsignedCharArray> colors =
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetName("Colors");
  colors->SetNumberOfComponents(3);
  colors->SetNumberOfTuples(n);

  const double low   = this->Level - this->Window/2.0;
  const double scale = 255.0 / this->Window;

  vtkSmartPointer<vtkDataArray> norms = this->PolyData->GetPointData()->GetNormals();

  if (!this->PointValue)
    {
    this->PointValue = vtkDoubleArray::New();
    this->NeedModelUpdate = 1;
    }

  if (this->NeedModelUpdate)
    {
    this->PointValue->Reset();
    }

  for (int i = 0; i < n; i ++)
    {
    if (this->NeedModelUpdate)
      {
      // Get coordinate
      double x[3];
      this->PolyData->GetPoint(i, x);
      
      // Get normal
      double n[3];
      norms->GetTuple(i, n);
      
      // Calculate projection
      double sum = 0;
      int nstep = 0;
      for (double d = this->Lower; d <= this->Upper; d += this->Step)
        {
        double p[3];
        p[0] = x[0] + n[0]*d;
        p[1] = x[1] + n[1]*d;
        p[2] = x[2] + n[2]*d;
        sum += TrilinearInterpolation(vnode, p);
        nstep ++;
        }
      double value = sum / (double)nstep;
      this->PointValue->InsertValue(i, value);
      }
      
    double intensity = (this->PointValue->GetValue(i) - low) * scale;
    if (intensity > 255.0)
      {
      intensity = 255.0;
      }
    else if (intensity < 0.0)
      {
      intensity = 0.0;
      }
    unsigned char cv = (unsigned char) intensity;

    colors->InsertTuple3(i, cv, cv, cv);
    }

  this->NeedModelUpdate = 0;
  this->PolyData->GetPointData()->SetScalars(colors);
  this->PolyData->Update();

  if (!this->Mapper)
    {
    this->Mapper = vtkPolyDataMapper::New();
    this->Mapper->SetInput(this->PolyData);
    this->Mapper->SetScalarModeToUsePointFieldData();
    this->Mapper->SelectColorArray("Colors");
    }
  this->Mapper->ScalarVisibilityOn();
  this->Mapper->Update();

  if (!this->Actor)
    {
    this->Actor = vtkActor::New();
    this->Actor->SetMapper(this->Mapper);
    }

  vtkSlicerViewerWidget *viewer_widget = this->GetApplicationGUI()->GetActiveViewerWidget();
  if (viewer_widget)
    {
    if (!this->Renderer)
      {
      this->Renderer = viewer_widget->GetMainViewer()->GetRenderer();
      this->Renderer->AddActor(this->Actor);
      }
    viewer_widget->RequestRender();
    }


}


double vtkSurfaceTextureGUI::TrilinearInterpolation(vtkMRMLVolumeNode * vnode, double x[3])
{
  int    dim[3];
  int    ijk[3];
  double r[3];

  if (!vnode)
    {
    return 0;
    }

  vtkImageData * image = vnode->GetImageData();
  if (!image)
    {
    return 0;
    }
  image->GetDimensions(dim);

  vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vnode->GetRASToIJKMatrix(RASToIJKMatrix);
  double in[4];
  double out[4];

  in[0] = x[0];
  in[1] = x[1];
  in[2] = x[2];
  in[3] = 1.0;
  RASToIJKMatrix->MultiplyPoint(in, out);

  ijk[0] = (int)out[0];
  ijk[1] = (int)out[1];
  ijk[2] = (int)out[2];

  r[0] = out[0] - (double)ijk[0];
  r[1] = out[1] - (double)ijk[1];
  r[2] = out[2] - (double)ijk[2];

  //if (spoints->ComputeStructuredCoordinates(x, ijk, r) == 0)
  if (ijk[0] < 0 || ijk[0] >= dim[0] ||
      ijk[1] < 0 || ijk[1] >= dim[1] ||
      ijk[2] < 0 || ijk[2] >= dim[2])
    {
    std::cerr << "Point (" << x[0] << ", " << x[1] << ", " << x[2]
              << ") is outside of the volume." << std::endl;
    return 0.0;
    }
  
  int ii = ijk[0];    int jj = ijk[1];    int kk = ijk[2];
  double v000 = (double)*((short*)image->GetScalarPointer(ii,   jj,   kk  ));
  double v100 = (double)*((short*)image->GetScalarPointer(ii+1, jj,   kk  ));
  double v010 = (double)*((short*)image->GetScalarPointer(ii,   jj+1, kk  ));
  double v110 = (double)*((short*)image->GetScalarPointer(ii+1, jj+1, kk  ));
  double v001 = (double)*((short*)image->GetScalarPointer(ii,   jj,   kk+1));
  double v101 = (double)*((short*)image->GetScalarPointer(ii+1, jj,   kk+1));
  double v011 = (double)*((short*)image->GetScalarPointer(ii,   jj+1, kk+1));
  double v111 = (double)*((short*)image->GetScalarPointer(ii+1, jj+1, kk+1));
  
  double r0 = r[0]; double r1 = r[1]; double r2 = r[2];
  double value =
    v000 * (1-r0) * (1-r1) * (1-r2) +
    v100 *    r0  * (1-r1) * (1-r2) +
    v010 * (1-r0) *    r1  * (1-r2) +
    v001 * (1-r0) * (1-r1) *    r2  +
    v101 *    r0  * (1-r1) *    r2  +
    v011 * (1-r0) *    r1  *    r2  +
    v110 *    r0  *    r1  * (1-r2) +
    v111 *    r0  *    r1  *    r2;

  return value;

}
