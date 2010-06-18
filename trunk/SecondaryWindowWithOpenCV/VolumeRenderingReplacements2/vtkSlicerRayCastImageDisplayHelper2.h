/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerRayCastImageDisplayHelper2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkSlicerRayCastImageDisplayHelper2 - helper class that draws the image to the screen
// .SECTION Description
// This is a helper class for drawing images created from ray casting on the screen.
// This is the abstract device-independent superclass.

// .SECTION see also
// vtkVolumeRayCastMapper vtkUnstructuredGridVolumeRayCastMapper
// vtkSlicerOpenGLRayCastImageDisplayHelper2

#ifndef __vtkSlicerRayCastImageDisplayHelper2_h
#define __vtkSlicerRayCastImageDisplayHelper2_h

#include "vtkObject.h"
#include "vtkVolumeRenderingReplacements2.h"

class vtkVolume;
class vtkRenderer;
class vtkSlicerFixedPointRayCastImage2;

class VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT vtkSlicerRayCastImageDisplayHelper2 : public vtkObject
{
public:
  static vtkSlicerRayCastImageDisplayHelper2 *New();
  vtkTypeRevisionMacro(vtkSlicerRayCastImageDisplayHelper2,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual void RenderTexture( vtkVolume *vol, vtkRenderer *ren,
                              int imageMemorySize[2],
                              int imageViewportSize[2],
                              int imageInUseSize[2],
                              int imageOrigin[2],
                              float requestedDepth,
                              unsigned char *image ) = 0;

  virtual void RenderTexture( vtkVolume *vol, vtkRenderer *ren,
                              int imageMemorySize[2],
                              int imageViewportSize[2],
                              int imageInUseSize[2],
                              int imageOrigin[2],
                              float requestedDepth,
                              unsigned short *image ) = 0;

  virtual void RenderTexture( vtkVolume *vol, vtkRenderer *ren,
                              vtkSlicerFixedPointRayCastImage2 *image,
                              float requestedDepth ) = 0;

  vtkSetClampMacro( PreMultipliedColors, int, 0, 1 );
  vtkGetMacro( PreMultipliedColors, int );
  vtkBooleanMacro( PreMultipliedColors, int );
  

  // Description:
  // Set / Get the pixel scale to be applied to the image before display.
  // Can be set to scale the incoming pixel values - for example the
  // fixed point mapper uses the unsigned short API but with 15 bit
  // values so needs a scale of 2.0.
  vtkSetMacro( PixelScale, float );
  vtkGetMacro( PixelScale, float );
  
  
protected:
  vtkSlicerRayCastImageDisplayHelper2();
  ~vtkSlicerRayCastImageDisplayHelper2();
  
  // Description:
  // Have the colors already been multiplied by alpha?
  int PreMultipliedColors;

  float PixelScale;
  
private:
  vtkSlicerRayCastImageDisplayHelper2(const vtkSlicerRayCastImageDisplayHelper2&);  // Not implemented.
  void operator=(const vtkSlicerRayCastImageDisplayHelper2&);  // Not implemented.
};

#endif

