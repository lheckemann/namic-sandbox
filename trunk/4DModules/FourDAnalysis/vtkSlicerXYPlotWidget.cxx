/*=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH)
  All rights reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $

=========================================================================*/
#include "vtkCommand.h"

#include "vtkSlicerXYPlotWidget.h"

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

#include <math.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkSlicerXYPlotWidget );
vtkCxxRevisionMacro(vtkSlicerXYPlotWidget, "$Revision: 1.49 $");

//----------------------------------------------------------------------------
vtkSlicerXYPlotWidget::vtkSlicerXYPlotWidget()
{

  // Set up callbacks
  this->MRMLObserverManager = vtkObserverManager::New();
  this->MRMLObserverManager->AssignOwner( this );
  this->MRMLObserverManager->GetCallbackCommand()->SetClientData( reinterpret_cast<void *> (this) );
  this->MRMLObserverManager->GetCallbackCommand()->SetCallback(vtkSlicerXYPlotWidget::MRMLCallback);
  this->MRMLCallbackCommand = this->MRMLObserverManager->GetCallbackCommand();

  this->MRMLScene  = NULL;
  this->XYPlotNode = NULL;

  this->Updating  = 0;
  this->PlotActor = NULL;
  //this->VerticalLines.clear();
  //this->HorizontalLines.clear();

  this->RangeX[0] = 0.0;
  this->RangeX[1] = 1.0;
  this->RangeY[0] = 0.0;
  this->RangeY[1] = 1.0;

  this->AxisLineColor[0] = 0.0;
  this->AxisLineColor[1] = 0.0;
  this->AxisLineColor[2] = 0.0;

  this->AutoUpdate = 0;
  this->InMRMLCallbackFlag = 0;

}


//----------------------------------------------------------------------------
vtkSlicerXYPlotWidget::~vtkSlicerXYPlotWidget()
{

  //this->VerticalLines.clear();
  //this->HorizontalLines.clear();

  if (this->PlotActor)
    {
    this->PlotActor->Delete();
    }

  if ( this->MRMLScene )
    {
    this->MRMLScene->RemoveObserver( this->MRMLCallbackCommand );
    this->MRMLScene->Delete ( );
    this->MRMLScene = NULL;
    //    this->MRMLScene->Delete();
    }

}

//----------------------------------------------------------------------------
void vtkSlicerXYPlotWidget::CreateWidget()
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

  this->PlotActor->SetTitle("");
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
  tprop->SetFontSize(10);

  this->PlotActor->SetAxisTitleTextProperty(tprop);
  this->PlotActor->SetAxisLabelTextProperty(tprop);
  
  //# Create the Renderers, RenderWindow, and RenderWindowInteractor.
  vtkRenderer* ren = this->GetRenderer();
  ren->SetBackground(1.0, 1.0, 1.0);
  ren->SetViewport(0.0, 0.0, 1.0, 1.0);
  ren->AddActor2D(this->PlotActor);
  //this->GetRenderWindow()->SetSize(500, 250);

}


//----------------------------------------------------------------------------
void vtkSlicerXYPlotWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkSlicerXYPlotWidget::ProcessMRMLEvents ( vtkObject * caller, 
                                                unsigned long event, void * callData )
{

  if ((this->AutoUpdate && event == vtkCommand::ModifiedEvent) ||
      event == vtkMRMLXYPlotManagerNode::UpdateGraphEvent)
    {
    vtkMRMLXYPlotManagerNode* pnode = vtkMRMLXYPlotManagerNode::SafeDownCast(caller);
    if (pnode)
      {
      this->UpdateGraph();
      }
    }
}


//----------------------------------------------------------------------------
// Description:
// the MRMLCallback is a static function to relay modified events from the 
// observed mrml node back into the gui layer for further processing
//
void vtkSlicerXYPlotWidget::MRMLCallback(vtkObject *caller, 
                                         unsigned long eid, void *clientData, void *callData)
{
  vtkSlicerXYPlotWidget *self = reinterpret_cast<vtkSlicerXYPlotWidget *>(clientData);
  
  if (self->GetInMRMLCallbackFlag())
    {
#ifdef _DEBUG
    vtkDebugWithObjectMacro(self, "In vtkSlicerXYPlotWidget *********MRMLCallback called recursively?");
#endif
    return;
    }
  
  vtkDebugWithObjectMacro(self, "In vtkSlicerXYPlotWidget MRMLCallback");
  
  self->SetInMRMLCallbackFlag(1);
  self->ProcessMRMLEvents(caller, eid, callData);
  self->SetInMRMLCallbackFlag(0);
}


//----------------------------------------------------------------------------
void vtkSlicerXYPlotWidget::SetMRMLScene( vtkMRMLScene *aMRMLScene)
{

  if ( this->MRMLScene )
    {
    this->MRMLScene->RemoveObserver( this->MRMLCallbackCommand );
    this->MRMLScene->Delete ( );
    this->MRMLScene = NULL;
    //    this->MRMLScene->Delete();
    }

  this->MRMLScene = aMRMLScene;

  if ( this->MRMLScene )
    {
    this->MRMLScene->Register(this);
    this->MRMLScene->AddObserver( vtkMRMLScene::NodeAddedEvent, this->MRMLCallbackCommand );
    this->MRMLScene->AddObserver( vtkMRMLScene::NodeRemovedEvent, this->MRMLCallbackCommand );
    this->MRMLScene->AddObserver( vtkMRMLScene::NewSceneEvent, this->MRMLCallbackCommand );
    this->MRMLScene->AddObserver( vtkMRMLScene::SceneCloseEvent, this->MRMLCallbackCommand );
    this->MRMLScene->AddObserver( vtkMRMLScene::SceneEditedEvent, this->MRMLCallbackCommand );
    }

}


//----------------------------------------------------------------------------
void vtkSlicerXYPlotWidget::SetAndObserveXYPlotNode(vtkMRMLXYPlotManagerNode* node)
{

  if (this->XYPlotNode)
    {
    vtkSetAndObserveMRMLObjectMacro(this->XYPlotNode, NULL);
    }
    
  this->XYPlotNode = node;
  
  if (node)
    {
    vtkMRMLXYPlotManagerNode* pnode = this->GetXYPlotNode();
    
    vtkIntArray *events = vtkIntArray::New();
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    events->InsertNextValue(vtkMRMLXYPlotManagerNode::UpdateGraphEvent);
    vtkSetAndObserveMRMLObjectEventsMacro(this->XYPlotNode, pnode, events);
    events->Delete();
    }

}


//----------------------------------------------------------------------------
void vtkSlicerXYPlotWidget::UpdateGraph()
{

  if (!this->XYPlotNode)
    {
    return;
    }

  if ( this->Updating ) 
    {
    return;
    }
  this->Updating = 1;

  unsigned int numPlots = this->XYPlotNode->GetNumberOfPlotNodes();
  vtkIntArray* idList = this->XYPlotNode->GetPlotNodeIDList();

  if (numPlots <= 0)
    {
    this->Updating = 0;
    return;
    }



  //--------------------------------------------------
  // Check automatic range adjustment flag
  if (this->XYPlotNode->GetAutoXRange() == 0)  // off
    {
    this->XYPlotNode->GetXRange(this->RangeX);
    }

  if (this->XYPlotNode->GetAutoYRange() == 0)  // off
    {
    this->XYPlotNode->GetYRange(this->RangeY);
    }
  
  //--------------------------------------------------
  // If auto-range options active, the ranges have to be
  // determined by finding minimum and maximum values in the data.
  int autoX = this->XYPlotNode->GetAutoXRange();
  int autoY = this->XYPlotNode->GetAutoYRange();
  
  if (autoX && autoY)
    {
    vtkMRMLPlotNode* node;
    int errorBar;

    double rangeX[2];
    double rangeY[2];
    
    // Substitute the first values
    int id = idList->GetValue(0);

    node  = this->XYPlotNode->GetPlotNode(id);
    node->GetXRange(rangeX);
    node->GetYRange(rangeY);

    if (autoX)
      {
      this->RangeX[0] = rangeX[0];
      this->RangeX[1] = rangeX[1];
      }
    if (autoY)
      {
      this->RangeY[0] = rangeY[0];
      this->RangeY[1] = rangeY[1];
      }

    // Search the list
    for (unsigned int i = 1; i < numPlots; i ++)
      {
      id = idList->GetValue(i);
      node = this->XYPlotNode->GetPlotNode(id);
      //errorBar = this->XYPlotNode->GetErrorBar(id);
      //node->GetRange(rangeX, rangeY, errorBar);
      if (autoX && node->GetXRange(rangeX))
        {
        if (rangeX[0] < this->RangeX[0])
          {
          this->RangeX[0] = rangeX[0];
          }
        if (rangeX[1] > this->RangeX[1])
          {
          this->RangeX[1] = rangeX[1];
          }
        }
      if (autoY && node->GetYRange(rangeY))
        {
        if (rangeY[0] < this->RangeY[0])
          {
          this->RangeY[0] = rangeY[0];
          }
        if (rangeY[1] > this->RangeY[1])
          {
          this->RangeY[1] = rangeY[1];
          }
        }
      }
    }

  // -----------------------------------------
  // Drawing

  if (this->PlotActor)
    {
    this->PlotActor->RemoveAllInputs();

    // -----------------------------------------
    // Draw curves

    int obj = 0;
    // Search the list
    for (unsigned int i = 0; i < numPlots; i ++)
      {
      int id = idList->GetValue(i);
      vtkMRMLPlotNode* node;
      node  = this->XYPlotNode->GetPlotNode(id);

      if (node->GetVisible())
        {
        double r;
        double g;
        double b;
        node->GetColor(r, g, b);

        vtkDataObject* dataObject = node->GetDrawObject(this->RangeX, this->RangeY);
        if (dataObject)
          {
          this->PlotActor->AddDataObjectInput(dataObject);
          this->PlotActor->SetDataObjectXComponent(obj, 0);
          this->PlotActor->SetDataObjectYComponent(obj, 1);
          this->PlotActor->SetPlotColor(obj, r, g, b);
          dataObject->Delete();
          obj ++;
          }
        
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
void vtkSlicerXYPlotWidget::SetAxisLineColor(double r, double g, double b)
{
  this->AxisLineColor[0] = r;
  this->AxisLineColor[1] = g;
  this->AxisLineColor[2] = b;
}


