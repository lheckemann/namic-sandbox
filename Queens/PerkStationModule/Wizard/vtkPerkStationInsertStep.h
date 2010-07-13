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


class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationInsertStep
: public vtkPerkStationStep
{
public:
  static vtkPerkStationInsertStep *New();
  vtkTypeRevisionMacro( vtkPerkStationInsertStep,vtkPerkStationStep );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  
  void OnMultiColumnListSelectionChanged();
  void OnCalibrationSelectionChanged();
  
  
  virtual void ShowUserInterface();
  virtual void HideUserInterface();
  

  virtual void ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData );
  
  void UpdateGUI();
  
  
protected:
  vtkPerkStationInsertStep();
  ~vtkPerkStationInsertStep();

    // virtual void PopulateIntensityImagesTargetVolumeSelector();
  virtual void InstallCallbacks();
  
  void AddGUIObservers();
  void RemoveGUIObservers();

  virtual void Validate();
  
  
  
  // Description:
  // GUI callback  
  static void WizardGUICallback( vtkObject *caller, unsigned long event, void *clientData, void *callData );
  
  
    // GUI elements
  
  vtkKWLabel* AngleInPlaneLabel;
  
  
    // Plan list.
  
  vtkKWFrame* PlanListFrame;
  vtkKWMultiColumnListWithScrollbars* PlanList;
    
    
    // Calibration list.
  
  vtkKWFrame* CalibrationListFrame;
  vtkKWMultiColumnListWithScrollbars* CalibrationList;
  
  
private:    
  bool ProcessingCallback;
  vtkPerkStationInsertStep( const vtkPerkStationInsertStep& );
  void operator=( const vtkPerkStationInsertStep& );
};

#endif
