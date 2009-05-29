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
class vtkKWTextWithScrollbars;
class vtkKWEntrySet;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWLoadSaveButton;

class vtkSlicerInteractorStyle;
class vtkMRMLVolumeNode;
class vtkActor;

class vtkMRMLScalarVolumeNode;
class vtkMRMLTRProstateBiopsyModuleNode;

class vtkImageData;
class vtkVolumeMapper;
class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkVolumeProperty;
class vtkVolume;
class vtkMatrix4x4; 
class vtkKWRenderWidget;

#include "vtkSmartPointer.h"

//BTX
class CalibPointRenderer
  {
  public:
    CalibPointRenderer();
    virtual ~CalibPointRenderer();
    void Update(vtkKWRenderWidget* renderer, vtkMRMLVolumeNode *volumeNode, vtkImageData *imagedata);
    vtkVolume* GetVolume();
  protected:
    vtkImageData* Render_Image;
    vtkVolumeMapper* Render_Mapper;
    vtkVolumeProperty* Render_VolumeProperty;
    vtkVolume* Render_Volume;
    vtkKWRenderWidget* Renderer;
  };
//ETX

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyCalibrationStep :
  public vtkTRProstateBiopsyStep
{
public:
  static vtkTRProstateBiopsyCalibrationStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyCalibrationStep,
                       vtkTRProstateBiopsyStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  virtual void Validate();

  virtual void UpdateMRML();
  virtual void UpdateGUI();

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

  void SaveToExperimentFile(ostream & of);

  void LoadFromExperimentFile(istream &file);


  /*static void MRMLCallback(vtkObject *caller, unsigned long event,
                           void *clientData, void *callData);*/

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

  void ShowExportImportControls();
  void ClearExportImportControls();

  void PopulateCalibrationResults();

  void AddGUIObservers();
  void RemoveGUIObservers();
  void RecordClick(double rasPoint[3]);
  void SegmentRegisterMarkers();
  void SegmentAxis(int nAxis);
  void Resegment();

  void UpdateAxesIn3DView();
  void ShowAxesIn3DView(bool show);

  void ShowMarkerVolumesIn3DView(bool show);

  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  //BTX

  vtkSmartPointer<vtkKWFrame> LoadVolumeDialogFrame;
  vtkSmartPointer<vtkKWLoadSaveButton> LoadCalibrationVolumeButton;

  // 1)  button: open file dialog box
  vtkSmartPointer<vtkKWLoadSaveButton> LoadCalibrationSettingsFileButton;
  // 2) button: save calib file dialog box
  vtkSmartPointer<vtkKWLoadSaveButton> SaveCalibrationSettingsFileButton;
  // also display save path??
  // 3) reset push button, this will require segmentation from start i.e.
  // new specification of 4 clicks on fiducials
  vtkSmartPointer<vtkKWPushButton> ResetCalibrationButton;
  // 4) Re-segment push button, this will not require new specification of 4 clicks, but this will
  // be used when Fiducials thresholds, initial angle, radius etc parameters are changed
  vtkSmartPointer<vtkKWPushButton> ResegmentButton;
  // also associated frames
  vtkSmartPointer<vtkKWFrame> LoadResetFrame;
  vtkSmartPointer<vtkKWFrame> ExportImportFrame;

  vtkSmartPointer<vtkKWFrameWithLabel> FiducialPropertiesFrame;
  vtkSmartPointer<vtkKWSpinBoxWithLabel> FiducialWidthSpinBox;
  vtkSmartPointer<vtkKWSpinBoxWithLabel> FiducialHeightSpinBox;
  vtkSmartPointer<vtkKWSpinBoxWithLabel> FiducialDepthSpinBox;
  vtkSmartPointer<vtkKWScaleWithEntry> FiducialThresholdScale[4];
  vtkSmartPointer<vtkKWSpinBoxWithLabel> RadiusSpinBox;
  vtkSmartPointer<vtkKWCheckButton> RadiusCheckButton;
  vtkSmartPointer<vtkKWSpinBoxWithLabel> InitialAngleSpinBox;

  vtkSmartPointer<vtkKWFrameWithLabel> SegmentationResultsFrame;
  vtkSmartPointer<vtkKWTextWithScrollbars> CalibrationResultsBox;

  vtkSmartPointer<vtkActor> Axes1Actor;
  vtkSmartPointer<vtkActor> Axes2Actor;

  //ETX

  unsigned int ClickNumber;
  bool ProcessingCallback;
  bool AllMarkersAcquired;

  //BTX
  std::vector<CalibPointRenderer> CalibPointPreProcRendererList;
  //ETX

private:  
  vtkTRProstateBiopsyCalibrationStep(const vtkTRProstateBiopsyCalibrationStep&);
  void operator=(const vtkTRProstateBiopsyCalibrationStep&);
};

#endif
