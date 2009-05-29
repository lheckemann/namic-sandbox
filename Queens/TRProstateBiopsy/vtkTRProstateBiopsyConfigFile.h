/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
// .NAME vtkTRProstateBiopsyConfigFile - ...
// .SECTION Description
// vtkTRProstateBiopsyConfigFile ... TODO: to be completed

#ifndef __vtkTRProstateBiopsyConfigFile_h
#define __vtkTRProstateBiopsyConfigFile_h

#include "vtkObject.h"
#include "vtkTRProstateBiopsyWin32Header.h"
#include "itkPoint.h"

class vtkMRMLTRProstateBiopsyModuleNode;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyConfigFile :
  public vtkObject
{
public:
  static vtkTRProstateBiopsyConfigFile *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyConfigFile,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // ... TODO: to be completed
  bool ReadConfigFile(vtkMRMLTRProstateBiopsyModuleNode *node);

  //BTX

  //ETX

protected:
  vtkTRProstateBiopsyConfigFile();
  virtual ~vtkTRProstateBiopsyConfigFile();

private:
  vtkTRProstateBiopsyConfigFile(const vtkTRProstateBiopsyConfigFile&);  // Not implemented.
  void operator=(const vtkTRProstateBiopsyConfigFile&);  // Not implemented.
};

#endif
