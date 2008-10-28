/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkVirtualFixtureLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkVirtualFixtureLogic_h
#define __vtkVirtualFixtureLogic_h

#include "vtkVirtualFixtureWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_VirtualFixture_EXPORT vtkVirtualFixtureLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkVirtualFixtureLogic *New();
  
  vtkTypeRevisionMacro(vtkVirtualFixtureLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

 protected:
  
  vtkVirtualFixtureLogic();
  ~vtkVirtualFixtureLogic();

  void operator=(const vtkVirtualFixtureLogic&);
  vtkVirtualFixtureLogic(const vtkVirtualFixtureLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
