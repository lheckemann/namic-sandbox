/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "vtkObjectFactory.h"

#include "vtkMRMLCurveAnalysisNode.h"
#include "vtkMRMLScene.h"

//------------------------------------------------------------------------------
vtkMRMLCurveAnalysisNode* vtkMRMLCurveAnalysisNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLCurveAnalysisNode");
  if(ret)
    {
      return (vtkMRMLCurveAnalysisNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLCurveAnalysisNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLCurveAnalysisNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLCurveAnalysisNode");
  if(ret)
    {
      return (vtkMRMLCurveAnalysisNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLCurveAnalysisNode;
}

//----------------------------------------------------------------------------
vtkMRMLCurveAnalysisNode::vtkMRMLCurveAnalysisNode()
{
  this->HideFromEditors = true;
}

//----------------------------------------------------------------------------
vtkMRMLCurveAnalysisNode::~vtkMRMLCurveAnalysisNode()
{
}


//----------------------------------------------------------------------------
void vtkMRMLCurveAnalysisNode::WriteXML(ostream& of, int nIndent)
{
  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLCurveAnalysisNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLCurveAnalysisNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLCurveAnalysisNode *node = (vtkMRMLCurveAnalysisNode *) anode;
}


//----------------------------------------------------------------------------
void vtkMRMLCurveAnalysisNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkMRMLCurveAnalysisNode::SetParameter(const char *name, const double value)
{
}


//----------------------------------------------------------------------------
vtkStringArray* vtkMRMLCurveAnalysisNode::GetParameterNameArray()
{
  OutputParameterMapType::iterator iter;

  vtkStringArray* array = vtkStringArray::New();

  for (iter = this->OutputParameters.begin(); iter != this->OutputParameters.begin(); iter ++)
    {
    array->InsertNextValue(iter->first.c_str());
    }

  return array;
}


//----------------------------------------------------------------------------
double vtkMRMLCurveAnalysisNode::GetParameter(const char* name)
{
  OutputParameterMapType::iterator iter;
  iter = this->OutputParameters.find(name);
  if (iter != this->OutputParameters.end())
    {
    return this->OutputParameters[name]; 
    }
  else
    {
    return 0.0;
    }
}


//----------------------------------------------------------------------------
void vtkMRMLCurveAnalysisNode::ClearParameters()
{
  this->OutputParameters.clear();
}



