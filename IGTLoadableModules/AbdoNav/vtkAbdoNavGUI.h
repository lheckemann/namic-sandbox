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

/* Slicer includes */
#include "vtkSlicerModuleGUI.h"

/* Slicer forward declarations */
class vtkSlicerNodeSelectorWidget;

/* KWWidgets forward declarations */
class vtkKWComboBoxWithLabel;
class vtkKWFrameWithLabel;
class vtkKWPushButton;

class VTK_AbdoNav_EXPORT vtkAbdoNavGUI : public vtkSlicerModuleGUI
{
 public:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  static vtkAbdoNavGUI* New();
  vtkTypeRevisionMacro(vtkAbdoNavGUI, vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);
  // Get the module's category
  const char* GetCategory() const { return "IGT"; }
  // Set the module logic
  void SetModuleLogic(vtkSlicerLogic* logic) { this->SetLogic(vtkObjectPointer(&this->Logic), logic); }

  //----------------------------------------------------------------
  // Get/Set methods for class members.
  vtkGetObjectMacro(Logic, vtkAbdoNavLogic);

  //----------------------------------------------------------------
  // Define behavior at module initialization, startup and exit.
  virtual void Init();  // called upon Slicer startup; not implemented
  virtual void Enter(); // called upon entering the module
  virtual void Exit();  // called upon exiting the module; not implemented

  //----------------------------------------------------------------
  // Build the GUI.
  virtual void BuildGUI();

  //----------------------------------------------------------------
  // Add/Remove observers, event handlers, etc.
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  void         AddLogicObservers();
  void         RemoveLogicObservers();
  void         AddMRMLObservers();
  void         RemoveMRMLObservers();

  //----------------------------------------------------------------
  // Mediator methods for processing events invoked by logic, GUI,
  // MRML, timer or mouse.
  virtual void ProcessGUIEvents(vtkObject* caller, unsigned long event, void* callData);
  virtual void ProcessLogicEvents(vtkObject* caller, unsigned long event, void* callData);
  virtual void ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData);
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle* style);
  static void  DataCallback(vtkObject* caller, unsigned long eventid, void* clientData, void* callData);
  void UpdateAll(); // not implemented

  void UpdateMRML();
  void UpdateGUI();

 protected:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  vtkAbdoNavGUI();
  virtual ~vtkAbdoNavGUI();

 private:
  //----------------------------------------------------------------
  // Usual VTK class functions.
  vtkAbdoNavGUI(const vtkAbdoNavGUI&);  // not implemented
  void operator=(const vtkAbdoNavGUI&); // not implemented

  //----------------------------------------------------------------
  // Logic values.
  vtkAbdoNavLogic* Logic;
  vtkMRMLAbdoNavNode* AbdoNavNode;
  vtkCallbackCommand* DataCallbackCommand;
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // Helper functions to build the different GUI frames.
  void BuildGUIHelpFrame();
  void BuildGUIConnectionFrame();
  void BuildGUIRegistrationFrame();

  //----------------------------------------------------------------
  // Widgets of the connection frame.
  vtkKWFrameWithLabel* TrackerFrame;
  vtkSlicerNodeSelectorWidget* TrackerNodeSelectorWidget;
  vtkKWComboBoxWithLabel* TrackerComboxBox;
  vtkKWPushButton* ResetConnectionPushButton;
  vtkKWPushButton* ConfigureConnectionPushButton;

  //----------------------------------------------------------------
  // Widgets of the registration frame.
  vtkKWFrameWithLabel* GuidanceNeedleFrame;
  vtkKWFrame* Point1Frame;
  vtkKWRadioButton* Point1RadioButton;
  vtkKWEntry* Point1XEntry;
  vtkKWEntry* Point1YEntry;
  vtkKWEntry* Point1ZEntry;
  vtkKWFrame* Point2Frame;
  vtkKWRadioButton* Point2RadioButton;
  vtkKWEntry* Point2XEntry;
  vtkKWEntry* Point2YEntry;
  vtkKWEntry* Point2ZEntry;
  vtkKWPushButton* ResetRegistrationPushButton;
  vtkKWPushButton* PerformRegistrationPushButton;

};

#endif // __vtkAbdoNavGUI_h
