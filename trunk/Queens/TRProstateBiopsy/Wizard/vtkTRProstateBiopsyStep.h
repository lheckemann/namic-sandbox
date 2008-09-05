#ifndef __vtkTRProstateBiopsyStep_h
#define __vtkTRProstateBiopsyStep_h

#include "vtkKWWizardStep.h"
#include "vtkTRProstateBiopsy.h"
#include "vtkCommand.h"

#include "vtkObserverManager.h"

class vtkTRProstateBiopsyGUI;
class vtkTRProstateBiopsyLogic;
class vtkMRMLScene;


class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyStep : public vtkKWWizardStep
{
public:
  static vtkTRProstateBiopsyStep *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyStep,vtkKWWizardStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // Get/Set GUI
  vtkGetObjectMacro(GUI, vtkTRProstateBiopsyGUI);
  vtkGetObjectMacro(Logic, vtkTRProstateBiopsyLogic);
  virtual void SetGUI(vtkTRProstateBiopsyGUI*);
  virtual void SetLogic(vtkTRProstateBiopsyLogic*);

  void SetInMRMLCallbackFlag (int flag) {
    this->InMRMLCallbackFlag = flag;
  }
  vtkGetMacro(InMRMLCallbackFlag, int);
  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);

  void SetAndObserveMRMLScene ( vtkMRMLScene *mrml )
    {
    vtkMRMLScene *oldValue = this->MRMLScene;
    this->MRMLObserverManager->SetAndObserveObject ( vtkObjectPointer( &this->MRMLScene), (vtkObject*)mrml );
    if ( oldValue != this->MRMLScene )
      {
      this->InvokeEvent (vtkCommand::ModifiedEvent);
      }
    }

  void SetTitleBackgroundColor (double r, double g, double b) {
    this->TitleBackgroundColor[0] = r;
    this->TitleBackgroundColor[1] = g;
    this->TitleBackgroundColor[2] = b;
  };

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void HideUserInterface();
  virtual void Validate();
  virtual int CanGoToSelf();
  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData) {};
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};


protected:
  vtkTRProstateBiopsyStep();
  ~vtkTRProstateBiopsyStep();

  double TitleBackgroundColor[3];

  int InGUICallbackFlag;
  int InMRMLCallbackFlag;
  vtkCallbackCommand *GUICallbackCommand;
  vtkCallbackCommand *MRMLCallbackCommand;

  vtkTRProstateBiopsyGUI   *GUI;
  vtkTRProstateBiopsyLogic *Logic;
  vtkMRMLScene        *MRMLScene;

  vtkObserverManager *MRMLObserverManager;
  


private:
  vtkTRProstateBiopsyStep(const vtkTRProstateBiopsyStep&);
  void operator=(const vtkTRProstateBiopsyStep&);

};

#endif
