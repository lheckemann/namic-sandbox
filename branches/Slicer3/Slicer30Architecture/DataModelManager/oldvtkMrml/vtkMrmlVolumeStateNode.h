/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlVolumeStateNode.h,v $
  Date:      $Date: 2005/12/20 22:44:33 $
  Version:   $Revision: 1.6.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlVolumeStateNode_h
#define __vtkMrmlVolumeStateNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlVolumeStateNode : public vtkMrmlNode
{
public:
  static vtkMrmlVolumeStateNode *New();
  vtkTypeMacro(vtkMrmlVolumeStateNode,vtkMrmlNode);
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
  // ID of the referenced volume
  vtkSetStringMacro(VolumeRefID);
  vtkGetStringMacro(VolumeRefID);

  // Description:
  // Color look up table
  vtkSetStringMacro(ColorLUT);
  vtkGetStringMacro(ColorLUT);

  // Description:
  // Is the volume shown in the foreground?
  vtkBooleanMacro(Foreground,int);
  vtkSetMacro(Foreground,int);
  vtkGetMacro(Foreground,int);

  // Description:
  // Is the volume shown in the background?
  vtkBooleanMacro(Background,int);
  vtkSetMacro(Background,int);
  vtkGetMacro(Background,int);

  // Description:
  // Is the volume faded?
  vtkBooleanMacro(Fade,int);
  vtkSetMacro(Fade,int);
  vtkGetMacro(Fade,int);
  
  // Description:
  // Opacity of the volume
  vtkSetMacro(Opacity,float);
  vtkGetMacro(Opacity,float);
 
};

#endif
