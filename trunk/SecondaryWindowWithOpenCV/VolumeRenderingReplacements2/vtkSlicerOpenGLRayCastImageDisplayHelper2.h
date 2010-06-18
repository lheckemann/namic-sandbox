/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerOpenGLRayCastImageDisplayHelper2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkSlicerOpenGLRayCastImageDisplayHelper2 - OpenGL subclass that draws the image to the screen
// .SECTION Description
// This is the concrete implementation of a ray cast image display helper -
// a helper class responsible for drawing the image to the screen.

// .SECTION see also
// vtkSlicerRayCastImageDisplayHelper2

#ifndef __vtkSlicerOpenGLRayCastImageDisplayHelper2_h
#define __vtkSlicerOpenGLRayCastImageDisplayHelper2_h

#include "vtkSlicerRayCastImageDisplayHelper2.h"
#include "vtkVolumeRenderingReplacements2.h"

class vtkVolume;
class vtkRenderer;
class vtkSlicerFixedPointRayCastImage2;

class VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT vtkSlicerOpenGLRayCastImageDisplayHelper2 : public vtkSlicerRayCastImageDisplayHelper2
{
public:
  static vtkSlicerOpenGLRayCastImageDisplayHelper2 *New();
  vtkTypeRevisionMacro(vtkSlicerOpenGLRayCastImageDisplayHelper2,vtkSlicerRayCastImageDisplayHelper2);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  void RenderTexture( vtkVolume *vol, vtkRenderer *ren,
                      int imageMemorySize[2],
                      int imageViewportSize[2],
                      int imageInUseSize[2],
                      int imageOrigin[2],
                      float requestedDepth,
                      unsigned char *image );

  void RenderTexture( vtkVolume *vol, vtkRenderer *ren,
                      int imageMemorySize[2],
                      int imageViewportSize[2],
                      int imageInUseSize[2],
                      int imageOrigin[2],
                      float requestedDepth,
                      unsigned short *image );

  void RenderTexture( vtkVolume *vol, vtkRenderer *ren,
                      vtkSlicerFixedPointRayCastImage2 *image,
                      float requestedDepth );

protected:
  vtkSlicerOpenGLRayCastImageDisplayHelper2();
  ~vtkSlicerOpenGLRayCastImageDisplayHelper2();
  
  void RenderTextureInternal( vtkVolume *vol, vtkRenderer *ren,
                              int imageMemorySize[2],
                              int imageViewportSize[2],
                              int imageInUseSize[2],
                              int imageOrigin[2],
                              float requestedDepth,
                              int imageScalarType,
                              void *image );

private:

  void *PreviousImage;
  vtkSlicerOpenGLRayCastImageDisplayHelper2(const vtkSlicerOpenGLRayCastImageDisplayHelper2&);  // Not implemented.
  void operator=(const vtkSlicerOpenGLRayCastImageDisplayHelper2&);  // Not implemented.
};

#endif

