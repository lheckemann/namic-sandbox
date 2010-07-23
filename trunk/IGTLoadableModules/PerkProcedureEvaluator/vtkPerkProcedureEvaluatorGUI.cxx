
#include "vtkPerkProcedureEvaluatorGUI.h"


#include <sstream>
#include <string>

#include "vtkCornerAnnotation.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkKWEvent.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWLabel.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWPushButton.h"
#include "vtkKWWidget.h"

#include "vtkMRMLLinearTransformNode.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"
#include "vtkSlicerTheme.h"



// ============================================================================


#define ADD_OBSERVER( obj, evnt ) \
  if ( obj ) \
    { \
    obj->AddObserver( evnt, (vtkCommand*)this->GUICallbackCommand ); \
    }

  
#define REMOVE_OBSERVERS( obj, evnt ) \
  if ( obj ) \
    { \
    obj->RemoveObservers( evnt, (vtkCommand *)this->GUICallbackCommand ); \
    }


#define CREATE_SETPARENT( obj, objType, parent ) \
  if ( ! obj ) obj = objType::New(); \
  obj->SetParent( parent ); \
  obj->Create();


#define DELETE_WITH_SETPARENT_NULL( obj ) \
  if ( obj ) \
    { \
    obj->SetParent( NULL ); \
    obj->Delete(); \
    obj = NULL; \
    };



//-----------------------------------------------------------------------------



std::string
DoubleToStr( double d )
{
  std::stringstream ss;
  ss << d;
  return ss.str();
}



//-----------------------------------------------------------------------------
vtkStandardNewMacro (vtkPerkProcedureEvaluatorGUI );
vtkCxxRevisionMacro ( vtkPerkProcedureEvaluatorGUI, "$Revision: 1.0 $");
//-----------------------------------------------------------------------------



// ============================================================================


enum
  {
  NOTES_COL_TIME = 0,
  NOTES_COL_EVENT,
  NOTES_COL_MESSAGE,
  NOTES_COL_COUNT
  };
static const char* NOTES_COL_LABELS[ NOTES_COL_COUNT ] = { "Time", "Event", "Message" };
static const int NOTES_COL_WIDTHS[ NOTES_COL_COUNT ] = { 8, 5, 30 };


// ============================================================================



vtkPerkProcedureEvaluatorGUI
::vtkPerkProcedureEvaluatorGUI()
{
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkPerkProcedureEvaluatorGUI::DataCallback);
  
  
  // GUI widgets
  
  this->PerkProcedureSelector = NULL;
  this->PlanningVolumeSelector = NULL;
  this->CalibrationSelector = NULL;
  this->NeedleTransformSelector = NULL;
  
  this->LoadButton = NULL;
  
  this->NotesFrame = NULL;
  this->NotesList = NULL;
  
  
    // Playback.
  
  this->ButtonBegin = NULL;
  this->ButtonPrevious = NULL;
  this->ButtonNext = NULL;
  this->ButtonEnd = NULL;
  this->ButtonPlay = NULL;
  this->ButtonStop = NULL;
  this->EntrySec = NULL;
  this->ButtonGo = NULL;
  this->PositionLabel = NULL;
  
  
    // Results frame.
  
  this->LabelBegin = NULL;
  this->LabelEnd = NULL;
  this->ButtonMeasureBegin = NULL;
  this->ButtonMeasureEnd = NULL;
  this->ButtonMeasure = NULL;
  this->LabelTotalTime = NULL;
  this->LabelPathInside = NULL;
  this->LabelTimeInside = NULL;
  
  
  // Locator  (MRML)
  
  this->TimerFlag = 0;
  
  this->ProcedureNode = NULL;
}



vtkPerkProcedureEvaluatorGUI
::~vtkPerkProcedureEvaluatorGUI()
{
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
  
  
    // Remove Observers
  
  this->RemoveGUIObservers();
  
  
    // Remove GUI widgets

  DELETE_WITH_SETPARENT_NULL( this->PerkProcedureSelector );
  DELETE_WITH_SETPARENT_NULL( this->PlanningVolumeSelector );
  DELETE_WITH_SETPARENT_NULL( this->CalibrationSelector );
  DELETE_WITH_SETPARENT_NULL( this->NeedleTransformSelector );
  DELETE_WITH_SETPARENT_NULL( this->LoadButton );
  
  DELETE_WITH_SETPARENT_NULL( this->NotesFrame );
  DELETE_WITH_SETPARENT_NULL( this->NotesList );
  
    // Playback frame.
  
  DELETE_WITH_SETPARENT_NULL( this->ButtonBegin );
  DELETE_WITH_SETPARENT_NULL( this->ButtonPrevious );
  DELETE_WITH_SETPARENT_NULL( this->ButtonNext );
  DELETE_WITH_SETPARENT_NULL( this->ButtonEnd );
  DELETE_WITH_SETPARENT_NULL( this->ButtonPlay );
  DELETE_WITH_SETPARENT_NULL( this->ButtonStop );
  DELETE_WITH_SETPARENT_NULL( this->EntrySec );
  DELETE_WITH_SETPARENT_NULL( this->ButtonGo );
  DELETE_WITH_SETPARENT_NULL( this->PositionLabel );
  
    // Results frame.
  
  DELETE_WITH_SETPARENT_NULL( this->LabelBegin );
  DELETE_WITH_SETPARENT_NULL( this->LabelEnd );
  DELETE_WITH_SETPARENT_NULL( this->ButtonMeasureBegin );
  DELETE_WITH_SETPARENT_NULL( this->ButtonMeasureEnd );
  DELETE_WITH_SETPARENT_NULL( this->ButtonMeasure );
  DELETE_WITH_SETPARENT_NULL( this->LabelTotalTime );
  DELETE_WITH_SETPARENT_NULL( this->LabelPathInside );
  DELETE_WITH_SETPARENT_NULL( this->LabelTimeInside );
  
  
    // Unregister Logic class

  this->SetModuleLogic ( NULL );
  
  
    // MRML
  
  if ( this->ProcedureNode )
    {
    vtkSetMRMLNodeMacro( this->ProcedureNode, NULL );
    }
}



void
vtkPerkProcedureEvaluatorGUI
::Init()
{
}



void
vtkPerkProcedureEvaluatorGUI
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
  
  this->AddGUIObservers();
}


//---------------------------------------------------------------------------
void vtkPerkProcedureEvaluatorGUI::Exit ( )
{
  this->RemoveGUIObservers();
}



void
vtkPerkProcedureEvaluatorGUI
::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "PerkProcedureEvaluatorGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}



void
vtkPerkProcedureEvaluatorGUI
::RemoveGUIObservers()
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  
    // GUI observers.
  
  
  REMOVE_OBSERVERS( this->PerkProcedureSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->PlanningVolumeSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->CalibrationSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->NeedleTransformSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  
  REMOVE_OBSERVERS( this->LoadButton, vtkKWPushButton::InvokedEvent );
  
  
    // Results frame.
  
  REMOVE_OBSERVERS( this->ButtonMeasure, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonMeasureBegin, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonMeasureEnd, vtkKWPushButton::InvokedEvent );
  
  
  if ( this->NotesList )
    {
    this->NotesList->GetWidget()->SetSelectionChangedCommand( this, "" );
    this->NotesList->GetWidget()->SetSelectionCommand( this, "" );
    }
  
  
  this->RemoveLogicObservers();
}



void
vtkPerkProcedureEvaluatorGUI
::AddGUIObservers()
{
  this->RemoveGUIObservers();
  
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  
    // MRML

  vtkIntArray* events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents( this->GetMRMLScene(), events );
    }
  events->Delete();

  
    // GUI Observers.
  
  ADD_OBSERVER( this->PerkProcedureSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  ADD_OBSERVER( this->PlanningVolumeSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  ADD_OBSERVER( this->CalibrationSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  ADD_OBSERVER( this->NeedleTransformSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  
  ADD_OBSERVER( this->LoadButton->GetLoadSaveDialog(), vtkKWTopLevel::WithdrawEvent );
  
  
    // Results frame.
  
  ADD_OBSERVER( this->ButtonMeasure, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonMeasureBegin, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonMeasureEnd, vtkKWPushButton::InvokedEvent );
  
  
  if ( this->NotesList )
    {
    this->NotesList->GetWidget()->SetSelectionChangedCommand( this, "OnNoteSelectionChanged" );
    this->NotesList->GetWidget()->SetSelectionCommand( this, "OnNoteSelectionChanged" );
    }
  
  
  this->AddLogicObservers();
}



void
vtkPerkProcedureEvaluatorGUI
::RemoveLogicObservers()
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}



void
vtkPerkProcedureEvaluatorGUI
::AddLogicObservers()
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkPerkProcedureEvaluatorLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}



void
vtkPerkProcedureEvaluatorGUI
::HandleMouseEvent( vtkSlicerInteractorStyle *style )
{
}



void
vtkPerkProcedureEvaluatorGUI
::ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData )
{
  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (    this->PerkProcedureSelector == vtkSlicerNodeSelectorWidget::SafeDownCast( caller ) 
       && (    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
            || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    this->ProcessProcedureSelected();
    }
  
  else if (    this->NeedleTransformSelector == vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
            && (    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
                 || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    vtkMRMLLinearTransformNode* node = vtkMRMLLinearTransformNode::SafeDownCast(
      this->NeedleTransformSelector->GetSelected() );
    this->ProcedureNode->SetAndObserveNeedleTransformNodeID( node->GetID() );
    }
  
  else if (    this->LoadButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast( caller )
            && event == vtkKWTopLevel::WithdrawEvent )
    {
    this->ProcessLoadButton();
    }
} 



void
vtkPerkProcedureEvaluatorGUI
::DataCallback( vtkObject *caller, unsigned long eid, void *clientData, void *callData )
{
  vtkPerkProcedureEvaluatorGUI *self = reinterpret_cast<vtkPerkProcedureEvaluatorGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkPerkProcedureEvaluatorGUI DataCallback");
  self->UpdateAll();
}



void
vtkPerkProcedureEvaluatorGUI
::ProcessLogicEvents( vtkObject *caller, unsigned long event, void *callData )
{
  if (this->GetLogic() == vtkPerkProcedureEvaluatorLogic::SafeDownCast(caller))
    {
    if (event == vtkPerkProcedureEvaluatorLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}



void
vtkPerkProcedureEvaluatorGUI
::ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData )
{
  vtkMRMLPerkProcedureNode* procedureNode = vtkMRMLPerkProcedureNode::SafeDownCast( caller );
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( caller );
  vtkMRMLScalarVolumeDisplayNode* volumeNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast( caller );
  
  
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}



void
vtkPerkProcedureEvaluatorGUI
::ProcessTimerEvents()
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
vtkPerkProcedureEvaluatorGUI
::BuildGUI()
{
  this->Logic->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLPerkProcedureNode >::New() );
  
  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "PerkProcedureEvaluator", "PerkProcedureEvaluator", NULL );

  this->BuildGUIForHelpFrame();
  this->BuildGUIForInputFrame();
  this->BuildGUIForPlaybackFrame();
  this->BuildGUIForNotesList();
  this->BuildGUIForResultsFrame();
}



void vtkPerkProcedureEvaluatorGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:PerkProcedureEvaluator</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "PerkProcedureEvaluator" );
  this->BuildHelpAndAboutFrame (page, help, about);
}



void
vtkPerkProcedureEvaluatorGUI
::BuildGUIForInputFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("PerkProcedureEvaluator");
  
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > inputFrame = vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    inputFrame->SetParent( page );
    inputFrame->Create();
    inputFrame->SetLabelText( "Input" );
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               inputFrame->GetWidgetName(), page->GetWidgetName() );
  
  
  if ( ! this->PerkProcedureSelector )
    {
    this->PerkProcedureSelector = vtkSlicerNodeSelectorWidget::New();
    this->PerkProcedureSelector->SetNodeClass( "vtkMRMLPerkProcedureNode", NULL, NULL, "PerkProcedure" );
    this->PerkProcedureSelector->SetParent( inputFrame->GetFrame() );
    this->PerkProcedureSelector->Create();
    this->PerkProcedureSelector->SetNewNodeEnabled( 1 );
    this->PerkProcedureSelector->NoneEnabledOff();
    this->PerkProcedureSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
    this->PerkProcedureSelector->UpdateMenu();
    this->PerkProcedureSelector->SetLabelText( "Perk procedure" );
    this->PerkProcedureSelector->SetSelectedNew( "vtkMRMLPerkProcedureNode" );
    }
  
  if ( ! this->PlanningVolumeSelector )
    {
    this->PlanningVolumeSelector = vtkSlicerNodeSelectorWidget::New();
    this->PlanningVolumeSelector->SetNodeClass( "vtkMRMLScalarVolumeNode", NULL, NULL, "Planning volume" );
    this->PlanningVolumeSelector->SetParent( inputFrame->GetFrame() );
    this->PlanningVolumeSelector->Create();
    this->PlanningVolumeSelector->NoneEnabledOn();
    this->PlanningVolumeSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
    this->PlanningVolumeSelector->UpdateMenu();
    this->PlanningVolumeSelector->SetLabelText( "Planning image" );
    }
  
  if ( ! this->CalibrationSelector )
    {
    this->CalibrationSelector = vtkSlicerNodeSelectorWidget::New();
    this->CalibrationSelector->SetNodeClass( "vtkMRMLTransformNode", NULL, NULL, "Calibration transform" );
    this->CalibrationSelector->SetParent( inputFrame->GetFrame() );
    this->CalibrationSelector->Create();
    this->CalibrationSelector->NoneEnabledOn();
    this->CalibrationSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
    this->CalibrationSelector->UpdateMenu();
    this->CalibrationSelector->SetLabelText( "Calibration transform" );
    }
  
  if ( ! this->NeedleTransformSelector )
    {
    this->NeedleTransformSelector = vtkSlicerNodeSelectorWidget::New();
    this->NeedleTransformSelector->SetNodeClass( "vtkMRMLTransformNode", NULL, NULL, "Needle tip transform" );
    this->NeedleTransformSelector->SetParent( inputFrame->GetFrame() );
    this->NeedleTransformSelector->Create();
    this->NeedleTransformSelector->NoneEnabledOn();
    this->NeedleTransformSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
    this->NeedleTransformSelector->UpdateMenu();
    this->NeedleTransformSelector->SetLabelText( "Needle tip transform" );
    }
  
  this->Script( "pack %s %s %s %s -side top -fill x -padx 2 -pady 2", 
                this->PerkProcedureSelector->GetWidgetName(),
                this->PlanningVolumeSelector->GetWidgetName(),
                this->CalibrationSelector->GetWidgetName(),
                this->NeedleTransformSelector->GetWidgetName() );
  
  
  if ( ! this->LoadButton )
    {
    this->LoadButton = vtkKWLoadSaveButton::New();
    this->LoadButton->SetParent( inputFrame->GetFrame() );
    this->LoadButton->Create();
    this->LoadButton->SetText( "Load data" );
    }
  
  this->Script( "pack %s -side top -fill x -padx 2 -pady 2", 
                this->LoadButton->GetWidgetName() );
  
}



void
vtkPerkProcedureEvaluatorGUI
::BuildGUIForNotesList()
{
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("PerkProcedureEvaluator");
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  
  
  if ( ! this->NotesFrame )
    {
    this->NotesFrame = vtkSlicerModuleCollapsibleFrame::New();
    this->NotesFrame->SetParent( page );
    this->NotesFrame->Create();
    this->NotesFrame->SetLabelText( "Procedure" );
    app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                 this->NotesFrame->GetWidgetName(), page->GetWidgetName() );
    }
  
  
  if ( ! this->NotesList )
    {
    this->NotesList = vtkKWMultiColumnListWithScrollbars::New();
    this->NotesList->SetParent( this->NotesFrame->GetFrame() );
    this->NotesList->Create();
    this->NotesList->SetHeight( 1 );
    this->NotesList->GetWidget()->SetSelectionTypeToRow();
    this->NotesList->GetWidget()->SetSelectionBackgroundColor( 1, 0, 0 );
    this->NotesList->GetWidget()->MovableRowsOff();
    this->NotesList->GetWidget()->MovableColumnsOff();
    this->NotesList->GetWidget()->SetSelectionModeToSingle();
    
      // Create the columns.
    
    for ( int col = 0; col < NOTES_COL_COUNT; ++ col )
      {
      this->NotesList->GetWidget()->AddColumn( NOTES_COL_LABELS[ col ] );
      this->NotesList->GetWidget()->SetColumnWidth( col, NOTES_COL_WIDTHS[ col ] );
      this->NotesList->GetWidget()->SetColumnAlignmentToLeft( col );
      }
    
    this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                  this->NotesList->GetWidgetName() );
  
    }
}



void
vtkPerkProcedureEvaluatorGUI
::BuildGUIForPlaybackFrame()
{
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "PerkProcedureEvaluator" );
  vtkSlicerApplication *app = (vtkSlicerApplication*)this->GetApplication();
  
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > playbackFrame = vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
  playbackFrame->SetParent( page );
  playbackFrame->Create();
  playbackFrame->SetLabelText( "Playback" );
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               playbackFrame->GetWidgetName(), page->GetWidgetName() );
  
    // Buttons frame.
    
  vtkSmartPointer< vtkKWFrame > buttonsFrame = vtkSmartPointer< vtkKWFrame >::New();
  buttonsFrame->SetParent( playbackFrame->GetFrame() );
  buttonsFrame->Create();
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2", buttonsFrame->GetWidgetName() );
  
  
  CREATE_SETPARENT( this->ButtonBegin, vtkKWPushButton, buttonsFrame );
  this->ButtonBegin->SetText( "Begin" );
  this->ButtonBegin->SetReliefToRaised();
  CREATE_SETPARENT( this->ButtonPrevious, vtkKWPushButton, buttonsFrame );
  this->ButtonPrevious->SetText( "Prev" );
  this->ButtonPrevious->SetReliefToRaised();
  CREATE_SETPARENT( this->ButtonNext, vtkKWPushButton, buttonsFrame );
  this->ButtonNext->SetText( "Next" );
  this->ButtonNext->SetReliefToRaised();
  CREATE_SETPARENT( this->ButtonEnd, vtkKWPushButton, buttonsFrame );
  this->ButtonEnd->SetText( "End" );
  this->ButtonEnd->SetReliefToRaised();
  CREATE_SETPARENT( this->ButtonPlay, vtkKWPushButton, buttonsFrame );
  this->ButtonPlay->SetText( "Play" );
  this->ButtonPlay->SetReliefToRaised();
  CREATE_SETPARENT( this->ButtonStop, vtkKWPushButton, buttonsFrame );
  this->ButtonStop->SetText( "Stop" );
  this->ButtonStop->SetReliefToRaised();
  vtkSmartPointer< vtkKWLabel > timeLabel = vtkSmartPointer< vtkKWLabel >::New();
    timeLabel->SetParent( buttonsFrame );
    timeLabel->Create();
    timeLabel->SetText( " Time:" );
  CREATE_SETPARENT( this->EntrySec, vtkKWEntry, buttonsFrame );
  this->EntrySec->SetWidth( 7 );
  CREATE_SETPARENT( this->ButtonGo, vtkKWPushButton, buttonsFrame );
  this->ButtonGo->SetText( "Go" );
  this->ButtonGo->SetReliefToRaised();
  
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->ButtonBegin->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->ButtonPrevious->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->ButtonNext->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->ButtonEnd->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->ButtonPlay->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->ButtonStop->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", timeLabel->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->EntrySec->GetWidgetName() );
  app->Script( "pack %s -side left -anchor nw -padx 1 -pady 2", this->ButtonGo->GetWidgetName() );
  
  
    // Position label.
  
  CREATE_SETPARENT( this->PositionLabel, vtkKWLabel, playbackFrame->GetFrame() );
  this->PositionLabel->SetText( "Needle position: " );
  app->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", this->PositionLabel->GetWidgetName() );
}



void
vtkPerkProcedureEvaluatorGUI
::BuildGUIForResultsFrame()
{
  
}



void
vtkPerkProcedureEvaluatorGUI
::UpdateAll()
{
  vtkMRMLPerkProcedureNode* procedure = this->GetProcedureNode();
  if ( ! procedure ) return;
  
  
    // Update list of procedure notes.
  
  if (    this->NotesList != NULL
       && this->NotesList->GetWidget() != NULL )
    {
    int numNotes = procedure->GetNumberOfNotes();
    
    bool deleteFlag = true;
    if ( numNotes != this->NotesList->GetWidget()->GetNumberOfRows() )
      {
      this->NotesList->GetWidget()->DeleteAllRows();
      }
    else
      {
      deleteFlag = false;
      }
    
    for ( int row = 0; row < numNotes; ++ row )
      {
      PerkNote* note = procedure->GetNoteAtIndex( row );
      
      if ( deleteFlag )
        {
        this->NotesList->GetWidget()->AddRow();
        }
      
      vtkKWMultiColumnList* colList = this->NotesList->GetWidget();
      
      colList->SetCellText( row, NOTES_COL_TIME, DoubleToStr( note->Time ).c_str() );
      colList->SetCellText( row, NOTES_COL_MESSAGE, note->Message.c_str() );
      }
    }
  
  
    // Playback.
  
  this->EntrySec->SetValueAsDouble( procedure->GetTimeAtTransformIndex( procedure->GetTransformIndex() ) );
}



void
vtkPerkProcedureEvaluatorGUI
::ProcessLoadButton()
{
  // this->LoadButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry( "PerkProcedurePath" );
  
  
  vtkMRMLPerkProcedureNode* selectedNode =
    vtkMRMLPerkProcedureNode::SafeDownCast( this->PerkProcedureSelector->GetSelected() );
  if ( ! selectedNode ) return;
  
  
  const char* fileName = this->LoadButton->GetLoadSaveDialog()->GetFileName();
  if ( fileName )
    {
    this->LoadButton->GetLoadSaveDialog()->SaveLastPathToRegistry( "PerkProcedurePath" );
    this->ProcedureNode->ImportFromFile( fileName );
    }
    
  this->UpdateAll();
}



void
vtkPerkProcedureEvaluatorGUI
::ProcessProcedureSelected()
{
  vtkMRMLPerkProcedureNode* node = vtkMRMLPerkProcedureNode::SafeDownCast( this->PerkProcedureSelector->GetSelected() );
  this->SetProcedureNode( node );
  vtkSetAndObserveMRMLNodeMacro( this->ProcedureNode, node );
  
  this->UpdateAll();
}



void
vtkPerkProcedureEvaluatorGUI
::OnNoteSelectionChanged()
{
  int numSelRows = this->NotesList->GetWidget()->GetNumberOfSelectedRows();
  if ( numSelRows != 1 ) return;
  
  vtkMRMLPerkProcedureNode* procedure = this->GetProcedureNode();
  
  int rowIndex = this->NotesList->GetWidget()->GetIndexOfFirstSelectedRow();
  procedure->SetNoteIndex( rowIndex );
  
  this->UpdateAll();
}

