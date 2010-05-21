/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkTransformRecorderLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkTransformRecorderLogic_h
#define __vtkTransformRecorderLogic_h

#include "vtkTransformRecorderWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_TransformRecorder_EXPORT vtkTransformRecorderLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkTransformRecorderLogic *New();
  
  vtkTypeRevisionMacro(vtkTransformRecorderLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

 protected:
  
  vtkTransformRecorderLogic();
  ~vtkTransformRecorderLogic();

  void operator=(const vtkTransformRecorderLogic&);
  vtkTransformRecorderLogic(const vtkTransformRecorderLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
