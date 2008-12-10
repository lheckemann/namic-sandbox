/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLBrpRobotCommandNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLBrpRobotCommandNode.h"
#include "vtkMRMLDiffusionTensorDisplayPropertiesNode.h"

#include "vtkSphereSource.h"

//------------------------------------------------------------------------------
vtkMRMLBrpRobotCommandNode* vtkMRMLBrpRobotCommandNode::New()
{
  vtkMRMLBrpRobotCommandNode* ret;

  // First try to create the object from the vtkObjectFactory
  vtkObject* r = vtkObjectFactory::CreateInstance("vtkMRMLBrpRobotCommandNode");
  if(r)
    {
    ret =  (vtkMRMLBrpRobotCommandNode*)ret;
    }
  else
    {
    // If the factory was unable to create the object, then create it here.
    ret =  new vtkMRMLBrpRobotCommandNode;
    }

  ret->Sphere = vtkSphereSource::New();

  double c[] = {0.0, 0.0, 0.0};
  ret->SetParameters(c, 1.0, 1.0);
  ret->SetAndObservePolyData(ret->Sphere->GetOutput());

  return ret;
}

//-----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLBrpRobotCommandNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLBrpRobotCommandNode");
  if(ret)
    {
    return (vtkMRMLBrpRobotCommandNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLBrpRobotCommandNode;
}

//----------------------------------------------------------------------------
void vtkMRMLBrpRobotCommandNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
void vtkMRMLBrpRobotCommandNode::SetParameters(double center[3], double radius, double hardness)
{
  this->Center[0] = center[0];
  this->Center[1] = center[1];
  this->Center[2] = center[2];
  this->Radius = radius;
  this->Hardness = hardness;

  if (this->Sphere)
    {
    this->Sphere->SetRadius(radius);
    this->Sphere->SetCenter(center);
    this->Sphere->Update();
    }

  this->Modified();
  this->InvokeEvent(vtkMRMLBrpRobotCommandNode::DisplayModifiedEvent, NULL);

}


//----------------------------------------------------------------------------
void vtkMRMLBrpRobotCommandNode::GetParameters(double* center, double* radius, double* hardness)
{
  center[0] = this->Center[0];
  center[1] = this->Center[1];
  center[2] = this->Center[2];
  *radius   = this->Radius;
  *hardness = this->Hardness;
}
