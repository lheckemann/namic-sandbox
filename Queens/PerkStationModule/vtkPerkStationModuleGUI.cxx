/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkPerkStationModuleGUI.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerModuleCollapsibleFrame.h"

#include "vtkCommand.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageChangeInformation.h"
#include "vtkImageReslice.h"
#include "vtkMatrixToHomogeneousTransform.h"

#include "vtkKWApplication.h"
#include "vtkKWLabel.h"
#include "vtkKWWidget.h"
#include "vtkKWFrame.h"
#include "vtkKWComboBox.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkPerkStationCalibrateStep.h"
#include "vtkPerkStationPlanStep.h"
#include "vtkPerkStationInsertStep.h"
#include "vtkPerkStationValidateStep.h"
#include "vtkPerkStationEvaluateStep.h"

#include "vtkPerkStationSecondaryMonitor.h"
// define modes
#define MODE_CLINICAL 1
#define MODE_TRAINING 2
// define states
#define STATE_CALIBRATION 1
#define STATE_PLAN 2
#define STATE_INSERT 3
#define STATE_VALIDATE 4
#define STATE_COMPARE 5

// define sub states
#define SUBSTATE_SCALE  1
#define SUBSTATE_TRANSLATE  2
#define SUBSTATE_ROTATE 3

//------------------------------------------------------------------------------
vtkPerkStationModuleGUI* vtkPerkStationModuleGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPerkStationModuleGUI");
  if(ret)
    {
      return (vtkPerkStationModuleGUI*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPerkStationModuleGUI;
}


//----------------------------------------------------------------------------
vtkPerkStationModuleGUI::vtkPerkStationModuleGUI()
{
  // gui elements
  this->VolumeSelector = vtkSlicerNodeSelectorWidget::New();
  this->PSNodeSelector = vtkSlicerNodeSelectorWidget::New();

  //logic and mrml nodes
  this->Logic = NULL;
  this->MRMLNode = NULL;

  this->WizardWidget = vtkKWWizardWidget::New();

  // wizard workflow
  this->CalibrateStep = NULL;
  this->PlanStep = NULL;
  this->InsertStep = NULL;
  this->ValidateStep = NULL;
  this->EvaluateStep = NULL;

  // secondary monitor
  this->SecondaryMonitor = NULL;

  this->SecondaryMonitor = vtkPerkStationSecondaryMonitor::New();
  this->SecondaryMonitor->SetGUI(this);
  this->SecondaryMonitor->Initialize();

  // state descriptors
  this->ModeBox = NULL;
  this->Mode = MODE_TRAINING;
  this->StateButtonSet = NULL;
  this->State = STATE_CALIBRATION;  
  this->SubState = SUBSTATE_TRANSLATE;
}

//----------------------------------------------------------------------------
vtkPerkStationModuleGUI::~vtkPerkStationModuleGUI()
{
  
  this->SetLogic (NULL);
  vtkSetMRMLNodeMacro(this->MRMLNode, NULL);

  if (this->ModeBox)
    {
    this->ModeBox->SetParent(NULL);
    this->ModeBox->Delete();
    this->ModeBox = NULL;
    }
    
  if (this->StateButtonSet)
    {
    this->StateButtonSet->SetParent(NULL);
    this->StateButtonSet->Delete();
    this->StateButtonSet = NULL;
    }

  if ( this->VolumeSelector ) 
    {
    this->VolumeSelector->SetParent(NULL);
    this->VolumeSelector->Delete();
    this->VolumeSelector = NULL;
    }
  if ( this->PSNodeSelector ) 
    {
    this->PSNodeSelector->SetParent(NULL);
    this->PSNodeSelector->Delete();
    this->PSNodeSelector = NULL;
    }

  if (this->WizardWidget)
    {
    this->WizardWidget->Delete();
    this->WizardWidget = NULL;
    }

  if (this->CalibrateStep)
    {
    this->CalibrateStep->Delete();
    this->CalibrateStep = NULL;
    }

  if (this->PlanStep)
    {
    this->PlanStep->Delete();
    this->PlanStep = NULL;
    }

  if (this->InsertStep)
    {
    this->InsertStep->Delete();
    this->InsertStep = NULL;
    }

  if (this->ValidateStep)
    {
    this->ValidateStep->Delete();
    this->ValidateStep = NULL;
    }

  if (this->EvaluateStep)
    {
    this->EvaluateStep->Delete();
    this->EvaluateStep = NULL;
    }
  
}

//----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  
}

//------------------------------------------------------------------------------
void vtkPerkStationModuleGUI::Enter()
{
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    p->RepackMainViewer ( vtkMRMLLayoutNode::SlicerLayoutOneUpSliceView, "Red");

    // to make sure the slice gets displayed without any scaling
    vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI0()->GetLogic();
    vtkMRMLSliceNode *sliceNode = sliceLogic->GetSliceNode();
    //sliceNode->SetFieldOfView()

    //this->RenderSecondaryMonitor();
    }
  
}
//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::AddGUIObservers ( ) 
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  // add listener to main slicer's red slice view
  appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
   
  
  // add listener to own render window created for secondary monitor
  this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);

  this->VolumeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->PSNodeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );    
}



//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::RemoveGUIObservers ( )
{
  this->VolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->PSNodeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );    
}



//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::ProcessGUIEvents ( vtkObject *caller,
                                           unsigned long event,
                                           void *callData ) 
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);
  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    this->CalibrateStep->ProcessImageClickEvents(caller, event, callData);  
    this->PlanStep->ProcessImageClickEvents(caller, event, callData);
    this->ValidateStep->ProcessImageClickEvents(caller, event, callData);
    }
  else
    {  
    vtkKWMenu *v = vtkKWMenu::SafeDownCast(caller);
    vtkSlicerNodeSelectorWidget *selector = vtkSlicerNodeSelectorWidget::SafeDownCast(caller);  
    

    if (selector == this->VolumeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent && this->VolumeSelector->GetSelected() != NULL) 
      { 
      this->UpdateMRML();
      }
    
    if (selector == this->PSNodeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  && this->PSNodeSelector->GetSelected() != NULL) 
      { 
      vtkMRMLPerkStationModuleNode* n = vtkMRMLPerkStationModuleNode::SafeDownCast(this->PSNodeSelector->GetSelected());
      this->Logic->SetAndObservePerkStationModuleNode(n);
      vtkSetAndObserveMRMLNodeMacro( this->MRMLNode, n);
      this->UpdateGUI();
      }
    
    }
}

//---------------------------------------------------------------------------
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
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetCalibrationMRMLTransformNode());
    vtkMRMLLinearTransformNode *node = NULL;
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
    vtkSetAndObserveMRMLNodeMacro(node, this->MRMLNode->GetCalibrationMRMLTransformNode());
    vtkSetAndObserveMRMLNodeEventsMacro(node,this->MRMLNode->GetCalibrationMRMLTransformNode(),nodeEvents);
    }

  // save node parameters for Undo
  this->GetLogic()->GetMRMLScene()->SaveStateForUndo(n);
   

  /*n->SetConductance(this->ConductanceScale->GetValue());
  
  n->SetTimeStep(this->TimeStepScale->GetValue());
  
  n->SetNumberOfIterations((int)floor(this->NumberOfIterationsScale->GetValue()));

  n->SetUseImageSpacing(this->UseImageSpacing->GetState());*/
  
  if (this->VolumeSelector->GetSelected() != NULL)
    {
    if (n->GetInputVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        if (strcmpi(n->GetInputVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
            return;
    n->SetInputVolumeRef(this->VolumeSelector->GetSelected()->GetID());
    // the volume selection has changed/added, so make sure that the wizard is in the intial calibration state!
    while (this->WizardWidget->GetWizardWorkflow()->GetCurrentState()!= this->WizardWidget->GetWizardWorkflow()->GetInitialState())
      {
      this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
      }
    this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
    this->SecondaryMonitor->SetupImageData();
    //this->RenderSecondaryMonitor();
    }

  



}

//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::UpdateGUI ()
{
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  if (n != NULL)
    {
    // set GUI widgest from parameter node
    /*this->ConductanceScale->SetValue(n->GetConductance());
    
    this->TimeStepScale->SetValue(n->GetTimeStep());
    
    this->NumberOfIterationsScale->SetValue(n->GetNumberOfIterations());

    this->UseImageSpacing->SetSelectedState(n->GetUseImageSpacing());*/
    
    // update in the mrml scene with latest transform that we have
    // TODO:
    //this->GetLogic()->GetMRMLScene()->UpdateNode();
    }
}

//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event,
                                            void *callData ) 
{
  // if parameter node has been changed externally, update GUI widgets with new values
  vtkMRMLPerkStationModuleNode* node = vtkMRMLPerkStationModuleNode::SafeDownCast(caller);
  vtkMRMLLinearTransformNode *transformNode = vtkMRMLLinearTransformNode::SafeDownCast(caller);

  if (node != NULL && this->GetMRMLNode() == node ) 
  {
    this->UpdateGUI();
  }
  
  /*if (transformNode == this->GetMRMLNode()->GetCalibrationMRMLTransformNode() && event == vtkMRMLTransformableNode::TransformModifiedEvent)
    {
    // this is when the user modifies the calibration transform matrix from "Transforms" module
    // update our internal CalibrationTransform
    // set node parameters from MRML scene
  
    // get the scene collection
    vtkCollection* collection = this->GetLogic()->GetMRMLScene()->GetNodesByName("PerkStationCalibrationTransform");
    int nItems = collection->GetNumberOfItems();
    
    // traverse through scene collection to get the transform node
    if (nItems > 0)
      {
      // pick the very first, well, in fact, there should be only 1
      // i.e. if nItems > 1, there is something wrong
      transformNode = vtkMRMLLinearTransformNode::SafeDownCast(collection->GetItemAsObject(0));        
//    vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();
      this->GetMRMLNode()->SetTransformNodeMatrix(transformNode->GetMatrixTransformToParent());
      //matrix->Delete();
      // actually transform the volume!
      // TO DO:
      }
    }*/
    

 
}




//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::BuildGUI ( ) 
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  //register MRML PS node
  vtkMRMLPerkStationModuleNode* PSNode = vtkMRMLPerkStationModuleNode::New();
  this->Logic->GetMRMLScene()->RegisterNodeClass(PSNode);
  PSNode->Delete();

  this->UIPanel->AddPage ( "PerkStationModule", "PerkStationModule", NULL );
  vtkKWWidget *modulePage =  this->UIPanel->GetPageWidget("PerkStationModule");
  
  // ---
  // MODULE GUI FRAME 
  // ---
  // Define your help text and build the help frame here.
  const char *help = "**PERK Station Module:** **Under Construction** Use this module to perform image overlay guided percutaneous interventions ....";
  const char *about = "This work was supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See <a>http://www.slicer.org</a> for details. ";
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "PerkStationModule" );
  this->BuildHelpAndAboutFrame ( page, help, about );
  
  // add individual collapsible pages/frames


  // Mode collapsible frame with combo box label
  vtkSlicerModuleCollapsibleFrame *modeFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  modeFrame->SetParent(modulePage);
  modeFrame->Create();
  modeFrame->SetLabelText("Mode frame");
  modeFrame->ExpandFrame();
  
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
              modeFrame->GetWidgetName(), modulePage->GetWidgetName());
  
  // add combo box in the frame
  this->ModeBox = vtkKWComboBoxWithLabel::New();
  this->ModeBox->SetParent(modeFrame->GetFrame());
  this->ModeBox->Create();
  this->ModeBox->SetLabelText("Mode");
  this->ModeBox->SetBalloonHelpString("This indicates whether the PERK Station module is being used in CLINICAL or TRAINING mode");
  this->ModeBox->GetWidget()->AddValue("CLINICAL");
  this->ModeBox->GetWidget()->AddValue("TRAINING");  
  this->ModeBox->GetWidget()->ReadOnlyOn();
  this->ModeBox->GetWidget()->SetValue("TRAINING");
  this->ModeBox->GetWidget()->SetStateToDisabled();
  app->Script("pack %s -side top -anchor nw -expand n -padx 4 -pady 4", 
    this->ModeBox->GetWidgetName(), modeFrame->GetWidgetName());

  modeFrame->Delete();

  // Work phase collapsible frame with push buttons/radio buttons set
  vtkSlicerModuleCollapsibleFrame *workPhaseFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  workPhaseFrame->SetParent(modulePage);
  workPhaseFrame->Create();
  workPhaseFrame->SetLabelText("Work phase frame");
  workPhaseFrame->ExpandFrame();
  
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 4 -pady 2",
              workPhaseFrame->GetWidgetName(), modulePage->GetWidgetName());
 
  // add radio buttons set
  this->StateButtonSet = vtkKWRadioButtonSet::New();
  this->StateButtonSet->SetParent(workPhaseFrame->GetFrame());
  this->StateButtonSet->Create();
  this->StateButtonSet->SetBorderWidth(2);
  //this->StateButtonSet->SetReliefToGroove();
  this->StateButtonSet->SetPackHorizontally(1);
  this->StateButtonSet->SetWidgetsInternalPadX(2);
  this->StateButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(5);
  const char *buffer[]= {"Calibrate", "Plan", "Insert", "Validate", "Evaluate"};
  for (int id = 0; id < 5; id++)
    {
    vtkKWRadioButton *radiob = this->StateButtonSet->AddWidget(id);
    radiob->SetText(buffer[id]);
    radiob->SetBorderWidth(2);
    radiob->SetReliefToSunken();
    radiob->SetOffReliefToRaised();
    radiob->SetHighlightThickness(2);
    radiob->IndicatorVisibilityOff();
    //radiob->SetStateToReadOnly();
    radiob->SetBackgroundColor(0.85,0.85,0.85);
    radiob->SetActiveBackgroundColor(1,1,1);
    }
  this->StateButtonSet->GetWidget(0)->SetSelectedState(1);
 

  app->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
    this->StateButtonSet->GetWidgetName());

  workPhaseFrame->Delete();
  
  // collapsible? frame for volume node selection, and parameters selection?
  vtkSlicerModuleCollapsibleFrame *volumeSelFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  volumeSelFrame->SetParent(modulePage);
  volumeSelFrame->Create();
  volumeSelFrame->SetLabelText("Volume selection frame");
  volumeSelFrame->ExpandFrame();  
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
              volumeSelFrame->GetWidgetName(), modulePage->GetWidgetName());
  

  //MRML node
  this->PSNodeSelector->SetNodeClass("vtkMRMLPerkStationModuleNode", NULL, NULL, "PS Parameters");
  this->PSNodeSelector->SetNewNodeEnabled(1);
  this->PSNodeSelector->NoneEnabledOn();
  this->PSNodeSelector->SetShowHidden(1);
  this->PSNodeSelector->SetParent( volumeSelFrame->GetFrame() );
  this->PSNodeSelector->Create();
  this->PSNodeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->PSNodeSelector->UpdateMenu();

  this->PSNodeSelector->SetBorderWidth(2);
  this->PSNodeSelector->SetLabelText( "PERK Parameters");
  this->PSNodeSelector->SetBalloonHelpString("select a PS node from the current mrml scene.");
  app->Script("pack %s -side left -anchor w -padx 2 -pady 4", 
                this->PSNodeSelector->GetWidgetName());

  //input volume selector
  this->VolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
  this->VolumeSelector->SetParent( volumeSelFrame->GetFrame() );
  this->VolumeSelector->Create();
  this->VolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->VolumeSelector->UpdateMenu();

  this->VolumeSelector->SetBorderWidth(2);
  this->VolumeSelector->SetLabelText( "Input Volume: ");
  this->VolumeSelector->SetBalloonHelpString("select an input volume from the current mrml scene.");
  app->Script("pack %s -side left -anchor e -padx 2 -pady 4", 
                this->VolumeSelector->GetWidgetName());

  volumeSelFrame->Delete();

  // Wizard collapsible frame with individual steps inside
  vtkSlicerModuleCollapsibleFrame *wizardFrame = vtkSlicerModuleCollapsibleFrame::New();
  wizardFrame->SetParent(modulePage);
  wizardFrame->Create();
  wizardFrame->SetLabelText("Wizard");
  wizardFrame->ExpandFrame();

  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
              wizardFrame->GetWidgetName(), 
              modulePage->GetWidgetName());

  // individual page/collapsible frame with their own widgets inside:
  this->WizardWidget->SetParent(wizardFrame->GetFrame());
  this->WizardWidget->Create();
  this->WizardWidget->GetSubTitleLabel()->SetHeight(1);
  this->WizardWidget->SetClientAreaMinimumHeight(320);
  //this->WizardWidget->SetButtonsPositionToTop();
  this->WizardWidget->HelpButtonVisibilityOn();
  app->Script("pack %s -side top -anchor nw -fill both -expand y",
              this->WizardWidget->GetWidgetName());
  wizardFrame->Delete();

  vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();
  vtkNotUsed(vtkKWWizardWidget *wizard_widget = this->WizardWidget;);

  // -----------------------------------------------------------------
  // Calibration step

  if (!this->CalibrateStep)
    {
    this->CalibrateStep = vtkPerkStationCalibrateStep::New();
    this->CalibrateStep->SetGUI(this);
    }
  wizard_workflow->AddStep(this->CalibrateStep);

  
  // -----------------------------------------------------------------
  // Plan step

  if (!this->PlanStep)
    {
    this->PlanStep = vtkPerkStationPlanStep::New();
    this->PlanStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->PlanStep);

  // -----------------------------------------------------------------
  // Insert step

  if (!this->InsertStep)
    {
    this->InsertStep = vtkPerkStationInsertStep::New();
    this->InsertStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->InsertStep);

  // -----------------------------------------------------------------
  // Validate step

  if (!this->ValidateStep)
    {
    this->ValidateStep = vtkPerkStationValidateStep::New();
    this->ValidateStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->ValidateStep);

  // -----------------------------------------------------------------
  // Evaluate step

  if (!this->EvaluateStep)
    {
    this->EvaluateStep = vtkPerkStationEvaluateStep::New();
    this->EvaluateStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->EvaluateStep);

  // -----------------------------------------------------------------
  // Initial and finish step

  wizard_workflow->SetFinishStep(this->EvaluateStep);
  wizard_workflow->CreateGoToTransitionsToFinishStep();
  wizard_workflow->SetInitialStep(this->CalibrateStep);
  
 
  
}


//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::TearDownGUI() 
{
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

  if (this->EvaluateStep)
    {
    this->EvaluateStep->SetGUI(NULL);
    }
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
