/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlDataTetraMesh.h,v $
  Date:      $Date: 2005/12/20 22:44:22 $
  Version:   $Revision: 1.5.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlDataTetraMesh_h
#define __vtkMrmlDataTetraMesh_h

#include <stdlib.h>

#include "vtkMrmlData.h"

#include "vtkMrmlTetraMeshNode.h"
#include "vtkUnstructuredGrid.h"
#include "vtkSlicer.h"

//----------------------------------------------------------------------------
class VTK_SLICER_BASE_EXPORT vtkMrmlDataTetraMesh : public vtkMrmlData
{
public:
  static vtkMrmlDataTetraMesh *New();
  vtkTypeMacro(vtkMrmlDataTetraMesh,vtkMrmlData);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Provides opportunity to insure internal consistency before access. 
  // Transfers all ivars from MrmlNode to internal VTK objects
  void Update();
  unsigned long int GetMTime();

  // Description:
  // Set the image data
  // Use GetOutput to get the image data.
  vtkSetObjectMacro(TheMesh, vtkUnstructuredGrid);
  vtkUnstructuredGrid* GetOutput();

  // Description:
  // Read/Write image 
  int Read();
  int Write();

};

#endif
