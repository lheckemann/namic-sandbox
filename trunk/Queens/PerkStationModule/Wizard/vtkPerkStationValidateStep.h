
#ifndef __vtkPerkStationValidateStep_h
#define __vtkPerkStationValidateStep_h

#include "vtkPerkStationStep.h"


class vtkKWFrame;
class vtkKWLabel;
class vtkKWEntryWithLabel;
class vtkKWEntrySet;
class vtkKWMultiColumnList;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWPushButton;
class vtkKWFrameWithLabel;
class vtkActor;


class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationValidateStep
: public vtkPerkStationStep
{
public:
  
  static vtkPerkStationValidateStep *New();
  vtkTypeRevisionMacro( vtkPerkStationValidateStep, vtkPerkStationStep );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  virtual void ShowUserInterface();
  virtual void ShowPlanListFrame();
  virtual void ShowValidationListFrame();

  void ProcessImageClickEvents( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData );
  
  void OnMultiColumnListSelectionChanged();
  void OnSliceOffsetChanged( double offset );
  
  
  virtual void Reset();
  
  void UpdateGUI();
  void HideOverlays();
  
protected:
  
  vtkPerkStationValidateStep();
  ~vtkPerkStationValidateStep();

  void AddGUIObservers();
  void RemoveGUIObservers();
  
  static void WizardGUICallback( vtkObject *caller, unsigned long event, void *clientData, void *callData );

  virtual void InstallCallbacks();
  
  
  void OverlayValidationNeedleAxis();
  void RemoveValidationNeedleAxis();
  void OverlayPlan( bool visible );
  
  
  void PresentValidationErrors();

  
    // GUI widgets. -----------------------------------------------------------
  
  
    // Plan list.
  
  vtkKWFrame*                          PlanListFrame;
  vtkKWLabel*                          PlanListLabel;
  vtkKWMultiColumnListWithScrollbars*  PlanList;
  
  
    // reset push button
  
  vtkKWFrame*       ResetFrame;
  vtkKWPushButton*  ResetValidationButton;
  
  
    // validation points
  
  vtkKWEntryWithLabel* InsertionDepth;


    // for insertion/validation
  
  vtkKWFrame*                          ValidationListFrame;
  vtkKWLabel*                          ValidationListLabel;
  vtkKWMultiColumnListWithScrollbars*  ValidationList;
  
  vtkKWFrameWithLabel* ValidationErrorsFrame;
  vtkKWEntryWithLabel* EntryPointError;
  vtkKWEntryWithLabel* TargetPointError;
  vtkKWEntryWithLabel* InsertionAngleError;
  vtkKWEntryWithLabel* InsertionDepthError;
  
  
    // On second monitor.
  
  vtkActor* ValidationNeedleActor;
  vtkActor* PlanActor;
  
  
    // Helper functions.
  
  void RasToWorld( const double ras[ 3 ], double wc[ 3 ] );
  
  
private:
  
  unsigned int ClickNumber;
  
  bool ProcessingCallback;

  vtkPerkStationValidateStep( const vtkPerkStationValidateStep& );
  void operator=( const vtkPerkStationValidateStep& );
};

#endif
