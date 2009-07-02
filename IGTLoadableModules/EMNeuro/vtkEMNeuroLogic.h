/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkEMNeuroLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkEMNeuroLogic_h
#define __vtkEMNeuroLogic_h

#include "vtkEMNeuroWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_EMNeuro_EXPORT vtkEMNeuroLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkEMNeuroLogic *New();
  
  vtkTypeRevisionMacro(vtkEMNeuroLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);
  void ProcessMatrix(vtkMRMLNode*);

 protected:
  
  vtkEMNeuroLogic();
  ~vtkEMNeuroLogic();

  void operator=(const vtkEMNeuroLogic&);
  vtkEMNeuroLogic(const vtkEMNeuroLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
