/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkPerkStationModuleGUI.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkPerkStationModuleGUI_h
#define __vtkPerkStationModuleGUI_h

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkPerkStationModuleLogic.h"


class vtkKWWizardWidget;
class vtkKWFrame;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWComboBoxWithLabel;
class vtkKWPushButtonSet;
class vtkKWMenuButtonWithLabel;
class vtkKWLoadSaveButton;
class vtkKWScale;
class vtkKWScaleWithEntry;

class vtkPerkStationCalibrateStep;
class vtkPerkStationPlanStep;
class vtkPerkStationInsertStep;
class vtkPerkStationValidateStep;
class vtkPerkStationEvaluateStep;

class vtkPerkStationSecondaryMonitor;

/**
 * GUI class. One of the three main classes of a standard Slicer3
 * interactive module.
 */
class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationModuleGUI : public vtkSlicerModuleGUI
{
  public:
  static vtkPerkStationModuleGUI *New();
  vtkTypeMacro(vtkPerkStationModuleGUI,vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: Get/Set module logic
  vtkGetObjectMacro (Logic, vtkPerkStationModuleLogic);
  vtkSetObjectMacro (Logic, vtkPerkStationModuleLogic);

  // Description:
  // Set the logic pointer from parent class pointer.
  // Overloads implementation in vtkSlicerModulesGUI
  // to allow loadable modules.
  virtual void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic(reinterpret_cast<vtkPerkStationModuleLogic*> (logic)); 
  }

  // Description: Get/Set MRML node
  vtkGetObjectMacro (MRMLNode, vtkMRMLPerkStationModuleNode);
  vtkSetObjectMacro (MRMLNode, vtkMRMLPerkStationModuleNode);

  // Description: Get Secondary monitor
  vtkGetObjectMacro (SecondaryMonitor, vtkPerkStationSecondaryMonitor);

  // Description: 
  // Get wizard widget
  vtkGetObjectMacro(WizardWidget, vtkKWWizardWidget);

  // Description:
  // Get modelist menu
  vtkGetObjectMacro(ModeListMenu, vtkKWMenuButtonWithLabel);

  // Description
  // Get/set mode of Perk station (clinical or training)
  vtkGetMacro(Mode,int);
  vtkSetMacro(Mode,int);

  // Description:
  // Create widgets
  virtual void BuildGUI ( );

  // Description:
  // Delete Widgets
  virtual void TearDownGUI();

  // Description:
  // Add obsereves to GUI widgets
  virtual void AddGUIObservers ( );
  
  // Description:
  // Remove obsereves to GUI widgets
  virtual void RemoveGUIObservers ( );
  
  // Description:
  // Pprocess events generated by Logic
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event,
                                  void *callData ){};

  // Description:
  // Pprocess events generated by GUI widgets
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event,
                                  void *callData );

  // Description:
  // Create render window in the secondary monitor
  virtual void RenderSecondaryMonitor ( );

  // Description
  // Reset and Start afresh
  virtual void ResetAndStartNewExperiment();

  // Description
  // Callback on the load calibration button
  void LoadPlanningVolumeButtonCallback(const char *fileName);
  // Description
  // Callback on the load calibration button
  void LoadValidationVolumeButtonCallback(const char *fileName);
  // Description
  // Callback on the load experiment button
  void LoadExperimentButtonCallback(const char *fileName);

  // Description
  // Callback on the save calibration button
  void SaveExperimentButtonCallback(const char *fileName);

  // Description
  // Save experiment
  virtual void SaveExperiment(ostream& of);

  // Description
  // Load experiment
  virtual void LoadExperiment(istream &file);

  // Description
  // Save information about open volumes
  void SaveVolumeInformation(ostream& of);

  void EnableLoadValidationVolumeButton(bool enable);

  //BTX
    // Description:
    // Mode identifier
    enum ModeId
      {
        Clinical = 1,
        Training = 2,
      };
    //ETX

    //BTX
    // Description:
    // State identifier
    enum StateId
      {
        Calibrate = 0,
        Plan,
        Insert,
        Validate,
        Evaluate
      };
    //ETX
  

protected:
  vtkPerkStationModuleGUI();
  virtual ~vtkPerkStationModuleGUI();
  vtkPerkStationModuleGUI(const vtkPerkStationModuleGUI&);
  void operator=(const vtkPerkStationModuleGUI&);

  
  // Pprocess events generated by MRML
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, 
                                   void *callData);
  
  // Describe behavior at module startup and exit.
  virtual void Enter ( );
  virtual void Exit ( ){};

  
  // Updates GUI widgets based on parameters values in MRML node
  void UpdateGUI();

  void UpdateMRML();

  // Description
  // For creating a unique filename, to save the experiment
  char  *CreateFileName();

  // Description
  // To set up environment depending upon the mode
  void SetUpPerkStationMode();
  
  void SetUpPerkStationWizardWorkflow();
  
  vtkSlicerModuleCollapsibleFrame *WizardFrame;
  
    // standard: for volume selection and for parameters
  vtkSlicerNodeSelectorWidget* VolumeSelector; 
  vtkSlicerNodeSelectorWidget* PSNodeSelector;
  
  // logic and mrml nodes
  vtkPerkStationModuleLogic *Logic;
  vtkMRMLPerkStationModuleNode *MRMLNode;
  
  
  vtkPerkStationSecondaryMonitor *SecondaryMonitor; // secondary monitor
  
  vtkKWMenuButtonWithLabel *ModeListMenu; // mode selector menu
  
  
    // 1)  button: open file dialog box
  vtkKWLoadSaveButton *LoadPlanningVolumeButton;
    // 2) button: save calib file dialog box
  vtkKWLoadSaveButton *LoadValidationVolumeButton;
  
  
    // 1)  button: open file dialog box
  vtkKWLoadSaveButton *LoadExperimentFileButton;
    // 2) button: save calib file dialog box
  vtkKWLoadSaveButton *SaveExperimentFileButton;
  
    // two scales for window and level
  vtkKWScaleWithEntry *DisplayVolumeWindowValue;
  vtkKWScaleWithEntry *DisplayVolumeLevelValue;
  
    // Description:
    // Describes whether the GUI has been built or not
  bool Built;


private:

    // Description:
    // The wizard widget and steps
  vtkKWWizardWidget *WizardWidget;
  vtkPerkStationCalibrateStep *CalibrateStep;
  vtkPerkStationPlanStep *PlanStep;
  vtkPerkStationInsertStep *InsertStep;
  vtkPerkStationValidateStep *ValidateStep;
  vtkPerkStationEvaluateStep *EvaluateStep;
  
    // gui state variables
  int Mode; // clinical mode or training mode
  double SliceOffset;
  int ObserverCount;
  int State; // whether in calibration mode, insert, validation , or
  
  
    // Workflow phase tracking.

private:
  
  vtkKWFrame *WorkphaseButtonFrame;
  vtkKWPushButtonSet *WorkphaseButtonSet;
  
  int ChangeWorkphase( int phase );
  void UpdateWorkphaseButtons();
  
};

#endif
