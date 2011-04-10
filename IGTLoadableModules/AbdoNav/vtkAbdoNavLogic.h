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

/* MRML includes */
#include "vtkMRMLCrosshairNode.h"

/* Slicer includes */
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerModuleLogic.h"

class VTK_AbdoNav_EXPORT vtkAbdoNavLogic : public vtkSlicerModuleLogic
{
 public:
  //BTX
  //----------------------------------------------------------------
  // Events to be passed to and processed by the GUI.
  //----------------------------------------------------------------
  enum LogicEvents {
    RequestFitToBackground = 50001,
    //AnotherEvent         = 50002
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

  //----------------------------------------------------------------
  // Calculate registration matrix based on three manually identified
  // points on the guidance needle artifact.
  // See implementation for a detailed comment.
  //----------------------------------------------------------------
  int PerformRegistration();
  /// Convenience function that calls UpdateSlicePlane(...) on each slice node driven
  /// by the locator, updates the crosshair in each slice view (if selected by user)
  /// and calls UpdateNeedleProjection(...) on each slice view (if selected by user).
  void UpdateAll();

  //----------------------------------------------------------------
  // 3D visualization.
  //----------------------------------------------------------------
  /// Find and return the locator. Return NULL if not found.
  vtkMRMLModelNode* FindLocator(const char* locatorName);
  /// Create locator model and make it observe the selected tracker transform node.
  int EnableLocatorDriver(const char* locatorName);
  /// Show or hide locator model.
  void ToggleLocatorVisibility(int vis);
  /// Freeze or unfreeze locator model.
  void ToggleLocatorFreeze(int freeze);
  /// Create a locator model.
  vtkMRMLModelNode* AddLocatorModel(const char* locatorName, double r, double g, double b);

  //----------------------------------------------------------------
  // 2D visualization.
  //----------------------------------------------------------------
  /// Update the needle projection in the specified slice view. Due to being a projection of
  /// the procedure needle's direction vector, the line being drawn grows and shrinks. That
  /// is, the line has maximum length when the needle is in-plane and is invisible when the
  /// needle is perpendicular to the slice plane. Due to this behavior, it's impossible for
  /// the physician to decide whether or not the needle is in-plane. Therefore, the projected
  /// line consists of a green and a red part with the red part indicating the off-plane angle.
  /// That is, the line is completely green when the needle is in-plane and is almost comple-
  /// tely red when the needle is close to being perpendicular to the slice plane. Depending on
  /// whether the needle is in front of the slice plane or behind it, the line is either solid
  /// or dashed respectively.
  void UpdateNeedleProjection(vtkMatrix4x4* registeredTracker);

  //----------------------------------------------------------------
  // Reslicing.
  //----------------------------------------------------------------
  /// Set pointers to the three slice nodes (Red, Yellow, Green).
  void CheckSliceNodes();
  /// Set a slice node's (Red == 0, Yellow == 1, Green == 2) slice
  /// driver ("User" or "Locator").
  int SetSliceDriver(int sliceNode, const char* sliceDriver);
  /// Reslice the specified slice node (Red == 0, Yellow == 1, Green == 2)
  /// given the registered tracking data.
  void ResliceNode(int sliceNode, vtkMatrix4x4* registeredTracker);

  //----------------------------------------------------------------
  // Getters and Setters to access (private) logic values from the GUI.
  //----------------------------------------------------------------
  vtkGetMacro(RegistrationPerformed, bool);
  vtkGetObjectMacro(Crosshair, vtkMRMLCrosshairNode);
  vtkSetObjectMacro(Crosshair, vtkMRMLCrosshairNode);
  vtkSetMacro(ShowCrosshair, bool);
  vtkGetObjectMacro(AppGUI, vtkSlicerApplicationGUI);
  vtkSetObjectMacro(AppGUI, vtkSlicerApplicationGUI);
  vtkSetMacro(DrawNeedleProjection, bool);
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
  /// The relative tracking transform node created by OpenIGTLinkIF.
  vtkMRMLLinearTransformNode* RelativeTrackingTransform;
  /// Matrix holding the position at which the locator model was frozen.
  vtkMatrix4x4* LocatorFreezePosition;
  /// Pointer to access the crosshair node.
  vtkMRMLCrosshairNode* Crosshair;
  /// Pointer to access the Slicer GUI.
  vtkSlicerApplicationGUI* AppGUI;
  /// Actor used to draw the green part of the needle projection.
  vtkActor2D* Actor2DGreen;
  /// Actor used to draw the red part of the needle projection.
  vtkActor2D* Actor2DRed;
  /// Pointers to the three slice nodes (Red == 0, Yellow == 1, Green == 2).
  vtkMRMLSliceNode* SliceNode[3];
  //BTX
  /// Holds a slice node's (Red == 0, Yellow == 1, Green == 2) slice driver
  /// ("User" or "Locator").
  std::string SliceDriver[3];
  /// Holds a slice node's (Red == 0, Yellow == 1, Green == 2) slice orientation
  /// (Axial, Sagittal or Coronal).
  std::string SliceOrientation[3];
  //ETX
  /// Flag indicating whether or not registration has been performed yet.
  bool RegistrationPerformed;
  /// Flag indicating whether or not to show the crosshair in the slice views.
  bool ShowCrosshair;
  /// Flag indicating whether or not to draw the needle projection in the slice views.
  bool DrawNeedleProjection;
  /// Flag indicating whether or not to freeze reslicing.
  bool FreezeReslicing;
  /// Flag indicating whether or not position and orientation of the tracked tool should be used for reslicing.
  bool ObliqueReslicing;

};

#endif // __vtkAbdoNavLogic_h
