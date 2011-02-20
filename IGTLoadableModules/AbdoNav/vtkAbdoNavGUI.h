/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/
/// vtkAbdoNavGUI - manages the GUI of the abdominal navigation module
///
/// This class implements the GUI associated with the abdominal navigation
/// module.

#ifndef __vtkAbdoNavGUI_h
#define __vtkAbdoNavGUI_h

/* AbdoNav includes */
#include "vtkAbdoNavLogic.h"
#include "vtkAbdoNavWin32Header.h"

/* KWWidgets forward declarations */
class vtkKWComboBoxWithLabel;

/* Slicer includes */
#include "vtkSlicerModuleGUI.h"

/* Slicer forward declarations */
class vtkSlicerNodeSelectorWidget;

class VTK_AbdoNav_EXPORT vtkAbdoNavGUI : public vtkSlicerModuleGUI
{
 public:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  static vtkAbdoNavGUI* New();
  vtkTypeRevisionMacro(vtkAbdoNavGUI, vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);
  /// Get the module's category.
  const char* GetCategory() const { return "IGT"; }
  /// Set the module logic.
  void SetModuleLogic(vtkSlicerLogic* logic) { this->SetLogic(vtkObjectPointer(&this->AbdoNavLogic), logic); }

  //----------------------------------------------------------------
  // Define behavior at module initialization, startup and exit.
  //----------------------------------------------------------------
  virtual void Init();  // called upon Slicer startup; not implemented
  virtual void Enter(); // called upon entering the module
  virtual void Exit();  // called upon exiting the module; not implemented

  //----------------------------------------------------------------
  // Build the GUI.
  //----------------------------------------------------------------
  virtual void BuildGUI();

  //----------------------------------------------------------------
  // Addition and removal of observers.
  //----------------------------------------------------------------
  virtual void AddGUIObservers();      // called automatically
  virtual void RemoveGUIObservers();   // called automatically
  void         AddLogicObservers();    // must be called manually
  void         RemoveLogicObservers(); // must be called manually
  void         AddMRMLObservers();     // must be called manually
  void         RemoveMRMLObservers();  // must be called manually

  //----------------------------------------------------------------
  // Mediator methods for event processing and GUI/MRML updating.
  //----------------------------------------------------------------
  /// Process events generated by GUI widgets.
  virtual void ProcessGUIEvents(vtkObject* caller, unsigned long event, void* callData);
  /// Process events generated by logic.
  virtual void ProcessLogicEvents(vtkObject* caller, unsigned long event, void* callData);
  /// Process events generated by MRML.
  virtual void ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData);
  /// Update this module's MRML parameter node based on the values specified in the GUI.
  void UpdateMRMLFromGUI();
  /// Update the GUI based on the values stored in this module's MRML parameter node.
  void UpdateGUIFromMRML();

 protected:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  vtkAbdoNavGUI();
  virtual ~vtkAbdoNavGUI();

 private:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  //----------------------------------------------------------------
  vtkAbdoNavGUI(const vtkAbdoNavGUI&);  // not implemented
  void operator=(const vtkAbdoNavGUI&); // not implemented

  //----------------------------------------------------------------
  // Logic values.
  //----------------------------------------------------------------
  /// Logic class associated with this module.
  vtkAbdoNavLogic* AbdoNavLogic;
  /// Parameter node associated with this module.
  vtkMRMLAbdoNavNode* AbdoNavNode;

  //----------------------------------------------------------------
  // Helper functions to build the different GUI frames.
  //----------------------------------------------------------------
  void BuildGUIHelpFrame();
  void BuildGUIConnectionFrame();
  void BuildGUIRegistrationFrame();
  void BuildGUINavigationFrame();

  //----------------------------------------------------------------
  // Widgets of the connection frame.
  //----------------------------------------------------------------
  vtkSlicerNodeSelectorWidget* TrackerTransformSelector;
  vtkKWComboBoxWithLabel* TrackingSystemComboBox;
  vtkKWPushButton* ResetConnectionPushButton;
  vtkKWPushButton* ConfigureConnectionPushButton;

  //----------------------------------------------------------------
  // Widgets of the registration frame.
  //----------------------------------------------------------------
  vtkKWRadioButton* Point1RadioButton;
  vtkKWEntry* Point1REntry;
  vtkKWEntry* Point1AEntry;
  vtkKWEntry* Point1SEntry;
  vtkKWRadioButton* Point2RadioButton;
  vtkKWEntry* Point2REntry;
  vtkKWEntry* Point2AEntry;
  vtkKWEntry* Point2SEntry;
  vtkKWPushButton* ResetRegistrationPushButton;
  vtkKWPushButton* PerformRegistrationPushButton;

  //----------------------------------------------------------------
  // Widgets of the navigation frame.
  //----------------------------------------------------------------
  vtkKWCheckButton* ShowLocatorCheckButton;
  vtkKWScaleWithEntry* ProjectionLengthScale;
  vtkKWCheckButton* FreezeLocatorCheckButton;
  vtkKWCheckButton* ShowCrosshairCheckButton;
  vtkKWCheckButton* DrawNeedleCheckButton;
  vtkKWMenuButton* RedSliceMenuButton;
  vtkKWMenuButton* YellowSliceMenuButton;
  vtkKWMenuButton* GreenSliceMenuButton;
  vtkKWPushButton* SetLocatorAllPushButton;
  vtkKWPushButton* SetUserAllPushButton;
  vtkKWCheckButton* FreezeSliceCheckButton;
  vtkKWCheckButton* ObliqueCheckButton;

};

#endif // __vtkAbdoNavGUI_h
