#include "vtkWFTestWizard.h"

#include "vtkObjectFactory.h"

#include "vtkKWApplication.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWLabel.h"
#include "vtkKWIcon.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardStep.h"
#include "vtkKWStateMachineState.h"
#include "vtkKWStateMachineCluster.h"
#include "vtkKWStateMachineTransition.h"
#include "vtkKWFrameWithLabel.h"

#include "vtkCallbackCommand.h"

#include <vtksys/SystemTools.hxx>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkWFTestWizard );
vtkCxxRevisionMacro(vtkWFTestWizard, "$Revision: 1.2 $");

vtkWFTestWizard::vtkWFTestWizard()
{
  this->m_FirstStep = NULL;
//  this->OperatorRadioButtonSet = vtkKWRadioButtonSet::New();
  
  this->m_SecondStep = NULL;
  this->m_ThirdStep = NULL;
  this->m_FourthStep = NULL;
  this->m_LastStep = NULL;
}

vtkWFTestWizard::~vtkWFTestWizard()
{
}

void vtkWFTestWizard::CreateWidget()
{
    this->initializeWizard();
}

void vtkWFTestWizard::initializeWizard()
{
  // Check if already created

    if (this->IsCreated())
      {
      vtkErrorMacro("class already created");
      return;
      }

    // Call the superclass to create the whole widget

    this->Superclass::CreateWidget();

    vtkKWWizardWorkflow *wizard_workflow = this->GetWizardWorkflow();
    vtkKWWizardWidget *wizard_widget = this->GetWizardWidget();

    wizard_widget->GetTitleIconLabel()->SetImageToPredefinedIcon(
      vtkKWIcon::IconMagGlass);

    // -----------------------------------------------------------------
    // m_FirstStep

    if (!this->m_FirstStep)
      {
      this->m_FirstStep = vtkKWWizardStep::New();
      }
    this->m_FirstStep->SetName("pre-operation phase");
    this->m_FirstStep->SetDescription("Welcome");
    this->m_FirstStep->SetShowUserInterfaceCommand(
      this, "Show1UserInterfaceCallback");
    this->m_FirstStep->SetHideUserInterfaceCommand(wizard_widget, "ClearPage");
    this->m_FirstStep->SetValidateCommand(this, "Validate1Callback");

    wizard_workflow->AddStep(this->m_FirstStep);
      
      // -----------------------------------------------------------------
      // m_SecondStep
      // load patient data

      if (!this->m_SecondStep)
        {
        this->m_SecondStep = vtkKWWizardStep::New();
        }
      this->m_SecondStep->SetName("pre-operation phase");
      this->m_SecondStep->SetDescription("Loading patient data");
      this->m_SecondStep->SetShowUserInterfaceCommand(
        this, "Show2UserInterfaceCallback");
      this->m_SecondStep->SetHideUserInterfaceCommand(wizard_widget, "ClearPage");
      this->m_SecondStep->SetValidateCommand(this, "Validate2Callback");

      wizard_workflow->AddNextStep(this->m_SecondStep);
      
      // -----------------------------------------------------------------
      // m_ThirdStep
      // show patient data and validate it

      if (!this->m_ThirdStep)
        {
        this->m_ThirdStep = vtkKWWizardStep::New();
        }
      this->m_ThirdStep->SetName("pre-operation phase");
      this->m_ThirdStep->SetDescription("Validate patient data");
      this->m_ThirdStep->SetShowUserInterfaceCommand(
        this, "Show3UserInterfaceCallback");
      this->m_ThirdStep->SetHideUserInterfaceCommand(wizard_widget, "ClearPage");
      this->m_ThirdStep->SetValidateCommand(this, "Validate3Callback");

      wizard_workflow->AddNextStep(this->m_ThirdStep);
      
      // -----------------------------------------------------------------
      // m_FourthStep
      // loading MR-Images into Slicer

      if (!this->m_FourthStep)
        {
        this->m_FourthStep = vtkKWWizardStep::New();
        }
      this->m_FourthStep->SetName("operation phase");
      this->m_FourthStep->SetDescription("Getting MR-Images");
      this->m_FourthStep->SetShowUserInterfaceCommand(
        this, "Show4UserInterfaceCallback");
      this->m_FourthStep->SetHideUserInterfaceCommand(wizard_widget, "ClearPage");
      this->m_FourthStep->SetValidateCommand(this, "Validate4Callback");

      wizard_workflow->AddNextStep(this->m_FourthStep);
      
      // -----------------------------------------------------------------
      // m_LastStep
      // last step

      if (!this->m_LastStep)
        {
        this->m_LastStep = vtkKWWizardStep::New();
        }
      this->m_LastStep->SetName("operation phase");
      this->m_LastStep->SetDescription("Getting MR-Images");
      this->m_LastStep->SetShowUserInterfaceCommand(
        this, "ShowLastUserInterfaceCallback");
      this->m_LastStep->SetHideUserInterfaceCommand(wizard_widget, "ClearPage");
      this->m_LastStep->SetValidateCommand(this, "ValidateLastCallback");

      wizard_workflow->AddNextStep(this->m_LastStep);
    
    // -----------------------------------------------------------------
  // Initial and finish step
  wizard_workflow->SetInitialStep(this->m_FirstStep);
  wizard_workflow->SetFinishStep(this->m_LastStep);
  wizard_workflow->CreateGoToTransitionsToFinishStep();
}

void vtkWFTestWizard::Show1UserInterfaceCallback()
{
//  if (!this->OperatorRadioButtonSet)
//    {
//    this->OperatorRadioButtonSet = vtkKWRadioButtonSet::New();
//    }
//
//  // Create radio buttons for each of the mathematical operator we support
//
//  if (!this->OperatorRadioButtonSet->IsCreated())
//    {
//    this->OperatorRadioButtonSet->SetParent(
//      this->GetWizardWidget()->GetClientArea());
//    this->OperatorRadioButtonSet->Create();
//    this->OperatorRadioButtonSet->SetBackgroundColor(255,0,0);
//
//    vtkKWRadioButton *radiob;
//
//    radiob = this->OperatorRadioButtonSet->AddWidget(
//        vtkWFTestWizard::OperatorAddition);
//    radiob->SetText("Addition");
//    radiob->SetCommand(this->GetWizardWidget(), "Update");
//
//    radiob = this->OperatorRadioButtonSet->AddWidget(
//        vtkWFTestWizard::OperatorDivision);
//    radiob->SetText("Division");
//    radiob->SetCommand(this->GetWizardWidget(), "Update");
//
//    radiob = this->OperatorRadioButtonSet->AddWidget(
//        vtkWFTestWizard::OperatorSquareRoot);
//    radiob->SetText("Square Root");
//    radiob->SetCommand(this->GetWizardWidget(), "Update");
//
//    this->OperatorRadioButtonSet->GetWidget(
//        vtkWFTestWizard::OperatorAddition)->Select();
//    }
//  
//  this->Script("pack %s -side top -expand y -fill both -anchor center", 
//               this->OperatorRadioButtonSet->GetWidgetName());

    vtkKWLabel *welcomeLbl = vtkKWLabel::New();
    
    welcomeLbl->SetParent(this->GetWizardWidget()->GetClientArea());
    welcomeLbl->Create();
    
    welcomeLbl->SetText("Welcome to the prostata biopsy wizard!");
    welcomeLbl->SetFont("helvetica 12 {bold}");
    welcomeLbl->SetJustificationToCenter();
    
    this->Script("pack %s -side top -fill x -anchor center", 
               welcomeLbl->GetWidgetName());
               
    vtkKWLabel *descLbl = vtkKWLabel::New();
    
    descLbl->SetParent(this->GetWizardWidget()->GetClientArea());
    descLbl->Create();
    
    descLbl->SetText("This wizard supports you in a pre- and in operation phase of a prostata biopsy.\n Press \"next\" to begin procedure.");
    descLbl->SetFont("helvetica 12");
    descLbl->SetJustificationToCenter();
    descLbl->SetAdjustWrapLengthToWidth(descLbl->GetWidth());
    
    this->Script("pack %s -side top -expand y -fill x -anchor center", 
               descLbl->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkWFTestWizard::Validate1Callback()
{
  vtkKWWizardWorkflow *wizard_workflow = this->GetWizardWorkflow();

  wizard_workflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
  wizard_workflow->ProcessInputs();
}

void vtkWFTestWizard::Show2UserInterfaceCallback()
{
    if(!this->m_patientSelectorRBS)
    {
        this->m_patientSelectorRBS = vtkKWRadioButtonSet::New();   
    }
    if(!this->m_patientSelectorRBS->IsCreated())
    {
        this->m_patientSelectorRBS->SetParent(this->GetWizardWidget()->GetClientArea());
        this->m_patientSelectorRBS->Create();
    }
    
    this->m_patientSelectorRBS->PackHorizontallyOn();
    
    vtkKWRadioButton *patRBtn;
    
    vtkCallbackCommand *patientButtonChangedCmd = vtkCallbackCommand::New();
    patientButtonChangedCmd->SetCallback(&vtkWFTestWizard::patientSelectorCallback);
    
    if(!this->m_patientSelectorRBS->HasWidget(patientNew))
    {
        int i = patientNew;
        patientButtonChangedCmd->SetClientData((void*)&i);
        patRBtn = this->m_patientSelectorRBS->AddWidget(patientNew);
        patRBtn->SetText("Create new patient");
        patRBtn->SetCommand(this->GetWizardWidget(), "Update");
        patRBtn->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, patientButtonChangedCmd);
    }
    
    if(!this->m_patientSelectorRBS->HasWidget(patientLoad))
    {
        int i = patientLoad;
        patientButtonChangedCmd->SetClientData((void*)&i);
        patRBtn = this->m_patientSelectorRBS->AddWidget(patientLoad);
        patRBtn->SetText("Load patient data from file");
        patRBtn->SetCommand(this->GetWizardWidget(), "Update");
        patRBtn->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, patientButtonChangedCmd);
    }   
    
    patientButtonChangedCmd->Delete();
    patientButtonChangedCmd = NULL;
    
    this->Script("pack %s -side top -fill x -anchor center", 
               this->m_patientSelectorRBS->GetWidgetName());
    
}

void vtkWFTestWizard::Show3UserInterfaceCallback()
{
    
}

void vtkWFTestWizard::Show4UserInterfaceCallback()
{
    
}

void vtkWFTestWizard::ShowLastUserInterfaceCallback()
{
    
}


void vtkWFTestWizard::Validate2Callback()
{
    vtkKWWizardWorkflow *wizard_workflow = this->GetWizardWorkflow();

  // If the operand is invalid, display an error and push the input
  // that will bring us back to the same state. Otherwise move on
  // to the next step!

//  int valid = this->IsOperand1Valid();
//  if (valid == WFTestWizard::OperandIsEmpty)
//    {
//    this->GetWizardWidget()->SetErrorText("Empty operand!");
//    wizard_workflow->PushInput(vtkKWWizardStep::GetValidationFailedInput());
//    }
//  else if (valid == WFTestWizard::OperandIsNegative)
//    {
//    this->GetWizardWidget()->SetErrorText("Negative operand!");
//    wizard_workflow->PushInput(vtkKWWizardStep::GetValidationFailedInput());
//    }
//  else
//    {
//    wizard_workflow->PushInput(
//      vtkKWWizardStep::GetValidationSucceededInput());
//    }

  if(this->m_patientSelectorRBS->GetWidget(0)->GetSelectedState())
  {
    wizard_workflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());    
  }
  else if(this->m_patientSelectorRBS->GetWidget(1)->GetSelectedState())
       {
         wizard_workflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput()); 
       }
       else 
       {
         this->GetWizardWidget()->SetErrorText("No option selected!");
         wizard_workflow->PushInput(vtkKWWizardStep::GetValidationFailedInput());
       }
  wizard_workflow->ProcessInputs();
}

void vtkWFTestWizard::Validate3Callback()
{
    vtkKWWizardWorkflow *wizard_workflow = this->GetWizardWorkflow();
    wizard_workflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
    wizard_workflow->ProcessInputs();
}

void vtkWFTestWizard::Validate4Callback()
{
    vtkKWWizardWorkflow *wizard_workflow = this->GetWizardWorkflow();
    wizard_workflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
    wizard_workflow->ProcessInputs();
}

void vtkWFTestWizard::ValidateLastCallback()
{
    vtkKWWizardWorkflow *wizard_workflow = this->GetWizardWorkflow();
    wizard_workflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
    wizard_workflow->ProcessInputs();
}

int vtkWFTestWizard::EntryChangedCallback(const char* value)
{
  
}

void vtkWFTestWizard::UpdateEnableState()
{
  
}

int vtkWFTestWizard::getPatientSelector()
{
    if(m_patientSelectorRBS->GetWidget(patientNew)->GetSelectedState())
    {
        return patientNew;
    }
    
    if(this->m_patientSelectorRBS->GetWidget(patientLoad)->GetSelectedState())
    {
        return patientLoad;
    }
}

void vtkWFTestWizard::patientSelectorCallback(vtkObject* obj, unsigned long, void* param, void*)
{
    std::cout<<1<<std::endl;
    vtkKWRadioButton *selectedRB = vtkKWRadioButton::SafeDownCast(obj);
    std::cout<<1<<std::endl;
    int radioBtnType = *(int*)param;
    std::cout<<1<<std::endl;        
    if(!selectedRB)
    {
        std::cout<<"radiobutton is NULL"<<std::endl;
        return;
    }
    std::cout<<radioBtnType<<std::endl;
    vtkKWWizardDialog *wizWidg = vtkKWWizardDialog::SafeDownCast(selectedRB->GetParentTopLevel());
    std::cout<<1<<std::endl;
    
//    for(int i = 0; i < GetWizardWidget()->GetClientArea()->GetNumberOfChildren(); i++)
//    {
//        
//    }      
    vtkKWFrameWithLabel *newFrame = vtkKWFrameWithLabel::New(); 

    newFrame->SetParent(wizWidg->GetWizardWidget()->GetClientArea());
    newFrame->Create();
                
    if(selectedRB->GetSelectedState())
    {
        std::cout<<"true"<<std::endl;
        if(radioBtnType == 0)
        {
            std::cout<<"create label"<<std::endl;
            vtkKWLabel *newPatientLbl = vtkKWLabel::New();
            newPatientLbl->SetParent(newFrame);
            newPatientLbl->Create();
            newPatientLbl->SetText("Please click on the \"next\" button\n to define the patient data.");
            newFrame->Script("pack %s -side top -expand y -fill both -anchor center", 
               newPatientLbl->GetWidgetName());
        }
    
        if(radioBtnType == 1)
        {
            std::cout<<"create button"<<std::endl;
            vtkKWLoadSaveButton *load_button1 = vtkKWLoadSaveButton::New();
            load_button1->SetParent(newFrame);
            load_button1->Create();
            load_button1->SetText("Load patient data file");
            load_button1->GetLoadSaveDialog()->SaveDialogOff(); // load mode
            newFrame->Script("pack %s -side top -expand y -fill both -anchor center", 
               load_button1->GetWidgetName());
        }
    }
    
    wizWidg->Script("pack %s -side top -expand y -fill both -anchor center", 
               newFrame->GetWidgetName());    
}
