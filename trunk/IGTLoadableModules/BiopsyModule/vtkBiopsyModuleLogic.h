/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkBiopsyModuleLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkBiopsyModuleLogic_h
#define __vtkBiopsyModuleLogic_h

#include "vtkBiopsyModuleWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;
class vtkMRMLFiducialListNode;
class vtkSlicerApplicationGUI;
class vtkActorCollection;

class VTK_BiopsyModule_EXPORT vtkBiopsyModuleLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX


 public:
  
  static vtkBiopsyModuleLogic *New();
  
  vtkTypeRevisionMacro(vtkBiopsyModuleLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  void CheckSliceNode();
  void UpdateSliceNode(double*, double*, double*, int);

  vtkMRMLSliceNode* SliceNode[3];


 protected:
  
  vtkBiopsyModuleLogic();
  ~vtkBiopsyModuleLogic();

  void operator=(const vtkBiopsyModuleLogic&);
  vtkBiopsyModuleLogic(const vtkBiopsyModuleLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:

};

#endif


  
