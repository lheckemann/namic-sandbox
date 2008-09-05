#include "vtkTRProstateBiopsyTargetingStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"

#include "vtkKWFrame.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyTargetingStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyTargetingStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsyTargetingStep::vtkTRProstateBiopsyTargetingStep()
{
  this->SetName("2/3. Targeting");
  this->SetDescription("Select targets and prepare for biopsy.");

  this->SelectImageFrame  = NULL;
  this->SelectImageButton = NULL;
}

//----------------------------------------------------------------------------
vtkTRProstateBiopsyTargetingStep::~vtkTRProstateBiopsyTargetingStep()
{
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget *parent = wizardWidget->GetClientArea();

  // Create frame
  if (!this->SelectImageFrame)
    {
    this->SelectImageFrame = vtkKWFrame::New();
    this->SelectImageFrame->SetParent(parent);
    this->SelectImageFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->SelectImageFrame->GetWidgetName());

  if (!this->SelectImageButton)
    {
    this->SelectImageButton = vtkKWLoadSaveButtonWithLabel::New();
    this->SelectImageButton->SetParent(this->SelectImageFrame);
    this->SelectImageButton->Create();
    this->SelectImageButton->SetWidth(50);
    this->SelectImageButton->GetWidget()->SetText ("Browse Image File");
    this->SelectImageButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes(
      "{ {TRProstateBiopsy} {*.dcm} }");
    this->SelectImageButton->GetWidget()->GetLoadSaveDialog()
      ->RetrieveLastPathFromRegistry("OpenPath");
    }

  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SelectImageButton->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyTargetingStep::ProcessGUIEvents(vtkObject *caller,
                                           unsigned long event, void *callData)
{
}
