#ifndef __vtkPerkStationCalibrateStep_h
#define __vtkPerkStationCalibrateStep_h

#include "vtkPerkStationStep.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include <string>

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


/**
 * GUI panel for calibration step.
 */
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
  virtual void HideUserInterface();
  
  
  virtual void ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData );

    // Callbacks to capture keyboard events which will do translation/rotation
    // depending on key pressed
  virtual void ProcessKeyboardEvents( vtkObject *caller, unsigned long event, void *callData );
  
  
    // Callbacks
  
  virtual void UpdateAutoScaleCallback();
  virtual void VerticalFlipCallback( bool value );
  virtual void HorizontalFlipCallback( bool value );
  
  virtual void Reset();
  
  virtual void SuggestFileName();

  virtual void Validate();
  
  void OnMultiColumnListUpdate(int row, int col, char * str);
  void OnMultiColumnListSelectionChanged();
  
  
    // Hardware selection.
  
  void HardwareSelected( int index );
  
  void UpdateGUI();
  
  
protected:
  
  vtkPerkStationCalibrateStep();
  ~vtkPerkStationCalibrateStep(); 
  
  void ShowCalibrationList();
  void ShowHardwareCalibration();
  
  void AddGUIObservers();
  void RemoveGUIObservers();
  
    // GUI callback  
  static void WizardGUICallback( vtkObject *caller, unsigned long event,
                                 void *clientData, void *callData );
  
  void InstallCallbacks();
  
  void EnableDisableFlipComponents( bool enable );
  void EnableDisableScaleComponents( bool enable );
  
  
  // Widgets ------------------------------------------------------------------
  //BTX
  
    // Table calibration.
  
  vtkKWFrameWithLabel* TableFrame;
  vtkKWLabel*          TableOverlayLabel;
  vtkKWEntry*          TableOverlayEntry;
  vtkKWPushButton*     TableUpdateButton;
  
  
    // Hardware selection.
  
  vtkKWFrameWithLabel*      HardwareFrame;
  vtkKWMenuButtonWithLabel* HardwareMenu;
  
  
    // Calibration list.
  
  vtkKWFrame*        ListButtonsFrame;
  vtkKWFrame*        CalibrationListFrame;
  vtkKWMultiColumnListWithScrollbars* CalibrationList;
  vtkKWPushButton*   AddButton;
  vtkKWPushButton*   DeleteButton;
  unsigned int       CalibrationUID;
  
  
    // Hardware calibration.
  
  vtkKWFrameWithLabel* HardwareCalibrationFrame;
  
  vtkKWFrame*                FlipFrame;
  vtkKWCheckButtonWithLabel* VerticalFlipCheckButton;
  vtkKWCheckButtonWithLabel* HorizontalFlipCheckButton;
  
  vtkKWFrame*    MonPhySizeFrame;
  vtkKWLabel*    MonPhySizeLabel;
  vtkKWEntrySet* MonPhySize; 
  
  vtkKWFrame*    MonPixResFrame;
  vtkKWLabel*    MonPixResLabel;  
  vtkKWEntrySet* MonPixRes; 
  
  vtkKWPushButton* HardwareUpdateButton;
  
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
