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

  /// Calculate registration matrix based on two identified points on the guidance needle.
  void PerformRegistration();
  /// Set the slice driver (user == 0, locator == 1) for each slice orientation (Red == 0,
  /// Yellow == 1, Green == 2).
  void SetSliceDriver(int sliceIndex, int driverValue);
  /// Perform reslicing and update the crosshair.
  void UpdateSlicePlanes();
  /// Find and return the locator. Return NULL if not found.
  vtkMRMLModelNode* FindLocator(const char* locatorName);
  /// Create locator model and make it observe the selected tracker transform node.
  vtkMRMLModelNode* EnableLocatorDriver(const char* locatorName);
  /// Show or hide locator model.
  void ToggleLocatorVisibility(int vis);
  /// Freeze or unfreeze locator model.
  void ToggleLocatorFreeze(int freeze);
  /// Create a locator model.
  vtkMRMLModelNode* AddLocatorModel(const char* locatorName, double r, double g, double b);

  //----------------------------------------------------------------
  // Getters and Setters for (private) logic values that need  to be
  // accessed from the GUI class.
  //----------------------------------------------------------------
  vtkGetMacro(RegistrationPerformed, bool);
  vtkSetMacro(ShowCrosshair, bool);
  vtkSetMacro(FreezeReslicing, bool);
  vtkSetMacro(ObliqueReslicing, bool);

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
  /// Transform node holding the static registration matrix.
  vtkMRMLLinearTransformNode* RegistrationTransform;
  /// Matrix holding the position at which the locator model was frozen.
  vtkMatrix4x4* LocatorFreezePosition;
  /// Holds the slice driver (user == 0, locator == 1) for each slice orientation (Red == 0,
  /// Yellow == 1, Green == 2).
  int SliceDriver[3];
  /// Flag indicating whether or not registration has been performed yet.
  bool RegistrationPerformed;
  /// Flag indicating whether or not to show a crosshair corresponding to the locator's tip position.
  bool ShowCrosshair;
  /// Flag indicating whether or not to freeze reslicing.
  bool FreezeReslicing;
  /// Flag indicating whether or not position and orientation of the tracked tool should be used for reslicing.
  bool ObliqueReslicing;

};

#endif // __vtkAbdoNavLogic_h
