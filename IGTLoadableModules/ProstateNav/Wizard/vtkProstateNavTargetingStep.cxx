/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkProstateNavTargetingStep.h"

#include "vtkProstateNavGUI.h"
#include "vtkProstateNavLogic.h"

#include "vtkKWMatrixWidget.h"
#include "vtkKWMatrixWidgetWithLabel.h"

#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"
#include "vtkMRMLSelectionNode.h"

#include "vtkMRMLLinearTransformNode.h"
#include "igtlMath.h"

#include "vtkMRMLRobotNode.h"

////

#include "vtkProstateNavTargetDescriptor.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkSlicerVolumesGUI.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkKWRadioButton.h"

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
#include "vtkKWCheckButton.h"

const char TARGET_INDEX_ATTR[]="TARGET_IND";

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj) \
    { \
    obj->SetParent(NULL); \
    obj->Delete(); \
    obj = NULL; \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavTargetingStep);
vtkCxxRevisionMacro(vtkProstateNavTargetingStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkProstateNavTargetingStep::vtkProstateNavTargetingStep()
{
  //this->SetName("Targeting");
  this->SetTitle("Targeting");
  this->SetDescription("Set target points.");

  this->MainFrame=NULL;
  
  // TargetPlanning
  this->TargetPlanningFrame=NULL;
  this->LoadTargetingVolumeButton=NULL;
  this->TargetPlanningFrame=NULL;
  this->ShowCoverageButton=NULL;
  this->AddTargetsOnClickButton=NULL;
  this->NeedleTypeMenuList=NULL; 

  // TargetList frame
  this->TargetListFrame=NULL;
  // multi-column list to display target, params, etc
  // this needle will display targets corresponding to the needle type selected
  this->TargetList=NULL;
  this->DeleteButton=NULL;

  // TargetControl frame
  this->TargetControlFrame=NULL;
  this->NeedlePositionMatrix=NULL;
  this->NeedleOrientationMatrix=NULL;
  this->MoveButton=NULL;
  this->StopButton=NULL;

  this->Message=NULL;

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 0.8;

  this->TargetDescriptorIndicesIndexedByListIndex.clear();

  this->ProcessingCallback = false;
  this->LastSelectedTargetDescriptorIndex = -1;
  this->CurrentSelectedTargetDescriptorIndex = -1;



}

//----------------------------------------------------------------------------
vtkProstateNavTargetingStep::~vtkProstateNavTargetingStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  
  // TargetPlanning
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetPlanningFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(LoadTargetingVolumeButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetPlanningFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ShowCoverageButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(AddTargetsOnClickButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(NeedleTypeMenuList); 

  // TargetList frame
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetListFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetList);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(DeleteButton);

  // TargetControl frame
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetControlFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(NeedlePositionMatrix);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(NeedleOrientationMatrix);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(MoveButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(StopButton);

  DELETE_IF_NULL_WITH_SETPARENT_NULL(Message);
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  this->ShowTargetPlanningFrame();
  this->ShowTargetListFrame();
  this->ShowTargetControlFrame();

  UpdateMRMLObserver();
  
  this->AddGUIObservers();
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::ShowTargetPlanningFrame()
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

  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->TargetPlanningFrame->GetWidgetName());
 
 if (!this->LoadTargetingVolumeButton)
    {
     this->LoadTargetingVolumeButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LoadTargetingVolumeButton->IsCreated())
    {
    this->LoadTargetingVolumeButton->SetParent(this->TargetPlanningFrame);
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

  if (!this->ShowCoverageButton)
  {
    this->ShowCoverageButton = vtkKWCheckButton::New();
  } 
  if (!this->ShowCoverageButton->IsCreated()) {
    this->ShowCoverageButton->SetParent(this->TargetPlanningFrame);
    this->ShowCoverageButton->Create();
    this->ShowCoverageButton->SelectedStateOff();
    this->ShowCoverageButton->SetText("Show coverage");
    this->ShowCoverageButton->SetBalloonHelpString("Show coverage volume of the robot");
  }

  if (!this->AddTargetsOnClickButton)
  {
    this->AddTargetsOnClickButton = vtkKWCheckButton::New();
  } 
  if (!this->AddTargetsOnClickButton->IsCreated()) {
    this->AddTargetsOnClickButton->SetParent(this->TargetPlanningFrame);
    this->AddTargetsOnClickButton->Create();
    this->AddTargetsOnClickButton->SelectedStateOff();
    this->AddTargetsOnClickButton->SetText("Add target by image click");
    this->AddTargetsOnClickButton->SetBalloonHelpString("Add a target if image is clicked, with the current needle");
  }

  // add combo box in the frame
  if (!this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList = vtkKWMenuButtonWithLabel::New();
    }
  if (!this->NeedleTypeMenuList->IsCreated())
    {
    this->NeedleTypeMenuList->SetParent(this->TargetPlanningFrame);
    this->NeedleTypeMenuList->Create();
    this->NeedleTypeMenuList->SetLabelText("Needle type");
    this->NeedleTypeMenuList->SetBalloonHelpString("Select the needle type");
    }
  
  this->Script("grid %s -row 0 -column 0 -columnspan 2 -padx 2 -pady 2 -sticky w", this->LoadTargetingVolumeButton->GetWidgetName());
  this->Script("grid %s -row 0 -column 2 -padx 2 -pady 2 -sticky w", this->ShowCoverageButton->GetWidgetName());
  this->Script("grid %s -row 1 -column 0 -padx 2 -pady 2 -sticky w", this->AddTargetsOnClickButton->GetWidgetName());
  this->Script("grid %s -row 1 -column 1 -columnspan 2 -padx 2 -pady 2 -sticky w", this->NeedleTypeMenuList->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::ShowTargetListFrame()
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
    this->TargetList->GetWidget()->MovableRowsOff();
    this->TargetList->GetWidget()->MovableColumnsOff();

    const int COL_COUNT=9;
    const char* labels[COL_COUNT] =
      { "Name", "X", "Y", "Z", "OrW", "OrX", "OrY", "OrZ", "Needle" };
    const int widths[COL_COUNT] = 
      { 8, 6, 6, 6, 6, 6, 6, 6, 8 };

    for (int col = 0; col < COL_COUNT; col ++)
      {
      this->TargetList->GetWidget()->AddColumn(labels[col]);
      this->TargetList->GetWidget()->SetColumnWidth(col, widths[col]);
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
void vtkProstateNavTargetingStep::ShowTargetControlFrame()
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

  if (!this->NeedleOrientationMatrix)
    {
    this->NeedleOrientationMatrix = vtkKWMatrixWidgetWithLabel::New();
    this->NeedleOrientationMatrix->SetParent(this->TargetControlFrame);
    this->NeedleOrientationMatrix->Create();
    this->NeedleOrientationMatrix->SetLabelText("Orientation (W, X, Y, Z):");
    this->NeedleOrientationMatrix->ExpandWidgetOff();
    this->NeedleOrientationMatrix->GetLabel()->SetWidth(18);
    this->NeedleOrientationMatrix->SetBalloonHelpString("Set the needle orientation");

    vtkKWMatrixWidget *matrix =  this->NeedleOrientationMatrix->GetWidget();
    matrix->SetNumberOfColumns(4);
    matrix->SetNumberOfRows(1);
    matrix->SetElementWidth(12);
    matrix->SetRestrictElementValueToDouble();
    matrix->SetElementChangedCommandTriggerToAnyChange();
    }

  this->Script("pack %s %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->NeedlePositionMatrix->GetWidgetName(),
               this->NeedleOrientationMatrix->GetWidgetName());

  if (!this->MoveButton)
    {
    this->MoveButton = vtkKWPushButton::New();
    this->MoveButton->SetParent (this->TargetControlFrame);
    this->MoveButton->Create();
    this->MoveButton->SetText("Move");
    this->MoveButton->SetBalloonHelpString("Move the robot to the position");
    }

  if (!this->StopButton)
    {
    this->StopButton = vtkKWPushButton::New();
    this->StopButton->SetParent (this->TargetControlFrame);
    this->StopButton->Create();
    this->StopButton->SetText("Stop");
    this->StopButton->SetBalloonHelpString("Stop the robot");
    }

  this->Script("pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->MoveButton->GetWidgetName(),
               this->StopButton->GetWidgetName());

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
    //this->Message->SetForegroundColor(0.0, 1.0, 0.0);
    }
  this->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 6", 
                this->Message->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::ProcessGUIEvents(vtkObject *caller,
                                          unsigned long event, void *callData)
{

  // -----------------------------------------------------------------
  // Move Button Pressed

  if (this->MoveButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->Logic && this->NeedlePositionMatrix && this->NeedleOrientationMatrix)
      {
      float position[3];   // position parameters
      float orientation[4]; // orientation parameters

      vtkKWMatrixWidget* matrix = this->NeedlePositionMatrix->GetWidget();
      position[0] = (float) matrix->GetElementValueAsDouble(0, 0);
      position[1] = (float) matrix->GetElementValueAsDouble(0, 1);
      position[2] = (float) matrix->GetElementValueAsDouble(0, 2);

      matrix = this->NeedleOrientationMatrix->GetWidget();
      orientation[0] = (float) matrix->GetElementValueAsDouble(0, 0);
      orientation[1] = (float) matrix->GetElementValueAsDouble(0, 1);
      orientation[2] = (float) matrix->GetElementValueAsDouble(0, 2);
      orientation[3] = (float) matrix->GetElementValueAsDouble(0, 3);

      vtkMRMLNode* node = this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->GetNodeByID(this->GetProstateNavManager()->GetRobotNode()->GetTargetTransformNodeID());
      vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(node);

      if (transformNode)
        {
        vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();
        igtl::Matrix4x4 igtlmatrix;

        igtl::QuaternionToMatrix(orientation, igtlmatrix);
        igtlmatrix[0][3] = position[0];
        igtlmatrix[1][3] = position[1];
        igtlmatrix[2][3] = position[2];
        
        matrix->SetElement(0, 0, igtlmatrix[0][0]);
        matrix->SetElement(1, 0, igtlmatrix[1][0]);
        matrix->SetElement(2, 0, igtlmatrix[2][0]);
        matrix->SetElement(3, 0, igtlmatrix[3][0]);

        matrix->SetElement(0, 1, igtlmatrix[0][1]);
        matrix->SetElement(1, 1, igtlmatrix[1][1]);
        matrix->SetElement(2, 1, igtlmatrix[2][1]);
        matrix->SetElement(3, 1, igtlmatrix[3][1]);

        matrix->SetElement(0, 2, igtlmatrix[0][2]);
        matrix->SetElement(1, 2, igtlmatrix[1][2]);
        matrix->SetElement(2, 2, igtlmatrix[2][2]);
        matrix->SetElement(3, 2, igtlmatrix[3][2]);

        matrix->SetElement(0, 3, igtlmatrix[0][3]);
        matrix->SetElement(1, 3, igtlmatrix[1][3]);
        matrix->SetElement(2, 3, igtlmatrix[2][3]);
        matrix->SetElement(3, 3, igtlmatrix[3][3]);
        

        std::cerr << "TARGETPOISITION = "
                  << igtlmatrix[0][3] << ", "
                  << igtlmatrix[1][3] << ", "
                  << igtlmatrix[2][3] << std::endl;

        vtkMatrix4x4* transformToParent = transformNode->GetMatrixTransformToParent();
        transformToParent->DeepCopy(matrix);

        // Send move to command 
        this->GetProstateNavManager()->GetRobotNode()->MoveTo(transformNode->GetID());

        }
      }
    }

  // -----------------------------------------------------------------
  // Stop Button Pressed

  else if (this->StopButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {

    }

  /////////

  vtkMRMLProstateNavManagerNode *mrmlNode = this->GetGUI()->GetProstateNavManager();

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

  // show coverage dialog button
   if (this->ShowCoverageButton && this->ShowCoverageButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
      this->ShowCoverage(this->ShowCoverageButton->GetSelectedState() == 1);
    }

 // activate fiducial placement
 if (this->AddTargetsOnClickButton && this->AddTargetsOnClickButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
  {
    SetTargetAsCurrentFiducialList();
    if (this->AddTargetsOnClickButton->GetSelectedState() == 1)
    {
      SetMouseInteractionMode(vtkMRMLInteractionNode::Place);
    }
    else
    {
      SetMouseInteractionMode(vtkMRMLInteractionNode::ViewTransform);
    }
  }

  if (this->NeedleTypeMenuList && this->NeedleTypeMenuList->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller) && (event == vtkKWMenu::MenuItemInvokedEvent))
    {
      mrmlNode->SetCurrentNeedleIndex(this->NeedleTypeMenuList->GetWidget()->GetMenu()->GetIndexOfSelectedItem());
    }

}


//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::ProcessMRMLEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  vtkMRMLFiducialListNode *node = vtkMRMLFiducialListNode::SafeDownCast(caller);
  if (!node)
    {
    return;
    }

  // -----------------------------------------------------------------
  // Modified Event

  if (node == this->GetProstateNavManager()->GetTargetPlanListNode()
      && event == vtkCommand::ModifiedEvent)
    {
    SetGUIFromList(this->GetProstateNavManager()->GetTargetPlanListNode());
    }

  // -----------------------------------------------------------------
  // Node Added Event : when a fiducial is added to the list

  if (node == this->GetProstateNavManager()->GetTargetPlanListNode()
      && event == vtkMRMLScene::NodeAddedEvent)
    {
    SetGUIFromList(this->GetProstateNavManager()->GetTargetPlanListNode());
    }

  // -----------------------------------------------------------------
  // Fiducial Modified Event

  else if (node == this->GetProstateNavManager()->GetTargetPlanListNode()
           && event == vtkMRMLFiducialListNode::FiducialModifiedEvent)
    {
    SetGUIFromList(this->GetProstateNavManager()->GetTargetPlanListNode());
    }
  
  // -----------------------------------------------------------------
  // Display Modified Event
  
  else if (node == this->GetProstateNavManager()->GetTargetPlanListNode()
           && event == vtkMRMLFiducialListNode::DisplayModifiedEvent)
    {
    SetGUIFromList(this->GetProstateNavManager()->GetTargetPlanListNode());
    }
  
}


//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::UpdateMRMLObserver()
{
  if (this->GetProstateNavManager())
  {
    vtkMRMLFiducialListNode* plan      = this->GetProstateNavManager()->GetTargetPlanListNode();
    if (plan)
    {
      vtkIntArray *events = vtkIntArray::New();
      events->InsertNextValue(vtkCommand::ModifiedEvent);
      events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
      events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
      events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);

      // Set and observe target plan list
      vtkObject *oldNode = (plan);
      this->MRMLObserverManager
        ->SetAndObserveObjectEvents(vtkObjectPointer(&(plan)),(plan),(events));
      if ( oldNode != (plan) )
      {
        this->GetProstateNavManager()->SetAndObserveTargetPlanListNodeID(plan->GetID());
        this->InvokeEvent (vtkCommand::ModifiedEvent);
      } 

      events->Delete();
    }
  }

}


//---------------------------------------------------------------------------
void vtkProstateNavTargetingStep::OnMultiColumnListUpdate(int row, int col, char * str)
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
    if (col == 0)
      {
      fidList->SetNthFiducialLabelText(row, str);
      updated=true;
      }
    else if (col >= 1 && col <= 3)
      {
      // get the current xyz
      float * xyz = fidList->GetNthFiducialXYZ(row);
      // now set the new one
      float newCoordinate = atof(str);
      if ( xyz )
        {
        if (col == 1)
          {
          fidList->SetNthFiducialXYZ(row, newCoordinate, xyz[1], xyz[2]);
          updated=true;
          }
        if (col == 2)
          {
          fidList->SetNthFiducialXYZ(row, xyz[0], newCoordinate, xyz[2]);
          updated=true;
          }
        if (col == 3)
          {
          fidList->SetNthFiducialXYZ(row, xyz[0], xyz[1], newCoordinate);
          updated=true;
          }
        }            
      }
    else if (col >= 4  && col <= 7)
      {
      float * wxyz = fidList->GetNthFiducialOrientation(row);
      float newCoordinate = atof(str);
      if (col == 4)
        {
        fidList->SetNthFiducialOrientation(row, newCoordinate, wxyz[1], wxyz[2], wxyz[3]);
        updated=true;
        }
      if (col == 5)
        {
        fidList->SetNthFiducialOrientation(row, wxyz[0], newCoordinate, wxyz[2], wxyz[3]);
        updated=true;
        }
      if (col == 6)
        {
        fidList->SetNthFiducialOrientation(row, wxyz[0], wxyz[1], newCoordinate, wxyz[3]);
        updated=true;
        }
      if (col == 7)
        {
        fidList->SetNthFiducialOrientation(row, wxyz[0], wxyz[1], wxyz[2], newCoordinate);
        updated=true;
        }
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
void vtkProstateNavTargetingStep::OnMultiColumnListSelectionChanged()
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

    // Copy the values to inputs
    vtkKWMatrixWidget* matrix = this->NeedlePositionMatrix->GetWidget();
    double* xyz=targetDesc->GetRASLocation();
    matrix->SetElementValueAsDouble(0, 0, xyz[0]);
    matrix->SetElementValueAsDouble(0, 1, xyz[1]);
    matrix->SetElementValueAsDouble(0, 2, xyz[2]);

    matrix = this->NeedleOrientationMatrix->GetWidget();
    double* wxyz=targetDesc->GetRASOrientation();
    matrix->SetElementValueAsDouble(0, 0, wxyz[0]);
    matrix->SetElementValueAsDouble(0, 1, wxyz[1]);
    matrix->SetElementValueAsDouble(0, 2, wxyz[2]);
    matrix->SetElementValueAsDouble(0, 3, wxyz[3]);
      
    // use row index to get information about the target
    this->CurrentSelectedTargetDescriptorIndex = rowIndex;
    this->LastSelectedTargetDescriptorIndex = this->CurrentSelectedTargetDescriptorIndex;   
    
    // things to do on selection  
    int oldModify=this->GetGUI()->GetProstateNavManager()->StartModify();
    this->GetGUI()->GetProstateNavManager()->SetCurrentTargetIndex(this->CurrentSelectedTargetDescriptorIndex);
    // bring target to view in all three views
    this->BringTargetToViewIn2DViews();

    this->GetProstateNavManager()->SetCurrentTargetIndex(targetIndex);

    UpdateCurrentTargetDisplay();

    this->GetGUI()->GetProstateNavManager()->EndModify(oldModify);
    }
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::SetGUIFromList(vtkMRMLFiducialListNode * activeFiducialListNode)
{

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

  vtkMRMLProstateNavManagerNode *manager = this->GetGUI()->GetProstateNavManager();
  if (!manager)
  {
    return;
  }

  //int numPoints = activeFiducialListNode->GetNumberOfFiducials();
  int numPoints = manager->GetTotalNumberOfTargets();

  bool deleteFlag = true;

  if (numPoints != this->TargetList->GetWidget()->GetNumberOfRows())
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

    xyz=target->GetRASLocation();
    wxyz=target->GetRASOrientation();

    if (xyz == NULL)
      {
      vtkErrorMacro ("SetGUIFromList: ERROR: got null xyz for point " << row << endl);
      }

    if (target->GetName().compare(this->TargetList->GetWidget()->GetCellText(row,0)) != 0)
        {
          this->TargetList->GetWidget()->SetCellText(row,0,target->GetName().c_str());
        }               

    // selected
    vtkKWMultiColumnList* columnList = this->TargetList->GetWidget();
    if (xyz != NULL)
      {
      for (int i = 0; i < 3; i ++) // for position (x, y, z)
        {
        if (deleteFlag || columnList->GetCellTextAsDouble(row,1+i) != xyz[i])
          {
          columnList->SetCellTextAsDouble(row,1+i,xyz[i]);
          }
        }
      }
    if (wxyz != NULL)
      {
      for (int i = 0; i < 4; i ++) // for orientation (w, x, y, z)
        {
        if (deleteFlag || columnList->GetCellTextAsDouble(row, 4+i) != wxyz[i])
          {
          columnList->SetCellTextAsDouble(row,4+i,wxyz[i]);
          }
        }
      }

    if (target->GetNeedleTypeString().compare(this->TargetList->GetWidget()->GetCellText(row,8)) != 0)
    {
      this->TargetList->GetWidget()->SetCellText(row,8,target->GetNeedleTypeString().c_str());
    }

    }

  UpdateCurrentTargetDisplay(); // if a new node is added then it is selected by default => keep only the current target as selected

  vtkDebugMacro("Now going to update GUI from the logic's active list");

}



//-----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

    // 1) click on load targeting volume dialog
  if (this->LoadTargetingVolumeButton)
    {
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->ShowCoverageButton)
    {
      this->ShowCoverageButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->AddTargetsOnClickButton)
    {
      this->AddTargetsOnClickButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MoveButton)
    {
    this->MoveButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->StopButton)
    {
    this->StopButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetList)
    {
    this->TargetList->GetWidget()->SetCellUpdatedCommand(this, "OnMultiColumnListUpdate");
    this->TargetList->GetWidget()->SetSelectionChangedCommand(this, "OnMultiColumnListSelectionChanged");
    }
}
//-----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::RemoveGUIObservers()
{
  if (this->LoadTargetingVolumeButton)
    {
    this->LoadTargetingVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->ShowCoverageButton)
    {
    this->ShowCoverageButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->AddTargetsOnClickButton)
    {
      this->AddTargetsOnClickButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->NeedleTypeMenuList)
    {
      this->NeedleTypeMenuList->GetWidget()->GetMenu()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MoveButton)
    {
    this->MoveButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->StopButton)
    {
    this->StopButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetList)
    {
    this->TargetList->GetWidget()->SetCellUpdatedCommand(this, "");
    this->TargetList->GetWidget()->SetSelectionChangedCommand(this, "");
    }
}

//-----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::LoadTargetingVolumeButtonCallback(const char *fileName)
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

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddVolumeToScene(fileString.c_str(), VOL_TARGETING);
        
  if (volumeNode)
    {
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();
    }
  else 
    {
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
    dialog->SetParent ( this->TargetPlanningFrame );
    dialog->SetStyleToMessage();
    std::string msg = std::string("Unable to read volume file ") + std::string(fileName);
    dialog->SetText(msg.c_str());
    dialog->Create ( );
    dialog->Invoke();
    dialog->Delete();
    }
     
}

//--------------------------------------------------------------------------------
void vtkProstateNavTargetingStep::UpdateGUI()
{
  vtkMRMLProstateNavManagerNode *mrmlNode = this->GetGUI()->GetProstateNavManager();

  if (!mrmlNode)
  {
    return;
  }

  if (this->Message)
  {
    // get the information about the currently selected target descriptor
    int targetIndex=mrmlNode->GetCurrentTargetIndex();
    vtkProstateNavTargetDescriptor *targetDesc = mrmlNode->GetTargetDescriptorAtIndex(targetIndex); 
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

  SetGUIFromList(mrmlNode->GetTargetPlanListNode());

  if (this->NeedleTypeMenuList)
    {
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->DeleteAllItems();
    for (int i = 0; i < mrmlNode->GetNumberOfNeedles(); i++)
      {
      std::string needle = mrmlNode->GetNeedleType(i);
      this->NeedleTypeMenuList->GetWidget()->GetMenu()->AddRadioButton(needle.c_str());
      }
    //this->NeedleTypeMenuList->GetWidget()->SetValue(this->GetGUI()->GetProstateNavManager()->GetNeedleType(0).c_str());
    int needleIndex=mrmlNode->GetCurrentNeedleIndex();
    this->NeedleTypeMenuList->GetWidget()->GetMenu()->SelectItem(needleIndex);
    }
}
//--------------------------------------------------------------------------------
void vtkProstateNavTargetingStep::BringTargetToViewIn2DViews()
{
  if (this->CurrentSelectedTargetDescriptorIndex != -1)
    {
    // set up the three 2D views

    // the slices may not be really orthogonal, they could be oblique
    // we could directly call slice node -> JumpAllSlices (r, a, s), this brings target in view
    // in all slices, but with target fiducial at the center of the view, moving (disturbing) the image altogether
    // for this function ->JumpSliceByOffsetting does the job
    
    // get the point ras location of the target fiducial (P) that lies on the image plane
    vtkProstateNavTargetDescriptor *targetDesc = this->GetGUI()->GetProstateNavManager()->GetTargetDescriptorAtIndex(this->CurrentSelectedTargetDescriptorIndex);    
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
// return:
//  0=error
//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::ShowCoverage(bool show) 
{
  // :TODO: show/hide depending on show parameter value

  vtkProstateNavLogic *logic=this->GetGUI()->GetLogic();
  if (!logic)
  {
    vtkErrorMacro("Invalid logic object");
    return;
  }
  logic->ShowCoverage(show);
}

void vtkProstateNavTargetingStep::UpdateCurrentTargetDisplay()
{
  vtkMRMLProstateNavManagerNode *manager= this->GetGUI()->GetProstateNavManager();
  if(manager==NULL)
  {
    return;
  }

  vtkMRMLFiducialListNode* fidList = this->GetProstateNavManager()->GetTargetPlanListNode();
  if(fidList==NULL)
  {
    return;
  }

  int currentTargetInd=manager->GetCurrentTargetIndex();
  std::string fidID="INVALID";
  if (currentTargetInd>=0)
  {
    vtkProstateNavTargetDescriptor* targetDesc=manager->GetTargetDescriptorAtIndex(currentTargetInd);
    if (targetDesc!=NULL)
    {
      fidID=targetDesc->GetFiducialID();
    }
  }

  int oldModify=fidList->StartModify();
  for (int i = 0; i < fidList->GetNumberOfFiducials(); i ++)
  {              
    // select only the active target
    fidList->SetNthFiducialSelected(i, fidID.compare(fidList->GetNthFiducialID(i))==0);
  }
  fidList->EndModify(oldModify);
  // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
  fidList->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::HideUserInterface()
{
  Superclass::HideUserInterface();
  RemoveGUIObservers();
  if (this->GetProstateNavManager())
    {
    vtkMRMLFiducialListNode* plan = this->GetProstateNavManager()->GetTargetPlanListNode();
    if (plan)
      {
      //this->MRMLObserverManager->RemoveObjectEvents(vtkObjectPointer(&(plan)));
      this->MRMLObserverManager->RemoveObjectEvents(plan);
      }
    }
  
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetingStep::SetMouseInteractionMode(int mode)
{
  if (this->GetLogic())
  {
    if (this->GetLogic()->GetApplicationLogic())
    {
      if (this->GetLogic()->GetApplicationLogic()->GetMRMLScene())
      {
        vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLInteractionNode"));
        if (interactionNode!=NULL)
        {
        interactionNode->SetCurrentInteractionMode(mode); 
        } 
      }
    }
  }
  vtkSlicerApplication* app=vtkSlicerApplication::SafeDownCast(this->GetApplication());
  if (app)
  {
    vtkSlicerApplicationGUI* appGUI = app->GetApplicationGUI();
    if ( appGUI )
    {
      vtkSlicerToolbarGUI *tGUI = appGUI->GetApplicationToolbar();
      if ( tGUI )
      {
        switch (mode)
        {
        case vtkMRMLInteractionNode::Place:
          if (tGUI->GetMousePlaceButton())
          {
            tGUI->GetMousePlaceButton()->SelectedStateOn();
          }
          break;
        case vtkMRMLInteractionNode::ViewTransform:
          if (tGUI->GetMouseTransformViewButton())
          {
            tGUI->GetMouseTransformViewButton()->SelectedStateOn();
          }
          break;
        case vtkMRMLInteractionNode::PickManipulate:
          if (tGUI->GetMousePickButton())
          {
            tGUI->GetMousePickButton()->SelectedStateOn();
          }
          break;
        }        
      }
    }
  } 
}

void vtkProstateNavTargetingStep::SetTargetAsCurrentFiducialList()
{
  // Activate target fiducials in the Fiducial GUI
  if (this->GetProstateNavManager() && this->GetGUI())
  {
    vtkSlicerApplication *app = (vtkSlicerApplication *)(this->GetGUI()->GetApplication());
    vtkSlicerFiducialsGUI* fidGUI = vtkSlicerFiducialsGUI::SafeDownCast ( app->GetModuleGUIByName ("Fiducials"));
    if (fidGUI)
    {
      vtkMRMLFiducialListNode* fidNode = this->GetProstateNavManager()->GetTargetPlanListNode();
      if (fidNode)
      {
        fidGUI->Enter();
        fidGUI->SetFiducialListNodeID (fidNode->GetID());
      }
    }
  }
}
