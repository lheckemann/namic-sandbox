/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/
/// vtkAbdoNavLogic - manages the logic of the abdominal navigation module
///
/// This class implements the logic associated with the abdominal navigation
/// module.

#ifndef __vtkAbdoNavLogic_h
#define __vtkAbdoNavLogic_h

/* AbdoNav includes */
#include "vtkAbdoNavWin32Header.h"

/* Slicer includes */
#include "vtkSlicerModuleLogic.h"

/* VTK includes */
#include "vtkCallbackCommand.h"

class VTK_AbdoNav_EXPORT vtkAbdoNavLogic : public vtkSlicerModuleLogic
{
 public:
  //----------------------------------------------------------------
  // Events to be passed to and processed by the GUI.
  //BTX
  enum {
    StatusUpdateEvent = 50001,
    //AnotherEvent    = 50002
  };
  //ETX

  //----------------------------------------------------------------
  // Usual VTK class functions.
  static vtkAbdoNavLogic* New();
  vtkTypeRevisionMacro(vtkAbdoNavLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  //----------------------------------------------------------------
  // Mediator method for processing events invoked by MRML.
  void ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData);
  static void DataCallback(vtkObject* caller, unsigned long eventid, void* clientData, void* callData);
  void UpdateAll(); // not implemented

 protected:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  vtkAbdoNavLogic();
  virtual ~vtkAbdoNavLogic();

 private:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  vtkAbdoNavLogic(const vtkAbdoNavLogic&); // not implemented
  void operator=(const vtkAbdoNavLogic&);  // not implemented

  //----------------------------------------------------------------
  // Logic values.
  vtkCallbackCommand* DataCallbackCommand;

};

#endif // __vtkAbdoNavLogic_h
