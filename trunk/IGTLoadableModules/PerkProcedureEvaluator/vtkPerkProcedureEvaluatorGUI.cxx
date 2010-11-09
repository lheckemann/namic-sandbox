
#include "vtkPerkProcedureEvaluatorGUI.h"


#include <sstream>
#include <string>

#include "vtkCornerAnnotation.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"

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
#include "vtkMRMLStorableNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkMRMLPerkProcedureStorageNode.h"

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
  this->PlanFiducialsSelector = NULL;
  this->BoxFiducialsSelector = NULL;
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
  this->LabelSurfaceInside = NULL;
  this->LabelAngleFromAxial = NULL;
  this->LabelAngleInAxial = NULL;
  
  
  this->TimerFlag = 0;
  this->ProcedureNode = NULL;
  this->AutoPlayOn = false;
  this->TimerLog = vtkTimerLog::New();
  this->TimerEventProcessing = false;
}



vtkPerkProcedureEvaluatorGUI
::~vtkPerkProcedureEvaluatorGUI()
{
  
  if ( this->ProcedureNode )
    {
    vtkSetMRMLNodeMacro( this->ProcedureNode, NULL );
    }
  
  
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
  
  
  
    // Remove GUI widgets

  DELETE_WITH_SETPARENT_NULL( this->PerkProcedureSelector );
  DELETE_WITH_SETPARENT_NULL( this->PlanFiducialsSelector );
  DELETE_WITH_SETPARENT_NULL( this->BoxFiducialsSelector );
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
  DELETE_WITH_SETPARENT_NULL( this->LabelSurfaceInside );
  DELETE_WITH_SETPARENT_NULL( this->LabelAngleFromAxial );
  DELETE_WITH_SETPARENT_NULL( this->LabelAngleInAxial );
  
  
    // Unregister Logic class

  this->SetModuleLogic ( NULL );
  
  
  
  if ( this->TimerLog )
    {
    this->TimerLog->Delete();
    this->TimerLog = NULL;
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
  REMOVE_OBSERVERS( this->PlanFiducialsSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->BoxFiducialsSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  REMOVE_OBSERVERS( this->NeedleTransformSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  
  REMOVE_OBSERVERS( this->LoadButton, vtkKWPushButton::InvokedEvent );
  
  
    // Playback.
  
  REMOVE_OBSERVERS( this->ButtonBegin, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonEnd, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonNext, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonPrevious, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonPlay, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonStop, vtkKWPushButton::InvokedEvent );
  
  
    // Results frame.
  
  REMOVE_OBSERVERS( this->ButtonMeasureBegin, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonMeasureEnd, vtkKWPushButton::InvokedEvent );
  REMOVE_OBSERVERS( this->ButtonMeasure, vtkKWPushButton::InvokedEvent );
  
  
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
  ADD_OBSERVER( this->PlanFiducialsSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  ADD_OBSERVER( this->BoxFiducialsSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  ADD_OBSERVER( this->NeedleTransformSelector, vtkSlicerNodeSelectorWidget::NodeSelectedEvent );
  
  ADD_OBSERVER( this->LoadButton->GetLoadSaveDialog(), vtkKWTopLevel::WithdrawEvent );
  
  
    // Playback.
  
  ADD_OBSERVER( this->ButtonBegin, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonEnd, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonNext, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonPrevious, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonPlay, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonStop, vtkKWPushButton::InvokedEvent );
  
  
    // Results frame.
  
  ADD_OBSERVER( this->ButtonMeasureBegin, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonMeasureEnd, vtkKWPushButton::InvokedEvent );
  ADD_OBSERVER( this->ButtonMeasure, vtkKWPushButton::InvokedEvent );
  
  
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
  
  else if (    this->PlanFiducialsSelector == vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
            && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    vtkMRMLFiducialListNode* fids = vtkMRMLFiducialListNode::SafeDownCast(
      this->PlanFiducialsSelector->GetSelected() );
    this->ProcedureNode->SetPlan( fids );
    this->UpdateAll();
    }
  
  else if (    this->BoxFiducialsSelector == vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
            && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    if ( this->ProcedureNode )
      {
      vtkMRMLFiducialListNode* fiducials = vtkMRMLFiducialListNode::SafeDownCast(
        this->BoxFiducialsSelector->GetSelected() );
      this->ProcedureNode->BoxShapeFromFiducials( fiducials );
      }
    }
  
  else if (    this->NeedleTransformSelector == vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
            && (    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
                 || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    if ( this->ProcedureNode )
      {
      vtkMRMLLinearTransformNode* node = vtkMRMLLinearTransformNode::SafeDownCast(
        this->NeedleTransformSelector->GetSelected() );
      this->ProcedureNode->SetAndObserveNeedleTransformNodeID( node->GetID() );
      }
    }
  
  else if (    this->LoadButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast( caller )
            && event == vtkKWTopLevel::WithdrawEvent )
    {
    this->ProcessLoadButton();
    }
  
  
    // Playback.
  
  else if (    this->ButtonBegin == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    this->ProcedureNode->SetTransformIndex( 0 );
    this->UpdateAll();
    }
  
  else if (    this->ButtonEnd == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    this->ProcedureNode->SetTransformIndex( this->ProcedureNode->GetNumberOfTransforms() );
    this->UpdateAll();
    }
  
  else if (    this->ButtonNext == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    this->ProcedureNode->SetTransformIndex( this->ProcedureNode->GetTransformIndex() + 5 );
    this->UpdateAll();
    }
  
  else if (    this->ButtonPrevious == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    this->ProcedureNode->SetTransformIndex( this->ProcedureNode->GetTransformIndex() - 5 );
    this->UpdateAll();
    }
  
  else if (    this->ButtonPlay == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    if ( ! this->AutoPlayOn ) this->AutoPlayOn = true;
    
    this->TimerLog->StartTimer();
    vtkKWTkUtilities::CreateTimerHandler( this->GetApplication(), 0, this, "TimerHandler" );
    }
  
  else if (    this->ButtonStop == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    if ( this->AutoPlayOn ) this->AutoPlayOn = false;
    this->TimerLog->StopTimer();
    this->TimerEventProcessing = false;
    }
  
  
    // Measure results.
  
  else if (    this->ButtonMeasureBegin == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    this->ProcedureNode->MarkIndexBegin();
    this->UpdateAll();
    }
  
  else if (    this->ButtonMeasureEnd == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    this->ProcedureNode->MarkIndexEnd();
    this->UpdateAll();
    }
  
  else if (    this->ButtonMeasure == vtkKWPushButton::SafeDownCast( caller )
            && event == vtkKWPushButton::InvokedEvent )
    {
    this->ProcedureNode->UpdateMeasurements();
    this->UpdateAll();
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
  
  
  if ( procedureNode != NULL && this->ProcedureNode != NULL )
    {
    this->UpdateAll();
    }
  
  
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
  
  this->TimerHandler();
}



void
vtkPerkProcedureEvaluatorGUI
::BuildGUI()
{
  this->Logic->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLBoxShape >::New() );
  this->Logic->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLPerkProcedureNode >::New() );
  this->Logic->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLPerkProcedureStorageNode >::New() );
  // this->Logic->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLSurgicalShape >::New() );
  
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
    // this->PerkProcedureSelector->NoneEnabledOff();
    this->PerkProcedureSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
    this->PerkProcedureSelector->UpdateMenu();
    this->PerkProcedureSelector->SetLabelText( "Perk procedure" );
    this->PerkProcedureSelector->SetSelectedNew( "vtkMRMLPerkProcedureNode" );
    }
  
  if ( ! this->PlanFiducialsSelector )
    {
    this->PlanFiducialsSelector = vtkSlicerNodeSelectorWidget::New();
    this->PlanFiducialsSelector->SetNodeClass( "vtkMRMLFiducialListNode", NULL, NULL, "Plan fiducial list" );
    this->PlanFiducialsSelector->SetParent( inputFrame->GetFrame() );
    this->PlanFiducialsSelector->Create();
    this->PlanFiducialsSelector->NoneEnabledOn();
    this->PlanFiducialsSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
    this->PlanFiducialsSelector->UpdateMenu();
    this->PlanFiducialsSelector->SetLabelText( "Plan fiducial list" );
    }
  
  if ( ! this->BoxFiducialsSelector )
    {
    this->BoxFiducialsSelector = vtkSlicerNodeSelectorWidget::New();
    this->BoxFiducialsSelector->SetNodeClass( "vtkMRMLFiducialListNode", NULL, NULL, "Box fiducial list" );
    this->BoxFiducialsSelector->SetParent( inputFrame->GetFrame() );
    this->BoxFiducialsSelector->Create();
    this->BoxFiducialsSelector->NoneEnabledOn();
    this->BoxFiducialsSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
    this->BoxFiducialsSelector->UpdateMenu();
    this->BoxFiducialsSelector->SetLabelText( "Box fiducial list" );
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
                this->PlanFiducialsSelector->GetWidgetName(),
                this->BoxFiducialsSelector->GetWidgetName(),
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
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "PerkProcedureEvaluator" );
  vtkSlicerApplication *app = (vtkSlicerApplication*)this->GetApplication();
  
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > resultsFrame = vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
  resultsFrame->SetParent( page );
  resultsFrame->Create();
  resultsFrame->SetLabelText( "Measurement Results" );
  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               resultsFrame->GetWidgetName(), page->GetWidgetName() );
  
  
    // Label begin.
  
  vtkSmartPointer< vtkKWLabel > labelBegin = vtkSmartPointer< vtkKWLabel >::New();
    labelBegin->SetParent( resultsFrame->GetFrame() );
    labelBegin->Create();
    labelBegin->SetText( "Begin of measurement" );
    
  if ( ! this->LabelBegin )
    {
    this->LabelBegin = vtkKWLabel::New();
    this->LabelBegin->SetParent( resultsFrame->GetFrame() );
    this->LabelBegin->Create();
    this->LabelBegin->SetText( "-" );
    this->LabelBegin->SetWidth( 10 );
    }
  
  if ( ! this->ButtonMeasureBegin )
    {
    this->ButtonMeasureBegin = vtkKWPushButton::New();
    this->ButtonMeasureBegin->SetParent( resultsFrame->GetFrame() );
    this->ButtonMeasureBegin->Create();
    this->ButtonMeasureBegin->SetText( "Mark Begin" );
    }
  
  this->Script( "grid %s -column 0 -row 0 -sticky w -padx 4 -pady 1", labelBegin->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 0 -sticky w -padx 4 -pady 1", this->LabelBegin->GetWidgetName() );
  this->Script( "grid %s -column 2 -row 0 -sticky w -padx 4 -pady 1", this->ButtonMeasureBegin->GetWidgetName() );
  
  
    // Label end.
  
  vtkSmartPointer< vtkKWLabel > labelEnd = vtkSmartPointer< vtkKWLabel >::New();
    labelEnd->SetParent( resultsFrame->GetFrame() );
    labelEnd->Create();
    labelEnd->SetText( "End of measurement" );
  
  if ( ! this->LabelEnd )
    {
    this->LabelEnd = vtkKWLabel::New();
    this->LabelEnd->SetParent( resultsFrame->GetFrame() );
    this->LabelEnd->Create();
    this->LabelEnd->SetText( "-" );
    this->LabelEnd->SetWidth( 10 );
    }
  
  if ( ! this->ButtonMeasureEnd )
    {
    this->ButtonMeasureEnd = vtkKWPushButton::New();
    this->ButtonMeasureEnd->SetParent( resultsFrame->GetFrame() );
    this->ButtonMeasureEnd->Create();
    this->ButtonMeasureEnd->SetText( "Mark End" );
    }
  
  this->Script( "grid %s -column 0 -row 1 -sticky w -padx 4 -pady 1", labelEnd->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 1 -sticky w -padx 4 -pady 1", this->LabelEnd->GetWidgetName() );
  this->Script( "grid %s -column 2 -row 1 -sticky w -padx 4 -pady 1", this->ButtonMeasureEnd->GetWidgetName() );
  
  
    // Measure button.
  
  if ( ! this->ButtonMeasure )
    {
    this->ButtonMeasure = vtkKWPushButton::New();
    this->ButtonMeasure->SetParent( resultsFrame->GetFrame() );
    this->ButtonMeasure->Create();
    this->ButtonMeasure->SetText( "Perform Measurements" );
    }
  
  this->Script( "grid %s -column 2 -row 2 -sticky w -padx 4 -pady 1", this->ButtonMeasure->GetWidgetName() );
  
  
    // Total time.
  
  vtkSmartPointer< vtkKWLabel > labelTotalTime = vtkSmartPointer< vtkKWLabel >::New();
    labelTotalTime->SetParent( resultsFrame->GetFrame() );
    labelTotalTime->Create();
    labelTotalTime->SetText( "Total procedure time (sec): " );
  
  if ( ! this->LabelTotalTime )
    {
    this->LabelTotalTime = vtkKWLabel::New();
    this->LabelTotalTime->SetParent( resultsFrame->GetFrame() );
    this->LabelTotalTime->Create();
    this->LabelTotalTime->SetText( " - " );
    }
  
  this->Script( "grid %s -column 0 -row 3 -sticky w -padx 4 -pady 1", labelTotalTime->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 3 -sticky w -padx 4 -pady 1", this->LabelTotalTime->GetWidgetName() );
  
  
    // Label path inside.
  
  vtkSmartPointer< vtkKWLabel > labelPathInside = vtkSmartPointer< vtkKWLabel >::New();
    labelPathInside->SetParent( resultsFrame->GetFrame() );
    labelPathInside->Create();
    labelPathInside->SetText( "Path inside body (mm): " );
  
  if ( ! this->LabelPathInside )
    {
    this->LabelPathInside = vtkKWLabel::New();
    this->LabelPathInside->SetParent( resultsFrame->GetFrame() );
    this->LabelPathInside->Create();
    this->LabelPathInside->SetText( " - " );
    }
  
  this->Script( "grid %s -column 0 -row 4 -sticky w -padx 4 -pady 1", labelPathInside->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 4 -sticky w -padx 4 -pady 1", this->LabelPathInside->GetWidgetName() );
  
  
    // Label time inside.
  
  vtkSmartPointer< vtkKWLabel > labelTimeInside = vtkSmartPointer< vtkKWLabel >::New();
    labelTimeInside->SetParent( resultsFrame->GetFrame() );
    labelTimeInside->Create();
    labelTimeInside->SetText( "Time inside body (sec): " );
  
  if ( ! this->LabelTimeInside )
    {
    this->LabelTimeInside = vtkKWLabel::New();
    this->LabelTimeInside->SetParent( resultsFrame->GetFrame() );
    this->LabelTimeInside->Create();
    }
  
  this->Script( "grid %s -column 0 -row 5 -sticky w -padx 4 -pady 1", labelTimeInside->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 5 -sticky w -padx 4 -pady 1", this->LabelTimeInside->GetWidgetName() );
  
  
    // Label surface inside.
  
  
  vtkSmartPointer< vtkKWLabel > labelSurfaceInside = vtkSmartPointer< vtkKWLabel >::New();
    labelSurfaceInside->SetParent( resultsFrame->GetFrame() );
    labelSurfaceInside->Create();
    labelSurfaceInside->SetText( "Tissue surface covered (mm2): " );
  
  if ( ! this->LabelSurfaceInside )
    {
    this->LabelSurfaceInside = vtkKWLabel::New();
    this->LabelSurfaceInside->SetParent( resultsFrame->GetFrame() );
    this->LabelSurfaceInside->Create();
    }
  
  this->Script( "grid %s -column 0 -row 6 -sticky w -padx 4 -pady 1",
                labelSurfaceInside->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 6 -sticky w -padx 4 -pady 1",
                this->LabelSurfaceInside->GetWidgetName() );
  
  
    // Label angle deviations.
  
  vtkSmartPointer< vtkKWLabel > labelAngleFromAxial = vtkSmartPointer< vtkKWLabel >::New();
    labelAngleFromAxial->SetParent( resultsFrame->GetFrame() );
    labelAngleFromAxial->Create();
    labelAngleFromAxial->SetText( "Deviation from axial plane (deg): " );
  
  if ( ! this->LabelAngleFromAxial )
    {
    this->LabelAngleFromAxial = vtkKWLabel::New();
    this->LabelAngleFromAxial->SetParent( resultsFrame->GetFrame() );
    this->LabelAngleFromAxial->Create();
    }
  
  this->Script( "grid %s -column 0 -row 7 -sticky w -padx 4 -pady 1", labelAngleFromAxial->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 7 -sticky w -padx 4 -pady 1", this->LabelAngleFromAxial->GetWidgetName() );
  
  
  vtkSmartPointer< vtkKWLabel > labelAngleInAxial = vtkSmartPointer< vtkKWLabel >::New();
    labelAngleInAxial->SetParent( resultsFrame->GetFrame() );
    labelAngleInAxial->Create();
    labelAngleInAxial->SetText( "Deviation in axial plane (deg): " );
  
  if ( ! this->LabelAngleInAxial )
    {
    this->LabelAngleInAxial = vtkKWLabel::New();
    this->LabelAngleInAxial->SetParent( resultsFrame->GetFrame() );
    this->LabelAngleInAxial->Create();
    }
  
  this->Script( "grid %s -column 0 -row 8 -sticky w -padx 4 -pady 1", labelAngleInAxial->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 8 -sticky w -padx 4 -pady 1", this->LabelAngleInAxial->GetWidgetName() );
  
}



void
vtkPerkProcedureEvaluatorGUI
::TearDownGUI()
{
    // Remove Observers
  
  this->RemoveGUIObservers();
  
  if ( this->ProcedureNode )
    {
    vtkSetAndObserveMRMLNodeMacro( this->ProcedureNode, NULL );
    }
}



void
vtkPerkProcedureEvaluatorGUI
::UpdateAll()
{
  vtkMRMLPerkProcedureNode* procedure = this->GetProcedureNode();
  if ( ! procedure ) return;
  
  
  // debug
  int ib = procedure->GetIndexBegin();
  int ie = procedure->GetIndexEnd();
  const char* pID = procedure->GetID();
  
  
  
    // Update list of procedure notes.
  
  if (    this->NotesList != NULL
       && this->NotesList->GetWidget() != NULL )
    {
    int numNotes = procedure->GetNumberOfNotes();
    
    bool deleteFlag = true;
    //if ( numNotes != this->NotesList->GetWidget()->GetNumberOfRows() )
    //  {
      this->NotesList->GetWidget()->DeleteAllRows();
    //  }
    //else
    //  {
    //  deleteFlag = false;
    //  }
    
    if ( deleteFlag )
      {
      for ( int row = 0; row < numNotes; ++ row )
        {
        PerkNote* note = procedure->GetNoteAtIndex( row );
        
        this->NotesList->GetWidget()->AddRow();
        
        vtkKWMultiColumnList* colList = this->NotesList->GetWidget();
        
        colList->SetCellText( row, NOTES_COL_TIME, DoubleToStr( note->Time ).c_str() );
        colList->SetCellText( row, NOTES_COL_MESSAGE, note->Message.c_str() );
        }
      }
    }
  
  
    // Playback.
  this->UpdatePlayback();
  
  
    // Measurement results.
  
  
  
  if ( procedure->GetIndexBegin() >= 0 )
    {
    this->LabelBegin->SetText( DoubleToStr( procedure->GetTimeAtTransformIndex( procedure->GetIndexBegin() ) ).c_str() );
    }
  
  if ( procedure->GetIndexEnd() >= 0 )
    {
    this->LabelEnd->SetText( DoubleToStr( procedure->GetTimeAtTransformIndex( procedure->GetIndexEnd() ) ).c_str() );
    }
  
  if ( procedure->GetIndexBegin() >= 0 && procedure->GetIndexEnd() >= 0 )
    {
    this->LabelTotalTime->SetText( DoubleToStr( procedure->GetTotalTime() ).c_str() );
    this->LabelPathInside->SetText( DoubleToStr( procedure->GetPathInside() ).c_str() );
    this->LabelTimeInside->SetText( DoubleToStr( procedure->GetTimeInside() ).c_str() );
    this->LabelSurfaceInside->SetText( DoubleToStr( procedure->GetSurfaceInside() ).c_str() );
    this->LabelAngleFromAxial->SetText( DoubleToStr( procedure->GetAngleFromAxial() ).c_str() );
    
    if ( procedure->GetAngleInAxial() < 0.0 )
      {
      this->LabelAngleInAxial->SetText( "N/A" );
      }
    else
      {
      this->LabelAngleInAxial->SetText( DoubleToStr( procedure->GetAngleInAxial() ).c_str() );
      }
    }
}



void
vtkPerkProcedureEvaluatorGUI
::UpdatePlayback()
{
  vtkMRMLPerkProcedureNode* procedure = this->GetProcedureNode();
  if ( ! procedure ) return;
  
  this->EntrySec->SetValueAsDouble( procedure->GetTimeAtTransformIndex( procedure->GetTransformIndex() ) );
  
  vtkTransform* txform = procedure->GetTransformAtTransformIndex( procedure->GetTransformIndex() );
  if ( txform )
    {
    vtkMatrix4x4* mtx = txform->GetMatrix();
    std::stringstream ss;
    ss << mtx->GetElement( 0, 3 ) << " " << mtx->GetElement( 1, 3 ) << " " << mtx->GetElement( 2, 3 );
    if ( procedure->IsNeedleInsideBody() ) ss << " (inside body)";
    else ss << " (outside body)";
    this->PositionLabel->SetText( ss.str().c_str() );
    }
}



void
vtkPerkProcedureEvaluatorGUI
::TimerHandler()
{
  if ( ! this->AutoPlayOn ) return;
  
  if ( this->TimerEventProcessing ) return;
  this->TimerEventProcessing = true;
  
  
  int index = this->ProcedureNode->GetTransformIndex();
  double now = this->ProcedureNode->GetTimeAtTransformIndex( index );
  double then = this->ProcedureNode->GetTimeAtTransformIndex( index + 1 );
  
  if ( then < now )
    {
    this->AutoPlayOn = false;
    this->TimerLog->StopTimer();
    this->TimerEventProcessing = false;
    return;
    }
  
  
    // Do the changes.
  
  this->ProcedureNode->SetTransformIndex( this->ProcedureNode->GetTransformIndex() + 1 );
  this->UpdatePlayback();
  // this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
  // this->GetApplicationGUI()->UpdateMain3DViewers();
  
  // this->EntrySec->SetValueAsDouble( this->ProcedureNode->GetTimeAtTransformIndex( this->ProcedureNode->GetTransformIndex() ) );
  // vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  // vtkKWWidget *page = this->UIPanel->GetPageWidget ("PerkProcedureEvaluator");
  
  
  
  this->TimerLog->StopTimer();
  double elapsed = this->TimerLog->GetElapsedTime();
  this->TimerLog->StartTimer();
  
  double delay = then - now - elapsed;
  if ( delay < 0.0 ) delay = 0.0;
  std::stringstream ss;
  ss << ( (int)( delay ) );
  
  this->Script ( "update idletasks" );
  // this->Script ( "after 10 \"%s TimerHandler \"",  this->GetTclName() );
  this->Script ( "after %d \"%s TimerHandler \"", (int)delay, this->GetTclName() );
  // vtkKWTkUtilities::CreateTimerHandler( this->GetApplication(), delay, this, "TimerHandler" );
  
  this->TimerEventProcessing = false;
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
  
  
    // Why is this becoming NULL?
  vtkMRMLStorableNode* storableNode = vtkMRMLStorableNode::SafeDownCast( node );
  
  
  if (    node
       && node->GetStorageNode() == NULL )
    {
    vtkMRMLStorageNode *snode = node->CreateDefaultStorageNode();
    if ( snode )
      {
      snode->SetScene( this->GetMRMLScene() );
      snode->SetFileName( "PerkProcedure.xml" );
      this->GetMRMLScene()->AddNode( snode );
      
      storableNode->SetAndObserveStorageNodeID( snode->GetID() );
      storableNode->ModifiedSinceReadOn();
      
      snode->Delete();
      }
    }
  
  
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

