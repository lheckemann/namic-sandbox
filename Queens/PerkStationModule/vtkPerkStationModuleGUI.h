
#ifndef __vtkPerkStationModuleGUI_h
#define __vtkPerkStationModuleGUI_h


#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationModuleLogic.h"
class vtkPerkStationSecondaryMonitor;

#include "vtkKWWizardWidget.h"

#include "vtkTimerLog.h"

#include "PerkStationCommon.h"

#include "vtkSmartPointer.h"

class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWPushButton;
class vtkKWComboBoxWithLabel;
class vtkKWPushButtonSet;
class vtkKWMenuButtonWithLabel;
class vtkKWLoadSaveButton;
class vtkKWScale;
class vtkKWScaleWithEntry;
class vtkSlicerNodeSelectorWidget;

class vtkPerkStationCalibrateStep;
class vtkPerkStationPlanStep;
class vtkPerkStationInsertStep;
class vtkPerkStationValidateStep;



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
  
  
  virtual void SetModuleLogic ( vtkSlicerLogic *logic ) {
    this->SetLogic( reinterpret_cast< vtkPerkStationModuleLogic* >( logic ) ); 
  }
  
  
  vtkGetObjectMacro( MRMLNode, vtkMRMLPerkStationModuleNode );
  vtkSetObjectMacro( MRMLNode, vtkMRMLPerkStationModuleNode );
  
  
  vtkPerkStationSecondaryMonitor* GetSecondaryMonitor();
  vtkKWWizardWidget* GetWizardWidget();
  
  
  virtual void BuildGUI();
  virtual void TearDownGUI();
  
  virtual void AddGUIObservers();
  virtual void RemoveGUIObservers();
  
  void AddMRMLObservers();
  void RemoveMRMLObservers();
  
  
  virtual void ResetAndStartNewExperiment();
  
  
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
  
  void TimerHandler();
  
  //BTX
  vtkSmartPointer< vtkMRMLFiducialListNode > TwoFiducials;
  //ETX
  
  
protected:
  
  vtkPerkStationModuleGUI();
  virtual ~vtkPerkStationModuleGUI();
  vtkPerkStationModuleGUI( const vtkPerkStationModuleGUI& );
  void operator=( const vtkPerkStationModuleGUI& );
  
  
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData ){};
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  
    // Describe behavior at module startup and exit.
  
  virtual void Enter( vtkMRMLNode *node );
  virtual void Enter();
  virtual void Exit();
  
  
    // Handle mrml node events.
  
  virtual void PlanningVolumeChanged();
  virtual void ValidationVolumeChanged();
  
  
  void UpdateGUI();
  void UpdateMRML();
  
  
  //BTX
    // Collaborator classes.
  
  vtkPerkStationModuleLogic*      Logic;
  vtkMRMLPerkStationModuleNode*   MRMLNode;
  vtkPerkStationSecondaryMonitor* SecondaryMonitor;
  //ETX
  
  
  bool Built;
  
  
private:
  
  virtual void BuildGUIForHelpFrame();
  virtual void BuildGUIForExperimentFrame();
  virtual void BuildGUIForWorkphases();
  
  int ChangeWorkphase( int phase );
  void UpdateWorkphaseButtons();
  
  void UpdateTimerDisplay();
  
  
  // GUI widgets ----------------------------------------------------
  
  
  //BTX
  
  vtkKWFrameWithLabel*          InputFrame;
  vtkSlicerNodeSelectorWidget*  VolumeSelector;
  vtkSlicerNodeSelectorWidget*  ValidationVolumeSelector;
  vtkSlicerNodeSelectorWidget*  PSNodeSelector;
  
  vtkKWFrame*            TimerFrame;
  vtkKWLabel*            CalibrateTimeLabel;
  vtkKWLabel*            PlanTimeLabel;
  vtkKWLabel*            InsertTimeLabel;
  vtkKWLabel*            ValidateTimeLabel;
  vtkKWPushButton*       TimerButton;
  vtkKWPushButton*       ResetTimerButton;
  
  vtkKWFrame*            WorkphaseButtonFrame;
  vtkKWPushButtonSet*    WorkphaseButtonSet;
  
  vtkSlicerModuleCollapsibleFrame* WizardFrame;
  vtkKWWizardWidget*               WizardWidget;
  vtkPerkStationCalibrateStep*     CalibrateStep;
  vtkPerkStationPlanStep*          PlanStep;
  vtkPerkStationInsertStep*        InsertStep;
  vtkPerkStationValidateStep*      ValidateStep;
  
  
    // gui state variables
  
  bool Entered;
  double SliceOffset;
  int ObserverCount;
  int State;  // Calibration, planning, insertion and validation.
  double CurrentWindow;
  double CurrentLevel;
  
  
    // Time spent by calibration, planning, insertion and validation.
  
  vtkTimerLog* TimerLog;
  double WorkingTimes[ 4 ];
  double LastTime;
  bool TimerOn;
  
  //ETX
  
};

#endif

