/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlNode.h,v $
  Date:      $Date: 2005/12/20 22:44:28 $
  Version:   $Revision: 1.21.12.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlNode_h
#define __vtkMrmlNode_h

class VTK_SLICER_BASE_EXPORT vtkMrmlNode : public vtkObject
{
  public:
  static vtkMrmlNode *New();
  vtkTypeMacro(vtkMrmlNode,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Copy a MrmlNode's parameters.
  // But don't copy: ID
  void MrmlNodeCopy(vtkMrmlNode *node);

  // Description:
  // Copy everything from another node of the same type.
  // Instances of vtkMrmlNode must define the Copy function.
  // Instances of vtkMrmlNode::Copy should call vtkMrmlNode::CopyMrmlNode
  virtual void Copy(vtkMrmlNode *node) = 0;
  
  // Description:
  // Set/Get a numerical ID for the calling program to use to keep track
  // of its various node objects.
  vtkSetMacro(ID, int);
  vtkGetMacro(ID, int);

  // Description:
  // Text description of this node, to be set by the user
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

  // Description:
  // Name of this node, to be set by the user
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  // Description:
  // The Title is for use when graphically displaying the 
  // contents of a MRML file.
  char *GetTitle();

  // Description:
  // Node's effect on indentation when displaying the
  // contents of a MRML file. (0, +1, -1)
  vtkGetMacro(Indent, int);

  // Description:
  // Optional attributes not defined in the MRML standard,
  // but recognized by various browsers
  vtkSetStringMacro(Options);
  vtkGetStringMacro(Options);

  // Description:
  // Utility functions for converting between vtkMatrix4x4 and
  // a string of 16 numbers in row-major order.
  void SetMatrixToString(vtkMatrix4x4 *m, char *s);
  char* GetMatrixToString(vtkMatrix4x4 *m);

  // Description:
  // Write this node's information to a MRML file in XML format.
  // Only write attributes that differ from the default values,
  // which are set in the node's constructor.
  // This is a virtual function that all subclasses must overload.
  virtual void Write(ofstream& of, int indent);

};

#endif


