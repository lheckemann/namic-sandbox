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

#include "vtkAbdoNavWin32Header.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerModuleLogic.h"

class VTK_AbdoNav_EXPORT vtkAbdoNavLogic : public vtkSlicerModuleLogic
{
 public:
  //BTX
  /// Events to be passed to and processed by the GUI (currently not used).
  enum {
    StatusUpdateEvent = 50001,
    //AnotherEvent    = 50002
  };
  //ETX

  //----------------------------------------------------------------
  // Usual vtk class declarations.
  //----------------------------------------------------------------
  static vtkAbdoNavLogic *New();
  vtkTypeRevisionMacro(vtkAbdoNavLogic, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  //----------------------------------------------------------------
  // Mediator method for processing events invoked by MRML
  //----------------------------------------------------------------
  void ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData);

 protected:
   //----------------------------------------------------------------
   // Usual vtk class declarations.
   //----------------------------------------------------------------
  vtkAbdoNavLogic();
  ~vtkAbdoNavLogic();

  //----------------------------------------------------------------
  // Currently not used.
  //----------------------------------------------------------------
  static void DataCallback(vtkObject *caller, unsigned long eventid, void *clientData, void *callData);
  vtkCallbackCommand *DataCallbackCommand;
  void UpdateAll();

 private:
   //----------------------------------------------------------------
   // Usual vtk class declarations.
   //----------------------------------------------------------------
   /// Not implemented, thus private.
   vtkAbdoNavLogic(const vtkAbdoNavLogic&);
   /// Not implemented, thus private.
   void operator=(const vtkAbdoNavLogic&);

};

#endif // __vtkAbdoNavLogic_h
