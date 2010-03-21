
#ifndef __vtkPerkStationModuleGUI_h
#define __vtkPerkStationModuleGUI_h

#include "vtkSmartPointer.h"

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include "vtkKWWizardWidget.h"

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

#include "vtkPerkStationSecondaryMonitor.h"


/**
 * GUI class. One of the three main classes of a standard Slicer3
 * interactive module.
 */
class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationModuleGUI 
: public vtkSlicerModuleGUI
{
public:
  static vtkPerkStationModuleGUI *New();
  vtkTypeMacro( vtkPerkStationModuleGUI, vtkSlicerModuleGUI );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  
  vtkGetObjectMacro( Logic, vtkPerkStationModuleLogic );
  vtkSetObjectMacro( Logic, vtkPerkStationModuleLogic );
  
  // Description:
  // Set the logic pointer from parent class pointer.
  // Overloads implementation in vtkSlicerModulesGUI
  // to allow loadable modules.
  virtual void SetModuleLogic ( vtkSlicerLogic *logic )
  {
    this->SetLogic( reinterpret_cast< vtkPerkStationModuleLogic* >( logic ) ); 
  }
  
  
  // Description: Get/Set MRML node
  vtkGetObjectMacro( MRMLNode, vtkMRMLPerkStationModuleNode );
  vtkSetObjectMacro( MRMLNode, vtkMRMLPerkStationModuleNode );
  
  vtkPerkStationSecondaryMonitor* GetSecondaryMonitor() {
    return this->SecondaryMonitor.GetPointer();
  }
  
  vtkKWWizardWidget* GetWizardWidget();
  
  
  // Description:
  // Create widgets
  virtual void BuildGUI();

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

  void LoadPlanningVolumeButtonCallback( const char* fileName );
  void LoadValidationVolumeButtonCallback( const char* fileName );
  void LoadExperimentButtonCallback( const char* fileName );
  void SaveExperimentButtonCallback( const char* fileName );
  
  virtual void SaveExperiment(ostream& of);
  
  virtual void LoadExperiment(istream &file);
  
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
      Validate
    };
  //ETX
  

protected:
  vtkPerkStationModuleGUI();
  virtual ~vtkPerkStationModuleGUI();
  vtkPerkStationModuleGUI( const vtkPerkStationModuleGUI& );
  void operator=( const vtkPerkStationModuleGUI& );

  
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
  
  vtkSmartPointer< vtkSlicerModuleCollapsibleFrame > WizardFrame;
  
  
  vtkSmartPointer< vtkSlicerNodeSelectorWidget > VolumeSelector; 
  vtkSmartPointer< vtkSlicerNodeSelectorWidget > PSNodeSelector;
  
  
  vtkPerkStationModuleLogic* Logic;
  vtkMRMLPerkStationModuleNode* MRMLNode;
  
  
  vtkSmartPointer< vtkPerkStationSecondaryMonitor > SecondaryMonitor;
  
  
  vtkSmartPointer< vtkKWLoadSaveButton > LoadPlanningVolumeButton;
  vtkSmartPointer< vtkKWLoadSaveButton > LoadValidationVolumeButton;
  
  vtkSmartPointer< vtkKWLoadSaveButton > LoadExperimentFileButton;
  vtkSmartPointer< vtkKWLoadSaveButton > SaveExperimentFileButton;
  
    // two scales for window and level
  vtkSmartPointer< vtkKWScaleWithEntry > DisplayVolumeWindowValue;
  vtkSmartPointer< vtkKWScaleWithEntry > DisplayVolumeLevelValue;
  
    // Description:
    // Describes whether the GUI has been built or not
  bool Built;


private:

    // Description:
    // The wizard widget and steps
  vtkKWWizardWidget* WizardWidget;
  vtkSmartPointer< vtkPerkStationCalibrateStep > CalibrateStep;
  vtkSmartPointer< vtkPerkStationPlanStep > PlanStep;
  vtkSmartPointer< vtkPerkStationInsertStep > InsertStep;
  vtkSmartPointer< vtkPerkStationValidateStep > ValidateStep;
  
    // gui state variables
  int Mode; // clinical mode or training mode
  double SliceOffset;
  int ObserverCount;
  int State; // whether in calibration mode, insert, validation , or
  
  
    // Workflow phase tracking.

private:
  
  vtkSmartPointer< vtkKWFrame > WorkphaseButtonFrame;
  vtkSmartPointer< vtkKWPushButtonSet > WorkphaseButtonSet;
  
  int ChangeWorkphase( int phase );
  void UpdateWorkphaseButtons();
  
};

#endif

