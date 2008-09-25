#ifndef __vtkPerkStationCalibrateStep_h
#define __vtkPerkStationCalibrateStep_h

#include "vtkPerkStationStep.h"

class vtkKWFrame;
class vtkKWLabel;
class vtkKWText;
class vtkKWEntrySet;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWCheckButtonWithLabel;

class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationCalibrateStep : public vtkPerkStationStep
{
public:
  static vtkPerkStationCalibrateStep *New();
  vtkTypeRevisionMacro(vtkPerkStationCalibrateStep,vtkPerkStationStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();
  
  // Description:
  // Reimplement the superclass's method.
  // TO DO:
  //virtual void Validate();

  // Description:
  // Callbacks
  // TO DO:
  
  // Description:
  // Callbacks to capture keyboard events which will do translation/rotation depending on key pressed only in clinical mode
  virtual void ProcessKeyboardEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description:
  // Callbacks to capture mouse click on image
  virtual void ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description:
  // Callback on value entered in the Image scaling entry set
  virtual void ImageScalingEntryCallback(int widgetIndex, double value);
  
  // Description:
  // Callback on value entered in the Monitor physical size entry set
  virtual void MonitorPhysicalSizeEntryCallback(int widgetIndex, double value);
  
  // Description:
  // Callback on value entered in the Monitor physical size entry set
  virtual void MonitorPixelResolutionEntryCallback(int widgetIndex, double value);

  // Description:
  // Callback on value entered in the Image translation entry set
  virtual void ImageTranslationEntryCallback(int widgetIndex, double value);
  
  // Description:
  // Callback on value entered in the Image rotation angle entry
  virtual void ImageRotationEntryCallback(double value);
  
  // Description:
  // Callback on check button of vertical flip
  virtual void VerticalFlipCallback(int value);

  // Description:
  // Callback on check button of horizontal flip
  virtual void HorizontalFlipCallback(int value);

  // Description
  // Reset
  virtual void Reset();
protected:
  vtkPerkStationCalibrateStep();
  ~vtkPerkStationCalibrateStep(); 

  void ShowFlipComponents();
  void ShowScaleComponents();
  void ShowTranslateComponents();
  void ShowRotateComponents();

  void ClearFlipComponents();
  void ClearScaleComponents();
  void ClearTranslateComponents();
  void ClearRotateComponents();


  void InstallCallbacks();
  void PopulateControls();

  void EnableDisableControls();
  void EnableDisableFlipComponents(bool enable);
  void EnableDisableScaleComponents(bool enable);
  void EnableDisableTranslateComponents(bool enable);
  void EnableDisableRotateComponents(bool enable);

  void RecordClick(int xyPoint[2], double rasPoint[3], unsigned int & clickNum);

  void CalculateImageRotation(double & rotationAngle);

  void FlipImage();
  void ScaleImage();
  void TranslateImage();
  void RotateImage();
  
  void ResetControls();


  // TO DO: Question: other controls in two different modes (CLINICAL or TRANING)


  // for flip, the controls remain same for both modes
  // for monitor associated flip
  vtkKWFrameWithLabel *FlipFrame;
  // information from the user
  vtkKWCheckButtonWithLabel *VerticalFlipCheckButton;
  vtkKWCheckButtonWithLabel *HorizontalFlipCheckButton;
  bool ImageFlipDone;
  
  
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
  vtkKWEntrySet *Translation;
  
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

  bool ImageRotationDone;
  
private:
  int CurrentSubState;
  vtkPerkStationCalibrateStep(const vtkPerkStationCalibrateStep&);
  void operator=(const vtkPerkStationCalibrateStep&);
};

#endif
