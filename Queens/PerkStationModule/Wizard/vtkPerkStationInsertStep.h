#ifndef __vtkPerkStationInsertStep_h
#define __vtkPerkStationInsertStep_h

#include "vtkPerkStationStep.h"
#include <vector>

class vtkKWCheckButton;
class vtkKWCheckButtonWithLabel;
class vtkKWEntrySet;
class vtkKWFrame;
class vtkKWFrameWithLabel;
class vtkKWLabel;
class vtkKWLoadSaveButton;
class vtkKWMultiColumnList;
class vtkKWMultiColumnListWithScrollbars;
class vtkLineSource;


#if defined(USE_NDIOAPI)
class vtkNDITracker;
#endif



class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationInsertStep : public vtkPerkStationStep
{
public:
  static vtkPerkStationInsertStep *New();
  vtkTypeRevisionMacro(vtkPerkStationInsertStep,vtkPerkStationStep);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  
  void OnMultiColumnListSelectionChanged();
  void OnCalibrationSelectionChanged();
  
  
  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();

  // Description:
  // Process GUI events
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  
  void Reset();
  
  void UpdateGUI();
  
  
  // Description:
  // Tracker timer event callback to receive data from tracker and update on the GUI
  void TrackerTimerEvent();

  // Description
  // Callback on the load calibration button
  void LogFileSaveButtonCallback();

  // Description
  // Callback on the load calibration button
  void LogFileCheckButtonCallback(bool state);

protected:
  vtkPerkStationInsertStep();
  ~vtkPerkStationInsertStep();

   // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void PopulateControls();
  virtual void InstallCallbacks();
  void ResetControls();

  void AddGUIObservers();
  void RemoveGUIObservers();

  virtual void Validate();
  
  
  
  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  void ConnectTrackerCallback(bool value);
  void WriteTrackingRecordToFile(double timestamp, const double *matrix);
  bool TimerProcessing;
  char *TrackerTimerId;
  
  
    // GUI elements
  
  
    // Plan list.
  
  vtkKWFrame* PlanListFrame;
  vtkKWMultiColumnListWithScrollbars* PlanList;
    
    
    // Calibration list.
  
  vtkKWFrame* CalibrationListFrame;
  vtkKWMultiColumnListWithScrollbars* CalibrationList;
  
  
private:    
  bool ProcessingCallback;
  vtkPerkStationInsertStep(const vtkPerkStationInsertStep&);
  void operator=(const vtkPerkStationInsertStep&);
};

#endif
