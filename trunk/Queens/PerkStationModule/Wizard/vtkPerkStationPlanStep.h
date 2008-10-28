#ifndef __vtkPerkStationPlanStep_h
#define __vtkPerkStationPlanStep_h

#include "vtkPerkStationStep.h"

class vtkKWLabel;
class vtkKWFrame;
class vtkKWEntryWithLabel;
class vtkKWEntrySet;
class vtkKWPushButton;
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
  // Process GUI events
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description:
  // Callback on value entered in the Insertion angle entry
  virtual void InsertionAngleEntryCallback();

  // Description:
  // Callback on value entered in the Insertion depth entry
  virtual void InsertionDepthEntryCallback();

  // Description:
  // Reimplement the superclass's method.
  //virtual void Validate();

  virtual void OverlayNeedleGuide();

  // Description:
  // reset
  virtual void Reset();
  
  void RemoveOverlayNeedleGuide();

  void LoadPlanning(istream &file);

  void SavePlanning(ostream& of);

  void PopulateControlsOnLoadPlanning();

protected:
  vtkPerkStationPlanStep();
  ~vtkPerkStationPlanStep();

  void AddGUIObservers();
  void RemoveGUIObservers();
  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void PopulateControls();
  virtual void InstallCallbacks();

  void ResetControls();
  bool DoubleEqual(double val1, double val2);
  virtual void CalculatePlanInsertionAngleAndDepth();


  // in clinical mode
  
  // reset push button
  vtkKWPushButton *ResetPlanButton;
  vtkKWFrame *ResetFrame;

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

  bool EntryTargetAcquired;
  unsigned int ClickNumber;
  vtkActor *NeedleActor;
private:
  bool ProcessingCallback;

  vtkPerkStationPlanStep(const vtkPerkStationPlanStep&);
  void operator=(const vtkPerkStationPlanStep&);
};

#endif
