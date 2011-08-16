/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLOsteoPlanNode.cxx,v $
  Date:      $Date: 2006/03/17 15:10:10 $
  Version:   $Revision: 1.2 $

  =========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "vtkObjectFactory.h"
#include "vtkMRMLOsteoPlanNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"

#include "vtkObjectFactory.h"
#include "vtkCollection.h"


//------------------------------------------------------------------------------
vtkMRMLOsteoPlanNode* vtkMRMLOsteoPlanNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLOsteoPlanNode");
  if(ret)
    {
    return (vtkMRMLOsteoPlanNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLOsteoPlanNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLOsteoPlanNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLOsteoPlanNode");
  if(ret)
    {
    return (vtkMRMLOsteoPlanNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLOsteoPlanNode;
}

//----------------------------------------------------------------------------
vtkMRMLOsteoPlanNode::vtkMRMLOsteoPlanNode()
{
  this->ListOfModels     = vtkCollection::New();
  this->ListOfTransforms = vtkCollection::New();
}

//----------------------------------------------------------------------------
vtkMRMLOsteoPlanNode::~vtkMRMLOsteoPlanNode()
{
  if(this->ListOfModels)
    {
    this->ListOfModels->Delete();
    }

  if(this->ListOfTransforms)
    {
    this->ListOfTransforms->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLOsteoPlanNode::WriteXML(ostream& of, int nIndent)
{
}

//----------------------------------------------------------------------------
void vtkMRMLOsteoPlanNode::ReadXMLAttributes(const char** atts)
{
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLOsteoPlanNode::Copy(vtkMRMLNode *anode)
{
}

//----------------------------------------------------------------------------
void vtkMRMLOsteoPlanNode::PrintSelf(ostream& os, vtkIndent indent)
{
}
