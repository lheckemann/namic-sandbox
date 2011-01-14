
#include "vtkTransformRecorderGUI.h"

#include <fstream>
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
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWEvent.h"

#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLLinearTransformNode.h"

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



vtkTransformRecorderGUI
::vtkTransformRecorderGUI()
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
  this->ConnectionSelector = NULL;
  
  this->TransformsList = NULL;
  this->FileSelectButton = NULL;
  this->LogFileLabel = NULL;
  this->ClearBufferButton = NULL;
  this->SaveButton = NULL;
  
  this->StartButton = NULL;
  this->StopButton = NULL;
  
  this->AnnotFrame = NULL;
  this->QuickFrame = NULL;
  this->AnnotationsNumberEntry = NULL;
  this->AnnotationsUpdateButton = NULL;
  
  this->StatusLabel = NULL;
  this->TranslationLabel = NULL;
  
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;
  
  
  this->ModuleNodeID = NULL;
  this->ModuleNode = NULL;
  
  
  this->AnnotationsNumber = 2;
}



vtkTransformRecorderGUI
::~vtkTransformRecorderGUI()
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
  DESTRUCT( this->ConnectionSelector );
  
  DESTRUCT( this->FileSelectButton );
  DESTRUCT( this->LogFileLabel );
  DESTRUCT( this->ClearBufferButton );
  DESTRUCT( this->SaveButton );
  
  DESTRUCT( this->StartButton );
  DESTRUCT( this->StopButton );
  
  DESTRUCT( this->AnnotFrame );
  DESTRUCT( this->QuickFrame );
  DESTRUCT( this->AnnotationsNumberEntry );
  DESTRUCT( this->AnnotationsUpdateButton );
  
  DESTRUCT( this->StatusLabel );
  DESTRUCT( this->TranslationLabel );
  
  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );
  
  this->SetAndObserveModuleNodeID( NULL );
}



void
vtkTransformRecorderGUI
::Init()
{
  this->GetMRMLScene()->RegisterNodeClass(
    vtkSmartPointer< vtkMRMLTransformRecorderNode >::New() );
}



void
vtkTransformRecorderGUI
::Enter()
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
  
  this->UpdateGUI();
}



void
vtkTransformRecorderGUI
::Exit()
{
  // Fill in
}



void
vtkTransformRecorderGUI
::PrintSelf( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "TransformRecorderGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}



void
vtkTransformRecorderGUI
::RemoveGUIObservers()
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  REMOVE_OBSERVERS( this->ModuleNodeSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->TransformSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->ConnectionSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  
  REMOVE_OBSERVERS( this->FileSelectButton->GetWidget(), vtkKWLoadSaveDialog::WithdrawEvent );
  
  REMOVE_OBSERVER( this->SaveButton );
  
  
  REMOVE_OBSERVER( this->StartButton );
  REMOVE_OBSERVER( this->StopButton );
  REMOVE_OBSERVER( this->ClearBufferButton );
  
  this->RemoveLogicObservers();
  this->RemoveMRMLObservers();
}



void
vtkTransformRecorderGUI
::AddGUIObservers()
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
  
  this->ConnectionSelector
    ->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                   (vtkCommand *)this->GUICallbackCommand );
  
  
  if ( this->TransformsList )
    {
    this->TransformsList->GetWidget()->SetCellUpdatedCommand( this, "OnTransformsListUpdate" );    
    }
  
  
  this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->AddObserver(
    vtkKWLoadSaveDialog::WithdrawEvent, (vtkCommand*)( this->GUICallbackCommand ) );
  
  ADD_BUTTONINVOKED_OBSERVER( this->SaveButton );
  
  
  ADD_BUTTONINVOKED_OBSERVER( this->StartButton );
  ADD_BUTTONINVOKED_OBSERVER( this->StopButton );
  ADD_BUTTONINVOKED_OBSERVER( this->ClearBufferButton );
  
  ADD_BUTTONINVOKED_OBSERVER( this->AnnotationsUpdateButton );
  
  
  this->AddLogicObservers();
}



void vtkTransformRecorderGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers( vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}



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

  
    // Change the module node (TransformRecorder).
  
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
      
      this->ModuleNode->AddObserver( vtkMRMLTransformRecorderNode::TransformChangedEvent,
                                       (vtkCommand*)this->MRMLCallbackCommand );
      this->ModuleNode->AddObserver( vtkMRMLTransformRecorderNode::RecordingStartEvent,
                                      (vtkCommand*)this->MRMLCallbackCommand );
      this->ModuleNode->AddObserver( vtkMRMLTransformRecorderNode::RecordingStopEvent,
                                       (vtkCommand*)this->MRMLCallbackCommand );
      }
    }
  
  
    // A new IGTL connector node was selected. Update observers.
  
  if (    this->ModuleNode
       && this->ConnectionSelector == vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
       && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    char* selectedNodeID = NULL;
    vtkMRMLIGTLConnectorNode* selectedNode = vtkMRMLIGTLConnectorNode::SafeDownCast(
        this->ConnectionSelector->GetSelected() );
    
    if ( selectedNode != NULL )
      {
      selectedNodeID = selectedNode->GetID();
      }
    
    this->ModuleNode->SetAndObserveObservedConnectorNodeID( selectedNodeID );
    
    
    }
  
  
    // Save into file.
  
  if ( this->FileSelectButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast( caller ) )
    {
    if ( this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->GetStatus() == vtkKWDialog::StatusOK )
      {
      this->SelectLogFile();
      }
    }
  
  
    // Clear buffer button pressed.
  
  if (    this->ClearBufferButton == vtkKWPushButton::SafeDownCast( caller )
       && event == vtkKWPushButton::InvokedEvent )
    {
    this->ModuleNode->ClearBuffer();
    }
  
  
    // Save button pressed.
  
  if (    this->SaveButton == vtkKWPushButton::SafeDownCast( caller )
       && event == vtkKWPushButton::InvokedEvent )
    {
    this->ModuleNode->UpdateFileFromBuffer();
    }
  
  
  // Controls panel -----------------------------------------------------------
  
  
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
  
  
  for ( unsigned int i = 0; i < this->AnnotationButtonVector.size(); ++ i )
    {
    if ( this->AnnotationButtonVector[ i ] == vtkKWPushButton::SafeDownCast( caller )
         && event == vtkKWPushButton::InvokedEvent )
      {
      this->ModuleNode->CustomMessage(
        std::string( this->AnnotationEntryVector[ i ]->GetValue() ) );
      }
    }
  
  
    // Annotations panel
  
  if ( this->AnnotationsUpdateButton == vtkKWPushButton::SafeDownCast( caller ) && event == vtkKWPushButton::InvokedEvent )
    {
    int anum = this->AnnotationsNumberEntry->GetValueAsInt();
    this->SetAnnotationsNumber( anum );
    }
  
  
  this->UpdateGUI();
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



void
vtkTransformRecorderGUI
::ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData )
{
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
  
  
  this->UpdateGUI();
}



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
    this->ModuleNode->SaveIntoFile( std::string( fileName ) );
    }
}



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
  this->BuildGUIForAnnotationsFrame();
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
  
  /*
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->TransformSelector->GetWidgetName() );
  */
  
  
  this->ConnectionSelector = vtkSlicerNodeSelectorWidget::New();
  this->ConnectionSelector->SetParent( inputFrame->GetFrame() );
  this->ConnectionSelector->Create();
  this->ConnectionSelector->NoneEnabledOn();
  this->ConnectionSelector->SetNodeClass( "vtkMRMLIGTLConnectorNode", NULL, NULL, "Connector" );
  this->ConnectionSelector->SetMRMLScene( this->GetMRMLScene() );
  this->ConnectionSelector->SetLabelText( "IGT Connector: " );
  
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ConnectionSelector->GetWidgetName() );
  
  
  // Just try how this widget works.
  this->TransformsList = vtkKWMultiColumnListWithScrollbars::New();
  this->TransformsList->SetParent( inputFrame->GetFrame() );
  this->TransformsList->Create();
  this->TransformsList->GetWidget()->SetHeight( 3 );
  this->TransformsList->GetWidget()->SetSelectionTypeToRow();
  this->TransformsList->GetWidget()->MovableRowsOff();
  this->TransformsList->GetWidget()->MovableColumnsOff();
  this->TransformsList->GetWidget()->AddColumn( "Record" );
  this->TransformsList->GetWidget()->SetColumnEditable( 0, 1 );
  this->TransformsList->GetWidget()->SetColumnEditWindowToCheckButton( 0 );
  this->TransformsList->GetWidget()->AddColumn( "Transform" );
  this->TransformsList->GetWidget()->SetColumnEditable( 1, 0 );
  this->TransformsList->GetWidget()->SetColumnEditWindowToEntry( 1 );
  this->TransformsList->GetWidget()->SetColumnWidth( 1, 35 );
  
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->TransformsList->GetWidgetName() );
  
  
  
  
  
  // Output ---------------------------------------------------------------------
  
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > outputFrame =
      vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    outputFrame->SetParent( page );
    outputFrame->Create();
    outputFrame->SetLabelText( "Output" );
  
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               outputFrame->GetWidgetName(), page->GetWidgetName() );
  
  
  vtkSmartPointer< vtkKWFrame > fileSelectFrame = vtkSmartPointer< vtkKWFrame >::New();
    fileSelectFrame->SetParent( outputFrame->GetFrame() );
    fileSelectFrame->Create();
  
  app->Script( "pack %s -side left -anchor nw -fill x -padx 2 -pady 2",
               fileSelectFrame->GetWidgetName() );
  
  
  this->FileSelectButton = vtkKWLoadSaveButtonWithLabel::New();
  this->FileSelectButton->SetParent( fileSelectFrame );
  this->FileSelectButton->Create();
  this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOn();
  this->FileSelectButton->SetLabelText( "Tracking record file: " );
  this->FileSelectButton->GetWidget()->SetText( "Select log file" );
  this->FileSelectButton->GetWidget()->GetLoadSaveDialog()
      ->RetrieveLastPathFromRegistry( "TransformRecorderLogFile" );
  this->FileSelectButton->GetWidget()->TrimPathFromFileNameOff();
  // this->FileSelectButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes( "{{TXT File} {.txt}}" );
  
  app->Script( "pack %s -side left -anchor nw -fill x -padx 2 -pady 2",
               this->FileSelectButton->GetWidgetName() );
  
  
  this->LogFileLabel = vtkKWLabel::New();
  this->LogFileLabel->SetParent( fileSelectFrame );
  this->LogFileLabel->Create();
  this->LogFileLabel->SetText( "No log file specified." );
  
  // app->Script( "pack %s -side left -anchor nw -fill x -padx 2 -pady 2", this->LogFileLabel->GetWidgetName() );
  
  
  this->SaveButton = vtkKWPushButton::New();
  this->SaveButton->SetParent( fileSelectFrame );
  this->SaveButton->Create();
  this->SaveButton->SetText( "Save" );
  
  // app->Script( "pack %s -side right -anchor ne -fill x -padx 2 -pady 2", this->SaveButton->GetWidgetName() );
  
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
  this->StartButton->SetBackgroundColor( 0.7, 1.0, 0.7 );
  this->StartButton->SetWidth( 10 );
  this->StartButton->SetText( "Start" );
  
  this->StopButton = vtkKWPushButton::New();
  this->StopButton->SetParent( startFrame );
  this->StopButton->Create();
  this->StopButton->SetBackgroundColor( 1.0, 0.7, 0.7 );
  this->StopButton->SetWidth( 10 );
  this->StopButton->SetText( "Stop" );
  
  this->ClearBufferButton = vtkKWPushButton::New();
  this->ClearBufferButton->SetParent( startFrame );
  this->ClearBufferButton->Create();
  this->ClearBufferButton->SetBackgroundColor( 1.0,0.9, 0.7 );
  this->ClearBufferButton->SetWidth( 10 );
  this->ClearBufferButton->SetText( "Clear buffer" );
  
  this->Script( "pack %s -side top -anchor w -padx 0 -pady 0", startFrame->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", this->StartButton->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", this->StopButton->GetWidgetName() );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", this->ClearBufferButton->GetWidgetName() );
  
  
    // Monitor frame.
  
  vtkSmartPointer< vtkKWFrame > monitorFrame = vtkSmartPointer< vtkKWFrame >::New();
    monitorFrame->SetParent( controlsFrame->GetFrame() );
    monitorFrame->Create();
    
  this->Script( "pack %s -side top -anchor w -padx 0 -pady 0", monitorFrame->GetWidgetName() );
  
  
  vtkSmartPointer< vtkKWLabel > decStatusLabel = vtkSmartPointer< vtkKWLabel >::New();
    decStatusLabel->SetParent( monitorFrame );
    decStatusLabel->Create();
    decStatusLabel->SetText( "Status: " );
  
  this->StatusLabel = vtkKWLabel::New();
  this->StatusLabel->SetParent( monitorFrame );
  this->StatusLabel->Create();
  this->StatusLabel->SetText( "Waiting." );
  
  vtkSmartPointer< vtkKWLabel > decTranslationLabel = vtkSmartPointer< vtkKWLabel >::New();
    decTranslationLabel->SetParent( monitorFrame );
    decTranslationLabel->Create();
    decTranslationLabel->SetText( "Number of records: " );
  
  this->TranslationLabel = vtkKWLabel::New();
  this->TranslationLabel->SetParent( monitorFrame );
  this->TranslationLabel->Create();
  this->TranslationLabel->SetText( " - " );
  
  
  this->Script( "grid %s -column 0 -row 0 -sticky w -padx 2 -pady 2", decStatusLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 0 -sticky w -padx 2 -pady 2", this->StatusLabel->GetWidgetName() );
  this->Script( "grid %s -column 0 -row 1 -sticky w -padx 2 -pady 2", decTranslationLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 1 -sticky w -padx 2 -pady 2", this->TranslationLabel->GetWidgetName() );
  
  this->Script( "grid columnconfigure %s 0 -weight 1", monitorFrame->GetWidgetName() );
  this->Script( "grid columnconfigure %s 1 -weight 100", monitorFrame->GetWidgetName() );
}



void
vtkTransformRecorderGUI
::BuildGUIForAnnotationsFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget( "TransformRecorder" );
  
  
  this->AnnotFrame = vtkSlicerModuleCollapsibleFrame::New();
    this->AnnotFrame->SetParent( page );
    this->AnnotFrame->Create();
    this->AnnotFrame->SetLabelText( "Annotations" );
  
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                this->AnnotFrame->GetWidgetName(), page->GetWidgetName() );
  
  
  vtkSmartPointer< vtkKWFrame > topRow = vtkSmartPointer< vtkKWFrame >::New();
    topRow->SetParent( this->AnnotFrame->GetFrame() );
    topRow->Create();
  this->Script( "pack %s -side top -anchor w -padx 0 -pady 0", topRow->GetWidgetName() );
  
  
  vtkSmartPointer< vtkKWLabel > numLabel = vtkSmartPointer< vtkKWLabel >::New();
    numLabel->SetParent( topRow );
    numLabel->Create();
    numLabel->SetText( "Number of quick annotations: " );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2", numLabel->GetWidgetName() );
  
  this->AnnotationsNumberEntry = vtkKWEntry::New();
    this->AnnotationsNumberEntry->SetParent( topRow );
    this->AnnotationsNumberEntry->Create();
    this->AnnotationsNumberEntry->SetWidth( 3 );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2",
                this->AnnotationsNumberEntry->GetWidgetName() );
  
  this->AnnotationsUpdateButton = vtkKWPushButton::New();
    this->AnnotationsUpdateButton->SetParent( topRow );
    this->AnnotationsUpdateButton->Create();
    this->AnnotationsUpdateButton->SetText( "Update" );
  this->Script( "pack %s -side left -anchor w -padx 2 -pady 2",
                this->AnnotationsUpdateButton->GetWidgetName() );
  
  
    // Quick annotations.
  
  this->QuickFrame = vtkKWFrame::New();
    this->QuickFrame->SetParent( this->AnnotFrame->GetFrame() );
    this->QuickFrame->Create();
  this->Script( "pack %s -side top -anchor w -padx 0 -pady 0", this->QuickFrame->GetWidgetName() );
  // This frame is filled with widgets in the UpdateGUI function.
}



void
vtkTransformRecorderGUI
::OnTransformsListUpdate( int row, int col, char * str )
{
    // Communicate to the MRML node, which transforms should be saved.
  
  std::vector< int > transformSelections;
  for ( int row = 0; row < this->TransformsList->GetWidget()->GetNumberOfRows(); ++ row )
    {
    transformSelections.push_back( this->TransformsList->GetWidget()->GetCellTextAsInt( row, 0 ) );
    }
  this->ModuleNode->SetTransformSelections( transformSelections );
  
  
  this->UpdateGUI();
}



void
vtkTransformRecorderGUI
::UpdateAll()
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
  vtkMRMLTransformRecorderNode* moduleNode = this->GetModuleNode();
  if ( moduleNode == NULL )
    {
    return;
    }
  
  vtkMRMLScene* scene = this->GetLogic()->GetApplicationLogic()->GetMRMLScene();
  
  std::list< std::string > classesList = scene->GetNodeClassesList();
  
  
    // Update status monitor.
  
  if ( moduleNode->GetRecording() )
    {
    this->StatusLabel->SetText( "Recording" );
    }
  else
    {
    this->StatusLabel->SetText( "Waiting" );
    }
  
  int numRec = moduleNode->GetTransformsBufferSize() + moduleNode->GetMessagesBufferSize();
  std::stringstream ss;
  ss << numRec;
  this->TranslationLabel->SetText( ss.str().c_str() );
  
  
    // Update annotations.
  
    // If the number of quick annotations changed, delete the old ones.
  
  if ( this->AnnotationsNumber != this->AnnotationButtonVector.size() )
    {
    for ( unsigned int i = 0; i < this->AnnotationButtonVector.size(); ++ i )
      {
      this->Script( "grid forget %s", this->AnnotationButtonVector[ i ]->GetWidgetName() );
      this->Script( "grid forget %s", this->AnnotationEntryVector[ i ]->GetWidgetName() );
      REMOVE_OBSERVER( this->AnnotationButtonVector[ i ] );
      }
    this->AnnotationEntryVector.clear();
    this->AnnotationButtonVector.clear();
    
    
      // Create new widgets.
    
    for ( int i = 0; i < this->AnnotationsNumber; ++ i )
      {
      vtkSmartPointer< vtkKWEntry > entry = vtkSmartPointer< vtkKWEntry >::New();
        entry->SetParent( this->QuickFrame );
        entry->Create();
        entry->SetWidth( 35 );
      this->Script( "grid %s -column 0 -row %i -sticky w -padx 2 -pady 2",
                    entry->GetWidgetName(), i );
      this->AnnotationEntryVector.push_back( entry );
      
      vtkSmartPointer< vtkKWPushButton > button = vtkSmartPointer< vtkKWPushButton >::New();
        button->SetParent( this->QuickFrame );
        button->Create();
        button->SetText( "Insert" );
      this->Script( "grid %s -column 1 -row %i -sticky w -padx 2 -pady 2",
                    button->GetWidgetName(), i );
      this->AnnotationButtonVector.push_back( button );
      ADD_BUTTONINVOKED_OBSERVER( this->AnnotationButtonVector[ i ] );
      }
    }
  
  
    // Get the transform nodes and fill the list with them.
    // Proceed only with valid Connector Node.
    
  vtkMRMLIGTLConnectorNode* connectorNode = this->ModuleNode->GetObservedConnectorNode();
  if ( connectorNode == NULL )
    {
    return;
    }
  
  int numOutNodes = connectorNode->GetNumberOfIncomingMRMLNodes();
  
  
  std::vector< vtkMRMLLinearTransformNode* > txformNodes;
  for ( int i = 0; i < numOutNodes; ++ i )
    {
    vtkMRMLNode* node = connectorNode->GetIncomingMRMLNode( i );
    vtkMRMLLinearTransformNode* txformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
    if ( txformNode != NULL )
      {
      txformNodes.push_back( txformNode );
      }
    }
  
  
  /*
  // debug
  // Get all transforms from the scene.
  std::vector< vtkMRMLNode* > transformNodes;
  scene->GetNodesByClass( "vtkMRMLLinearTransformNode", transformNodes );
  */
  
  
  int numTransforms = txformNodes.size();
  
  bool delRows = false;
  if ( numTransforms != this->TransformsList->GetWidget()->GetNumberOfRows() )
    {
    delRows = true;
    }
  
  for ( int row = 0; row < numTransforms; ++ row )
    {
    if ( delRows )
      {
      this->TransformsList->GetWidget()->AddRow();
      this->TransformsList->GetWidget()->SetCellWindowCommandToCheckButton( row, 0 );
      }
    
    this->TransformsList->GetWidget()->SetCellText( row, 1, txformNodes[ row ]->GetName() );
    }
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
