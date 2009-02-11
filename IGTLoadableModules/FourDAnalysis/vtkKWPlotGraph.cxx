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
  this->RangeX[0] = 0.0;
  this->RangeX[1] = 1.0;
  this->RangeY[0] = 0.0;
  this->RangeY[1] = 1.0;

  this->AutoRangeX = 1;
  this->AutoRangeY = 1;
}

//----------------------------------------------------------------------------
vtkKWPlotGraph::~vtkKWPlotGraph()
{



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
int vtkKWPlotGraph::SetNumberOfPlots(int n)
{
  this->PlotDataVector.resize(n);
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::SetData(int id, vtkDoubleArray* data, const char* label)
{
  if (id >= this->PlotDataVector.size())
    {
    return;
    }

  this->PlotDataVector[id].data  = data;
  this->PlotDataVector[id].label = label;
  this->PlotDataVector[id].visible = 1;
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
void vtkKWPlotGraph::UpdateWidget()
{
  if ( this->Updating ) 
    {
    return;
    }
  this->Updating = 1;

  this->Updating = 0;

  this->Modified();
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

  std::cerr << "data range X: [" << this->RangeX[0] << ", " << this->RangeX[1] << "]" << std::endl;
  std::cerr << "data range Y: [" << this->RangeY[0] << ", " << this->RangeY[1] << "]" << std::endl;


  if (this->PlotActor)
    {
    this->PlotActor->RemoveAllInputs();
    
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
        }
      }
    
    int i = 0;
    for (iter = this->PlotDataVector.begin(); iter != this->PlotDataVector.end(); iter ++)
      {
      if (iter->visible)
        {
        this->PlotActor->SetDataObjectXComponent(i, 0);
        this->PlotActor->SetDataObjectYComponent(i, 1);
        this->PlotActor->SetPlotColor(i, iter->color[0], iter->color[1], iter->color[2]);
        i ++;
        }
      }

    //this->PlotActor->SetXRange(0.0, 360.0);
    this->PlotActor->SetXRange(this->RangeX[0], this->RangeX[1]);
    //this->PlotActor->SetYRange(-1.0, 1.0);
    this->PlotActor->SetYRange(this->RangeY[0], this->RangeY[1]);
    
    this->GetRenderWindowInteractor()->Initialize();
    this->GetRenderWindow()->Render();
    this->GetRenderWindowInteractor()->Start();

    }

    
//    vtkFloatArray* vtkXYValues = vtkFloatArray::New();
//    vtkXYValues->SetNumberOfComponents( static_cast<vtkIdType>(2) );
//    for ( int i = 0; i < 360; i+=10 )
//      {
//      float xy[2];
//      xy[0] = static_cast<float>(i);
//      xy[1] = sin( xy[0]*vtkMath::DegreesToRadians() );
//      vtkXYValues->InsertNextTuple( xy );
//      }
//    
//    vtkFieldData* vtkGraphFieldData = vtkFieldData::New();
//    vtkGraphFieldData->AddArray( vtkXYValues );
//    vtkXYValues->Delete();
//
//  
//    vtkDataObject* vtkGraphDataObject = vtkDataObject::New();
//    vtkGraphDataObject->SetFieldData( vtkGraphFieldData );
//    vtkGraphFieldData->Delete();
//
//    //
//    vtkFloatArray* vtkXYValues2 = vtkFloatArray::New();
//    vtkXYValues2->SetNumberOfComponents( static_cast<vtkIdType>(2) );
//    /*
//    for ( int i = 0; i < 360; i+=10 )
//      {
//      float xy[2];
//      xy[0] = static_cast<float>(i);
//      xy[1] = sin( xy[0]*vtkMath::DegreesToRadians() )/2.0;
//      vtkXYValues2->InsertNextTuple( xy );
//      }
//    */
//    float xy[2];
//    xy[0] = 100.0;     xy[1] = 1.0; 
//    vtkXYValues2->InsertNextTuple( xy );
//    xy[0] = 100.0;     xy[1] = -1.0; 
//    vtkXYValues2->InsertNextTuple( xy );
//
//
//    vtkFieldData* vtkGraphFieldData2 = vtkFieldData::New();
//    vtkGraphFieldData2->AddArray( vtkXYValues2 );
//    vtkXYValues2->Delete();
//
//    vtkDataObject* vtkGraphDataObject2 = vtkDataObject::New();
//    vtkGraphDataObject2->SetFieldData( vtkGraphFieldData2 );
//    vtkGraphFieldData2->Delete();
//\
//\
//\    this->PlotActor->AddDataObjectInput(vtkGraphDataObject);
//\    this->PlotActor->AddDataObjectInput(vtkGraphDataObject2);
//\
//\    this->PlotActor->SetDataObjectXComponent(0, 0);
//\    this->PlotActor->SetDataObjectYComponent(0, 1);
//\    this->PlotActor->SetDataObjectXComponent(1, 0);
//\    this->PlotActor->SetDataObjectYComponent(1, 1);
//\
//\    this->PlotActor->SetPlotColor(0, 0.0, 0.0, 0.0);
//\    this->PlotActor->SetPlotColor(1, 1.0, 0.0, 0.0);
//
//    vtkGraphDataObject->Delete();
    

  this->Updating = 0;
}


//----------------------------------------------------------------------------
void vtkKWPlotGraph::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  //os << indent << "Matrix4x4: " << this->GetMatrix4x4() << endl;
}

