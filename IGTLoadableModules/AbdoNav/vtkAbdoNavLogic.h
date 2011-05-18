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

/* IGT forward declarations */
class vtkIGTPat2ImgRegistration;

/* MRML includes */
#include "vtkMRMLCrosshairNode.h"

/* Slicer includes */
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerModuleLogic.h"

//----------------------------------------------------------------
// Marker geometry definitions relative to a tool's local
// coordinate system (see Polaris Vicra Tool Kit Guide).
//----------------------------------------------------------------
extern const float markerA_8700338[];
extern const float markerB_8700338[];
extern const float markerC_8700338[];
extern const float markerD_8700338[];
extern const float markerA_8700339[];
extern const float markerB_8700339[];
extern const float markerC_8700339[];
extern const float markerD_8700339[];
extern const float markerA_8700340[];
extern const float markerB_8700340[];
extern const float markerC_8700340[];
extern const float markerD_8700340[];

//----------------------------------------------------------------
// Definition of search patterns for the x-, y- and z-offset in the
// NDI ToolBox ".trackProperties" file.
//----------------------------------------------------------------
const char* const tool_8700338 = "NDI\\:8700338\\:34801401.offset.t";
const char* const tool_8700339 = "NDI\\:8700339\\:34801403.offset.t";
const char* const tool_8700340 = "NDI\\:8700340\\:34802401.offset.t";

class VTK_AbdoNav_EXPORT vtkAbdoNavLogic : public vtkSlicerModuleLogic
{
 public:
  //BTX
  //----------------------------------------------------------------
  // Events to be passed to and processed by the GUI.
  //----------------------------------------------------------------
  enum LogicEvents {
    RequestFitToBackground  = 50001,
    LocatorPositionRecorded = 50002,
    //AnotherEvent          = 50003
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
  /// Parse NDI ToolBox ".trackProperties" file.
  int ParseToolBoxProperties();
  /// Convenience function that calls UpdateSlicePlane(...) on each slice node driven
  /// by the locator, updates the crosshair in each slice view (if selected by user)
  /// and calls UpdateNeedleProjection(...) on each slice view (if selected by user).
  void UpdateAll();
  /// Helper function to observe the tracking transform node.
  int ObserveTrackingTransformNode();

  //----------------------------------------------------------------
  // 3D visualization.
  //----------------------------------------------------------------
  /// Show or hide locator model.
  void ToggleLocatorVisibility(int vis);
  /// Freeze or unfreeze locator model.
  void ToggleLocatorFreeze(int freeze);
  /// Find and return the locator. Return NULL if not found.
  vtkMRMLModelNode* FindLocator(const char* locatorName);
  /// Make locator observe the (relative) tracking transform node.
  int EnableLocatorDriver(const char* locatorName);
  /// Create a locator model with the specified color.
  vtkMRMLModelNode* AddLocatorModel(const char* locatorName, double r, double g, double b);

  //----------------------------------------------------------------
  // 2D visualization.
  //----------------------------------------------------------------
  /// Update the projection of the procedure needle.
  /// See implementation for a detailed comment.
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
  vtkSetMacro(RecordLocatorPosition, bool);
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
  /// Implementation of Horn's closed-form solution to the least-squares
  /// problem of absolute orientation.
  vtkIGTPat2ImgRegistration* Pat2ImgReg;
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
  /// Flag indicating whether or not to record the locator position in image space.
  bool RecordLocatorPosition;
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
