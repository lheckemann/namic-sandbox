/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkProstateNavStepTargetingCryoTemplate.h"

#include "vtkProstateNavGUI.h"
#include "vtkProstateNavLogic.h"

#include "vtkKWMatrixWidget.h"
#include "vtkKWMatrixWidgetWithLabel.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"
#include "vtkMRMLSelectionNode.h"

#include "vtkMRMLLinearTransformNode.h"
#include "igtlMath.h"

#include "vtkMRMLRobotNode.h"

#include "vtkMath.h"

////

#include "vtkProstateNavTargetDescriptor.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLFiducial.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkSlicerVolumesGUI.h"
#include "vtkMRMLInteractionNode.h"

#include "vtkKWFrame.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWFrame.h"
#include "vtkKWEntry.h"
#include "vtkKWLabel.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWText.h"
#include "vtkKWPushButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWCheckButton.h"

#include "vtkMRMLTransPerinealProstateTemplateNode.h"
#include "vtkMRMLTransPerinealProstateCryoTemplateNode.h"

#include "vtkProstateNavReportWindow.h"

const char TARGET_INDEX_ATTR[]="TARGET_IND";

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

// Definition of target list columns
enum
{
  COL_NAME = 0,
  COL_X,
  COL_Y,
  COL_Z,
  COL_NEEDLE,
  COL_OR_W,
  COL_OR_X,
  COL_OR_Y,
  COL_OR_Z,
  COL_COUNT // all valid columns should be inserted above this line
};
static const char* COL_LABELS[COL_COUNT] = { "Name", "R", "A", "S", "Needle", "OrW", "OrX", "OrY", "OrZ" };
static const int COL_WIDTHS[COL_COUNT] = { 8, 6, 6, 6, 8, 6, 6, 6, 6 };


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavStepTargetingCryoTemplate);
vtkCxxRevisionMacro(vtkProstateNavStepTargetingCryoTemplate, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkProstateNavStepTargetingCryoTemplate::vtkProstateNavStepTargetingCryoTemplate()
{
  //this->SetName("Targeting");
  this->SetTitle("Targeting");
  this->SetDescription("Set target points.");

  this->MainFrame=NULL;

  // TargetPlanning frame
  this->TargetPlanningFrame=NULL;
  //this->LoadTargetingVolumeButton=NULL;
  this->VolumeSelectorWidget=NULL;
  this->TargetListSelectorWidget=NULL;
  this->TargetPlanningFrame=NULL;
  this->ShowTemplateButton=NULL;
  //this->ShowNeedleButton=NULL;
  this->AddTargetsOnClickButton=NULL;
  this->NeedleTypeMenuList=NULL;

  // TargetList frame
  this->TargetListFrame=NULL;
  this->TargetList=NULL;
  this->DeleteButton=NULL;

  // TargetControl frame
  this->TargetControlFrame=NULL;
  this->NeedlePositionMatrix=NULL;
  //this->NeedleOrientationMatrix=NULL;
  this->GenerateListButton=NULL;
  this->MoveButton=NULL;

  this->Message=NULL;

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 0.8;

  this->ProcessingCallback = false;

  this->TargetPlanListNode=NULL;

  this->OptionFrame = NULL;

  this->ReportWindow = NULL;
}

//----------------------------------------------------------------------------
vtkProstateNavStepTargetingCryoTemplate::~vtkProstateNavStepTargetingCryoTemplate()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);

  // TargetPlanning
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetPlanningFrame);
  //DELETE_IF_NULL_WITH_SETPARENT_NULL(LoadTargetingVolumeButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(VolumeSelectorWidget);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetListSelectorWidget);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetPlanningFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ShowTemplateButton);
  //DELETE_IF_NULL_WITH_SETPARENT_NULL(ShowNeedleButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(AddTargetsOnClickButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(NeedleTypeMenuList);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(OptionFrame);

  // TargetList frame
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetListFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetList);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(DeleteButton);

  // TargetControl frame
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetControlFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(NeedlePositionMatrix);
  //DELETE_IF_NULL_WITH_SETPARENT_NULL(NeedleOrientationMatrix);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(GenerateListButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(MoveButton);

  DELETE_IF_NULL_WITH_SETPARENT_NULL(Message);

  if (this->ReportWindow)
    {
    this->ReportWindow->Withdraw();
    this->ReportWindow->SetApplication(NULL);
    this->ReportWindow->Delete();
    this->ReportWindow = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  this->ShowTargetPlanningFrame();
  this->ShowTargetListFrame();
  this->ShowTargetControlFrame();

  AddMRMLObservers();

  this->AddGUIObservers();

  EnableAddTargetsOnClickButton(this->AddTargetsOnClickButton->GetSelectedState()==1);
}

//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ShowTargetPlanningFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->TargetPlanningFrame)
    {
    this->TargetPlanningFrame = vtkKWFrame::New();
    }
  if (!this->TargetPlanningFrame->IsCreated())
    {
    this->TargetPlanningFrame->SetParent(parent);
    this->TargetPlanningFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2",
               this->TargetPlanningFrame->GetWidgetName());

  //if (!this->LoadTargetingVolumeButton)
  //  {
  //   this->LoadTargetingVolumeButton = vtkKWPushButton::New();
  //  }
  //if (!this->LoadTargetingVolumeButton->IsCreated())
  //  {
  //  this->LoadTargetingVolumeButton->SetParent(this->TargetPlanningFrame);
  //  this->LoadTargetingVolumeButton->Create();
  //  this->LoadTargetingVolumeButton->SetBorderWidth(2);
  //  this->LoadTargetingVolumeButton->SetReliefToRaised();
  //  this->LoadTargetingVolumeButton->SetHighlightThickness(2);
  //  this->LoadTargetingVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
  //  this->LoadTargetingVolumeButton->SetActiveBackgroundColor(1,1,1);
  //  this->LoadTargetingVolumeButton->SetText( "Load volume");
  //  this->LoadTargetingVolumeButton->SetBalloonHelpString("Click to load a volume. Need to additionally select the volume to make it the current targeting volume.");
  //  }

  if (!this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget = vtkSlicerNodeSelectorWidget::New();
    }
  if (!this->VolumeSelectorWidget->IsCreated())
    {
    this->VolumeSelectorWidget->SetParent(this->TargetPlanningFrame);
    this->VolumeSelectorWidget->Create();
    this->VolumeSelectorWidget->SetBorderWidth(2);
    this->VolumeSelectorWidget->SetNodeClass("vtkMRMLVolumeNode", NULL, NULL, NULL);
    this->VolumeSelectorWidget->SetMRMLScene(this->GetLogic()->GetApplicationLogic()->GetMRMLScene());
    this->VolumeSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
    this->VolumeSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
    this->VolumeSelectorWidget->SetLabelText( "Targeting Volume: ");
    this->VolumeSelectorWidget->SetBalloonHelpString("Select the targeting volume from the current scene.");
    }

  if (!this->TargetListSelectorWidget)
    {
    this->TargetListSelectorWidget = vtkSlicerNodeSelectorWidget::New();
    }
  if (!this->TargetListSelectorWidget->IsCreated())
    {
    this->TargetListSelectorWidget->SetParent(this->TargetPlanningFrame);
    this->TargetListSelectorWidget->Create();
    this->TargetListSelectorWidget->SetBorderWidth(2);
    this->TargetListSelectorWidget->SetNodeClass("vtkMRMLFiducialListNode", NULL, NULL, NULL);
    this->TargetListSelectorWidget->SetMRMLScene(this->GetLogic()->GetApplicationLogic()->GetMRMLScene());
    this->TargetListSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
    this->TargetListSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
    this->TargetListSelectorWidget->SetLabelText( "Target List: ");
    this->TargetListSelectorWidget->SetBalloonHelpString("Select the fiducial list from the current scene.");
    }

  if (!this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList = vtkKWMenuButtonWithLabel::New();
    }
  if (!this->NeedleTypeMenuList->IsCreated())
    {
    this->NeedleTypeMenuList->SetParent(this->TargetPlanningFrame);
    this->NeedleTypeMenuList->Create();
    this->NeedleTypeMenuList->SetLabelText("Needle type: ");
    this->NeedleTypeMenuList->SetBalloonHelpString("Select the needle type");
    }

  this->Script("pack %s %s -side top -anchor nw -expand y -padx 2 -pady 2",
               this->VolumeSelectorWidget->GetWidgetName(),
               this->TargetListSelectorWidget->GetWidgetName());
  //this->NeedleTypeMenuList->GetWidgetName());

  if (!this->OptionFrame)
    {
    this->OptionFrame = vtkKWFrame::New();
    }
  if (!this->OptionFrame->IsCreated())
    {
    this->OptionFrame->SetParent(this->TargetPlanningFrame);
    this->OptionFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 2",
               this->OptionFrame->GetWidgetName());

  if (!this->AddTargetsOnClickButton)
    {
    this->AddTargetsOnClickButton = vtkKWCheckButton::New();
    }
  if (!this->AddTargetsOnClickButton->IsCreated()) {
  this->AddTargetsOnClickButton->SetParent(this->OptionFrame);
  this->AddTargetsOnClickButton->Create();
  this->AddTargetsOnClickButton->SelectedStateOff();
  this->AddTargetsOnClickButton->SetText("Add target by image click");
  this->AddTargetsOnClickButton->SetBalloonHelpString("Add a target if image is clicked, with the current needle");
  }

/*
  if (!this->ShowNeedleButton)
  {
  this->ShowNeedleButton = vtkKWCheckButton::New();
  }
  if (!this->ShowNeedleButton->IsCreated())
  {
  this->ShowNeedleButton->SetParent(this->OptionFrame);
  this->ShowNeedleButton->Create();
  this->ShowNeedleButton->SelectedStateOff();
  this->ShowNeedleButton->SetText("Needle");
  this->ShowNeedleButton->SetBalloonHelpString("Show needle path");
  }
*/
  if (!this->ShowTemplateButton)
    {
    this->ShowTemplateButton = vtkKWCheckButton::New();
    }
  if (!this->ShowTemplateButton->IsCreated()) {
  this->ShowTemplateButton->SetParent(this->OptionFrame);
  this->ShowTemplateButton->Create();
  this->ShowTemplateButton->SelectedStateOff();
  this->ShowTemplateButton->SetText("Template");
  this->ShowTemplateButton->SetBalloonHelpString("Show predicted needle path");
  }


  this->Script("pack %s %s -side left -expand y -padx 2 -pady 2",
               this->AddTargetsOnClickButton->GetWidgetName(),
               this->ShowTemplateButton->GetWidgetName());

  this->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 2",
               this->NeedleTypeMenuList->GetWidgetName());

//  this->Script("pack %s %s %s -side left -expand y -padx 2 -pady 2",
//               this->AddTargetsOnClickButton->GetWidgetName(),
//               this->ShowTemplateButton->GetWidgetName(),
//               this->ShowNeedleButton->GetWidgetName());
  //this->Script("grid %s -row 0 -column 0 -columnspan 2 -padx 2 -pady 2 -sticky ew", this->LoadTargetingVolumeButton->GetWidgetName());
  //this->Script("grid %s -row 0 -column 0 -columnspan 2 -padx 2 -pady 2 -sticky ew", this->VolumeSelectorWidget->GetWidgetName());
  //this->Script("grid %s -row 0 -column 0 -columnspan 2 -padx 2 -pady 2 -sticky ew", this->NeedleTypeMenuList->GetWidgetName());
  //this->Script("grid %s -row 0 -column 0 -padx 2 -pady 2", this->AddTargetsOnClickButton->GetWidgetName());
  //this->Script("grid %s -row 0 -column 1 -padx 2 -pady 2", this->ShowWorkspaceButton->GetWidgetName());

  if (!this->ReportWindow)
    {
    this->ReportWindow = vtkProstateNavReportWindow::New();
    this->ReportWindow->SetApplication(this->GetApplication());
    this->ReportWindow->Create();
    }


}


//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ShowTargetListFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->TargetListFrame)
    {
    this->TargetListFrame = vtkKWFrame::New();
    }
  if (!this->TargetListFrame->IsCreated())
    {
    this->TargetListFrame->SetParent(parent);
    this->TargetListFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->TargetListFrame->GetWidgetName());

  if (!this->TargetList)
    {
    this->TargetList = vtkKWMultiColumnListWithScrollbars::New();
    this->TargetList->SetParent(this->TargetListFrame);
    this->TargetList->Create();
    this->TargetList->SetHeight(1);
    this->TargetList->GetWidget()->SetSelectionTypeToRow();
    this->TargetList->GetWidget()->SetSelectionBackgroundColor(1,0,0);
    this->TargetList->GetWidget()->MovableRowsOff();
    this->TargetList->GetWidget()->MovableColumnsOff();

    for (int col = 0; col < COL_COUNT; col ++)
      {
      this->TargetList->GetWidget()->AddColumn(COL_LABELS[col]);
      this->TargetList->GetWidget()->SetColumnWidth(col, COL_WIDTHS[col]);
      this->TargetList->GetWidget()->SetColumnAlignmentToLeft(col);
      //this->TargetList->GetWidget()->ColumnEditableOff(col);
      this->TargetList->GetWidget()->ColumnEditableOn(col);
      this->TargetList->GetWidget()->SetColumnEditWindowToSpinBox(col);
      }

    this->TargetList->GetWidget()->SetColumnEditWindowToCheckButton(0);

    }
  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                this->TargetList->GetWidgetName());


  if(!this->DeleteButton)
    {
    this->DeleteButton = vtkKWPushButton::New();
    }
  if(!this->DeleteButton->IsCreated())
    {
    this->DeleteButton->SetParent(this->TargetListFrame);
    this->DeleteButton->SetText("Delete");
    this->DeleteButton->SetBalloonHelpString("Delete selected target");
    this->DeleteButton->Create();
    }
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 4",
               this->DeleteButton->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ShowTargetControlFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->TargetControlFrame)
    {
    this->TargetControlFrame = vtkKWFrame::New();
    }
  if (!this->TargetControlFrame->IsCreated())
    {
    this->TargetControlFrame->SetParent(parent);
    this->TargetControlFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->TargetControlFrame->GetWidgetName());

  if (!this->NeedlePositionMatrix)
    {
    this->NeedlePositionMatrix = vtkKWMatrixWidgetWithLabel::New();
    this->NeedlePositionMatrix->SetParent(this->TargetControlFrame);
    this->NeedlePositionMatrix->Create();
    this->NeedlePositionMatrix->SetLabelText("Position (X, Y, Z):");
    this->NeedlePositionMatrix->ExpandWidgetOff();
    this->NeedlePositionMatrix->GetLabel()->SetWidth(18);
    this->NeedlePositionMatrix->SetBalloonHelpString("Set the needle position");

    vtkKWMatrixWidget *matrix =  this->NeedlePositionMatrix->GetWidget();
    matrix->SetNumberOfColumns(3);
    matrix->SetNumberOfRows(1);
    matrix->SetElementWidth(12);
    matrix->SetRestrictElementValueToDouble();
    matrix->SetElementChangedCommandTriggerToAnyChange();
    }

  //if (!this->NeedleOrientationMatrix)
  //  {
  //  this->NeedleOrientationMatrix = vtkKWMatrixWidgetWithLabel::New();
  //  this->NeedleOrientationMatrix->SetParent(this->TargetControlFrame);
  //  this->NeedleOrientationMatrix->Create();
  //  this->NeedleOrientationMatrix->SetLabelText("Orientation (W, X, Y, Z):");
  //  this->NeedleOrientationMatrix->ExpandWidgetOff();
  //  this->NeedleOrientationMatrix->GetLabel()->SetWidth(18);
  //  this->NeedleOrientationMatrix->SetBalloonHelpString("Set the needle orientation");
  //
  //  vtkKWMatrixWidget *matrix =  this->NeedleOrientationMatrix->GetWidget();
  //  matrix->SetNumberOfColumns(4);
  //  matrix->SetNumberOfRows(1);
  //  matrix->SetElementWidth(12);
  //  matrix->SetRestrictElementValueToDouble();
  //  matrix->SetElementChangedCommandTriggerToAnyChange();
  //  }

  //this->Script("pack %s %s -side top -anchor nw -expand n -padx 2 -pady 2",
  //             this->NeedlePositionMatrix->GetWidgetName(),
  //             this->NeedleOrientationMatrix->GetWidgetName());
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->NeedlePositionMatrix->GetWidgetName());

  if(!this->Message)
    {
    this->Message = vtkKWText::New();
    }
  if(!this->Message->IsCreated())
    {
    this->Message->SetParent(this->TargetControlFrame);
    this->Message->Create();
    this->Message->SetText("Select needle type, then click on image to add a target");
    this->Message->SetBackgroundColor(0.7, 0.7, 0.95);
    this->Message->SetHeight(6);
    this->Message->SetWrapToWord();
    this->Message->ReadOnlyOn();
    this->Message->SetBorderWidth(2);
    this->Message->SetReliefToGroove();
    this->Message->SetFont("times 12 bold");
    this->Message->SetForegroundColor(0.0, 0.0, 0.0);
    }
  this->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 6",
               this->Message->GetWidgetName());

  if (!this->MoveButton)
    {
    this->MoveButton = vtkKWPushButton::New();
    this->MoveButton->SetParent (this->TargetControlFrame);
    this->MoveButton->Create();
    this->MoveButton->SetText("Set");
    this->MoveButton->SetBalloonHelpString("Move the robot to the position");
    }

  this->Script("pack %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->MoveButton->GetWidgetName());

  if (!this->GenerateListButton)
    {
    this->GenerateListButton = vtkKWPushButton::New();
    this->GenerateListButton->SetParent (this->TargetControlFrame);
    this->GenerateListButton->Create();
    this->GenerateListButton->SetText("Generate List");
    this->GenerateListButton->SetBalloonHelpString("Generate a list of targets");
    }

  this->Script("pack %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->GenerateListButton->GetWidgetName());


}


//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ProcessGUIEvents(vtkObject *caller,
                                                               unsigned long event, void *callData)
{

  // -----------------------------------------------------------------
  // Manager node
  vtkMRMLProstateNavManagerNode *mrmlNode = this->GetGUI()->GetProstateNavManagerNode();

  // -----------------------------------------------------------------
  // Move Button Pressed
  if (this->MoveButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->Logic && this->NeedlePositionMatrix /*&& this->NeedleOrientationMatrix*/)
      {
      float position[3];   // position parameters

      vtkKWMatrixWidget* matrix = this->NeedlePositionMatrix->GetWidget();
      position[0] = (float) matrix->GetElementValueAsDouble(0, 0);
      position[1] = (float) matrix->GetElementValueAsDouble(0, 1);
      position[2] = (float) matrix->GetElementValueAsDouble(0, 2);

      vtkMRMLNode* node = this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->GetNodeByID(this->GetProstateNavManager()->GetRobotNode()->GetTargetTransformNodeID());
      vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(node);

      if (transformNode)
        {
        vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();
        matrix->Identity();

        matrix->SetElement(0, 3, position[0]);
        matrix->SetElement(1, 3, position[1]);
        matrix->SetElement(2, 3, position[2]);

        vtkMatrix4x4* transformToParent = transformNode->GetMatrixTransformToParent();
        transformToParent->DeepCopy(matrix);

        // Send move to command
        this->GetProstateNavManager()->GetRobotNode()->MoveTo(transformNode->GetID());
        this->UpdateGUI();

        }
      }
    }

  if (this->GenerateListButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    vtkMRMLTransPerinealProstateTemplateNode* robotNode =
      vtkMRMLTransPerinealProstateTemplateNode::SafeDownCast(this->GetProstateNavManager()->GetRobotNode());
    vtkProstateNavTargetDescriptor* targetDesc=this->GetProstateNavManager()->GetTargetDescriptorAtIndex(mrmlNode->GetCurrentTargetIndex());
    NeedleDescriptorStruct *needle = mrmlNode->GetNeedle(targetDesc);

    this->ReportWindow->GenerateReport(robotNode, targetDesc, needle);
    this->ReportWindow->DisplayOnWindow();
    return;
    }

  if(!mrmlNode)
    return;

  if (this->DeleteButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    vtkProstateNavTargetDescriptor *targetDesc = mrmlNode->GetTargetDescriptorAtIndex(mrmlNode->GetCurrentTargetIndex());
    if (this->TargetPlanListNode!=NULL && targetDesc!=NULL)
      {
      int fidIndex=this->TargetPlanListNode->GetFiducialIndex(targetDesc->GetFiducialID());
      if (fidIndex>=0)
        {
        this->TargetPlanListNode->RemoveFiducial(fidIndex);
        mrmlNode->SetCurrentTargetIndex(-1);
        UpdateTargetListGUI();
        }
      else
        {
        vtkErrorMacro("Cannot delete target, fiducial not found");
        }
      }
    else
      {
      vtkErrorMacro("Cannot delete target, fiducial or target descriptor is invalid");
      }
    }

  //// load targeting volume dialog button
  //if (this->LoadTargetingVolumeButton && this->LoadTargetingVolumeButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
  //  {
  //  this->GetApplication()->Script("::LoadVolume::ShowDialog");
  //  }

  if (this->ShowTemplateButton && this->ShowTemplateButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
    this->ShowTemplate(this->ShowTemplateButton->GetSelectedState() == 1);
    }

/*
  if (this->ShowNeedleButton && this->ShowNeedleButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
  {
  this->ShowNeedle(this->ShowNeedleButton->GetSelectedState() == 1);
  }
*/

  // activate fiducial placement
  if (this->AddTargetsOnClickButton && this->AddTargetsOnClickButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
    // Activate target fiducials in the Fiducial GUI
    if (this->GetLogic()==NULL)
      {
      vtkErrorMacro("Logic is invalid");
      }
    else
      {
      EnableAddTargetsOnClickButton(this->AddTargetsOnClickButton->GetSelectedState()==1);
      }

    }

  if (this->NeedleTypeMenuList && this->NeedleTypeMenuList->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller) && (event == vtkKWMenu::MenuItemInvokedEvent))
    {
    vtkMRMLTransPerinealProstateCryoTemplateNode* robotNode =
      vtkMRMLTransPerinealProstateCryoTemplateNode::SafeDownCast(mrmlNode->GetRobotNode());

    if (robotNode)
      {
      robotNode->SetNeedleType(this->NeedleTypeMenuList->GetWidget()->GetMenu()->GetIndexOfSelectedItem());

      vtkMRMLFiducialListNode* fidList = mrmlNode->GetTargetPlanListNode();

      int numberOfNeedles = fidList->GetNumberOfFiducials();
      for (int i = 0; i < numberOfNeedles; ++i)
        {
        const char* fiducialID = fidList->GetNthFiducialID(i);
        if (fiducialID)
          {
          const char* needleModelID = fidList->GetAttribute(fiducialID);
          if (needleModelID)
            {
            robotNode->UpdateIceBallModel(needleModelID);
            }
          }
        }
      }
    }

  if (this->TargetListSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
      event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    vtkMRMLFiducialListNode *fid = vtkMRMLFiducialListNode::SafeDownCast(this->TargetListSelectorWidget->GetSelected());
    if (fid != NULL)
      {
      this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&(this->TargetPlanListNode)), NULL, NULL);

      vtkMRMLProstateNavManagerNode* manager=this->GetProstateNavManager();
      manager->SetAndObserveTargetPlanListNodeID(fid->GetID());

      vtkMRMLFiducialListNode* plan = manager->GetTargetPlanListNode();
      if (plan)
        {
        vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
        events->InsertNextValue(vtkCommand::ModifiedEvent);
        events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
        events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
        events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);

        // Set and observe target plan list
        //vtkObject *oldNode = this->TargetPlanListNode;
        this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&(this->TargetPlanListNode)),(plan),(events));
        EnableAddTargetsOnClickButton(this->AddTargetsOnClickButton->GetSelectedState()==1);
        }
      }
    }


  if (this->VolumeSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
      event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    vtkMRMLScalarVolumeNode *volume = vtkMRMLScalarVolumeNode::SafeDownCast(this->VolumeSelectorWidget->GetSelected());
    if (volume != NULL)
      {
      this->GetGUI()->GetLogic()->SelectVolumeInScene(volume, VOL_TARGETING);
      this->AddTargetsOnClickButton->SetSelectedState(1);
      }
    }

  if (this->TargetList->GetWidget() == vtkKWMultiColumnList::SafeDownCast(caller)
      && event == vtkKWMultiColumnList::SelectionChangedEvent)
    {


    }
}


//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ProcessMRMLEvents(vtkObject *caller,
                                                                unsigned long event, void *callData)
{

  vtkMRMLFiducialListNode *targetNode = vtkMRMLFiducialListNode::SafeDownCast(caller);
  if (targetNode!=NULL && targetNode == this->TargetPlanListNode )
    {
    switch (event)
      {
      case vtkCommand::ModifiedEvent:
        break;
      case vtkMRMLScene::NodeAddedEvent: // Node Added Event : when a fiducial is added to the list
        this->NewFiducialAdded();
        UpdateTargetListGUI();
        break;
      case vtkMRMLFiducialListNode::FiducialModifiedEvent:
        break;
      case vtkMRMLFiducialListNode::DisplayModifiedEvent:
        UpdateTargetListGUI();
        break;
      }
    }

  vtkMRMLProstateNavManagerNode *managerNode = vtkMRMLProstateNavManagerNode::SafeDownCast(caller);
  if (managerNode!=NULL && managerNode==GetProstateNavManager())
    {
    switch (event)
      {
      case vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent:
        this->GUI->BringTargetToViewIn2DViews(vtkProstateNavGUI::BRING_MARKERS_TO_VIEW_KEEP_CURRENT_ORIENTATION);
        break;
      }
    }

}

//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::AddMRMLObservers()
{
  vtkMRMLProstateNavManagerNode* manager=this->GetProstateNavManager();
  if (manager==NULL)
    {
    return;
    }

  vtkMRMLFiducialListNode* plan = manager->GetTargetPlanListNode();
  if (plan)
    {
    vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
    events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);

    // Set and observe target plan list
    //vtkObject *oldNode = this->TargetPlanListNode;
    this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&(this->TargetPlanListNode)),(plan),(events));
    }

  manager->AddObserver(vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent, this->MRMLCallbackCommand);
}

//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::RemoveMRMLObservers()
{
  if (this->TargetPlanListNode!=NULL)
    {
    this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&(this->TargetPlanListNode)), NULL, NULL);
    }
  vtkMRMLProstateNavManagerNode* manager=this->GetProstateNavManager();
  if (manager!=NULL)
    {
    manager->RemoveObservers(vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent, this->MRMLCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::OnMultiColumnListUpdate(int row, int col, char * str)
{
  vtkMRMLFiducialListNode* fidList = this->GetProstateNavManager()->GetTargetPlanListNode();

  if (fidList == NULL)
    {
    return;
    }

  bool updated=false;

  // make sure that the row and column exists in the table
  if ((row >= 0) && (row < this->TargetList->GetWidget()->GetNumberOfRows()) &&
      (col >= 0) && (col < this->TargetList->GetWidget()->GetNumberOfColumns()))
    {

    // now update the requested value
    if (col == COL_NAME)
      {
      fidList->SetNthFiducialLabelText(row, str);
      updated=true;
      }
    else if (col >= COL_X && col <= COL_Z)
      {
      // get the current xyz
      float * xyz = fidList->GetNthFiducialXYZ(row);
      // now set the new one
      float newCoordinate = atof(str);
      if ( xyz )
        {
        if (col == COL_X)
          {
          fidList->SetNthFiducialXYZ(row, newCoordinate, xyz[1], xyz[2]);
          updated=true;
          }
        if (col == COL_Y)
          {
          fidList->SetNthFiducialXYZ(row, xyz[0], newCoordinate, xyz[2]);
          updated=true;
          }
        if (col == COL_Z)
          {
          fidList->SetNthFiducialXYZ(row, xyz[0], xyz[1], newCoordinate);
          updated=true;
          }
        }
      }
    else if (col >= COL_OR_W  && col <= COL_OR_Z)
      {
      float * wxyz = fidList->GetNthFiducialOrientation(row);
      float newCoordinate = atof(str);
      if (col == COL_OR_W)
        {
        fidList->SetNthFiducialOrientation(row, newCoordinate, wxyz[1], wxyz[2], wxyz[3]);
        updated=true;
        }
      if (col == COL_OR_X)
        {
        fidList->SetNthFiducialOrientation(row, wxyz[0], newCoordinate, wxyz[2], wxyz[3]);
        updated=true;
        }
      if (col == COL_OR_Y)
        {
        fidList->SetNthFiducialOrientation(row, wxyz[0], wxyz[1], newCoordinate, wxyz[3]);
        updated=true;
        }
      if (col == COL_OR_Z)
        {
        fidList->SetNthFiducialOrientation(row, wxyz[0], wxyz[1], wxyz[2], newCoordinate);
        updated=true;
        }
      }
    else if (col == COL_NEEDLE)
      {
      // Get Needle model connected to fiducial
      const char* fiducialID = fidList->GetNthFiducialID(row);
      if (fiducialID)
        {
        const char* needleModelID = fidList->GetAttribute(fiducialID);
        vtkMRMLModelNode* needleModelNode =
          vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(needleModelID));
        if (needleModelNode)
          {
          std::stringstream ss;
          ss << str;
          int checked;
          ss >> checked;

          vtkMRMLModelDisplayNode* needleDisplayNode = needleModelNode->GetModelDisplayNode();
          if (needleDisplayNode)
            {
            needleDisplayNode->SetVisibility(checked == 1);
            needleDisplayNode->SetSliceIntersectionVisibility(checked == 1);
            }

          // Update iceball visibility
          const char* cryoBallID = needleModelNode->GetAttribute("CryoBallID");
          if (cryoBallID)
            {
            vtkMRMLModelNode* cryoBallModelNode =
              vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(cryoBallID));
            if (cryoBallModelNode)
              {
              vtkMRMLModelDisplayNode* cryoBallModelDisplayNode =
                cryoBallModelNode->GetModelDisplayNode();
              if (cryoBallModelDisplayNode)
                {
                cryoBallModelDisplayNode->SetVisibility(checked == 1);
                cryoBallModelDisplayNode->SetSliceIntersectionVisibility(checked == 1);
                }
              }
            }
          }
        }
      updated=true;
      }
    else
      {
      return;
      }
    }
  else
    {
    }
  if (updated)
    {
    this->GetLogic()->UpdateTargetListFromMRML();
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::OnMultiColumnListSelectionChanged()
{

  vtkMRMLFiducialListNode* fidList = this->GetProstateNavManager()->GetTargetPlanListNode();

  if (fidList == NULL)
    {
    return;
    }

  if (this->MRMLScene)
    {
    this->MRMLScene->SaveStateForUndo();
    }

  int numRows = this->TargetList->GetWidget()->GetNumberOfSelectedRows();
  if (numRows == 1)
    {

    int rowIndex = this->TargetList->GetWidget()->GetIndexOfFirstSelectedRow();
    int targetIndex=this->TargetList->GetWidget()->GetRowAttributeAsInt(rowIndex, TARGET_INDEX_ATTR);
    vtkProstateNavTargetDescriptor* targetDesc=this->GetProstateNavManager()->GetTargetDescriptorAtIndex(targetIndex);

    if (targetDesc==NULL)
      {
      vtkErrorMacro("Target descriptor not found");
      return;
      }

    // Copy the values to inputs
    vtkKWMatrixWidget* matrix = this->NeedlePositionMatrix->GetWidget();
    double* xyz=targetDesc->GetRASLocation();
    matrix->SetElementValueAsDouble(0, 0, xyz[0]);
    matrix->SetElementValueAsDouble(0, 1, xyz[1]);
    matrix->SetElementValueAsDouble(0, 2, xyz[2]);

    // The following code was inherited from the robot code.
    // TODO: For Template-based biopsy, the GUI should be updated, while
    // the ModifiedEvent of ProstateNavManager node is handled.
    if ( this->Logic )
      {
      vtkMRMLNode* node = this->GetLogic()->GetApplicationLogic()->GetMRMLScene()
        ->GetNodeByID(this->GetProstateNavManager()->GetRobotNode()->GetTargetTransformNodeID());
      vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(node);

      if (transformNode)
        {
        vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();
        matrix->Identity();

        matrix->SetElement(0, 3, xyz[0]);
        matrix->SetElement(1, 3, xyz[1]);
        matrix->SetElement(2, 3, xyz[2]);

        vtkMatrix4x4* transformToParent = transformNode->GetMatrixTransformToParent();
        transformToParent->DeepCopy(matrix);

        // Send move to command
        this->GetProstateNavManager()->GetRobotNode()->MoveTo(transformNode->GetID());
        this->UpdateGUI();

        }
      }

    // The following function should be called after calling MoveTo(),
    // because it invokes Modified event that requires the RobotNode
    // to update its target information to update GUI.
    this->GetProstateNavManager()->SetCurrentTargetIndex(targetIndex);
    }
}

//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::UpdateTargetListGUI()
{
  vtkMRMLFiducialListNode* activeFiducialListNode=NULL;
  if (this->GetProstateNavManager()!=NULL)
    {
    activeFiducialListNode=this->GetProstateNavManager()->GetTargetPlanListNode();
    }

  if (activeFiducialListNode == NULL)    //clear out the list box
    {
    if (this->TargetList)
      {
      if (this->TargetList->GetWidget()->GetNumberOfRows() != 0)
        {
        this->TargetList->GetWidget()->DeleteAllRows();
        }
      }
    return;
    }

  // create new target points, if necessary
  this->GetLogic()->UpdateTargetListFromMRML();

  vtkMRMLProstateNavManagerNode *manager = this->GetGUI()->GetProstateNavManagerNode();
  if (!manager)
    {
    return;
    }

  //int numPoints = activeFiducialListNode->GetNumberOfFiducials();
  int numPoints = manager->GetTotalNumberOfTargets();

  bool deleteFlag = true;

  if (this->TargetList && numPoints != this->TargetList->GetWidget()->GetNumberOfRows())
    {
    // clear out the multi column list box and fill it in with the
    // new list
    this->TargetList->GetWidget()->DeleteAllRows();
    }
  else
    {
    deleteFlag = false;
    }

  double *xyz;
  double *wxyz;

  for (int row = 0; row < numPoints; row++)
    {
    int targetIndex=row;
    vtkProstateNavTargetDescriptor* target = manager->GetTargetDescriptorAtIndex(targetIndex);

    if (deleteFlag)
      {
      // add a row for this point
      this->TargetList->GetWidget()->AddRow();
      }
    this->TargetList->GetWidget()->SetRowAttributeAsInt(row, TARGET_INDEX_ATTR, targetIndex);
    this->TargetList->GetWidget()->SetColumnFormatCommandToEmptyOutput(COL_NEEDLE);
    this->TargetList->GetWidget()->SetCellWindowCommandToCheckButton(row, COL_NEEDLE);

    xyz=target->GetRASLocation();
    wxyz=target->GetRASOrientation();

    vtkMRMLFiducialListNode* targetPlanListNode = manager->GetTargetPlanListNode();
    if (targetPlanListNode)
      {
      const char* fiducialID = targetPlanListNode->GetNthFiducialID(row);
      if (fiducialID)
        {
        const char* needleModelID = targetPlanListNode->GetAttribute(fiducialID);
        vtkMRMLModelNode* needleModel =
          vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(needleModelID));
        if (needleModel)
          {
          vtkMRMLModelDisplayNode* needleDisplayNode = needleModel->GetModelDisplayNode();
          if (needleDisplayNode)
            {
            this->TargetList->GetWidget()->GetCellWindowAsCheckButton(row, COL_NEEDLE)
              ->SetSelectedState(needleDisplayNode->GetVisibility());
            }
          }
        }
      }

    if (xyz == NULL)
      {
      vtkErrorMacro ("UpdateTargetListGUI: ERROR: got null xyz for point " << row << endl);
      }

    if (target->GetName().compare(this->TargetList->GetWidget()->GetCellText(row,COL_NAME)) != 0)
      {
      this->TargetList->GetWidget()->SetCellText(row,COL_NAME,target->GetName().c_str());
      }

    // selected
    vtkKWMultiColumnList* columnList = this->TargetList->GetWidget();
    if (xyz != NULL)
      {
      for (int i = 0; i < 3; i ++) // for position (x, y, z)
        {
        if (deleteFlag || columnList->GetCellTextAsDouble(row,COL_X+i) != xyz[i])
          {
          columnList->SetCellTextAsDouble(row,COL_X+i,xyz[i]);
          }
        }
      }
    if (wxyz != NULL)
      {
      for (int i = 0; i < 4; i ++) // for orientation (w, x, y, z)
        {
        if (deleteFlag || columnList->GetCellTextAsDouble(row, COL_OR_W+i) != wxyz[i])
          {
          columnList->SetCellTextAsDouble(row,COL_OR_W+i,wxyz[i]);
          }
        }
      }

    //if (target->GetNeedleTypeString().compare(this->TargetList->GetWidget()->GetCellText(row,COL_NEEDLE)) != 0)
    //{
    //  this->TargetList->GetWidget()->SetCellText(row,COL_NEEDLE,target->GetNeedleTypeString().c_str());
    //}
    //
    }
}

//-----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::AddGUIObservers()
{
  this->RemoveGUIObservers();

  //if (this->LoadTargetingVolumeButton)
  //  {
  //  this->LoadTargetingVolumeButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  //  }
  if (this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget->AddObserver ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetListSelectorWidget)
    {
    this->TargetListSelectorWidget->AddObserver ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ShowTemplateButton)
    {
    this->ShowTemplateButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
/*
  if (this->ShowNeedleButton)
  {
  this->ShowNeedleButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  }
*/
  if (this->AddTargetsOnClickButton)
    {
    this->AddTargetsOnClickButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->DeleteButton)
    {
    this->DeleteButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MoveButton)
    {
    this->MoveButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->GenerateListButton)
    {
    this->GenerateListButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetList)
    {
    this->TargetList->GetWidget()->SetCellUpdatedCommand(this, "OnMultiColumnListUpdate");
    this->TargetList->GetWidget()->SetSelectionChangedCommand(this, "OnMultiColumnListSelectionChanged");
    }
}
//-----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::RemoveGUIObservers()
{
  //if (this->LoadTargetingVolumeButton)
  //  {
  //  this->LoadTargetingVolumeButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  //  }
  if (this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget->RemoveObserver ((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetListSelectorWidget)
    {
    this->TargetListSelectorWidget->RemoveObserver ((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ShowTemplateButton)
    {
    this->ShowTemplateButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
/*
  if (this->ShowNeedleButton)
  {
  this->ShowNeedleButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  }
*/
  if (this->AddTargetsOnClickButton)
    {
    this->AddTargetsOnClickButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->DeleteButton)
    {
    this->DeleteButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MoveButton)
    {
    this->MoveButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->GenerateListButton)
    {
    this->GenerateListButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetList)
    {
    this->TargetList->GetWidget()->SetCellUpdatedCommand(this, "");
    this->TargetList->GetWidget()->SetSelectionChangedCommand(this, "");
    }

  this->RemoveObserver((vtkCommand *)this->GUICallbackCommand);

}

//--------------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::UpdateGUI()
{
  vtkMRMLProstateNavManagerNode *mrmlNode = this->GetGUI()->GetProstateNavManagerNode();

  if (!mrmlNode)
    {
    return;
    }

  const char* volNodeID = mrmlNode->GetTargetingVolumeNodeRef();
  vtkMRMLScalarVolumeNode *volNode=vtkMRMLScalarVolumeNode::SafeDownCast(this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->GetNodeByID(volNodeID));
  if (volNode && this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget->UpdateMenu();
    this->VolumeSelectorWidget->SetSelected( volNode );
    }

  const char* targetNodeID = mrmlNode->GetTargetPlanListNodeID();
  vtkMRMLFiducialListNode *targetNode=vtkMRMLFiducialListNode::SafeDownCast(this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->GetNodeByID(targetNodeID));
  if (targetNode && this->TargetListSelectorWidget)
    {
    this->TargetListSelectorWidget->UpdateMenu();
    this->TargetListSelectorWidget->SetSelected(targetNode);
    }

  // Display information about the currently selected target descriptor
  if (this->Message)
    {
    // NOTE: This part will be rewritten with TargetDescriptor.
    //       Currently TargetDescriptor does not support template index...
    /*
      vtkMRMLRobotNode* robot=NULL;
      if (this->GetProstateNavManager()!=NULL)
      {
      robot=this->GetProstateNavManager()->GetRobotNode();
      }
      vtkProstateNavTargetDescriptor *targetDesc = mrmlNode->GetTargetDescriptorAtIndex(mrmlNode->GetCurrentTargetIndex());

      if (robot!=NULL && targetDesc!=NULL)
      {
      std::string info=robot->GetTargetInfoText(targetDesc);
      this->Message->SetText(info.c_str());
      }
      else
      {
      // no target info available for the current robot with the current target
      this->Message->SetText("");
      }
    */
    if (this->GetProstateNavManager()!=NULL && this->Message)
      {
      vtkProstateNavTargetDescriptor *targetDesc = mrmlNode->GetTargetDescriptorAtIndex(mrmlNode->GetCurrentTargetIndex());
      NeedleDescriptorStruct *needle = mrmlNode->GetNeedle(targetDesc);
      vtkMRMLTransPerinealProstateCryoTemplateNode* robot;
      robot = vtkMRMLTransPerinealProstateCryoTemplateNode::SafeDownCast(this->GetProstateNavManager()->GetRobotNode());
      if (targetDesc && robot)
        {
        std::string info = robot->GetTargetInfoText(targetDesc, needle);
        this->Message->SetText(info.c_str());
        }
      else
        {
        this->Message->SetText("");
        }
      }
    }

  UpdateTargetListGUI();

  if (this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->DeleteAllItems();

    this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddRadioButton("IceSeed");
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddRadioButton("IceRod");

    vtkMRMLTransPerinealProstateCryoTemplateNode* robotNode =
      vtkMRMLTransPerinealProstateCryoTemplateNode::SafeDownCast(mrmlNode->GetRobotNode());
    if (robotNode)
      {
      int needleIndex = robotNode->GetNeedleType();
      this->NeedleTypeMenuList->GetWidget()->GetMenu()->SelectItem(needleIndex);
      }
    }
}

//---------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ShowNeedle(bool show)
{
  vtkMRMLTransPerinealProstateCryoTemplateNode* robotNode =
    vtkMRMLTransPerinealProstateCryoTemplateNode::SafeDownCast(this->GetProstateNavManager()->GetRobotNode());

  if (robotNode == NULL)
    return;

  vtkMRMLModelNode* modelNode = robotNode->GetActiveNeedleModelNode();

  if (modelNode == NULL)
    return;

  vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();

  if (displayNode == NULL)
    return;

  // Show the predicted needle path
  displayNode->SetVisibility(show);
  displayNode->SetSliceIntersectionVisibility(show);
}

//---------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::ShowTemplate(bool show)
{
  vtkMRMLModelNode* modelNode = NULL;
  vtkMRMLModelNode* modelIntersectionNode = NULL;

  vtkMRMLTransPerinealProstateCryoTemplateNode* cryoNode =
    vtkMRMLTransPerinealProstateCryoTemplateNode::SafeDownCast(this->GetProstateNavManager()->GetRobotNode());

  if (cryoNode != NULL)
    {
    modelNode = cryoNode->GetTemplateModelNode();
    modelIntersectionNode = cryoNode->GetTemplateIntersectionModelNode();

    if (modelNode == NULL || modelIntersectionNode == NULL)
      return;
    }

  if (modelNode == NULL || modelIntersectionNode == NULL)
    return;

  vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
  vtkMRMLDisplayNode* displayIntersectionNode = modelIntersectionNode->GetDisplayNode();

  if (displayNode == NULL || displayIntersectionNode == NULL)
    return;

  // Show the predicted needle path
  displayNode->SetVisibility(show);
  displayIntersectionNode->SetSliceIntersectionVisibility(show);
}


//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::HideUserInterface()
{
  RemoveMRMLObservers(); // HideUserInterface deletes the reference to the scene, so RemoveMRMLObservers shall be done before calling HideUserInterface
  RemoveGUIObservers();
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::EnableAddTargetsOnClickButton(bool enable)
{
  if (this->GetProstateNavManager()==NULL)
    {
    return;
    }
  vtkMRMLFiducialListNode* fidNode = this->GetProstateNavManager()->GetTargetPlanListNode();
  GetLogic()->SetCurrentFiducialList(fidNode);
  GetLogic()->SetMouseInteractionMode( (enable) ?
                                       vtkMRMLInteractionNode::Place :
                                       vtkMRMLInteractionNode::ViewTransform
  );
}

//----------------------------------------------------------------------------
void vtkProstateNavStepTargetingCryoTemplate::NewFiducialAdded()
{
  if (!this->GetProstateNavManager())
    {
    return;
    }

  vtkMRMLTransPerinealProstateCryoTemplateNode* cryoNode =
    vtkMRMLTransPerinealProstateCryoTemplateNode::SafeDownCast(this->GetProstateNavManager()->GetRobotNode());
  if (!cryoNode)
    {
    return;
    }

  // Snap fiducial to closest hole
  if (this->TargetPlanListNode)
    {
    float *originalPosition;
    int nearest_i, nearest_j;
    double nearest_depth;
    double errorX, errorY, errorZ;
    vtkMatrix4x4* holeTransform = vtkMatrix4x4::New();

    // Get position of last fiducial added
    int lastFiducialIndex = this->TargetPlanListNode->GetNumberOfFiducials()-1;
    originalPosition = this->TargetPlanListNode->GetNthFiducialXYZ(lastFiducialIndex);
    const char* fiducialID = this->TargetPlanListNode->GetNthFiducialID(lastFiducialIndex);

    // Find closest hole of the target fiducial
    cryoNode->FindHole(originalPosition[0], originalPosition[1], originalPosition[2],
                       nearest_i, nearest_j, nearest_depth,
                       errorX, errorY, errorZ);
    int j_corrected = nearest_j - ((nearest_i%2 == 0 && nearest_j > 7) ? 7 : 8);
    std::stringstream ss;
    ss << this->TargetPlanListNode->GetNthFiducialLabelText(lastFiducialIndex)
       << " (" << (char)('A' + nearest_i) << "," << (j_corrected) << ")";

    this->TargetPlanListNode->SetNthFiducialLabelText(lastFiducialIndex, ss.str().c_str());

    // Get coordinates of the closest hole
    cryoNode->GetHoleTransform(nearest_i, nearest_j, holeTransform);

    // Project distance from hole to fiducial in template direction
    vtkMRMLLinearTransformNode* tnode = cryoNode->GetZFrameTransformNode();
    vtkMatrix4x4* zFrameTransform = NULL;
    if (tnode)
      {
      zFrameTransform = tnode->GetMatrixTransformToParent();
      }

    if (!zFrameTransform)
      {
      return;
      }

    double holeToOriginalPosition[3] = { originalPosition[0]-holeTransform->GetElement(0,3),
                                         originalPosition[1]-holeTransform->GetElement(1,3),
                                         originalPosition[2]-holeTransform->GetElement(2,3)};

    double zFrameDirection[3] = {zFrameTransform->GetElement(0,2),
                                 zFrameTransform->GetElement(1,2),
                                 zFrameTransform->GetElement(2,2)};
    vtkMath::Normalize(zFrameDirection);
    double lenProjection = vtkMath::Dot(holeToOriginalPosition, zFrameDirection);

    double projection[3] = {zFrameDirection[0]*lenProjection,
                            zFrameDirection[1]*lenProjection,
                            zFrameDirection[2]*lenProjection};

    double intermediatePosition[3] = {holeTransform->GetElement(0,3)+projection[0],
                                      holeTransform->GetElement(1,3)+projection[1],
                                      holeTransform->GetElement(2,3)+projection[2]};

    // Find distance between projected point and hole in the slice
    vtkSlicerApplicationGUI* appGUI = GUI->GetApplicationGUI();
    vtkMatrix4x4* sliceToRAS = NULL;
    if (appGUI)
      {
      vtkMRMLSliceNode* redSliceViewer = appGUI->GetMainSliceGUI("Red")->GetLogic()->GetSliceNode();
      if (redSliceViewer)
        {
        sliceToRAS = redSliceViewer->GetSliceToRAS();
        }
      }

    if (!sliceToRAS)
      {
      return;
      }

    double viewerNormalDirection[3] = {sliceToRAS->GetElement(0,2),
                                       sliceToRAS->GetElement(1,2),
                                       sliceToRAS->GetElement(2,2)};

    double dotProduct = std::fabs(vtkMath::Dot(viewerNormalDirection, zFrameDirection));
    double theta = std::acos(dotProduct);

    double error = std::sqrt(std::pow(originalPosition[0]-intermediatePosition[0],2) +
                             std::pow(originalPosition[1]-intermediatePosition[1],2) +
                             std::pow(originalPosition[2]-intermediatePosition[2],2));

    double k[3] = {zFrameDirection[0]*error*std::tan(theta),
                   zFrameDirection[1]*error*std::tan(theta),
                   zFrameDirection[2]*error*std::tan(theta)};

    double snappedPosition1[3] = {intermediatePosition[0]+k[0],
                                  intermediatePosition[1]+k[1],
                                  intermediatePosition[2]+k[2]};
    double snappedPosition2[3] = {intermediatePosition[0]-k[0],
                                  intermediatePosition[1]-k[1],
                                  intermediatePosition[2]-k[2]};

    double testPosition1[3] = {snappedPosition1[0]-originalPosition[0],
                               snappedPosition1[1]-originalPosition[1],
                               snappedPosition1[2]-originalPosition[2]};
    double testPosition2[3] = {snappedPosition2[0]-originalPosition[0],
                               snappedPosition2[1]-originalPosition[1],
                               snappedPosition2[2]-originalPosition[2]};

    vtkMath::Normalize(testPosition1);
    vtkMath::Normalize(testPosition2);

    double testDotProduct1 = std::fabs(vtkMath::Dot(viewerNormalDirection, testPosition1));
    double testDotProduct2 = std::fabs(vtkMath::Dot(viewerNormalDirection, testPosition2));

    if (testDotProduct1 < testDotProduct2)
      {
      this->TargetPlanListNode->SetNthFiducialXYZ(lastFiducialIndex,
                                                  snappedPosition1[0],
                                                  snappedPosition1[1],
                                                  snappedPosition1[2]);
      }
    else
      {
      this->TargetPlanListNode->SetNthFiducialXYZ(lastFiducialIndex,
                                                  snappedPosition2[0],
                                                  snappedPosition2[1],
                                                  snappedPosition2[2]);
      }

    // Add new needle model for this target
    const char* needleID = cryoNode->AddNeedleModel("Needle", 200.0, 4.0);
    if (needleID)
      {
      vtkMRMLModelNode* needleNode =
        vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(needleID));
      if (needleNode)
        {
        const char* cryoBallID = needleNode->GetAttribute("CryoBallID");
        if (cryoBallID)
          {
          vtkMRMLModelNode* cryoBallNode =
            vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(cryoBallID));
          if (cryoBallNode)
            {
            vtkMRMLLinearTransformNode* needleTransform = vtkMRMLLinearTransformNode::New();
            vtkMatrix4x4* needleMatrix = needleTransform->GetMatrixTransformToParent();
            if (needleMatrix)
              {
              float* fiducialPosition = this->TargetPlanListNode->GetNthFiducialXYZ(lastFiducialIndex);
              needleMatrix->DeepCopy(holeTransform);
              needleMatrix->SetElement(0,3, fiducialPosition[0]);
              needleMatrix->SetElement(1,3, fiducialPosition[1]);
              needleMatrix->SetElement(2,3, fiducialPosition[2]);
              needleTransform->Modified();
              this->MRMLScene->AddNode(needleTransform);
              needleNode->SetAndObserveTransformNodeID(needleTransform->GetID());
              needleNode->Modified();
              cryoBallNode->SetAndObserveTransformNodeID(needleTransform->GetID());
              cryoBallNode->Modified();
              }
            needleTransform->Delete();

            if (fiducialID)
              {
              this->TargetPlanListNode->SetAttribute(fiducialID, needleID);
              }
            }
          }
        }
      }
    holeTransform->Delete();
    }
}
