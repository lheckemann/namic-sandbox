/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerRayCastImageDisplayHelper2.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSlicerRayCastImageDisplayHelper2.h"
#include "vtkSlicerVolumeRenderingFactory2.h"

vtkCxxRevisionMacro(vtkSlicerRayCastImageDisplayHelper2, "$Revision: 1.4 $");

//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkSlicerRayCastImageDisplayHelper2);
//----------------------------------------------------------------------------

vtkSlicerRayCastImageDisplayHelper2* vtkSlicerRayCastImageDisplayHelper2::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret=vtkSlicerVolumeRenderingFactory2::CreateInstance("vtkSlicerRayCastImageDisplayHelper2");
  return (vtkSlicerRayCastImageDisplayHelper2*)ret;
}


// Construct a new vtkSlicerRayCastImageDisplayHelper2 with default values
vtkSlicerRayCastImageDisplayHelper2::vtkSlicerRayCastImageDisplayHelper2()
{
  this->PreMultipliedColors = 0;
  this->PixelScale = 1.0;
}

// Destruct a vtkSlicerRayCastImageDisplayHelper2 - clean up any memory used
vtkSlicerRayCastImageDisplayHelper2::~vtkSlicerRayCastImageDisplayHelper2()
{
}

void vtkSlicerRayCastImageDisplayHelper2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "PreMultiplied Colors: "
     << (this->PreMultipliedColors ? "On" : "Off") << endl;
  
  os << indent << "Pixel Scale: " << this->PixelScale << endl;
}

