
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
 * Protected constructor.
 */
vtkPerkStationModuleGUI
::vtkPerkStationModuleGUI()
{
    // gui elements
  
  this->VolumeSelector = vtkSmartPointer< vtkSlicerNodeSelectorWidget >::New();
  this->PSNodeSelector = vtkSmartPointer< vtkSlicerNodeSelectorWidget >::New();

  this->LoadPlanningVolumeButton =
    vtkSmartPointer< vtkKWLoadSaveButton >::New();
  this->LoadValidationVolumeButton = 
    vtkSmartPointer< vtkKWLoadSaveButton >::New();

  this->LoadExperimentFileButton =
    vtkSmartPointer< vtkKWLoadSaveButton >::New();
  this->SaveExperimentFileButton = 
    vtkSmartPointer< vtkKWLoadSaveButton >::New();

  
  this->Logic = NULL;
  this->MRMLNode = NULL;
  
  
  this->WizardWidget = vtkSmartPointer< vtkKWWizardWidget >::New();
  
  
    // secondary monitor
  
  this->SecondaryMonitor =
                       vtkSmartPointer< vtkPerkStationSecondaryMonitor >::New();
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
  this->DisplayVolumeWindowValue =
                                vtkSmartPointer< vtkKWScaleWithEntry >::New();

  this->Built = false;
  this->SliceOffset = 0;

  this->ObserverCount = 0;
  
  
    // Workphase handling.
  
  this->WorkphaseButtonFrame = vtkSmartPointer< vtkKWFrame >::New();
  this->WorkphaseButtonSet = vtkSmartPointer< vtkKWPushButtonSet >::New();
}


//----------------------------------------------------------------------------
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
void vtkPerkStationModuleGUI::PrintSelf( ostream& os, vtkIndent indent )
{
  
}

//------------------------------------------------------------------------------
void vtkPerkStationModuleGUI::Enter()
{
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast(
      this->GetApplicationGUI ( ));
    p->RepackMainViewer ( vtkMRMLLayoutNode::SlicerLayoutOneUpSliceView, "Red");       
    }
  
  
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  
  if ( n == NULL )
    {
    // no parameter node selected yet, create new
    this->PSNodeSelector->SetSelectedNew( "vtkMRMLPerkStationModuleNode" );
    this->PSNodeSelector->ProcessNewNodeCommand(
      "vtkMRMLPerkStationModuleNode", "PS" );
    n = vtkMRMLPerkStationModuleNode::SafeDownCast(
          this->PSNodeSelector->GetSelected() );

    // set an observe new node in Logic
    this->Logic->SetAndObservePerkStationModuleNode( n );
    vtkSetAndObserveMRMLNodeMacro( this->MRMLNode, n );

    // add the transform of mrml node to the MRML scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode(
            this->MRMLNode->GetCalibrationMRMLTransformNode() );
    vtkMRMLLinearTransformNode *node = NULL;
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(
      vtkMRMLTransformableNode::TransformModifiedEvent );
    vtkSetAndObserveMRMLNodeMacro(
      node, this->MRMLNode->GetCalibrationMRMLTransformNode() );
    vtkSetAndObserveMRMLNodeEventsMacro(
      node, this->MRMLNode->GetCalibrationMRMLTransformNode(), nodeEvents );
    nodeEvents->Delete();
    // add MRMLFiducialListNode to the scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode(
            this->MRMLNode->GetPlanMRMLFiducialListNode() );
    
    // add listener to the slice logic, so that any time user makes change to slice viewer in laptop, the display needs to be updated on secondary monitor
    // e.g. user may move to a certain slice in a series of slices
  
    vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic();
    if (sliceLogic)
      {  
      sliceLogic->GetSliceNode()->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
      }
    
    this->SecondaryMonitor->SetPSNode( n );
    
    
      // This is the place where module node is created,
      // and should be initialized.
    
    // this->CalibrateStep->HardwareSelected( 0 );
    
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
  
  this->PSNodeSelector->AddObserver(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
    (vtkCommand*)( this->GUICallbackCommand ) );
  
  
  vtkInteractorStyle* iStyle = vtkInteractorStyle::SafeDownCast(
    appGUI->GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle() );
  
  
    // add listener to main slicer's red slice view
  iStyle->AddObserver( vtkCommand::LeftButtonPressEvent,
                       ( vtkCommand* )this->GUICallbackCommand );
  
    // calibrate planning image
  iStyle->AddObserver( vtkCommand::KeyPressEvent,
                       ( vtkCommand* )this->GUICallbackCommand );
  
  iStyle->AddObserver( vtkCommand::MouseMoveEvent,
                       ( vtkCommand* )this->GUICallbackCommand );
  
  
    // load volumes buttons
  if ( this->LoadPlanningVolumeButton )
    {
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()
      ->AddObserver( vtkKWTopLevel::WithdrawEvent,
                     (vtkCommand *)this->GUICallbackCommand );
    }
  if ( this->LoadValidationVolumeButton )
    {
    this->LoadValidationVolumeButton->GetLoadSaveDialog()
      ->AddObserver( vtkKWTopLevel::WithdrawEvent,
                     ( vtkCommand* )this->GUICallbackCommand );
    }
  
  
    // ----------------------------------------------------------------
    // Workphase pushbutton set.
  
  for ( int i = 0; i < this->WorkphaseButtonSet->GetNumberOfWidgets(); ++ i )
    {
    this->WorkphaseButtonSet->GetWidget( i )->AddObserver(
      vtkKWPushButton::InvokedEvent,
      ( vtkCommand* )( this->GUICallbackCommand ) );
    }
  
  
    // ----------------------------------------------------------------
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
  
  this->DisplayVolumeLevelValue->AddObserver(
    vtkKWScale::ScaleValueChangedEvent,
    ( vtkCommand* )this->GUICallbackCommand );
  
  this->DisplayVolumeWindowValue->AddObserver(
    vtkKWScale::ScaleValueChangedEvent,
    ( vtkCommand* )this->GUICallbackCommand );

  this->ObserverCount++;
  
}



//---------------------------------------------------------------------------
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

  if (this->LoadPlanningVolumeButton)
    {
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->RemoveObservers(
      vtkKWTopLevel::WithdrawEvent, (vtkCommand*)( this->GUICallbackCommand ) );
    }
  if (this->LoadValidationVolumeButton)
    {
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->RemoveObservers(
      vtkKWTopLevel::WithdrawEvent, (vtkCommand*)( this->GUICallbackCommand ) );
    }

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
  
  if ( vtkSlicerNodeSelectorWidget::SafeDownCast( caller )
       == this->PSNodeSelector
       && (    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
            || event == vtkSlicerNodeSelectorWidget::NewNodeEvent ) )
    {
    vtkMRMLPerkStationModuleNode* node =
      vtkMRMLPerkStationModuleNode::SafeDownCast(
                                         this->PSNodeSelector->GetSelected() );
    
    this->SetMRMLNode( node );
    this->UpdateGUI();
    
    return;
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
   this->SecondaryMonitor->UpdateImageDataOnSliceOffset( this->SliceOffset );
   this->MRMLNode->SetCurrentSliceOffset( this->SliceOffset );
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
  
  
    // Load planning volume.
  
  if ( this->LoadPlanningVolumeButton
       && this->LoadPlanningVolumeButton->GetLoadSaveDialog()
       == vtkKWLoadSaveDialog::SafeDownCast( caller )
       && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
    // load planning volume dialog button
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "PSOpenPathPlanVol" );
    const char *fileName =
      this->LoadPlanningVolumeButton->GetLoadSaveDialog()->GetFileName();    
    if ( fileName ) 
      {
      this->LoadPlanningVolumeButton->GetLoadSaveDialog()->
        SaveLastPathToRegistry( "PSOpenPathPlanVol" );
      
        // call the callback function
      this->LoadPlanningVolumeButtonCallback( fileName ); 
      this->LoadPlanningVolumeButton->SetText( "Load planning volume" );
      } 
    }
  
  
    // Load validation volume.
  
  if ( this->LoadValidationVolumeButton
       && this->LoadValidationVolumeButton->GetLoadSaveDialog()
       == vtkKWLoadSaveDialog::SafeDownCast( caller )
       && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
    // load validation volume dialog button    
    const char *fileName = this->LoadValidationVolumeButton->
      GetLoadSaveDialog()->GetFileName();    
    if ( fileName ) 
      {      
      // call the callback function
      this->LoadValidationVolumeButtonCallback(fileName);   
      this->LoadValidationVolumeButton->SetText( "Validation volume loaded" );
      } 
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
  
  if ( this->DisplayVolumeLevelValue
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
  
  if ( this->DisplayVolumeWindowValue
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
  
  
  vtkSlicerNodeSelectorWidget *selector
    = vtkSlicerNodeSelectorWidget::SafeDownCast( caller );
    
  
  if ( selector == this->VolumeSelector
       && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
       && this->VolumeSelector->GetSelected() != NULL ) 
    { 
    //this->UpdateMRML();
    // handle here, when user changes the selection of the active volume
    // the selection has to be propagated in Slicer's viewers and enviroment
    // the system state has to updated that the volume in use is the one that is selected
    // the secondary monitor has to be updated
    // all this to be done here
    }
  
  
  if ( selector == this->PSNodeSelector
       && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent
       && this->PSNodeSelector->GetSelected() != NULL ) 
    { 
    vtkMRMLPerkStationModuleNode* n =
      vtkMRMLPerkStationModuleNode::SafeDownCast(
        this->PSNodeSelector->GetSelected() );
    this->Logic->SetAndObservePerkStationModuleNode( n );
    vtkSetAndObserveMRMLNodeMacro( this->MRMLNode, n );
    this->UpdateGUI();
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
    std::stringstream ss;
    ss << str;
    ss << std::endl;
    ss << pptext;
    anno->SetText( 2, ss.str().c_str() );
    }
}


/**
 * Updates parameters values in MRML node based on GUI widgets.
 */
void vtkPerkStationModuleGUI::UpdateMRML ()
{
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  
  if (n == NULL)
    {
      // no parameter node selected yet, create new
    this->PSNodeSelector->SetSelectedNew("vtkMRMLPerkStationModuleNode");
    this->PSNodeSelector->ProcessNewNodeCommand("vtkMRMLPerkStationModuleNode", "PS");
    n = vtkMRMLPerkStationModuleNode::SafeDownCast(this->PSNodeSelector->GetSelected());

    // set an observe new node in Logic
    this->Logic->SetAndObservePerkStationModuleNode(n);
    vtkSetAndObserveMRMLNodeMacro(this->MRMLNode, n);

    // add the transform of mrml node to the MRML scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetCalibrationMRMLTransformNode());
    vtkMRMLLinearTransformNode *node = NULL;
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
    vtkSetAndObserveMRMLNodeMacro(node, this->MRMLNode->GetCalibrationMRMLTransformNode());
    vtkSetAndObserveMRMLNodeEventsMacro(node,this->MRMLNode->GetCalibrationMRMLTransformNode(),nodeEvents);
    // add MRMLFiducialListNode to the scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetPlanMRMLFiducialListNode());

    // add listener to the slice logic, so that any time user makes change
    // to slice viewer in laptop, the display needs to be updated on secondary monitor
    // e.g. user may move to a certain slice in a series of slices
  
    vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic();
    if (sliceLogic)
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
      if (n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Validation");          
          return;
          }
        }

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
        // this implies that a validation volume already existed but now there is new image/volume chosen as validation volume in validate step
        }
           

      // validate volume set
      n->SetValidationVolumeRef(this->VolumeSelector->GetSelected()->GetID());    
      n->SetValidationVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetValidationVolumeRef())));
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

//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::UpdateGUI ()
{
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  if ( n == NULL )
    {
    return;
    }
  
  this->CalibrateStep->UpdateGUI();
  
  
  // set GUI widgest from parameter node
  /*this->ConductanceScale->SetValue(n->GetConductance());
  
  this->TimeStepScale->SetValue(n->GetTimeStep());
  
  this->NumberOfIterationsScale->SetValue(n->GetNumberOfIterations());

  this->UseImageSpacing->SetSelectedState(n->GetUseImageSpacing());*/
  
  // update in the mrml scene with latest transform that we have
  // TODO:
  //this->GetLogic()->GetMRMLScene()->UpdateNode();
    
}

//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event,
                                            void *callData ) 
{
  // if parameter node has been changed externally, update GUI widgets with new values
  vtkMRMLPerkStationModuleNode* node = vtkMRMLPerkStationModuleNode::SafeDownCast(caller);
  vtkMRMLLinearTransformNode *transformNode = vtkMRMLLinearTransformNode::SafeDownCast(caller);
  vtkMRMLScalarVolumeDisplayNode *displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(caller);

  if (node != NULL && this->GetMRMLNode() == node ) 
  {
    this->UpdateGUI();
  }
  
  if (displayNode != NULL && this->GetMRMLNode()->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode() == displayNode && event == vtkCommand::ModifiedEvent)
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
  
    // MRML node
  
  this->PSNodeSelector->SetNodeClass( "vtkMRMLPerkStationModuleNode", NULL,
                                      NULL, "PS Parameters" );
  this->PSNodeSelector->SetParent( volSelFrame );
  this->PSNodeSelector->Create();
  this->PSNodeSelector->SetNewNodeEnabled( 1 );
  this->PSNodeSelector->NoneEnabledOn();
  this->PSNodeSelector->SetShowHidden( 1 );
  this->PSNodeSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
  this->PSNodeSelector->UpdateMenu();

  this->PSNodeSelector->SetBorderWidth( 2 );
  this->PSNodeSelector->SetLabelText( "PERK Parameters");
  this->PSNodeSelector->SetBalloonHelpString(
    "select a PS node from the current mrml scene." );
  
  app->Script( "pack %s -side left -anchor w -padx 2 -pady 4", 
               this->PSNodeSelector->GetWidgetName() );
  
    //input volume selector
  
  this->VolumeSelector->SetNodeClass( "vtkMRMLScalarVolumeNode", NULL,
                                      NULL, NULL );
  this->VolumeSelector->SetParent( volSelFrame );
  this->VolumeSelector->Create();
  this->VolumeSelector->SetMRMLScene( this->Logic->GetMRMLScene() );
  this->VolumeSelector->UpdateMenu();
  this->VolumeSelector->SetBorderWidth(2);
  this->VolumeSelector->SetLabelText( "Active Volume: ");
  this->VolumeSelector->SetBalloonHelpString(
    "select an input volume from the current mrml scene." );
  
  app->Script( "pack %s -side top -anchor e -padx 2 -pady 4", 
               this->VolumeSelector->GetWidgetName() );
  
    // Load volumes frame.
  
  vtkSmartPointer< vtkKWFrame > loadVolFrame =
      vtkSmartPointer< vtkKWFrame >::New();
    loadVolFrame->SetParent( loadSaveExptFrame->GetFrame() );
    loadVolFrame->Create();     
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                loadVolFrame->GetWidgetName() );
  
    // Load planning volume dialog button.
  
  if ( ! this->LoadPlanningVolumeButton->IsCreated() )
    {
    this->LoadPlanningVolumeButton->SetParent( loadVolFrame );
    this->LoadPlanningVolumeButton->Create();
    this->LoadPlanningVolumeButton->SetBorderWidth( 2 );
    this->LoadPlanningVolumeButton->SetReliefToRaised();       
    this->LoadPlanningVolumeButton->SetHighlightThickness( 2 );
    this->LoadPlanningVolumeButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    this->LoadPlanningVolumeButton->SetActiveBackgroundColor( 1, 1, 1 );
    this->LoadPlanningVolumeButton->SetText( "Load planning volume" );
    this->LoadPlanningVolumeButton->SetImageToPredefinedIcon(
      vtkKWIcon::IconPresetLoad );
    this->LoadPlanningVolumeButton->SetBalloonHelpString(
      "Click to load a planning image or a volume" );
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "OpenPath" );
    this->LoadPlanningVolumeButton->TrimPathFromFileNameOn();
    this->LoadPlanningVolumeButton->SetMaximumFileNameLength( 256 );
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->SaveDialogOff();
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->SetFileTypes(
      "{ {DICOM Files} {*} }" );      
    }
  
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->LoadPlanningVolumeButton->GetWidgetName() );

    // Load validation volume dialog button.
  
  if ( ! this->LoadValidationVolumeButton->IsCreated() )
    {
    this->LoadValidationVolumeButton->SetParent(loadVolFrame);
    this->LoadValidationVolumeButton->Create();
    this->LoadValidationVolumeButton->SetBorderWidth(2);
    this->LoadValidationVolumeButton->SetReliefToRaised();       
    this->LoadValidationVolumeButton->SetHighlightThickness(2);
    this->LoadValidationVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadValidationVolumeButton->SetActiveBackgroundColor(1,1,1);
    this->LoadValidationVolumeButton->SetText("Load validation volume");
    this->LoadValidationVolumeButton->SetImageToPredefinedIcon(
      vtkKWIcon::IconPresetLoad );
    this->LoadValidationVolumeButton->SetBalloonHelpString(
      "Click to load a validation image or a volume" );
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "OpenPath" );
    this->LoadPlanningVolumeButton->TrimPathFromFileNameOn();
    this->LoadValidationVolumeButton->SetMaximumFileNameLength( 256 );
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->SaveDialogOff();
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->SetFileTypes(
      "{ {DICOM Files} {*} }" );
    this->LoadValidationVolumeButton->SetEnabled( 0 );
    }
  
  this->Script( "pack %s -side top -anchor ne -padx 2 -pady 2", 
                this->LoadValidationVolumeButton->GetWidgetName() );
  
    // Window level frame.
  
  vtkSmartPointer< vtkKWFrameWithLabel > windowLevelFrame =
      vtkSmartPointer< vtkKWFrameWithLabel >::New();
    windowLevelFrame->SetParent( loadSaveExptFrame->GetFrame() );
    windowLevelFrame->SetLabelText( "Window/Level" );
    windowLevelFrame->Create();
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                windowLevelFrame->GetWidgetName() );
  
  
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
  
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->DisplayVolumeWindowValue->GetWidgetName() );
  
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
  
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 2", 
                        this->DisplayVolumeLevelValue->GetWidgetName());
  
  
    // Load save frame.
  
  vtkSmartPointer< vtkKWFrame > loadSaveFrame =
      vtkSmartPointer< vtkKWFrame >::New();
    loadSaveFrame->SetParent( loadSaveExptFrame->GetFrame() );
    loadSaveFrame->Create();
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                loadSaveFrame->GetWidgetName() );

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
    this->SaveExperimentFileButton->SetParent(loadSaveFrame);
    this->SaveExperimentFileButton->Create();
    this->SaveExperimentFileButton->SetText( "Save experiment" );
    this->SaveExperimentFileButton->SetBorderWidth(2);
    this->SaveExperimentFileButton->SetReliefToRaised();       
    this->SaveExperimentFileButton->SetHighlightThickness(2);
    this->SaveExperimentFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->SaveExperimentFileButton->SetActiveBackgroundColor(1,1,1);               
    this->SaveExperimentFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconFloppy);
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
  if ( this->LoadPlanningVolumeButton )
    {
    this->LoadPlanningVolumeButton->SetParent( NULL );
    if( this->LoadPlanningVolumeButton->IsCreated() )
      {      
      // vtkKWLoadSaveButton set itself as a parent of its member function
      // LoadSaveDialog automatically when it is created by LoadSaveDialog->SetParent(this),       
      // but it does not do LoadSaveDialog->SetParent(NULL) in its destructor, so we do it here.
      this->LoadPlanningVolumeButton->GetLoadSaveDialog()->SetParent(NULL);
      }
    }
  
  if (this->LoadValidationVolumeButton)
    {
    this->LoadValidationVolumeButton->SetParent(NULL);
    if(this->LoadValidationVolumeButton->IsCreated())
      {
      // same as the above reason
      this->LoadValidationVolumeButton->GetLoadSaveDialog()->SetParent(NULL);
      }
    }
  
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
    vtkErrorMacro("No secondary monitor detected");
    }
}


//---------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::ResetAndStartNewExperiment()
{
  // objectives:
  // 1) Reset individual work phase steps to bring to fresh state, who are in turn, responsible for reseting MRML node parameters
  // 2) Go back to the first step in GUI in workflow widget

  // 1) Reset individual work phase steps to bring to fresh state, who are in turn, responsible for reseting MRML node parameters
  this->CalibrateStep->Reset();
  this->PlanStep->Reset();
  this->InsertStep->Reset();
  this->ValidateStep->Reset();

  // 2)Go back to the first step in GUI in workflow widget
  while (this->WizardWidget->GetWizardWorkflow()->GetCurrentState()!= this->WizardWidget->GetWizardWorkflow()->GetInitialState())
    {
    this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
    }
  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
}


//----------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::SaveExperiment( ostream& of )
{
  if ( ! this->GetMRMLNode() ) return;

  // save information about image volumes used
  this->SaveVolumeInformation( of );
  
  this->MRMLNode->SaveExperiment( of );
}


//----------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::LoadExperiment( istream &file )
{
  // reset before you load
  // 1) Reset individual work phase steps to bring to fresh state, who are
  // in turn, responsible for reseting MRML node parameters
  this->CalibrateStep->Reset();
  this->PlanStep->Reset();
  this->InsertStep->Reset();
  this->ValidateStep->Reset();

  this->MRMLNode->LoadExperiment( file );
}


//---------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::SaveExperimentButtonCallback( const char* fileName )
{
    ofstream file( fileName );

    this->SaveExperiment( file );
    
    file.close();
}


//-----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::LoadExperimentButtonCallback(const char *fileName)
{
    ifstream file(fileName);    
    this->LoadExperiment( file );
    file.close();
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


//-----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SaveVolumeInformation( ostream& of )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  vtkMRMLScalarVolumeNode *planVol = mrmlNode->GetPlanningVolumeNode();
  if (planVol == NULL)
    {
    return;
    }
  const itk::MetaDataDictionary &planVolDictionary = 
    planVol->GetMetaDataDictionary();

  std::string tagValue;

  

  // following information needs to be written to the file

  // 1) Volume location file path
  // 2) Series number from dictionary
  // 3) Series description from dictionary
  // 4) FOR UID from dictionary
  // 5) Origin from node
  // 6) Pixel spacing from node
  
  // entry point
  of << " PlanVolumeFileLocation=\"" ;
//  of <<  << " ";
  of << "\" \n";
  
  // series number
  tagValue.clear(); itk::ExposeMetaData<std::string>( planVolDictionary, "0020|0011", tagValue );
  of << " PlanVolumeSeriesNumber=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // series description
  tagValue.clear(); itk::ExposeMetaData<std::string>( planVolDictionary, "0008|103e", tagValue );
  of << " PlanVolumeSeriesDescription=\"" ;
  of << tagValue << " ";
  of << "\" \n";
   
  // frame of reference uid
  tagValue.clear(); itk::ExposeMetaData<std::string>( planVolDictionary, "0020|0052", tagValue );
  of << " PlanVolumeFORUID=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // origin
  of << " PlanVolumeOrigin=\"" ;
  double origin[3];
  planVol->GetOrigin(origin);
  for(int i = 0; i < 3; i++)
      of << origin[i] << " ";
  of << "\" \n";
  
  
  // spacing
  of << " PlanVolumeSpacing=\"" ;
  double spacing[3];
  planVol->GetSpacing(spacing);
  for(int i = 0; i < 3; i++)
      of << spacing[i] << " ";
  of << "\" \n";
  
  
  vtkMRMLScalarVolumeNode *validateVol = mrmlNode->GetValidationVolumeNode();
  if (validateVol == NULL)
    {
    return;
    }
  const itk::MetaDataDictionary &validateVolDictionary = validateVol->GetMetaDataDictionary();
  
  
  
  // following information needs to be written to the file
  
  // 1) Volume location file path
  // 2) Series number from dictionary
  // 3) Series description from dictionary
  // 4) FOR UID from dictionary
  // 5) Origin from node
  // 6) Pixel spacing from node
  
  // entry point
  of << " ValidationVolumeFileLocation=\"" ;
//  of <<  << " ";
  of << "\" \n";
  
  // series number
  tagValue.clear(); itk::ExposeMetaData<std::string>( validateVolDictionary, "0020|0011", tagValue );
  of << " ValidateVolumeSeriesNumber=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // series description
  tagValue.clear(); itk::ExposeMetaData<std::string>( validateVolDictionary, "0008|103e", tagValue );
  of << " ValidateVolumeSeriesDescription=\"" ;
  of << tagValue << " ";
  of << "\" \n";
   
  // frame of reference uid
  tagValue.clear(); itk::ExposeMetaData<std::string>( validateVolDictionary, "0020|0052", tagValue );
  of << " ValidateVolumeFORUID=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // origin
  of << " ValidateVolumeOrigin=\"" ; 
  validateVol->GetOrigin(origin);
  for(int i = 0; i < 3; i++)
      of << origin[i] << " ";
  of << "\" \n";
  

  // spacing
  of << " ValidateVolumeSpacing=\"" ; 
  validateVol->GetSpacing(spacing);
  for(int i = 0; i < 3; i++)
      of << spacing[i] << " ";
  of << "\" \n";
}


//-----------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::LoadPlanningVolumeButtonCallback( const char *fileName )
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }  

  bool planningVolumePreExists = false;
  
  
    // in case the planning volume already exists, it is just that the user has
    // loaded another dicom series as a new planning volume
    // then later in the code, this implies that the experiment has to be
    // started over
  if ( this->MRMLNode->GetPlanningVolumeNode() )
    {
    planningVolumePreExists = true;
    }

  this->MRMLNode->SetVolumeInUse( "Planning" );

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>( 
    this->GetApplication() );
  vtkMRMLScalarVolumeNode *volumeNode = this->GetLogic()->
    AddVolumeToScene( app, fileString.c_str(), VOL_CALIBRATE_PLAN );
  
  
  if ( ! volumeNode )  // Not successful image read.
    {
    std::string msg = std::string( "Unable to read volume file " )
                      + std::string(fileName);
    
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
      dialog->SetStyleToMessage();
      dialog->SetText( msg.c_str() );
      dialog->Create();
      dialog->Invoke();
      dialog->Delete();
    
    return;
    }
  
  
  vtkMRMLScalarVolumeDisplayNode *node = NULL;
  vtkSetAndObserveMRMLNodeMacro( node,
    volumeNode->GetScalarVolumeDisplayNode() );

    // set up the image on secondary monitor    
  this->SecondaryMonitor->SetupImageData();
  
  if ( ! planningVolumePreExists)
    {
      // bring the wizard GUI back to Calibrate step
      // the volume selection has changed/added, so make sure that the wizard
      // is in the intial calibration state!
    while( this->WizardWidget->GetWizardWorkflow()->GetCurrentState()
           != this->WizardWidget->GetWizardWorkflow()->GetInitialState() )
      {
      this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
      }
    this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->
      ShowUserInterface();
    }
  else // Selected image could not be read.
    {
    this->ResetAndStartNewExperiment();
    }
      
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
  
  
    // Make the planning volume the selected volume.
  this->VolumeSelector->SetSelected( volumeNode );
  const char *strName = this->VolumeSelector->GetSelected()->GetName();
  std::string strPlan = std::string( strName ) + "-Plan";
  this->VolumeSelector->GetSelected()->SetName( strPlan.c_str() );
  this->VolumeSelector->GetSelected()->SetDescription(
    "Planning image/volume; created by PerkStation module" );
  this->VolumeSelector->GetSelected()->Modified();
  this->VolumeSelector->UpdateMenu();
  
    // enable the load validation volume button
  this->EnableLoadValidationVolumeButton( true );
  
}

  
// ----------------------------------------------------------------------------
void
vtkPerkStationModuleGUI
::LoadValidationVolumeButtonCallback( const char *fileName )
{
  std::string fileString( fileName );
  for ( unsigned int i = 0; i < fileString.length(); i ++ )
    {
    if (fileString[ i ] == '\\')
      {
      fileString[ i ] = '/';
      }
    }
  
  bool validationVolumePreExists = false;

    // in case the planning volume already exists, it is just that the user has
    // loaded another dicom series as a new planning volume then later in the 
    // code, this implies that the experiment has to be started over
  if ( this->MRMLNode->GetValidationVolumeNode() )
    {
    validationVolumePreExists = true;
    }

  this->MRMLNode->SetVolumeInUse( "Validation" );

  vtkSlicerApplication* app =
    static_cast< vtkSlicerApplication* >( this->GetApplication() );
  vtkMRMLScalarVolumeNode* volumeNode =
    this->GetLogic()->AddVolumeToScene( app, fileString.c_str(),
                                        VOL_VALIDATION );
        
  if ( volumeNode )
    {
      // as of now, don't want to disturb display on the secondary monitor
      // i.e. don't want the secondary monitor to display or overlay the
      // validation image if such a request come from clinician, it will be
      // handled then

      // set the window/level controls values from the scalar volume node
    this->DisplayVolumeLevelValue->SetValue(
      volumeNode->GetScalarVolumeDisplayNode()->GetLevel() );
    this->DisplayVolumeWindowValue->SetValue(
      volumeNode->GetScalarVolumeDisplayNode()->GetWindow() );
    
    this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID(
      volumeNode->GetID() );
    this->GetApplicationLogic()->PropagateVolumeSelection();
    
    this->GetLogic()->AdjustSliceView();  // Position as the planning volume.
    
    this->VolumeSelector->SetSelected(volumeNode);
    const char *strName = this->VolumeSelector->GetSelected()->GetName();
    std::string strPlan = std::string( strName ) + "-Validation";
    this->VolumeSelector->GetSelected()->SetName( strPlan.c_str() );
    this->VolumeSelector->GetSelected()->SetDescription(
      "Validation image/volume; created by PerkStation module" );
    this->VolumeSelector->GetSelected()->Modified();
    
    this->VolumeSelector->UpdateMenu();
    }
  else 
    {
    std::string msg = std::string( "Unable to read volume file " )
      + std::string( fileName );
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
      dialog->SetStyleToMessage();
      dialog->SetText(msg.c_str());
      dialog->Create ( );
      dialog->Invoke();
      dialog->Delete();
    
      // default to planning
    this->MRMLNode->SetVolumeInUse( "Planning" );
    }
}


// ----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::EnableLoadValidationVolumeButton( bool enable )
{
  if ( this->LoadValidationVolumeButton )
    {
    this->LoadValidationVolumeButton->SetEnabled( enable );
    }
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
