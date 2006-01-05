/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlOptionsNode.h,v $
  Date:      $Date: 2005/12/20 22:44:28 $
  Version:   $Revision: 1.16.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlOptionsNode_h
#define __vtkMrmlOptionsNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlOptionsNode : public vtkMrmlNode
{
  public:
  static vtkMrmlOptionsNode *New();
  vtkTypeMacro(vtkMrmlOptionsNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the nodes attributes to this object
  void Copy(vtkMrmlNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Name of the program that should recognize these options.
  vtkGetStringMacro(Program);
  vtkSetStringMacro(Program);

  // Description:
  // Short description of the contents of this node. ie: 'presets'
  vtkGetStringMacro(Contents);
  vtkSetStringMacro(Contents);

  // Description:
  // List of options expressed in this form: key1='value1' key2='value2'
  vtkGetStringMacro(Options);
  vtkSetStringMacro(Options);

};

#endif









