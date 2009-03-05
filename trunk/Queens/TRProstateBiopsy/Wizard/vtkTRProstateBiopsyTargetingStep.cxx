
#include "vtkTRProstateBiopsyTargetingStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkTRProstateBiopsyTargetDescriptor.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMath.h"

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

  mrmlNode->GetCalibrationFiducialsList()->SetAllFiducialsVisibility(false);

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
    const char *fileName = this->LoadTargetingVolumeButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
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
  
  sprintf(msg, "Clicked 1st marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);

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
  unsigned int fiducialIndex;
  std::string needleType = this->NeedleTypeMenuList->GetWidget()->GetValue();
  if (!this->GetGUI()->GetLogic()->AddTargetToNeedle(needleType, rasPoint, targetDescIndex, fiducialIndex))
    {
    // error message
    return;
    }
  // 6) populate the list on GUI
  unsigned int listIndex = this->PopulateListWithTargetDetails(targetDescIndex);
  // resize the mapping array to correct size
  int numRows = this->TargetsMultiColumnList->GetWidget()->GetNumberOfRows();
  this->TargetDescriptorIndicesIndexedByListIndex.resize(numRows);
  TargetIndicesStruct *tempSt = new TargetIndicesStruct;
  tempSt->TargetDescIndex = targetDescIndex;
  tempSt->FiducialIndex = fiducialIndex;
  this->TargetDescriptorIndicesIndexedByListIndex[listIndex] = tempSt;

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
  double r, a, s;
  target->GetRASLocation(r, a, s);
  sprintf(rasStr, "%.1f, %.1f, %.1f", r, a, s);
  this->TargetsMultiColumnList->GetWidget()->SetCellText(rowIndex, this->RASLocationColumn, rasStr);

  // reachable
  char reachableStr[30] = "Reachable";
  if (target->GetIsOutside())
      strcpy(reachableStr, "Out of reach!!");
  this->TargetsMultiColumnList->GetWidget()->SetCellText(rowIndex, this->ReachableColumn, reachableStr);

  // rotation
  this->TargetsMultiColumnList->GetWidget()->SetCellTextAsDouble(rowIndex, this->RotationColumn, target->GetRotation());

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
  // 1) click on load calibration volume dialog
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

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddTargetingVolume(app,fileString.c_str());
        
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
    this->CurrentSelectedTargetDescriptorIndex = this->TargetDescriptorIndicesIndexedByListIndex[rowIndex]->TargetDescIndex;
    this->LastSelectedTargetDescriptorIndex = this->CurrentSelectedTargetDescriptorIndex;   
    
    // things to do on selection
    
    // display information in the message box
    this->PopulateMessageBoxWithTargetInfo();

    // display information in the secondary window
    this->DisplayTargetInfoInSecondaryWindow();

    // bring target to view in all three views
    //this->BringTargetToViewIn2DViews();

    // change target color
    this->SetTargetFiducialColor(true);

    // show needle path in 2D views
    this->ShowNeedlePathIn2DViews();

    // show needle path in 3D view
    this->ShowNeedlePathIn3DView();

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
    // hide needle path in 2D views
    // hide needle path in 3D view
    }
}

//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::PopulateMessageBoxWithTargetInfo()
{
  this->Message->SetText("");
  // get the information about the currently selected target descriptor
  if (this->CurrentSelectedTargetDescriptorIndex != -1)
    {
    vtkTRProstateBiopsyTargetDescriptor *targetDesc = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);
    char buffer[1024];
    sprintf(buffer, "Needle type: %s\nRAS location: %s\nReachable: %s\nDepth (cm): %.1f\nDevice rotation (degrees): %.1f\nNeedle angle (degrees): %.1f", 
            targetDesc->GetNeedleTypeString().c_str(), targetDesc->GetRASLocationString().c_str(), targetDesc->GetReachableString().c_str(), targetDesc->GetDepthCM(), targetDesc->GetRotation(), targetDesc->GetNeedleAngle());
    this->Message->SetText(buffer);
    }
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::DisplayTargetInfoInSecondaryWindow()
{
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::BringTargetToViewIn2DViews()
{
  if (this->CurrentSelectedTargetDescriptorIndex != -1)
    {
    // set up the three 2D views

    // since the slices may not be really orthogonal, they could be oblique
    // we could directly call slice node -> JumpAllSlices (r, a, s), this brings target in view
    // in all slices, but with target fiducial at the center of the view, moving (disturbing) the image altogether
    
    // we rather want the image itself to stay sort of stationary, the r,a,s poitn where it is spacialy, just moving to the right slice
    // Given:
    // we have an oblique plane defined by our target point P (x0, y0, z0) and the normal vector (can be had from SliceToRAS matrix)
    // Point Q (x1, y1, y2) of current slice location which is on a parallel plane at the center of the slice
    // To find:
    // A point R (x2, y2, z2) lying on oblique, corresponding to point Q
    // Solution: Point Q lies on the plane as well on line containing R in direction of its normal vector


    // get the point ras location of the target fiducial (P) that lies on the image p
    vtkTRProstateBiopsyTargetDescriptor *targetDesc = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);    
    double P[3];
    targetDesc->GetRASLocation(P[0], P[1], P[2]);

    // get the normal vector of the plane, and also direction of the line
    vtkSlicerSliceLogic *redSlice = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Red");    
    vtkMatrix4x4 *sliceToRAS = vtkMatrix4x4::New();
    sliceToRAS->DeepCopy(redSlice->GetSliceNode()->GetSliceToRAS());  
    double r1[3] = {sliceToRAS->GetElement(0, 0), sliceToRAS->GetElement(0, 1), sliceToRAS->GetElement(0, 2)};
    double r2[3] = {sliceToRAS->GetElement(1, 0), sliceToRAS->GetElement(1, 1), sliceToRAS->GetElement(1, 2)};
    double normal[3];
    vtkMath::Cross(r1, r2, normal);
    

    /*
    double rasDimensions[3], rasCenter[3], sliceCenter[3], sliceDimensions[3];
    redSlice->GetVolumeRASBox (this->GetGUI()->GetMRMLNode()->GetTargetingVolumeNode(), rasDimensions, rasCenter);
    redSlice->GetVolumeSliceDimensions(this->GetGUI()->GetMRMLNode()->GetTargetingVolumeNode(), sliceDimensions, sliceCenter);
    */
    double spacing[3];
    this->GetGUI()->GetMRMLNode()->GetTargetingVolumeNode()->GetSpacing(spacing);
    //vtkMath::Do
    //vtkMath::Normalize(normal);
    
    
    /*
    double orthoNormal[3] = { 0.0, 0.0, 1.0};

    double theta =  vtkMath::Pi() - acos(vtkMath::Dot(normal, orthoNormal));

    double deg = vtkMath::DoubleRadiansToDegrees() * theta;

    double r = P[0]* cos(theta);
    double a = P[1]* cos(theta);
    double s = P[2]* cos(theta);

    s += 2.94;
    */
    double Q[3];
    /*Q[0] = rasCenter[0];
    Q[1] = rasCenter[1];
    Q[2] = rasCenter[2];*/

    Q[0] = sliceToRAS->GetElement(0, 3);
    Q[1] = sliceToRAS->GetElement(1, 3);
    Q[2] = redSlice->GetSliceOffset();

    // calculate vector QP
    double QP[3];
    QP[0] = P[0] - Q[0];
    QP[1] = P[1] - Q[1];
    QP[2] = P[2] - Q[2];

    /*double normal[3];
    normal[0] = sliceToRAS->GetElement(2, 0);
    normal[0] = sliceToRAS->GetElement(2, 1);
    normal[0] = sliceToRAS->GetElement(2, 2);
    */
    if (Q[2] > P[2])
        normal[2] = -normal[2];
    
    double vecMagnitude = sqrt(QP[0]*QP[0] + QP[1]*QP[1] + QP[2]*QP[2]);
    vtkMath::Normalize(normal);
    vtkMath::Normalize(QP);
    double angle =  acos(vtkMath::Dot(normal,QP));

    double degrees = angle * vtkMath::RadiansToDegrees();

    double distance = vecMagnitude * cos (angle);

    double s = Q[2] + distance;

    /*

    // define plane containing P, and in direction of the normal vector of the form ax + by + cz + d = 0, where normal (a,b,c) and intercept d
    // calculate d, making point P to lie on plane d = - (a*x0 + b*y0 + c*z0) 
    double d = - (normal[0]* P[0] + normal[1]* P[1] + normal[2]* P[2]);
    // define line in parameteric form x2 = x1 + a*t; y2 = y1 + b*t; z2 = z1 + c*t
    // Point R(x2, y2, z2) lies on the plane and also the line
    // substituing & solving for t = -(ax1 + by1 + cz1 + d)/(a*a + b*b + c*c)
    double nom = - ( normal[0]* Q[0] + normal[1]* Q[1] + normal[2]* Q[2] + d);
    double denom = normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2];
    double t = nom/denom;
    double R[3];
    R[0] = Q[0] + normal[0] * t;
    R[1] = Q[1] + normal[1] * t;
    R[2] = Q[2] + normal[2] * t;

    */


    //redSlice->GetSliceNode()->JumpSlice(sr, sa, ras[2]);
    //sliceToRAS->SetElement(0, 3, sr);
    //sliceToRAS->SetElement(1, 3, sa);
    sliceToRAS->SetElement(2, 3, s);



    redSlice->GetSliceNode()->GetSliceToRAS()->DeepCopy(sliceToRAS);
    redSlice->GetSliceNode()->UpdateMatrices();
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->RequestRender();

    sliceToRAS->Delete();
    
    

    


    }
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::SetTargetFiducialColor(bool selected)
{
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ShowNeedlePathIn2DViews()
{
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ShowNeedlePathIn3DView()
{
}
