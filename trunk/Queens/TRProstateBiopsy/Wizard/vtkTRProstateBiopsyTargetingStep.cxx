#include "vtkTRProstateBiopsyTargetingStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"

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
vtkStandardNewMacro(vtkTRProstateBiopsyTargetingStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyTargetingStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsyTargetingStep::vtkTRProstateBiopsyTargetingStep()
{
  this->SetName("2/3. Targeting");
  this->SetDescription("Select targets and prepare for biopsy.");

  // load calibration volume dicom series
  this->LoadVolumeDialogFrame  = NULL;
  this->LoadTargetingVolumeButton = NULL;

  // needle type list
  this->NeedleTypeMenuList = NULL;
  
  // RAS coordinates for manually entry of target
  this->RASManualEntryFrame = NULL; 
  this->RASManualEntryLabel = NULL;
  this->RASManualEntry = NULL; 
  this->RASManualEntryButton = NULL;

  // multi-column list to display target, params, etc
  // this needle will display targets corresponding to the needle type selected
  this->ListFrame = NULL;
  this->TargetsMultiColumnList = NULL;

  // delete button
  this->MsgAndDelFrame = NULL;
  this->Message = NULL;
  this->DeleteButton = NULL;

  this->ProcessingCallback = false;
}

//----------------------------------------------------------------------------
vtkTRProstateBiopsyTargetingStep::~vtkTRProstateBiopsyTargetingStep()
{
  if (this->LoadVolumeDialogFrame)
    {
    this->LoadVolumeDialogFrame->Delete();
    this->LoadVolumeDialogFrame = NULL;
    }
  if (this->LoadTargetingVolumeButton)
    {
    this->LoadTargetingVolumeButton->Delete();
    this->LoadTargetingVolumeButton = NULL;
    }
  if (this->RASManualEntryFrame)
    {
    this->RASManualEntryFrame->Delete();
    this->RASManualEntryFrame = NULL;
    }
  if (this->RASManualEntryLabel)
    {
    this->RASManualEntryLabel->Delete();
    this->RASManualEntryLabel = NULL;
    }
  if (this->RASManualEntry)
    {
    this->RASManualEntry->Delete();
    this->RASManualEntry = NULL;
    }
  if (this->RASManualEntryButton)
    {
    this->RASManualEntryButton->Delete();
    this->RASManualEntryButton = NULL;
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
  if (this->MsgAndDelFrame)
    {
    this->MsgAndDelFrame->Delete();
    this->MsgAndDelFrame = NULL;
    }
  if (this->Message)
    {
    this->Message->Delete();
    this->Message = NULL;
    }
  if (this->DeleteButton)
    {
    this->DeleteButton->Delete();
    this->DeleteButton = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ShowUserInterface()
{
  // clear page, as HideUserInterface of previous step is not getting called, a bug
  // until that bug is resolved, we will manually call ClearPage
  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }

  this->Superclass::ShowUserInterface();

  this->ShowLoadVolumeControls();
  
  this->ShowManualAddTargetControls();

  this->ShowNeedleTypeAndTargetsListControls();

  this->ShowMessageAndDeleteControls();
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ShowLoadVolumeControls()
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

 
 if (!this->LoadTargetingVolumeButton)
    {
     this->LoadTargetingVolumeButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LoadTargetingVolumeButton->IsCreated())
    {
    this->LoadTargetingVolumeButton->SetParent(this->LoadVolumeDialogFrame);
    this->LoadTargetingVolumeButton->Create();
    this->LoadTargetingVolumeButton->SetBorderWidth(2);
    this->LoadTargetingVolumeButton->SetReliefToRaised();       
    this->LoadTargetingVolumeButton->SetHighlightThickness(2);
    this->LoadTargetingVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadTargetingVolumeButton->SetActiveBackgroundColor(1,1,1);        
    this->LoadTargetingVolumeButton->SetText( "Browse for Targeting Volume DICOM Series");
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->SetFileTypes("{ {DICOM Files} {*} }");
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadTargetingVolumeButton->TrimPathFromFileNameOff();    
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode        
    }
  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
               this->LoadTargetingVolumeButton->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ClearLoadVolumeControls()
{
  if (this->LoadVolumeDialogFrame)
    {
    this->Script("pack forget %s", 
                    this->LoadVolumeDialogFrame->GetWidgetName());
    }
  if (this->LoadTargetingVolumeButton)
    {
    this->Script("pack forget %s", 
                    this->LoadTargetingVolumeButton->GetWidgetName());
    }
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ShowManualAddTargetControls()
{
  // first clear the components
  this->ClearManualAddTargetControls();
  
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->RASManualEntryFrame)
    {
    this->RASManualEntryFrame = vtkKWFrame::New();
    }
  if (!this->RASManualEntryFrame->IsCreated())
    {
    this->RASManualEntryFrame->SetParent(parent);
    this->RASManualEntryFrame->Create();
    }
    
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->RASManualEntryFrame->GetWidgetName());

  // label
  if (!this->RASManualEntryLabel)
    { 
    this->RASManualEntryLabel = vtkKWLabel::New();
    }
  if (!this->RASManualEntryLabel->IsCreated())
    {
    this->RASManualEntryLabel->SetParent(this->RASManualEntryFrame);
    this->RASManualEntryLabel->Create();
    this->RASManualEntryLabel->SetText("RAS:     ");
    this->RASManualEntryLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }  
    
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->RASManualEntryLabel->GetWidgetName());
    
  if (!this->RASManualEntry)
    {
    this->RASManualEntry =  vtkKWEntrySet::New();  
    }
  if (!this->RASManualEntry->IsCreated())
    {
    this->RASManualEntry->SetParent(this->RASManualEntryFrame);
    this->RASManualEntry->Create();
    this->RASManualEntry->SetBorderWidth(2);
    this->RASManualEntry->SetReliefToGroove();
    this->RASManualEntry->SetPackHorizontally(1);   
    this->RASManualEntry->SetMaximumNumberOfWidgetsInPackingDirection(3);
    
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->RASManualEntry->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);     
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->RASManualEntry->GetWidgetName());

  // create the push button
  if(!this->RASManualEntryButton)
    {
    this->RASManualEntryButton = vtkKWPushButton::New();
    }
  if(!this->RASManualEntryButton->IsCreated())
    {
    this->RASManualEntryButton->SetParent(this->RASManualEntryFrame);
    this->RASManualEntryButton->SetText("Add");
    this->RASManualEntryButton->SetBalloonHelpString("Click to add a target at RAS location mentioned in left entries");
    this->RASManualEntryButton->Create();
    }
    
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                    this->RASManualEntryButton->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ClearManualAddTargetControls()
{
  if (this->RASManualEntryFrame)
    {
    this->Script("pack forget %s", 
                    this->RASManualEntryFrame->GetWidgetName());
    }
  if (this->RASManualEntryFrame)
    {
    this->Script("pack forget %s", 
                    this->RASManualEntryFrame->GetWidgetName());
    }
  if (this->RASManualEntry)
    {
    this->Script("pack forget %s", 
                    this->RASManualEntry->GetWidgetName());
    }
  if (this->RASManualEntryButton)
    {
    this->Script("pack forget %s", 
                    this->RASManualEntryButton->GetWidgetName());
    }
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ShowNeedleTypeAndTargetsListControls()
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
    this->TargetsMultiColumnList->GetWidget()->AddColumn("Rotation");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("Needle angle");
    this->TargetsMultiColumnList->GetWidget()->AddColumn("Depth");
    
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
    this->TargetsMultiColumnList->GetWidget()->SetColumnWidth(this->NeedleAngleColumn, 12);
    }

  this->Script ( "pack %s -fill both -expand true", this->TargetsMultiColumnList->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ClearNeedleTypeAndTargetsListControls()
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
void vtkTRProstateBiopsyTargetingStep::ShowMessageAndDeleteControls()
{
  // first clear the components
  this->ClearMessageAndDeleteControls();
  
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  // Create the frame
  if (!this->MsgAndDelFrame)
    {
    this->MsgAndDelFrame = vtkKWFrame::New();
    }
  if (!this->MsgAndDelFrame->IsCreated())
    {
    this->MsgAndDelFrame->SetParent(parent);
    this->MsgAndDelFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->MsgAndDelFrame->GetWidgetName());
  // create the push button
  if(!this->DeleteButton)
    {
    this->DeleteButton = vtkKWPushButton::New();
    }
  if(!this->DeleteButton->IsCreated())
    {
    this->DeleteButton->SetParent(this->MsgAndDelFrame);
    this->DeleteButton->SetText("Delete");
    this->DeleteButton->SetBalloonHelpString("Delete selected target");
    this->DeleteButton->Create();
    }
    
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                    this->DeleteButton->GetWidgetName());

  if(!this->Message)
    {
    this->Message = vtkKWText::New();
    }
  if(!this->Message->IsCreated())
    {
    this->Message->SetParent(this->MsgAndDelFrame);
    this->Message->Create();
    this->Message->SetText("Select needle type, then click on image to add a target");      
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
void vtkTRProstateBiopsyTargetingStep::ClearMessageAndDeleteControls()
{
  if (this->MsgAndDelFrame)
    {
    this->Script("pack forget %s", 
                    this->MsgAndDelFrame->GetWidgetName());
    }
  if (this->Message)
    {
    this->Script("pack forget %s", 
                    this->Message->GetWidgetName());
    }
  if (this->DeleteButton)
    {
    this->Script("pack forget %s", 
                    this->DeleteButton->GetWidgetName());
    }
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ProcessGUIEvents(vtkObject *caller,
                                           unsigned long event, void *callData)
{
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
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
void vtkTRProstateBiopsyTargetingStep::RecordClick(double rasPoint[])
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  char *msg = new char[40];
  
  sprintf(msg, "Clicked 1st marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::Reset()
{
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::AddGUIObservers()
{
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::RemoveGUIObservers()
{
}
