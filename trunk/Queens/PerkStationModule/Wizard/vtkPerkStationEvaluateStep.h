#ifndef __vtkPerkStationEvaluateStep_h
#define __vtkPerkStationEvaluateStep_h

#include "vtkPerkStationStep.h"


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


private:
  vtkPerkStationEvaluateStep(const vtkPerkStationEvaluateStep&);
  void operator=(const vtkPerkStationEvaluateStep&);
};

#endif
