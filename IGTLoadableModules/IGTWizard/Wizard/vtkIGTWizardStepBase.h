/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkIGTWizardStepBase_h
#define __vtkIGTWizardStepBase_h

#include "vtkIGTWizardWin32Header.h"

#include "vtkObject.h"
#include "vtkKWObject.h"
#include "vtkKWWidget.h"
#include "vtkCommand.h"
#include "vtkKWWizardStep.h"
#include "vtkObserverManager.h"

class vtkIGTWizardGUI;
class vtkIGTWizardLogic;
class vtkMRMLScene;

class VTK_IGTWIZARD_EXPORT vtkIGTWizardStepBase : public vtkKWWizardStep
{
public:
  static vtkIGTWizardStepBase *New();
  vtkTypeRevisionMacro(vtkIGTWizardStepBase,vtkKWWizardStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Register(vtkObject *o) { Superclass::Register(o); };
  virtual void UnRegister(vtkObjectBase *o) { Superclass::UnRegister(o); };

  // Description: 
  // Get/Set GUI
  vtkGetObjectMacro(GUI, vtkIGTWizardGUI);
  vtkGetObjectMacro(Logic, vtkIGTWizardLogic);

  virtual void SetGUI(vtkIGTWizardGUI*);
  virtual void SetLogic(vtkIGTWizardLogic*);

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
  vtkIGTWizardStepBase();
  ~vtkIGTWizardStepBase();

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );

  static void MRMLCallback(vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData );
  
  double TitleBackgroundColor[3];

  int InGUICallbackFlag;
  int InMRMLCallbackFlag;

  vtkIGTWizardGUI   *GUI;
  vtkIGTWizardLogic *Logic;
  vtkMRMLScene        *MRMLScene;

  vtkCallbackCommand *GUICallbackCommand;
  vtkCallbackCommand *MRMLCallbackCommand;

  vtkObserverManager *MRMLObserverManager;


private:
  vtkIGTWizardStepBase(const vtkIGTWizardStepBase&);
  void operator=(const vtkIGTWizardStepBase&);

};

#endif
