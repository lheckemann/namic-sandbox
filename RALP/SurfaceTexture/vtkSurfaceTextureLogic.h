/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkSurfaceTextureLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkSurfaceTextureLogic_h
#define __vtkSurfaceTextureLogic_h

#include "vtkSurfaceTextureWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_SurfaceTexture_EXPORT vtkSurfaceTextureLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkSurfaceTextureLogic *New();
  
  vtkTypeRevisionMacro(vtkSurfaceTextureLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

 protected:
  
  vtkSurfaceTextureLogic();
  ~vtkSurfaceTextureLogic();

  void operator=(const vtkSurfaceTextureLogic&);
  vtkSurfaceTextureLogic(const vtkSurfaceTextureLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
