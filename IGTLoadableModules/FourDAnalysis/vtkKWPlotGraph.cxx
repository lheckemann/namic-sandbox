/*=========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH)
  All rights reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $

=========================================================================*/
#include "vtkCommand.h"

#include "vtkKWPlotGraph.h"

#include "vtkKWMultiColumnList.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWPlotGraph );
vtkCxxRevisionMacro(vtkKWPlotGraph, "$Revision: 1.49 $");

//----------------------------------------------------------------------------
vtkKWPlotGraph::vtkKWPlotGraph()
{
  this->Updating = 0;
  this->UpdateWidget();
}

//----------------------------------------------------------------------------
vtkKWPlotGraph::~vtkKWPlotGraph()
{

}

//----------------------------------------------------------------------------
void vtkKWPlotGraph::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }

  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  //this->MultiColumnList->SetParent(this);
  //this->MultiColumnList->Create();
  //this->MultiColumnList->SetHeight(4);
  //this->MultiColumnList->SetSelectionTypeToCell();
  //
  //int col;
  //for (col = 0; col < 4; col++)
  //  {
  //  this->MultiColumnList->AddColumn(" ");
  //  this->MultiColumnList->SetColumnWidth(col, 7);
  //  this->MultiColumnList->SetColumnAlignmentToCenter(col);
  //  this->MultiColumnList->ColumnEditableOn(col);
  //  }
  //
  //// avoid the empty row of labels (and avoid accidental sorting!)
  //this->MultiColumnList->SetColumnLabelsVisibility(0);
  //
  //this->Script("pack %s -fill both -expand true",
  //               this->MultiColumnList->GetWidgetName());
  //
  //this->MultiColumnList->SetCellUpdatedCommand(this, "UpdateElement");

  // Override the column sorting behavior by always updating 
}

//----------------------------------------------------------------------------
void vtkKWPlotGraph::UpdateWidget()
{
  if ( this->Updating ) 
    {
    return;
    }
  this->Updating = 1;

  //int row, col;
  //for (row=0; row<4; row++)
  //  {
  //  for (col=0; col<4; col++)
  //    {
  //    this->MultiColumnList->InsertCellTextAsDouble(row, col, this->Matrix4x4->GetElement(row, col));
  //    this->MultiColumnList->SetCellEditWindowToSpinBox(row, col);
  //    }
  //  }
  this->Updating = 0;

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkKWPlotGraph::UpdateVTK()
{
  if ( this->Updating ) 
    {
    return;
    }
  this->Updating = 1;

  // update something

  this->Updating = 0;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  //os << indent << "Matrix4x4: " << this->GetMatrix4x4() << endl;
}

