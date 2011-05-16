/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkWizardTestStepTwo.h"

#include "vtkObject.h"

#include "vtkWizardTestGUI.h"
#include "vtkWizardTestLogic.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"

#include "vtkKWFrame.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWFrame.h"
#include "vtkKWEntry.h"
#include "vtkKWLabel.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWText.h"
#include "vtkKWPushButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWCheckButton.h"

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj) \
    { \
    obj->SetParent(NULL); \
    obj->Delete(); \
    obj = NULL; \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkWizardTestStepTwo);
vtkCxxRevisionMacro(vtkWizardTestStepTwo, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkWizardTestStepTwo::vtkWizardTestStepTwo()
{
  this->SetTitle("Calibration");
  this->SetDescription("Point the tip and pivot around.");

  this->MainFrame=NULL;
  
  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 1;
  this->TitleBackgroundColor[2] = 0.8;

  this->ProcessingCallback = false;
}

//----------------------------------------------------------------------------
vtkWizardTestStepTwo::~vtkWizardTestStepTwo()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
}

//----------------------------------------------------------------------------
void vtkWizardTestStepTwo::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  this->AddGUIObservers();
  
  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkWizardTestStepTwo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkWizardTestStepTwo::ProcessGUIEvents(vtkObject *caller,
                                          unsigned long event, void *callData)
{

}


//-----------------------------------------------------------------------------
void vtkWizardTestStepTwo::AddGUIObservers()
{
  this->RemoveGUIObservers();
}
//-----------------------------------------------------------------------------
void vtkWizardTestStepTwo::RemoveGUIObservers()
{

}

//--------------------------------------------------------------------------------
void vtkWizardTestStepTwo::UpdateGUI()
{

}

//----------------------------------------------------------------------------
void vtkWizardTestStepTwo::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkWizardTestStepTwo::TearDownGUI()
{  
  RemoveGUIObservers();
}

