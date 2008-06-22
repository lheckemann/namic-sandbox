#ifndef __vtkTRProstateBiopsyCalibrationStep_h
#define __vtkTRProstateBiopsyCalibrationStep_h

#include "vtkTRProstateBiopsyStep.h"

#include "vtkstd/vector"

class vtkKWPushButton;
class vtkKWSpinBoxWithLabel;
class vtkKWScaleWithEntry;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWFrame;

class vtkSlicerInteractorStyle;
class vtkSlicerVolumesLogic;
class vtkMRMLVolumeNode;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyCalibrationStep : public vtkTRProstateBiopsyStep
{
public:
  static vtkTRProstateBiopsyCalibrationStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyCalibrationStep,vtkTRProstateBiopsyStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event,
                  void *callData);  
  virtual vtkMRMLVolumeNode *AddVolumeNode(vtkSlicerVolumesLogic *volLogic,
                  const char *volumeNodeName);

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
  vtkKWScaleWithEntry *FiducialThresholdScale1;
  vtkKWScaleWithEntry *FiducialThresholdScale2;
  vtkKWScaleWithEntry *FiducialThresholdScale3;
  vtkKWScaleWithEntry *FiducialThresholdScale4;
  
  vtkKWFrameWithLabel *FiducialRadiusFrame;
  vtkKWSpinBoxWithLabel *FiducialRadiusSpinBox;

  vtkKWSpinBoxWithLabel *InitialAngleSpinBox;
  
private:
  vtkTRProstateBiopsyCalibrationStep(const vtkTRProstateBiopsyCalibrationStep&);
  void operator=(const vtkTRProstateBiopsyCalibrationStep&);
};

#endif
