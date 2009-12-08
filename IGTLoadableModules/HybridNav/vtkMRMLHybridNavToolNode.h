/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLHybridNavToolNode.h,v $
  Date:      $Date: 2009/10/15 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/

#ifndef VTKMRMLHYBRIDNAVTOOLNODE_H_
#define VTKMRMLHYBRIDNAVTOOLNODE_H_

#include "vtkHybridNavWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLStorageNode.h"
#include "vtkObject.h"
#include "vtkMatrix4x4.h"

class VTK_HybridNav_EXPORT vtkMRMLHybridNavToolNode : public vtkMRMLModelNode
{

public:
  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------
  static vtkMRMLHybridNavToolNode *New();
  vtkTypeMacro(vtkMRMLHybridNavToolNode, vtkMRMLModelNode);
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
  virtual const char* GetNodeTagName() {return "HybridNavTool";};
  
  //Disactivate the possibility to apply non-linear transforms
  virtual bool CanApplyNonLinearTransforms() {return 0;};

protected:
  //----------------------------------------------------------------
  // Constructor and destructor
  //----------------------------------------------------------------
  vtkMRMLHybridNavToolNode();
  ~vtkMRMLHybridNavToolNode();
  vtkMRMLHybridNavToolNode(const vtkMRMLHybridNavToolNode&);
  void operator=(const vtkMRMLHybridNavToolNode&);

public:
  //----------------------------------------------------------------
  // Tool properties
  //----------------------------------------------------------------

  vtkGetMacro( Calibrated, int );
  vtkSetMacro( Calibrated, int );

  // tool name, description, node and visibility
  void SetToolName(const char* str) { this->ToolName = str; }
  const char* GetToolName() { return this->ToolName.c_str(); }
  void SetToolDescription(const char* str) { this->ToolDescription = str; }
  const char* GetToolDescription() { return this->ToolDescription.c_str(); }

private:
  //BTX
  std::string ToolName;
  std::string ToolDescription;
  //ETX
  int Calibrated;
  vtkMatrix4x4* calMatrix;        // Tranformation matrix from sensor to tool tip (calibration)

};

#endif /* VTKMRMLHYBRIDNAVNODETOOL_H_ */
