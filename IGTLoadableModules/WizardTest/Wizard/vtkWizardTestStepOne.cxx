/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkWizardTestStepOne.h"

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
vtkStandardNewMacro(vtkWizardTestStepOne);
vtkCxxRevisionMacro(vtkWizardTestStepOne, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkWizardTestStepOne::vtkWizardTestStepOne()
{
  this->SetTitle("Verification");
  this->SetDescription("Measure needle distance from target.");

  this->MainFrame=NULL;
  
  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1;

  this->ProcessingCallback = false;

  this->LoadMe = NULL;
}

//----------------------------------------------------------------------------
vtkWizardTestStepOne::~vtkWizardTestStepOne()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(LoadMe);

}

//----------------------------------------------------------------------------
void vtkWizardTestStepOne::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  if(!this->LoadMe)
    {
      this->LoadMe = vtkKWPushButton::New();
    }
  if(!this->LoadMe->IsCreated())
    {
      this->LoadMe->SetParent(this->GetGUI()->GetWizardWidget()->GetClientArea());
      this->LoadMe->Create();
      this->LoadMe->SetBorderWidth(2);
      this->LoadMe->SetReliefToRaised();
      this->LoadMe->SetHighlightThickness(2);
      this->LoadMe->SetBackgroundColor(0.85,0.85,0.85);
      this->LoadMe->SetActiveBackgroundColor(1,1,1);
      this->LoadMe->SetText("LoadMe");
    }

  this->Script("pack %s -side top -anchor nw -padx 0 -pady 2",
         this->LoadMe->GetWidgetName());

  this->AddGUIObservers();
  
  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkWizardTestStepOne::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkWizardTestStepOne::ProcessGUIEvents(vtkObject *caller,
                                          unsigned long event, void *callData)
{

}


//-----------------------------------------------------------------------------
void vtkWizardTestStepOne::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if(this->LoadMe)
    {
      this->LoadMe->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
}
//-----------------------------------------------------------------------------
void vtkWizardTestStepOne::RemoveGUIObservers()
{
  if(this->LoadMe)
    {
      this->LoadMe->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
}

//--------------------------------------------------------------------------------
void vtkWizardTestStepOne::UpdateGUI()
{

}

//----------------------------------------------------------------------------
void vtkWizardTestStepOne::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkWizardTestStepOne::TearDownGUI()
{  
  RemoveGUIObservers();
}

