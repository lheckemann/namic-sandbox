/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerFixedPointVolumeRayCastMIPHelper2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSlicerFixedPointVolumeRayCastMIPHelper2 - A helper that generates MIP images for the volume ray cast mapper
// .SECTION Description
// This is one of the helper classes for the vtkSlicerFixedPointVolumeRayCastMapper2.
// It will generate maximum intensity images.
// This class should not be used directly, it is a helper class for
// the mapper and has no user-level API.
//
// .SECTION see also
// vtkSlicerFixedPointVolumeRayCastMapper2

#ifndef __vtkSlicerFixedPointVolumeRayCastMIPHelper2_h
#define __vtkSlicerFixedPointVolumeRayCastMIPHelper2_h

#include "vtkSlicerFixedPointVolumeRayCastHelper2.h"
#include "vtkVolumeRenderingReplacements2.h"

class vtkSlicerFixedPointVolumeRayCastMapper2;
class vtkVolume;

class VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT vtkSlicerFixedPointVolumeRayCastMIPHelper2 : public vtkSlicerFixedPointVolumeRayCastHelper2
{
public:
  static vtkSlicerFixedPointVolumeRayCastMIPHelper2 *New();
  vtkTypeRevisionMacro(vtkSlicerFixedPointVolumeRayCastMIPHelper2,vtkSlicerFixedPointVolumeRayCastHelper2);
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual void  GenerateImage( int threadID, 
                               int threadCount,
                               vtkVolume *vol,
                               vtkSlicerFixedPointVolumeRayCastMapper2 *mapper);

protected:
  vtkSlicerFixedPointVolumeRayCastMIPHelper2();
  ~vtkSlicerFixedPointVolumeRayCastMIPHelper2();

private:
  vtkSlicerFixedPointVolumeRayCastMIPHelper2(const vtkSlicerFixedPointVolumeRayCastMIPHelper2&);  // Not implemented.
  void operator=(const vtkSlicerFixedPointVolumeRayCastMIPHelper2&);  // Not implemented.
};

#endif



