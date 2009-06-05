#include "vtkTRProstateBiopsySegmentationStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkKWLabel.h"
#include "vtkKWPushButton.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWMessageDialog.h"

#include <vtksys/ios/sstream>
#include "itkImage.h"
#include "itkCastImageFilter.h"

#include "../Segmentation/randomWalkSeg.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsySegmentationStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsySegmentationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsySegmentationStep::vtkTRProstateBiopsySegmentationStep()
{
  this->SetName("2/4. Segmentation");
  this->SetDescription("Perform prostate segmentation.");

  this->WizardGUICallbackCommand->SetCallback(&vtkTRProstateBiopsySegmentationStep::WizardGUICallback);

  // input volume controls
  this->InputVolumeDialogFrame = NULL;
  this->InputVolumeButton = NULL;
  this->InputVolumeSelector = NULL;

  // seed volume controls
  this->SeedVolumeDialogFrame = NULL;
  this->SeedVolumeButton = NULL;
  this->SeedVolumeSelector = NULL;

  // output volume controls
  this->OutputVolumeDialogFrame = NULL;
  this->OutputVolumeSelector = NULL;

  // parameters frame
  this->ParametersFrame = NULL;
  this->ParameterBetaScale = NULL;
 
  // segment button frame
  this->SegmentFrame = NULL;
  this->SegmentButton = NULL;
 
 
  this->ProcessingCallback = false;
  this->ObserverCount = 0;
  this->ClickNumber = 0;

  this->InputVolFileName = new char[256];
  this->SeedVolFileName = new char[256];
  this->OutputVolFileName = new char[256];
 

}

//----------------------------------------------------------------------------
vtkTRProstateBiopsySegmentationStep::~vtkTRProstateBiopsySegmentationStep()
{
  // input volume controls
  if (this->InputVolumeDialogFrame)
    {
    this->InputVolumeDialogFrame->Delete();
    this->InputVolumeDialogFrame = NULL;
    }
  if (this->InputVolumeButton)
    {
    this->InputVolumeButton->Delete();
    this->InputVolumeButton = NULL;
    }
  if ( this->InputVolumeSelector ) 
    {
    this->InputVolumeSelector->SetParent(NULL);
    this->InputVolumeSelector->Delete();
    this->InputVolumeSelector = NULL;
    }
 
  // seed volume controls
  if (this->SeedVolumeDialogFrame)
    {
    this->SeedVolumeDialogFrame->Delete();
    this->SeedVolumeDialogFrame = NULL;
    }
  if (this->SeedVolumeButton)
    {
    this->SeedVolumeButton->Delete();
    this->SeedVolumeButton = NULL;
    }
  if ( this->SeedVolumeSelector ) 
    {
    this->SeedVolumeSelector->SetParent(NULL);
    this->SeedVolumeSelector->Delete();
    this->SeedVolumeSelector = NULL;
    }

 // output volume controls
  if (this->OutputVolumeDialogFrame)
    {
    this->OutputVolumeDialogFrame->Delete();
    this->OutputVolumeDialogFrame = NULL;
    }
  if ( this->OutputVolumeSelector ) 
    {
    this->OutputVolumeSelector->SetParent(NULL);
    this->OutputVolumeSelector->Delete();
    this->OutputVolumeSelector = NULL;
    }
  if (this->ParametersFrame)
    {
    this->ParametersFrame->Delete();
    this->ParametersFrame = NULL;
    }
  if (this->ParameterBetaScale)
    {
    this->ParameterBetaScale->Delete();
    this->ParameterBetaScale = NULL;
    }
  if (this->SegmentFrame)
    {
    this->SegmentFrame->Delete();
    this->SegmentFrame = NULL;
    }
  if (this->SegmentButton)
    {
    this->SegmentButton->Delete();
    this->SegmentButton = NULL;
    }

  
  if (this->InputVolFileName)
    {
    delete this->InputVolFileName;
    this->InputVolFileName = NULL;
    }
  if (this->SeedVolFileName)
    {
    delete this->SeedVolFileName;
    this->SeedVolFileName = NULL;
    }
  if (this->OutputVolFileName)
    {
    delete this->OutputVolFileName;
    this->OutputVolFileName = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ShowVolumeControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetGUI()->GetApplication();

  // first clear the components
  this->ClearVolumeControls();
  
  if (!this->InputVolumeDialogFrame)
    {
    this->InputVolumeDialogFrame = vtkKWFrame::New();
    }
  if (!this->InputVolumeDialogFrame->IsCreated())
    {
    this->InputVolumeDialogFrame->SetParent(parent);
    this->InputVolumeDialogFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->InputVolumeDialogFrame->GetWidgetName());
  
  if (!this->InputVolumeButton)
    {
     this->InputVolumeButton = vtkKWLoadSaveButton::New();
    }
  if (!this->InputVolumeButton->IsCreated())
    {
    this->InputVolumeButton->SetParent(this->InputVolumeDialogFrame);
    this->InputVolumeButton->Create();
    this->InputVolumeButton->SetBorderWidth(2);
    this->InputVolumeButton->SetReliefToRaised();       
    this->InputVolumeButton->SetHighlightThickness(2);
    this->InputVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->InputVolumeButton->SetActiveBackgroundColor(1,1,1);        
    this->InputVolumeButton->SetText( "Browse for Input Volume");
    this->InputVolumeButton->GetLoadSaveDialog()->SetFileTypes("{ {Nrrd Files} {*} }");
    this->InputVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->InputVolumeButton->TrimPathFromFileNameOff();    
    this->InputVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode    
    //this->LoadCalibrationVolumeButton->GetWidget()->AddObserver(vtkKWPushButton::InvokedEvent, this->WizardGUICallbackCommand);
    }
  this->Script("pack %s -side left -anchor w -padx 2 -pady 2", 
               this->InputVolumeButton->GetWidgetName());


  //input volume selector
  if (!this->InputVolumeSelector)
    {
    this->InputVolumeSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if (!this->InputVolumeSelector->IsCreated())
    {
    this->InputVolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
    this->InputVolumeSelector->SetParent( this->InputVolumeDialogFrame );
    this->InputVolumeSelector->Create();
    this->InputVolumeSelector->SetMRMLScene(this->GetGUI()->GetLogic()->GetMRMLScene());
    this->InputVolumeSelector->UpdateMenu();
    this->InputVolumeSelector->SetBorderWidth(2);
    this->InputVolumeSelector->SetLabelText( "Input Volume: ");
    this->InputVolumeSelector->SetBalloonHelpString("Select an input volume from the current mrml scene.");
    }
  app->Script("pack %s -side top -anchor e -padx 2 -pady 4", 
                this->InputVolumeSelector->GetWidgetName());

  if (!this->SeedVolumeDialogFrame)
    {
    this->SeedVolumeDialogFrame = vtkKWFrame::New();
    }
  if (!this->SeedVolumeDialogFrame->IsCreated())
    {
    this->SeedVolumeDialogFrame->SetParent(parent);
    this->SeedVolumeDialogFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->SeedVolumeDialogFrame->GetWidgetName());
  
  if (!this->SeedVolumeButton)
    {
     this->SeedVolumeButton = vtkKWLoadSaveButton::New();
    }
  if (!this->SeedVolumeButton->IsCreated())
    {
    this->SeedVolumeButton->SetParent(this->SeedVolumeDialogFrame);
    this->SeedVolumeButton->Create();
    this->SeedVolumeButton->SetBorderWidth(2);
    this->SeedVolumeButton->SetReliefToRaised();       
    this->SeedVolumeButton->SetHighlightThickness(2);
    this->SeedVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->SeedVolumeButton->SetActiveBackgroundColor(1,1,1);        
    this->SeedVolumeButton->SetText( "Browse for Seed Volume");
    this->SeedVolumeButton->GetLoadSaveDialog()->SetFileTypes("{ {Nrrd Files} {*} }");
    this->SeedVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->SeedVolumeButton->TrimPathFromFileNameOff();    
    this->SeedVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode    
    }
  this->Script("pack %s -side left -anchor w -padx 2 -pady 2", 
               this->SeedVolumeButton->GetWidgetName());

  //seed volume selector
  if (!this->SeedVolumeSelector)
    {
    this->SeedVolumeSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if (!this->SeedVolumeSelector->IsCreated())
    {
    this->SeedVolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
    this->SeedVolumeSelector->SetParent( this->SeedVolumeDialogFrame );
    this->SeedVolumeSelector->Create();
    this->SeedVolumeSelector->SetMRMLScene(this->GetGUI()->GetLogic()->GetMRMLScene());
    this->SeedVolumeSelector->UpdateMenu();
    this->SeedVolumeSelector->SetBorderWidth(2);
    this->SeedVolumeSelector->SetLabelText( "Seed Volume: ");
    this->SeedVolumeSelector->SetBalloonHelpString("select a seed volume from the current mrml scene.");
    }
  app->Script("pack %s -side top -anchor e -padx 2 -pady 4", 
                this->SeedVolumeSelector->GetWidgetName());

  // output volume controls
  if (!this->OutputVolumeDialogFrame)
    {
    this->OutputVolumeDialogFrame = vtkKWFrame::New();
    }
  if (!this->OutputVolumeDialogFrame->IsCreated())
    {
    this->OutputVolumeDialogFrame->SetParent(parent);
    this->OutputVolumeDialogFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->OutputVolumeDialogFrame->GetWidgetName());

  //output volume selector
  if (!this->OutputVolumeSelector)
    {
    this->OutputVolumeSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if (!this->OutputVolumeSelector->IsCreated())
    {
    this->OutputVolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, "TRPBVolumeOut");
    this->OutputVolumeSelector->SetNewNodeEnabled(1);
    this->OutputVolumeSelector->SetParent( this->OutputVolumeDialogFrame );
    this->OutputVolumeSelector->Create();
    this->OutputVolumeSelector->SetMRMLScene(this->GetGUI()->GetLogic()->GetMRMLScene());
    this->OutputVolumeSelector->UpdateMenu();
    this->OutputVolumeSelector->SetBorderWidth(2);
    this->OutputVolumeSelector->SetLabelText( "Output Volume: ");
    this->OutputVolumeSelector->SetBalloonHelpString("select a output volume");
    }
  app->Script("pack %s -side top -anchor nw -padx 2 -pady 4", 
                this->OutputVolumeSelector->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ClearVolumeControls()
{
  if (this->InputVolumeDialogFrame)
    {
    this->Script("pack forget %s", 
                    this->InputVolumeDialogFrame->GetWidgetName());
    }
  if (this->InputVolumeButton)
    {
    this->Script("pack forget %s", 
                    this->InputVolumeButton->GetWidgetName());
    }
  if (this->InputVolumeSelector)
    {
    this->Script("pack forget %s", 
                    this->InputVolumeSelector->GetWidgetName());
    }
  if (this->SeedVolumeDialogFrame)
    {
    this->Script("pack forget %s", 
                    this->SeedVolumeDialogFrame->GetWidgetName());
    }
  if (this->SeedVolumeButton)
    {
    this->Script("pack forget %s", 
                    this->SeedVolumeButton->GetWidgetName());
    }
  if (this->SeedVolumeSelector)
    {
    this->Script("pack forget %s", 
                    this->SeedVolumeSelector->GetWidgetName());
    }
  if (this->OutputVolumeDialogFrame)
    {
    this->Script("pack forget %s", 
                    this->OutputVolumeDialogFrame->GetWidgetName());
    }
  if (this->OutputVolumeSelector)
    {
    this->Script("pack forget %s", 
                    this->OutputVolumeSelector->GetWidgetName());
    }
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ShowParametersControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  this->ClearParametersControls();
  
  if (!this->ParametersFrame)
    {
    this->ParametersFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->ParametersFrame->IsCreated())
    {
    this->ParametersFrame->SetParent(parent);
    this->ParametersFrame->Create();
    this->ParametersFrame->SetLabelText("Parameters");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ParametersFrame->GetWidgetName());

 
  if (!this->ParameterBetaScale)
    {
    this->ParameterBetaScale = vtkKWScaleWithEntry::New();
    }
  if (!this->ParameterBetaScale->IsCreated())
    {
    this->ParameterBetaScale->SetParent(this->ParametersFrame->GetFrame());
    this->ParameterBetaScale->Create();
    this->ParameterBetaScale->SetLabelText("Beta");
    this->ParameterBetaScale->SetWidth(3);
    this->ParameterBetaScale->SetLength(200);
    this->ParameterBetaScale->SetRange(0,1);
    this->ParameterBetaScale->SetResolution(0.001);
    this->ParameterBetaScale->SetValue(0.001);
    }

  this->Script("pack %s -side top -anchor w -padx 2 -pady 2", 
               this->ParameterBetaScale->GetWidgetName() );
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ClearParametersControls()
{
  if (this->ParametersFrame)
    {
    this->Script("pack forget %s", 
                    this->ParametersFrame->GetWidgetName());
    }
  if (this->ParameterBetaScale)
    {
    this->Script("pack forget %s", 
                    this->ParameterBetaScale->GetWidgetName());
    }
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ShowSegmentControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  this->ClearSegmentControls();
  
  if (!this->SegmentFrame)
    {
    this->SegmentFrame = vtkKWFrame::New();
    }
  if (!this->SegmentFrame->IsCreated())
    {
    this->SegmentFrame->SetParent(parent);
    this->SegmentFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->SegmentFrame->GetWidgetName());
  
  // create the resegment button
  if (!this->SegmentButton)
    {
    this->SegmentButton = vtkKWPushButton::New();
    }
  if (!this->SegmentButton->IsCreated())
    {
    this->SegmentButton->SetParent(this->SegmentFrame);
    this->SegmentButton->Create();
    this->SegmentButton->SetText("Segment");
    this->SegmentButton->SetBorderWidth(2);
    this->SegmentButton->SetReliefToRaised();      
    this->SegmentButton->SetHighlightThickness(2);
    this->SegmentButton->SetBackgroundColor(0.85,0.85,0.85);
    this->SegmentButton->SetActiveBackgroundColor(1,1,1);      
    this->SegmentButton->SetBalloonHelpString("Click to start segmentation");
    }
    
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                    this->SegmentButton->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ClearSegmentControls()
{
  if (this->SegmentFrame)
    {
    this->Script("pack forget %s", 
                    this->SegmentFrame->GetWidgetName());
    }
  if (this->SegmentButton)
    {
    this->Script("pack forget %s", 
                    this->SegmentButton->GetWidgetName());
    }
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ShowUserInterface()
{
  //slicerCerr("vtkTRProstateBiopsySegmentationStep::ShowUserInterface");

  // clear page, as HideUserInterface of previous step is not getting called, a bug
  // until that bug is resolved, we will manually call ClearPage
  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }

  this->Superclass::ShowUserInterface();

  // show volume controls
  this->ShowVolumeControls();

  // show parameters controls
  this->ShowParametersControls();

  // show segmentation controls
  this->ShowSegmentControls();

  this->AddGUIObservers();

  // Activate additional details for entering this step
  //this->Enter();
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  // add observers 
  // 1) click on input volume button
  if (this->InputVolumeButton)
    {
    this->InputVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  // 2) click on seed volume button
  if (this->SeedVolumeButton)
    {
    this->SeedVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  // 3) click on segment
  if (this->SegmentButton)
    {
    this->SegmentButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  
  this->InputVolumeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
  
  this->SeedVolumeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->WizardGUICallbackCommand );

  this->OutputVolumeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
  
  this->ObserverCount++;

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::RemoveGUIObservers()
{
  // 1) click on input volume button
  if (this->InputVolumeButton)
    {
    this->InputVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  // 2) click on seed volume button
  if (this->SeedVolumeButton)
    {
    this->SeedVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  // 3) click on segment button
  if (this->SegmentButton)
    {
    this->SegmentButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

  
  this->InputVolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
  
  this->SeedVolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->WizardGUICallbackCommand );

  this->OutputVolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->WizardGUICallbackCommand );

  this->ObserverCount--;
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::Enter()
{
  //slicerCerr("vtkTRProstateBiopsySegmentationStep::Enter");
/*
  // change the view orientation to the calibration image orientation
  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(
    this->GetGUI()->GetApplication());
  vtkTRProstateBiopsyLogic *logic = this->GetGUI()->GetLogic();

  if (this->GetGUI()->GetLogic()->GetCalibrationSliceNodeXML())
    {
    static const char *panes[3] = { "Red", "Yellow", "Green" };

    vtkSlicerApplicationLogic *appLogic =
      this->GetGUI()->GetApplicationGUI()->GetApplicationLogic();

    const char *xml = this->GetGUI()->GetLogic()->GetCalibrationSliceNodeXML();

    //slicerCerr(xml << "\n");

    for (int i = 0; i < 3; i++)
      {
      //slicerCerr(i);

      vtkSlicerSliceLogic *slice = appLogic->GetSliceLogic(
        const_cast<char *>(panes[i]));

      vtkMRMLSliceNode *sliceNode = slice->GetSliceNode();

      sliceNode->ReadXMLAttributes(&xml);
      }
    }
*/

  // change the fiducials to the calibration fiducials

}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::Exit()
{
  //slicerCerr("vtkTRProstateBiopsySegmentationStep::Exit");

/*
  vtksys_ios::ostringstream xml;

  static const char *panes[3] = { "Red", "Yellow", "Green" };

  vtkSlicerApplicationLogic *appLogic =
    this->GetGUI()->GetApplicationGUI()->GetApplicationLogic();

  for (int i = 0; i < 3; i++)
    {
    vtkSlicerSliceLogic *slice = appLogic->GetSliceLogic(
      const_cast<char *>(panes[i]));

    vtkMRMLSliceNode *sliceNode = slice->GetSliceNode();

    sliceNode->WriteXML(xml, 0);
    }

  this->GetGUI()->GetLogic()->SetCalibrationSliceNodeXML(xml.str().c_str());
*/
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::WizardGUICallback( vtkObject *caller,
                         unsigned long eid, void *clientData, void *callData )
{

  vtkTRProstateBiopsySegmentationStep *self = reinterpret_cast<vtkTRProstateBiopsySegmentationStep *>(clientData);
  
  //if (self->GetInGUICallbackFlag())
    {
    // check for infinite recursion here
    }

  vtkDebugWithObjectMacro(self, "In vtkTRProstateCalibrationBiopsyStep GUICallback");
  
  //self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  //self->SetInGUICallbackFlag(0);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ProcessGUIEvents(vtkObject *caller,
        unsigned long event, void *callData)
{
  //slicerCerr("vtkTRProstateBiopsySegmentationStep::ProcessGUIEvents");

  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;
  
  // input volume dialog button
  if (this->InputVolumeButton && this->InputVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    this->InputVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("TRProstateOpenPathVol");          
    const char *fileName = this->InputVolumeButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->InputVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPathVol");
      // call the callback function
      this->InputVolumeButtonCallback(fileName);    
      } 
   
    }

   // seed volume dialog button
  if (this->SeedVolumeButton && this->SeedVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    this->SeedVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("TRProstateOpenPathVol");          
    const char *fileName = this->SeedVolumeButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->SeedVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPathVol");
      // call the callback function
      this->SeedVolumeButtonCallback(fileName);    
      } 
   
    }


  // segmet 
  if (this->SegmentButton && this->SegmentButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    this->SegmentYiRandomWalkMethod();
    }
  
  if (this->OutputVolumeSelector && this->OutputVolumeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  && this->OutputVolumeSelector->GetSelected() != NULL) 
    { 
    mrmlNode->SetSegmentationOutputVolumeRef(this->OutputVolumeSelector->GetSelected()->GetID());
    }
  
  if (this->InputVolumeSelector && this->InputVolumeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  && this->InputVolumeSelector->GetSelected() != NULL) 
    { 
    mrmlNode->SetSegmentationInputVolumeRef(this->InputVolumeSelector->GetSelected()->GetID());
    }
  if (this->SeedVolumeSelector && this->SeedVolumeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  && this->SeedVolumeSelector->GetSelected() != NULL) 
    { 
    mrmlNode->SetSegmentationSeedVolumeRef(this->SeedVolumeSelector->GetSelected()->GetID());
    }

}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::InputVolumeButtonCallback(const char *fileName)
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }
  
  strcpy(this->InputVolFileName, fileString.c_str());

  this->InputVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPath");

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetGUI()->GetApplication());

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddVolumeToScene(app,fileString.c_str());
        
  if (volumeNode)
    {
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();
    this->InputVolumeSelector->SetSelected(volumeNode);
    }
  else 
    {
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
    dialog->SetParent ( this->InputVolumeDialogFrame );
    dialog->SetStyleToMessage();
    std::string msg = std::string("Unable to read volume file ") + std::string(fileName);
    dialog->SetText(msg.c_str());
    dialog->Create ( );
    dialog->Invoke();
    dialog->Delete();
    }
     
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::SeedVolumeButtonCallback(const char *fileName)
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }
  strcpy(this->SeedVolFileName, fileString.c_str());

  this->SeedVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPath");

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetGUI()->GetApplication());

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddVolumeToScene(app,fileString.c_str());
        
  if (volumeNode)
    {
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();
    this->SeedVolumeSelector->SetSelected(volumeNode);
    }
  else 
    {
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
    dialog->SetParent ( this->SeedVolumeDialogFrame );
    dialog->SetStyleToMessage();
    std::string msg = std::string("Unable to read volume file ") + std::string(fileName);
    dialog->SetText(msg.c_str());
    dialog->Create ( );
    dialog->Invoke();
    dialog->Delete();
    }
     
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::Reset()
{

    this->ClickNumber = 0;
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::SegmentYiRandomWalkMethod()
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  // find all the volumes (input, seed, output) and convert them to itkImageData, so that they
  // can used by Yi's algorithm
  vtkMRMLScalarVolumeNode *inVolume =  vtkMRMLScalarVolumeNode::SafeDownCast(this->GetGUI()->GetMRMLScene()->GetNodeByID(mrmlNode->GetSegmentationInputVolumeRef()));
  if (inVolume == NULL)
    {
    vtkErrorMacro("No output volume found with id= " << mrmlNode->GetSegmentationInputVolumeRef());
    return;
    }

  // gather info about direction cosines and origin
  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  inVolume->GetIJKToRASMatrix(ijkToRAS);
  double origin[3];
  inVolume->GetOrigin(origin);
  origin[0] = -origin[0];
  origin[1] = -origin[1];

  // get vtkImageData
  vtkImageData *input = inVolume->GetImageData();

  // cast it to double data
  vtkImageCast* castInput = vtkImageCast::New();

  castInput->SetInput(input);
  castInput->SetOutputScalarTypeToDouble();
  castInput->Update();

  //dimensions
  int dims[3];
  input->GetDimensions(dims);
  // spacing
  double spacing[3];
  input->GetSpacing(spacing);

  // Wrap scalars into an ITK image
  // - mostly rely on defaults for spacing, origin etc for this filter
  typedef itk::Image<double,3> ImageType;
  
  // set memory import pointer
  ImageType::Pointer inImage = ImageType::New();
  inImage->GetPixelContainer()->SetImportPointer(static_cast<double *> (castInput->GetOutput()->GetScalarPointer()), dims[0]*dims[1]*dims[2], false);
  ImageType::RegionType region;
  ImageType::IndexType index;
  ImageType::SizeType size;
  // set region, index, size
  index[0] = index[1] = index[2] = 0;
  size[0] = dims[0];
  size[1] = dims[1];
  size[2] = dims[2];
  region.SetIndex(index);
  region.SetSize(size);
  inImage->SetLargestPossibleRegion(region);
  inImage->SetBufferedRegion(region);
  inImage->SetSpacing(spacing);

  // set origin
  inImage->SetOrigin(origin);
  // set direction cosines
  ImageType::DirectionType direction;
  direction(0,0) = ijkToRAS->GetElement(0,0);
  direction(0,1) = ijkToRAS->GetElement(0,1);
  direction(0,2) = ijkToRAS->GetElement(0,2);

  direction(1,0) = ijkToRAS->GetElement(1,0);
  direction(1,1) = ijkToRAS->GetElement(1,1);
  direction(1,2) = ijkToRAS->GetElement(1,2);

  direction(2,0) = ijkToRAS->GetElement(2,0);
  direction(2,1) = ijkToRAS->GetElement(2,1);
  direction(2,2) = ijkToRAS->GetElement(2,2);
  
  inImage->SetDirection(direction);
  

  vtkMRMLScalarVolumeNode *seedVolume =  vtkMRMLScalarVolumeNode::SafeDownCast(this->GetGUI()->GetMRMLScene()->GetNodeByID(mrmlNode->GetSegmentationSeedVolumeRef()));
  if (seedVolume == NULL)
    {
    vtkErrorMacro("No output volume found with id= " << mrmlNode->GetSegmentationSeedVolumeRef());
    return;
    }
  vtkImageData *seed = seedVolume->GetImageData();
  // cast it to double data
  vtkImageCast* seedCast = vtkImageCast::New();

  seedCast->SetInput(seed);
  seedCast->SetOutputScalarTypeToDouble();
  seedCast->Update();

  ImageType::Pointer seedImage = ImageType::New();
  seedImage->GetPixelContainer()->SetImportPointer(static_cast<double *> (seedCast->GetOutput()->GetScalarPointer()), dims[0]*dims[1]*dims[2], false);
  index[0] = index[1] = index[2] = 0;
  size[0] = dims[0];
  size[1] = dims[1];
  size[2] = dims[2];
  region.SetIndex(index);
  region.SetSize(size);
  seedImage->SetLargestPossibleRegion(region);
  seedImage->SetBufferedRegion(region);
  seedImage->SetSpacing(spacing);
  seedImage->SetOrigin(origin);
  seedImage->SetDirection(direction);

 
  randomWalkSeg rw;

  //  rw.setVol(volSmth);
  rw.setInput(inImage);
  rw.setSeedImage(seedImage);

  int numIter = 1000;
  rw.set_solver_num_iter(numIter);

  double solverErrTol = 1e-8;
  rw.set_solver_err_tol(solverErrTol);

  double rw_beta = 0.001;
  rw.setBeta(rw_beta);
  // std::cout<<"beta value is "<<rw_beta<<std::endl<<std::flush;

  std::cout<<"computing result...."<<std::flush;
  rw.update();
  std::cout<<"done\n"<<std::flush;

  std::cout<<"smoothing result potential volume...."<<std::flush;
  ImageType::Pointer output_image = rw.getOutputImage();

  output_image->SetSpacing(spacing);
  output_image->SetOrigin(origin);
  output_image->SetDirection(direction);

 
  typedef itk::Image<short,3> OutImageType;

  typedef itk::CastImageFilter<ImageType, OutImageType> CastFilterType;
  CastFilterType::Pointer caster = CastFilterType::New();

  caster->SetInput(output_image);
  caster->Update();

  
  typedef itk::ImageFileWriter<OutImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();

  writer->SetInput(caster->GetOutput());
  writer->SetFileName("SegResult.nrrd");

  try
  {
      writer->Update();
  }
  catch(itk::ExceptionObject &e)
  {
      std::cout<<"Error: "<< e << std::endl;
  }
  
  // find output volume
  vtkMRMLScalarVolumeNode *outVolume =  vtkMRMLScalarVolumeNode::SafeDownCast(this->GetGUI()->GetMRMLScene()->GetNodeByID(mrmlNode->GetSegmentationOutputVolumeRef()));
  if (outVolume == NULL)
    {
    vtkErrorMacro("No output volume found with id= " << mrmlNode->GetSegmentationOutputVolumeRef());
    return;
    }
  outVolume->CopyOrientation(inVolume);
  
  // convert from itkImage to vtkImageData, and put the vtkImageData in MRMLVolumeNode
  
  // set ouput of the filter to VolumeNode's ImageData
  vtkImageData* output = vtkImageData::New();

  output->SetExtent(input->GetExtent());
  output->SetDimensions(input->GetDimensions());
  output->SetNumberOfScalarComponents(input->GetNumberOfScalarComponents());
  output->SetScalarType(input->GetScalarType());
  output->AllocateScalars();

   // Copy to the output
  memcpy(output->GetScalarPointer(), caster->GetOutput()->GetBufferPointer(),
         caster->GetOutput()->GetBufferedRegion().GetNumberOfPixels() * sizeof(short));
  outVolume->SetAndObserveImageData(output);
  outVolume->SetLabelMap(1);
  output->Delete();
  outVolume->SetModifiedSinceRead(1);

  



}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  //  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Planning")!=0)
  //    return;

  
  // first identify if the step is pertinent, i.e. current step of wizard workflow is actually calibration step

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  if (!wizard_widget || wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this)
    {
    return;
    }

  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast(caller);
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle1 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle2 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  
  // listen to click
  if (( (s == istyle0)|| (s == istyle1) || (s == istyle2) )&& (event == vtkCommand::LeftButtonPressEvent))
    {  
    if (this->ProcessingCallback)
    {
    return;
    }

    this->ProcessingCallback = true;

    ++this->ClickNumber;

    vtkRenderWindowInteractor *rwi;
    vtkMatrix4x4 *matrix;        
    
    if (s == istyle0)
      {
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();     
      }
    else if (s == istyle1)
      {
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Yellow");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();     
      }
    else if (s == istyle2)
      {
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Green");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();     
      }
    

    // capture the point
    int point[2];
    rwi->GetLastEventPosition(point);
    double inPt[4] = {point[0], point[1], 0, 1};
    double outPt[4];    
    matrix->MultiplyPoint(inPt, outPt); 
    double ras[3] = {outPt[0], outPt[1], outPt[2]};
    this->RecordClick(ras);

    this->ProcessingCallback = false;
    }
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::RecordClick(double rasPoint[])
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::SaveToExperimentFile(ostream &of)
{
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySegmentationStep::LoadFromExperimentFile(istream &file)
{
}
