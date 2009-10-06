/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLTransRectalProstateRobotNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"

#include "vtkMRMLTransRectalProstateRobotNode.h"
#include "vtkMRMLScene.h"

//------------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode* vtkMRMLTransRectalProstateRobotNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransRectalProstateRobotNode");
  if(ret)
    {
      return (vtkMRMLTransRectalProstateRobotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransRectalProstateRobotNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode* vtkMRMLTransRectalProstateRobotNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransRectalProstateRobotNode");
  if(ret)
    {
      return (vtkMRMLTransRectalProstateRobotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransRectalProstateRobotNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode::vtkMRMLTransRectalProstateRobotNode()
{
  
}

//----------------------------------------------------------------------------
vtkMRMLTransRectalProstateRobotNode::~vtkMRMLTransRectalProstateRobotNode()
{
}


//----------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

}


//----------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts);

}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLTransRectalProstateRobotNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
  vtkMRMLTransRectalProstateRobotNode *node = (vtkMRMLTransRectalProstateRobotNode *) anode;

  //int type = node->GetType();
  
}


void vtkMRMLTransRectalProstateRobotNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
  return;
}


//----------------------------------------------------------------------------
void vtkMRMLTransRectalProstateRobotNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


