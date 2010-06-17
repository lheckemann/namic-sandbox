#ifndef __vtkPerkStationCalibrateStep_h
#define __vtkPerkStationCalibrateStep_h

#include "vtkPerkStationStep.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include <string>

#include "vtkSmartPointer.h"

#include "vtkKWEntryWithLabel.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWPushButton.h"

class vtkKWCheckButtonWithLabel;
class vtkKWEntrySet;
class vtkKWFrame;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWText;



class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationCalibrateStep
: public vtkPerkStationStep
{
public:
  
  static vtkPerkStationCalibrateStep* New();
  vtkTypeRevisionMacro( vtkPerkStationCalibrateStep, vtkPerkStationStep );
  
  void PrintSelf( ostream& os, vtkIndent indent );

    // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void ShowUserInterface();
  
  virtual void ProcessGUIEvents( vtkObject *caller,
                                 unsigned long event,
                                 void *callData );

    // Callbacks to capture keyboard events which will do translation/rotation
    // depending on key pressed
  virtual void ProcessKeyboardEvents( vtkObject *caller,
                                      unsigned long event,
                                      void *callData );
  
  
    // Callbacks
  
  virtual void UpdateAutoScaleCallback();
  virtual void VerticalFlipCallback( bool value );
  virtual void HorizontalFlipCallback( bool value );
  
  virtual void Reset();
  void LoadCalibrationButtonCallback();
  void SaveCalibrationButtonCallback();

  virtual void SuggestFileName();

  virtual void Validate();
  
  
    // Hardware selection.
  
  void HardwareSelected( int index );
  
  void UpdateGUI();
  
  
protected:
  
  vtkPerkStationCalibrateStep();
  ~vtkPerkStationCalibrateStep(); 
  
  void ShowLoadResetControls();
  void ShowCalibrationList();
  void ShowHardwareCalibration();
  
  void AddGUIObservers();
  void RemoveGUIObservers();
  
    // GUI callback  
  static void WizardGUICallback( vtkObject *caller, unsigned long event,
                                 void *clientData, void *callData );
  
  void InstallCallbacks();
  void PopulateControls();
  void PopulateControlsOnLoadCalibration();

  void EnableDisableLoadResetControls( bool enable );
  void EnableDisableSaveControls( bool enable );
  void EnableDisableFlipComponents( bool enable );
  void EnableDisableScaleComponents( bool enable );
  
  
  // Widgets ------------------------------------------------------------------
  //BTX
  
    // Load, save, reset calibration.
  
  vtkSmartPointer< vtkKWFrame >          LoadResetFrame;
  vtkSmartPointer< vtkKWLoadSaveButton > LoadCalibrationFileButton;
  vtkSmartPointer< vtkKWLoadSaveButton > SaveCalibrationFileButton;
  vtkSmartPointer< vtkKWPushButton >     ResetCalibrationButton;
  

    // Table calibration.
  
  vtkSmartPointer< vtkKWFrameWithLabel > TableFrame;
  vtkSmartPointer< vtkKWLabel >          TableOverlayLabel;
  vtkSmartPointer< vtkKWEntry >          TableOverlayEntry;
  vtkSmartPointer< vtkKWPushButton >     TableUpdateButton;
  
  
    // Hardware selection.
  
  vtkSmartPointer< vtkKWFrameWithLabel >      HardwareFrame;
  vtkSmartPointer< vtkKWMenuButtonWithLabel > HardwareMenu;
  
  
    // Calibration list.
  
  vtkKWMultiColumnListWithScrollbars* CalibrationList;
  vtkKWPushButton* AddButton;
  vtkKWPushButton* DeleteButton;
  
  
    // Hardware calibration.
  
  vtkSmartPointer< vtkKWFrameWithLabel > HardwareCalibrationFrame;
  
  vtkSmartPointer< vtkKWFrame >                FlipFrame;
  vtkSmartPointer< vtkKWCheckButtonWithLabel > VerticalFlipCheckButton;
  vtkSmartPointer< vtkKWCheckButtonWithLabel > HorizontalFlipCheckButton;
  
  vtkSmartPointer< vtkKWFrame >    MonPhySizeFrame;
  vtkSmartPointer< vtkKWLabel >    MonPhySizeLabel;
  vtkSmartPointer< vtkKWEntrySet > MonPhySize; 
  
  vtkSmartPointer< vtkKWFrame >    MonPixResFrame;
  vtkSmartPointer< vtkKWLabel >    MonPixResLabel;  
  vtkSmartPointer< vtkKWEntrySet > MonPixRes; 
  
  vtkSmartPointer< vtkKWPushButton > HardwareUpdateButton;
  
  //ETX
  // --------------------------------------------------------------------------
  
  
    // GUI state.
  
  bool ProcessingCallback;
  int ObserverCount;
  unsigned int ClickNumber;
  int SecondMonitor;
  
  //BTX
  std::string CalibFileName;
  std::string CalibFilePath;
  //ETX
  
  
private:
  
  bool DoubleEqual( double val1, double val2 );
  vtkPerkStationCalibrateStep( const vtkPerkStationCalibrateStep& );
  void operator=( const vtkPerkStationCalibrateStep& );
};

#endif
