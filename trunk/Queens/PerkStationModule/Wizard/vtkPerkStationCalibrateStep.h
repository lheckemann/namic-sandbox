#ifndef __vtkPerkStationCalibrateStep_h
#define __vtkPerkStationCalibrateStep_h

#include "vtkPerkStationStep.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include <string>

#include "vtkSmartPointer.h"

#include "vtkKWEntryWithLabel.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"

class vtkKWCheckButtonWithLabel;
class vtkKWEntrySet;
class vtkKWFrame;
class vtkKWLoadSaveButton;
class vtkKWMenuButtonWithLabel;
class vtkKWPushButton;
class vtkKWText;



class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationCalibrateStep : public vtkPerkStationStep
{
public:
  static vtkPerkStationCalibrateStep *New();
  vtkTypeRevisionMacro(vtkPerkStationCalibrateStep,vtkPerkStationStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();
  
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  // Callbacks to capture keyboard events which will do translation/rotation depending on key pressed only in clinical mode
  virtual void ProcessKeyboardEvents(vtkObject *caller, unsigned long event, void *callData);
  
  // Callback on value entered in COR entry set
  virtual void COREntryCallback(int widgetIndex);

  // Callback on update button in scale frame in clinical mode
  virtual void UpdateAutoScaleCallback();
  
  // Callback on check button of vertical flip
  virtual void VerticalFlipCallback(bool value);

  // Callback on check button of horizontal flip
  virtual void HorizontalFlipCallback(bool value);
  
  
  // Reset
  virtual void Reset();

  // Callback on the load calibration button
  void LoadCalibrationButtonCallback();

  // Callback on the save calibration button
  void SaveCalibrationButtonCallback();

  // Save calibration
  virtual void SaveCalibration(ostream& of);

  virtual void LoadCalibration(istream &file);

  virtual void SuggestFileName();

  virtual void Validate();

protected:
  vtkPerkStationCalibrateStep();
  ~vtkPerkStationCalibrateStep(); 
  
  void ShowLoadResetControls();
  void ShowHardwareCalibration();
  void ShowFlipComponents();
  void ShowScaleComponents();
  void ShowTranslateComponents();
  void ShowRotateComponents();
  void ShowSaveControls();

  void ClearLoadResetControls();
  void ClearHardwareCalibration();
  void ClearSaveControls();
  void ClearFlipComponents();
  void ClearScaleComponents();
  void ClearTranslateComponents();
  void ClearRotateComponents();

  void AddGUIObservers();
  void RemoveGUIObservers();
  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);
  bool ProcessingCallback;
  void InstallCallbacks();
  void PopulateControls();
  void PopulateControlsOnLoadCalibration();

  void EnableDisableControls();
  void EnableDisableLoadResetControls(bool enable);
  void EnableDisableSaveControls(bool enable);
  void EnableDisableFlipComponents(bool enable);
  void EnableDisableScaleComponents(bool enable);
  void EnableDisableTranslateComponents(bool enable);
  void EnableDisableRotateComponents(bool enable);

  void RecordClick(int xyPoint[2], double rasPoint[3]);

  void CalculateImageRotation(double & rotationAngle);

  void ResetControls();

  
  vtkKWLoadSaveButton *LoadCalibrationFileButton;
  vtkKWLoadSaveButton *SaveCalibrationFileButton;
  vtkKWPushButton *ResetCalibrationButton;
  vtkKWFrame *LoadResetFrame;
  vtkKWFrame *SaveFrame;


    // Hardware calibration.
  
  vtkSmartPointer< vtkKWFrameWithLabel > TableFrame;
  vtkSmartPointer< vtkKWLabel >          TableScannerLabel;
  vtkSmartPointer< vtkKWEntrySet >       TableScannerEntry;
  vtkSmartPointer< vtkKWLabel >          TableOverlayLabel;
  vtkSmartPointer< vtkKWEntrySet >       TableOverlayEntry;
  
  vtkSmartPointer< vtkKWFrameWithLabel >      HardwareFrame;
  vtkSmartPointer< vtkKWMenuButtonWithLabel > HardwareMenu;

  
    // Flipping. To be deleted from the final software.
  
  vtkSmartPointer< vtkKWFrameWithLabel > FlipFrame;
  vtkSmartPointer< vtkKWCheckButtonWithLabel > VerticalFlipCheckButton;
  vtkSmartPointer< vtkKWCheckButtonWithLabel > HorizontalFlipCheckButton;
  
  
  // for scaling, 
  // frame
  vtkKWFrameWithLabel *ScaleFrame;

  // for clinical mode
  // no need to display/use the text entry of scale, 
  // just display the monitor's physical size/resolution & if they are changed, 
  // the image should be scaled new

  // for clinical mode, the scale frame still is common
  // we would have additionally, monitor's physical size text boxes
  // and monitor's resolution boxes
  // information for the user
  vtkKWFrame *MonPhySizeFrame;
  vtkKWLabel *MonPhySizeLabel;
  vtkKWEntrySet      *MonPhySize; 
  
  vtkKWFrame  *MonPixResFrame;
  vtkKWLabel    *MonPixResLabel;  
  vtkKWEntrySet      *MonPixRes; 

  // update button, so that any changes made to these two can be updated
  vtkKWPushButton *UpdateAutoScale;


  // training mode
  // for scaling step  
   // information for the user
  vtkKWFrame *ImgPixSizeFrame;
  vtkKWLabel *ImgPixSizeLabel;
  vtkKWEntrySet      *ImgSpacing; // read only
  
  vtkKWFrame  *MonPixSizeFrame;
  vtkKWLabel    *MonPixSizeLabel;  
  vtkKWEntrySet      *MonSpacing; // read only
  // information to be had from the user
  vtkKWFrame  *ImgScaleFrame;
  vtkKWLabel    *ImgScaleLabel;  
  vtkKWEntrySet      *ImgScaling;  
  
  bool ImageScalingDone; // for state management, enable/disable frames within
  
  // for translation step
  // frame
  vtkKWFrameWithLabel *TranslateFrame;  
  

  // Question: other controls in two different modes (CLINICAL or TRANING)
  // In clinical mode, the frame is shared
  // in addtion only a static text message will be there, of informing user on how to translate
  vtkKWText *TransMessage;

  // in training mode:

  // information to/from the user  
  // image fiducial click RAS location(will be populated on click)
  vtkKWFrame  *TransImgFidFrame;
  vtkKWLabel    *TransImgFidLabel;  
  vtkKWEntrySet *TransImgFid;
  // physical fiducial click RAS location(will be populated on click)
  vtkKWFrame  *TransPhyFidFrame;
  vtkKWLabel    *TransPhyFidLabel;  
  vtkKWEntrySet *TransPhyFid;
  // required translation ( to be keyed in by user in TRAINING mode)
  vtkKWFrame  *TransEntryFrame;
  vtkKWLabel    *TransEntryLabel;    
  // vtkKWEntrySet *Translation;
  
  bool ImageTranslationDone;

  // for rotation step

  // TO DO: Question: other controls in two different modes (CLINICAL or TRANING)
  // frame
  vtkKWFrameWithLabel *RotateFrame;  
  // center of rotation (COR) : could be input by user, or could be marked as 
  vtkKWFrame  *CORFrame;
  vtkKWLabel    *CORLabel;    
  vtkKWEntrySet *COR;

  // In clinical mode, the frame, COR is shared
  // in addtion only a static text message will be there, of informing user on how to rotate
  vtkKWText *RotMessage;


  // information to/from the user  
  
  // image fiducial click RAS location(will be populated on click)
  vtkKWFrame  *RotImgFidFrame;
  vtkKWLabel    *RotImgFidLabel;    
  vtkKWEntrySet *RotImgFid;
  // physical fiducial click RAS location(will be populated on click)
  vtkKWFrame  *RotPhyFidFrame;
  vtkKWLabel    *RotPhyFidLabel;      
  vtkKWEntrySet *RotPhyFid;
  // angle of rotation
  vtkKWEntryWithLabel *RotationAngle;
  bool CORSpecified;
  bool ImageRotationDone;
  

  bool TrainingModeControlsPopulated;
  bool ClinicalModeControlsPopulated;
  int ObserverCount;
  unsigned int ClickNumber;

  //BTX
  std::string CalibFileName;
  std::string CalibFilePath;
  //ETX

private:
  bool DoubleEqual(double val1, double val2);
  int CurrentSubState; // only used in 'training' mode
  vtkPerkStationCalibrateStep(const vtkPerkStationCalibrateStep&);
  void operator=(const vtkPerkStationCalibrateStep&);
  
  
  // Second monitor calibration data.

public:
  
  OverlayMonitor GetOverlayMonitor();
  void HardwareSelected( OverlayMonitor );
  
private:
  
  OverlayMonitor SecondMonitor;
  
private:
  
  double Translation[ 2 ];
  double Rotation;
  double RotationCenter[ 2 ];

};

#endif
