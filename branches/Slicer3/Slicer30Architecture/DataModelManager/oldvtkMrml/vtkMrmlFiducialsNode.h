/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlFiducialsNode.h,v $
  Date:      $Date: 2005/12/20 22:44:25 $
  Version:   $Revision: 1.15.12.1 $

=========================================================================auto=*/
#ifndef __vtkMrmlFiducialsNode_h
#define __vtkMrmlFiducialsNode_h

#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlFiducialsNode : public vtkMrmlNode
{
public:
  static vtkMrmlFiducialsNode *New();
  vtkTypeMacro(vtkMrmlFiducialsNode,vtkMrmlNode);
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
  // Get/Set for Symbol size
  vtkSetMacro(SymbolSize,float);
  vtkGetMacro(SymbolSize,float);


  // Description:
  // Get/Set for Symbol size
  vtkSetMacro(Visibility,int);
  vtkGetMacro(Visibility,int);

  // Description:
  // Get/Set for Text size
  vtkSetMacro(TextSize,float);
  vtkGetMacro(TextSize,float);

  // Description:
  // Get/Set for Glyph and Text color
  vtkSetVector3Macro(Color,float);
  vtkGetVectorMacro(Color,float,3);

  vtkSetStringMacro(Type);
  vtkGetStringMacro(Type);

  void SetTypeToEndoscopic();
  void SetTypeToMeasurement();
  void SetTypeToDefault();

};

#endif
