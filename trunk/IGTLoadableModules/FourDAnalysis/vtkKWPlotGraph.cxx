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

#include "vtkXYPlotActor.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPiecewiseFunction.h"
#include "vtkFieldData.h"
#include "vtkDataArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkTextProperty.h"
#include "vtkGlyphSource2D.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWPlotGraph );
vtkCxxRevisionMacro(vtkKWPlotGraph, "$Revision: 1.49 $");

//----------------------------------------------------------------------------
vtkKWPlotGraph::vtkKWPlotGraph()
{
  this->Updating  = 0;
  this->PlotActor = NULL;
  this->PlotDataVector.clear();
  this->VerticalLines.clear();
  this->HorizontalLines.clear();

  this->RangeX[0] = 0.0;
  this->RangeX[1] = 1.0;
  this->RangeY[0] = 0.0;
  this->RangeY[1] = 1.0;

  this->AutoRangeX = 1;
  this->AutoRangeY = 1;

  this->AxisLineColor[0] = 0.0;
  this->AxisLineColor[1] = 0.0;
  this->AxisLineColor[2] = 0.0;
}


//----------------------------------------------------------------------------
vtkKWPlotGraph::~vtkKWPlotGraph()
{

  this->PlotDataVector.clear();
  this->VerticalLines.clear();
  this->HorizontalLines.clear();

  if (this->PlotActor)
    {
    this->PlotActor->Delete();
    }
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

  this->Superclass::CreateWidget();

  this->PlotActor = vtkXYPlotActor::New();

  this->PlotActor->SetDataObjectPlotModeToColumns();
  this->PlotActor->SetXValuesToValue();

  this->PlotActor->GetPositionCoordinate()->SetValue(0.0, 0.0, 0);
  this->PlotActor->GetPosition2Coordinate()->SetValue(1.0, 1.0, 0); //#relative to Position

  //this->PlotActor->SetXValuesToArcLength();
  //this->PlotActor->SetNumberOfXLabels(6);

  this->PlotActor->SetTitle("Intensity curve");
  this->PlotActor->SetXTitle("Time (s)");
  this->PlotActor->SetYTitle("");
  this->PlotActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
  this->PlotActor->GetProperty()->SetLineWidth(1);
  //this->PlotActor->GetProperty()->SetPointSize(3);

  //# Set text prop color (same color for backward compat with test)
  //# Assign same object to all text props
  vtkTextProperty* tprop = this->PlotActor->GetTitleTextProperty();
  tprop->SetColor(0.0, 0.0, 0.0);
  tprop->ItalicOff();
  tprop->BoldOff();
  tprop->ShadowOff();
  tprop->SetFontSize(8);

  this->PlotActor->SetAxisTitleTextProperty(tprop);
  this->PlotActor->SetAxisLabelTextProperty(tprop);
  
  //this->PlotActor->PlotPointsOn();
  //this->PlotActor->SetGlyphSize(10.0);
  //this->PlotActor->GetGlyphSource()->SetGlyphTypeToSquare();
  //this->PlotActor->PlotCurvePointsOn();

  //# Create the Renderers, RenderWindow, and RenderWindowInteractor.
  vtkRenderer* ren = this->GetRenderer();
  ren->SetBackground(1.0, 1.0, 1.0);
  ren->SetViewport(0.0, 0.0, 1.0, 1.0);
  ren->AddActor2D(this->PlotActor);
  //this->GetRenderWindow()->SetSize(500, 250);

}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::ClearPlot()
{
  this->PlotDataVector.clear();
}


//----------------------------------------------------------------------------
int vtkKWPlotGraph::AddPlot(vtkDoubleArray* data, const char* label)
{
  PlotDataType plotData;
  plotData.data    = data;
  plotData.label   = label;
  plotData.visible = 1;

  this->PlotDataVector.push_back(plotData);
  return this->PlotDataVector.size()-1;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::AddVerticalLine(double x)
{
  AxisLineType lineData;
  lineData.pos     = x;
  lineData.visible = 1;
  this->VerticalLines.push_back(lineData);
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::AddHorizontalLine(double y)
{
  AxisLineType lineData;
  lineData.pos     = y;
  lineData.visible = 1;
  this->HorizontalLines.push_back(lineData);
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::SetAxisLineColor(double r, double g, double b)
{
  this->AxisLineColor[0] = r;
  this->AxisLineColor[1] = g;
  this->AxisLineColor[2] = b;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::RemoveLines()
{
  this->VerticalLines.clear();
  this->HorizontalLines.clear();
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::SetColor(int id, double r, double g, double b)
{
  if (id >= this->PlotDataVector.size())
    {
    return;
    }

  this->PlotDataVector[id].color[0] = r;
  this->PlotDataVector[id].color[1] = g;
  this->PlotDataVector[id].color[2] = b;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::AutoRangeOn()
{
  this->AutoRangeX = 1;
  this->AutoRangeY = 1;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::AutoRangeOff()
{
  this->AutoRangeX = 0;
  this->AutoRangeY = 0;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::SetXrange(double min, double max)
{
  if (min < max)
    {
    this->AutoRangeX = 0;
    this->RangeX[0] = min;
    this->RangeX[1] = max;
    }
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::SetYrange(double min, double max)
{
  if (min < max)
    {
    this->AutoRangeY = 0;
    this->RangeY[0] = min;
    this->RangeY[1] = max;
    }
}

//----------------------------------------------------------------------------
void vtkKWPlotGraph::UpdateGraph()
{
  if ( this->Updating ) 
    {
    return;
    }
  this->Updating = 1;

  // if AutoRange is set, check the range of the values
  PlotDataVectorType::iterator it;
  double xy[2];

  // put the first value as an initial range
  it = this->PlotDataVector.begin();
  if (it != this->PlotDataVector.end())
    {
    it->data->GetTupleValue(0, xy);
    if (this->AutoRangeX)
      {
      this->RangeX[0] = xy[0];
      this->RangeX[1] = xy[0];
      }
    if (this->AutoRangeY)
      {
      this->RangeY[0] = xy[1];
      this->RangeY[1] = xy[1];
      }
    if (this->AutoRangeX || this->AutoRangeY)
      {
      for (; it != this->PlotDataVector.end(); it ++)
        {
        int n = it->data->GetNumberOfTuples();
        for (int i = 0; i < n; i ++)
          {
          it->data->GetTupleValue(i, xy);
          if (this->AutoRangeX)
            {
            if (xy[0] < this->RangeX[0]) this->RangeX[0] = xy[0];  // minimum X
            if (xy[0] > this->RangeX[1]) this->RangeX[1] = xy[0];  // maximum X
            }
          if (this->AutoRangeY)
            {
            if (xy[1] < this->RangeY[0]) this->RangeY[0] = xy[1];  // minimum Y
            if (xy[1] > this->RangeY[1]) this->RangeY[1] = xy[1];  // maximum Y
            }
          }
        }
      }
    }

  std::cerr << "data range X: [" << this->RangeX[0] << ", " << this->RangeX[1] << "]" << std::endl;
  std::cerr << "data range Y: [" << this->RangeY[0] << ", " << this->RangeY[1] << "]" << std::endl;

  // set color for axis lines
  AxisLineVectorType::iterator aiter;
  for (aiter = this->VerticalLines.begin(); aiter != this->VerticalLines.end(); aiter ++)
    {
    aiter->color[0] = this->AxisLineColor[0];
    aiter->color[1] = this->AxisLineColor[1];
    aiter->color[2] = this->AxisLineColor[2];
    }
  for (aiter = this->HorizontalLines.begin(); aiter != this->HorizontalLines.end(); aiter ++)
    {
    aiter->color[0] = this->AxisLineColor[0];
    aiter->color[1] = this->AxisLineColor[1];
    aiter->color[2] = this->AxisLineColor[2];
    }

  if (this->PlotActor)
    {
    this->PlotActor->RemoveAllInputs();

    int i;
    PlotDataVectorType::iterator iter;
    for (iter = this->PlotDataVector.begin(); iter != this->PlotDataVector.end(); iter ++)
      {
      if (iter->visible)
        {
        vtkFieldData* fieldData = vtkFieldData::New();
        fieldData->AddArray(iter->data);

        vtkDataObject* dataObject = vtkDataObject::New();
        dataObject->SetFieldData( fieldData );
        fieldData->Delete();

        this->PlotActor->AddDataObjectInput(dataObject);
        dataObject->Delete();

        this->PlotActor->SetDataObjectXComponent(i, 0);
        this->PlotActor->SetDataObjectYComponent(i, 1);
        this->PlotActor->SetPlotColor(i, iter->color[0], iter->color[1], iter->color[2]);

        i ++;
        }
      }
    
    AxisLineVectorType::iterator aiter;
    for (aiter = this->VerticalLines.begin(); aiter != this->VerticalLines.end(); aiter ++)
      {
      if (aiter->visible)
        {
        vtkFloatArray* value = vtkFloatArray::New();
        value->SetNumberOfComponents( static_cast<vtkIdType>(2) );
        float xy[2];
        xy[0] = aiter->pos;  xy[1] = this->RangeY[0]; 
        value->InsertNextTuple( xy );
        xy[0] = aiter->pos;  xy[1] = this->RangeY[1]; 
        value->InsertNextTuple( xy );

        vtkFieldData* fieldData = vtkFieldData::New();
        fieldData->AddArray(value);
        value->Delete();

        vtkDataObject* dataObject = vtkDataObject::New();
        dataObject->SetFieldData( fieldData );
        fieldData->Delete();

        this->PlotActor->AddDataObjectInput(dataObject);
        dataObject->Delete();

        this->PlotActor->SetDataObjectXComponent(i, 0);
        this->PlotActor->SetDataObjectYComponent(i, 1);
        this->PlotActor->SetPlotColor(i, aiter->color[0], aiter->color[1], aiter->color[2]);
        i ++;
        }
      }
    
    for (aiter = this->HorizontalLines.begin(); aiter != this->HorizontalLines.end(); aiter ++)
      {
      if (aiter->visible)
        {
        vtkFloatArray* value = vtkFloatArray::New();
        value->SetNumberOfComponents( static_cast<vtkIdType>(2) );
        float xy[2];
        xy[0] = this->RangeX[0];  xy[1] = aiter->pos; 
        value->InsertNextTuple( xy );
        xy[0] = this->RangeX[1];  xy[1] = aiter->pos; 
        value->InsertNextTuple( xy );

        vtkFieldData* fieldData = vtkFieldData::New();
        fieldData->AddArray(value);
        value->Delete();

        vtkDataObject* dataObject = vtkDataObject::New();
        dataObject->SetFieldData( fieldData );
        fieldData->Delete();

        this->PlotActor->AddDataObjectInput(dataObject);
        dataObject->Delete();

        this->PlotActor->SetDataObjectXComponent(i, 0);
        this->PlotActor->SetDataObjectYComponent(i, 1);
        this->PlotActor->SetPlotColor(i, aiter->color[0], aiter->color[1], aiter->color[2]);
        i ++;
        }
      }

    
    this->PlotActor->SetXRange(this->RangeX[0], this->RangeX[1]);
    this->PlotActor->SetYRange(this->RangeY[0], this->RangeY[1]);
    
    this->GetRenderWindowInteractor()->Initialize();
    this->GetRenderWindow()->Render();
    this->GetRenderWindowInteractor()->Start();

    }

    
  this->Updating = 0;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  //os << indent << "Matrix4x4: " << this->GetMatrix4x4() << endl;
}

