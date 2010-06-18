/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerVolumeRenderingFactory2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSlicerVolumeRenderingFactory2 - 
// .SECTION Description

#ifndef __vtkSlicerVolumeRenderingFactory2_h
#define __vtkSlicerVolumeRenderingFactory2_h

#include "vtkObject.h"
#include "vtkVolumeRenderingReplacements2.h"

class VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT vtkSlicerVolumeRenderingFactory2 : public vtkObject
{
public:
  static vtkSlicerVolumeRenderingFactory2 *New();
  vtkTypeRevisionMacro(vtkSlicerVolumeRenderingFactory2,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create and return an instance of the named vtk object.
  // This method first checks the vtkObjectFactory to support
  // dynamic loading. 
  static vtkObject* CreateInstance(const char* vtkclassname);

protected:
  vtkSlicerVolumeRenderingFactory2() {};

private:
  vtkSlicerVolumeRenderingFactory2(const vtkSlicerVolumeRenderingFactory2&);  // Not implemented.
  void operator=(const vtkSlicerVolumeRenderingFactory2&);  // Not implemented.
};

#endif
