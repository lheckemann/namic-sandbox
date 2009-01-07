/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkControl4DGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkMRMLColorNode.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWScaleWithEntry.h"
#include "vtkKWScale.h"
#include "vtkKWPushButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWSpinBox.h"
#include "vtkKWCanvas.h"

#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWPiecewiseFunctionEditor.h"
#include "vtkPiecewiseFunction.h"
#include "vtkDoubleArray.h"

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkControl4DGUI );
vtkCxxRevisionMacro ( vtkControl4DGUI, "$Revision$");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkControl4DGUI::vtkControl4DGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkControl4DGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->SelectImageButton             = NULL;
  this->LoadImageButton               = NULL;
  this->ForegroundVolumeSelectorScale = NULL;
  this->BackgroundVolumeSelectorScale = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkControl4DGUI::~vtkControl4DGUI ( )
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

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Enter()
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
void vtkControl4DGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "Control4DGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->LoadImageButton)
    {
    this->LoadImageButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->GetMenu()
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

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

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  if (this->LoadImageButton)
    this->LoadImageButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton11)
    this->TestButton11
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  if (this->TestButton12)
    this->TestButton12
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  if (this->TestButton21)
    this->TestButton21
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  if (this->TestButton22)
    this->TestButton22
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkControl4DLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if (this->LoadImageButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    const char* path = this->SelectImageButton->GetWidget()->GetFileName();
    int n = this->GetLogic()->LoadImagesFromDir(path);
    // Adjust range of the scale
    this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n);
    this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n);
    SetForeground(0);
    SetBackground(0);
    }
  else if (this->ForegroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/)
    {
    int value = (int)this->ForegroundVolumeSelectorScale->GetValue();
    SetForeground(value);
    }
  else if (this->BackgroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/ )
    {
    int value = (int)this->BackgroundVolumeSelectorScale->GetValue();
    SetBackground(value);
    }
  else if (this->MaskSelectMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    SelectMask(nodeID, label);
    }
  else if (this->MaskSelectSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    SelectMask(nodeID, label);
    }
} 


//---------------------------------------------------------------------------
void vtkControl4DGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkControl4DGUI *self = reinterpret_cast<vtkControl4DGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkControl4DGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkControl4DLogic::SafeDownCast(caller))
    {
    if (event == vtkControl4DLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    UpdateMaskSelectMenu();
    }
  else if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessTimerEvents()
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
void vtkControl4DGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "Control4D", "Control4D", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLoadFrame();
  BuildGUIForFrameControlFrame();
  BuildGUIForFunctionViewer();
  BuildGUIForTestFrame2();

}


void vtkControl4DGUI::BuildGUIForHelpFrame ()
{
  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------

  // Define your help text here.
  const char *help = 
    "**The OpenIGTLink Interface Module** helps you to manage OpenIGTLink connections:"
    "OpenIGTLink is an open network protocol for communication among software / hardware "
    "for image-guided therapy. See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:OpenIGTLinkIF</a> for details.";
  const char *about =
    "The module is designed and implemented by Junichi Tokuda for Brigham and Women's Hospital."
    "This work is supported by NCIGT, NA-MIC and BRP \"Enabling Technologies for MRI-Guided Prostate Intervention\" project.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Control4D" );
  this->BuildHelpAndAboutFrame (page, help, about);

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForLoadFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Load");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Select File Frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Input Directory");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  
  this->SelectImageButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SelectImageButton->SetParent(frame->GetFrame());
  this->SelectImageButton->Create();
  this->SelectImageButton->SetWidth(50);
  this->SelectImageButton->GetWidget()->SetText ("Browse Image File");
  this->SelectImageButton->GetWidget()->GetLoadSaveDialog()->ChooseDirectoryOn();
  /*
    this->SelectImageButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes(
    "{ {ProstateNav} {*.dcm} }");
  */
  this->SelectImageButton->GetWidget()->GetLoadSaveDialog()
    ->RetrieveLastPathFromRegistry("OpenPath");

  this->LoadImageButton = vtkKWPushButton::New ( );
  this->LoadImageButton->SetParent ( frame->GetFrame() );
  this->LoadImageButton->Create ( );
  this->LoadImageButton->SetText ("Load Series");
  this->LoadImageButton->SetWidth (12);

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SelectImageButton->GetWidgetName(),
               this->LoadImageButton->GetWidgetName());

  conBrowsFrame->Delete();
  frame->Delete();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForFrameControlFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Frame Control");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Foreground child frame

  vtkKWFrameWithLabel *fframe = vtkKWFrameWithLabel::New();
  fframe->SetParent(conBrowsFrame->GetFrame());
  fframe->Create();
  fframe->SetLabelText ("Frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 fframe->GetWidgetName() );

  this->ForegroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->ForegroundVolumeSelectorScale->SetParent( fframe->GetFrame() );
  this->ForegroundVolumeSelectorScale->Create();
  this->ForegroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->ForegroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->ForegroundVolumeSelectorScale->SetLabelText("FG Frame #");
  this->ForegroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->ForegroundVolumeSelectorScale->SetResolution(1.0);

  this->BackgroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->BackgroundVolumeSelectorScale->SetParent( fframe->GetFrame() );
  this->BackgroundVolumeSelectorScale->Create();
  this->BackgroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->BackgroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->BackgroundVolumeSelectorScale->SetLabelText("BG Frame #");
  this->BackgroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->BackgroundVolumeSelectorScale->SetResolution(1.0);

  this->Script("pack %s %s -side bottom -fill x -padx 2 -pady 2", 
               this->ForegroundVolumeSelectorScale->GetWidgetName(),
               this->BackgroundVolumeSelectorScale->GetWidgetName());


  // -----------------------------------------
  // Delete pointers

  conBrowsFrame->Delete();
  fframe->Delete();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForFunctionViewer()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Function Viewer");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());


  // -----------------------------------------
  // Mask selector frame
  
  vtkKWFrameWithLabel *msframe = vtkKWFrameWithLabel::New();
  msframe->SetParent(conBrowsFrame->GetFrame());
  msframe->Create();
  msframe->SetLabelText ("Mask Selector");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 msframe->GetWidgetName() );

  vtkKWLabel *menuLabel = vtkKWLabel::New();
  menuLabel->SetParent(msframe->GetFrame());
  menuLabel->Create();
  menuLabel->SetText("Mask: ");

  this->MaskSelectMenu = vtkKWMenuButton::New();
  this->MaskSelectMenu->SetParent(msframe->GetFrame());
  this->MaskSelectMenu->Create();
  this->MaskSelectMenu->SetWidth(20);

  this->MaskSelectSpinBox = vtkKWSpinBox::New();
  this->MaskSelectSpinBox->SetParent(msframe->GetFrame());
  this->MaskSelectSpinBox->Create();
  this->MaskSelectSpinBox->SetWidth(3);

  this->MaskColorCanvas = vtkKWCanvas::New();
  this->MaskColorCanvas->SetParent(msframe->GetFrame());
  this->MaskColorCanvas->Create();
  this->MaskColorCanvas->SetWidth(15);
  this->MaskColorCanvas->SetHeight(15);
  this->MaskColorCanvas->SetBackgroundColor(0.0, 0.0, 0.0);
  this->MaskColorCanvas->SetBorderWidth(2);
  this->MaskColorCanvas->SetReliefToSolid();

  this->Script("pack %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               menuLabel->GetWidgetName(),
               this->MaskSelectMenu->GetWidgetName(),
               this->MaskSelectSpinBox->GetWidgetName(),
               this->MaskColorCanvas->GetWidgetName());
  
  // -----------------------------------------
  // Plot frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Intencity Plot");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  vtkPiecewiseFunction* tfun = vtkPiecewiseFunction::New();
  tfun->AddPoint(70.0, 0.0);
  tfun->AddPoint(599.0, 0);
  tfun->AddPoint(600.0, 0);
  tfun->AddPoint(1195.0, 0);
  tfun->AddPoint(1200, .2);
  tfun->AddPoint(1300, .3);
  tfun->AddPoint(2000, .3);
  tfun->AddPoint(4095.0, 1.0);

  this->FunctionEditor = vtkKWPiecewiseFunctionEditor::New();
  this->FunctionEditor->SetParent(frame->GetFrame());
  this->FunctionEditor->Create();
  this->FunctionEditor->SetReliefToGroove();
  this->FunctionEditor->SetPadX(2);
  this->FunctionEditor->SetPadY(2);
  this->FunctionEditor->ParameterRangeVisibilityOff();
  this->FunctionEditor->SetCanvasHeight(200);
  this->FunctionEditor->SetPiecewiseFunction(tfun);
  this->FunctionEditor->ReadOnlyOn();
  this->FunctionEditor->SetPointRadius(2);
  this->FunctionEditor->ParameterTicksVisibilityOn();
  this->FunctionEditor->SetParameterTicksFormat("%-#4.2f");
  this->FunctionEditor->ValueTicksVisibilityOn();
  this->FunctionEditor->SetValueTicksFormat("%-#4.2f");

  this->Script("pack %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->FunctionEditor->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
}

//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
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
void vtkControl4DGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetForeground(int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  const char* nodeID = this->GetLogic()->SwitchNodeFG(index);
  vtkMRMLVolumeNode* volNode =
  vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (volNode)
    {
    std::cerr << "node id = " << nodeID << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");          
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetForegroundVolumeID( nodeID );
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetBackground(int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  const char* nodeID = this->GetLogic()->SwitchNodeBG(index);
  vtkMRMLVolumeNode* volNode =
    vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (volNode)
    {
    std::cerr << "node id = " << nodeID << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");          
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetForegroundVolumeID( nodeID );
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateMaskSelectMenu()
{

  if (!this->MaskSelectMenu)
    {
    return;
    }

  const char* className = this->GetMRMLScene()->GetClassNameByTag("Volume");
  std::vector<vtkMRMLNode*> nodes;
  this->GetMRMLScene()->GetNodesByClass(className, nodes);

  std::vector<vtkMRMLNode*>::iterator iter;
  if (nodes.size() > 0)
    {
    this->MaskSelectMenu->GetMenu()->DeleteAllItems();
    this->MaskNodeIDList.clear();
    for (iter = nodes.begin(); iter != nodes.end(); iter ++)
      {
      //(*iter)->GetName();
      //nodeInfo.nodeID = (*iter)->GetID();
      char str[256];

      vtkMRMLScalarVolumeNode* node =
        vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID((*iter)->GetID()));
      if (node != NULL && node->GetLabelMap())
        {
        //sprintf(str, "%s (%s)", (*iter)->GetName(), (*iter)->GetID());
        sprintf(str, "%s", (*iter)->GetName());
        this->MaskSelectMenu->GetMenu()->AddRadioButton(str);
        this->MaskNodeIDList.push_back(std::string((*iter)->GetID()));
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SelectMask(const char* nodeID, int label)
{

  vtkMRMLScalarVolumeNode* node =
    vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (node == NULL || !node->GetLabelMap())
    {
    return;
    }

  vtkMRMLVolumeDisplayNode* dnode = node->GetVolumeDisplayNode();
  if (dnode == NULL)
    {
    return;
    }

  vtkMRMLColorNode* cnode = dnode->GetColorNode();
  if (cnode == NULL)
    {
    return;
    }

  vtkLookupTable* lt = cnode->GetLookupTable();
  double l, h;

  l = lt->GetRange()[0];
  h = lt->GetRange()[1];
  if (label < l)
    {
    this->MaskSelectSpinBox->SetValue(l);
    label = (int)l;
    }
  else if (label > h)
    {
    this->MaskSelectSpinBox->SetValue(h);
    label = (int)h;
    }
  double color[3];
  lt->GetColor(label, color);
  if (this->MaskColorCanvas)
    {
    this->MaskColorCanvas->SetBackgroundColor(color[0], color[1], color[2]);
    }
  
}
