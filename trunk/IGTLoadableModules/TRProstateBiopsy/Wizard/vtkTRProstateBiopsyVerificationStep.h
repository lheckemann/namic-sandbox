#ifndef __vtkTRProstateBiopsyVerificationStep_h
#define __vtkTRProstateBiopsyVerificationStep_h

#include "vtkTRProstateBiopsyStep.h"

#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWFrame.h"


class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyVerificationStep : public vtkTRProstateBiopsyStep
{
public:
  static vtkTRProstateBiopsyVerificationStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyVerificationStep,vtkTRProstateBiopsyStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  

protected:
  vtkTRProstateBiopsyVerificationStep();
  ~vtkTRProstateBiopsyVerificationStep();

  vtkKWFrame *SelectImageFrame;
  vtkKWLoadSaveButtonWithLabel *SelectImageButton;

private:
  vtkTRProstateBiopsyVerificationStep(const vtkTRProstateBiopsyVerificationStep&);
  void operator=(const vtkTRProstateBiopsyVerificationStep&);
};

#endif
