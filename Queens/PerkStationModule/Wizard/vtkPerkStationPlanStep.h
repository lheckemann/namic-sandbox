#ifndef __vtkPerkStationPlanStep_h
#define __vtkPerkStationPlanStep_h

#include "vtkPerkStationStep.h"

class vtkKWLabel;
class vtkKWFrame;
class vtkKWEntryWithLabel;
class vtkKWEntrySet;
class vtkActor;

class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationPlanStep : public vtkPerkStationStep
{
public:
  static vtkPerkStationPlanStep *New();
  vtkTypeRevisionMacro(vtkPerkStationPlanStep,vtkPerkStationStep);
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
  // Callback on value entered in the Insertion angle entry
  virtual void InsertionAngleEntryCallback(double value);

  // Description:
  // Callback on value entered in the Insertion depth entry
  virtual void InsertionDepthEntryCallback(double value);

  // Description:
  // Reimplement the superclass's method.
  //virtual void Validate();

  virtual void OverlayNeedleGuide();

  // Description:
  // reset
  virtual void Reset();
  
  void RemoveOverlayNeedleGuide();
protected:
  vtkPerkStationPlanStep();
  ~vtkPerkStationPlanStep();

  // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void PopulateControls();
  virtual void InstallCallbacks();

  void ResetControls();

  virtual void CalculatePlanInsertionAngleAndDepth();
  // entry point RAS
  // information to be had from the user
  vtkKWFrame *EntryPointFrame;
  vtkKWLabel *EntryPointLabel;
  vtkKWEntrySet      *EntryPoint;
  vtkKWFrame *TargetPointFrame;
  vtkKWLabel *TargetPointLabel;  
  vtkKWEntrySet      *TargetPoint;
  vtkKWEntryWithLabel *InsertionAngle;
  vtkKWEntryWithLabel *InsertionDepth;

  double WCEntryPoint[3];
  double WCTargetPoint[3];

  vtkActor *NeedleActor;
private:
  vtkPerkStationPlanStep(const vtkPerkStationPlanStep&);
  void operator=(const vtkPerkStationPlanStep&);
};

#endif
