/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLOsteoPlanNode.h,v $
  Date:      $Date: 2009/10/15 17:12:29 $
  Version:   $Revision: 1.3 $

  =========================================================================auto=*/

#ifndef VTKMRMLOSTEOPLANNODE_H_
#define VTKMRMLOSTEOPLANNODE_H_

#include "vtkOsteoPlanWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLStorageNode.h"
#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkCollection.h"

class VTK_OsteoPlan_EXPORT vtkMRMLOsteoPlanNode : public vtkMRMLNode
{

 public:
  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------
  static vtkMRMLOsteoPlanNode *New();
  vtkTypeMacro(vtkMRMLOsteoPlanNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Create instance of a HybridNavTool node.
  virtual vtkMRMLNode* CreateNodeInstance();

  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts);

  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "OsteoPlanNode";};

  //Disactivate the possibility to apply non-linear transforms
  virtual bool CanApplyNonLinearTransforms() {return 0;};

 protected:
  //----------------------------------------------------------------
  // Constructor and destructor
  //----------------------------------------------------------------
  vtkMRMLOsteoPlanNode();
  ~vtkMRMLOsteoPlanNode();
  vtkMRMLOsteoPlanNode(const vtkMRMLOsteoPlanNode&);
  void operator=(const vtkMRMLOsteoPlanNode&);

 public:
  //----------------------------------------------------------------
  // Variables
  //----------------------------------------------------------------
  vtkGetObjectMacro(ListOfModels, vtkCollection);
  vtkGetObjectMacro(ListOfTransforms, vtkCollection);

  vtkCollection *ListOfModels;
  vtkCollection *ListOfTransforms;


 private:

};

#endif
