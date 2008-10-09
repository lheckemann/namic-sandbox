/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/vtkITK/vtkITK.h $
  Date:      $Date: 2008-05-20 16:29:59 +0900 (火, 20 5 2008) $
  Version:   $Revision: 6773 $

==========================================================================*/
//
// use an ifdef on Slicer3_VTK5 to flag code that won't
// compile on vtk4.4 and before
//
#if ( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
#define Slicer3_VTK5
#endif

#include "vtkITKWin32Header.h"
#include "vtkITKNumericTraits.h"
