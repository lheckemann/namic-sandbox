/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlModelStateNode.h,v $
  Date:      $Date: 2005/12/20 22:44:28 $
  Version:   $Revision: 1.5.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlModelStateNode_h
#define __vtkMrmlModelStateNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlModelStateNode : public vtkMrmlNode
{
public:
  static vtkMrmlModelStateNode *New();
  vtkTypeMacro(vtkMrmlModelStateNode,vtkMrmlNode);
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
  
  // Description:
  // Visibility
  vtkBooleanMacro(Visible,int);
  vtkSetMacro(Visible,int);
  vtkGetMacro(Visible,int);
  
  // Description:
  // Visibility of the opacity slider
  // This is not used in Slicer, but only saved to maintain compatibility
  // with SPLViz
  vtkBooleanMacro(SliderVisible,int);
  vtkSetMacro(SliderVisible,int);
  vtkGetMacro(SliderVisible,int);
  
  // Description:
  // Visibility of the children (for model groups)
  vtkBooleanMacro(SonsVisible,int);
  vtkSetMacro(SonsVisible,int);
  vtkGetMacro(SonsVisible,int);
  
  // Description:
  // Opacity
  vtkSetMacro(Opacity,float);
  vtkGetMacro(Opacity,float);

  // Description:
  // Is clipping active?
  vtkBooleanMacro(Clipping,int);
  vtkSetMacro(Clipping,int);
  vtkGetMacro(Clipping,int);
  
  // Description:
  // Is backface culling active?
  vtkBooleanMacro(BackfaceCulling,int);
  vtkSetMacro(BackfaceCulling,int);
  vtkGetMacro(BackfaceCulling,int); 
  
 };

#endif
