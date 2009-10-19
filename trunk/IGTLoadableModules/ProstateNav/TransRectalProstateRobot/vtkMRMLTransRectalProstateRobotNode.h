/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTransRectalProstateRobotNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLTransRectalRobotNode_h
#define __vtkMRMLTransRectalRobotNode_h

#include "vtkOpenIGTLinkIFWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLRobotNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkObject.h"
#include "vtkProstateNavWin32Header.h" 

class VTK_PROSTATENAV_EXPORT vtkMRMLTransRectalProstateRobotNode : public vtkMRMLRobotNode
{

 public:

 public:

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLTransRectalProstateRobotNode *New();
  vtkTypeMacro(vtkMRMLTransRectalProstateRobotNode,vtkMRMLRobotNode);
  
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLTransRectalProstateRobotNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "TransRectalProstateRobot";};

  virtual const char* GetWorkflowStepsString()
    {return "PointTargeting PointVerification"; };

  // method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  virtual bool FindTargetingParams(vtkProstateNavTargetDescriptor *targetDesc);
  //BTX
  virtual std::string GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc);
  //ETX


 protected:
  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------
  
  vtkMRMLTransRectalProstateRobotNode();
  ~vtkMRMLTransRectalProstateRobotNode();
  vtkMRMLTransRectalProstateRobotNode(const vtkMRMLTransRectalProstateRobotNode&);
  void operator=(const vtkMRMLTransRectalProstateRobotNode&);

 protected:
  //  
  
};

#endif

