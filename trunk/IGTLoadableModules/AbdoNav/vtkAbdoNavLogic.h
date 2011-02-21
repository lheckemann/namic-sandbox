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
    StatusUpdateEvent = 50001,
    //AnotherEvent    = 50002
  };

  enum SliceOrientation {
    SLICE_ORIENT_AXIAL    = 0,
    SLICE_ORIENT_SAGITTAL = 1,
    SLICE_ORIENT_CORONAL  = 2
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
  /// Set the slice driver (User == 0, Locator == 1) for each slice orientation (Red == 0,
  /// Yellow == 1, Green == 2).
  void SetSliceDriver(int sliceIndex, const char* driver);
  /// Convenience function that calls UpdateSlicePlane(...) on each slice node driven
  /// by the locator, updates the crosshair in each slice view (if selected by user)
  /// and calls UpdateNeedleProjection(...) on each slice view (if selected by user).
  void UpdateAll();
  /// Perform reslicing in the specified slice node given the registered tracking data.
  void UpdateSliceNode(int sliceNodeIndex, vtkMatrix4x4* registeredTracker);
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
  /// Set pointers to access the three different slice orientations.
  void CheckSliceNode();
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
  vtkGetObjectMacro(Crosshair, vtkMRMLCrosshairNode);
  vtkSetObjectMacro(Crosshair, vtkMRMLCrosshairNode);
  vtkSetMacro(ShowCrosshair, bool);
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
  /// The original tracker transform node created by OpenIGTLinkIF.
  vtkMRMLLinearTransformNode* OriginalTrackerTransform;
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
  /// Pointers to access the three different slice orientations.
  vtkMRMLSliceNode* SliceNode[3];
  /// Holds the slice driver (User == 0, Locator == 1) for each slice orientation (Red == 0,
  /// Yellow == 1, Green == 2).
  int SliceDriver[3];
  /// Holds the slice orientation (Axial == 0, Sagittal == 1, Coronal == 2) for each slice view
  /// (Red == 0, Yellow == 1, Green == 2)
  int SliceOrientation[3];
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
