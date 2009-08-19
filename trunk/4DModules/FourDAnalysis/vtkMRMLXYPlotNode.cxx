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
int vtkMRMLXYPlotNode::AddArrayNode(vtkMRMLDoubleArrayNode* node)
{
  PlotDataType data;

  // default color is black
  data.lineColor[0] = 0.0;
  data.lineColor[1] = 0.0;
  data.lineColor[2] = 0.0;

  data.visibility = 1;
  data.arrayNode  = node;
  data.errorBar   = 0;

  // Check if the number of arrays hasn't reached to the maximum
  
  if (this->Data.size() >= 10000)
    {
    return -1;
    }

  // Find unique ID
  int next_id = (this->LastArrayID + 1) % 10000;
  std::map< int, PlotDataType >::iterator iter;
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
  this->Data[this->LastArrayID] = data;

  this->Modified();

  return this->LastArrayID;
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::RemoveArrayNode(int id)
{
  
  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    this->Data.erase(iter);
    }

  this->Modified();
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::RemoveArrayNodeByNodeID(const char* nodeID)
{

  std::map< int, PlotDataType >::iterator iter;

  for (iter = this->Data.begin(); iter != this->Data.end(); iter ++)
    {
    vtkMRMLDoubleArrayNode* anode = iter->second.arrayNode;
    if (anode && strcmp(anode->GetID(), nodeID) == 0)
      {
      this->Data.erase(iter);
      }
    }

  this->Modified();
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::RemoveAllArrayNodes()
{
  this->Data.clear();
  this->LastArrayID = -1;
  this->Modified();
}


//----------------------------------------------------------------------------
unsigned int vtkMRMLXYPlotNode::GetNumberOfArrays()
{
  this->Data.size();
}


//----------------------------------------------------------------------------
vtkIntArray* vtkMRMLXYPlotNode::GetArrayNodeIDList()
{

  vtkIntArray* list;
    
  list = vtkIntArray::New();

  std::map< int, PlotDataType >::iterator iter;
  for (iter = this->Data.begin(); iter != this->Data.end(); iter ++)
    {
    
    list->InsertNextValue(iter->first);
    }

  return list;

}


//----------------------------------------------------------------------------
vtkMRMLDoubleArrayNode* vtkMRMLXYPlotNode::GetArrayNode(int id)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    return iter->second.arrayNode;
    }
  else
    {
    return NULL;
    }
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetColor(int id, double r, double g, double b)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    iter->second.lineColor[0] = r;
    iter->second.lineColor[1] = g;
    iter->second.lineColor[2] = b;
    this->Modified();
    }

}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::GetColor(int id, double* r, double* g, double* b)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    *r = iter->second.lineColor[0];
    *g = iter->second.lineColor[1];
    *b = iter->second.lineColor[2];
    }
  else
    {
    *r = 0.0;
    *g = 0.0;
    *b = 0.0;
    }
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetLineName(int id, const char* str)
{
  
  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    vtkMRMLDoubleArrayNode* anode = iter->second.arrayNode;
    if (anode)
      {
      anode->SetName(str);
      this->Modified();
      }
    }
}


//----------------------------------------------------------------------------
const char* vtkMRMLXYPlotNode::GetLineName(int id)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    vtkMRMLDoubleArrayNode* anode = iter->second.arrayNode;
    if (anode)
      {
      return anode->GetName();
      }
    }
  else
    {
    return NULL;
    }
  
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetVisibility(int id, int i)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    iter->second.visibility = i;
    }
}



//----------------------------------------------------------------------------
int vtkMRMLXYPlotNode::GetVisibility(int id)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    return iter->second.visibility;
    }
  else
    {
    return 0;
    }
}



//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetVisibilityAll(int i)
{
  std::map< int, PlotDataType >::iterator iter;

  for (iter = this->Data.begin();
       iter != this->Data.end();
       iter ++)
    {
    (iter->second).visibility = i;
    }

}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetErrorBar(int id, int i)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
   iter->second.errorBar = i;
    }
}


//----------------------------------------------------------------------------
int vtkMRMLXYPlotNode::GetErrorBar(int id)
{

  std::map< int, PlotDataType >::iterator iter;
  iter = this->Data.find(id);

  if (iter != this->Data.end())
    {
    return iter->second.errorBar;
    }
  else
    {
    return 0;
    }
}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::SetErrorBarAll(int i)
{
  std::map< int, PlotDataType >::iterator iter;
  
  for (iter = this->Data.begin();
       iter != this->Data.end();
       iter ++)
    {
    (iter->second).errorBar = i;
    }

}


//----------------------------------------------------------------------------
void vtkMRMLXYPlotNode::Plot()
{
  this->InvokeEvent(UpdateGraphEvent);
}







