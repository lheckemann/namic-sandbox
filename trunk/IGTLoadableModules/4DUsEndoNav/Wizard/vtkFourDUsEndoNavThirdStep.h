/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkFourDUsEndoNavThirdStep_h
#define __vtkFourDUsEndoNavThirdStep_h

#include "vtkFourDUsEndoNavStepBase.h"

class vtkKWLoadSaveButtonWithLabel;
class vtkKWFrame;
class vtkKWEntry;
class vtkKWCheckButton;
class vtkKWPushButton;
class vtkKWLabel;

class VTK_FourDUsEndoNav_EXPORT vtkFourDUsEndoNavThirdStep :
  public vtkFourDUsEndoNavStepBase
{
public:
  static vtkFourDUsEndoNavThirdStep *New();
  vtkTypeRevisionMacro(vtkFourDUsEndoNavThirdStep,vtkFourDUsEndoNavStepBase);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ShowUserInterface();
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);  

protected:
  vtkFourDUsEndoNavThirdStep();
  ~vtkFourDUsEndoNavThirdStep();

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
  vtkFourDUsEndoNavThirdStep(const vtkFourDUsEndoNavThirdStep&);
  void operator=(const vtkFourDUsEndoNavThirdStep&);
};

#endif
