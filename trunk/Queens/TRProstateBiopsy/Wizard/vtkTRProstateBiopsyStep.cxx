#include "vtkTRProstateBiopsyStep.h"
#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkObserverManager.h"
#include "vtkMRMLFiducialListNode.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyStep, "$Revision: 1.2 $");
vtkCxxSetObjectMacro(vtkTRProstateBiopsyStep,GUI,vtkTRProstateBiopsyGUI);
//vtkCxxSetObjectMacro(vtkTRProstateBiopsyStep,Logic,vtkTRProstateBiopsyLogic);

//----------------------------------------------------------------------------
vtkTRProstateBiopsyStep::vtkTRProstateBiopsyStep()
{
  this->GUI = NULL;
//  this->Logic = NULL;
  //this->MRMLScene = NULL;

  this->WizardGUICallbackCommand = vtkCallbackCommand::New();
  this->WizardGUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );

 // this->MRMLObserverManager = vtkObserverManager::New();
 // this->MRMLObserverManager->GetCallbackCommand()->SetClientData( reinterpret_cast<void *> (this) );
  //this->MRMLObserverManager->GetCallbackCommand()->SetCallback(vtkTRProstateBiopsyStep::MRMLCallback);
  //this->MRMLCallbackCommand = this->MRMLObserverManager->GetCallbackCommand();

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1.0;

  //this->InGUICallbackFlag = 0;
  //this->InMRMLCallbackFlag = 0;

}

//----------------------------------------------------------------------------
vtkTRProstateBiopsyStep::~vtkTRProstateBiopsyStep()
{
//  this->SetAndObserveMRMLScene ( NULL );

  //if (this->MRMLObserverManager)
    {
  //  this->MRMLObserverManager->Delete();
    }    

  if ( this->WizardGUICallbackCommand != NULL )
    {
    this->WizardGUICallbackCommand->Delete ( );
    this->WizardGUICallbackCommand = NULL;
    }

  this->SetGUI(NULL);
 // this->SetLogic(NULL);

}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyStep::HideUserInterface()
{
  this->Superclass::HideUserInterface();

  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyStep::Validate()
{
  this->Superclass::Validate();

  vtkKWWizardWorkflow *wizardWorkflow = 
    this->GetGUI()->GetWizardWidget()->GetWizardWorkflow();

  wizardWorkflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
  wizardWorkflow->ProcessInputs(); 
}

//----------------------------------------------------------------------------
int vtkTRProstateBiopsyStep::CanGoToSelf()
{
  return this->Superclass::CanGoToSelf() || 1;
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
//  if (!this->MRMLScene)
    {
   // this->MRMLScene = this->GetGUI()->GetMRMLScene();
    }

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  wizardWidget->GetCancelButton()->SetEnabled(0);
  wizardWidget->SetTitleAreaBackgroundColor(this->TitleBackgroundColor[0],
                                            this->TitleBackgroundColor[1],
                                            this->TitleBackgroundColor[2]);

}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  // has to be reimplemented in derived classes
}
