/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlVolumeReadWriteStructuredPointsNode.h,v $
  Date:      $Date: 2005/12/20 22:44:33 $
  Version:   $Revision: 1.4.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlVolumeReadWriteStructuredPointsNode_h
#define __vtkMrmlVolumeReadWriteStructuredPointsNode_h

#include "vtkMrmlVolumeReadWriteNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlVolumeReadWriteStructuredPointsNode : public vtkMrmlVolumeReadWriteNode
{
  public:
  static vtkMrmlVolumeReadWriteStructuredPointsNode *New();
  vtkTypeMacro(vtkMrmlVolumeReadWriteStructuredPointsNode,vtkMrmlVolumeReadWriteNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------
 
  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);
  
  //--------------------------------------------------------------------------
  // Specifics for each type of volume data
  //--------------------------------------------------------------------------

  // Subclasses will add more here to handle their types of volume

};

#endif
