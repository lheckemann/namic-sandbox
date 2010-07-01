/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkStereoCalibLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkNeuroEndoscopicNavigationLogic_h
#define __vtkNeuroEndoscopicNavigationLogic_h

#include "vtkNeuroEndoscopicNavigationWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

class VTK_NeuroEndoscopicNavigation_EXPORT vtkNeuroEndoscopicNavigationLogic : 
  public vtkSlicerModuleLogic
{
 public:
  
  static vtkNeuroEndoscopicNavigationLogic *New();

  vtkTypeRevisionMacro(vtkNeuroEndoscopicNavigationLogic, vtkObject);
  void PrintSelf(ostream&, vtkIndent);

 protected:

  vtkNeuroEndoscopicNavigationLogic();
  ~vtkNeuroEndoscopicNavigationLogic();

  void operator=(const vtkNeuroEndoscopicNavigationLogic&);
  vtkNeuroEndoscopicNavigationLogic(const vtkNeuroEndoscopicNavigationLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif

