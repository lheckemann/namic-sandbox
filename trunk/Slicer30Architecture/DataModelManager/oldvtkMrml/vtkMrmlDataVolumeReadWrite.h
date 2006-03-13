/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlDataVolumeReadWrite.h,v $
  Date:      $Date: 2005/12/20 22:44:22 $
  Version:   $Revision: 1.3.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlDataVolumeReadWrite_h
#define __vtkMrmlDataVolumeReadWrite_h

#include "vtkObject.h"
#include "vtkSlicer.h"
#include "vtkMrmlVolumeNode.h"
#include "vtkImageSource.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlDataVolumeReadWrite : public vtkObject
{
  public:
  static vtkMrmlDataVolumeReadWrite *New();
  vtkTypeMacro(vtkMrmlDataVolumeReadWrite,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Return code indicates success (1) or error (0)
  virtual int Read(vtkMrmlVolumeNode *node, vtkImageSource **output);
  virtual int Write(vtkMrmlVolumeNode *node, vtkImageData *input);

  //--------------------------------------------------------------------------
  // Specifics for reading/writing each type of volume data
  //--------------------------------------------------------------------------

  // Subclasses must fill these in.

};

#endif
