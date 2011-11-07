/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFGUI.h $
  Date:      $Date: 2010-04-01 11:42:15 -0400 (Thu, 01 Apr 2010) $
  Version:   $Revision: 12582 $

==========================================================================*/


#include "vtkObjectFactory.h"

#include "vtkProstateNavNeedleOrientationWindow.h"

#include "vtkSlicerApplication.h"

#include "vtkKWFrameWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWText.h"
#include "vtkKWTextWithScrollbars.h"

#include "igtlOSUtil.h"
#include "igtlMath.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"

#include "vtkMRMLTransPerinealProstateTemplateNode.h"
#include "vtkMRMLProstateNavManagerNode.h"
#include "vtkProstateNavTargetDescriptor.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"


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
vtkStandardNewMacro(vtkProstateNavNeedleOrientationWindow);
vtkCxxRevisionMacro(vtkProstateNavNeedleOrientationWindow, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
vtkProstateNavNeedleOrientationWindow::vtkProstateNavNeedleOrientationWindow()
{
  
  // Logic values
  this->WindowPosition[0]=0;
  this->WindowPosition[1]=0;
  this->WindowSize[0]=600;
  this->WindowSize[1]=400;

  // GUI widgets
  this->MainFrame = vtkKWFrame::New();
  this->MultipleMonitorsAvailable = false; 

  this->StartTrackingButton  = NULL;
  this->StopTrackingButton = NULL;
  this->CloseButton    = NULL;

  this->EntryListSelectorWidget=NULL;
  this->EntryList=NULL;

  // GUI callback command
  this->InGUICallbackFlag = 0;
  this->IsObserverAddedFlag = 0;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkProstateNavNeedleOrientationWindow::GUICallback);

}


//----------------------------------------------------------------------------
vtkProstateNavNeedleOrientationWindow::~vtkProstateNavNeedleOrientationWindow()
{

  if ( this->GUICallbackCommand != NULL )
    {
    this->GUICallbackCommand->Delete ( );
    this->GUICallbackCommand = NULL;
    }

  if (this->StartTrackingButton)
    {
    this->StartTrackingButton->SetParent(NULL);
    this->StartTrackingButton->Delete();
    }
  if (this->StopTrackingButton)
    {
    this->StopTrackingButton->SetParent(NULL);
    this->StopTrackingButton->Delete();
    }
  if (this->CloseButton)
    {
    this->CloseButton->SetParent(NULL);
    this->CloseButton->Delete();
    }

  this->MainFrame->Delete();
  this->SetApplication(NULL);
  
  if (this->EntryListSelectorWidget)
    {
    this->EntryListSelectorWidget->SetParent(NULL);
    this->EntryListSelectorWidget->Delete();
    }
  if (this->EntryList)
    {
    this->EntryList->SetParent(NULL);
    this->EntryList->Delete();
    }

}


//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{
  
  vtkProstateNavNeedleOrientationWindow *self = reinterpret_cast<vtkProstateNavNeedleOrientationWindow *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    }

  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
  
}

//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{

  if (this->EntryListSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    vtkMRMLFiducialListNode *fid = vtkMRMLFiducialListNode::SafeDownCast(this->EntryListSelectorWidget->GetSelected());
    if (fid != NULL)
      {
      UpdateEntryList();
      }
    }
  else if (this->StartTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    }
  else if (this->StopTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    }
  else if (this->CloseButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    this->Withdraw();
    }

}


//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::ProcessTimerEvents()
{
  // NOTE: this function has to be called by an external timer event handler
  // e.g. vtkOpenIGTLinkIFGUI::ProcessTimerEvents();
  
}


//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if (this->StopTrackingButton)
    {
    this->StopTrackingButton->AddObserver(vtkKWPushButton::InvokedEvent,(vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CloseButton)
    {
    this->CloseButton->AddObserver(vtkKWPushButton::InvokedEvent,(vtkCommand *)this->GUICallbackCommand);
    }
  if (this->EntryListSelectorWidget)
    {
    this->EntryListSelectorWidget->AddObserver ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);  
    }
  if (this->EntryList)
    {
    this->EntryList->GetWidget()->SetCellUpdatedCommand(this, "OnMultiColumnListUpdate");
    this->EntryList->GetWidget()->SetSelectionChangedCommand(this, "OnMultiColumnListSelectionChanged");
    }

}


//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::RemoveGUIObservers()
{
  if (this->StopTrackingButton)
    {
    this->StopTrackingButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CloseButton)
    {
    this->CloseButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->EntryListSelectorWidget)
    {
    this->EntryListSelectorWidget->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->EntryList)
    {
    // ??
    }
}



//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::CreateWidget()
{
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  if (app==NULL)
  {
  vtkErrorMacro("CreateWindow: application is invalid");
  return;
  }

  if (this->IsCreated())
  {
    return;
  }


  vtkKWTopLevel::CreateWidget();
  this->SetApplication ( app );
  //this->SetBorderWidth ( 1 );
  //this->SetReliefToFlat();
  //this->SetParent (this->GetApplicationGUI()->GetMainSlicerWindow());

  this->SetTitle ("NeedleOrientation");
  //this->SetSize (400, 100);
  this->Withdraw();

  this->MainFrame->SetParent ( this );

  this->MainFrame->Create();
  this->MainFrame->SetBorderWidth ( 1 );
  this->Script ( "pack %s -side top -anchor nw -fill both -expand 1 -padx 0 -pady 1", this->MainFrame->GetWidgetName() ); 

  // --------------------------------------------------
  // Tracking Test Data Source (Ramdom or File)
  vtkKWFrameWithLabel *entryPointFrame = vtkKWFrameWithLabel::New();
  entryPointFrame->SetParent(this->MainFrame);
  entryPointFrame->Create();
  entryPointFrame->SetLabelText ("Needle Entry Points");
  app->Script ( "pack %s -fill both -expand n ",  
                entryPointFrame->GetWidgetName());
  
  this->EntryListSelectorWidget = vtkSlicerNodeSelectorWidget::New();
  this->EntryListSelectorWidget->SetParent(entryPointFrame->GetFrame());
  this->EntryListSelectorWidget->Create();
  this->EntryListSelectorWidget->SetBorderWidth(2);  
  this->EntryListSelectorWidget->SetNodeClass("vtkMRMLFiducialListNode", NULL, NULL, NULL);
  this->EntryListSelectorWidget->SetMRMLScene(app->GetApplicationGUI()->GetMRMLScene());
  this->EntryListSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->EntryListSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->EntryListSelectorWidget->SetLabelText( "Entry Point List: ");
  this->EntryListSelectorWidget->SetBalloonHelpString("Select the needle entry point.");

  this->EntryList = vtkKWMultiColumnListWithScrollbars::New();
  this->EntryList->SetParent(entryPointFrame->GetFrame());
  this->EntryList->Create();
  this->EntryList->SetHeight(1);
  this->EntryList->GetWidget()->SetSelectionTypeToRow();
  this->EntryList->GetWidget()->SetSelectionBackgroundColor(1,0,0);
  this->EntryList->GetWidget()->MovableRowsOff();
  this->EntryList->GetWidget()->MovableColumnsOff();

  for (int col = 0; col < COL_COUNT; col ++)
    {
    this->EntryList->GetWidget()->AddColumn(COL_LABELS[col]);
    this->EntryList->GetWidget()->SetColumnWidth(col, COL_WIDTHS[col]);
    this->EntryList->GetWidget()->SetColumnAlignmentToLeft(col);
    this->EntryList->GetWidget()->ColumnEditableOn(col);
    this->EntryList->GetWidget()->SetColumnEditWindowToSpinBox(col);
    }


  app->Script("pack %s %s -side top -anchor w -fill x -padx 2 -pady 2", 
              this->EntryListSelectorWidget->GetWidgetName(), 
              this->EntryList->GetWidgetName());


  // --------------------------------------------------
  // Control Buttons

  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(this->MainFrame);
  buttonFrame->Create();
  app->Script ( "pack %s -side top -fill both -expand n",  
                buttonFrame->GetWidgetName());

  this->StartTrackingButton = vtkKWPushButton::New();
  this->StartTrackingButton->SetParent(buttonFrame);
  this->StartTrackingButton->Create();
  this->StartTrackingButton->SetText( "Start" );
  this->StartTrackingButton->SetWidth (10);

  this->StopTrackingButton = vtkKWPushButton::New();
  this->StopTrackingButton->SetParent(buttonFrame);
  this->StopTrackingButton->Create();
  this->StopTrackingButton->SetText( "Stop" );
  this->StopTrackingButton->SetWidth (10);
  this->StopTrackingButton->SetEnabled(0);

  this->CloseButton = vtkKWPushButton::New();
  this->CloseButton->SetParent(buttonFrame);
  this->CloseButton->Create();
  this->CloseButton->SetText( "Close" );
  this->CloseButton->SetWidth (10);

  app->Script ( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                this->StartTrackingButton->GetWidgetName(),
                this->StopTrackingButton->GetWidgetName(),
                this->CloseButton->GetWidgetName());

  buttonFrame->Delete();

}


//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::DisplayOnWindow()
{
  //-- display
  this->DeIconify();
  this->Raise();

  this->SetPosition(this->WindowPosition[0], this->WindowPosition[1]);
  this->SetSize(this->WindowSize[0], this->WindowSize[1]);

  if (!this->IsObserverAddedFlag)
    {
    this->IsObserverAddedFlag = 1;
    this->AddGUIObservers();
    }

}


//----------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::UpdateEntryList()
{
  vtkMRMLFiducialListNode *fid = vtkMRMLFiducialListNode::SafeDownCast(this->EntryListSelectorWidget->GetSelected());

  if (fid == NULL)    //clear out the list box
    {
    if (this->EntryList)
      {
      if (this->EntryList->GetWidget()->GetNumberOfRows() != 0)
        {
        this->EntryList->GetWidget()->DeleteAllRows();
        }
      }
    return;
    }
  
  /*
  // create new target points, if necessary
  this->GetLogic()->UpdateEntryListFromMRML();

  vtkMRMLProstateNavManagerNode *manager = this->GetGUI()->GetProstateNavManagerNode();
  if (!manager)
  {
    return;
  }

  //int numPoints = activeFiducialListNode->GetNumberOfFiducials();
  int numPoints = manager->GetTotalNumberOfTargets();

  bool deleteFlag = true;

  if (this->EntryList && numPoints != this->EntryList->GetWidget()->GetNumberOfRows())
    {
    // clear out the multi column list box and fill it in with the
    // new list
    this->EntryList->GetWidget()->DeleteAllRows();
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
      this->EntryList->GetWidget()->AddRow();
      }
    this->EntryList->GetWidget()->SetRowAttributeAsInt(row, TARGET_INDEX_ATTR, targetIndex);

    xyz=target->GetRASLocation();
    wxyz=target->GetRASOrientation();

    if (xyz == NULL)
      {
      vtkErrorMacro ("UpdateEntryListGUI: ERROR: got null xyz for point " << row << endl);
      }

    if (target->GetName().compare(this->EntryList->GetWidget()->GetCellText(row,COL_NAME)) != 0)
        {
          this->EntryList->GetWidget()->SetCellText(row,COL_NAME,target->GetName().c_str());
        }               

    // selected
    vtkKWMultiColumnList* columnList = this->EntryList->GetWidget();
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

    //if (target->GetNeedleTypeString().compare(this->EntryList->GetWidget()->GetCellText(row,COL_NEEDLE)) != 0)
    //{
    //  this->EntryList->GetWidget()->SetCellText(row,COL_NEEDLE,target->GetNeedleTypeString().c_str());
    //}
    //
    }  
  */
}


//---------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::OnMultiColumnListUpdate(int row, int col, char * str)
{
/*
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
*/
}

//---------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::OnMultiColumnListSelectionChanged()
{
/*
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
*/
}
