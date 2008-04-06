/*=auto=========================================================================

  Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

// .NAME vtkLegoLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkLegoLogic_h
#define __vtkLegoLogic_h

#include "vtkLegoWin32Header.h"
#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerLogic.h"


class VTK_LEGO_EXPORT vtkLegoLogic : public vtkSlicerLogic 
{
public:

    // The Usual vtk class functions
    static vtkLegoLogic *New();
    vtkTypeRevisionMacro(vtkLegoLogic,vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);


protected:

    vtkLegoLogic();
    ~vtkLegoLogic();
    vtkLegoLogic(const vtkLegoLogic&);
    void operator=(const vtkLegoLogic&);

};

#endif


  
