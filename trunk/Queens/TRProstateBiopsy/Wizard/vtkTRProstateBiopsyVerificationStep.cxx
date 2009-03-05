#include "vtkTRProstateBiopsyVerificationStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"

#include "vtkKWFrame.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkKWFrame.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWFrame.h"
#include "vtkKWEntry.h"
#include "vtkKWLabel.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWText.h"
#include "vtkKWPushButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyVerificationStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyVerificationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsyVerificationStep::vtkTRProstateBiopsyVerificationStep()
{
  this->SetName("4/4. Verification");
  this->SetDescription("Select targets and prepare for biopsy.");

   // load calibration volume dicom series
  this->LoadVolumeDialogFrame  = NULL;
  this->LoadVerificationVolumeButton = NULL;

  // needle type list
  this->NeedleTypeMenuList = NULL;

  // multi-column list to display target, params, etc
  // this needle will display targets corresponding to the needle type selected
  this->ListFrame = NULL;
  this->TargetsMultiColumnList = NULL;

  // delete button
  this->MsgAndErrorFrame = NULL;
  this->Message = NULL;
  this->ComputeErrorButton = NULL;

  this->ProcessingCallback = false;
  this->ClickNumber = 0;
  
}

//----------------------------------------------------------------------------
vtkTRProstateBiopsyVerificationStep::~vtkTRProstateBiopsyVerificationStep()
{
  if (this->LoadVolumeDialogFrame)
    {
    this->LoadVolumeDialogFrame->Delete();
    this->LoadVolumeDialogFrame = NULL;
    }
  if (this->LoadVerificationVolumeButton)
    {
    this->LoadVerificationVolumeButton->Delete();
    this->LoadVerificationVolumeButton = NULL;
    }
  if (this->ListFrame)
    {
    this->ListFrame->Delete();
    this->ListFrame = NULL;
    }
  if (this->TargetsMultiColumnList) 
    {
    this->TargetsMultiColumnList->Delete();
    this->TargetsMultiColumnList = NULL;
    }
  if (this->MsgAndErrorFrame)
    {
    this->MsgAndErrorFrame->Delete();
    this->MsgAndErrorFrame = NULL;
    }
  if (this->Message)
    {
    this->Message->Delete();
    this->Message = NULL;
    }
  if (this->ComputeErrorButton)
    {
    this->ComputeErrorButton->Delete();
    this->ComputeErrorButton = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ShowUserInterface()
{
 // clear page, as HideUserInterface of previous step is not getting called, a bug
  // until that bug is resolved, we will manually call ClearPage
  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }

  this->Superclass::ShowUserInterface();

  this->ShowLoadVolumeControls();

  this->ShowNeedleTypeAndTargetsListControls();

  this->ShowMessageAndErrorControls();
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ShowLoadVolumeControls()
{
  
  // first clear the components
  this->ClearLoadVolumeControls();

  
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();


  // Create frame
  if (!this->LoadVolumeDialogFrame)
    {
    this->LoadVolumeDialogFrame = vtkKWFrame::New();
    }
  if (!this->LoadVolumeDialogFrame->IsCreated())
    {
    this->LoadVolumeDialogFrame->SetParent(parent);
    this->LoadVolumeDialogFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->LoadVolumeDialogFrame->GetWidgetName());

 
 if (!this->LoadVerificationVolumeButton)
    {
     this->LoadVerificationVolumeButton = vtkKWLoadSaveButton::New();
    }
 if (!this->LoadVerificationVolumeButton->IsCreated())
    {
    this->LoadVerificationVolumeButton->SetParent(this->LoadVolumeDialogFrame);
    this->LoadVerificationVolumeButton->Create();
    this->LoadVerificationVolumeButton->SetBorderWidth(2);
    this->LoadVerificationVolumeButton->SetReliefToRaised();       
    this->LoadVerificationVolumeButton->SetHighlightThickness(2);
    this->LoadVerificationVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadVerificationVolumeButton->SetActiveBackgroundColor(1,1,1);        
    this->LoadVerificationVolumeButton->SetText( "Browse for Verification Volume DICOM Series");
    this->LoadVerificationVolumeButton->GetLoadSaveDialog()->SetFileTypes("{ {DICOM Files} {*} }");
    this->LoadVerificationVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadVerificationVolumeButton->TrimPathFromFileNameOff();    
    this->LoadVerificationVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode        
    }
  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
               this->LoadVerificationVolumeButton->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ClearLoadVolumeControls()
{
  if (this->LoadVolumeDialogFrame)
    {
    this->Script("pack forget %s", 
                    this->LoadVolumeDialogFrame->GetWidgetName());
    }
  if (this->LoadVerificationVolumeButton)
    {
    this->Script("pack forget %s", 
                    this->LoadVerificationVolumeButton->GetWidgetName());
    }
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ShowNeedleTypeAndTargetsListControls()
{
  // first clear the components
  this->ClearNeedleTypeAndTargetsListControls();
  
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->ListFrame)
    {
    this->ListFrame = vtkKWFrame::New();
    }
  if (!this->ListFrame->IsCreated())
    {
    this->ListFrame->SetParent(parent);
    this->ListFrame->Create();
    }
    
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->ListFrame->GetWidgetName());

  // add combo box in the frame
  if (!this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList = vtkKWMenuButtonWithLabel::New();
    }
  if (!this->NeedleTypeMenuList->IsCreated())
    {
    this->NeedleTypeMenuList->SetParent(this->ListFrame);
    this->NeedleTypeMenuList->Create();
    this->NeedleTypeMenuList->SetLabelText("Needle type");
    this->NeedleTypeMenuList->SetBalloonHelpString("Select the needle type");
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddRadioButton("BIOPSY");
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddRadioButton("SEED");
    this->NeedleTypeMenuList->GetWidget()->SetValue("BIOPSY");
    }
  this->Script("pack %s -side top -fill x -anchor nw -expand n -padx 4 -pady 4", 
                this->NeedleTypeMenuList->GetWidgetName());


  if (!this->TargetsMultiColumnList)
    {
    this->TargetsMultiColumnList = vtkKWMultiColumnListWithScrollbars::New();
    }
  if (!this->TargetsMultiColumnList->IsCreated())
    {
    this->TargetsMultiColumnList->SetParent ( this->ListFrame );
    this->TargetsMultiColumnList->Create ( );
    this->TargetsMultiColumnList->SetHeight(4);
    this->TargetsMultiColumnList->GetWidget()->SetSelectionTypeToCell();
    this->TargetsMultiColumnList->GetWidget()->MovableRowsOff();
    this->TargetsMultiColumnList->GetWidget()->MovableColumnsOff();
    
    // set up the columns of data for each point
    // refer to the header file for order
    this->TargetsMultiColumnList->GetWidget()->AddColumn("No.");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("Needle type");    
    this->TargetsMultiColumnList->GetWidget()->AddColumn("RAS location");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("Reachable");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("Overall Error");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("LR Error");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("AP Error");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("IS Error");

    // now set the attributes that are equal across the columns
    int col;
    for (col = 0; col < this->NumberOfColumns; col++)
      {        
      this->TargetsMultiColumnList->GetWidget()->SetColumnWidth(col, 10);
      this->TargetsMultiColumnList->GetWidget()->SetColumnAlignmentToLeft(col);
      this->TargetsMultiColumnList->GetWidget()->ColumnEditableOff(col);
      }

    // set some column widths to custom values
    this->TargetsMultiColumnList->GetWidget()->SetColumnWidth(this->TargetNumberColumn, 6);
    this->TargetsMultiColumnList->GetWidget()->SetColumnWidth(this->NeedleTypeColumn, 12);
    this->TargetsMultiColumnList->GetWidget()->SetColumnWidth(this->RASLocationColumn, 12);
    }

  this->Script ( "pack %s -fill both -expand true", this->TargetsMultiColumnList->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ClearNeedleTypeAndTargetsListControls()
{
  if (this->ListFrame)
    {
    this->Script("pack forget %s", 
                    this->ListFrame->GetWidgetName());
    }
    if (this->NeedleTypeMenuList)
    {
    this->Script("pack forget %s", 
                    this->NeedleTypeMenuList->GetWidgetName());
    }
  if (this->TargetsMultiColumnList)
    {
    this->Script("pack forget %s", 
                    this->TargetsMultiColumnList->GetWidgetName());
    }
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ShowMessageAndErrorControls()
{
  // first clear the components
  this->ClearMessageAndErrorControls();
  
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  // Create the frame
  if (!this->MsgAndErrorFrame)
    {
    this->MsgAndErrorFrame = vtkKWFrame::New();
    }
  if (!this->MsgAndErrorFrame->IsCreated())
    {
    this->MsgAndErrorFrame->SetParent(parent);
    this->MsgAndErrorFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->MsgAndErrorFrame->GetWidgetName());

   // create the push button
  if(!this->ComputeErrorButton)
    {
    this->ComputeErrorButton = vtkKWPushButton::New();
    }
  if(!this->ComputeErrorButton->IsCreated())
    {
    this->ComputeErrorButton->SetParent(this->MsgAndErrorFrame);
    this->ComputeErrorButton->SetText("CopmuteError");
    this->ComputeErrorButton->SetBalloonHelpString("Compute error");
    this->ComputeErrorButton->Create();
    }
    
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                    this->ComputeErrorButton->GetWidgetName());

  if(!this->Message)
    {
    this->Message = vtkKWText::New();
    }
  if(!this->Message->IsCreated())
    {
    this->Message->SetParent(this->MsgAndErrorFrame);
    this->Message->Create();
    this->Message->SetText("Select the target from list, then click on image to indicate needle position");      
    this->Message->SetBackgroundColor(0.7, 0.7, 0.95);
    this->Message->SetHeight(6);
    this->Message->SetWrapToWord();
    this->Message->ReadOnlyOn();
    this->Message->SetBorderWidth(2);
    this->Message->SetReliefToGroove();
    }
  this->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 6", 
                this->Message->GetWidgetName());

 
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ClearMessageAndErrorControls()
{
  if (this->MsgAndErrorFrame)
    {
    this->Script("pack forget %s", 
                    this->MsgAndErrorFrame->GetWidgetName());
    }
  if (this->Message)
    {
    this->Script("pack forget %s", 
                    this->Message->GetWidgetName());
    }
  if (this->ComputeErrorButton)
    {
    this->Script("pack forget %s", 
                    this->ComputeErrorButton->GetWidgetName());
    }
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ProcessGUIEvents(vtkObject *caller,
                                           unsigned long event, void *callData)
{
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
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
  //vtkSlicerInteractorStyle *istyleSecondary = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  // listen to click only when they come from secondary monitor's window
  if ((s == istyle0)&& (event == vtkCommand::LeftButtonPressEvent) )
    {
    // hear clicks only if the current sub state is Translate or Rotate with one exception // to be able to specify COR from slicer's laptop window, only in clinical mode
     // if ( (!((this->CurrentSubState == 2) || (this->CurrentSubState == 3)))  && !( (s == istyle0) && (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical) && (this->CurrentSubState == 1) && (this->CORSpecified == false)))
      //if (!((this->CurrentSubState == 2) || (this->CurrentSubState == 3)))
      //  return;


    if (this->ProcessingCallback)
    {
    return;
    }

    this->ProcessingCallback = true;

    ++this->ClickNumber;


    vtkRenderWindowInteractor *rwi;
    vtkMatrix4x4 *matrix;    
    
    // Note: at the moment, in calibrate step, listening only to clicks done in secondary monitor
    // because looking through secondary monitor mirror only can do calibration

    if ( (s == istyle0) )
      {
      // coming from main gui viewer of SLICER
      // to be able to specify COR from slicer's laptop window, only in clinical mode
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();    
      }
    /*else if ( s == istyleSecondary)
      {
      // coming from click on secondary monitor
      rwi = this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor();
      matrix = this->GetGUI()->GetSecondaryMonitor()->GetXYToRAS();  
      //matrix1 = this->GetGUI()->GetSecondaryMonitor()->GetXYToIJK();
      }*/

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
void vtkTRProstateBiopsyVerificationStep::RecordClick(double rasPoint[])
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  char *msg = new char[40];
  
  
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::Reset()
{
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::AddGUIObservers()
{
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::RemoveGUIObservers()
{
}
