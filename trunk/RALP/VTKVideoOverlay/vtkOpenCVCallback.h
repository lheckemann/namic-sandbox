/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVRendererDelegate.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenCVRendererDelegate - abstract specification for actor
// .SECTION Description

// .SECTION See Also
// vtkRendererDelegate vtkRenderWindow vtkRendererDelegate vtkCamera vtkLight vtkVolume

#ifndef __vtkOpenCVCallback_h
#define __vtkOpenCVCallback_h


// VTK header files and prototypes
#include "vtkObject.h"

// OpenCV header files and protoptyes
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui.hpp"

class VTK_COMMON_EXPORT vtkOpenCVCallback : public vtkObject
{
  
public:
  static vtkOpenCVCallback *New();
  void PrintSelf(ostream&, vtkIndent);

  vtkTypeMacro(vtkOpenCVCallback, vtkObject);
  virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId, void *callData) {};
  
protected:
  vtkOpenCVCallback();
  ~vtkOpenCVCallback();

};


#endif
