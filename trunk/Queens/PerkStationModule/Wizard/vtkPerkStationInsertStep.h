#ifndef __vtkPerkStationInsertStep_h
#define __vtkPerkStationInsertStep_h

#include "vtkPerkStationStep.h"

class vtkKWFrame;
class vtkKWLabel;
class vtkKWEntrySet;
class vtkKWCheckButtonWithLabel;

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
  // Callbacks.
  // TO DO:
  /*virtual void NormalizationTargetSelectionChangedCallback(
    vtkIdType VolId);
  virtual void NormalizationEnableCallback(
    vtkIdType VolId, int state);
  virtual void NormalizationNormTypeCallback(
    vtkIdType VolId, int enumType);
  virtual void NormalizationPrintInfoCallback(
    vtkIdType VolId, int checked);
  virtual void NormalizationNormValueCallback(
    vtkIdType VolId, double dValue);
  virtual void NormalizationSmoothingWidthCallback(
    vtkIdType VolId, int iValue);
  virtual void NormalizationMaxSmoothingWidthCallback(
    vtkIdType VolId, int iValue);
  virtual void NormalizationRelativeMaxVoxelNumCallback(
    vtkIdType VolId, double dValue);*/

  void Reset();
protected:
  vtkPerkStationInsertStep();
  ~vtkPerkStationInsertStep();

   // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void PopulateControls();
  virtual void InstallCallbacks();
  void ResetControls();
/*  virtual void PopulateNormalizationTargetVolumeSelector();
  virtual void ResetDefaultParameters(vtkIdType target_vol_id);*/

  vtkKWCheckButtonWithLabel *ConnectTrackerCheckButton;
  vtkKWFrame *NeedleTipPositionFrame;
  vtkKWLabel *NeedleTipPositionLabel;
  vtkKWEntrySet       *NeedleTipPosition;
  

private:
  vtkPerkStationInsertStep(const vtkPerkStationInsertStep&);
  void operator=(const vtkPerkStationInsertStep&);
};

#endif
