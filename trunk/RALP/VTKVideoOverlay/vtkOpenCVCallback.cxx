/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVRendererDelegate.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <vector>
#include <iostream>

// VTK header files and prototypes
#include "vtkObjectFactory.h"
#include "vtkOpenCVCallback.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOpenCVCallback);

//----------------------------------------------------------------------------
vtkOpenCVCallback::vtkOpenCVCallback()
{
}

//----------------------------------------------------------------------------
vtkOpenCVCallback::~vtkOpenCVCallback()
{
}

//----------------------------------------------------------------------------
void vtkOpenCVCallback::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
