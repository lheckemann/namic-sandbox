/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTrajectoryNode.cxx,v $
  Date:      $Date: 2006/03/17 15:10:10 $
  Version:   $Revision: 1.2 $

  =========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "vtkObjectFactory.h"
#include "vtkMRMLTrajectoryNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"

#include "vtkObjectFactory.h"


//------------------------------------------------------------------------------
vtkMRMLTrajectoryNode* vtkMRMLTrajectoryNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTrajectoryNode");
  if(ret)
    {
    return (vtkMRMLTrajectoryNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTrajectoryNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLTrajectoryNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTrajectoryNode");
  if(ret)
    {
    return (vtkMRMLTrajectoryNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTrajectoryNode;
}

//----------------------------------------------------------------------------
vtkMRMLTrajectoryNode::vtkMRMLTrajectoryNode()
{
  this->HideFromEditorsOff();

  this->TrajectoryName = "DefaultTrajectory";
  this->TrajectoryGroupName = "TrajectoryGroup";

  this->RGBA[0] = 0.5;
  this->RGBA[1] = 0.5;
  this->RGBA[2] = 0.5;
  this->RGBA[3] = 0.5;

  this->P1[0] = 0;
  this->P1[1] = 0;
  this->P1[2] = 0;

  this->P2[0] = 0;
  this->P2[1] = 0;
  this->P2[2] = 0;

  this->diameter = 0.0;

  this->OwnerImage = "";

}

//----------------------------------------------------------------------------
vtkMRMLTrajectoryNode::~vtkMRMLTrajectoryNode()
{

}

//----------------------------------------------------------------------------
void vtkMRMLTrajectoryNode::WriteXML(ostream& of, int nIndent)
{

}

//----------------------------------------------------------------------------
void vtkMRMLTrajectoryNode::ReadXMLAttributes(const char** atts)
{

}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLTrajectoryNode::Copy(vtkMRMLNode *anode)
{

}

//----------------------------------------------------------------------------
void vtkMRMLTrajectoryNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "\n";
  os << indent << "TrajectoryName: " << this->TrajectoryName.c_str() << "\n";
  os << indent << "TrajectoryGroupName: " << this->TrajectoryGroupName.c_str() << "\n";
  os << indent << "RGBA: " << this->RGBA[0] << "," << this->RGBA[1] << "," << this->RGBA[2] << "," << this->RGBA[3] << "\n";
  os << indent << "P1: (" << this->P1[0] << "," << this->P1[1] << "," << this->P1[2] << ")\n";
  os << indent << "P2: (" << this->P2[0] << "," << this->P2[1] << "," << this->P2[2] << ")\n";
  os << indent << "Diameter: " << this->diameter << "\n";
  os << indent << "OwnerImage: " << this->OwnerImage.c_str() << "\n";
}
