/*=auto=========================================================================

Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLHybridNavToolNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "vtkObjectFactory.h"
#include "vtkMRMLHybridNavToolNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"


//------------------------------------------------------------------------------
vtkMRMLHybridNavToolNode* vtkMRMLHybridNavToolNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLHybridNavToolNode");
  if(ret)
    {
      return (vtkMRMLHybridNavToolNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLHybridNavToolNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLHybridNavToolNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLHybridNavToolNode");
  if(ret)
    {
      return (vtkMRMLHybridNavToolNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLHybridNavToolNode;
}

//----------------------------------------------------------------------------
vtkMRMLHybridNavToolNode::vtkMRMLHybridNavToolNode()
{
  this->HideFromEditors = 0;

  this->Calibrated = 0;
  this->ToolName = "";
  this->ToolDescription = "Description";
  transformNode = NULL;
}

//----------------------------------------------------------------------------
vtkMRMLHybridNavToolNode::~vtkMRMLHybridNavToolNode()
{
  if (transformNode)
    {
    this->transformNode->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLHybridNavToolNode::WriteXML(ostream& of, int nIndent)
{

}

//----------------------------------------------------------------------------
void vtkMRMLHybridNavToolNode::ReadXMLAttributes(const char** atts)
{

}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLHybridNavToolNode::Copy(vtkMRMLNode *anode)
{

}

//----------------------------------------------------------------------------
void vtkMRMLHybridNavToolNode::PrintSelf(ostream& os, vtkIndent indent)
{

}

//---------------------------------------------------------------------------
void vtkMRMLHybridNavToolNode::SetToolNode(vtkMRMLNode* node)
{
  //cast the node to a vtkMRMLLinearNode
  if (node)
  {
    this->transformNode = vtkMRMLLinearTransformNode::SafeDownCast(node);
  }
}

//--------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLHybridNavToolNode::GetToolNode()
{
  return this->transformNode;
}
