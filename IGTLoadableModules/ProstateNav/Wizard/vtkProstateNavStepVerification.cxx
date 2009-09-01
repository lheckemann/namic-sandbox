/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkProstateNavStepVerification.h"

#include "vtkProstateNavGUI.h"
#include "vtkProstateNavLogic.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWPushButton.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"
#include "vtkMRMLSelectionNode.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavStepVerification);
vtkCxxRevisionMacro(vtkProstateNavStepVerification, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkProstateNavStepVerification::vtkProstateNavStepVerification()
{
  this->SetTitle("Verification");
  this->SetDescription("Verify targeting results.");

  this->TargetListFrame  = NULL;
  this->MultiColumnList = NULL;
  this->TargetControlFrame = NULL; 
  this->AddButton        = NULL;
  this->RemoveButton     = NULL;
  this->RemoveAllButton  = NULL;

}

//----------------------------------------------------------------------------
vtkProstateNavStepVerification::~vtkProstateNavStepVerification()
{
  if (this->TargetListFrame)
    {
    this->TargetListFrame->SetParent(NULL);
    this->TargetListFrame->Delete();
    this->TargetListFrame = NULL;
    }
  if (this->MultiColumnList)
    {
    this->MultiColumnList->SetParent(NULL);
    this->MultiColumnList->Delete();
    this->MultiColumnList = NULL;
    }
  if (this->TargetControlFrame)
    {
    this->TargetControlFrame->SetParent(NULL);
    this->TargetControlFrame->Delete();
    this->TargetControlFrame = NULL;
    }
  if (this->AddButton)
    {
    this->AddButton->SetParent(NULL);
    this->AddButton->Delete();
    this->AddButton = NULL;
    }
  if (this->RemoveButton)
    {
    this->RemoveButton->SetParent(NULL);
    this->RemoveButton->Delete();
    this->RemoveButton = NULL;
    }
  if (this->RemoveAllButton)
    {
    this->RemoveAllButton->SetParent(NULL);
    this->RemoveAllButton->Delete();
    this->RemoveAllButton = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget *parent = wizardWidget->GetClientArea();


  // -----------------------------------------------------------------
  // Target List Frame

  if (!this->TargetListFrame)
    {
    this->TargetListFrame = vtkKWFrame::New();
    this->TargetListFrame->SetParent(parent);
    this->TargetListFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                this->TargetListFrame->GetWidgetName());

  if (!this->MultiColumnList)
    {
    // add the multicolumn list to show the points
    this->MultiColumnList = vtkKWMultiColumnListWithScrollbars::New();
    this->MultiColumnList->SetParent(TargetListFrame);
    this->MultiColumnList->Create();
    this->MultiColumnList->SetHeight(4);
    //this->MultiColumnList->GetWidget()->SetSelectionTypeToCell();
    this->MultiColumnList->GetWidget()->SetSelectionTypeToRow();
    this->MultiColumnList->GetWidget()->MovableRowsOff();
    this->MultiColumnList->GetWidget()->MovableColumnsOff();

    const char* labels[] =
      { "Name", "X", "Y", "Z", "OrW", "OrX", "OrY", "OrZ" };
    const int widths[] = 
      { 8, 6, 6, 6, 6, 6, 6, 6 };

    for (int col = 0; col < 8; col ++)
      {
      this->MultiColumnList->GetWidget()->AddColumn(labels[col]);
      this->MultiColumnList->GetWidget()->SetColumnWidth(col, widths[col]);
      this->MultiColumnList->GetWidget()->SetColumnAlignmentToLeft(col);
      //this->MultiColumnList->GetWidget()->ColumnEditableOff(col);
      this->MultiColumnList->GetWidget()->ColumnEditableOn(col);
      this->MultiColumnList->GetWidget()->SetColumnEditWindowToSpinBox(col);
      }
    // make the Name column editable by checkbutton
    this->MultiColumnList->GetWidget()->SetColumnEditWindowToCheckButton(0);
    this->MultiColumnList->GetWidget()->SetCellUpdatedCommand(this, "OnMultiColumnListUpdate");
    this->MultiColumnList->GetWidget()->SetSelectionChangedCommand(this, "OnMultiColumnListSelectionChanged");
    }

  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                this->MultiColumnList->GetWidgetName());


  // -----------------------------------------------------------------
  // Target Control Frame

  if (!this->TargetControlFrame)
    {
    this->TargetControlFrame = vtkKWFrame::New();
    this->TargetControlFrame->SetParent(TargetListFrame);
    this->TargetControlFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->TargetControlFrame->GetWidgetName());

  if (!this->AddButton)
    {
    this->AddButton = vtkKWPushButton::New();
    this->AddButton->SetParent (this->TargetControlFrame);
    this->AddButton->Create();
    this->AddButton->SetText("Add Target");
    this->AddButton->SetBalloonHelpString("Move the robot to the position");
    this->AddButton->AddObserver(vtkKWPushButton::InvokedEvent,
                                  (vtkCommand *)this->GUICallbackCommand);
    }

  if (!this->RemoveButton)
    {
    this->RemoveButton = vtkKWPushButton::New();
    this->RemoveButton->SetParent (this->TargetControlFrame);
    this->RemoveButton->Create();
    this->RemoveButton->SetText("Remove Target");
    this->RemoveButton->SetBalloonHelpString("Move the robot to the position");
    this->RemoveButton->AddObserver(vtkKWPushButton::InvokedEvent,
                                  (vtkCommand *)this->GUICallbackCommand);
    }

  if (!this->RemoveAllButton)
    {
    this->RemoveAllButton = vtkKWPushButton::New();
    this->RemoveAllButton->SetParent (this->TargetControlFrame);
    this->RemoveAllButton->Create();
    this->RemoveAllButton->SetText("Remove All");
    this->RemoveAllButton->SetBalloonHelpString("Move the robot to the position");
    this->RemoveAllButton->AddObserver(vtkKWPushButton::InvokedEvent,
                                  (vtkCommand *)this->GUICallbackCommand);
    }

  this->Script("pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->AddButton->GetWidgetName(),
               this->RemoveButton->GetWidgetName(),
               this->RemoveAllButton->GetWidgetName());


}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{


}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ProcessMRMLEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{
  if (!this->GetGUI())
    {
    return;
    }

  vtkMRMLSelectionNode *selnode;
  if (this->GetGUI()->GetApplicationLogic())
    {
    selnode = this->GetGUI()->GetApplicationLogic()->GetSelectionNode();
    }

  // -----------------------------------------------------------------
  // Fiducial Modified

  if (event == vtkCommand::WidgetValueChangedEvent)
    {
    vtkDebugMacro("got a widget value changed event... the list node was changed.\n");
    }

  // -----------------------------------------------------------------
  // Modified Event

  // -----------------------------------------------------------------
  // Fiducial Modified Event

 
  // -----------------------------------------------------------------
  // Display Modified Event


}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::UpdateMRMLObserver(vtkMRMLSelectionNode* selnode)
{

}


//---------------------------------------------------------------------------
void vtkProstateNavStepVerification::OnMultiColumnListUpdate(int row, int col, char * str)
{

}


//---------------------------------------------------------------------------
void vtkProstateNavStepVerification::OnMultiColumnListSelectionChanged()
{

}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::SetGUIFromList(vtkMRMLFiducialListNode * activeFiducialListNode)
{

}
