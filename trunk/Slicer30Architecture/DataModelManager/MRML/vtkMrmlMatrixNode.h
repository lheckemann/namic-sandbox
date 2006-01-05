/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlMatrixNode.h,v $
  Date:      $Date: 2005/12/20 22:44:26 $
  Version:   $Revision: 1.16.14.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlMatrixNode_h
#define __vtkMrmlMatrixNode_h

#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlMatrixNode : public vtkMrmlNode
{
public:
  static vtkMrmlMatrixNode *New();
  vtkTypeMacro(vtkMrmlMatrixNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // 16 numbers that form a 4x4 matrix. The matrix is multiplied by a 
  // point (M*P) to compute the transformed point
  void SetMatrix(char *str);
  char *GetMatrix();

  // Description:
  // Rotate around each axis: x,y, and z in degrees
  void Scale(float x, float y, float z) ;

  // Description:
  // Rotate around each axis: x,y, and z in degrees
  void RotateX(float d) ;
  void RotateY(float d) ;
  void RotateZ(float d) ;

  // Description:
  // Rotate around each axis: x,y, and z in degrees
  void Translate(float x, float y, float z) ;

  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Get/Set for Matrix
  vtkGetObjectMacro(Transform, vtkTransform);
  vtkSetObjectMacro(Transform, vtkTransform);

};

#endif

