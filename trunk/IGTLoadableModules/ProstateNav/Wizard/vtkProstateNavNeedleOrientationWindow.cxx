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



const char TARGET_INDEX_ATTR[]="TARGET_IND";

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

  this->SetButton = NULL;
  this->ResetButton = NULL;
  this->CloseButton = NULL;

  this->EntryListSelectorWidget=NULL;
  this->EntryList=NULL;

  // GUI callback command
  this->InGUICallbackFlag = 0;
  this->IsObserverAddedFlag = 0;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkProstateNavNeedleOrientationWindow::GUICallback);

  this->TargetFiducialID = NULL;
}


//----------------------------------------------------------------------------
vtkProstateNavNeedleOrientationWindow::~vtkProstateNavNeedleOrientationWindow()
{

  if ( this->GUICallbackCommand != NULL )
    {
    this->GUICallbackCommand->Delete ( );
    this->GUICallbackCommand = NULL;
    }

  if (this->SetButton)
    {
    this->SetButton->SetParent(NULL);
    this->SetButton->Delete();
    }
  if (this->ResetButton)
    {
    this->ResetButton->SetParent(NULL);
    this->ResetButton->Delete();
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
  else if (this->SetButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    SetOrientation();
    }
  else if (this->ResetButton == vtkKWPushButton::SafeDownCast(caller) 
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

  if (this->SetButton)
    {
    this->SetButton->AddObserver(vtkKWPushButton::InvokedEvent,(vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ResetButton)
    {
    this->ResetButton->AddObserver(vtkKWPushButton::InvokedEvent,(vtkCommand *)this->GUICallbackCommand);
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
  if (this->SetButton)
    {
    this->SetButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ResetButton)
    {
    this->ResetButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
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

  UpdateEntryList();

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

  this->SetButton = vtkKWPushButton::New();
  this->SetButton->SetParent(buttonFrame);
  this->SetButton->Create();
  this->SetButton->SetText( "Set" );
  this->SetButton->SetWidth (10);

  this->ResetButton = vtkKWPushButton::New();
  this->ResetButton->SetParent(buttonFrame);
  this->ResetButton->Create();
  this->ResetButton->SetText( "Unset" );
  this->ResetButton->SetWidth (10);

  this->CloseButton = vtkKWPushButton::New();
  this->CloseButton->SetParent(buttonFrame);
  this->CloseButton->Create();
  this->CloseButton->SetText( "Close" );
  this->CloseButton->SetWidth (10);

  app->Script ( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                this->SetButton->GetWidgetName(),
                this->ResetButton->GetWidgetName(),
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
  
  int numPoints = fid->GetNumberOfFiducials();

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

  for (int row = 0; row < numPoints; row++)
    {      
    int targetIndex=row;

    if (deleteFlag)
      {
      // add a row for this point
      this->EntryList->GetWidget()->AddRow();
      }

    this->EntryList->GetWidget()->SetRowAttributeAsInt(row, TARGET_INDEX_ATTR, targetIndex);
    float * xyz = fid->GetNthFiducialXYZ(row);
    //wxyz=target->GetRASOrientation();

    if (xyz == NULL)
      {
      vtkErrorMacro ("UpdateEntryListGUI: ERROR: got null xyz for point " << row << endl);
      }
    
    const char * label = fid->GetNthFiducialLabelText(row);
    if (strcmp(label, this->EntryList->GetWidget()->GetCellText(row,COL_NAME)) != 0)
      {
      this->EntryList->GetWidget()->SetCellText(row,COL_NAME,label);
      }

    // selected
    vtkKWMultiColumnList* columnList = this->EntryList->GetWidget();
    if (xyz != NULL)
      {
      for (int i = 0; i < 3; i ++) // for position (x, y, z)
        {
        if ((float)columnList->GetCellTextAsDouble(row,COL_X+i) != xyz[i])
          {
          columnList->SetCellTextAsDouble(row,COL_X+i,xyz[i]);
          }
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::OnMultiColumnListUpdate(int row, int col, char * str)
{

  vtkMRMLFiducialListNode *fidList = vtkMRMLFiducialListNode::SafeDownCast(this->EntryListSelectorWidget->GetSelected());

  if (fidList == NULL)
    {
    return;
    }

  bool updated=false;

  // make sure that the row and column exists in the table
  if ((row >= 0) && (row < this->EntryList->GetWidget()->GetNumberOfRows()) &&
      (col >= 0) && (col < this->EntryList->GetWidget()->GetNumberOfColumns()))
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
    else
      {
      return;
      }
    }
  else
    {
    }

}


//---------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::OnMultiColumnListSelectionChanged()
{

  vtkMRMLFiducialListNode *fidList = vtkMRMLFiducialListNode::SafeDownCast(this->EntryListSelectorWidget->GetSelected());

  if (fidList == NULL)
    {
    return;
    }

  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  if (app==NULL)
  {
  vtkErrorMacro("CreateWindow: application is invalid");
  return;
  }

  if (app->GetApplicationGUI()->GetMRMLScene())
    {
    app->GetApplicationGUI()->GetMRMLScene()->SaveStateForUndo();
    }

  int numRows = this->EntryList->GetWidget()->GetNumberOfSelectedRows();
  if (numRows == 1)
    {   
    //int rowIndex = this->EntryList->GetWidget()->GetIndexOfFirstSelectedRow();    
    //int targetIndex=this->EntryList->GetWidget()->GetRowAttributeAsInt(rowIndex, TARGET_INDEX_ATTR);
    //// Copy the values to inputs
    //vtkKWMatrixWidget* matrix = this->NeedlePositionMatrix->GetWidget();
    //double* xyz=targetDesc->GetRASLocation();
    //matrix->SetElementValueAsDouble(0, 0, xyz[0]);
    //matrix->SetElementValueAsDouble(0, 1, xyz[1]);
    //matrix->SetElementValueAsDouble(0, 2, xyz[2]);
    }

}


typedef float  Matrix4x4[4][4];

void QuaternionToMatrix(float* q, Matrix4x4& m)
{

  // normalize
  float mod = sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);

  // convert to the matrix
  const float x = q[0] / mod;
  const float y = q[1] / mod; 
  const float z = q[2] / mod; 
  const float w = q[3] / mod;
  
  const float xx = x * x * 2.0;
  const float xy = x * y * 2.0;
  const float xz = x * z * 2.0;
  const float xw = x * w * 2.0;
  const float yy = y * y * 2.0;
  const float yz = y * z * 2.0;
  const float yw = y * w * 2.0;
  const float zz = z * z * 2.0;
  const float zw = z * w * 2.0;
  
  m[0][0] = 1.0 - (yy + zz);
  m[1][0] = xy + zw;
  m[2][0] = xz - yw;
  
  m[0][1] = xy - zw;
  m[1][1] = 1.0 - (xx + zz);
  m[2][1] = yz + xw;
  
  m[0][2] = xz + yw;
  m[1][2] = yz - xw;
  m[2][2] = 1.0 - (xx + yy);

  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 0.0;
  m[3][3] = 1.0;

  m[0][3] = 0.0;
  m[1][3] = 0.0;
  m[2][3] = 0.0;

}


void MatrixToQuaternion(Matrix4x4& m, float* q)
{
  float trace = m[0][0] + m[1][1] + m[2][2];

  if( trace > 0.0 ) {

    float s = 0.5f / sqrt(trace + 1.0f);

    q[3] = 0.25f / s;
    q[0] = ( m[2][1] - m[1][2] ) * s;
    q[1] = ( m[0][2] - m[2][0] ) * s;
    q[2] = ( m[1][0] - m[0][1] ) * s;

  } else {

    if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {

      float s = 2.0f * sqrt( 1.0f + m[0][0] - m[1][1] - m[2][2]);

      q[3] = (m[2][1] - m[1][2] ) / s;
      q[0] = 0.25f * s;
      q[1] = (m[0][1] + m[1][0] ) / s;
      q[2] = (m[0][2] + m[2][0] ) / s;

    } else if (m[1][1] > m[2][2]) {

      float s = 2.0f * sqrt( 1.0f + m[1][1] - m[0][0] - m[2][2]);

      q[3] = (m[0][2] - m[2][0] ) / s;
      q[0] = (m[0][1] + m[1][0] ) / s;
      q[1] = 0.25f * s;
      q[2] = (m[1][2] + m[2][1] ) / s;

    } else {

      float s = 2.0f * sqrt( 1.0f + m[2][2] - m[0][0] - m[1][1] );

      q[3] = (m[1][0] - m[0][1] ) / s;
      q[0] = (m[0][2] + m[2][0] ) / s;
      q[1] = (m[1][2] + m[2][1] ) / s;
      q[2] = 0.25f * s;

    }
  }
}
  

  
void Cross(float *a, float *b, float *c)
{
    a[0] = b[1]*c[2] - c[1]*b[2];
    a[1] = c[0]*b[2] - b[0]*c[2];
    a[2] = b[0]*c[1] - c[0]*b[1];
}


//---------------------------------------------------------------------------
void vtkProstateNavNeedleOrientationWindow::SetOrientation()
{

  if (this->GetTargetFiducialID() == NULL)
    {
    return ;
    }

  vtkMRMLFiducialListNode *fidList = vtkMRMLFiducialListNode::SafeDownCast(this->EntryListSelectorWidget->GetSelected());
  if (fidList == NULL)
    {
    //this->TargetDesc->SetRASOrientation(0.0, 0.0, 0.0, 1.0); 
    return;
    }

  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  if (app==NULL)
    {
    vtkErrorMacro("CreateWindow: application is invalid");
    return;
    }
  
  if (app->GetApplicationGUI()->GetMRMLScene())
    {
    app->GetApplicationGUI()->GetMRMLScene()->SaveStateForUndo();
    }

  vtkMRMLFiducialListNode *targetFidList 
    = vtkMRMLFiducialListNode::SafeDownCast(app->GetApplicationGUI()->GetMRMLScene()->GetNodeByID(this->GetTargetFiducialID()));
  if (targetFidList == NULL)
    {
    //this->TargetDesc->SetRASOrientation(0.0, 0.0, 0.0, 1.0); 
    return;
    }

  int numRows = this->EntryList->GetWidget()->GetNumberOfSelectedRows();

  if (numRows == 1)
    {   
    int rowIndex = this->EntryList->GetWidget()->GetIndexOfFirstSelectedRow();
    float * target = fidList->GetNthFiducialXYZ(this->GetSelectedTarget());
    //double * entry = this->TargetDesc->GetRASLocation();

    float * entry = fidList->GetNthFiducialXYZ(rowIndex);

    float normal_z[3]; // needle orientation
    normal_z[0] = target[0] - (float)entry[0];
    normal_z[1] = target[1] - (float)entry[1];
    normal_z[2] = target[2] - (float)entry[2];

    float length = sqrtf(normal_z[0]*normal_z[0] + normal_z[1]*normal_z[1] + normal_z[2]*normal_z[2]);
    normal_z[0] /= length;
    normal_z[1] /= length;
    normal_z[2] /= length;

    float vert[3] = {0.0, 1.0, 0.0};
    
    float normal_x[3]; // horizontal normal vector perpendicular to the needle
    Cross(normal_x, vert, normal_z);

    float normal_y[3]; // normal vector perpendicular to the needle and normal_x
    Cross(normal_y, normal_z, normal_x);

    Matrix4x4 m;
    m[0][0] = normal_x[0]; m[0][1] = normal_y[0]; m[0][2] = normal_z[0]; m[0][3] = 0.0;
    m[1][0] = normal_x[1]; m[1][1] = normal_y[1]; m[1][2] = normal_z[1]; m[1][3] = 0.0;
    m[2][0] = normal_x[2]; m[2][1] = normal_y[2]; m[2][2] = normal_z[2]; m[2][3] = 0.0;
    m[3][0] = 0.0;         m[3][1] = 0.0;         m[3][2] = 0.0;         m[3][3] = 1.0;

    float q[4];
    MatrixToQuaternion(m, q);
    //this->TargetDesc->SetRASOrientation(q[0], q[1], q[2], q[3]); 
    targetFidList->SetNthFiducialOrientation(this->GetSelectedTarget(), q[0], q[1], q[2], q[3]);
    }

}
