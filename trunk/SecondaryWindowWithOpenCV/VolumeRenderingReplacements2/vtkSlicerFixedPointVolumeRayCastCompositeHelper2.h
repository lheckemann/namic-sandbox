/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerFixedPointVolumeRayCastCompositeHelper2.h,v $
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

// .NAME vtkSlicerFixedPointVolumeRayCastCompositeHelper2 - A helper that generates composite images for the volume ray cast mapper
// .SECTION Description
// This is one of the helper classes for the vtkSlicerFixedPointVolumeRayCastMapper2. 
// It will generate composite images using an alpha blending operation. 
// This class should not be used directly, it is a helper class for
// the mapper and has no user-level API.
//
// .SECTION see also
// vtkSlicerFixedPointVolumeRayCastMapper2

#ifndef __vtkSlicerFixedPointVolumeRayCastCompositeHelper2_h
#define __vtkSlicerFixedPointVolumeRayCastCompositeHelper2_h

#include "vtkSlicerFixedPointVolumeRayCastHelper2.h"
#include "vtkVolumeRenderingReplacements2.h"

class vtkSlicerFixedPointVolumeRayCastMapper2;
class vtkVolume;

class VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT vtkSlicerFixedPointVolumeRayCastCompositeHelper2 : public vtkSlicerFixedPointVolumeRayCastHelper2
{
public:
  static vtkSlicerFixedPointVolumeRayCastCompositeHelper2 *New();
  vtkTypeRevisionMacro(vtkSlicerFixedPointVolumeRayCastCompositeHelper2,vtkSlicerFixedPointVolumeRayCastHelper2);
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual void  GenerateImage( int threadID, 
                               int threadCount,
                               vtkVolume *vol,
                               vtkSlicerFixedPointVolumeRayCastMapper2 *mapper);

protected:
  vtkSlicerFixedPointVolumeRayCastCompositeHelper2();
  ~vtkSlicerFixedPointVolumeRayCastCompositeHelper2();

private:
  vtkSlicerFixedPointVolumeRayCastCompositeHelper2(const vtkSlicerFixedPointVolumeRayCastCompositeHelper2&);  // Not implemented.
  void operator=(const vtkSlicerFixedPointVolumeRayCastCompositeHelper2&);  // Not implemented.
};

#endif


