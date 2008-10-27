#ifndef __vtkPerkStationValidateStep_h
#define __vtkPerkStationValidateStep_h

#include "vtkPerkStationStep.h"

class vtkKWFrame;
class vtkKWLabel;
class vtkKWEntryWithLabel;
class vtkKWEntrySet;
class vtkKWPushButton;

class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationValidateStep : public vtkPerkStationStep
{
public:
  static vtkPerkStationValidateStep *New();
  vtkTypeRevisionMacro(vtkPerkStationValidateStep,vtkPerkStationStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();

  // Description:
  // Callbacks.
  // TO DO:
  /*virtual void IntensityImagesTargetSelectionChangedCallback();
  virtual void AlignTargetImagesCallback(int state);*/
  void ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description:
  // Process GUI events
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description
  // reset
  virtual void Reset();

  void LoadValidation(istream &file);

  void SaveValidation(ostream& of);

  void StartOverNewExperiment();
protected:
  vtkPerkStationValidateStep();
  ~vtkPerkStationValidateStep();

  void AddGUIObservers();
  void RemoveGUIObservers();
  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);


  // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void PopulateControls();
  virtual void InstallCallbacks();

  void ResetControls();

  // in clinical mode
  // reset push button
  vtkKWPushButton *ResetValidationButton;
  vtkKWFrame *ResetFrame;

  // entry point RAS
  // information to be had from the user
  vtkKWFrame *EntryPointFrame;
  vtkKWLabel *EntryPointLabel; 
  vtkKWEntrySet      *EntryPoint;
  vtkKWFrame *TargetPointFrame;
  vtkKWLabel *TargetPointLabel;  
  vtkKWEntrySet      *TargetPoint; 
  vtkKWEntryWithLabel *InsertionDepth;
 
  bool EntryTargetAcquired;
  unsigned int ClickNumber;

private:
  bool ProcessingCallback;

  vtkPerkStationValidateStep(const vtkPerkStationValidateStep&);
  void operator=(const vtkPerkStationValidateStep&);
};

#endif
