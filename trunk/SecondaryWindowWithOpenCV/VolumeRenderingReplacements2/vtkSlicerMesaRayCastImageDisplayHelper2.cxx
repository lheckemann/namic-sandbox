/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerMesaRayCastImageDisplayHelper2.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Make sure this is first, so any includes of gl.h can be stoped if needed
#define VTK_IMPLEMENT_MESA_CXX

#include "MangleMesaInclude/gl_mangle.h"
#include "MangleMesaInclude/gl.h"

#include <math.h>
#include "vtkSlicerMesaRayCastImageDisplayHelper2.h"

// make sure this file is included before the #define takes place
// so we don't get two vtkSlicerMesaRayCastImageDisplayHelper2 classes defined.
#include "vtkSlicerOpenGLRayCastImageDisplayHelper2.h"
#include "vtkSlicerMesaRayCastImageDisplayHelper2.h"

// Make sure vtkSlicerMesaRayCastImageDisplayHelper2 is a copy of 
// vtkSlicerOpenGLRayCastImageDisplayHelper2 with vtkSlicerOpenGLRayCastImageDisplayHelper2 
// replaced with vtkSlicerMesaRayCastImageDisplayHelper2
#define vtkSlicerOpenGLRayCastImageDisplayHelper2 vtkSlicerMesaRayCastImageDisplayHelper2
#include "vtkSlicerOpenGLRayCastImageDisplayHelper2.cxx"
#undef vtkSlicerOpenGLRayCastImageDisplayHelper2

vtkCxxRevisionMacro(vtkSlicerMesaRayCastImageDisplayHelper2, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkSlicerMesaRayCastImageDisplayHelper2);
