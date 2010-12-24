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
#include "vtkMRMLAbdoNavNode.h"

/* Slicer includes */
#include "vtkSlicerModuleLogic.h"

/* VTK includes */
#include "vtkCallbackCommand.h"

class VTK_AbdoNav_EXPORT vtkAbdoNavLogic : public vtkSlicerModuleLogic
{
 public:
  //----------------------------------------------------------------
  // Events to be passed to and processed by the GUI.
  //----------------------------------------------------------------
  //BTX
  enum {
    StatusUpdateEvent = 50001,
    //AnotherEvent    = 50002
  };
  //ETX

  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  static vtkAbdoNavLogic* New();
  vtkTypeRevisionMacro(vtkAbdoNavLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  /// Set and observe the parameter node.
  void SetAndObserveAbdoNavNode(vtkMRMLAbdoNavNode* node) { vtkSetAndObserveMRMLNodeMacro(this->AbdoNavNode, node); }
  /// Get the parameter node.
  vtkGetObjectMacro(AbdoNavNode, vtkMRMLAbdoNavNode);

  //----------------------------------------------------------------
  // Mediator method for processing events invoked by MRML.
  //----------------------------------------------------------------
  void ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData);
  static void DataCallback(vtkObject* caller, unsigned long eventid, void* clientData, void* callData);
  void UpdateAll(); // not implemented

  /// Create locator model and make it observe the selected tracker transform node.
  vtkMRMLModelNode* EnableLocatorDriver(const char* locatorName);
  /// Show or hide locator model.
  void ToggleLocatorVisibility(int vis);
  /// Freeze or unfreeze locator model.
  void ToggleLocatorUpdate(int freeze);
  /// Create a locator model.
  vtkMRMLModelNode* AddLocatorModel(const char* nodeName, double r, double g, double b);

 protected:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  vtkAbdoNavLogic();
  virtual ~vtkAbdoNavLogic();

 private:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  vtkAbdoNavLogic(const vtkAbdoNavLogic&); // not implemented
  void operator=(const vtkAbdoNavLogic&);  // not implemented

  //----------------------------------------------------------------
  // Logic values.
  //----------------------------------------------------------------
  /// Parameter node associated with this module.
  vtkMRMLAbdoNavNode* AbdoNavNode;
  /// TODO: insert missing comment!
  vtkCallbackCommand* DataCallbackCommand;
  /// Matrix holding the position at which the locator model was frozen.
  vtkMatrix4x4* LocatorFreezePosition;

};

#endif // __vtkAbdoNavLogic_h
