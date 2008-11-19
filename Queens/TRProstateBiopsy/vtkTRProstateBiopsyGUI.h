/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#ifndef __vtkTRProstateBiopsyGUI_h
#define __vtkTRProstateBiopsyGUI_h

#include "vtkSlicerModuleGUI.h"
#include "vtkTRProstateBiopsy.h"
#include "vtkTRProstateBiopsyLogic.h"

class vtkKWPushButton;
class vtkKWPushButtonSet;
class vtkKWRadioButton;
class vtkKWRadioButtonSet;
class vtkKWEntryWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWMenuButton;
class vtkKWCheckButton;
class vtkKWScaleWithEntry;
class vtkKWEntry;
class vtkKWFrame;
class vtkKWEntryWithLabel;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWWizardWidget;
class vtkKWLoadSaveButton;
class vtkSlicerNodeSelectorWidget;

class vtkCallbackCommand;
class vtkTransform;

class vtkTRProstateBiopsyLogic;
class vtkTRProstateBiopsyStep;
class vtkIGTDataManager;
class vtkIGTPat2ImgRegistration;
class vtkSlicerInteractorStyle;

#include "vtkTRProstateBiopsyLogic.h"

// Description:    
// This class implements Slicer's Volumes GUI
//



class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyGUI :
  public vtkSlicerModuleGUI
{
 public:
  //BTX
  enum {
    SLICE_DRIVER_USER    = 0,
    SLICE_DRIVER_LOCATOR = 1,
    SLICE_DRIVER_RTIMAGE = 2
  };
  enum {
    SLICE_PLANE_RED    = 0,
    SLICE_PLANE_YELLOW = 1,
    SLICE_PLANE_GREEN  = 2
  };
  enum {
    SLICE_RTIMAGE_PERP      = 0,
    SLICE_RTIMAGE_INPLANE90 = 1,
    SLICE_RTIMAGE_INPLANE   = 2
  };
  
  static const double WorkPhaseColor[vtkTRProstateBiopsyLogic::NumPhases][3];
  static const double WorkPhaseColorActive[vtkTRProstateBiopsyLogic::NumPhases][3];
  static const double WorkPhaseColorDisabled[vtkTRProstateBiopsyLogic::NumPhases][3];
  static const char* WorkPhaseStr[vtkTRProstateBiopsyLogic::NumPhases];
  //ETX
  
 public:
  // Description:    
  // Usual vtk class functions
  static vtkTRProstateBiopsyGUI* New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyGUI, vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:    
  // Get methods on class members (no Set methods required)
  vtkGetObjectMacro(Logic, vtkTRProstateBiopsyLogic);
  vtkSetObjectMacro(Logic, vtkTRProstateBiopsyLogic);

  // Description: Get/Set MRML node
  vtkGetObjectMacro (MRMLNode, vtkMRMLTRProstateBiopsyModuleNode);
  vtkSetObjectMacro (MRMLNode, vtkMRMLTRProstateBiopsyModuleNode);

  // Description:
  // API for setting VolumeNode, VolumeLogic and
  // for both setting and observing them.
  void SetModuleLogic(vtkSlicerLogic *logic);

  //void SetAndObserveModuleLogic(vtkTRProstateBiopsyLogic *logic)
  //{ this->SetAndObserveLogic(vtkObjectPointer (&this->Logic), logic); };

  // Description: 
  // Get wizard widget
  vtkGetObjectMacro(WizardWidget, vtkKWWizardWidget);

  // Description:    
  // This method builds the IGTDemo module GUI
  virtual void BuildGUI();
  
  // Description:
  // Add/Remove observers on widgets in the GUI.
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();

  // Description:
  // Add/Remove observers on the module logic.
  void AddLogicObservers();
  void RemoveLogicObservers();
  
  // Description:
  // Class's mediator methods for processing events invoked by
  // either the Logic, MRML or GUI.    
  virtual void ProcessLogicEvents(vtkObject *caller, unsigned long event,
                                  void *callData);
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event,
                                void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event,
                                 void *callData);
  virtual void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  
  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter();
  virtual void Exit();
  
  // Description:
  // Method called at module initialization.
  void Init();

  //BTX
  static void DataCallback(vtkObject *caller, unsigned long eid,
                           void *clientData, void *callData);
  
  //ETX

  // Description
  // Callback on the load experiment button
  void LoadExperimentButtonCallback(const char *fileName);

  // Description
  // Callback on the save calibration button
  void SaveExperimentButtonCallback(const char *fileName);
  
 protected:
  vtkTRProstateBiopsyGUI();
  virtual ~vtkTRProstateBiopsyGUI();
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  vtkKWRadioButtonSet *WorkPhaseButtonSet;

  //----------------------------------------------------------------
  // Wizard Frame
  
  vtkKWWizardWidget *WizardWidget;
  vtkTRProstateBiopsyStep **WizardSteps;
  
  //----------------------------------------------------------------
  // Logic Values

  // logic and mrml nodes
  vtkTRProstateBiopsyLogic *Logic;
  vtkMRMLTRProstateBiopsyModuleNode *MRMLNode;

  // standard: for volume selection and for parameters
  vtkSlicerNodeSelectorWidget* VolumeSelector; 
  vtkSlicerNodeSelectorWidget* TRNodeSelector;

  // 1)  button: open file dialog box
  vtkKWLoadSaveButton *LoadExperimentFileButton;
  // 2) button: save calib file dialog box
  vtkKWLoadSaveButton *SaveExperimentFileButton;


  vtkCallbackCommand *DataCallbackCommand;

  // Description:
  // Updates GUI widgets based on parameters values in MRML node
  void UpdateGUI();

  // Description:
  // Updates parameters values in MRML node based on GUI widgets 
  void UpdateMRML();

  void UpdateAll();

 protected:
 
  // optical encoder timer related
  bool TimerProcessing;
  void OpticalEncoderTimerEvent();

 private:

  vtkTRProstateBiopsyGUI(const vtkTRProstateBiopsyGUI&); // Not implemented.
  void operator=(const vtkTRProstateBiopsyGUI&); //Not implemented.
  
  void BuildGUIForWorkPhaseFrame();
  void BuildGUIForModuleParamsVolumeAndExperimentFrame();
  void BuildGUIForWizardFrame();
  void BuildGUIForHelpFrame();
  
  int  ChangeWorkPhase(int phase, int fChangeWizard=0);
};



#endif
