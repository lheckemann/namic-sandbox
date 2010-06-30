#include "vtkPerkStationStep.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationStep);
vtkCxxRevisionMacro(vtkPerkStationStep, "$Revision: 1.0 $");
vtkCxxSetObjectMacro(vtkPerkStationStep,GUI,vtkPerkStationModuleGUI);

//----------------------------------------------------------------------------
vtkPerkStationStep::vtkPerkStationStep()
{
  this->GUI = NULL;
  this->WizardGUICallbackCommand = vtkCallbackCommand::New();
  this->LogTimer = vtkTimerLog::New();
  this->WizardGUICallbackCommand->SetClientData(reinterpret_cast<void *>(this));
}

//----------------------------------------------------------------------------
vtkPerkStationStep::~vtkPerkStationStep()
{
  this->SetGUI(NULL);
  if(this->WizardGUICallbackCommand) 
  {
    this->WizardGUICallbackCommand->Delete();
    this->WizardGUICallbackCommand=NULL;
  }
  this->LogTimer->Delete();
}

//----------------------------------------------------------------------------
void vtkPerkStationStep::HideUserInterface()
{
  this->Superclass::HideUserInterface();

  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationStep::Validate()
{
  this->Superclass::Validate();

  vtkKWWizardWorkflow *wizard_workflow = 
    this->GetGUI()->GetWizardWidget()->GetWizardWorkflow();

  wizard_workflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
  wizard_workflow->ProcessInputs();
}

//----------------------------------------------------------------------------
int vtkPerkStationStep::CanGoToSelf()
{
  return this->Superclass::CanGoToSelf() || 1;
}

//----------------------------------------------------------------------------
void vtkPerkStationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkPerkStationStep::ReleaseReferencesForKWEntrySet(vtkKWEntrySet *entrySet)
{
  if ( entrySet )
    {
    if ( entrySet->IsCreated() )
      {
      for( int i=0; i < entrySet->GetNumberOfWidgets(); i++ )
        {
        entrySet->GetWidget( i )->SetParent( NULL );
        }
      }  
    }
}
