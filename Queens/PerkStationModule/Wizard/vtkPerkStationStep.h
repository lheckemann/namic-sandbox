#ifndef __vtkPerkStationStep_h
#define __vtkPerkStationStep_h

#include "vtkPerkStationModule.h"
#include "vtkKWWizardStep.h"

class vtkPerkStationModuleGUI;
class vtkKWMenu;

//#define EMSEG_MENU_BUTTON_WIDTH 15
//#define EMSEG_WIDGETS_LABEL_WIDTH 25

class VTK_PERKSTATIONMODULE_EXPORT vtkPerkStationStep : public vtkKWWizardStep
{
public:
  static vtkPerkStationStep *New();
  vtkTypeRevisionMacro(vtkPerkStationStep,vtkKWWizardStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // Get/Set GUI
  vtkGetObjectMacro(GUI, vtkPerkStationModuleGUI);
  virtual void SetGUI(vtkPerkStationModuleGUI*);

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void HideUserInterface();
  virtual void Validate();
  virtual int CanGoToSelf();

  void AddGUIObservers() { }
  virtual void RemoveGUIObservers() { }
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData) { }

protected:
  vtkPerkStationStep();
  ~vtkPerkStationStep();

  vtkPerkStationModuleGUI *GUI;
  vtkCallbackCommand *WizardGUICallbackCommand;
private:
  vtkPerkStationStep(const vtkPerkStationStep&);
  void operator=(const vtkPerkStationStep&);
};

#endif
