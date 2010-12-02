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

#ifdef WIN32
#include "vtkAbdoNavWin32Header.h"
#endif
#include "vtkAbdoNavLogic.h"

#include "vtkSlicerModuleGUI.h"

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWSeparator;

class VTK_AbdoNav_EXPORT vtkAbdoNavGUI : public vtkSlicerModuleGUI
{
 public:
  // Description:
  // Usual vtk class functions
  static vtkAbdoNavGUI *New();
  vtkTypeRevisionMacro(vtkAbdoNavGUI, vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the categorization of the module.
  const char *GetCategory() const { return "IGT"; }

  // Description:
  // Get methods on class members (no Set methods required)
  vtkGetObjectMacro ( Logic, vtkAbdoNavLogic );

  // Description:
  // API for setting VolumeNode, VolumeLogic and
  // for both setting and observing them.
  void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic ); }

 public:
  // Description:
  // This method builds the IGTDemo module GUI
  virtual void BuildGUI ( );

 private:
  // the usual help and acknowledgement frame
  void BuildGUIHelpFrame();
  // the frame to setup the connection via OpenIGTLinkIF
  void BuildGUIConnectionFrame();

 public:
  // Description:
  // Add/Remove observers and even handlers
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  void         AddMRMLObservers();
  void         RemoveMRMLObservers();
  void         AddLogicObservers();
  void         RemoveLogicObservers();

  void HandleMouseEvent(vtkSlicerInteractorStyle *style);

  // Description:
  // Class's mediator methods for processing events invoked by
  // either the Logic, MRML or GUI.
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void         ProcessTimerEvents();
  //BTX
  static void  DataCallback(vtkObject *caller, unsigned long eid, void *clientData, void *callData);
  //ETX

 public:
  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter();
  virtual void Exit();
  void UpdateAll();

  //----------------------------------------------------------------
  // Constructor / Destructor
  //----------------------------------------------------------------
 protected:
  vtkAbdoNavGUI();
  virtual ~vtkAbdoNavGUI();

 private:
  vtkAbdoNavGUI(const vtkAbdoNavGUI&); // Not implemented.
  void operator=(const vtkAbdoNavGUI&); // Not implemented.

 private:
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  int TimerFlag;
  int TimerInterval;

  vtkAbdoNavLogic* Logic;
  vtkCallbackCommand* DataCallbackCommand;

  vtkSlicerNodeSelectorWidget* GuidanceNeedleSelectorWidget;
  vtkSlicerNodeSelectorWidget* CryoprobeSelectorWidget;
  vtkKWPushButton* ConfigurePushButton;
  vtkKWPushButton* ResetPushButton;
  vtkKWSeparator* SeparatorBeforeButtons;
};

#endif // __vtkAbdoNavGUI_h
