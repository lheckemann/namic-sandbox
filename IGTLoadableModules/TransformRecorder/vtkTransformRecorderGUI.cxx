
#include "vtkTransformRecorderGUI.h"

#include <string>

#include "vtkObject.h"
#include "vtkObjectFactory.h"

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
#include "vtkKWLoadSaveButtonWithLabel.h"
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

#define ADD_BUTTONINVOKED_OBSERVER( obj ) \
  obj->AddObserver( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);


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
  
  this->ModuleNodeSelector = NULL;
  this->TransformSelector = NULL;
  this->FileSelectButton = NULL;
  this->LogFileLabel = NULL;
  
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

  DESTRUCT( this->ModuleNodeSelector );
  DESTRUCT( this->TransformSelector );
  DESTRUCT( this->FileSelectButton );
  DESTRUCT( this->LogFileLabel );
  
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

  REMOVE_OBSERVERS( this->ModuleNodeSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->TransformSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->FileSelectButton->GetWidget(), vtkKWLoadSaveDialog::WithdrawEvent );
  
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
  
  this->ModuleNodeSelector
    ->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                   (vtkCommand *)this->GUICallbackCommand );
  
  this->TransformSelector
    ->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                   (vtkCommand *)this->GUICallbackCommand );
  
  
  this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->AddObserver(
    vtkKWLoadSaveDialog::WithdrawEvent, (vtkCommand*)( this->GUICallbackCommand ) );
  
  
  ADD_BUTTONINVOKED_OBSERVER( this->StartButton );
  ADD_BUTTONINVOKED_OBSERVER( this->StopButton );
  
  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkTransformRecorderGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers( vtkCommand::ModifiedEvent,
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
  
  
  if ( this->FileSelectButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast( caller ) )
    {
    if ( this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->GetStatus() == vtkKWDialog::StatusOK )
      {
      this->SelectLogFile();
      }
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


void
vtkTransformRecorderGUI
::SelectLogFile()
{
  const char* fileName = this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->GetFileName();
  
  if ( fileName )
    {
    this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->SaveLastPathToRegistry( "TransformRecorderLogFile" );
    this->ModuleNode->SetLogFileName( std::string( fileName ) );
    }
}


//---------------------------------------------------------------------------
void
vtkTransformRecorderGUI
::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "TransformRecorder", "TransformRecorder", NULL );

  BuildGUIForHelpFrame();
  
  this->BuildGUIForIOFrame();
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



void
vtkTransformRecorderGUI
::BuildGUIForIOFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication*)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget( "TransformRecorder" );
  
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > inputFrame =
      vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    inputFrame->SetParent( page );
    inputFrame->Create();
    inputFrame->SetLabelText( "Input / Output" );
  
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
  
  
  vtkSmartPointer< vtkKWFrame > fileSelectFrame
      = vtkSmartPointer< vtkKWFrame >::New();
    fileSelectFrame->SetParent( inputFrame->GetFrame() );
    fileSelectFrame->Create();
  
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               fileSelectFrame->GetWidgetName() );
  
  
  this->FileSelectButton = vtkKWLoadSaveButtonWithLabel::New();
  this->FileSelectButton->SetParent( fileSelectFrame );
  this->FileSelectButton->Create();
  this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOn();
  this->FileSelectButton->SetLabelText( "Tracking record file: " );
  this->FileSelectButton->GetWidget()->SetText( "Select log file" );
  this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->RetrieveLastPathFromRegistry(
    "TransformRecorderLogFile" );
  this->FileSelectButton->GetWidget()->TrimPathFromFileNameOff();
  // this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes( "{{TXT File} {.txt}}" );
  
  app->Script( "pack %s -side left -anchor nw -fill x -padx 2 -pady 2",
               this->FileSelectButton->GetWidgetName() );
  
  
  this->LogFileLabel = vtkKWLabel::New();
  this->LogFileLabel->SetParent( fileSelectFrame );
  this->LogFileLabel->Create();
  this->LogFileLabel->SetText( "No log file specified." );
  
  // app->Script( "pack %s -side left -anchor nw -fill x -padx 2 -pady 2", this->LogFileLabel->GetWidgetName() );
  
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
  this->StartButton->SetBackgroundColor( 0.5, 1.0, 0.5 );
  this->StartButton->SetWidth( 10 );
  this->StartButton->SetText( "Start" );
  
  this->Script("pack %s -side left -padx 2 -pady 2", 
               this->StartButton->GetWidgetName());
  
  this->StopButton = vtkKWPushButton::New();
  this->StopButton->SetParent( controlsFrame->GetFrame() );
  this->StopButton->Create();
  this->StopButton->SetBackgroundColor( 1.0, 0.5, 0.5 );
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
