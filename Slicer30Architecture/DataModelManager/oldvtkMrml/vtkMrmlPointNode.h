/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlPointNode.h,v $
  Date:      $Date: 2005/12/20 22:44:29 $
  Version:   $Revision: 1.14.2.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlPointNode_h
#define __vtkMrmlPointNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlPointNode : public vtkMrmlNode
{
public:
  static vtkMrmlPointNode *New();
  vtkTypeMacro(vtkMrmlPointNode,vtkMrmlNode);
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
  // Get/Set for Point
  vtkSetVector3Macro(XYZ,float);
  vtkGetVectorMacro(XYZ,float,3);

  // Description:
  // Get/Set for endoscopic point
  vtkSetVector3Macro(FXYZ,float);
  vtkGetVectorMacro(FXYZ,float,3);

    // Description:
    // Get/Set for 2d slice point
    vtkSetVector4Macro(XYSO,float);
    vtkGetVectorMacro(XYSO,float,4);

  // Description:
  // Get/Set for orientation 
  vtkSetVector4Macro(OrientationWXYZ,float);
  vtkGetVectorMacro(OrientationWXYZ,float,4);
  
  void SetOrientationWXYZFromMatrix4x4(vtkMatrix4x4 *mat);

  vtkSetMacro(Index,int);
  vtkGetMacro(Index,int);

};

#endif
