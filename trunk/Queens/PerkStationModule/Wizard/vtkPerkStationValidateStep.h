
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


class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationValidateStep : public vtkPerkStationStep
{
public:
  
  static vtkPerkStationValidateStep *New();
  vtkTypeRevisionMacro(vtkPerkStationValidateStep,vtkPerkStationStep);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void ShowUserInterface();
  virtual void ShowPlanListFrame();

  void ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData);
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  void OnMultiColumnListSelectionChanged();
  
  virtual void Reset();
  
  void UpdateGUI();
  
  void StartOverNewExperiment();
  
  
protected:
  
  vtkPerkStationValidateStep();
  ~vtkPerkStationValidateStep();

  void AddGUIObservers();
  void RemoveGUIObservers();
  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  virtual void InstallCallbacks();
  
  
  void OverlayValidationNeedleAxis();
  void RemoveValidationNeedleAxis();
  void OverlayPlan( bool visible );
  
  
  void PresentValidationErrors();

  
    // GUI widgets.
  
    // Plan list.
  vtkKWFrame*                          PlanListFrame;
  vtkKWMultiColumnListWithScrollbars*  PlanList;
  
  
    // reset push button
  vtkKWFrame*       ResetFrame;
  vtkKWPushButton*  ResetValidationButton;
  
  
    // validation points
    
  vtkKWFrame*     EntryPointFrame;
  vtkKWLabel*     EntryPointLabel; 
  vtkKWEntrySet*  EntryPoint;
  
  vtkKWFrame*     TargetPointFrame;
  vtkKWLabel*     TargetPointLabel;  
  vtkKWEntrySet*  TargetPoint; 
  
  vtkKWEntryWithLabel* InsertionDepth;


    // for insertion/validation
  
  vtkKWFrameWithLabel* ValidationErrorsFrame;
  vtkKWEntryWithLabel* EntryPointError;
  vtkKWEntryWithLabel* TargetPointError;
  vtkKWEntryWithLabel* InsertionAngleError;
  vtkKWEntryWithLabel* InsertionDepthError;
  
  vtkKWFrameWithLabel* TimePerformanceFrame;
  vtkKWEntryWithLabel* CalibrationTime;
  vtkKWEntryWithLabel* PlanTime;
  vtkKWEntryWithLabel* InsertionTime;

  
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
