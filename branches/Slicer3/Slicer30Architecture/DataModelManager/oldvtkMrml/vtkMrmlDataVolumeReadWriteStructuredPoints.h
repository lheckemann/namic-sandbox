/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlDataVolumeReadWriteStructuredPoints.h,v $
  Date:      $Date: 2005/12/20 22:44:22 $
  Version:   $Revision: 1.4.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlDataVolumeReadWriteStructuredPoints_h
#define __vtkMrmlDataVolumeReadWriteStructuredPoints_h

#include "vtkObject.h"
#include "vtkSlicer.h"
#include "vtkMrmlVolumeNode.h"
#include "vtkStructuredPoints.h"
#include "vtkMrmlDataVolumeReadWrite.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlDataVolumeReadWriteStructuredPoints : public vtkMrmlDataVolumeReadWrite 
{
  public:
  static vtkMrmlDataVolumeReadWriteStructuredPoints *New();
  vtkTypeMacro(vtkMrmlDataVolumeReadWriteStructuredPoints,vtkMrmlDataVolumeReadWrite);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Return code indicates success (1) or error (0)
  int Read(vtkMrmlVolumeNode *node, vtkImageSource **output);
  int Write(vtkMrmlVolumeNode *node, vtkImageData *input);

  //--------------------------------------------------------------------------
  // Specifics for reading/writing each type of volume data
  //--------------------------------------------------------------------------

  // Subclasses must fill these in.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);


};

#endif
