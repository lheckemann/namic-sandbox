/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlEndFiducialsNode.h,v $
  Date:      $Date: 2005/12/20 22:44:23 $
  Version:   $Revision: 1.10.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlEndFiducialsNode_h
#define __vtkMrmlEndFiducialsNode_h

#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlEndFiducialsNode : public vtkMrmlNode
{
public:
  static vtkMrmlEndFiducialsNode *New();
  vtkTypeMacro(vtkMrmlEndFiducialsNode,vtkMrmlNode);
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

};

#endif
