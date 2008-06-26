#include "vtkPerkStationStep.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationStep);
vtkCxxRevisionMacro(vtkPerkStationStep, "$Revision: 1.0 $");
vtkCxxSetObjectMacro(vtkPerkStationStep,GUI,vtkPerkStationModuleGUI);

//----------------------------------------------------------------------------
vtkPerkStationStep::vtkPerkStationStep()
{
  this->GUI = NULL;
}

//----------------------------------------------------------------------------
vtkPerkStationStep::~vtkPerkStationStep()
{
  this->SetGUI(NULL);
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
