/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkSecondaryWindowLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkSecondaryWindowLogic_h
#define __vtkSecondaryWindowLogic_h

#include "vtkSecondaryWindowWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_SecondaryWindow_EXPORT vtkSecondaryWindowLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkSecondaryWindowLogic *New();
  
  vtkTypeRevisionMacro(vtkSecondaryWindowLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

 protected:
  
  vtkSecondaryWindowLogic();
  ~vtkSecondaryWindowLogic();

  void operator=(const vtkSecondaryWindowLogic&);
  vtkSecondaryWindowLogic(const vtkSecondaryWindowLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
