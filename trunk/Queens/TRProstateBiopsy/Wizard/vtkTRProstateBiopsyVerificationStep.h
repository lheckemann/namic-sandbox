#ifndef __vtkTRProstateBiopsyVerificationStep_h
#define __vtkTRProstateBiopsyVerificationStep_h

#include "vtkTRProstateBiopsyStep.h"

class vtkKWFrame;
class vtkKWEntry;
class vtkKWLabel;
class vtkKWEntryWithLabel;
class vtkKWEntrySet;
class vtkKWLoadSaveButton;
class vtkKWText;
class vtkKWPushButton;
class vtkKWMenuButton;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnList;
class vtkKWMultiColumnListWithScrollbars;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyVerificationStep : public vtkTRProstateBiopsyStep
{
public:
  static vtkTRProstateBiopsyVerificationStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyVerificationStep,vtkTRProstateBiopsyStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  

  
  // Description:
  // Callbacks to capture mouse click on image
  virtual void ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description
  // Reset
  virtual void Reset();


  void SaveToExperimentFile(ostream & of);

  void LoadFromExperimentFile(istream &file);


protected:
  vtkTRProstateBiopsyVerificationStep();
  ~vtkTRProstateBiopsyVerificationStep();

  void ShowLoadVolumeControls();
  void ClearLoadVolumeControls();

  void ShowNeedleTypeAndTargetsListControls();
  void ClearNeedleTypeAndTargetsListControls();

  void ShowMessageAndErrorControls();
  void ClearMessageAndErrorControls();

  void AddGUIObservers();
  void RemoveGUIObservers();
  void RecordClick(double rasPoint[3]);


  void PopulateListWithTargets();
  unsigned int PopulateListWithTargetDetails(unsigned int targetDescIndex);
  void UpdateListWithErrorDetails(unsigned int rowIndex, double ovrErr, double isErr, double apErr, double lrErr);

   // Description
  // Callback on the load targeting volume button
  void LoadVerificationVolumeButtonCallback(const char *fileName); 
  
  void ComputeTargetErrorCallback();
  void TargetSelectedFromListCallback();
  void TargetDeselectedFromListCallback();
  // display information in the message box
  void PopulateMessageBoxWithTargetInfo();

    // bring target to view in all three views
  void BringTargetToViewIn2DViews();

  // change target color
  void SetTargetFiducialColor(bool selected);

  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);


  // load targeting volume controls
  vtkKWFrame *LoadVolumeDialogFrame;
  vtkKWLoadSaveButton *LoadVerificationVolumeButton;
  

  vtkKWFrame *ListFrame;
   // needle type list
  vtkKWMenuButtonWithLabel *NeedleTypeMenuList;
  // multi-column list to display target, params, etc
  vtkKWMultiColumnListWithScrollbars *TargetsMultiColumnList;

  //BTX
  // Description:
  // The column orders in the list box
  enum
    {
    TargetNumberColumn = 0,
    NeedleTypeColumn = 1,
    RASLocationColumn = 2,
    ReachableColumn = 3,
    OverallErrorColumn = 4,
    ISErrorColumn = 5,
    APErrorColumn = 6,
    LRErrorColumn = 7,
    NumberOfColumns = 8,
    };
  //ETX

  // compute error button
  vtkKWFrame *MsgAndErrorFrame;
  vtkKWText *Message;
  vtkKWPushButton *ComputeErrorButton;

  double NeedleClick1[3];
  double NeedleClick2[3];
  bool ProcessingCallback;
  unsigned int ClickNumber;
  bool AcquireClicks;

  int LastSelectedTargetDescriptorIndex;
  int CurrentSelectedTargetDescriptorIndex;


private:
  vtkTRProstateBiopsyVerificationStep(const vtkTRProstateBiopsyVerificationStep&);
  void operator=(const vtkTRProstateBiopsyVerificationStep&);
};

#endif
