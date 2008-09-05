#ifndef __vtkPerkStationEvaluateStep_h
#define __vtkPerkStationEvaluateStep_h

#include "vtkPerkStationStep.h"

class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWLabel;
class vtkKWEntrySet;
class vtkKWEntryWithLabel;

class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationEvaluateStep : public vtkPerkStationStep
{
public:
  static vtkPerkStationEvaluateStep *New();
  vtkTypeRevisionMacro(vtkPerkStationEvaluateStep,vtkPerkStationStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();
  virtual void HideUserInterface();

  // Description:
  // Callbacks.
  // TO DO:
  /*virtual void IntensityImagesTargetSelectionChangedCallback();
  virtual void AlignTargetImagesCallback(int state);*/

  void StartOverNewExperiment();

  void Reset();
  // Description:
  // Observers.
  /*virtual void AddRunRegistrationOutputGUIObservers();
  virtual void RemoveRunRegistrationOutputGUIObservers();
  virtual void ProcessRunRegistrationOutputGUIEvents(
    vtkObject *caller, unsigned long event, void *callData);*/

protected:
  vtkPerkStationEvaluateStep();
  ~vtkPerkStationEvaluateStep();

  // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void PopulateControls();
  virtual void InstallCallbacks();
  
  void ResetControls();

  // for calibrate step
  vtkKWFrameWithLabel *CalibrateErrorsFrame;
  // information for the user
  vtkKWFrame *CalibrateScaleErrorFrame;
  vtkKWLabel *CalibrateScaleErrorLabel;
  vtkKWEntrySet      *CalibrateScaleError; // read only

  // information for the user
  vtkKWFrame *CalibrateTranslationErrorFrame;
  vtkKWLabel *CalibrateTranslationErrorLabel;
  vtkKWEntrySet      *CalibrateTranslationError; // read only  
  
  vtkKWEntryWithLabel      *CalibrateRotationError; // read only  
  

  // for plan step
  vtkKWFrameWithLabel *PlanErrorsFrame;
  vtkKWEntryWithLabel      *PlanInsertionAngleError; // read only
  vtkKWEntryWithLabel      *PlanInsertionDepthError; // read only
  
   
  // for insertion/validation
  vtkKWFrameWithLabel *InsertionValidationErrorsFrame;
  vtkKWEntryWithLabel      *EntryPointError; // read only  
  vtkKWEntryWithLabel      *TargetPointError; // read only
   

private:
  vtkPerkStationEvaluateStep(const vtkPerkStationEvaluateStep&);
  void operator=(const vtkPerkStationEvaluateStep&);
};

#endif
