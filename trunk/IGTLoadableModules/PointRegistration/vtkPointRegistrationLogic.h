/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkPointRegistrationLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkPointRegistrationLogic_h
#define __vtkPointRegistrationLogic_h

#include "vtkPointRegistrationWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkMRMLLinearTransformNode;
class vtkPoints;
class vtkMatrix4x4;
class vtkMRMLNode;

class VTK_PointRegistration_EXPORT vtkPointRegistrationLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkPointRegistrationLogic *New();
  
  vtkTypeRevisionMacro(vtkPointRegistrationLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);
  
  void GetCurrentPosition(vtkMRMLNode*, double *px, double *py, double *pz);
  int PerformPatientToImageRegistration(vtkPoints* patPoints, vtkPoints* imgPoints, vtkMatrix4x4* regMatrix);

 protected:
  
  vtkPointRegistrationLogic();
  ~vtkPointRegistrationLogic();

  void operator=(const vtkPointRegistrationLogic&);
  vtkPointRegistrationLogic(const vtkPointRegistrationLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:

  //Node to hold patient tracker node
  vtkMRMLLinearTransformNode* PatientTrackerNode;

};

#endif


  
