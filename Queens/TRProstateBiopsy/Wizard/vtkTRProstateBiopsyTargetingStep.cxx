#include "vtkTRProstateBiopsyTargetingStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkTRProstateBiopsyTargetDescriptor.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"

#include "vtkKWFrame.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWFrame.h"
#include "vtkKWEntry.h"
#include "vtkKWLabel.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWMessageDialog.h"
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
  this->SetName("3/4. Targeting");
  this->SetDescription("Select targets and prepare for biopsy.");

  this->WizardGUICallbackCommand->SetCallback(&vtkTRProstateBiopsyTargetingStep::WizardGUICallback);

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

  this->TargetDescriptorIndicesIndexedByListIndex.clear();

  this->ProcessingCallback = false;
  this->LastSelectedTargetDescriptorIndex = -1;
  this->CurrentSelectedTargetDescriptorIndex = -1;

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

  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  
  mrmlNode->GetCalibrationFiducialsListNode()->SetAllFiducialsVisibility(false);

  this->AddGUIObservers();
  
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
    for (int i = 0; i < this->GetGUI()->GetMRMLNode()->GetNumberOfNeedles(); i++)
      {
      std::string needle = this->GetGUI()->GetMRMLNode()->GetNeedleType(i);
      this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddRadioButton(needle.c_str());
      }
    this->NeedleTypeMenuList->GetWidget()->SetValue(this->GetGUI()->GetMRMLNode()->GetNeedleType(0).c_str());
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

    this->TargetsMultiColumnList->GetWidget()->SetSelectionTypeToRow();
    this->TargetsMultiColumnList->GetWidget()->SetSelectionModeToSingle();
    this->TargetsMultiColumnList->GetWidget()->SetSelectionBackgroundColor(0.0, 0.0, 1.0);
    this->TargetsMultiColumnList->GetWidget()->SetSelectionForegroundColor(1.0,1.0,1.0);
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
    this->Message->SetFont("times 12 bold");
    //this->Message->SetForegroundColor(0.0, 1.0, 0.0);
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
void vtkTRProstateBiopsyTargetingStep::WizardGUICallback( vtkObject *caller,
                         unsigned long eid, void *clientData, void *callData )
{

  vtkTRProstateBiopsyTargetingStep *self = reinterpret_cast<vtkTRProstateBiopsyTargetingStep *>(clientData);
  
  //if (self->GetInGUICallbackFlag())
    {
    // check for infinite recursion here
    }

  vtkDebugWithObjectMacro(self, "In vtkTRProstateBiopsyTargetingStep GUICallback");
  
  //self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  //self->SetInGUICallbackFlag(0);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ProcessGUIEvents(vtkObject *caller,
                                           unsigned long event, void *callData)
{
   vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  // load targeting volume dialog button
  if (this->LoadTargetingVolumeButton && this->LoadTargetingVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("TRProstateOpenPathVol");          
    const char *fileName = this->LoadTargetingVolumeButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->LoadTargetingVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPathVol");
      // call the callback function
      this->LoadTargetingVolumeButtonCallback(fileName);    
      } 
   
    }
  // targeting list selected
  if (this->TargetsMultiColumnList && this->TargetsMultiColumnList->GetWidget() == vtkKWMultiColumnList::SafeDownCast(caller) && (event == vtkKWMultiColumnList::SelectionChangedEvent))
    {
    this->TargetSelectedFromListCallback();
    }
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
  vtkSlicerInteractorStyle *istyle1 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle2 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());

  //vtkSlicerInteractorStyle *istyleSecondary = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  // listen to click only when they come from secondary monitor's window
  if (((s == istyle0)||(s == istyle1) ||(s == istyle2))&& (event == vtkCommand::LeftButtonPressEvent) )
    {


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
    else if ( (s == istyle1) )
      {
      // coming from main gui viewer of SLICER
      // to be able to specify COR from slicer's laptop window, only in clinical mode
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Green");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();    
      }
    else if ( (s == istyle2) )
      {
      // coming from main gui viewer of SLICER
      // to be able to specify COR from slicer's laptop window, only in clinical mode
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Yellow");
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
void vtkTRProstateBiopsyTargetingStep::RecordClick(double rasPoint[])
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  char *msg = new char[40];
  
  sprintf(msg, "Clicked marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);

  // get the active target list, from the list

  // Objective: Add a target
  // things to do:
  // 1) validate FoR
  // 2) calculate targeting parameters for active needle, store in a target descriptor
  // 3) store the target descriptors in a list
  // 4) add the target to targets FiducialListNode, searching which list to add to, knowing which needle list is being populated to?
  // 5) visual feedback of creation of the click
  // 6) populate the list on GUI

  // 1) validate FoR
  //if (!this->GetGUI()->GetLogic()->ValidateFoR())
    {
    // error message?
    //return;
    }
  // 2) calculate targeting parameters for active needle, store in a target descriptor
  // 3) store the target descriptors in a list
  // 4) add the target to targets FiducialListNode, searching which list to add to, knowing which needle list is being populated to?
  // 5) visual feedback of creation of the click
  // all the above tasks taken care of the by the function below
  unsigned int targetDescIndex;
  std::string needleType = this->NeedleTypeMenuList->GetWidget()->GetValue();
  if (!this->GetGUI()->GetLogic()->AddTargetToNeedle(needleType, rasPoint, targetDescIndex))
    {
    // error message
    return;
    }
  // 6) populate the list on GUI
  unsigned int listIndex = this->PopulateListWithTargetDetails(targetDescIndex);
  // resize the mapping array to correct size
  int numRows = this->TargetsMultiColumnList->GetWidget()->GetNumberOfRows();
  this->TargetDescriptorIndicesIndexedByListIndex.resize(numRows);  
  this->TargetDescriptorIndicesIndexedByListIndex[listIndex] = targetDescIndex;

}
//-----------------------------------------------------------------------------
unsigned int vtkTRProstateBiopsyTargetingStep::PopulateListWithTargetDetails(unsigned int targetDescIndex)
{

  vtkTRProstateBiopsyTargetDescriptor *target = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex (targetDescIndex);
  if ( target == NULL)
    {
    // error
    return 0;
    }

  unsigned int rowIndex = this->TargetsMultiColumnList->GetWidget()->GetNumberOfRows();

  this->TargetsMultiColumnList->GetWidget()->AddRow();

  // target number
  this->TargetsMultiColumnList->GetWidget()->SetCellTextAsInt(rowIndex, this->TargetNumberColumn, rowIndex+1);

  // needle type
  this->TargetsMultiColumnList->GetWidget()->SetCellText(rowIndex, this->NeedleTypeColumn, target->GetNeedleTypeString().c_str());

  // target location
  char rasStr[256] = "";
  double ras[3];
  target->GetRASLocation(ras);
  sprintf(rasStr, "%.1f, %.1f, %.1f", ras[0], ras[1], ras[2]);
  this->TargetsMultiColumnList->GetWidget()->SetCellText(rowIndex, this->RASLocationColumn, rasStr);

  // reachable
  char reachableStr[30] = "Reachable";
  if (target->GetIsOutsideReach())
      strcpy(reachableStr, "Out of reach!!");
  this->TargetsMultiColumnList->GetWidget()->SetCellText(rowIndex, this->ReachableColumn, reachableStr);

  // rotation
  this->TargetsMultiColumnList->GetWidget()->SetCellTextAsDouble(rowIndex, this->RotationColumn, target->GetAxisRotation());

  // needle angle
  this->TargetsMultiColumnList->GetWidget()->SetCellTextAsDouble(rowIndex, this->NeedleAngleColumn, target->GetNeedleAngle());

  // depth in cm
  this->TargetsMultiColumnList->GetWidget()->SetCellTextAsDouble(rowIndex, this->DepthColumn, target->GetDepthCM());

  return rowIndex;
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::Reset()
{
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

    // 1) click on load targeting volume dialog
  if (this->LoadTargetingVolumeButton)
    {
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->TargetsMultiColumnList)
    {
    this->TargetsMultiColumnList->GetWidget()->AddObserver(vtkKWMultiColumnList::SelectionChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }



}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::RemoveGUIObservers()
{
  // 1) click on load targeting volume dialog
  if (this->LoadTargetingVolumeButton)
    {
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->TargetsMultiColumnList)
    {
    this->TargetsMultiColumnList->GetWidget()->RemoveObservers(vtkKWMultiColumnList::SelectionChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::LoadTargetingVolumeButtonCallback(const char *fileName)
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }
  
  this->LoadTargetingVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPath");

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetGUI()->GetApplication());

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddVolumeToScene(app,fileString.c_str(), VOL_TARGETING);
        
  if (volumeNode)
    {
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();
    }
  else 
    {
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
    dialog->SetParent ( this->LoadVolumeDialogFrame );
    dialog->SetStyleToMessage();
    std::string msg = std::string("Unable to read volume file ") + std::string(fileName);
    dialog->SetText(msg.c_str());
    dialog->Create ( );
    dialog->Invoke();
    dialog->Delete();
    }
     
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::TargetSelectedFromListCallback()
{
  this->TargetDeselectedFromListCallback();

  // get the row index of the selected row
  if (this->TargetsMultiColumnList->GetWidget()->GetNumberOfSelectedRows() == 1)
    {
    int rowIndex = this->TargetsMultiColumnList->GetWidget()->GetIndexOfFirstSelectedRow();
    // use row index to get information about the target
    this->CurrentSelectedTargetDescriptorIndex = this->TargetDescriptorIndicesIndexedByListIndex[rowIndex];
    this->LastSelectedTargetDescriptorIndex = this->CurrentSelectedTargetDescriptorIndex;   
    
    // things to do on selection  
    int oldModify=this->GetGUI()->GetMRMLNode()->StartModify();
    this->GetGUI()->GetMRMLNode()->SetCurrentTargetIndex(this->CurrentSelectedTargetDescriptorIndex);
    // bring target to view in all three views
    this->BringTargetToViewIn2DViews();
    // change target color
    this->SetTargetFiducialColor(true);
    this->GetGUI()->GetMRMLNode()->EndModify(oldModify);
    }

}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::TargetDeselectedFromListCallback()
{
    // first ascertain that there was a valid target selected before
  if (this->LastSelectedTargetDescriptorIndex != -1)
    {
    // things to do on de-selection
    // clear information in the message box
    // clear information in the secondary window    
    // change target color
    this->SetTargetFiducialColor(false);
    }
}

//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::UpdateGUI()
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if (!mrmlNode)
  {
    return;
  }

  if (this->Message)
  {
    // get the information about the currently selected target descriptor
    int targetIndex=mrmlNode->GetCurrentTargetIndex();
    vtkTRProstateBiopsyTargetDescriptor *targetDesc = mrmlNode->GetTargetDescriptorAtIndex(targetIndex); 
    if (targetDesc!=NULL)
    {
      std::ostrstream os;  
      os << "Needle type:"<<targetDesc->GetNeedleTypeString()<<std::endl;
      os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(1);
      os << "RAS location: "<<targetDesc->GetRASLocationString().c_str()<<std::endl;
      os << "Reachable: "<<targetDesc->GetReachableString().c_str()<<std::endl;
      os << "Depth: "<<targetDesc->GetDepthCM()<<" cm"<<std::endl;
      os << "Device rotation: "<<targetDesc->GetAxisRotation()<<" deg"<<std::endl;
      os << "Needle angle: "<<targetDesc->GetNeedleAngle()<<" deg"<<std::endl;
      os << std::ends;
      this->Message->SetText(os.str());
      os.rdbuf()->freeze();
    }
    else
    {
      this->Message->SetText("");
    }
  }
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::BringTargetToViewIn2DViews()
{
  if (this->CurrentSelectedTargetDescriptorIndex != -1)
    {
    // set up the three 2D views

    // the slices may not be really orthogonal, they could be oblique
    // we could directly call slice node -> JumpAllSlices (r, a, s), this brings target in view
    // in all slices, but with target fiducial at the center of the view, moving (disturbing) the image altogether
    // for this function ->JumpSliceByOffsetting does the job
    
    // get the point ras location of the target fiducial (P) that lies on the image plane
    vtkTRProstateBiopsyTargetDescriptor *targetDesc = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);    
    double P[3];
    targetDesc->GetRASLocation(P);

    // red slice node    
    vtkSlicerSliceLogic *redSlice = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Red");    
    vtkSlicerSliceLogic *yellowSlice = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Yellow");    
    vtkSlicerSliceLogic *greenSlice = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Green");    

    int redOldModify=redSlice->GetSliceNode()->StartModify();
    int yellowOldModify=yellowSlice->GetSliceNode()->StartModify();
    int greenOldModify=greenSlice->GetSliceNode()->StartModify();

    redSlice->GetSliceNode()->JumpSliceByOffsetting(P[0], P[1], P[2]);
    yellowSlice->GetSliceNode()->JumpSliceByOffsetting(P[0], P[1], P[2]);
    greenSlice->GetSliceNode()->JumpSliceByOffsetting(P[0], P[1], P[2]);

    redSlice->GetSliceNode()->EndModify(redOldModify);
    yellowSlice->GetSliceNode()->EndModify(yellowOldModify);
    greenSlice->GetSliceNode()->EndModify(greenOldModify);
    }
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::SetTargetFiducialColor(bool selected)
{
  // get the target descriptor
  vtkTRProstateBiopsyTargetDescriptor *targetDesc = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);

  // locate the fiducial in the fiducial list
  // get the needle index, and fiducial index to access the fiducial from MRML list
  int needleIndex = targetDesc->GetNeedleListIndex();
  int fiducialIndex = targetDesc->GetFiducialIndex();

  // set its color to selected or not selected
  if (needleIndex != -1 && fiducialIndex != -1)
    {
    this->GetGUI()->GetMRMLNode()->SetFiducialColor(needleIndex, fiducialIndex, selected);
    }
  
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::SaveToExperimentFile(ostream &of)
{
   // reset parameters at MRML node
   vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  

  // for each target
  // parameters to be saved for targeting
  // 1) FoR str
  // 2) Needle type
  // 3) Needle depth
  // 4) Needle overshoot
  // 5) RAS location
  // 6) Reachable
  // 7) Axis rotation
  // 8) Needle angle
  // 9) DepthInCM
  // 10) Validated
  // 11) ValidationVolumeFoR str
  // 12) Overall error
  // 13) IS error
  // 14) AP error
  // 15) LR error
 // read the information from the TargetDescriptorsVector
  int totalTargets = mrmlNode->GetTotalNumberOfTargets();

  if (totalTargets > 0)
    {
    for (int targetDescIndex = 0; targetDescIndex < totalTargets; targetDescIndex++)
      {   
      vtkTRProstateBiopsyTargetDescriptor *target = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex (targetDescIndex);
      if ( target == NULL)
        {
        // error
        continue;
        }

      // 1) FoR str
      std::string str = target->GetFoRStr();
      of << " FoR=\"" ;
      of << str << " ";
      of << "\" \n";
    
      // 2) Needle type
      str = target->GetNeedleTypeString();
      of << " NeedleType=\"" ;
      of << str << " ";
      of << "\" \n";

      // 3) Needle depth
      double depth = target->GetNeedleDepth();
      of << " NeedleDepth=\"" ;
      of << depth << " ";
      of << "\" \n";

      // 4) Needle overshoot
      double overshoot = target->GetNeedleOvershoot();
      of << " NeedleOvershoot=\"" ;
      of << overshoot << " ";
      of << "\" \n";

      // 5) RAS location
      double ras[3];
      target->GetRASLocation(ras);
      of << " RAS=\"";
      for(int i = 0; i < 3; i++)
        of << ras[i] << " ";
      of << "\" \n";

      // 6) Reachable
      of << " Reachable=\"" ;     
      if(target->GetIsOutsideReach())
        {
        of << "false" << " ";
        }
      else
        {
        of << "true" << " ";
        }
      of << "\" \n";

      // 7) Axis rotation
      double rotation = target->GetAxisRotation();
      of << " AxisRotation=\"" ;
      of << rotation << " ";
      of << "\" \n";

      // 8) Needle angle
      double needleAngle = target->GetNeedleAngle();
      of << " NeedleAngle=\"" ;
      of << needleAngle << " ";
      of << "\" \n";

      // 9) DepthInCM
      double depthcm = target->GetDepthCM();
      of << " Depth(cm)=\"" ;
      of << depthcm << " ";
      of << "\" \n";

      // 10) Validated
      of << " Validated=\"" ;     
      if(target->GetTargetValidated())
        {
        of << "true" << " ";
        of << "\" \n";
        // 11) ValidationVolumeFoR str
        str = target->GetNeedleConfirmationVolumeFoRStr();
        of << " ValidationVolumeFoR=\"" ;
        of << str << " ";
        of << "\" \n";

        // 12) Overall error
        double err = target->GetOverallError();
        of << " OverallError(mm)=\"" ;
        of << err << " ";
        of << "\" \n";

        // 13) IS error
        err = target->GetISError();
        of << " ISError(mm)=\"" ;
        of << err << " ";
        of << "\" \n";

        // 14) AP error
        err = target->GetAPError();
        of << " APError(mm)=\"" ;
        of << err << " ";
        of << "\" \n";

        // 15) LR error
        err = target->GetLRError();
        of << " LRError(mm)=\"" ;
        of << err << " ";
        of << "\" \n";
        }
      else
        {
        of << "false" << " ";
        of << "\" \n";
        // 11) ValidationVolumeFoR str      
        of << " ValidationVolumeFoR=\"" ;
        of << "" << " ";
        of << "\" \n";

        // 12) Overall error        
        of << " OverallError(mm)=\"" ;
        of << "" << " ";
        of << "\" \n";

        // 13) IS error     
        of << " ISError(mm)=\"" ;
        of << "" << " ";
        of << "\" \n";

        // 14) AP error     
        of << " APError(mm)=\"" ;
        of << "" << " ";
        of << "\" \n";

        // 15) LR error     
        of << " LRError(mm)=\"" ;
        of << "" << " ";
        of << "\" \n";
        }  

      }
    }
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::LoadFromExperimentFile(istream &file)
{
}
