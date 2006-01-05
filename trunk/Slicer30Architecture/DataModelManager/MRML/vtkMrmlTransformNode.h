/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlTransformNode.h,v $
  Date:      $Date: 2005/12/20 22:44:31 $
  Version:   $Revision: 1.16.16.1 $

=========================================================================auto=*/
// .NAME vtkMrmlTransformNode - MRML node for representing a transform.
// .SECTION Description
// A Transform is not a node with attributes, but a construct for 
// building MRML files.  A Transform encapsulates the Matrix nodes inside it 
// such that they are invisible to nodes outside the Transform.  

#ifndef __vtkMrmlTransformNode_h
#define __vtkMrmlTransformNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlTransformNode : public vtkMrmlNode
{
public:
  static vtkMrmlTransformNode *New();
  vtkTypeMacro(vtkMrmlTransformNode,vtkMrmlNode);
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

protected:
  vtkMrmlTransformNode();
  ~vtkMrmlTransformNode();
  vtkMrmlTransformNode(const vtkMrmlTransformNode&) {};
  void operator=(const vtkMrmlTransformNode&) {};

};

#endif

