/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtk4DUsEndoNavStepBase_h
#define __vtk4DUsEndoNavStepBase_h

#include "vtk4DUsEndoNavWin32Header.h"

#include "vtkObject.h"
#include "vtkKWObject.h"
#include "vtkKWWidget.h"
#include "vtkCommand.h"
#include "vtkKWWizardStep.h"
#include "vtkObserverManager.h"

class vtk4DUsEndoNavGUI;
class vtk4DUsEndoNavLogic;
class vtkMRMLScene;

class VTK_4DUsEndoNav_EXPORT vtk4DUsEndoNavStepBase : public vtkKWWizardStep
{
public:
  static vtk4DUsEndoNavStepBase *New();
  vtkTypeRevisionMacro(vtk4DUsEndoNavStepBase,vtkKWWizardStep);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Register(vtkObject *o) { Superclass::Register(o); };
  virtual void UnRegister(vtkObjectBase *o) { Superclass::UnRegister(o); };

  // Description: 
  // Get/Set GUI
  vtkGetObjectMacro(GUI, vtk4DUsEndoNavGUI);
  vtkGetObjectMacro(Logic, vtk4DUsEndoNavLogic);

  virtual void SetGUI(vtk4DUsEndoNavGUI*);
  virtual void SetLogic(vtk4DUsEndoNavLogic*);

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
  vtk4DUsEndoNavStepBase();
  ~vtk4DUsEndoNavStepBase();

  static void GUICallback(vtkObject *caller,
                          unsigned long eid, void *clientData, void *callData );

  static void MRMLCallback(vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData );
  
  double TitleBackgroundColor[3];

  int InGUICallbackFlag;
  int InMRMLCallbackFlag;

  vtk4DUsEndoNavGUI   *GUI;
  vtk4DUsEndoNavLogic *Logic;
  vtkMRMLScene        *MRMLScene;

  vtkCallbackCommand *GUICallbackCommand;
  vtkCallbackCommand *MRMLCallbackCommand;

  vtkObserverManager *MRMLObserverManager;


private:
  vtk4DUsEndoNavStepBase(const vtk4DUsEndoNavStepBase&);
  void operator=(const vtk4DUsEndoNavStepBase&);

};

#endif
