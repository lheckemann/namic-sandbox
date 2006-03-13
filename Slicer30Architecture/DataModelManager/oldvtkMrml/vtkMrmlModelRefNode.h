/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlModelRefNode.h,v $
  Date:      $Date: 2005/12/20 22:44:27 $
  Version:   $Revision: 1.6.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlModelRefNode_h
#define __vtkMrmlModelRefNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlModelRefNode : public vtkMrmlNode
{
public:
  static vtkMrmlModelRefNode *New();
  vtkTypeMacro(vtkMrmlModelRefNode,vtkMrmlNode);
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
  // ID of the referenced model
  vtkSetStringMacro(ModelRefID);
  vtkGetStringMacro(ModelRefID);

};

#endif
