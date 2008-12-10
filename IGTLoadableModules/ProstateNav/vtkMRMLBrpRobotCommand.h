/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLBrpRobotCommandNode.h,v $
  Date:      $Date: 2006/03/19 17:12:28 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/
// .NAME vtkMRMLBrpRobotCommandNode - MRML node to represent BRP Prostate Robot
// .SECTION Description
//

#ifndef __vtkMRMLBrpRobotCommandNode_h
#define __vtkMRMLBrpRobotCommandNode_h

#include "vtkPolyData.h" 

#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLModelNode.h"
//#include "vtkMRMLBrpRobotCommandDisplayNode.h"
//#include "vtkMRMLBrpRobotCommandLineDisplayNode.h"
//#include "vtkMRMLBrpRobotCommandTubeDisplayNode.h"
//#include "vtkMRMLBrpRobotCommandGlyphDisplayNode.h"

#include "string"

class vtkCallbackCommand;
class vtkSphereSource;

class VTK_MRML_EXPORT vtkMRMLBrpRobotCommandNode : public vtkMRMLModelNode
{
 public:
  static vtkMRMLBrpRobotCommandNode *New();
  vtkTypeMacro(vtkMRMLBrpRobotCommandNode,vtkMRMLModelNode);
  //vtkTypeMacro(vtkMRMLBrpRobotCommandNode,vtkMRMLTransformableNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "BrpRobotCommand";};

  

 protected:
  vtkMRMLBrpRobotCommandNode(){};
  ~vtkMRMLBrpRobotCommandNode(){};
  vtkMRMLBrpRobotCommandNode(const vtkMRMLBrpRobotCommandNode&);
  void operator=(const vtkMRMLBrpRobotCommandNode&);
  
 protected:
  
  //BTX
  std::queue<std::string> InCommandQueue;
  std::queue<std::string> OutCommandQueue;
  //ETX

};

#endif
