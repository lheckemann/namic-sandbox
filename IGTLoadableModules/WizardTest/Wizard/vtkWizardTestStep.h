/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkWizardTestStep_h
#define __vtkWizardTestStep_h

#include "vtkKWWizardStep.h"
#include "vtkWizardTestWin32Header.h"
#include "vtkCommand.h"

#include "vtkObserverManager.h"

class vtkWizardTestGUI;
class vtkWizardTestLogic;
class vtkMRMLScene;

class VTK_WizardTest_EXPORT vtkWizardTestStep : public vtkKWWizardStep
{
 public:
  static vtkWizardTestStep *New();
  vtkTypeRevisionMacro(vtkWizardTestStep, vtkKWWizardStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Register(vtkObject *o) { Superclass::Register(o); };
  virtual void UnRegister(vtkObject *o) { Superclass::UnRegister(o); };

  // Description: 
  // Get/Set GUI
  vtkGetObjectMacro(GUI, vtkWizardTestGUI);
  vtkGetObjectMacro(Logic, vtkWizardTestLogic);

  virtual void SetGUI(vtkWizardTestGUI*);
  virtual void SetLogic(vtkWizardTestLogic*);
  virtual void TearDownGUI();

  vtkSetMacro(TotalSteps, int);
  vtkGetMacro(TotalSteps, int);
  vtkSetMacro(StepNumber, int);
  vtkGetMacro(StepNumber, int);

  void SetTitle(const char* title) {
    this->Title = title;
  }

  const char* GetTitle() {
    return this->Title.c_str();
  }

  void UpdateName();

  void SetInMRMLCallbackFlag (int flag) {
    this->InMRMLCallbackFlag = flag;
  }
  vtkGetMacro(InMRMLCallbackFlag, int);
  void SetInGUICallbackFlag (int flag) {
    this->InGUICallbackFlag = flag;
    }
  vtkGetMacro(InGUICallbackFlag, int);

  /*
  void SetAndObserveMRMLScene ( vtkMRMLScene *mrml )
    {
    vtkMRMLScene *oldValue = this->MRMLScene;
    this->MRMLObserverManager->SetAndObserveObject ( vtkObjectPointer( &this->MRMLScene), (vtkObject*)mrml );
    if ( oldValue != this->MRMLScene )
      {
      this->InvokeEvent (vtkCommand::ModifiedEvent);
      }
    }

  void SetAndObserveMRMLSceneEvents ( vtkMRMLScene *mrml, vtkIntArray *events )
    {
    vtkObject *oldValue = this->MRMLScene;
    this->MRMLObserverManager->SetAndObserveObjectEvents ( vtkObjectPointer( &this->MRMLScene), mrml, events );
    if ( oldValue != this->MRMLScene )
      {
      this->InvokeEvent (vtkCommand::ModifiedEvent);
      }
    }

  */
  void SetTitleBackgroundColor (double r, double g, double b) {
    this->TitleBackgroundColor[0] = r;
    this->TitleBackgroundColor[1] = g;
    this->TitleBackgroundColor[2] = b;
  };

  void GetTitleBackgroundColor (double* r, double* g, double* b) {
    *r = this->TitleBackgroundColor[0];
    *g = this->TitleBackgroundColor[1];
    *b = this->TitleBackgroundColor[2];
  };

  // Description:
  // Reimplement the superclass's method (see vtkKWWizardStep).
  virtual void HideUserInterface();
  virtual void Validate();
  virtual int CanGoToSelf();
  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData) {};
  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) {};

  virtual void UpdateGUI() {};

protected:
  vtkWizardTestStep();
  ~vtkWizardTestStep();

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );

  static void MRMLCallback(vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData );

protected:
  
  double TitleBackgroundColor[3];

  int InGUICallbackFlag;
  int InMRMLCallbackFlag;

  vtkWizardTestGUI   *GUI;
  vtkWizardTestLogic *Logic;
  vtkMRMLScene        *MRMLScene;

  vtkCallbackCommand *GUICallbackCommand;
  vtkCallbackCommand *MRMLCallbackCommand;
  vtkObserverManager *MRMLObserverManager;

  //BTX
  std::string Title;
  //ETX
  
  int TotalSteps;     // Total number of steps in the wizard
  int StepNumber;     // Step number for this step.

private:
  vtkWizardTestStep(const vtkWizardTestStep&);
  void operator=(const vtkWizardTestStep&);

};

#endif
