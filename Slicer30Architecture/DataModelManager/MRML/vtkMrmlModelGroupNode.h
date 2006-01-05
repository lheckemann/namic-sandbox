/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlModelGroupNode.h,v $
  Date:      $Date: 2005/12/20 22:44:26 $
  Version:   $Revision: 1.5.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlModelGroupNode_h
#define __vtkMrmlModelGroupNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlModelGroupNode : public vtkMrmlNode
{
public:
  static vtkMrmlModelGroupNode *New();
  vtkTypeMacro(vtkMrmlModelGroupNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // ID of the model group
  vtkSetStringMacro(ModelGroupID);
  vtkGetStringMacro(ModelGroupID);
  
  // Description:
  // Name of the model group's color, which is defined by a Color node
  vtkSetStringMacro(Color);
  vtkGetStringMacro(Color);
  
  // Description:
  // Opacity of the surface expressed as a number from 0 to 1
  vtkSetMacro(Opacity, float);
  vtkGetMacro(Opacity, float);
  
  // Description:
  // Indicates if the surface is visible
  vtkBooleanMacro(Visibility, int);
  vtkSetMacro(Visibility, int);
  vtkGetMacro(Visibility, int);

  // Description:
  // Indicates if the ModelGroup is displayed expanded or collapsed
  vtkBooleanMacro(Expansion, int);
  vtkSetMacro(Expansion, int);
  vtkGetMacro(Expansion, int);
   
};

#endif
