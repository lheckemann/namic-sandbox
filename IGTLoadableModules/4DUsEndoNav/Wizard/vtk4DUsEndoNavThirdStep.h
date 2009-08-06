/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtk4DUsEndoNavThirdStep_h
#define __vtk4DUsEndoNavThirdStep_h

#include "vtk4DUsEndoNavStepBase.h"

class vtkKWLoadSaveButtonWithLabel;
class vtkKWFrame;
class vtkKWEntry;
class vtkKWCheckButton;
class vtkKWPushButton;
class vtkKWLabel;

class VTK_4DUsEndoNav_EXPORT vtk4DUsEndoNavThirdStep :
  public vtk4DUsEndoNavStepBase
{
public:
  static vtk4DUsEndoNavThirdStep *New();
  vtkTypeRevisionMacro(vtk4DUsEndoNavThirdStep,vtk4DUsEndoNavStepBase);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  

protected:
  vtk4DUsEndoNavThirdStep();
  ~vtk4DUsEndoNavThirdStep();

  // GUI Widgets
  vtkKWFrame *RobotFrame;
  vtkKWLabel *RobotLabel1;
  vtkKWLabel *RobotLabel2;
  vtkKWEntry *RobotAddressEntry;
  vtkKWEntry *RobotPortEntry;
  vtkKWPushButton *RobotConnectButton;
  
  vtkKWFrame *ScannerFrame;
  vtkKWLabel *ScannerLabel1;
  vtkKWLabel *ScannerLabel2;
  vtkKWEntry *ScannerAddressEntry;
  vtkKWEntry *ScannerPortEntry;
  vtkKWPushButton *ScannerConnectButton;
  
private:
  vtk4DUsEndoNavThirdStep(const vtk4DUsEndoNavThirdStep&);
  void operator=(const vtk4DUsEndoNavThirdStep&);
};

#endif
