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
void vtkKWPlotGraph::UpdateVTK()
{
  if ( this->Updating ) 
    {
    return;
    }
  this->Updating = 1;



  if (this->PlotActor)
    {
    vtkFloatArray* vtkXYValues = vtkFloatArray::New();
    vtkXYValues->SetNumberOfComponents( static_cast<vtkIdType>(2) );
    for ( int i = 0; i < 360; i+=10 )
      {
      float xy[2];
      xy[0] = static_cast<float>(i);
      xy[1] = sin( xy[0]*vtkMath::DegreesToRadians() );
      vtkXYValues->InsertNextTuple( xy );
      }
    
    vtkFieldData* vtkGraphFieldData = vtkFieldData::New();
    vtkGraphFieldData->AddArray( vtkXYValues );
    vtkXYValues->Delete();

  
    vtkDataObject* vtkGraphDataObject = vtkDataObject::New();
    vtkGraphDataObject->SetFieldData( vtkGraphFieldData );
    vtkGraphFieldData->Delete();

    //
    vtkFloatArray* vtkXYValues2 = vtkFloatArray::New();
    vtkXYValues2->SetNumberOfComponents( static_cast<vtkIdType>(2) );
    /*
    for ( int i = 0; i < 360; i+=10 )
      {
      float xy[2];
      xy[0] = static_cast<float>(i);
      xy[1] = sin( xy[0]*vtkMath::DegreesToRadians() )/2.0;
      vtkXYValues2->InsertNextTuple( xy );
      }
    */
    float xy[2];
    xy[0] = 100.0;     xy[1] = 1.0; 
    vtkXYValues2->InsertNextTuple( xy );
    xy[0] = 100.0;     xy[1] = -1.0; 
    vtkXYValues2->InsertNextTuple( xy );


    vtkFieldData* vtkGraphFieldData2 = vtkFieldData::New();
    vtkGraphFieldData2->AddArray( vtkXYValues2 );
    vtkXYValues2->Delete();

    vtkDataObject* vtkGraphDataObject2 = vtkDataObject::New();
    vtkGraphDataObject2->SetFieldData( vtkGraphFieldData2 );
    vtkGraphFieldData2->Delete();


    this->PlotActor->AddDataObjectInput(vtkGraphDataObject);
    this->PlotActor->AddDataObjectInput(vtkGraphDataObject2);

    this->PlotActor->SetDataObjectXComponent(0, 0);
    this->PlotActor->SetDataObjectYComponent(0, 1);
    this->PlotActor->SetDataObjectXComponent(1, 0);
    this->PlotActor->SetDataObjectYComponent(1, 1);

    this->PlotActor->SetPlotColor(0, 0.0, 0.0, 0.0);
    this->PlotActor->SetPlotColor(1, 1.0, 0.0, 0.0);

    vtkGraphDataObject->Delete();
    

    this->PlotActor->SetXRange(0.0, 360.0);
    this->PlotActor->SetYRange(-1.0, 1.0);

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

