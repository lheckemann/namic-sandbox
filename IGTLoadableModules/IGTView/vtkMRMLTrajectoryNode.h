/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTrajectoryNode.h,v $
  Date:      $Date: 2009/10/15 17:12:29 $
  Version:   $Revision: 1.3 $

  =========================================================================auto=*/

#ifndef VTKMRMLTRAJECTORYNODE_H_
#define VTKMRMLTRAJECTORYNODE_H_

#include "vtkIGTViewWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLStorageNode.h"
#include "vtkObject.h"
#include "vtkMatrix4x4.h"

class VTK_IGTView_EXPORT vtkMRMLTrajectoryNode : public vtkMRMLNode
{

 public:
  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------
  static vtkMRMLTrajectoryNode *New();
  vtkTypeMacro(vtkMRMLTrajectoryNode, vtkMRMLNode);
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
  virtual const char* GetNodeTagName() {return "Trajectory";};

  //Disactivate the possibility to apply non-linear transforms
  virtual bool CanApplyNonLinearTransforms() {return 0;};

 protected:
  //----------------------------------------------------------------
  // Constructor and destructor
  //----------------------------------------------------------------
  vtkMRMLTrajectoryNode();
  ~vtkMRMLTrajectoryNode();
  vtkMRMLTrajectoryNode(const vtkMRMLTrajectoryNode&);
  void operator=(const vtkMRMLTrajectoryNode&);

 public:


  //BTX
  std::string GetTrajectoryName(){return this->TrajectoryName;};
  void SetTrajectoryName(std::string n){this->TrajectoryName = n;};
  std::string GetTrajectoryGroupName(){return this->TrajectoryGroupName;};
  void SetTrajectoryGroupName(std::string gn){this->TrajectoryGroupName = gn;};
  //ETX

  vtkSetMacro(Type, int);
  vtkGetMacro(Type, int);
  vtkSetVector4Macro(RGBA, int);
  vtkGetVector4Macro(RGBA, int);
  vtkSetVector3Macro(P1, float);
  vtkGetVector3Macro(P1, float);
  vtkSetVector3Macro(P2, float);
  vtkGetVector3Macro(P2, float);
  vtkSetMacro(diameter, float);
  vtkGetMacro(diameter, float);

  //BTX
  std::string GetOwnerImage(){return this->OwnerImage;};
  void SetOwnerImage(std::string oi){this->OwnerImage = oi;};
  //ETX


 private:

  //BTX
  std::string TrajectoryName;
  std::string TrajectoryGroupName;
  int Type;
  int RGBA[4];
  float P1[3];
  float P2[3];
  float diameter;
  std::string OwnerImage;
  //ETX

};

#endif
