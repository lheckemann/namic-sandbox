#ifndef __vtkTRProstateBiopsySegmentationStep_h
#define __vtkTRProstateBiopsySegmentationStep_h

#include "vtkTRProstateBiopsyStep.h"

#include "vtkstd/vector"

class vtkKWPushButton;
class vtkKWScaleWithEntry;
class vtkKWFrame;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWLoadSaveButton;
class vtkSlicerNodeSelectorWidget;

class vtkSlicerInteractorStyle;
class vtkMRMLVolumeNode;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsySegmentationStep :
  public vtkTRProstateBiopsyStep
{
public:
  static vtkTRProstateBiopsySegmentationStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsySegmentationStep,
                       vtkTRProstateBiopsyStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();

  // Description:
  // The Enter and Exit functions are not part of vtkKWWizardStep.
  // They have been added here sot that they can be called by
  // ShowUserInterface and HideUserInterface.
  virtual void Enter();
  virtual void Exit();


  // Description:
  // Process GUI events
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event,
                                void *callData);

   // Description:
  // Callbacks to capture mouse click on image
  virtual void ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData);

  // Description
  // Reset
  virtual void Reset();

  // Description
  // Segment using Yi/Allen's random walk method
  void SegmentYiRandomWalkMethod();

  // Description
  // Callback on the input volume button
  void InputVolumeButtonCallback(const char *fileName);

  // Description
  // Callback on the input volume button
  void SeedVolumeButtonCallback(const char *fileName);
  
  /*static void MRMLCallback(vtkObject *caller, unsigned long event,
                           void *clientData, void *callData);
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event,
                                 void *callData);*/

protected:
  vtkTRProstateBiopsySegmentationStep();
  ~vtkTRProstateBiopsySegmentationStep();

  
  void ShowVolumeControls();
  void ClearVolumeControls();
 
  void ShowParametersControls();
  void ClearParametersControls();

  void ShowSegmentControls();
  void ClearSegmentControls();

  void AddGUIObservers();
  void RemoveGUIObservers();
  void RecordClick(double rasPoint[3]);

  // Description:
  // GUI callback  
  static void WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  vtkKWFrame *InputVolumeDialogFrame;
  vtkKWLoadSaveButton *InputVolumeButton;
  // standard: for volume selection
  vtkSlicerNodeSelectorWidget *InputVolumeSelector;

  vtkKWFrame *SeedVolumeDialogFrame;
  vtkKWLoadSaveButton *SeedVolumeButton;
  // standard: for volume selection
  vtkSlicerNodeSelectorWidget *SeedVolumeSelector;

  vtkKWFrame *OutputVolumeDialogFrame;
  // standard: for volume selection
  vtkSlicerNodeSelectorWidget *OutputVolumeSelector;

  vtkKWFrameWithLabel *ParametersFrame;
  vtkKWScaleWithEntry *ParameterBetaScale;

  // segment frame
  vtkKWFrame *SegmentFrame;
  // segment push button
  vtkKWPushButton *SegmentButton;

  int ObserverCount;
  unsigned int ClickNumber;
  bool ProcessingCallback;
  
  char *InputVolFileName;
  char *SeedVolFileName;
  char *OutputVolFileName;

  
private:  
  vtkTRProstateBiopsySegmentationStep(const vtkTRProstateBiopsySegmentationStep&);
  void operator=(const vtkTRProstateBiopsySegmentationStep&);
};

#endif
