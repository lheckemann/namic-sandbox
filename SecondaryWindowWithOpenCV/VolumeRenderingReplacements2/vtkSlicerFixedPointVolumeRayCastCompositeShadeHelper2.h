/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2.h,v $
  Language:  C++
  Date:      $Date: 2005/05/04 14:13:58 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2 - A helper that generates composite images for the volume ray cast mapper
// .SECTION Description
// This is one of the helper classes for the vtkSlicerFixedPointVolumeRayCastMapper2.
// It will generate composite images using an alpha blending operation.
// This class should not be used directly, it is a helper class for
// the mapper and has no user-level API.
//
// .SECTION see also
// vtkSlicerFixedPointVolumeRayCastMapper2

#ifndef __vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2_h
#define __vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2_h

#include "vtkSlicerFixedPointVolumeRayCastHelper2.h"
#include "vtkVolumeRenderingReplacements2.h"
class vtkSlicerFixedPointVolumeRayCastMapper2;
class vtkVolume;

class VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2 : public vtkSlicerFixedPointVolumeRayCastHelper2
{
public:
  static vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2 *New();
  vtkTypeRevisionMacro(vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2,vtkSlicerFixedPointVolumeRayCastHelper2);
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual void  GenerateImage( int threadID, 
                               int threadCount,
                               vtkVolume *vol,
                               vtkSlicerFixedPointVolumeRayCastMapper2 *mapper);

protected:
  vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2();
  ~vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2();

private:
  vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2(const vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2&);  // Not implemented.
  void operator=(const vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2&);  // Not implemented.
};

#endif


