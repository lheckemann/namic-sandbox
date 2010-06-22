
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <io.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().

#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"

#include "vtkCommand.h"
#include "vtkCornerAnnotation.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageActor.h"
#include "vtkImageChangeInformation.h"
#include "vtkImageReslice.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkObjectFactory.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkWin32OpenGLRenderWindow.h"

#include "vtkKWApplication.h"
#include "vtkKWComboBox.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWPushButton.h"
#include "vtkKWPushButtonSet.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLLayoutNode.h"

#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerModuleCollapsibleFrame.h"

#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationCalibrateStep.h"
#include "vtkPerkStationInsertStep.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationPlanStep.h"
#include "vtkPerkStationSecondaryMonitor.h"
#include "vtkPerkStationValidateStep.h"


/**
 * Factory.
 */
vtkPerkStationModuleGUI* vtkPerkStationModuleGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance(
    "vtkPerkStationModuleGUI" );
  if( ret )
    {
      return ( vtkPerkStationModuleGUI* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPerkStationModuleGUI;
}


/**
 * Constructor.
 */
vtkPerkStationModuleGUI
::vtkPerkStationModuleGUI()
{
    // gui elements
  
  this->VolumeSelector = vtkSmartPointer< vtkSlicerNodeSelectorWidget >::New();
  this->ValidationVolumeSelector = vtkSmartPointer< vtkSlicerNodeSelectorWidget >::New();
  this->PSNodeSelector = vtkSmartPointer< vtkSlicerNodeSelectorWidget >::New();
  
  this->LoadExperimentFileButton = vtkSmartPointer< vtkKWLoadSaveButton >::New();
  this->SaveExperimentFileButton = vtkSmartPointer< vtkKWLoadSaveButton >::New();
  
  this->CalibrateTimeLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->PlanTimeLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->InsertTimeLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->ValidateTimeLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->TimerButton = vtkSmartPointer< vtkKWPushButton >::New();
  this->ResetTimerButton = vtkSmartPointer< vtkKWPushButton >::New();
  
  for ( int i = 0; i < 4; ++ i ) this->WorkingTimes[ i ] = 0.0;
  this->TimerOn = false;
  this->LastTime = 0.0;
  
  this->Logic = NULL;
  this->MRMLNode = NULL;
  
  
  this->WizardWidget = vtkSmartPointer< vtkKWWizardWidget >::New();
  
  
    // secondary monitor
  
  this->SecondaryMonitor = vtkSmartPointer< vtkPerkStationSecondaryMonitor >::New();
    this->SecondaryMonitor->SetGUI( this );
    this->SecondaryMonitor->Initialize();
    
  
    // wizard workflow
  
  this->WizardFrame = vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();  
  this->CalibrateStep = vtkSmartPointer< vtkPerkStationCalibrateStep >::New();
  this->PlanStep = vtkSmartPointer< vtkPerkStationPlanStep >::New();
  this->InsertStep = vtkSmartPointer< vtkPerkStationInsertStep >::New();
  this->ValidateStep = vtkSmartPointer< vtkPerkStationValidateStep >::New();
  
  this->State = vtkPerkStationModuleGUI::Calibrate;  
  this->DisplayVolumeLevelValue = vtkSmartPointer< vtkKWScaleWithEntry >::New();
  this->DisplayVolumeWindowValue = vtkSmartPointer< vtkKWScaleWithEntry >::New();

  this->Entered = false;
  this->Built = false;
  this->SliceOffset = 0;
  
  this->ObserverCount = 0;
  
  
    // Workphase handling.
  
  this->WorkphaseButtonFrame = vtkSmartPointer< vtkKWFrame >::New();
  this->WorkphaseButtonSet = vtkSmartPointer< vtkKWPushButtonSet >::New();
  
  this->TimerLog = vtkSmartPointer< vtkTimerLog >::New();
}


// ----------------------------------------------------------------------------
vtkPerkStationModuleGUI::~vtkPerkStationModuleGUI()
{
  this->RemoveGUIObservers();
  
  vtkSlicerSliceLogic *sliceLogic =
    this->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic();
  if ( sliceLogic )
    {  
    sliceLogic->GetSliceNode()->RemoveObservers(
      vtkCommand::ModifiedEvent, ( vtkCommand *)( this->GUICallbackCommand ) );
    }
  
  this->SetLogic( NULL );

  if ( this->MRMLNode )
    {
    vtkSetMRMLNodeMacro( this->MRMLNode, NULL );
    }
}


//----------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::PrintSelf( ostream& os, vtkIndent indent )
{
  
}


//---------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::Enter( vtkMRMLNode *node )
{
  Enter();

  vtkMRMLPerkStationModuleNode *moduleNode = vtkMRMLPerkStationModuleNode::SafeDownCast( node );
  if ( moduleNode )
    {
    this->PSNodeSelector->UpdateMenu();
    this->PSNodeSelector->SetSelected( moduleNode ); // :TODO: check if observers are updated after this
    }

  this->UpdateGUI();
}


//------------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::Enter()
{
    // The user interface is hidden on Exit, show it now
  
  if ( this->WizardWidget != NULL )
    {
    vtkKWWizardWorkflow* wizard_workflow = this->WizardWidget->GetWizardWorkflow();
    if ( wizard_workflow != NULL )
      {
      vtkPerkStationStep* step = vtkPerkStationStep::SafeDownCast( wizard_workflow->GetCurrentStep() );
      if ( step )
        {
        step->ShowUserInterface();
        }
      }
    }
  
  
    // Set red slice only view.
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    p->RepackMainViewer ( vtkMRMLLayoutNode::SlicerLayoutOneUpSliceView, "Red");       
    }
  
  
  /*
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  if ( n == NULL )
    {
      // no parameter node selected yet, create new
    
    this->PSNodeSelector->SetSelectedNew( "vtkMRMLPerkStationModuleNode" );
    this->PSNodeSelector->ProcessNewNodeCommand( "vtkMRMLPerkStationModuleNode", "PS" );
    n = vtkMRMLPerkStationModuleNode::SafeDownCast( this->PSNodeSelector->GetSelected() );

      // set an observe new node in Logic
    
    this->Logic->SetAndObservePerkStationModuleNode( n );
    vtkSetAndObserveMRMLNodeMacro( this->MRMLNode, n );
    
      // add MRMLFiducialListNode to the scene
    
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode( this->MRMLNode->GetPlanMRMLFiducialListNode() );
    
    
    // add listener to the slice logic, so that any time user makes change to
    // slice viewer in laptop, the display needs to be updated on secondary
    // monitor e.g. user may move to a certain slice in a series of slices
  
    vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic();
    
    if ( sliceLogic )
      {  
      sliceLogic->GetSliceNode()->AddObserver( vtkCommand::ModifiedEvent,
                                               (vtkCommand*)( this->GUICallbackCommand ) );
      }
    }
  */
  
  this->Entered = true;
  
  this->AddMRMLObservers();
  this->UpdateGUI();
}


//------------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::Exit()
{
  this->RemoveGUIObservers();
  
  if ( this->WizardWidget != NULL )
    {
    vtkKWWizardWorkflow* wizard_workflow = this->WizardWidget->GetWizardWorkflow();
    if ( wizard_workflow != NULL )
      {
      vtkPerkStationStep* step = vtkPerkStationStep::SafeDownCast( wizard_workflow->GetCurrentStep() );
      if ( step )
        {
        step->HideUserInterface();
        }
      }
    }
}


/**
 * Set observers on widgets and GUI classes.
 * (GUICallbackCommand will call ProcessGUIEvents.)
 */
void vtkPerkStationModuleGUI::AddGUIObservers() 
{
  this->RemoveGUIObservers();
  
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  
    // Node selector and volume selector.
  
  this->PSNodeSelector->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                                    (vtkCommand*)( this->GUICallbackCommand ) );
  
  this->VolumeSelector->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                                     (vtkCommand*)( this->GUICallbackCommand ) );
  
  this->ValidationVolumeSelector->AddObserver( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                                               (vtkCommand*)( this->GUICallbackCommand ) );
    
  
    // Timer buttons.
  
  this->TimerButton->AddObserver( vtkKWPushButton::InvokedEvent, (vtkCommand*)( this->GUICallbackCommand ) );
  this->ResetTimerButton->AddObserver( vtkKWPushButton::InvokedEvent, (vtkCommand*)( this->GUICallbackCommand ) );
  
  
    // Red slice keyboard and mouse events.
  
  vtkInteractorStyle* iStyle = vtkInteractorStyle::SafeDownCast(
    appGUI->GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle() );
  
  
  iStyle->AddObserver( vtkCommand::LeftButtonPressEvent, ( vtkCommand* )this->GUICallbackCommand );
  iStyle->AddObserver( vtkCommand::KeyPressEvent, ( vtkCommand* )this->GUICallbackCommand );
  iStyle->AddObserver( vtkCommand::MouseMoveEvent, ( vtkCommand* )this->GUICallbackCommand );
  
  
    // Workphase pushbutton set.
  
  for ( int i = 0; i < this->WorkphaseButtonSet->GetNumberOfWidgets(); ++ i )
    {
    this->WorkphaseButtonSet->GetWidget( i )->AddObserver( vtkKWPushButton::InvokedEvent,
                                  ( vtkCommand* )( this->GUICallbackCommand ) );
    }
  
  
    // wizard workflow
    
  this->WizardWidget->GetWizardWorkflow()->AddObserver(
    vtkKWWizardWorkflow::CurrentStateChangedEvent,
    static_cast< vtkCommand* >( this->GUICallbackCommand ) );  

  this->VolumeSelector->AddObserver(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
    ( vtkCommand* )this->GUICallbackCommand );
  
  if ( this->LoadExperimentFileButton )
    {
    this->LoadExperimentFileButton->GetLoadSaveDialog()->AddObserver(
      vtkKWTopLevel::WithdrawEvent, ( vtkCommand* )this->GUICallbackCommand );
    }
  
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->GetLoadSaveDialog()->AddObserver(
      vtkKWTopLevel::WithdrawEvent, ( vtkCommand* )this->GUICallbackCommand );
    }
  
  this->DisplayVolumeLevelValue->AddObserver( vtkKWScale::ScaleValueChangedEvent,
    ( vtkCommand* )this->GUICallbackCommand );
  
  this->DisplayVolumeWindowValue->AddObserver( vtkKWScale::ScaleValueChangedEvent,
    ( vtkCommand* )this->GUICallbackCommand );

  this->ObserverCount++;
  
}



// ----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::RemoveGUIObservers ( )
{
  this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()
      ->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()
      ->RemoveObserver( (vtkCommand*)( this->GUICallbackCommand ) );
  
  
  this->WizardWidget->GetWizardWorkflow()->RemoveObserver(
    static_cast< vtkCommand* >( this->GUICallbackCommand ) );
  
  this->VolumeSelector->RemoveObservers(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
    ( vtkCommand* )this->GUICallbackCommand );

  this->PSNodeSelector->RemoveObservers(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
    ( vtkCommand* )this->GUICallbackCommand );    
  
  
  this->DisplayVolumeLevelValue->RemoveObservers(
    vtkKWScale::ScaleValueChangedEvent,
    ( vtkCommand* )this->GUICallbackCommand );

  this->DisplayVolumeWindowValue->RemoveObservers(
    vtkKWScale::ScaleValueChangedEvent,
    (vtkCommand*)( this->GUICallbackCommand ) );
  
  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(
      vtkKWTopLevel::WithdrawEvent, (vtkCommand*)( this->GUICallbackCommand ) );
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(
      vtkKWTopLevel::WithdrawEvent, (vtkCommand*)( this->GUICallbackCommand ) );
    }

  this->ObserverCount--;
}



void
vtkPerkStationModuleGUI
::AddMRMLObservers()
{
  if ( this->MRMLScene != NULL )
    {
    if ( this->MRMLScene->HasObserver( vtkMRMLScene::NodeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand ) < 1 )
      {
      this->MRMLScene->AddObserver( vtkMRMLScene::NodeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand );
      }
    if ( this->MRMLScene->HasObserver(vtkMRMLScene::NodeAboutToBeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand ) < 1)
      {
      this->MRMLScene->AddObserver(vtkMRMLScene::NodeAboutToBeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand );
      }
    if ( this->MRMLScene->HasObserver(vtkMRMLScene::NodeAddedEvent, (vtkCommand*)this->MRMLCallbackCommand ) < 1)
      {
      this->MRMLScene->AddObserver(vtkMRMLScene::NodeAddedEvent, (vtkCommand*)this->MRMLCallbackCommand );
      }
    if ( this->MRMLScene->HasObserver(vtkMRMLScene::SceneCloseEvent, (vtkCommand*)this->MRMLCallbackCommand ) < 1)
      {
      this->MRMLScene->AddObserver(vtkMRMLScene::SceneCloseEvent, (vtkCommand*)this->MRMLCallbackCommand );
      }
    }
}


void
vtkPerkStationModuleGUI
::RemoveMRMLObservers()
{
  if (this->MRMLScene!=NULL)
  {
    this->MRMLScene->RemoveObservers(vtkMRMLScene::NodeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand );
    this->MRMLScene->RemoveObservers(vtkMRMLScene::NodeAboutToBeRemovedEvent, (vtkCommand*)this->MRMLCallbackCommand );
    this->MRMLScene->RemoveObservers(vtkMRMLScene::NodeAddedEvent, (vtkCommand*)this->MRMLCallbackCommand );
    this->MRMLScene->RemoveObservers(vtkMRMLScene::SceneCloseEvent, (vtkCommand*)this->MRMLCallbackCommand );
  }
}


/**
 * Responds to events form widgets or slicer GUI.
 *
 * @param caller
 * @param event Event ID.
 * @param callData
 */
void
vtkPerkStationModuleGUI
::ProcessGUIEvents ( vtkObject*    caller,
                     unsigned long event,
                     void*         callData ) 
{
  const char* eventName = vtkCommand::GetStringFromEventId( event );
  
  
    // Module node is changed.
  
  if (    vtkSlicerNodeSelectorWidget::SafeDownCast( caller ) == this->PSNodeSelector
       && (    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
            || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    vtkMRMLPerkStationModuleNode* node =
      vtkMRMLPerkStationModuleNode::SafeDownCast( this->PSNodeSelector->GetSelected() );
    
    this->SetMRMLNode( node );
    this->Logic->SetAndObservePerkStationModuleNode( node );
    this->SecondaryMonitor->SetPSNode( node );
    this->GetLogic()->GetMRMLScene()->AddNode( this->MRMLNode->GetPlanMRMLFiducialListNode() );
    this->UpdateGUI();
    
    return;
    }
  
  
    // Planning volume is changed.
  
  if (    vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
          == this->VolumeSelector
       && (    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
            || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    this->PlanningVolumeChanged();
    }
  
  
    // Validation volume changed.
  
  if (    vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
          == this->ValidationVolumeSelector
       && (    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
            || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    this->ValidationVolumeChanged();
    }
  
  
    // Timer buttons.
  
  if ( vtkKWPushButton::SafeDownCast( caller ) == this->TimerButton )
    {
    this->TimerOn = ! this->TimerOn;
    
    if ( this->TimerOn )
      {
      this->TimerLog->StartTimer();
      this->LastTime = 0.0;
      
      vtkKWTkUtilities::CreateTimerHandler(
        this->GetApplication(), 1000, this, "TimerHandler" );
      }
    }
  
  if ( vtkKWPushButton::SafeDownCast( caller ) == this->ResetTimerButton )
    {
    int step = this->GetMRMLNode()->GetCurrentStep();
    this->WorkingTimes[ step ] = 0.0;
    this->LastTime = this->TimerLog->GetElapsedTime();
    this->UpdateTimerDisplay();
    }
  
  
    // Workphase pushbutton set.
  
  if ( event == vtkKWPushButton::InvokedEvent
       && this->WorkphaseButtonSet != NULL )
    {
    for ( int i = 0; i < this->WorkphaseButtonSet->GetNumberOfWidgets(); ++ i )
      {
      if ( vtkKWPushButton::SafeDownCast( caller )
           == this->WorkphaseButtonSet->GetWidget( i ) )
        {
        this->ChangeWorkphase( i );
        this->UpdateWorkphaseButtons();
        }
      }
    }
  
  
    // Press mouse press events on images to the different workflow steps.
    
  if ( strcmp( eventName, "LeftButtonPressEvent" ) == 0 )
    {    
    this->PlanStep->ProcessImageClickEvents( caller, event, callData );
    this->ValidateStep->ProcessImageClickEvents( caller, event, callData );
    }
  
  
    // Pass the keyboard events to the calibration workflow step.
  
  if ( strcmp( eventName, "KeyPressEvent" ) == 0 )
    {
    this->CalibrateStep->ProcessKeyboardEvents( caller, event, callData );
    this->SecondaryMonitor->UpdateImageDisplay();
    return;
    }
  
    // Mouse move in the planning phase.
  
  if (    this->MRMLNode
       && strcmp( eventName, "MouseMoveEvent" ) == 0
       && this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()
          == this->PlanStep.GetPointer()
       && strcmp( this->MRMLNode->GetVolumeInUse(), "Planning" ) == 0 )
    {
    this->PlanStep->ProcessMouseMoveEvent( caller, event, callData );
    }
  
  
    // Update window/level display.
  
  if (    this->MRMLNode
       && strcmp( eventName, "MouseMoveEvent" ) == 0 )
    {
    vtkSlicerSliceGUI* sliceGUI =
      this->GetApplicationGUI()->GetMainSliceGUI( "Red" );
    
    vtkCornerAnnotation* anno = sliceGUI->GetSliceViewer()->GetRenderWidget()->
                                GetCornerAnnotation();
     
    vtkMRMLScalarVolumeNode* volume = this->MRMLNode->GetActiveVolumeNode();
    
    if (    volume
         && volume->GetScalarVolumeDisplayNode() )
      {
      double window = volume->GetScalarVolumeDisplayNode()->GetWindow();
      double level = volume->GetScalarVolumeDisplayNode()->GetLevel();
      
      std::string str( anno->GetText( 1 ) );
      
      std::string::size_type found = str.find( "W" );
      if ( found != std::string::npos )
        {
        str = str.substr( 0, found - 1 );
        }
      
      std::stringstream ss;
        ss << str << std::endl;
        ss << "W: " << std::fixed << setprecision( 1 ) << window << std::endl;
        ss << "L: " << std::fixed << setprecision( 1 ) << level;
      anno->SetText( 1, ss.str().c_str() );
      }
    }
  
  
    // Red slice is selected and slice offset is changed.
  
  if ( this->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic()
       ->GetSliceNode() == vtkMRMLSliceNode::SafeDownCast( caller )
       && event == vtkCommand::ModifiedEvent
       && ! vtkPerkStationModuleLogic::DoubleEqual(
                   this->SliceOffset,
                   this->GetApplicationGUI()->GetMainSliceGUI( "Red" )
                     ->GetLogic()->GetSliceOffset() ) )
   {
   this->SliceOffset = this->GetApplicationGUI()->GetMainSliceGUI( "Red" )
     ->GetLogic()->GetSliceOffset();
   this->MRMLNode->SetCurrentSliceOffset( this->SliceOffset );
   this->SecondaryMonitor->UpdateImageDataOnSliceOffset( this->SliceOffset );
   }
   
  
    // If the Wizard Widget changed state.
    
  if ( this->WizardWidget->GetWizardWorkflow()
       == vtkKWWizardWorkflow::SafeDownCast( caller )
       && event == vtkKWWizardWorkflow::CurrentStateChangedEvent )
    {
    // TO DO  -- What?
    
    if ( this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()
         == this->CalibrateStep.GetPointer() )
      {
      this->State = vtkPerkStationModuleGUI::Calibrate;
      this->MRMLNode->SwitchStep( 0 );
      }
    else if ( this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()
              == this->PlanStep.GetPointer() )
      {
      this->State = vtkPerkStationModuleGUI::Plan;
      this->MRMLNode->SwitchStep( 1 );
      }
    else if ( this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()
              == this->InsertStep.GetPointer() )
      {
      this->State = vtkPerkStationModuleGUI::Insert;
      this->MRMLNode->SwitchStep( 2 );
      }
    else if ( this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()
              == this->ValidateStep.GetPointer() )
      {
      this->State = vtkPerkStationModuleGUI::Validate;
      this->MRMLNode->SwitchStep( 3 );
      }
    
    this->WizardWidget->GetWizardWorkflow()->GetStepFromState(
      this->WizardWidget->GetWizardWorkflow()->GetPreviousState() )->
      HideUserInterface();
      
    this->UpdateWorkphaseButtons();
    }  
  
  
    // Load experiment.
  
  if ( this->LoadExperimentFileButton
       && this->LoadExperimentFileButton->GetLoadSaveDialog()
       == vtkKWLoadSaveDialog::SafeDownCast( caller )
       && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
      // load calib dialog button
    const char *fileName = this->LoadExperimentFileButton->
      GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
        // call the callback function
      this->LoadExperimentButtonCallback( fileName );
      }
    
    // reset the file browse button text
    this->LoadExperimentFileButton->SetText( "Load experiment" );
    }  
  
  
    // Save experiment.
  
  if ( this->SaveExperimentFileButton
            && this->SaveExperimentFileButton->GetLoadSaveDialog()
               == vtkKWLoadSaveDialog::SafeDownCast(caller)
            && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
      // save calib dialog button
    const char *fileName = this->SaveExperimentFileButton->GetLoadSaveDialog()->
                           GetFileName();
    if ( fileName ) 
      {
      std::string fullFileName = std::string(fileName) + ".xml";
        // get the file name and file path
      this->SaveExperimentFileButton->GetLoadSaveDialog()->
            SaveLastPathToRegistry( "OpenPath" );
        // call the callback function
      this->SaveExperimentButtonCallback( fullFileName.c_str() );
      }
    
      // reset the file browse button text
    this->SaveExperimentFileButton->SetText( "Save experiment" );
    }  
  
  
    // Grayscale window level change.
  
  if (    this->DisplayVolumeLevelValue
       && this->DisplayVolumeLevelValue
          == vtkKWScaleWithEntry::SafeDownCast( caller )
       && ( event == vtkKWScale::ScaleValueChangedEvent ) )
    {
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->
      SetAutoWindowLevel( 0 );
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->
      SetLevel( this->DisplayVolumeLevelValue->GetValue() );
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->
      Modified();
    this->SecondaryMonitor->UpdateImageDisplay();
    }
  
  
    // Grayscale window width change.
  
  if (    this->DisplayVolumeWindowValue
       && this->DisplayVolumeWindowValue
          == vtkKWScaleWithEntry::SafeDownCast( caller )
       && ( event == vtkKWScale::ScaleValueChangedEvent ) )
    {
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->
      SetAutoWindowLevel( 0 );
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->
      SetWindow( this->DisplayVolumeWindowValue->GetValue() );
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->
      Modified();
    this->SecondaryMonitor->UpdateImageDisplay();
    }
  
  
    // Corner annotation.
  
  if (    this->MRMLNode
       && this->MRMLNode->GetPatientPosition() != PPNA )
    {
    PatientPositionEnum pposition = this->MRMLNode->GetPatientPosition();
    std::string pptext = "PP: Unknown";
    switch ( pposition )
      {
      case HFP:  pptext = "PP: HFP"; break;
      case HFS:  pptext = "PP: HFS"; break;
      case HFDR: pptext = "PP: HFDR"; break;
      case HFDL: pptext = "PP: HFDL"; break;
      case FFDR: pptext = "PP: FFDR"; break;
      case FFDL: pptext = "PP: FFDL"; break;
      case FFP:  pptext = "PP: FFP"; break;
      case FFS:  pptext = "PP: FFS"; break;
      }
    
    vtkSlicerSliceGUI* sliceGUI =
      this->GetApplicationGUI()->GetMainSliceGUI( "Red" );
    
    vtkCornerAnnotation* anno = sliceGUI->GetSliceViewer()->GetRenderWidget()->
                                GetCornerAnnotation();
     
    std::string str( anno->GetText( 2 ) );
    
    std::string::size_type found = str.find( "P" );
    if ( found != std::string::npos )
      {
      str = str.substr( 0, found - 1 );
      }
    
    std::stringstream ss;
      ss << str;
      ss << std::endl;
      ss << pptext;
    anno->SetText( 2, ss.str().c_str() );
    }
}


/**
 * Updates parameter values in MRML node based on GUI widgets.
 */
void vtkPerkStationModuleGUI::UpdateMRML ()
{
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  
  if ( n == NULL )
    {
      // no parameter node selected yet, create new
    
    this->PSNodeSelector->SetSelectedNew( "vtkMRMLPerkStationModuleNode" );
    this->PSNodeSelector->ProcessNewNodeCommand( "vtkMRMLPerkStationModuleNode", "PS" );
    n = vtkMRMLPerkStationModuleNode::SafeDownCast( this->PSNodeSelector->GetSelected() );

    // set an observe new node in Logic
    
    this->Logic->SetAndObservePerkStationModuleNode(n);
    vtkSetAndObserveMRMLNodeMacro(this->MRMLNode, n);

    // add MRMLFiducialListNode to the scene
    
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode( this->MRMLNode->GetPlanMRMLFiducialListNode() );

    // add listener to the slice logic, so that any time user makes change
    // to slice viewer in laptop, the display needs to be updated on secondary monitor
    // e.g. user may move to a certain slice in a series of slices
  
    vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic();
    if ( sliceLogic )
      {  
      sliceLogic->GetSliceNode()->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
      }

    
    }

  // save node parameters for Undo
  this->GetLogic()->GetMRMLScene()->SaveStateForUndo( n );
   
  
  
  if (this->VolumeSelector->GetSelected() != NULL)
    {    
    
    // see in what state wizard gui is, act accordingly
    if ( this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() ==
         this->CalibrateStep.GetPointer() )
      {

      bool planningVolumePreExists = false;
      // inside calibrate step

      // what if the volume selected is actually validation
      // in that case just return
      if ( n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL )
        {
        if ( strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0 )
          {
          n->SetVolumeInUse( "Validation" );          
          return;
          }
        }

      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse( "Planning" );   
          return;
          }
        // this implies that a planning volume already existed but now there is new image/volume chosen as planning volume in calibrate step
        planningVolumePreExists = true;
        }
      
      
      // calibrate/planning volume set
      n->SetPlanningVolumeRef(this->VolumeSelector->GetSelected()->GetID());
      n->SetPlanningVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetPlanningVolumeRef())));
      n->SetVolumeInUse( "Planning" );
      

      vtkMRMLScalarVolumeDisplayNode *node = NULL;
      vtkSetAndObserveMRMLNodeMacro(node, n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode());

      const char *strName = this->VolumeSelector->GetSelected()->GetName();
      std::string strPlan = std::string(strName) + "-Plan";
      this->VolumeSelector->GetSelected()->SetName(strPlan.c_str());
      this->VolumeSelector->GetSelected()->SetDescription("Planning image/volume; created by PerkStation module");
      this->VolumeSelector->GetSelected()->Modified();

      this->VolumeSelector->UpdateMenu();

      // set up the image on secondary monitor    
      this->SecondaryMonitor->SetupImageData();
      
      // set the window/level controls values from the scalar volume display node
      this->DisplayVolumeLevelValue->SetValue(n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode()->GetLevel());
      this->DisplayVolumeWindowValue->SetValue(n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode()->GetWindow());
    
      if (!planningVolumePreExists)
        {
        // repopulate/enable/disable controls now that volume has been loaded
        this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
        }
      else
        {
        this->ResetAndStartNewExperiment();
        }

      }
    else if ( this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() ==
              this->ValidateStep.GetPointer() )
      {
      // what if the volume selected is actually planning
      // in that case just return
      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Planning");          
          return;
          }
        }

      // in case, the validation volume already exists
      if (n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        // in case, the selected volume is actually validation volume
        if (strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Validation");  
          return;
          }
        // this implies that a validation volume already existed but now there
        // is new image/volume chosen as validation volume in validate step
        }
           

      // validate volume set
      n->SetValidationVolumeRef(this->VolumeSelector->GetSelected()->GetID());    
      n->SetValidationVolumeNode( vtkMRMLScalarVolumeNode::SafeDownCast(
                                  this->GetMRMLScene()->GetNodeByID( n->GetValidationVolumeRef() ) ) );
      n->SetVolumeInUse("Validation");

      const char *strName = this->VolumeSelector->GetSelected()->GetName();
      std::string strPlan = std::string(strName) + "-Validation";
      this->VolumeSelector->GetSelected()->SetName(strPlan.c_str());
      this->VolumeSelector->GetSelected()->SetDescription("Validation image/volume; created by PerkStation module");
      this->VolumeSelector->GetSelected()->Modified();

      this->VolumeSelector->UpdateMenu();

       // set the window/level controls values from the scalar volume display node
      this->DisplayVolumeLevelValue->SetValue(n->GetValidationVolumeNode()->GetScalarVolumeDisplayNode()->GetLevel());
      this->DisplayVolumeWindowValue->SetValue(n->GetValidationVolumeNode()->GetScalarVolumeDisplayNode()->GetWindow());


      }
    else
      {
    
      // situation, when the user in neither calibration step, nor validation step
      // he could be fiddling around with GUI, and be in either planning or insertion step or evaluation step

      // what if the volume selected is actually validation
      // in that case just return
      if (n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Validation");          
          return;
          }
        }

      bool planningVolumePreExists = false;

      // what if the volume selected is actually planning
      // in that case just return
      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Planning");          
          return;
          }
        // this implies that a planning volume already existed but now there is new image/volume chosen as planning volume in calibrate step
        planningVolumePreExists = true;
        }
      
      // calibrate/planning volume set
      n->SetPlanningVolumeRef(this->VolumeSelector->GetSelected()->GetID());
      n->SetPlanningVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetPlanningVolumeRef())));
      n->SetVolumeInUse("Planning");
     
      vtkMRMLScalarVolumeDisplayNode *node = NULL;
      vtkSetAndObserveMRMLNodeMacro(node, n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode());

       // set up the image on secondary monitor    
      this->SecondaryMonitor->SetupImageData();

      if (!planningVolumePreExists)
        {
        // bring the wizard GUI back to Calibrate step
        // the volume selection has changed/added, so make sure that the wizard is in the intial calibration state!
        while (this->WizardWidget->GetWizardWorkflow()->GetCurrentState()!= this->WizardWidget->GetWizardWorkflow()->GetInitialState())
          {
          this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
          }
        this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
        }
      else
        {
        this->ResetAndStartNewExperiment();
        }
      }
    }
}


void
vtkPerkStationModuleGUI
::TimerHandler()
{
  if ( ! this->TimerOn )
    {
    return;
    }
  
  vtkKWTkUtilities::CreateTimerHandler(
    this->GetApplication(), 1000, this, "TimerHandler" );
  
  std::stringstream ss;
  
  int step = this->GetMRMLNode()->GetCurrentStep();
  this->TimerLog->StopTimer();
  double elapsed = this->TimerLog->GetElapsedTime();
  
  if ( step < 0 || step > 3 ) return;
  
  this->WorkingTimes[ step ] += ( elapsed - this->LastTime );
  this->LastTime = elapsed;
  
  this->UpdateTimerDisplay();
}


void
vtkPerkStationModuleGUI
::PlanningVolumeChanged()
{
  vtkMRMLScalarVolumeNode* volumeNode =
      vtkMRMLScalarVolumeNode::SafeDownCast( this->VolumeSelector->GetSelected() );
  
  if ( ! volumeNode ) return;
  
    // Didn't actually change selection.
  if ( volumeNode == this->MRMLNode->GetPlanningVolumeNode() ) return;
  
  
  vtkMRMLScalarVolumeDisplayNode *node = NULL;
  vtkSetAndObserveMRMLNodeMacro( node, volumeNode->GetScalarVolumeDisplayNode() );
  
  
  this->MRMLNode->SetVolumeInUse( "Planning" );
  this->MRMLNode->SetPlanningVolumeNode( volumeNode );
  this->SecondaryMonitor->SetupImageData();
  
  
    // Bring wizard to Calibration phase.
  
  while( this->WizardWidget->GetWizardWorkflow()->GetCurrentState()
         != this->WizardWidget->GetWizardWorkflow()->GetInitialState() )
    {
    this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
    }
  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->
    ShowUserInterface();
  
  this->ResetAndStartNewExperiment();
  
  
    // set the window/level controls values from scalar volume display node
  this->DisplayVolumeLevelValue->SetValue(
    volumeNode->GetScalarVolumeDisplayNode()->GetLevel() );
  this->DisplayVolumeWindowValue->SetValue(
    volumeNode->GetScalarVolumeDisplayNode()->GetWindow() );
  
  
    // Update the application logic.
  this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID(
    volumeNode->GetID() );
  this->GetApplicationLogic()->PropagateVolumeSelection();
  
  
    // Set the Slice view to the correct position.
  this->Logic->AdjustSliceView();
  
  
  this->CalibrateStep->UpdateGUI();
  
  this->SecondaryMonitor->UpdateImageDisplay();
  
  /*
    // Uncomment this only if volume is checked for existing 0 offset slice.
    // Make the zero (RAS) slice the starting slice.
  this->MRMLNode->SetCurrentSliceOffset( 0.0 );
  this->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic()
    ->SetSliceOffset( this->MRMLNode->GetCurrentSliceOffset() );
  */
}


void
vtkPerkStationModuleGUI
::ValidationVolumeChanged()
{
  vtkMRMLScalarVolumeNode* volumeNode =
      vtkMRMLScalarVolumeNode::SafeDownCast(
                    this->ValidationVolumeSelector->GetSelected() );
  
  if ( ! volumeNode ) return;
  
  this->MRMLNode->SetValidationVolumeNode( volumeNode );
  
  this->DisplayVolumeLevelValue->SetValue(
      volumeNode->GetScalarVolumeDisplayNode()->GetLevel() );
  this->DisplayVolumeWindowValue->SetValue(
      volumeNode->GetScalarVolumeDisplayNode()->GetWindow() );
  
  this->Logic->AdjustSliceView();
}


// ----------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::UpdateGUI()
{
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  if ( n == NULL )
    {
    return;
    }
  
  
    // Update node selector widget.
  
  if ( this->PSNodeSelector != NULL )
    {
    vtkMRMLPerkStationModuleNode* moduleNode = vtkMRMLPerkStationModuleNode::SafeDownCast(
      this->PSNodeSelector->GetSelected() );
    if ( moduleNode != this->GetMRMLNode() )
      {
      this->PSNodeSelector->SetSelected( this->GetMRMLNode() );
      this->SecondaryMonitor->SetPSNode( this->GetMRMLNode() );
    
      }
    }
  
  
  this->WorkingTimes[ 0 ] = n->GetTimeOnCalibrateStep();
  this->WorkingTimes[ 1 ] = n->GetTimeOnPlanStep();
  this->WorkingTimes[ 2 ] = n->GetTimeOnInsertStep();
  this->WorkingTimes[ 3 ] = n->GetTimeOnValidateStep();
  this->UpdateTimerDisplay();
  
  this->CalibrateStep->UpdateGUI();
}


// ----------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::ProcessMRMLEvents ( vtkObject *caller,
                      unsigned long event,
                      void *callData ) 
{
    // if parameter node has been changed externally,
    // update GUI widgets with new values
  
  vtkMRMLPerkStationModuleNode*   node =
                       vtkMRMLPerkStationModuleNode::SafeDownCast( caller );
  vtkMRMLLinearTransformNode*     transformNode =
                       vtkMRMLLinearTransformNode::SafeDownCast( caller );
  vtkMRMLScalarVolumeDisplayNode* displayNode =
                       vtkMRMLScalarVolumeDisplayNode::SafeDownCast( caller );
  
  
  if (    node != NULL
       && this->GetMRMLNode() == node ) 
  {
    this->UpdateGUI();
  }
  
  
  if (    displayNode != NULL
       && this->GetMRMLNode()->GetPlanningVolumeNode()
       && this->GetMRMLNode()->GetPlanningVolumeNode()->
                GetScalarVolumeDisplayNode()
          == displayNode
       && event == vtkCommand::ModifiedEvent )
    {
    this->GetSecondaryMonitor()->UpdateImageDisplay();
    }
}


vtkPerkStationSecondaryMonitor*
vtkPerkStationModuleGUI
::GetSecondaryMonitor()
{
  return this->SecondaryMonitor.GetPointer();
}
  

vtkKWWizardWidget*
vtkPerkStationModuleGUI
::GetWizardWidget()
{
  return this->WizardWidget;
}


//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::BuildGUI() 
{
  if ( this->Built) return;
  
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  
    // Register MRML PS node, not create it.
  
  this->Logic->GetMRMLScene()->RegisterNodeClass(
    vtkSmartPointer< vtkMRMLPerkStationModuleNode >::New() );
  
  this->UIPanel->AddPage ( "PerkStationModule", "PerkStationModule", NULL );
  
  
  // add individual collapsible pages/frames
  
  this->BuildGUIForHelpFrame();
  this->BuildGUIForExperimentFrame();
  this->BuildGUIForWorkphases();
  
  this->Built = true;
}


/**
 * Creates widgets for the frame, and places them by TCL scripts.
 */
void
vtkPerkStationModuleGUI
::BuildGUIForHelpFrame()
{
  vtkSlicerApplication* app = (vtkSlicerApplication*)( this->GetApplication() );
  vtkKWWidget* page = this->UIPanel->GetPageWidget( "PerkStationModule" );
  
  std::stringstream helpss;
  helpss << "**PERK Station Module** " << std::endl
         << "**Revision " << PerkStationModule_REVISION << "**" << std::endl
         << "Use this module to perform image overlay guided percutaneous "
         << "interventions.";
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > PerkStationHelpFrame =
      vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    PerkStationHelpFrame->SetParent ( page );
    PerkStationHelpFrame->Create ( );
    PerkStationHelpFrame->CollapseFrame ( );
    PerkStationHelpFrame->SetLabelText ( "Help" );
  
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                PerkStationHelpFrame->GetWidgetName(), page->GetWidgetName() );
  
  this->HelpText->SetParent( PerkStationHelpFrame->GetFrame() );
  this->HelpText->Create();
  this->HelpText->SetHorizontalScrollbarVisibility( 0 );
  this->HelpText->SetVerticalScrollbarVisibility( 1 );
  this->HelpText->GetWidget()->SetText( helpss.str().c_str() );
  this->HelpText->GetWidget()->SetReliefToFlat();
  this->HelpText->GetWidget()->SetWrapToWord();
  this->HelpText->GetWidget()->ReadOnlyOn();
  this->HelpText->GetWidget()->QuickFormattingOn();
  this->HelpText->GetWidget()->SetBalloonHelpString( "" );
  
  app->Script( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4",
               this->HelpText->GetWidgetName() );
  
  
  const char *about = "This work was supported by NA-MIC, NAC, BIRN, NCIGT,"
                      "and the Slicer Community."
                      "See <a>http://www.slicer.org</a> for details. ";
}


/**
 * Creates widgets for the Experiment frame, and places them by TCL scripts.
 */
void
vtkPerkStationModuleGUI
::BuildGUIForExperimentFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication*)( this->GetApplication() );
  vtkKWWidget* page = this->UIPanel->GetPageWidget( "PerkStationModule" );
  
  
  vtkSmartPointer< vtkKWFrameWithLabel > loadSaveExptFrame =
      vtkSmartPointer< vtkKWFrameWithLabel >::New();
    loadSaveExptFrame->SetParent( page );
    loadSaveExptFrame->Create();
    loadSaveExptFrame->SetLabelText( "Experiment" );
  
  app->Script( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
               loadSaveExptFrame->GetWidgetName(), page->GetWidgetName() );
  
  
    // Volume selection frame
  
  vtkSmartPointer< vtkKWFrame > volSelFrame =
      vtkSmartPointer< vtkKWFrame >::New();
    volSelFrame->SetParent( loadSaveExptFrame->GetFrame() );
    volSelFrame->Create();
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                volSelFrame->GetWidgetName() );
  
    // Node selectors.
  
  this->PSNodeSelector->SetNodeClass( "vtkMRMLPerkStationModuleNode", NULL,
                                      NULL, "Experiment data" );
  this->PSNodeSelector->SetParent( volSelFrame );
  this->PSNodeSelector->Create();
  this->PSNodeSelector->SetNewNodeEnabled( 1 );
  this->PSNodeSelector->NoneEnabledOff();
  this->PSNodeSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
  this->PSNodeSelector->UpdateMenu();
  this->PSNodeSelector->SetLabelText( "Module parameters");
  app->Script( "pack %s -side top -anchor w -fill x -padx 2 -pady 4", this->PSNodeSelector->GetWidgetName() );
  
  this->VolumeSelector->SetNodeClass( "vtkMRMLScalarVolumeNode", NULL, NULL, NULL );
  this->VolumeSelector->SetParent( volSelFrame );
  this->VolumeSelector->Create();
  this->VolumeSelector->NoneEnabledOn();
  this->VolumeSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
  this->VolumeSelector->UpdateMenu();
  this->VolumeSelector->SetLabelText( "Planning Volume:");
  this->VolumeSelector->SetBalloonHelpString( "Select image for needle insertion planning." );
  
  app->Script( "pack %s -side top -anchor w -fill x -padx 2 -pady 4", 
               this->VolumeSelector->GetWidgetName() );
  
  this->ValidationVolumeSelector->SetNodeClass( "vtkMRMLScalarVolumeNode", NULL, NULL, NULL );
  this->ValidationVolumeSelector->SetParent( volSelFrame );
  this->ValidationVolumeSelector->Create();
  this->ValidationVolumeSelector->NoneEnabledOn();
  this->ValidationVolumeSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
  this->ValidationVolumeSelector->UpdateMenu();
  this->ValidationVolumeSelector->SetLabelText( "Validation volume:" );
  this->ValidationVolumeSelector->SetBalloonHelpString(
              "Select image for validation of needle position." );
  
  app->Script( "pack %s -side top -anchor w -fill x -padx 2 -pady 4",
               this->ValidationVolumeSelector->GetWidgetName() );
  
  
    // Window level frame.
  
  vtkSmartPointer< vtkKWFrameWithLabel > windowLevelFrame =
      vtkSmartPointer< vtkKWFrameWithLabel >::New();
    windowLevelFrame->SetParent( loadSaveExptFrame->GetFrame() );
    windowLevelFrame->SetLabelText( "Window/Level" );
    windowLevelFrame->Create();
  
  /*
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                windowLevelFrame->GetWidgetName() );
  */
  
  if ( ! this->DisplayVolumeWindowValue->IsCreated() )
    {
    this->DisplayVolumeWindowValue->SetParent( windowLevelFrame->GetFrame() );
    this->DisplayVolumeWindowValue->Create();
    this->DisplayVolumeWindowValue->SetRange(0.0, 8192.0);
    this->DisplayVolumeWindowValue->SetResolution(10.0);
    this->DisplayVolumeWindowValue->SetLength(150);
    this->DisplayVolumeWindowValue->SetLabelText("Window"); 
    this->DisplayVolumeWindowValue->SetLabelPositionToTop();
    this->DisplayVolumeWindowValue->SetEntryPositionToTop();
    }
  
  if ( ! this->DisplayVolumeLevelValue->IsCreated() )
    {
    this->DisplayVolumeLevelValue->SetParent( windowLevelFrame->GetFrame() );
    this->DisplayVolumeLevelValue->Create();
    this->DisplayVolumeLevelValue->SetRange(0.0, 2048.0);
    this->DisplayVolumeLevelValue->SetResolution(10.0);
    this->DisplayVolumeLevelValue->SetLength(150);
    this->DisplayVolumeLevelValue->SetLabelText("Level");   
    this->DisplayVolumeLevelValue->SetLabelPositionToTop();
    this->DisplayVolumeLevelValue->SetEntryPositionToTop();
    }
  /*
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->DisplayVolumeWindowValue->GetWidgetName() );
  
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 2", 
                        this->DisplayVolumeLevelValue->GetWidgetName());
  */
  
    // Load save frame.
  
  vtkSmartPointer< vtkKWFrame > loadSaveFrame =
      vtkSmartPointer< vtkKWFrame >::New();
    loadSaveFrame->SetParent( loadSaveExptFrame->GetFrame() );
    loadSaveFrame->Create();
  /*
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                loadSaveFrame->GetWidgetName() );
  */
  
    // create the load file dialog button
  
  if ( ! this->LoadExperimentFileButton->IsCreated() )
    {
    this->LoadExperimentFileButton->SetParent( loadSaveFrame );
    this->LoadExperimentFileButton->Create();
    this->LoadExperimentFileButton->SetBorderWidth(2);
    this->LoadExperimentFileButton->SetReliefToRaised();       
    this->LoadExperimentFileButton->SetHighlightThickness(2);
    this->LoadExperimentFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadExperimentFileButton->SetActiveBackgroundColor(1,1,1);
    this->LoadExperimentFileButton->SetText("Load experiment");
    this->LoadExperimentFileButton->SetImageToPredefinedIcon(
      vtkKWIcon::IconPresetLoad );
    this->LoadExperimentFileButton->SetBalloonHelpString(
      "Click to load a previous experiment file" );
    this->LoadExperimentFileButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "OpenPath" );
    this->LoadExperimentFileButton->TrimPathFromFileNameOff();
    this->LoadExperimentFileButton->SetMaximumFileNameLength(256);
    this->LoadExperimentFileButton->GetLoadSaveDialog()->SaveDialogOff();
    this->LoadExperimentFileButton->GetLoadSaveDialog()->SetFileTypes(
      "{{XML File} {.xml}} {{All Files} {*.*}}" );
    }
    
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->LoadExperimentFileButton->GetWidgetName() );

      // create the load file dialog button
    
  if ( ! this->SaveExperimentFileButton->IsCreated() )
    {
    this->SaveExperimentFileButton->SetParent( loadSaveFrame );
    this->SaveExperimentFileButton->Create();
    this->SaveExperimentFileButton->SetText( "Save experiment" );
    this->SaveExperimentFileButton->SetBorderWidth(2);
    this->SaveExperimentFileButton->SetReliefToRaised();       
    this->SaveExperimentFileButton->SetHighlightThickness( 2 );
    this->SaveExperimentFileButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    this->SaveExperimentFileButton->SetActiveBackgroundColor( 1, 1, 1 );               
    this->SaveExperimentFileButton->SetImageToPredefinedIcon(
      vtkKWIcon::IconFloppy );
    this->SaveExperimentFileButton->SetBalloonHelpString(
      "Click to save experiment in a file" );
    this->SaveExperimentFileButton->GetLoadSaveDialog()->SaveDialogOn();
    this->SaveExperimentFileButton->TrimPathFromFileNameOff();
    this->SaveExperimentFileButton->SetMaximumFileNameLength( 256 );
    this->SaveExperimentFileButton->GetLoadSaveDialog()->SetFileTypes(
      "{{XML File} {.xml}} {{All Files} {*.*}}" ); 
    this->SaveExperimentFileButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "OpenPath" );
    }
  
  this->Script( "pack %s -side top -anchor ne -padx 2 -pady 2", 
                this->SaveExperimentFileButton->GetWidgetName() );
  
  
  vtkSmartPointer< vtkKWFrame > timerFrame =
      vtkSmartPointer< vtkKWFrame > ::New();
    timerFrame->SetParent( page );
    timerFrame->Create();
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                timerFrame->GetWidgetName() );
  
  if ( ! this->CalibrateTimeLabel->IsCreated() )
    {
    this->CalibrateTimeLabel->SetParent( timerFrame );
    this->CalibrateTimeLabel->Create();
    this->CalibrateTimeLabel->SetWidth( 8 );
    this->CalibrateTimeLabel->SetText( "00:00" );
    
    this->PlanTimeLabel->SetParent( timerFrame );
    this->PlanTimeLabel->Create();
    this->PlanTimeLabel->SetWidth( 8 );
    this->PlanTimeLabel->SetText( "00:00" );
    
    this->InsertTimeLabel->SetParent( timerFrame );
    this->InsertTimeLabel->Create();
    this->InsertTimeLabel->SetWidth( 8 );
    this->InsertTimeLabel->SetText( "00:00" );
    
    this->ValidateTimeLabel->SetParent( timerFrame );
    this->ValidateTimeLabel->Create();
    this->ValidateTimeLabel->SetWidth( 8 );
    this->ValidateTimeLabel->SetText( "00:00" );
    
    this->TimerButton->SetParent( timerFrame );
    this->TimerButton->Create();
    this->TimerButton->SetWidth( 8 );
    this->TimerButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    this->TimerButton->SetText( "Timer" );
    
    this->ResetTimerButton->SetParent( timerFrame );
    this->ResetTimerButton->Create();
    this->ResetTimerButton->SetWidth( 12 );
    this->ResetTimerButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    this->ResetTimerButton->SetText( "Reset Timer" );
    }
  
  this->Script( "grid %s -column 0 -row 0 -sticky w -padx 2 -pady 2",
    this->CalibrateTimeLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 0 -sticky w -padx 2 -pady 2",
    this->PlanTimeLabel->GetWidgetName() );
  this->Script( "grid %s -column 2 -row 0 -sticky w -padx 2 -pady 2",
    this->InsertTimeLabel->GetWidgetName() );
  this->Script( "grid %s -column 3 -row 0 -sticky w -padx 2 -pady 2",
    this->ValidateTimeLabel->GetWidgetName() );
  this->Script( "grid %s -column 4 -row 0 -sticky e -padx 2 -pady 2",
    this->TimerButton->GetWidgetName() );
  this->Script( "grid %s -column 5 -row 0 -sticky e -padx 2 -pady 2",
    this->ResetTimerButton->GetWidgetName() );
  
  this->Script( "grid columnconfigure %s 0 -weight 1",
                timerFrame->GetWidgetName() );
  this->Script( "grid columnconfigure %s 1 -weight 1",
                timerFrame->GetWidgetName() );
  this->Script( "grid columnconfigure %s 2 -weight 1",
                timerFrame->GetWidgetName() );
  this->Script( "grid columnconfigure %s 3 -weight 1",
                timerFrame->GetWidgetName() );
  this->Script( "grid columnconfigure %s 4 -weight 10",
                timerFrame->GetWidgetName() );
  this->Script( "grid columnconfigure %s 5 -weight 10",
                timerFrame->GetWidgetName() );
  
}


void
vtkPerkStationModuleGUI
::BuildGUIForWorkphases()
{
  vtkSlicerApplication *app = (vtkSlicerApplication*)( this->GetApplication() );
  vtkKWWidget* page = this->UIPanel->GetPageWidget( "PerkStationModule" );
  
    // Work phase collapsible frame with push buttons.
  
  this->WorkphaseButtonFrame->SetParent( page );
  this->WorkphaseButtonFrame->Create();
  
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 4 -pady 2",
                this->WorkphaseButtonFrame->GetWidgetName());
  
    // Workphase button set.
  
  this->WorkphaseButtonSet = vtkKWPushButtonSet::New();
    this->WorkphaseButtonSet->SetParent( this->WorkphaseButtonFrame );
    this->WorkphaseButtonSet->Create();
    this->WorkphaseButtonSet->PackHorizontallyOn();
    this->WorkphaseButtonSet->SetMaximumNumberOfWidgetsInPackingDirection( 5 );
    this->WorkphaseButtonSet->SetWidgetsPadX( 1 );
    this->WorkphaseButtonSet->SetWidgetsPadY( 1 );
    this->WorkphaseButtonSet->UniformColumnsOn();
    this->WorkphaseButtonSet->UniformRowsOn();  
    this->Script( "pack %s -side left -anchor w -fill x -padx 1 -pady 1", 
                  this->WorkphaseButtonSet->GetWidgetName() );    

    // Captions for radiobuttons.
  
  const char *buffer[] =
    {
    "Calibrate", "Plan", "Insert", "Validate"
    };
  
    // Individual radiobuttons.
  int numSteps = 4;
  for (int id = 0; id < numSteps; id ++ )
    {
    vtkKWPushButton *pb = this->WorkphaseButtonSet->AddWidget( id );
      pb->SetText( buffer[ id ] );
      pb->SetBorderWidth( 2 );
      pb->SetReliefToGroove();
      pb->SetHighlightThickness( 2 );
      pb->SetBackgroundColor( 0.85, 0.85, 0.85 );
      pb->SetActiveBackgroundColor( 1, 1, 1 );
    }
  
    // Initial state.
  
  this->WorkphaseButtonSet->GetWidget( 0 )->SetReliefToSunken();
  
    // Wizard collapsible frame with individual steps inside
  
  this->WizardFrame = vtkSmartPointer< vtkSlicerModuleCollapsibleFrame >::New();
    this->WizardFrame->SetParent( page );
    this->WizardFrame->Create();
    this->WizardFrame->SetLabelText( "Wizard" );
    this->WizardFrame->ExpandFrame();

  app->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               this->WizardFrame->GetWidgetName(), 
               page->GetWidgetName() );

    // individual page/collapsible frame with their own widgets inside:
  
  this->WizardWidget->SetParent( this->WizardFrame->GetFrame() );
  this->WizardWidget->Create();  
  this->WizardWidget->GetSubTitleLabel()->SetHeight( 1 );
  this->WizardWidget->SetClientAreaMinimumHeight( 320 );
  this->WizardWidget->HelpButtonVisibilityOn();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -fill both -expand y",
    this->WizardWidget->GetWidgetName() );
  
  
  vtkKWWizardWorkflow *wizard_workflow =
    this->WizardWidget->GetWizardWorkflow();
  
  this->CalibrateStep = vtkSmartPointer< vtkPerkStationCalibrateStep >::New();
  this->CalibrateStep->SetGUI( this );
  
  wizard_workflow->AddStep( this->CalibrateStep );
  

  this->PlanStep = vtkSmartPointer< vtkPerkStationPlanStep >::New();
  this->PlanStep->SetGUI( this );
  
  wizard_workflow->AddNextStep( this->PlanStep );
  
  
  this->InsertStep = vtkSmartPointer< vtkPerkStationInsertStep >::New();
  this->InsertStep->SetGUI(this);
  
  wizard_workflow->AddNextStep(this->InsertStep);
  
  
  this->ValidateStep = vtkSmartPointer< vtkPerkStationValidateStep >::New();
  this->ValidateStep->SetGUI(this);
  
  wizard_workflow->AddNextStep( this->ValidateStep );
  
  
  wizard_workflow->SetFinishStep( this->ValidateStep );
  
  // TODO: How to set up the transitions correctly?
  
  wizard_workflow->SetInitialStep( this->CalibrateStep );    
  
  this->SetUpPerkStationWizardWorkflow();
}


//---------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::TearDownGUI() 
{
  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->SetParent(NULL);
    if(this->LoadExperimentFileButton->IsCreated())
      {
      // same as the above reason
      this->LoadExperimentFileButton->GetLoadSaveDialog()->SetParent(NULL);
      }
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->SetParent(NULL);
    if(this->SaveExperimentFileButton->IsCreated())
      {
      // same as the above reason
      this->SaveExperimentFileButton->GetLoadSaveDialog()->SetParent(NULL);
      }    
    }
  if (this->WizardFrame)
    {    
    this->WizardFrame->SetParent(NULL);    
    }  
  if (this->WizardWidget)
    {
    this->WizardWidget->SetParent(NULL);
    }
  if (this->CalibrateStep)
    {
    this->CalibrateStep->SetGUI(NULL);
    }

  if (this->PlanStep)
    {
    this->PlanStep->SetGUI(NULL);
    }

  if (this->InsertStep)
    {
    this->InsertStep->SetGUI(NULL);
    }

  if (this->ValidateStep)
    {
    this->ValidateStep->SetGUI(NULL);
    }

  if (this->VolumeSelector)
    {
    if(this->VolumeSelector->IsCreated())
      {
      // vtkKWMenuButtonWithSpinButtonsWithLabel::CreateWidget() set itself as a parent of its Widget by Widget->SetParent(this).       
      // But it does not do Widget->SetParent(NULL) in its destructor, so we do it here.
      this->VolumeSelector->GetWidget()->SetParent(NULL);
      // vtkKWWidgetWithLabel::CreateLabel() set itself as a parent of its Widget by label->SetParent(this).       
      // But it does not do label->SetParent(NULL) in its destructor, so we do it here.
      this->VolumeSelector->GetLabel()->SetParent(NULL);
      }
    this->VolumeSelector->SetParent(NULL);
    }
  if (this->PSNodeSelector)
    {
    if(this->PSNodeSelector->IsCreated())
      {
      // same as the above reason
      this->PSNodeSelector->GetWidget()->SetParent(NULL);
      this->PSNodeSelector->GetLabel()->SetParent(NULL);
      }
    this->PSNodeSelector->SetParent(NULL);
    }
  if (this->DisplayVolumeLevelValue)
    {
    this->DisplayVolumeLevelValue->SetParent(NULL);
    }
  if (this->DisplayVolumeWindowValue)
    {
    this->DisplayVolumeWindowValue->SetParent(NULL);
    }
}


//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SetUpPerkStationMode()
{
  // note this can only be done during or before calibrate step
  // the following tasks need to be done
  // Wizard workflow wizard needs to be re-done i.e. steps/transitions as well
  this->SetUpPerkStationWizardWorkflow();
}


//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SetUpPerkStationWizardWorkflow()
{  
  // vtkNotUsed( vtkKWWizardWidget *wizard_widget = this->WizardWidget; );
  vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();
  
    // create transition/program validate button such that it doesn't go further
  wizard_workflow->SetFinishStep( this->ValidateStep );
  
  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();  
}


//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::RenderSecondaryMonitor()
{
  if (this->SecondaryMonitor->IsSecondaryMonitorActive())
    {
    vtkRenderer *ren1 = vtkRenderer::New();
    vtkWin32OpenGLRenderWindow *renWin = vtkWin32OpenGLRenderWindow::New();
    renWin->AddRenderer(ren1);
    renWin->SetBorders(0);
    
    // positioning window
    int virtualScreenPos[2];
    this->SecondaryMonitor->GetVirtualScreenCoord(virtualScreenPos[0], virtualScreenPos[1]);
    renWin->SetPosition(virtualScreenPos[0], virtualScreenPos[1]);

    // window size
    unsigned int winSize[2];
    this->SecondaryMonitor->GetScreenDimensions(winSize[0], winSize[1]);
    renWin->SetSize(winSize[0], winSize[1]);
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    vtkImageMapper *imageMapper = vtkImageMapper::New();
    imageMapper->SetColorWindow(255);
    imageMapper->SetColorLevel(127.5);
    
    vtkDICOMImageReader *reader = vtkDICOMImageReader::New();
    reader->SetFileName("C:\\Work\\SliceVolReg\\Data\\Pt1\\SR301\\I0022_1");
    reader->Update();
    
    float *direction = reader->GetImageOrientationPatient();

    int extent[6];
    double spacing[3];
    double origin[3];
    reader->GetOutput()->GetWholeExtent(extent);
    reader->GetOutput()->GetSpacing(spacing);
    reader->GetOutput()->GetOrigin(origin);

    /*
    vtkImageData *image = this->GetApplicationGUI()->GetMainSliceGUI0()->GetLogic()->GetImageData();
    imageMapper->SetInput(image);
    */

    vtkImageData *image = reader->GetOutput();
    

    
    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]); 
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]); 
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]); 

    vtkImageReslice *reslice = vtkImageReslice::New();
    reslice->SetInput(image);
    reslice->SetOutputDimensionality(2);
    reslice->SetOutputExtent(0, winSize[0]-1, 0, winSize[1]-1,0,0);
    //reslice->SetOutputSpacing(spacing);
    //reslice->SetOutputOrigin(center[0], center[1],0);

    //reslice->SetResliceTransform(transform);
    reslice->SetInterpolationModeToLinear();

    reslice->Update();

    
    imageMapper->SetInput(reslice->GetOutput());

    
    vtkActor2D *imageActor = vtkActor2D::New();
    imageActor->SetMapper(imageMapper);
    //imageActor->SetPosition(winSize[0]/2, winSize[1]/2, 0);

    ren1->AddActor(imageActor);
    renWin->Render();

    //iren->Start();
    }
  else
    {
    vtkErrorMacro( "No secondary monitor detected" );
    }
}


//---------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::ResetAndStartNewExperiment()
{
    // Reset individual work phase steps to bring to fresh state, who are in 
    // turn, responsible for reseting MRML node parameters
  
  this->CalibrateStep->Reset();
  this->PlanStep->Reset();
  this->InsertStep->Reset();
  this->ValidateStep->Reset();
}


//---------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::SaveExperimentButtonCallback( const char* fileName )
{
  
}


//-----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::LoadExperimentButtonCallback(const char *fileName)
{
  
}


//-----------------------------------------------------------------------------
char*
vtkPerkStationModuleGUI
::CreateFileName()
{
  // create a folder for current date, if not already created
  // get the system calendar time
  std::time_t tt = std::time(0);
  // convert it into tm struct
  std::tm ttm = *std::localtime(&tt);
  // extract the values for day, month, year
  char dirName[9] = "";
  sprintf(dirName, "%4d%2d%2d", ttm.tm_year, ttm.tm_mon+1, ttm.tm_mday);
  if (access(dirName,0) ==0)
      {
      struct stat status;
      stat( dirName, &status );
      if ( status.st_mode & S_IFDIR )
      {
      // directory exists
      }
      else
      {
      //create directory
      }
      }
  else
      {
      //create directory
          ::CreateDirectory(dirName,NULL);
      }
  
  return dirName;
}


void
vtkPerkStationModuleGUI
::UpdateTimerDisplay()
{
    // TODO: This is double booking of times. Should be simplified.
  this->GetMRMLNode()->SetTimeOnCalibrateStep( this->WorkingTimes[ 0 ] );
  this->GetMRMLNode()->SetTimeOnPlanStep( this->WorkingTimes[ 1 ] );
  this->GetMRMLNode()->SetTimeOnInsertStep( this->WorkingTimes[ 2 ] );
  this->GetMRMLNode()->SetTimeOnValidateStep( this->WorkingTimes[ 3 ] );
  

  std::stringstream ss;
  
  ss.str( "" );
  ss << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 0 ] ) / 60
     << ":"
     << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 0 ] ) % 60;
  
  this->CalibrateTimeLabel->SetText( ss.str().c_str() );
  
  ss.str( "" );
  ss << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 1 ] ) / 60
     << ":"
     << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 1 ] ) % 60;
  
  this->PlanTimeLabel->SetText( ss.str().c_str() );
  
  ss.str( "" );
  ss << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 2 ] ) / 60
     << ":"
     << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 2 ] ) % 60;
  
  this->InsertTimeLabel->SetText( ss.str().c_str() );
  
  ss.str( "" );
  ss << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 3 ] ) / 60
     << ":"
     << setfill( '0' ) << setw( 2 ) << (int)( this->WorkingTimes[ 3 ] ) % 60;
  
  this->ValidateTimeLabel->SetText( ss.str().c_str() );
}


/**
 * Call this to change workphase.
 *
 * @param phase New phase.
 * @returns 1 on success, 0 on error.
 */
int
vtkPerkStationModuleGUI
::ChangeWorkphase( int phase )
{
  if ( ! this->MRMLNode->SwitchStep( phase ) ) // Set next phase
    {
    cerr << "ChangeWorkphase: Cannot transition!" << endl;
    return 0;
    }
  
  int numSteps = this->MRMLNode->GetNumberOfSteps();
  
  
    // Switch wizard frame.
  
  vtkKWWizardWorkflow *wizard = this->WizardWidget->GetWizardWorkflow();
  
  int step_from;
  int step_to;
  
  step_to = this->MRMLNode->GetCurrentStep();
  step_from = this->MRMLNode->GetPreviousStep();
  
    // Walk from old step to new step.
  int steps =  step_to - step_from;
  if ( steps > 0 )
    {
    for (int i = 0; i < steps; i ++) 
      {
        // Hide old step UI.
      vtkPerkStationStep* oldStep =
        vtkPerkStationStep::SafeDownCast( wizard->GetCurrentStep() );
      if ( oldStep )
        {
        oldStep->HideUserInterface();
        }
      wizard->AttemptToGoToNextStep();
      }
    }
  else
    {
    steps = - steps;
    for ( int i = 0; i < steps; i ++ )
      {
        // Hide old step UI.
      vtkPerkStationStep* oldStep =
        vtkPerkStationStep::SafeDownCast( wizard->GetCurrentStep() );
      if ( oldStep )
        {
        oldStep->HideUserInterface();
        }
      wizard->AttemptToGoToPreviousStep();
      }
    }
  
    // Show new step UI.
  vtkPerkStationStep* step =
    vtkPerkStationStep::SafeDownCast( wizard->GetCurrentStep() );
  if ( step )
    {
    step->ShowUserInterface();
    }
  
  
    // Secondary monitor.
  
  if ( phase == this->Calibrate )
    {
    this->SecondaryMonitor->ShowCalibrationControls( true );
    this->SecondaryMonitor->UpdateImageDisplay();
    }
  else
    {
    this->SecondaryMonitor->ShowCalibrationControls( false );
    this->SecondaryMonitor->UpdateImageDisplay();
    }
  
  
  if ( phase != this->Plan )
    {
    this->PlanStep->RemoveOverlayNeedleGuide();
    }
  
  
    // Change working volume in validation phase.
  
  if ( phase == this->Validate && step_from != this->Validate )
    {
    this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID(
      this->GetMRMLNode()->GetValidationVolumeNode()->GetID() );
    this->GetApplicationLogic()->PropagateVolumeSelection();
    }
  else if ( step_from == this->Validate )
    {
    this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID(
      this->GetMRMLNode()->GetPlanningVolumeNode()->GetID() );
    this->GetApplicationLogic()->PropagateVolumeSelection();
    }
  
  
  return 1;  // Indicating success.
}


/**
 * Updates the workphase button faces according to current wizard state.
 */
void
vtkPerkStationModuleGUI
::UpdateWorkphaseButtons()
{
  int currentStep = this->MRMLNode->GetCurrentStep();
  
  for ( int i = 0; i < this->WorkphaseButtonSet->GetNumberOfWidgets(); ++ i )
    {
    if ( i == currentStep )
      {
      this->WorkphaseButtonSet->GetWidget( i )->SetReliefToSunken();
      }
    else
      {
      this->WorkphaseButtonSet->GetWidget( i )->SetReliefToGroove();
      }
    }
}
