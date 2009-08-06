/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkFourDUsEndoNavStepBase_h
#define __vtkFourDUsEndoNavStepBase_h

#include "vtkFourDUsEndoNavWin32Header.h"

#include "vtkObject.h"
#include "vtkKWObject.h"
#include "vtkKWWidget.h"
#include "vtkCommand.h"
#include "vtkKWWizardStep.h"
#include "vtkObserverManager.h"

class vtkFourDUsEndoNavGUI;
class vtkFourDUsEndoNavLogic;
class vtkMRMLScene;

class VTK_FourDUsEndoNav_EXPORT vtkFourDUsEndoNavStepBase : public vtkKWWizardStep
{
public:
  static vtkFourDUsEndoNavStepBase *New();
  vtkTypeRevisionMacro(vtkFourDUsEndoNavStepBase,vtkKWWizardStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Register(vtkObject *o) { Superclass::Register(o); };
  virtual void UnRegister(vtkObjectBase *o) { Superclass::UnRegister(o); };

  // Description: 
  // Get/Set GUI
  vtkGetObjectMacro(GUI, vtkFourDUsEndoNavGUI);
  vtkGetObjectMacro(Logic, vtkFourDUsEndoNavLogic);

  virtual void SetGUI(vtkFourDUsEndoNavGUI*);
  virtual void SetLogic(vtkFourDUsEndoNavLogic*);

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
  vtkFourDUsEndoNavStepBase();
  ~vtkFourDUsEndoNavStepBase();

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );

  static void MRMLCallback(vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData );
  
  double TitleBackgroundColor[3];

  int InGUICallbackFlag;
  int InMRMLCallbackFlag;

  vtkFourDUsEndoNavGUI   *GUI;
  vtkFourDUsEndoNavLogic *Logic;
  vtkMRMLScene        *MRMLScene;

  vtkCallbackCommand *GUICallbackCommand;
  vtkCallbackCommand *MRMLCallbackCommand;

  vtkObserverManager *MRMLObserverManager;


private:
  vtkFourDUsEndoNavStepBase(const vtkFourDUsEndoNavStepBase&);
  void operator=(const vtkFourDUsEndoNavStepBase&);

};

#endif
