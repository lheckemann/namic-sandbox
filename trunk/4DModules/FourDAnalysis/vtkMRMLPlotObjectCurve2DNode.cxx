/*=auto=========================================================================

Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkMRMLScene.h"

#include <sstream>
#include <string>


//------------------------------------------------------------------------------
vtkMRMLPlotObjectCurve2DNode* vtkMRMLPlotObjectCurve2DNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLPlotObjectCurve2DNode"); if(ret)
    {
      return (vtkMRMLPlotObjectCurve2DNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLPlotObjectCurve2DNode;
}


//----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLPlotObjectCurve2DNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLPlotObjectCurve2DNode");
  if(ret)
    {
      return (vtkMRMLPlotObjectCurve2DNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLPlotObjectCurve2DNode;
}


//----------------------------------------------------------------------------
vtkMRMLPlotObjectCurve2DNode::vtkMRMLPlotObjectCurve2DNode()
{
  this->Array = NULL;
  this->PlotError = 0:
}


//----------------------------------------------------------------------------
vtkMRMLPlotObjectCurve2DNode::~vtkMRMLPlotObjectCurve2DNode()
{
}


//----------------------------------------------------------------------------
void vtkMRMLPlotObjectCurve2DNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  std::stringstream ssX;
  std::stringstream ssY;
  std::stringstream ssYerr;

  int n = this->Array->GetNumberOfComponents();
  double xy[3];

  //if (this->Array->GetNumberOfComponents() > 3)
  //  {
  //  // Put values to the string streams except the last values.
  //  n = this->Array->GetNumberOfTuples() - 1;
  //  for (int i = 0; i < n; i ++)
  //    {
  //    this->Array->GetTupleValue(i, xy);
  //    ssX    << xy[0] << ", ";
  //    ssY    << xy[1] << ", ";
  //    ssYerr << xy[2] << ", ";
  //    }
  //  this->Array->GetTupleValue(n, xy);
  //  // put the last values
  //  ssX    << xy[0];
  //  ssY    << xy[1];
  //  ssYerr << xy[2];
  //  }
  //
  //of << " valueX=\""    << ssX.str() << "\"";
  //of << " valueY=\""    << ssY.str() << "\"";
  //of << " valueYErr=\"" << ssYerr.str() << "\"";

}


//----------------------------------------------------------------------------
void vtkMRMLPlotObjectCurve2DNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  vtkMRMLNode::ReadXMLAttributes(atts);

  //const char* attName;
  //const char* attValue;
  //
  //std::vector<double> valueX;
  //std::vector<double> valueY;
  //std::vector<double> valueYErr;
  //
  //valueX.clear();
  //valueY.clear();
  //valueYErr.clear();
  //
  //while (*atts != NULL) 
  //  {
  //  attName = *(atts++);
  //  attValue = *(atts++);
  //
  //  if (!strcmp(attName, "valueX")) 
  //    {
  //    std::stringstream ss;
  //    std::string s;
  //    double val;
  //
  //    ss << attValue;
  //
  //    while (std::getline(ss, s, ','))
  //      {
  //      double v = atof(s.c_str());
  //      valueX.push_back(v);
  //      }
  //    }
  //  else if (!strcmp(attName, "valueY"))
  //    {
  //    std::stringstream ss;
  //    std::string s;
  //    double val;
  //
  //    ss << attValue;
  //
  //    while (std::getline(ss, s, ','))
  //      {
  //      double v = atof(s.c_str());
  //      valueY.push_back(v);
  //      }
  //    }
  //  else if (!strcmp(attName, "valueYErr"))
  //    {
  //    std::stringstream ss;
  //    std::string s;
  //    double val;
  //
  //    ss << attValue;
  //
  //    while (std::getline(ss, s, ','))
  //      {
  //      double v = atof(s.c_str());
  //      valueYErr.push_back(v);
  //      }
  //    }
  //  }
  //
  //if (valueYErr.size() > 0)  // if Y error values have loaded
  //  {
  //  if (valueX.size() == valueY.size() && valueY.size() == valueYErr.size())
  //    {
  //    int n = valueX.size();
  //    this->Array->SetNumberOfComponents(3);
  //    this->Array->SetNumberOfTuples(n);
  //    for (int i = 0; i < n; i ++)
  //      {
  //      double xy[3];
  //      xy[0] = valueX[i];
  //      xy[1] = valueY[i];
  //      xy[2] = valueYErr[i];
  //      this->Array->SetTupleValue(i, xy);
  //      }
  //    }
  //  }
  //else
  //  {
  //  if (valueX.size() == valueY.size())
  //    {
  //    int n = valueX.size();
  //    this->Array->SetNumberOfComponents(3);
  //    this->Array->SetNumberOfTuples(n);
  //    for (int i = 0; i < n; i ++)
  //      {
  //      double xy[3];
  //      xy[0] = valueX[i];
  //      xy[1] = valueY[i];
  //      xy[2] = 0.0;
  //      this->Array->SetTupleValue(i, xy);
  //      }
  //    }
  //  }
  //
  //this->EndModify(disabledModify);

}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLPlotObjectCurve2DNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
  vtkMRMLPlotObjectCurve2DNode *node = (vtkMRMLPlotObjectCurve2DNode *) anode;

  //int type = node->GetType();
  
}


//----------------------------------------------------------------------------
void vtkMRMLPlotObjectCurve2DNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  //if (this->TargetPlanList && this->TargetPlanList == vtkMRMLFiducialListNode::SafeDownCast(caller) &&
  //  event ==  vtkCommand::ModifiedEvent)
  //  {
  //  //this->ModifiedSinceReadOn();
  //  //this->InvokeEvent(vtkMRMLVolumeNode::ImageDataModifiedEvent, NULL);
  //  //this->UpdateFromMRML();
  //  return;
  //  }
  //
  //if (this->TargetCompletedList && this->TargetCompletedList == vtkMRMLFiducialListNode::SafeDownCast(caller) &&
  //  event ==  vtkCommand::ModifiedEvent)
  //  {
  //  //this->ModifiedSinceReadOn();
  //  //this->InvokeEvent(vtkMRMLVolumeNode::ImageDataModifiedEvent, NULL);
  //  //this->UpdateFromMRML();
  //  return;
  //  }

  return;
}


//----------------------------------------------------------------------------
void vtkMRMLPlotObjectCurve2DNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkMRMLPlotObjectCurve2DNode::SetAndObserveArray( vtkMRMLDoubleArrayNode* node )
{
  if (this->Array != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->Array, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->Array = node;

  if (node != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      node, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  if ( this->Array != node )
    {
    this->Modified();
    }
}


//----------------------------------------------------------------------------
int vtkMRMLPlotObjectCurve2DNode::GetXRange(double* xrange)
{
  if (this->Array)
    {
    double yrange[2];
    this->Array->GetRange(xrange, yrange, this->PlotError);
    return 1;
    }
  else
    {
    return 0;
    }
}


//----------------------------------------------------------------------------
int vtkMRMLPlotObjectCurve2DNode::GetYRange(double* yrange)
{
  if (this->Array)
    {
    double xrange[2];
    this->Array->GetRange(xrange, yrange, this->PlotError);
    return 1;
    }
  else
    {
    return 0;
    }
}


//----------------------------------------------------------------------------
vtkDataObject* vtkMRMLPlotObjectCurve2DNode::GetDrawObject(double* xrange, double* yrange)
{
  if (this->Array)
    {
    vtkDoubleArray* array = this->Array->GetArray();
    vtkFieldData* fieldData = vtkFieldData::New();
  
    if (this->PlotError)
      {
      // if error bar plotting is enabled, generate plot data with error bars.
      vtkDoubleArray* data = CreatePlotDataWithErrorBar(array);
      fieldData->AddArray(data);
      data->Delete();
      }
    else
      {
      fieldData->AddArray(array);
      }
    
    vtkDataObject* dataObject = vtkDataObject::New();
    dataObject->SetFieldData( fieldData );
    
    this->PlotActor->AddDataObjectInput(dataObject);
    
    this->PlotActor->SetDataObjectXComponent(obj, 0);
    this->PlotActor->SetDataObjectYComponent(obj, 1);
    this->PlotActor->SetPlotColor(obj, r, g, b);
    
    fieldData->Delete();
    return dataObject;
    }
  else
    {
    return NULL;
    }

}


//----------------------------------------------------------------------------
vtkDoubleArray* vtkMRMLPlotObjectCurve2DNode::CreatePlotDataWithErrorBar(vtkDoubleArray* srcData)
{
  vtkDoubleArray* plotData;
  plotData = vtkDoubleArray::New();
  plotData->SetNumberOfComponents( static_cast<vtkIdType>(2) );
  
  // Check if the data exists and it has 3 components (x, y, and error)
  if (!srcData || srcData->GetNumberOfComponents() != static_cast<vtkIdType>(3))
    {
    int nData = srcData->GetNumberOfTuples();
    for (int i = 0; i < nData; i ++)
      {
      double* values = srcData->GetTuple(i);
      plotData->InsertNextTuple( values );
      }
    return plotData;
    }

  // Note: Error bar
  //
  //        p2
  //   p0 --+-- p1    ---
  //        |          ^
  //        *          | 2 * error
  //        |          v
  //   p4 --+-- p5    ---   
  //        p3
  //
  //      |<->| error bar width * 2
  //
  // 'error bar width' = ((this->RangeX[1] - this->RangeX[0]) / nData) / 8
  //
  
  int nData = srcData->GetNumberOfTuples();
  double errorBarWidth;

  if (nData > 10)
    {
    errorBarWidth = ((this->RangeX[1] - this->RangeX[0]) / (double)nData) / 8.0;
    }
  else
    {
    errorBarWidth = ((this->RangeX[1] - this->RangeX[0]) / 10.0) / 8.0;
    }
  
  for (int j = 0; j < nData; j ++)
    {
    double* values = srcData->GetTuple(j);
    double p[6][2];
    
    // set x coordinates
    p[0][0] = p[4][0] = values[0] - errorBarWidth;
    p[2][0] = p[3][0] = values[0];
    p[1][0] = p[5][0] = values[0] + errorBarWidth;
    
    // set y coordinates
    p[0][1] = p[1][1] = p[2][1] = values[1] + values[2];
    p[3][1] = p[4][1] = p[5][1] = values[1] - values[2];
    
    // Add data point
    plotData->InsertNextTuple( values );

    // Add points for error bars
    plotData->InsertNextTuple( p[2] );
    plotData->InsertNextTuple( p[0] );
    plotData->InsertNextTuple( p[1] );
    plotData->InsertNextTuple( p[2] );
    plotData->InsertNextTuple( p[3] );
    plotData->InsertNextTuple( p[4] );
    plotData->InsertNextTuple( p[5] );
    plotData->InsertNextTuple( p[3] );
    plotData->InsertNextTuple( values );
    }

  return plotData;
}
