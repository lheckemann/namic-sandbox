/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlColorNode.h,v $
  Date:      $Date: 2005/12/20 22:44:21 $
  Version:   $Revision: 1.18.8.1 $

=========================================================================auto=*/


#ifndef __vtkMrmlColorNode_h
#define __vtkMrmlColorNode_h

#include "vtkMrmlNode.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlColorNode : public vtkMrmlNode
{
public:
  static vtkMrmlColorNode *New();
  vtkTypeMacro(vtkMrmlColorNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the nodes attributes to this object to a MRML file in XML format
  void Copy(vtkMrmlNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Get/Set for DiffuseColor
  vtkGetVector3Macro(DiffuseColor, vtkFloatingPointType);
  vtkSetVector3Macro(DiffuseColor, vtkFloatingPointType);

  // Description:
  // Factor of the affect of ambient light from 0 to 1
  vtkGetMacro(Ambient, vtkFloatingPointType);
  vtkSetMacro(Ambient, vtkFloatingPointType);

  // Description:
  // Factor of the affect of diffuse reflection from 0 to 1
  vtkGetMacro(Diffuse, vtkFloatingPointType);
  vtkSetMacro(Diffuse, vtkFloatingPointType);

  // Description:
  // Factor of the affect of specular reflection from 0 to 1
  vtkGetMacro(Specular, vtkFloatingPointType);
  vtkSetMacro(Specular, vtkFloatingPointType);

  // Description:
  // Specular power in the range of 1 to 100
  vtkGetMacro(Power, int);
  vtkSetMacro(Power, int);

  // Description:
  // List of label values associated with this color
  vtkGetStringMacro(Labels);
  vtkSetStringMacro(Labels);

};

#endif









