#ifndef __vtkTRProstateBiopsyTargetingStep_h
#define __vtkTRProstateBiopsyTargetingStep_h

#include "vtkTRProstateBiopsyStep.h"

#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWFrame.h"


class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyTargetingStep : public vtkTRProstateBiopsyStep
{
public:
  static vtkTRProstateBiopsyTargetingStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyTargetingStep,vtkTRProstateBiopsyStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  

protected:
  vtkTRProstateBiopsyTargetingStep();
  ~vtkTRProstateBiopsyTargetingStep();

  vtkKWFrame *SelectImageFrame;
  vtkKWLoadSaveButtonWithLabel *SelectImageButton;

private:
  vtkTRProstateBiopsyTargetingStep(const vtkTRProstateBiopsyTargetingStep&);
  void operator=(const vtkTRProstateBiopsyTargetingStep&);
};

#endif
