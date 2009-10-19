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
#include "vtkProstateNavTargetDescriptor.h"

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


//----------------------------------------------------------------------------
bool vtkMRMLTransRectalProstateRobotNode::FindTargetingParams(vtkProstateNavTargetDescriptor *targetDesc)
{
  // this is used for coverage area computation (IsOutsideReach means that the target is outside the robot's coverage area)

  // :TODO: perform real targeting parameter computation  
  double *ras=targetDesc->GetRASLocation();
  const double center[3]={0,0,0};
  const double radius2=25*25;
  targetDesc->SetIsOutsideReach(
    (ras[0]-center[0])*(ras[0]-center[0])+
    (ras[1]-center[1])*(ras[1]-center[1])+
    (ras[2]-center[2])*(ras[2]-center[2])>radius2
    );
  return true;
}


std::string vtkMRMLTransRectalProstateRobotNode::GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc)
{
  std::ostrstream os;  
  os << "Needle type:"<<targetDesc->GetNeedleTypeString()<<std::endl;
  os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(1);
  os << "RAS location: "<<targetDesc->GetRASLocationString().c_str()<<std::endl;
  os << "Reachable: "<<targetDesc->GetReachableString().c_str()<<std::endl;
  os << "Depth: "<<targetDesc->GetDepthCM()<<" cm"<<std::endl;
  os << "Device rotation: "<<targetDesc->GetAxisRotation()<<" deg"<<std::endl;
  os << "Needle angle: "<<targetDesc->GetNeedleAngle()<<" deg"<<std::endl;
  os << std::ends;
  std::string result=os.str();
  os.rdbuf()->freeze();
  return result;
}
