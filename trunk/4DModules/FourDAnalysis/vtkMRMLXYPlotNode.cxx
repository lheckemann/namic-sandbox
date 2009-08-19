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

#include "vtkMRMLXYPlotNode.h"

#include "vtkMRMLScene.h"

#include "vtkMRMLPlotObjectCurve2DNode.h"

//------------------------------------------------------------------------------
vtkMRMLXYPlotNode* vtkMRMLXYPlotNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLXYPlotNode"); if(ret)
    {
      return (vtkMRMLXYPlotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLXYPlotNode;
}

//----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLXYPlotNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLXYPlotNode");
  if(ret)
    {
      return (vtkMRMLXYPlotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLXYPlotNode;
}

//----------------------------------------------------------------------------
vtkMRMLXYPlotNode::vtkMRMLXYPlotNode()
{

  this->Title  = "";
  this->XLabel = "";
  this->YLabel = "";

  this->Data.clear();
  this->LastArrayID = -1;

  //this->VerticalLines.clear();
  //this->HorizontalLines.clear();

  this->XRange[0] = 0.0;
  this->XRange[1] = 0.0;
  this->YRange[0] = 0.0;
  this->YRange[1] = 0.0;

  this->AutoXRange = 0.0;
  this->AutoYRange = 0.0;

}


//----------------------------------------------------------------------------
vtkMRMLXYPlotNode::~vtkMRMLXYPlotNode()
{
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  //std::stringstream ssX;
  //std::stringstream ssY;
  //std::stringstream ssYerr;
  //
  //int n = this->Array->GetNumberOfComponents();
  //double xy[3];
  //
  //if (this->Array->GetNumberOfComponents() > 3)
  //  {
  //  // Put values to the string streams except the last values.
  //  n = this->Array->GetSize() - 1;
  //  for (int i = 0; i < n; i ++)
  //    {
  //    this->Array->GetTupleValue(i, xy);
  //    ssX    << xy[0] << ", ";
  //    ssY    << xy[1] << ", ";
  //    ssYerr << xy[2] << ", ";
  //    }
  //  this->Array->GetTupleValue(n);
  //  // put the last values
  //  ssX    << xy[0];
  //  ssY    << xy[1];
  //  ssYerr << xy[2];
  //  }
  //
  //of << " valueX=\"" << ss.str() << "\"";
  //of << " valueY=\"" << ss.str() << "\"";
  //of << " valueYErr=\"" << ss.str() << "\"";

}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  vtkMRMLNode::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;

  std::vector<double> valueX;
  std::vector<double> valueY;
  std::vector<double> valueYErr;

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
void vtkMRMLXYPlotNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
  vtkMRMLXYPlotNode *node = (vtkMRMLXYPlotNode *) anode;

  //int type = node->GetType();
  
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
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
void vtkMRMLXYPlotNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
int vtkMRMLXYPlotNode::AddPlot(vtkMRMLPlotObjectNode* node)
{

  // Check if the number of arrays hasn't reached to the maximum
  
  if (this->Data.size() >= 10000)
    {
    return -1;
    }

  // Find unique ID
  int next_id = (this->LastArrayID + 1) % 10000;
  std::map< int, vtkMRMLPlotObjectNode* >::iterator iter;
  while (1)
    {
    iter = this->Data.find(next_id);
    if (iter == this->Data.end())
      {
      break;
      }
    next_id = (next_id + 1) % 10000;
    }
  this->LastArrayID = next_id;
  this->Data[this->LastArrayID] = node;

  this->Modified();

  return this->LastArrayID;
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::RemovePlot(int id)
{
  
  std::map< int, vtkMRMLPlotObjectNode* >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    this->Data.erase(iter);
    }

  this->Modified();
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::RemovePlotByNodeID(const char* nodeID)
{

  std::map< int, vtkMRMLPlotObjectNode* >::iterator iter;

  for (iter = this->Data.begin(); iter != this->Data.end(); iter ++)
    {
    vtkMRMLPlotObjectNode* anode = iter->second;
    if (anode && strcmp(anode->GetID(), nodeID) == 0)
      {
      this->Data.erase(iter);
      }
    }

  this->Modified();
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::ClearPlots()
{
  this->Data.clear();
  this->LastArrayID = -1;
  this->Modified();
}


//----------------------------------------------------------------------------
unsigned int vtkMRMLXYPlotNode::GetNumberOfPlots()
{
  this->Data.size();
}


//----------------------------------------------------------------------------
vtkIntArray* vtkMRMLXYPlotNode::GetPlotIDList()
{

  vtkIntArray* list;
    
  list = vtkIntArray::New();
  
  std::map< int, vtkMRMLPlotObjectNode* >::iterator iter;
  for (iter = this->Data.begin(); iter != this->Data.end(); iter ++)
    {
    
    list->InsertNextValue(iter->first);
    }

  return list;

}


//----------------------------------------------------------------------------
vtkMRMLPlotObjectNode* vtkMRMLXYPlotNode::GetPlot(int id)
{

  std::map< int, vtkMRMLPlotObjectNode* >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    return iter->second;
    }
  else
    {
    return NULL;
    }
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetVisibilityAll(int i)
{
  std::map< int, vtkMRMLPlotObjectNode* >::iterator iter;

  for (iter = this->Data.begin();
       iter != this->Data.end();
       iter ++)
    {
    vtkMRMLPlotObjectNode* node = (iter->second);
    if (node)
      {
      node->SetVisible(i);
      }
    }

}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetErrorBarAll(int i)
{
  std::map< int, vtkMRMLPlotObjectNode* >::iterator iter;
  
  for (iter = this->Data.begin();
       iter != this->Data.end();
       iter ++)
    {
    vtkMRMLPlotObjectNode* node = (iter->second);
    if (node && strcmp(node->GetNodeTagName(), "PlotObject") == 0)
      {
      vtkMRMLPlotObjectCurve2DNode* cnode = vtkMRMLPlotObjectCurve2DNode::SafeDownCast(node);
      if (cnode)
        {
        cnode->SetPlotError(i);
        }
      }
    }

}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::Refresh()
{
  this->InvokeEvent(UpdateGraphEvent);
}

