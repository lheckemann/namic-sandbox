#ifndef __vtkWFTestWizard_h
#define __vtkWFTestWizard_h

#include "vtkKWWizardDialog.h"

class vtkKWWizardStep;
class vtkKWRadioButtonSet;
class vtkCallbackCommand;
//class vtkKWEntry;
//class vtkKWLabel;
//class vtkKWSpinBox;
//class vtkKWStateMachineInput;

class vtkWFTestWizard : public vtkKWWizardDialog
{
public:
    static vtkWFTestWizard *New();
    
    vtkTypeRevisionMacro(vtkWFTestWizard,vtkKWWizardDialog);
  
    virtual void initializeWizard();
    
    // Description:
    // Operator step callbacks
    // Show the UI, validate the UI.
    virtual void Show1UserInterfaceCallback();
    virtual void Validate1Callback();
    
    virtual void Show2UserInterfaceCallback();
    virtual void Validate2Callback();
    
    virtual void Show3UserInterfaceCallback();
    virtual void Validate3Callback();
    
    virtual void Show4UserInterfaceCallback();
    virtual void Validate4Callback();
    
    virtual void ShowLastUserInterfaceCallback();
    virtual void ValidateLastCallback();
    
    // Description:
    // Update the "enable" state of the object and its internal parts.
    // Depending on different Ivars (this->Enabled, the application's 
    // Limited Edition Mode, etc.), the "enable" state of the object is updated
    // and propagated to its internal parts/subwidgets. This will, for example,
    // enable/disable parts of the widget UI, enable/disable the visibility
    // of 3D widgets, etc.
    virtual void UpdateEnableState();

    // Description:
    // Callbacks
    virtual int EntryChangedCallback(const char *value);
    
    
protected:
    vtkWFTestWizard();
    ~vtkWFTestWizard();
  
    // Description:
    // Create the widget.
    virtual void CreateWidget();
    
    //BTX
    enum
      {
       patientNew = 0,
       patientLoad
      };
      //ETX
    
    // Description:
    // Operator step
    vtkKWWizardStep *m_FirstStep;
    vtkKWWizardStep *m_SecondStep;
    vtkKWWizardStep *m_ThirdStep;
    vtkKWWizardStep *m_FourthStep;
    vtkKWWizardStep *m_LastStep;
  
    vtkKWRadioButtonSet *m_patientSelectorRBS;
    
    int getPatientSelector();
    
    static void patientSelectorCallback(vtkObject* obj, unsigned long,void*, void*);  
private:
    vtkWFTestWizard(const vtkWFTestWizard&);   // Not implemented.
    void operator=(const vtkWFTestWizard&);  // Not implemented.
};

//static bool WFTestWizard::classCreated = false;
#endif /*__vtkWFTestWizard_h*/
