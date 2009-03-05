#ifndef __vtkTRProstateBiopsyCalibrationStep_h
#define __vtkTRProstateBiopsyCalibrationStep_h

#include "vtkTRProstateBiopsyStep.h"

#include "vtkstd/vector"

class vtkKWPushButton;
class vtkKWSpinBoxWithLabel;
class vtkKWCheckButton;
class vtkKWScaleWithEntry;
class vtkKWFrame;
class vtkKWLabel;
class vtkKWText;
class vtkKWEntrySet;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWLoadSaveButton;

class vtkSlicerInteractorStyle;
class vtkMRMLVolumeNode;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyCalibrationStep :
  public vtkTRProstateBiopsyStep
{
public:
  static vtkTRProstateBiopsyCalibrationStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyCalibrationStep,
                       vtkTRProstateBiopsyStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();

  // Description:
  // The Enter and Exit functions are not part of vtkKWWizardStep.
  // They have been added here sot that they can be called by
  // ShowUserInterface and HideUserInterface.
  virtual void Enter();
  virtual void Exit();


  // Description:
  // Process GUI events
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event,
                                void *callData);

   // Description:
  // Callbacks to capture mouse click on image
  virtual void ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description
  // Reset
  virtual void Reset();

  // Description
  // Callback on the load calibration button
  void LoadCalibrationVolumeButtonCallback(const char *fileName);


  // Description
  // Callback on the load calibration button
  void LoadCalibrationSettingsFileButtonCallback(const char *fileName);

  // Description
  // Callback on the save calibration button
  void SaveCalibrationSettingsFileButtonCallback(const char *fileName);

  // Description
  // Save calibration
  virtual void SaveCalibrationSettings(ostream& of);

  // Description
  // Save calibration
  virtual void LoadCalibrationSettings(istream &file);


  /*static void MRMLCallback(vtkObject *caller, unsigned long event,
                           void *clientData, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event,
                                 void *callData);*/

protected:
  vtkTRProstateBiopsyCalibrationStep();
  ~vtkTRProstateBiopsyCalibrationStep();

  
  void ShowLoadResetControls();
  void ClearLoadResetControls();

  void ShowLoadVolumeControls();
  void ClearLoadVolumeControls();

  void ShowFiducialSegmentParamsControls();
  void ClearFiducialSegmentParamsControls();

  void ShowFiducialSegmentationResultsControls();
  void ClearFiducialSegmentationResultsControls();

  void ShowSaveResegmentControls();
  void ClearSaveResegmentControls();

  void PopulateSegmentationResults();
  void PopulateRegistrationResults();

  void AddGUIObservers();
  void RemoveGUIObservers();
  void RecordClick(double rasPoint[3]);
  void SegmentRegisterMarkers();
  void SegmentAxis(int nAxis);

  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  vtkKWFrame *LoadVolumeDialogFrame;
  vtkKWLoadSaveButton *LoadCalibrationVolumeButton;

  // 1)  button: open file dialog box
  vtkKWLoadSaveButton *LoadCalibrationSettingsFileButton;
  // 2) button: save calib file dialog box
  vtkKWLoadSaveButton *SaveCalibrationSettingsFileButton;
  // also display save path??
  // 3) reset push button, this will require segmentation from start i.e.
  // new specification of 4 clicks on fiducials
  vtkKWPushButton *ResetCalibrationButton;
  // 4) Re-segment push button, this will not require new specification of 4 clicks, but this will
  // be used when Fiducials thresholds, initial angle, radius etc parameters are changed
  vtkKWPushButton *ResegmentButton;
  // also associated frames
  vtkKWFrame *LoadResetFrame;
  vtkKWFrame *SaveResegmentFrame;


  vtkKWFrameWithLabel *FiducialDimensionsFrame;
  vtkKWSpinBoxWithLabel *FiducialWidthSpinBox;
  vtkKWSpinBoxWithLabel *FiducialHeightSpinBox;
  vtkKWSpinBoxWithLabel *FiducialDepthSpinBox;

  vtkKWFrameWithLabel *FiducialThresholdFrame;
  vtkKWScaleWithEntry *FiducialThresholdScale[4];
  
  vtkKWFrameWithLabel *RadiusInitialAngleFrame;
  vtkKWSpinBoxWithLabel *RadiusSpinBox;
  vtkKWCheckButton *RadiusCheckButton;
  vtkKWSpinBoxWithLabel *InitialAngleSpinBox;

  vtkKWFrameWithLabel *SegmentationResultsFrame;
 
  // marker/fiducial centroids
  // information for the user
  vtkKWFrame *Marker_1_CentroidFrame;
  vtkKWLabel *Marker_1_CentroidLabel;
  vtkKWEntrySet      *Marker_1_Centroid; // read only

  vtkKWFrame *Marker_2_CentroidFrame;
  vtkKWLabel *Marker_2_CentroidLabel;
  vtkKWEntrySet      *Marker_2_Centroid; // read only

  vtkKWFrame *Marker_3_CentroidFrame;
  vtkKWLabel *Marker_3_CentroidLabel;
  vtkKWEntrySet      *Marker_3_Centroid; // read only

  vtkKWFrame *Marker_4_CentroidFrame;
  vtkKWLabel *Marker_4_CentroidLabel;
  vtkKWEntrySet      *Marker_4_Centroid; // read only

  // axis angle
  vtkKWEntryWithLabel *AxesAngle;

  // axis distance
  vtkKWEntryWithLabel *AxesDistance;


  int ObserverCount;
  unsigned int ClickNumber;
  bool ProcessingCallback;
  bool AllMarkersAcquired;
  
private:  
  vtkTRProstateBiopsyCalibrationStep(const vtkTRProstateBiopsyCalibrationStep&);
  void operator=(const vtkTRProstateBiopsyCalibrationStep&);
};

#endif
