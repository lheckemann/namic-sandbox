/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlHierarchyNode.h,v $
  Date:      $Date: 2005/12/20 22:44:25 $
  Version:   $Revision: 1.5.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlHierarchyNode_h
#define __vtkMrmlHierarchyNode_h

#include "vtkMrmlNode.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlHierarchyNode : public vtkMrmlNode
{
public:
  static vtkMrmlHierarchyNode *New();
  vtkTypeMacro(vtkMrmlHierarchyNode,vtkMrmlNode);
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
  // Hierarchy ID
  vtkSetStringMacro(HierarchyID);
  vtkGetStringMacro(HierarchyID);

  // Description:
  // Hierarchy type
  vtkSetStringMacro(Type);
  vtkGetStringMacro(Type);

};

#endif
