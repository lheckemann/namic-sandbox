#include "vtkTRProstateBiopsyVerificationStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkTRProstateBiopsyTargetDescriptor.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMath.h"
#include "vtkTRProstateBiopsyMath.h"

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
#include "vtkKWTextWithScrollbars.h"
#include "vtkKWPushButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWMessageDialog.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyVerificationStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyVerificationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsyVerificationStep::vtkTRProstateBiopsyVerificationStep()
{
  this->SetName("4/4. Verification");
  this->SetDescription("Select targets and prepare for biopsy.");

  
  this->WizardGUICallbackCommand->SetCallback(&vtkTRProstateBiopsyVerificationStep::WizardGUICallback);


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
  this->AcquireClicks = false;
  
  this->NeedleClick1[0] = 0;
  this->NeedleClick1[1] = 0;
  this->NeedleClick1[2] = 0;
  
  this->NeedleClick2[0] = 0;
  this->NeedleClick2[1] = 0;
  this->NeedleClick2[2] = 0;

  this->LastSelectedTargetDescriptorIndex = -1;
  this->CurrentSelectedTargetDescriptorIndex = -1;

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

  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  this->PopulateListWithTargets();
  this->AddGUIObservers();
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

    this->TargetsMultiColumnList->GetWidget()->SetSelectionTypeToRow();
    this->TargetsMultiColumnList->GetWidget()->SetSelectionModeToSingle();
    this->TargetsMultiColumnList->GetWidget()->SetSelectionBackgroundColor(0.0, 0.0, 1.0);
    this->TargetsMultiColumnList->GetWidget()->SetSelectionForegroundColor(1.0,1.0,1.0);

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
void vtkTRProstateBiopsyVerificationStep::WizardGUICallback( vtkObject *caller,
                         unsigned long eid, void *clientData, void *callData )
{

  vtkTRProstateBiopsyVerificationStep *self = reinterpret_cast<vtkTRProstateBiopsyVerificationStep *>(clientData);
  
  //if (self->GetInGUICallbackFlag())
    {
    // check for infinite recursion here
    }

  vtkDebugWithObjectMacro(self, "In vtkTRProstateBiopsyVerificationStep GUICallback");
  
  //self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  //self->SetInGUICallbackFlag(0);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ProcessGUIEvents(vtkObject *caller,
                                           unsigned long event, void *callData)
{
   vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  // load targeting volume dialog button
  if (this->LoadVerificationVolumeButton && this->LoadVerificationVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    this->LoadVerificationVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("TRProstateOpenPathVol");          
    const char *fileName = this->LoadVerificationVolumeButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->LoadVerificationVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPathVol");
      // call the callback function
      this->LoadVerificationVolumeButtonCallback(fileName);    
      } 
   
    }
  // targeting list selected
  if (this->TargetsMultiColumnList && this->TargetsMultiColumnList->GetWidget() == vtkKWMultiColumnList::SafeDownCast(caller) && (event == vtkKWMultiColumnList::SelectionChangedEvent))
    {
    this->TargetSelectedFromListCallback();
    }
  
  // compute error button
  if (this->ComputeErrorButton && this->ComputeErrorButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    this->ComputeTargetErrorCallback();
    }
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

    ++this->ClickNumber;

    vtkRenderWindowInteractor *rwi;
    vtkMatrix4x4 *matrix;    
    
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
void vtkTRProstateBiopsyVerificationStep::RecordClick(double rasPoint[])
{
  if (this->AcquireClicks)
    {
    vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

    char *msg = new char[40];
      
    sprintf(msg, "Clicked 1st marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);
      
    wizard_widget->SetErrorText(msg);
    wizard_widget->Update();


    if (this->ClickNumber ==1)
        {    
        // first click on needle
        this->NeedleClick1[0] = rasPoint[0];
        this->NeedleClick1[1] = rasPoint[1];
        this->NeedleClick1[2] = rasPoint[2];
        }
    else if (this->ClickNumber == 2)
        {
        // calculate the errors for the particular target
        // second click on the needle
        this->NeedleClick2[0] = rasPoint[0];
        this->NeedleClick2[1] = rasPoint[1];
        this->NeedleClick2[2] = rasPoint[2];    

        // get the current active target descriptor
        vtkTRProstateBiopsyTargetDescriptor *targetDesc = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);

        double overallErr = 0;
        double apErr = 0; double lrErr = 0; double isErr = 0;
        double targetRAS[3];
        targetDesc->GetRASLocation(targetRAS);
        overallErr = vtkTRProstateBiopsyMath::ComputeDistanceLinePoint(NeedleClick1, NeedleClick2, targetRAS, apErr, lrErr, isErr);
        
        // set the variable values in target descriptor
        targetDesc->SetTargetValidated(true);
        targetDesc->SetOverallError(overallErr);

        // set the GUI feedback
        this->PopulateMessageBoxWithTargetInfo();
        this->UpdateListWithErrorDetails(this->CurrentSelectedTargetDescriptorIndex, overallErr, isErr, apErr, lrErr);

        this->ClickNumber = 0;
        this->AcquireClicks = false;
        }

    }
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::Reset()
{
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

    // 1) click on load targeting volume dialog
  if (this->LoadVerificationVolumeButton)
    {
    this->LoadVerificationVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->TargetsMultiColumnList)
    {
    this->TargetsMultiColumnList->GetWidget()->AddObserver(vtkKWMultiColumnList::SelectionChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  // 3) click on reset calib button
  if (this->ComputeErrorButton)
    {
    this->ComputeErrorButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::RemoveGUIObservers()
{
    // 1) click on load verifcation volume dialog
  if (this->LoadVerificationVolumeButton)
    {
    this->LoadVerificationVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->TargetsMultiColumnList)
    {
    this->TargetsMultiColumnList->GetWidget()->RemoveObservers(vtkKWMultiColumnList::SelectionChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
   // 3) click on compute error button
  if (this->ComputeErrorButton)
    {
    this->ComputeErrorButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

  
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::ComputeTargetErrorCallback()
{
  this->AcquireClicks = true;
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::LoadVerificationVolumeButtonCallback(const char *fileName)
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }
  
  this->LoadVerificationVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPath");

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetGUI()->GetApplication());

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddVolumeToScene(app,fileString.c_str(), VOL_VERIFICATION);
        
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
void vtkTRProstateBiopsyVerificationStep::TargetSelectedFromListCallback()
{
  this->TargetDeselectedFromListCallback();

  // get the row index of the selected row
  if (this->TargetsMultiColumnList->GetWidget()->GetNumberOfSelectedRows() == 1)
    {
    int rowIndex = this->TargetsMultiColumnList->GetWidget()->GetIndexOfFirstSelectedRow();
    // use row index to get information about the target
    // the target index in the target descriptor list corresponds to the row index, because that's how it was populated
    this->CurrentSelectedTargetDescriptorIndex = rowIndex;
    this->LastSelectedTargetDescriptorIndex = this->CurrentSelectedTargetDescriptorIndex;   
    
    // things to do on selection
    
    // display information in the message box
    this->PopulateMessageBoxWithTargetInfo();

    // display information in the secondary window
//    this->DisplayTargetInfoInSecondaryWindow();

    // bring target to view in all three views
    this->BringTargetToViewIn2DViews();

    // change target color
    this->SetTargetFiducialColor(true);

 

    }

}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::TargetDeselectedFromListCallback()
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

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::PopulateMessageBoxWithTargetInfo()
{
  this->Message->SetText("");
  // get the information about the currently selected target descriptor
  if (this->CurrentSelectedTargetDescriptorIndex != -1)
    {
    vtkTRProstateBiopsyTargetDescriptor *targetDesc = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);
    char buffer[1024];
    if (targetDesc->GetTargetValidated())
      {
          sprintf(buffer, "Needle type: %s\nRAS location: %s\nReachable: %s\nOverall error(mm): %.1f\nIS axis error (mm): %.1f\nAP axis error (mm): %.1f\nLR axis error(mm): %.1f", 
            targetDesc->GetNeedleTypeString().c_str(), targetDesc->GetRASLocationString().c_str(), targetDesc->GetReachableString().c_str(), targetDesc->GetOverallError(), targetDesc->GetISError(), targetDesc->GetAPError(), targetDesc->GetLRError());
      }
    else
      {
      sprintf(buffer, "Needle type: %s\nRAS location: %s\nReachable: %s\nTarget not validated yet", 
            targetDesc->GetNeedleTypeString().c_str(), targetDesc->GetRASLocationString().c_str(), targetDesc->GetReachableString().c_str());
      }
    this->Message->SetText(buffer);
    }
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::PopulateListWithTargets()
{
   vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  // clear the list first
  this->TargetsMultiColumnList->GetWidget()->DeleteAllRows();

  // read the information from the TargetDescriptorsVector
  int totalTargets = mrmlNode->GetTotalNumberOfTargets();

  if (totalTargets > 0)
    {
    for (int targetDescIndex = 0; targetDescIndex < totalTargets; targetDescIndex++)
      {   
      // this will ensure that rowIndex and targetDescIndex will match up
      // because at this point, there won't be any deletions
      this->PopulateListWithTargetDetails(targetDescIndex);
      }
    }
  
}
void vtkTRProstateBiopsyVerificationStep::UpdateListWithErrorDetails(unsigned int rowIndex, double ovrErr, double isErr, double apErr, double lrErr)
{
 

  // Overall error
  char errStr[30] = "";
  sprintf(errStr, "%.1f", ovrErr);
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->OverallErrorColumn, errStr);

  // IS error
  sprintf(errStr, "%.1f", isErr);
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->ISErrorColumn, errStr);

  // AP error
  sprintf(errStr, "%.1f", apErr);
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->APErrorColumn, errStr);

  // LR error
  sprintf(errStr, "%.1f", lrErr);
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->LRErrorColumn, errStr);

}
//-----------------------------------------------------------------------------
unsigned int vtkTRProstateBiopsyVerificationStep::PopulateListWithTargetDetails(unsigned int targetDescIndex)
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

  // Overall error
  char errStr[30] = "";
  if (target->GetTargetValidated())
      sprintf(errStr, "%.1f", target->GetOverallError());
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->OverallErrorColumn, errStr);

  // IS error
  if (target->GetTargetValidated())
      sprintf(errStr, "%.1f", target->GetISError());
  else
      sprintf(errStr,"");
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->ISErrorColumn, errStr);

  // AP error
  if (target->GetTargetValidated())
      sprintf(errStr, "%.1f", target->GetAPError());
  else
      sprintf(errStr,"");
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->APErrorColumn, errStr);

  // LR error
  if (target->GetTargetValidated())
      sprintf(errStr, "%.1f", target->GetLRError());
  else
      sprintf(errStr,"");
  this->TargetsMultiColumnList->GetWidget()->SetCellText (rowIndex, this->LRErrorColumn, errStr);


  return rowIndex;
}


//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::BringTargetToViewIn2DViews()
{
  if (this->CurrentSelectedTargetDescriptorIndex != -1)
    {
    // set up the three 2D views

    // the slices may not be really orthogonal, they could be oblique
    // we could directly call slice node -> JumpAllSlices (r, a, s), this brings target in view
    // in all slices, but with target fiducial at the center of the view, moving (disturbing) the image altogether
    
    // we rather want the image itself to stay sort of stationary, the r,a,s poitn where it is spacialy, just moving to the right slice
    // Given:
    // we have an oblique plane defined by our target point P (x0, y0, z0) and the normal vector (can be had from SliceToRAS matrix)
    // Point Q (x1, y1, y2) of current slice location which is on a parallel plane at the center of the slice
    // To find:
    // A point R (x2, y2, z2) lying on oblique, corresponding to point Q
    // Solution: The angle between vector QP and normal is used to calculate the distance between current slice and target slice


    // get the point ras location of the target fiducial (P) that lies on the image plane
    vtkTRProstateBiopsyTargetDescriptor *targetDesc = this->GetGUI()->GetMRMLNode()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);    
    double P[3];
    targetDesc->GetRASLocation(P);

    // get the normal vector of the plane, and also direction of the line
    vtkSlicerSliceLogic *redSlice = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Red");    

    /*
    vtkMatrix4x4 *sliceToRAS = vtkMatrix4x4::New();
    sliceToRAS->DeepCopy(redSlice->GetSliceNode()->GetSliceToRAS());  
    double r1[3] = {sliceToRAS->GetElement(0, 0), sliceToRAS->GetElement(0, 1), sliceToRAS->GetElement(0, 2)};
    double r2[3] = {sliceToRAS->GetElement(1, 0), sliceToRAS->GetElement(1, 1), sliceToRAS->GetElement(1, 2)};
    double normal[3] = {sliceToRAS->GetElement(2, 0), sliceToRAS->GetElement(2, 1), sliceToRAS->GetElement(2, 2)};
    vtkMath::Cross(r1, r2, normal);
    vtkMath::Normalize(normal);
    
    
    double spacing[3];
    this->GetGUI()->GetMRMLNode()->GetTargetingVolumeNode()->GetSpacing(spacing);
    
    double *sliceSpacing;
    sliceSpacing = redSlice->GetVolumeSliceSpacing(this->GetGUI()->GetMRMLNode()->GetTargetingVolumeNode());

    double rasCenter[3], rasDimensions[3];
    redSlice->GetVolumeRASBox(this->GetGUI()->GetMRMLNode()->GetTargetingVolumeNode(), rasDimensions, rasCenter);

    double sliceCenter[3], sliceDimensions[3];
    redSlice->GetVolumeSliceDimensions(this->GetGUI()->GetMRMLNode()->GetTargetingVolumeNode(), sliceDimensions, sliceCenter);

    // get two planes: one containing the origin, and one containing the target
    // one containing the origin: ax + by + cz = d, compute d
    // point rasCenter lies on the plane
    // d = ax + by + cz
    // intercept plane 1, 
    double d = normal[0]*rasCenter[0] + normal[1]*rasCenter[1] + normal[2]*rasCenter[2];

    // one containing the origin: ax + by + cz = e, compute e
    // point target lies on the plane
    // e = ax + by + cz
    // intercept plane 1, 
    double e = normal[0]*P[0] + normal[1]*P[1] + normal[2]*P[2];

    // since we've already normalized the normal vector
    double distance = fabs(d-e);

    */

    /*

    double Q[3];    
    //Q[0] = sliceToRAS->GetElement(0, 3);
    //Q[1] = sliceToRAS->GetElement(1, 3);
    //Q[2] = redSlice->GetSliceOffset();
    //Q[2] = sliceToRAS->GetElement(2, 3);

    // Let Q be the ras center
    Q[0] = rasCenter[0];
    Q[1] = rasCenter[1];
    Q[2] = rasCenter[2];

    // calculate vector QP
    double QP[3];
    QP[0] = P[0] - Q[0];
    QP[1] = P[1] - Q[1];
    QP[2] = P[2] - Q[2];

    double s = 0.0;
    if (Q[2] > P[2])
      {
      normal[2] = -normal[2];
      QP[0] = -QP[0];
      QP[1] = -QP[1];
      QP[2] = -QP[2];
      }
    

    double vecMagnitude = sqrt(QP[0]*QP[0] + QP[1]*QP[1] + QP[2]*QP[2]);
    vtkMath::Normalize(normal);
    vtkMath::Normalize(QP);
    double angle =  acos(vtkMath::Dot(normal,QP));

    double degrees = angle * vtkMath::RadiansToDegrees();

    double distance = vecMagnitude * cos (angle);

    s += Q[2] + distance;
    //sliceToRAS->SetElement(2, 3, s);
    //redSlice->GetSliceNode()->GetSliceToRAS()->DeepCopy(sliceToRAS);
    //redSlice->GetSliceNode()->UpdateMatrices();
    //redSlice->SetSliceOffset(s);
    //redSlice->GetSliceNode()->JumpSlice(P[0], P[1], P[2]);
    //redSlice->GetSliceNode()->JumpAllSlices(P[0], P[1], P[2]);

    */

    redSlice->GetSliceNode()->JumpSlice(P[0], P[1], P[2]);
    redSlice->GetSliceNode()->JumpAllSlices(P[0], P[1], P[2]);
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->RequestRender();
    //sliceToRAS->Delete();
    
    

    


    }
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::SetTargetFiducialColor(bool selected)
{
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::SaveToExperimentFile(ostream &of)
{
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyVerificationStep::LoadFromExperimentFile(istream &file)
{
}
