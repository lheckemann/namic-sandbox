
#include "vtkTransformRecorderGUI.h"

#include <sstream>
#include <string>

#include "vtkMatrix4x4.h"
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
#include "vtkKWEntry.h"
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
  this->CustomEntry = NULL;
  this->CustomButton = NULL;
  
  this->StatusLabel = NULL;
  this->TranslationLabel = NULL;
  
  for ( int i = 0; i < BUTTON_COUNT; ++ i )
    {
    vtkKWPushButton* button = NULL;
    this->MessageButtons.push_back( button );
    }
  
  
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
  DESTRUCT( this->CustomEntry );
  DESTRUCT( this->CustomButton );
  
  DESTRUCT( this->StatusLabel );
  DESTRUCT( this->TranslationLabel );
  
  for ( int i = 0; i < BUTTON_COUNT; ++ i )
    {
    DESTRUCT( this->MessageButtons[ i ] );
    }
  
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
    // this->GetMRMLScene()->AddNode( this->ModuleNode );
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
  
  for ( int i = 0; i < BUTTON_COUNT; ++ i )
    {
    REMOVE_OBSERVER( this->MessageButtons[ i ] );
    }
  
  this->RemoveLogicObservers();
  this->RemoveMRMLObservers();
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
  
  
  for ( int i = 0; i < BUTTON_COUNT; ++ i )
    {
    ADD_BUTTONINVOKED_OBSERVER( this->MessageButtons[ i ] );
    }
  
  
  ADD_BUTTONINVOKED_OBSERVER( this->StartButton );
  ADD_BUTTONINVOKED_OBSERVER( this->StopButton );
  ADD_BUTTONINVOKED_OBSERVER( this->CustomButton );
  
  
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



void vtkTransformRecorderGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}



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
      vtkMRMLTransformRecorderNode::SafeDownCast( this->ModuleNodeSelector->GetSelected() );
    
    if ( selectedNode != NULL )
      {
      selectedNodeID = selectedNode->GetID();
      this->SetAndObserveModuleNodeID( selectedNodeID );
      this->SetModuleNode( selectedNode );
      }
    }
  
  
  if (    this->ModuleNode
       && this->TransformSelector == vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
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
    this->ModuleNode->AddObserver( vtkMRMLTransformRecorderNode::TransformChangedEvent,
                                   (vtkCommand*)this->MRMLCallbackCommand );
    this->ModuleNode->AddObserver( vtkMRMLTransformRecorderNode::RecordingStartEvent,
                                   (vtkCommand*)this->MRMLCallbackCommand );
    this->ModuleNode->AddObserver( vtkMRMLTransformRecorderNode::RecordingStopEvent,
                                   (vtkCommand*)this->MRMLCallbackCommand );
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
  
  
  for ( int i = 0; i < BUTTON_COUNT; ++ i )
    {
    if ( this->MessageButtons[ i ] == vtkKWPushButton::SafeDownCast( caller )
         && event == vtkKWPushButton::InvokedEvent )
      {
      this->ModuleNode->CustomMessage( std::string( BUTTON_MESSAGES[ i ] ) );
      }
    }
  
  
  if ( this->CustomButton == vtkKWPushButton::SafeDownCast( caller ) && event == vtkKWPushButton::InvokedEvent )
    {
    this->ModuleNode->CustomMessage( std::string( this->CustomEntry->GetValue() ) );
    }
} 


void vtkTransformRecorderGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkTransformRecorderGUI *self = reinterpret_cast<vtkTransformRecorderGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkTransformRecorderGUI DataCallback");
  self->UpdateAll();
}


void
vtkTransformRecorderGUI
::RemoveMRMLObservers()
{
  REMOVE_OBSERVERS( this->ModuleNode, vtkMRMLTransformRecorderNode::TransformChangedEvent );
  REMOVE_OBSERVERS( this->ModuleNode, vtkMRMLTransformRecorderNode::RecordingStartEvent );
  REMOVE_OBSERVERS( this->ModuleNode, vtkMRMLTransformRecorderNode::RecordingStopEvent );
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
  
  if (    this->ModuleNode
       && this->ModuleNode == vtkMRMLTransformRecorderNode::SafeDownCast( caller )
       && event == vtkMRMLTransformRecorderNode::TransformChangedEvent )
    {
    vtkMRMLTransformNode* transform = this->ModuleNode->GetObservedTransformNode();
    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    matrix->Identity();
    
    transform->GetMatrixTransformToWorld( matrix );
    
    std::stringstream ss;
    ss << matrix->GetElement( 0, 3 ) << " " << matrix->GetElement( 1, 3 )
       << " " << matrix->GetElement( 2, 3 );
    this->TranslationLabel->SetText( ss.str().c_str() );
    matrix->Delete();
    }
  
  if (    this->ModuleNode
       && this->ModuleNode == vtkMRMLTransformRecorderNode::SafeDownCast( caller )
       && event == vtkMRMLTransformRecorderNode::RecordingStartEvent )
    {
    this->StatusLabel->SetText( "Recording" );
    }
  
  if (    this->ModuleNode
       && this->ModuleNode == vtkMRMLTransformRecorderNode::SafeDownCast( caller )
       && event == vtkMRMLTransformRecorderNode::RecordingStopEvent )
    {
    this->StatusLabel->SetText( "Waiting" );
    }
  
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
  this->BuildGUIForMonitorFrame();
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
  vtkKWWidget *page = this->UIPanel->GetPageWidget( "TransformRecorder" );
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > controlsFrame =
      vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    controlsFrame->SetParent( page );
    controlsFrame->Create();
    controlsFrame->SetLabelText( "Controls" );
  
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                controlsFrame->GetWidgetName(), page->GetWidgetName() );
  
  
  vtkSmartPointer< vtkKWFrame > startFrame = vtkSmartPointer< vtkKWFrame >::New();
    startFrame->SetParent( controlsFrame->GetFrame() );
    startFrame->Create();
  
  this->StartButton = vtkKWPushButton::New();
  this->StartButton->SetParent( startFrame );
  this->StartButton->Create();
  this->StartButton->SetBackgroundColor( 0.5, 1.0, 0.5 );
  this->StartButton->SetWidth( 10 );
  this->StartButton->SetText( "Start" );
  
  this->StopButton = vtkKWPushButton::New();
  this->StopButton->SetParent( startFrame );
  this->StopButton->Create();
  this->StopButton->SetBackgroundColor( 1.0, 0.5, 0.5 );
  this->StopButton->SetWidth( 10 );
  this->StopButton->SetText( "Stop" );
  
  this->Script( "pack %s -side top -anchor w -padx 0 -pady 0", startFrame->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", this->StopButton->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", this->StartButton->GetWidgetName() );
  
  
    // Message buttons.
  
  vtkSmartPointer< vtkKWFrame > messagesFrame = vtkSmartPointer< vtkKWFrame >::New();\
    messagesFrame->SetParent( controlsFrame->GetFrame() );
    messagesFrame->Create();
  this->Script( "pack %s -side top -anchor w -padx 0 -pady 0", messagesFrame->GetWidgetName() );
  
  for ( int i = 0; i < BUTTON_COUNT; ++ i )
    {
    this->MessageButtons[ i ] = vtkKWPushButton::New();
    this->MessageButtons[ i ]->SetParent( messagesFrame );
    this->MessageButtons[ i ]->Create();
    this->MessageButtons[ i ]->SetText( BUTTON_TEXTS[ i ] );
    this->MessageButtons[ i ]->SetBackgroundColor( 0.9, 0.9, 0.9 );
    this->MessageButtons[ i ]->SetWidth( 20 );
    this->Script( "pack %s -side top -anchor w -padx 2 -pady 2",
                  this->MessageButtons[ i ]->GetWidgetName() );
    }
  
  
  
  vtkSmartPointer< vtkKWFrame > customFrame = vtkSmartPointer< vtkKWFrame >::New();
    customFrame->SetParent( controlsFrame->GetFrame() );
    customFrame->Create();
  
  vtkSmartPointer< vtkKWLabel > customLabel = vtkSmartPointer< vtkKWLabel >::New();
    customLabel->SetParent( customFrame );
    customLabel->Create();
    customLabel->SetText( "Custom message: " );
  
  this->CustomEntry = vtkKWEntry::New();
  this->CustomEntry->SetParent( customFrame );
  this->CustomEntry->Create();
  
  this->CustomButton = vtkKWPushButton::New();
  this->CustomButton->SetParent( customFrame );
  this->CustomButton->Create();
  this->CustomButton->SetBackgroundColor( 0.9, 0.9, 0.9 );
  this->CustomButton->SetText( "Record" );
  
  this->Script( "pack %s -side top -anchor w -padx 0 -pady 0", customFrame->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", customLabel->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -fill x -padx 2 -pady 2", this->CustomEntry->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", this->CustomButton->GetWidgetName() );
}


void
vtkTransformRecorderGUI
::BuildGUIForMonitorFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget( "TransformRecorder" );
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > monitorFrame =
      vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    monitorFrame->SetParent( page );
    monitorFrame->Create();
    monitorFrame->SetLabelText( "Monitor" );
  
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                monitorFrame->GetWidgetName(), page->GetWidgetName() );
  
  vtkSmartPointer< vtkKWLabel > decStatusLabel = vtkSmartPointer< vtkKWLabel >::New();
    decStatusLabel->SetParent( monitorFrame->GetFrame() );
    decStatusLabel->Create();
    decStatusLabel->SetText( "Status: " );
  
  this->StatusLabel = vtkKWLabel::New();
  this->StatusLabel->SetParent( monitorFrame->GetFrame() );
  this->StatusLabel->Create();
  this->StatusLabel->SetText( "Waiting." );
  
  vtkSmartPointer< vtkKWLabel > decTranslationLabel = vtkSmartPointer< vtkKWLabel >::New();
    decTranslationLabel->SetParent( monitorFrame->GetFrame() );
    decTranslationLabel->Create();
    decTranslationLabel->SetText( "Translation: " );
  
  this->TranslationLabel = vtkKWLabel::New();
  this->TranslationLabel->SetParent( monitorFrame->GetFrame() );
  this->TranslationLabel->Create();
  this->TranslationLabel->SetText( " - " );
  
  this->Script( "grid %s -column 0 -row 0 -sticky w -padx 2 -pady 2", decStatusLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 0 -sticky w -padx 2 -pady 2", this->StatusLabel->GetWidgetName() );
  this->Script( "grid %s -column 0 -row 1 -sticky w -padx 2 -pady 2", decTranslationLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 1 -sticky w -padx 2 -pady 2", this->TranslationLabel->GetWidgetName() );
  
  this->Script( "grid columnconfigure %s 0 -weight 1", monitorFrame->GetFrame()->GetWidgetName() );
  this->Script( "grid columnconfigure %s 1 -weight 100", monitorFrame->GetFrame()->GetWidgetName() );
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
