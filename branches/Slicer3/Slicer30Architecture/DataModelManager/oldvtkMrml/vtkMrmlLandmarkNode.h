/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlLandmarkNode.h,v $
  Date:      $Date: 2005/12/20 22:44:25 $
  Version:   $Revision: 1.11.12.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlLandmarkNode_h
#define __vtkMrmlLandmarkNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlLandmarkNode : public vtkMrmlNode
{
public:
  static vtkMrmlLandmarkNode *New();
  vtkTypeMacro(vtkMrmlLandmarkNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Get/Set for Landmark
  vtkSetVector3Macro(XYZ,float);
  vtkGetVectorMacro(XYZ,float,3);

  // Description:
  // Get/Set for Landmark
  vtkSetVector3Macro(FXYZ,float);
  vtkGetVectorMacro(FXYZ,float,3);

  // Description:
  // Position of the landmark along the path
  vtkSetMacro(PathPosition, int);
  vtkGetMacro(PathPosition, int);

};

#endif

