#ifndef __vtkPerkStationInsertStep_h
#define __vtkPerkStationInsertStep_h

#include "vtkPerkStationStep.h"
#include <vector>

class vtkKWFrame;
class vtkKWLabel;
class vtkKWFrameWithLabel;
class vtkKWEntrySet;
class vtkKWCheckButtonWithLabel;
class vtkKWLoadSaveButton;
class vtkKWCheckButton;
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

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();

  // Description:
  // Process GUI events
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description:
  // Callbacks.
  // TO DO:
  void Reset();

  // Description:
  // Load/save information from the file
  void LoadInsertion(istream &file);
  void SaveInsertion(ostream& of);

  // Description:
  // Tracker timer event callback to receive data from tracker and update on the GUI
  void TrackerTimerEvent();

  // Description
  // Callback on the load calibration button
  void LoadConfigButtonCallback();

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
  
  bool LogToFile;
  

/*  virtual void PopulateNormalizationTargetVolumeSelector();
  virtual void ResetDefaultParameters(vtkIdType target_vol_id);*/

  // GUI elements
  // button, msg for loading the registration file
  vtkKWFrameWithLabel *LoadTrackerConfigFrame;
  vtkKWLabel *TrackerConfigFileLoadMsg;  
  vtkKWLoadSaveButton *LoadTrackerConfigFileButton;
  
   //BTX
  std::string ConfigFileName;  
  std::string InsertionLogFileName;
  //ETX


  // tracker connection, and status msg
  vtkKWFrameWithLabel *TrackerConnectionFrame;
  vtkKWCheckButtonWithLabel *ConnectTrackerCheckButton;
  vtkKWLabel *TrackerStatusMsg;

  // needle tip position info
  vtkKWFrameWithLabel *NeedleToolFrame;
  vtkKWFrame *NeedleTipPositionFrame;
  vtkKWLabel *NeedleTipPositionLabel;
  vtkKWEntrySet       *NeedleTipPosition;

  // tool tip offset infor
  vtkKWFrame *ToolTipOffsetFrame;
  vtkKWLabel *ToolTipOffsetLabel;
  vtkKWEntrySet       *ToolTipOffset;

  // GUI controls for logging the insertion attempt
  vtkKWFrameWithLabel *LoggingFrame;
  vtkKWCheckButton *StartStopLoggingToFileCheckButton;
  vtkKWLabel *LogFileLoadMsg;  
  vtkKWLoadSaveButton *LogFileButton;
  //BTX
  #if defined(USE_NDIOAPI)
  vtkNDITracker *Tracker;
  FILE *InsertionLogFile;
  #endif
  //ETX
private:    
  bool ProcessingCallback;
  vtkPerkStationInsertStep(const vtkPerkStationInsertStep&);
  void operator=(const vtkPerkStationInsertStep&);
};

#endif
