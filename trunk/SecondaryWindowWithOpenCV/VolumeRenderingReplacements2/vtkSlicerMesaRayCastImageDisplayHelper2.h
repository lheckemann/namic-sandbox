/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerMesaRayCastImageDisplayHelper2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkSlicerMesaRayCastImageDisplayHelper2 - Mesa subclass that draws the image to the screen
// .SECTION Description
// This is the concrete implementation of a ray cast image display helper -
// a helper class responsible for drawing the image to the screen.

// .SECTION see also
// vtkSlicerRayCastImageDisplayHelper2

#ifndef __vtkSlicerMesaRayCastImageDisplayHelper2_h
#define __vtkSlicerMesaRayCastImageDisplayHelper2_h

#include "vtkSlicerRayCastImageDisplayHelper2.h"
#include "vtkVolumeRenderingModule.h"

class vtkVolume;
class vtkRenderer;
class vtkFixedPointRayCastImage;

class VTK_VOLUMERENDERINGMODULE_EXPORT vtkSlicerMesaRayCastImageDisplayHelper2 : public vtkSlicerRayCastImageDisplayHelper2
{
public:
  static vtkSlicerMesaRayCastImageDisplayHelper2 *New();
  vtkTypeRevisionMacro(vtkSlicerMesaRayCastImageDisplayHelper2,vtkSlicerRayCastImageDisplayHelper2);
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
                      vtkFixedPointRayCastImage *image,
                      float requestedDepth );
  
protected:
  vtkSlicerMesaRayCastImageDisplayHelper2();
  ~vtkSlicerMesaRayCastImageDisplayHelper2();
  
  void RenderTextureInternal( vtkVolume *vol, vtkRenderer *ren,
                              int imageMemorySize[2],
                              int imageViewportSize[2],
                              int imageInUseSize[2],
                              int imageOrigin[2],
                              float requestedDepth,
                              int imageScalarType,
                              void *image );
private:
  vtkSlicerMesaRayCastImageDisplayHelper2(const vtkSlicerMesaRayCastImageDisplayHelper2&);  // Not implemented.
  void operator=(const vtkSlicerMesaRayCastImageDisplayHelper2&);  // Not implemented.
};

#endif

