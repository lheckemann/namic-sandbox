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


#ifndef __vtkStereoCalibLogic_h
#define __vtkStereoCalibLogic_h

#include "vtkStereoCalibWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_StereoCalib_EXPORT vtkStereoCalibLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkStereoCalibLogic *New();
  
  vtkTypeRevisionMacro(vtkStereoCalibLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

 protected:
  
  vtkStereoCalibLogic();
  ~vtkStereoCalibLogic();

  void operator=(const vtkStereoCalibLogic&);
  vtkStereoCalibLogic(const vtkStereoCalibLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
