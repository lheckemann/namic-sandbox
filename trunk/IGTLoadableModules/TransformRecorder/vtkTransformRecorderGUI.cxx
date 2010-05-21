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

#include "vtkTransformRecorderGUI.h"
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


// MACROS ---------------------------------------------------------------------

#define DESTRUCT(x) \
  if ( x ) \
    { \
    x->SetParent( NULL ); \
    x->Delete(); \
    x = NULL; \
    }

#define REMOVE_OBSERVER( obj ) \
  if ( obj ) \
    { \
    obj->RemoveObserver( (vtkCommand *)this->GUICallbackCommand ); \
    }

#define REMOVE_OBSERVERS( obj, evnt ) \
  if ( obj ) \
    { \
    obj->RemoveObservers( evnt, (vtkCommand *)this->GUICallbackCommand ); \
    }
  


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkTransformRecorderGUI );
vtkCxxRevisionMacro ( vtkTransformRecorderGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkTransformRecorderGUI::vtkTransformRecorderGUI ( )
{
  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkTransformRecorderGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->TestButton11 = NULL;
  this->TestButton12 = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;
  
  
  this->ModuleNodeSelector = NULL;
  this->TransformSelector = NULL;
  
  this->StartButton = NULL;
  this->StopButton = NULL;
  
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;
  
  
  this->ModuleNodeID = NULL;
  this->ModuleNode = NULL;
}

//---------------------------------------------------------------------------
vtkTransformRecorderGUI::~vtkTransformRecorderGUI ( )
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

  DESTRUCT( this->TestButton11 );
  DESTRUCT( this->TestButton12 );
  DESTRUCT( this->TestButton21 );
  DESTRUCT( this->TestButton22 );
  
  DESTRUCT( this->ModuleNodeSelector );
  DESTRUCT( this->TransformSelector );
  
  DESTRUCT( this->StartButton );
  DESTRUCT( this->StopButton );
  
  
  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );
  
  this->SetAndObserveModuleNodeID( NULL );
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::Init()
{
  this->GetMRMLScene()->RegisterNodeClass(
    vtkSmartPointer< vtkMRMLTransformRecorderNode >::New() );
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }
  
  vtkMRMLTransformRecorderNode* node = this->GetModuleNode();
  
  if ( node == NULL )
    {
    this->ModuleNodeSelector->SetSelectedNew( "vtkMRMLTransformRecorderNode" );
    this->ModuleNodeSelector->ProcessNewNodeCommand(
      "vtkMRMLTransformRecorderNode", "TransformRecorderNode" );
    
    node = vtkMRMLTransformRecorderNode::SafeDownCast(
      this->ModuleNodeSelector->GetSelected() );
    
    vtkSetAndObserveMRMLNodeMacro( this->ModuleNode, node );
    }
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "TransformRecorderGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  REMOVE_OBSERVER( this->TestButton11 );
  REMOVE_OBSERVER( this->TestButton12 );
  REMOVE_OBSERVER( this->TestButton21 );
  REMOVE_OBSERVER( this->TestButton22 );
  
  REMOVE_OBSERVERS( this->ModuleNodeSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->TransformSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  
  REMOVE_OBSERVER( this->StartButton );
  REMOVE_OBSERVER( this->StopButton );
  
  this->RemoveLogicObservers();
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::AddGUIObservers ( )
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

  this->TestButton11
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton12
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton21
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton22
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  
  
  this->ModuleNodeSelector
    ->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                   (vtkCommand *)this->GUICallbackCommand );
  
  this->TransformSelector
    ->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                   (vtkCommand *)this->GUICallbackCommand );
  
  this->StartButton->AddObserver(
    vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand );
  
  this->StopButton->AddObserver(
    vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand );
  
  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkTransformRecorderLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::ProcessGUIEvents(vtkObject *caller,
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
  
  
  if ( this->ModuleNodeSelector ==
       vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
       && ( event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
            || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    char* selectedNodeID = NULL;
    vtkMRMLTransformRecorderNode* selectedNode =
      vtkMRMLTransformRecorderNode::SafeDownCast(
        this->ModuleNodeSelector->GetSelected() );
    
    if ( selectedNode != NULL )
      {
      selectedNodeID = selectedNode->GetID();
      this->SetAndObserveModuleNodeID( selectedNodeID );
      // this->ModuleNode = selectedNode;
      this->SetModuleNode( selectedNode );
      }
    }
  
  
  if (    this->ModuleNode
       && this->TransformSelector ==
          vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
       && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    char* selectedNodeID = NULL;
    vtkMRMLTransformNode* selectedNode = vtkMRMLTransformNode::SafeDownCast(
      this->TransformSelector->GetSelected() );
    if ( selectedNode != NULL )
      {
      selectedNodeID = selectedNode->GetID();
      }
    this->ModuleNode->SetAndObserveObservedTransformNodeID( selectedNodeID );
    }
  
  
  if (    this->StartButton == vtkKWPushButton::SafeDownCast( caller )
       && event == vtkKWPushButton::InvokedEvent )
    {
    this->ModuleNode->SetRecording( true );
    }
  
  
  if (    this->StopButton == vtkKWPushButton::SafeDownCast( caller )
       && event == vtkKWPushButton::InvokedEvent )
    {
    this->ModuleNode->SetRecording( false );
    }
} 


void vtkTransformRecorderGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkTransformRecorderGUI *self = reinterpret_cast<vtkTransformRecorderGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkTransformRecorderGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkTransformRecorderLogic::SafeDownCast(caller))
    {
    if (event == vtkTransformRecorderLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::ProcessTimerEvents()
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
void vtkTransformRecorderGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "TransformRecorder", "TransformRecorder", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();
  BuildGUIForTestFrame2();
  
  
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("TransformRecorder");
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > inputFrame =
      vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    inputFrame->SetParent( page );
    inputFrame->Create();
    inputFrame->SetLabelText( "Input" );
  
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               inputFrame->GetWidgetName(), page->GetWidgetName() );
  
  
  this->ModuleNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->ModuleNodeSelector->SetNodeClass( "vtkMRMLTransformRecorderNode", NULL, NULL, "ModuleNode" );
  this->ModuleNodeSelector->SetParent( inputFrame->GetFrame() );
  this->ModuleNodeSelector->Create();
  this->ModuleNodeSelector->SetNewNodeEnabled( 1 );
  this->ModuleNodeSelector->NoneEnabledOn();
  this->ModuleNodeSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
  this->ModuleNodeSelector->UpdateMenu();
  this->ModuleNodeSelector->SetLabelText( "Module node: " );
  
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ModuleNodeSelector->GetWidgetName() );
  
  
  this->TransformSelector = vtkSlicerNodeSelectorWidget::New();
  this->TransformSelector->SetParent( inputFrame->GetFrame() );
  this->TransformSelector->Create();
  this->TransformSelector->NoneEnabledOn();
  this->TransformSelector->SetNodeClass( "vtkMRMLTransformNode", NULL, NULL, "Transform" );
  this->TransformSelector->SetMRMLScene( this->GetMRMLScene() );
  this->TransformSelector->SetLabelText( "Transform to track: " );
  
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->TransformSelector->GetWidgetName() );
               
  
  this->BuildGUIForControlsFrame();
}


void vtkTransformRecorderGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:TransformRecorder</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "TransformRecorder" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("TransformRecorder");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 1");
  //conBrowsFrame->CollapseFrame();
  // app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
  //              conBrowsFrame->GetWidgetName(), page->GetWidgetName());

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
void vtkTransformRecorderGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("TransformRecorder");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 2");
  //conBrowsFrame->CollapseFrame();
  // app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
  //              conBrowsFrame->GetWidgetName(), page->GetWidgetName());

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


void
vtkTransformRecorderGUI
::BuildGUIForControlsFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("TransformRecorder");
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > controlsFrame =
      vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    controlsFrame->SetParent( page );
    controlsFrame->Create();
    controlsFrame->SetLabelText( "Controls" );
  
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                controlsFrame->GetWidgetName(), page->GetWidgetName() );
  
  
  this->StartButton = vtkKWPushButton::New();
  this->StartButton->SetParent( controlsFrame->GetFrame() );
  this->StartButton->Create();
  this->StartButton->SetBackgroundColor( 0.6, 1.0, 0.6 );
  this->StartButton->SetWidth( 10 );
  this->StartButton->SetText( "Start" );
  
  this->Script("pack %s -side left -padx 2 -pady 2", 
               this->StartButton->GetWidgetName());
  
  this->StopButton = vtkKWPushButton::New();
  this->StopButton->SetParent( controlsFrame->GetFrame() );
  this->StopButton->Create();
  this->StopButton->SetBackgroundColor( 1.0, 0.6, 0.6 );
  this->StopButton->SetWidth( 10 );
  this->StopButton->SetText( "Stop" );
  
  this->Script("pack %s -side left -padx 2 -pady 2", 
               this->StopButton->GetWidgetName());
}


//----------------------------------------------------------------------------
void vtkTransformRecorderGUI::UpdateAll()
{
}


void
vtkTransformRecorderGUI
::SetAndObserveModuleNodeID( const char *nodeID )
{
  bool modified = false;
  if ( nodeID != NULL && this->ModuleNodeID != NULL )
  {
    if ( strcmp( nodeID, this->ModuleNodeID ) != 0 )
    {
      modified = true;
    }
  }
  else if ( nodeID != this->ModuleNodeID )
  {
    modified = true;
  }
  
  vtkSetAndObserveMRMLObjectMacro( this->ModuleNode, NULL);
  this->SetModuleNodeID( nodeID );
  vtkMRMLTransformRecorderNode* tnode = this->GetModuleNode();
  
  vtkSetAndObserveMRMLObjectMacro( this->ModuleNode, tnode );
  
  if ( ! modified)
  {
    return;
  }

  this->UpdateGUI();
}


void
vtkTransformRecorderGUI
::UpdateGUI()
{
  
}

vtkMRMLTransformRecorderNode*
vtkTransformRecorderGUI
::GetModuleNode()
{
  vtkMRMLTransformRecorderNode* node = NULL;
  if ( this->ModuleNodeID != NULL )
    {
    return this->ModuleNode;
    }
  return node;
}
