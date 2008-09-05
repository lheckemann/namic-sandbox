#ifndef __vtkPerkStationValidateStep_h
#define __vtkPerkStationValidateStep_h

#include "vtkPerkStationStep.h"

class vtkKWFrame;
class vtkKWLabel;
class vtkKWEntryWithLabel;
class vtkKWEntrySet;

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

  // Description
  // reset
  virtual void Reset();



protected:
  vtkPerkStationValidateStep();
  ~vtkPerkStationValidateStep();

  // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void PopulateControls();
  virtual void InstallCallbacks();

  void ResetControls();

  // entry point RAS
  // information to be had from the user
  vtkKWFrame *EntryPointFrame;
  vtkKWLabel *EntryPointLabel; 
  vtkKWEntrySet      *EntryPoint;
  vtkKWFrame *TargetPointFrame;
  vtkKWLabel *TargetPointLabel;  
  vtkKWEntrySet      *TargetPoint; 
  vtkKWEntryWithLabel *InsertionDepth;
 
private:
  vtkPerkStationValidateStep(const vtkPerkStationValidateStep&);
  void operator=(const vtkPerkStationValidateStep&);
};

#endif
