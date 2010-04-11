/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlLocatorNode.h,v $
  Date:      $Date: 2005/12/20 22:44:25 $
  Version:   $Revision: 1.5.16.1 $

=========================================================================auto=*/
#ifndef __vtkMrmlLocatorNode_h
#define __vtkMrmlLocatorNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlLocatorNode : public vtkMrmlNode
{
public:
  static vtkMrmlLocatorNode *New();
  vtkTypeMacro(vtkMrmlLocatorNode,vtkMrmlNode);
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
  // Who or what is driving the locator?
  vtkSetStringMacro(Driver);
  vtkGetStringMacro(Driver);
  
  // Description:
  // Is the locator visible?
  vtkBooleanMacro(Visibility,int);
  vtkSetMacro(Visibility,int);
  vtkGetMacro(Visibility,int);
  
  // Description:
  // Is the transverse visible?
  vtkBooleanMacro(TransverseVisibility,int);
  vtkSetMacro(TransverseVisibility,int);
  vtkGetMacro(TransverseVisibility,int);
  
  // Description:
  // Length of the normal
  vtkSetMacro(NormalLen,int);
  vtkGetMacro(NormalLen,int);
  
  // Description:
  // Lenght of the transverse
  vtkSetMacro(TransverseLen,int);
  vtkGetMacro(TransverseLen,int);
  
  // Description:
  // Radius
  vtkSetMacro(Radius,float);
  vtkGetMacro(Radius,float);
  
  // Description:
  // Locator color
  vtkSetStringMacro(DiffuseColor);
  vtkGetStringMacro(DiffuseColor);

};

#endif