#ifndef __vtkPerkStationCalibrateStep_h
#define __vtkPerkStationCalibrateStep_h

#include "vtkPerkStationStep.h"

class vtkKWFrame;
class vtkKWLabel;
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
  // Callbacks to capture mouse click on image
  virtual void ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description:
  // Callback on value entered in the Image scaling entry set
  virtual void ImageScalingEntryCallback(int widgetIndex, double value);
  
  // Description:
  // Callback on value entered in the Image translation entry set
  virtual void ImageTranslationEntryCallback(int widgetIndex, double value);
  
  // Description:
  // Callback on value entered in the Image rotation angle entry
  virtual void ImageRotationEntryCallback(double value);
  
  virtual void VerticalFlipCallback(int value);

  virtual void HorizontalFlipCallback(int value);

protected:
  vtkPerkStationCalibrateStep();
  ~vtkPerkStationCalibrateStep(); 

  void ShowFlipComponents();
  void ShowScaleComponents();
  void ShowTranslateComponents();
  void ShowRotateComponents();

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
  
  // for monitor associated flip
  vtkKWFrameWithLabel *FlipFrame;
  // information from the user
  vtkKWCheckButtonWithLabel *VerticalFlipCheckButton;
  vtkKWCheckButtonWithLabel *HorizontalFlipCheckButton;
  bool ImageFlipDone;

  
  
  // for scaling step

  // TO DO: Question: other controls in two different modes (CLINICAL or TRANING)
  // frame
  vtkKWFrameWithLabel *ScaleFrame;
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

  // TO DO: Question: other controls in two different modes (CLINICAL or TRANING)
  // frame
  vtkKWFrameWithLabel *TranslateFrame;  
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
  // information to/from the user  
  // center of rotation (COR) : could be input by user, or could be marked as 
  vtkKWFrame  *CORFrame;
  vtkKWLabel    *CORLabel;    
  vtkKWEntrySet *COR;
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
