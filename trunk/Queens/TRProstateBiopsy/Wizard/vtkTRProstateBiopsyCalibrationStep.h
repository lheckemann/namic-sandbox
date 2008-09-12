#ifndef __vtkTRProstateBiopsyCalibrationStep_h
#define __vtkTRProstateBiopsyCalibrationStep_h

#include "vtkTRProstateBiopsyStep.h"

#include "vtkstd/vector"

class vtkKWPushButton;
class vtkKWSpinBoxWithLabel;
class vtkKWCheckButton;
class vtkKWScaleWithEntry;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWFrame;

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
  virtual void HideUserInterface();

  // Description:
  // The Enter and Exit functions are not part of vtkKWWizardStep.
  // They have been added here sot that they can be called by
  // ShowUserInterface and HideUserInterface.
  virtual void Enter();
  virtual void Exit();

  static void GUICallback(vtkObject *caller, unsigned long event,
                          void *clientData, void *callData);
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event,
                                void *callData);

  /*static void MRMLCallback(vtkObject *caller, unsigned long event,
                           void *clientData, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event,
                                 void *callData);*/

protected:
  vtkTRProstateBiopsyCalibrationStep();
  ~vtkTRProstateBiopsyCalibrationStep();

  vtkKWFrame *ButtonFrame;
  vtkKWLoadSaveButtonWithLabel *LoadCalibrationImageButton;
  vtkKWPushButton *ResetCalibrationButton;

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
  
private:
  vtkTRProstateBiopsyCalibrationStep(const vtkTRProstateBiopsyCalibrationStep&);
  void operator=(const vtkTRProstateBiopsyCalibrationStep&);
};

#endif
