/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVirtualFixtureNode.h,v $
  Date:      $Date: 2006/03/19 17:12:28 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/
// .NAME vtkMRMLVirtualFixtureNode - MRML node to represent Virtual Fixture.
// .SECTION Description
//

#ifndef __vtkMRMLVirtualFixtureNode_h
#define __vtkMRMLVirtualFixtureNode_h

#include "vtkPolyData.h" 

#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLModelNode.h"
//#include "vtkMRMLVirtualFixtureDisplayNode.h"
//#include "vtkMRMLVirtualFixtureLineDisplayNode.h"
//#include "vtkMRMLVirtualFixtureTubeDisplayNode.h"
//#include "vtkMRMLVirtualFixtureGlyphDisplayNode.h"

class vtkCallbackCommand;
class vtkSphereSource;

class VTK_MRML_EXPORT vtkMRMLVirtualFixtureNode : public vtkMRMLModelNode
{
public:
  static vtkMRMLVirtualFixtureNode *New();
  vtkTypeMacro(vtkMRMLVirtualFixtureNode,vtkMRMLModelNode);
  //vtkTypeMacro(vtkMRMLVirtualFixtureNode,vtkMRMLTransformableNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "VirtualFixture";};

  void GetParameters(double* center, double* radius, double* hardness);
  void SetParameters(double center[3], double radius, double hardness);

protected:
  vtkMRMLVirtualFixtureNode(){};
  ~vtkMRMLVirtualFixtureNode(){};
  vtkMRMLVirtualFixtureNode(const vtkMRMLVirtualFixtureNode&);
  void operator=(const vtkMRMLVirtualFixtureNode&);

protected:
  double            Center[3];
  double            Radius;
  double            Hardness;
  vtkSphereSource*  Sphere;
};

#endif
