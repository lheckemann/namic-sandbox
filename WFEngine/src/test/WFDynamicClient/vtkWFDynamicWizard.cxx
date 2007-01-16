#include "vtkWFDynamicWizard.h"

#include <vtkObjectFactory.h>
#include <vtkKWNotebook.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWPushButtonSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWApplication.h>

#include <vtkKWWizardWidget.h>
#include <vtkKWWizardWorkflow.h>
#include <vtkKWWizardStep.h>

#include <vtkKWStateMachineState.h>
#include <vtkKWStateMachineInput.h>
#include <vtkKWStateMachineTransition.h>

#include <vtkCallbackCommand.h>

#include <WFStateConverter.h>

#include <time.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkWFDynamicWizard );
vtkCxxRevisionMacro(vtkWFDynamicWizard, "$Revision: 1.2 $");

vtkWFDynamicWizard::vtkWFDynamicWizard()
{
    this->m_loadedWF = NULL;
    this->m_kwwID2StepIDMap.clear();
    this->m_curStepID = 1;
}

vtkWFDynamicWizard::~vtkWFDynamicWizard()
{
}

void vtkWFDynamicWizard::CreateWidget()
{
    if (this->IsCreated())
    {
        vtkErrorMacro("class already created");
        return;
    }

    // Call the superclass to create the whole widget

    this->Superclass::CreateWidget();
    
    vtkKWApplication *app = this->GetApplication();
    
    this->SetMinimumSize(400,300);
    
    m_nbDW = vtkKWNotebook::New();
    m_nbDW->SetParent(this->GetFrame());
    m_nbDW->SetMinimumWidth(400);
    m_nbDW->SetMinimumHeight(200);
    m_nbDW->SetBackgroundColor(255,0,0);
    m_nbDW->Create();
    
    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
            m_nbDW->GetWidgetName());
    
    this->m_wfSel = m_nbDW->AddPage("Workflow-Selection");
    this->m_selWF = m_nbDW->AddPage("selected Workflow");
    
    m_nbDW->SetPageEnabled(this->m_selWF, 0);
    
//    vtkKWFrameWithLabel *frameDW = vtkKWFrameWithLabel::New();
//    frameDW->SetParent(nbDW->GetFrame(wfSel));
//    frameDW->Create();
//    frameDW->SetBackgroundColor(255,0,0);
//    
//    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
//            frameDW->GetWidgetName());
    
    m_mclDW = vtkKWMultiColumnList::New();
    m_mclDW->SetParent(m_nbDW->GetFrame(this->m_wfSel));
    m_mclDW->Create();
    
    m_mclDW->AddColumn("Workflow-Name");
    m_mclDW->AddColumn("File-Name");
    m_mclDW->AddColumn("Created");
    
    vtkCallbackCommand *mclDWSelectionChangedCmd = vtkCallbackCommand::New();
    mclDWSelectionChangedCmd->SetClientData(this);
    mclDWSelectionChangedCmd->SetCallback(&vtkWFDynamicWizard::mclDWSelectionChangedCallback);
    
    m_mclDW->AddObserver(vtkKWMultiColumnList::SelectionChangedEvent, mclDWSelectionChangedCmd);
//    m_mclDW->SetSelectionChangedCommand(this, "mclDWSelectionChangedCallback");
    
    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
            m_mclDW->GetWidgetName());
    
    m_pbtnSet = vtkKWPushButtonSet::New();
    m_pbtnSet->SetParent(m_nbDW->GetFrame(this->m_wfSel));
    m_pbtnSet->PackHorizontallyOn();
    
    m_pbtnSet->Create();

    vtkKWPushButton *pbtn = m_pbtnSet->AddWidget(0);
    pbtn->SetText("Load");
    pbtn->SetEnabled(0);
    
    vtkCallbackCommand *loadBtnPushCmd = vtkCallbackCommand::New();
    loadBtnPushCmd->SetCallback(&vtkWFDynamicWizard::loadBtnPushCmdCallback);
    loadBtnPushCmd->SetClientData(this);
    
    pbtn->AddObserver(vtkKWPushButton::InvokedEvent, loadBtnPushCmd);
        
    pbtn = m_pbtnSet->AddWidget(1);
    pbtn->SetText("Close");
    pbtn->SetEnabled(1);
    
    vtkCallbackCommand *closeBtnPushCmd = vtkCallbackCommand::New();
    closeBtnPushCmd->SetCallback(&vtkWFDynamicWizard::closeBtnPushCmdCallback);
    closeBtnPushCmd->SetClientData(this);
    
    pbtn->AddObserver(vtkKWPushButton::InvokedEvent, closeBtnPushCmd);
    
    app->Script("pack %s -side top -anchor se -expand n -fill none -padx 2 -pady 2", 
            m_pbtnSet->GetWidgetName());
}

int vtkWFDynamicWizard::ConnectToWFEngine()
{
    m_wfDI = WFDirectInterface::New();
    
    if(m_wfDI->InitializeWFEngine())
    {
        std::vector<WFDirectInterface::workflowDesc*> knownWFs;
        knownWFs = m_wfDI->GetAllKnownWorkflows();
        
        std::cout<<"knownWFs.size() "<<knownWFs.size()<<std::endl;
        
        for(std::vector<WFDirectInterface::workflowDesc*>::iterator iter = knownWFs.begin(); iter != knownWFs.end(); iter++)
        {
            WFDirectInterface::workflowDesc *curWFDesc = *iter;
            this->addWorkflowToList(curWFDesc->workflowName.c_str(), curWFDesc->fileName.c_str(), curWFDesc->createTime);
        }
        
        return 1;
    }
    else return 0;
}

void vtkWFDynamicWizard::addWorkflowToList(const char* workflowName, const char* fileName, int date)
{
    int id = this->m_mclDW->GetNumberOfRows();
    this->m_mclDW->InsertCellText(id,0, workflowName);
    this->m_mclDW->InsertCellText(id,1, fileName);
    
    time_t tim = date; 
    this->m_mclDW->InsertCellText(id,2, ctime(&tim));    
//    this->listToWFMap.insert(std::make_pair(id, fileName));
}

void vtkWFDynamicWizard::SaveState()
{
    
}

void vtkWFDynamicWizard::mclDWSelectionChangedCallback(vtkObject* obj, unsigned long,void* param, void*)
{
    vtkWFDynamicWizard *myDW = (vtkWFDynamicWizard*)param;
    
    if(myDW->m_mclDW->GetIndexOfFirstSelectedRow() != -1)
    {
        myDW->m_pbtnSet->GetWidget(0)->SetEnabled(1);
    }
    
    if(myDW->m_loadedWF != NULL && strcmp(myDW->m_mclDW->GetCellText(myDW->m_mclDW->GetIndexOfFirstSelectedRow(),1),myDW->m_loadedWF) == 0)
    {
        myDW->m_pbtnSet->GetWidget(0)->SetText("Unload");
    }
    else
        myDW->m_pbtnSet->GetWidget(0)->SetText("Load");
}

void vtkWFDynamicWizard::createWizard()
{
    this->m_nbDW->SetPageEnabled(this->m_selWF, 1);
    vtkKWWidget *wizardFrame = this->m_nbDW->GetFrame(this->m_selWF);
    
    m_curWizWidg = vtkKWWizardWidget::New();
    
    m_curWizWidg->SetParent(wizardFrame);
    m_curWizWidg->Create();
    
    this->GetApplication()->Script("pack %s -side top -anchor ne -expand y -fill both -padx 2 -pady 2", 
            m_curWizWidg->GetWidgetName());
    
    vtkKWWizardWorkflow *wizWorkflow = m_curWizWidg->GetWizardWorkflow();
           
//    create a virtual first and last step to take cover for the input and out vars in that both steps
    
    vtkKWWizardStep *virtFirstStep = vtkKWWizardStep::New();
//    vtkKWStateMachineState *virtFirstState = vtkKWStateMachineState::New();
    virtFirstStep->SetName("Welcome");
    virtFirstStep->SetDescription("");
    virtFirstStep->SetValidateCommand(this, "workStepCallBack");
        
    wizWorkflow->AddStep(virtFirstStep);
    
//    WFStateConverter *wfSC = new WFStateConverter(this->m_wfDI->getNextWorkStep());
//    vtkKWWizardStep *nextStep = wfSC->GetKWWizardStep();
//    nextStep->SetValidateCommand(this, "workStepCallBack");
//    wizWorkflow->AddNextStep(nextStep);
    
    vtkKWWizardStep *virtLastStep = vtkKWWizardStep::New();
    virtLastStep->SetName("Good Bye");
    virtLastStep->SetDescription("");
    
    wizWorkflow->AddNextStep(virtLastStep);
    
    wizWorkflow->SetInitialStep(virtFirstStep);
    wizWorkflow->SetFinishStep(virtLastStep);
    wizWorkflow->CreateGoToTransitionsToFinishStep();
//    wizWorkflow->Set(virtLastState);
    
//    vtkKWStateMachineInput *validationSucceededThisStep = vtkKWStateMachineInput::New();
//    validationSucceededThisStep->SetName("nextStep");
//    
//    wizWorkflow->AddInput(validationSucceededThisStep);
//    vtkKWStateMachineTransition *nextTrans = wizWorkflow->CreateTransition(virtFirstState, validationSucceededThisStep, virtLastState);
//    vtkCallbackCommand *nextTransCB = vtkCallbackCommand::New();
//    nextTransCB->SetCallback(&vtkWFDynamicWizard::nextTransitionCallback);
//    nextTransCB->SetClientData(this);
//    
//    nextTrans->AddObserver(vtkKWStateMachineTransition::StartEvent, nextTransCB);
//    
//    wizWorkflow->CreateBackTransition(wizWorkflow->GetStepFromState(virtFirstState), wizWorkflow->GetStepFromState(virtLastState));
//    vtkKWStateMachineInput *backInput = wizWorkflow->GetStepFromState(virtFirstState)->GetGoBackToSelfInput();
//    
//    vtkKWStateMachineTransition *backTrans = wizWorkflow->CreateTransition(virtLastState, backInput, virtFirstState);
//    vtkCallbackCommand *backTransCB = vtkCallbackCommand::New();
//    backTransCB->SetCallback(&vtkWFDynamicWizard::backTransitionCallback);
//    backTransCB->SetClientData(this);
//    
//    backTrans->AddObserver(vtkKWStateMachineTransition::StartEvent, backTransCB);
        
    vtkCallbackCommand *wizCB = vtkCallbackCommand::New();
    wizCB->SetCallback(&vtkWFDynamicWizard::navigationStackedChangedCallback);
    wizCB->SetClientData(this);
    wizWorkflow->AddObserver(vtkKWWizardWorkflow::NavigationStackedChangedEvent, wizCB);
//    
//    wizWorkflow->PushInput(validationSucceededThisStep);
//    wizWorkflow->ProcessInputs();
}

void vtkWFDynamicWizard::closeBtnPushCmdCallback(vtkObject* obj, unsigned long, void* param, void*)
{
    vtkWFDynamicWizard *myDW = (vtkWFDynamicWizard*)param;
    myDW->SaveState();
    myDW->closeWorkflow();
    myDW->OK();
}

void vtkWFDynamicWizard::loadBtnPushCmdCallback(vtkObject* obj, unsigned long, void* param, void*)
{
    vtkWFDynamicWizard *myDW = (vtkWFDynamicWizard*)param;
    std::string selectedWF = myDW->m_mclDW->GetCellText(myDW->m_mclDW->GetIndexOfFirstSelectedRow(), 1);
    std::cout<<selectedWF<<std::endl;
    vtkKWPushButton *loadBtn = vtkKWPushButton::SafeDownCast(obj);
    if(loadBtn != NULL && myDW->m_loadedWF != NULL)
    {
        myDW->m_curWizWidg->Delete();
        myDW->m_curWizWidg = NULL;
        myDW->m_loadedWF = NULL;
        myDW->m_nbDW->GetFrame(myDW->m_selWF)->RemoveAllChildren();
        loadBtn->SetText("Load");
        myDW->m_nbDW->SetPageEnabled(myDW->m_selWF,0);
        myDW->m_wfDI->CloseWorkflowManager();
    }
    
    if(loadBtn != NULL && strcmp(loadBtn->GetText(), "Load") == 0)
    {
        myDW->m_wfDI->loadWorkflowFromFile(selectedWF);
        myDW->createWizard();
        myDW->m_nbDW->RaisePage(myDW->m_selWF);
        myDW->m_loadedWF = (char*)selectedWF.c_str();
        myDW->m_mclDW->InvokeEvent(vtkKWMultiColumnList::SelectionChangedEvent, myDW);
    }    
}

void vtkWFDynamicWizard::closeWorkflow()
{
    this->m_wfDI->CloseWorkflowManager();
}

void vtkWFDynamicWizard::workStepCallBack()
{
//    vtkWFDynamicWizard *wfDW = (vtkWFDynamicWizard*)param;
    vtkKWWizardWorkflow *curWF = this->m_curWizWidg->GetWizardWorkflow();
    
//    std::cout<<obj->GetClassName()<<std::endl;
    std::cout<<this->m_curStepID<<" == "<<curWF->GetCurrentStep()->GetId() + 1<<std::endl;
    if(this->m_curStepID != (curWF->GetCurrentStep()->GetId() + 1))
    {
        std::cout<<"***workStepCallBack***"<<std::endl;
        
        WFStepObject *curWFStep = NULL;
        std::cout<<"***Current StateID: "<<curWF->GetCurrentStep()->GetId()<<" stateMapSize: "<<this->m_kwwID2StepIDMap.size()<<std::endl;
        std::cout<<"***Stack-Width: "<<curWF->GetNumberOfStepsInNavigationStack()<<" Number of known steps: "<<curWF->GetNumberOfSteps()<<std::endl;
        if(curWF->GetCurrentStep()->GetId() < curWF->GetNumberOfStepsInNavigationStack())
        {
            std::cout<<"Back pressed at position: "<<curWF->GetCurrentStep()->GetId()<<std::endl;
            curWFStep = this->m_wfDI->getBackWorkStep();
    //        this->m_kwwID2StepIDMap.erase(iter->first);
        }
        else
        {
            std::cout<<"Next pressed at position: "<<curWF->GetCurrentStep()->GetId()<<std::endl;
            curWFStep = this->m_wfDI->getNextWorkStep();
        }
        
        if(curWFStep)
        {
            WFStateConverter *wfSC = new WFStateConverter(curWFStep);
            vtkKWWizardStep *nextStep = wfSC->GetKWWizardStep();
                    
            nextStep->SetValidateCommand(this, "workStepCallBack");
            curWF->AddStep(nextStep);
            
            vtkKWStateMachineInput *validStepInput = vtkKWStateMachineInput::New();
            validStepInput->SetName("nextStepValidationInput");
            
            curWF->AddInput(validStepInput);
            
            curWF->CreateNextTransition(curWF->GetCurrentStep(), validStepInput, nextStep);
            curWF->CreateBackTransition(curWF->GetCurrentStep(), nextStep);
            
            curWF->CreateNextTransition(
                    nextStep,
                    vtkKWWizardStep::GetValidationSucceededInput(),
                    curWF->GetFinishStep());
            curWF->CreateBackTransition(nextStep, curWF->GetFinishStep());
            
            curWF->CreateGoToTransitionsToFinishStep();
            
            curWF->PushInput(validStepInput);
            
        }
        else
        {
            curWF->CreateNextTransition(
                    curWF->GetCurrentStep(),
                    vtkKWWizardStep::GetValidationSucceededInput(),
                    curWF->GetFinishStep());
            curWF->CreateBackTransition(curWF->GetFinishStep(),
                    curWF->GetCurrentStep());
            curWF->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
        }
        curWF->ProcessInputs();
        this->m_curStepID = curWF->GetCurrentStep()->GetId();
    }    
    
//    vtkKWWizardWorkflow *curWF = this->m_curWizWidg->GetWizardWorkflow();
//    WFStepObject *curWFStep = NULL;
//    std::map<int, std::string>::iterator iter;
//    for(iter = m_kwwID2StepIDMap.begin(); iter != m_kwwID2StepIDMap.end(); iter++)
//    {
//        std::cout<<iter->first<<"-"<<iter->second<<", ";
//    }
//    std::cout<<""<<std::endl;
//    iter = this->m_kwwID2StepIDMap.find(curWF->GetCurrentStep()->GetId()+1);
//    if(iter != m_kwwID2StepIDMap.end())
//    {
////        std::cout<<"Back pressed at position: "<<iter->first<<" with stepID: "<<iter->second<<std::endl;
////        curWFStep = this->m_wfDI->getBackWorkStep();
////        this->m_kwwID2StepIDMap.erase(iter->first);
//    }
//    else
//    {
////        curWFStep = this->m_wfDI->getNextWorkStep();
//    }
//    
//     
//    if(curWFStep)
//    {
//        WFStateConverter *wfSC = new WFStateConverter(curWFStep);
//        vtkKWStateMachineState *nextState = wfSC->GetKWState();
////        nextStep->SetValidateCommand(this, "workStepCallBack");
//                
//        curWF->AddState(nextState);
//        
//        vtkKWStateMachineInput *validationSucceededThisStep = vtkKWStateMachineInput::New();
//        validationSucceededThisStep->SetName("nextStep");
//        
//        
//        // Manually connect the next step to this step
//
////        vtkKWStateMachineTransition *nextTrans = vtkKWStateMachineTransition::New();
////        
////        nextTrans->SetInput(validationSucceededThisStep);
////        nextTrans->SetOriginState(curWF->GetCurrentState());
////        nextTrans->SetDestinationState(nextState);
//        
//        curWF->AddInput(validationSucceededThisStep);
//        vtkKWStateMachineTransition *nextTrans = curWF->CreateTransition(curWF->GetCurrentState(), validationSucceededThisStep, nextState);
//        vtkCallbackCommand *nextTransCB = vtkCallbackCommand::New();
//        nextTransCB->SetCallback(&vtkWFDynamicWizard::nextTransitionCallback);
//        nextTransCB->SetClientData(this);
//        
//        nextTrans->AddObserver(vtkKWStateMachineTransition::StartEvent, nextTransCB);
////        curWF->CreateNextTransition(curWF->GetCurrentStep(),
////                validationSucceededThisStep,
////                nextStep);
//        
////        curWF->CreateBackTransition(curWF->GetStepFromState(nextState), curWF->CurrentState());
//        
////        curWF->GetStepFromState(nextState)->GetGoBackToSelfInput();
//        
//        vtkKWStateMachineInput *backValidationInput = vtkKWStateMachineInput::New();
//        backValidationInput->SetName("backValidation");
//        curWF->AddInput(backValidationInput);
//        vtkKWStateMachineTransition *backTrans = curWF->CreateTransition(nextState, backValidationInput, curWF->GetCurrentState());
//                        
//        vtkCallbackCommand *backTransCB = vtkCallbackCommand::New();
//        backTransCB->SetCallback(&vtkWFDynamicWizard::backTransitionCallback);
//        backTransCB->SetClientData(this);
//        
//        backTrans->AddObserver(vtkKWStateMachineTransition::StartEvent, nextTransCB);
////        curWF->AddTransition(backTrans);
////        curWF->CreateBackTransition(
////        curWF->GetCurrentStep(), nextStep);
//
//        this->m_kwwID2StepIDMap.insert(std::make_pair(nextState->GetId(), curWFStep->GetID()));
//        // Manually connect this step to the result step
//
////        curWF->CreateNextTransition(
////                curWF->GetCurrentStep(),
////                vtkKWWizardStep::GetValidationSucceededInput(),
////                curWF->GetFinishStep());
////        curWF->CreateBackTransition(
////                curWF->GetFinishStep(), curWF->GetCurrentStep());
//        
//    //    curWF->CreateGoToTransitionsToFinishStep();
//        curWF->PushInput(validationSucceededThisStep);
//    }
//    else
//    {
//        curWF->CreateNextTransition(
//                curWF->GetCurrentStep(),
//                vtkKWWizardStep::GetValidationSucceededInput(),
//                curWF->GetFinishStep());
//        curWF->CreateBackTransition(curWF->GetFinishStep(),
//                curWF->GetCurrentStep());
//        curWF->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
//    }
//    curWF->ProcessInputs();
}

void vtkWFDynamicWizard::navigationStackedChangedCallback(vtkObject *obj, unsigned long, void* param, void* cbParam)
{
    vtkWFDynamicWizard *wfDW = (vtkWFDynamicWizard*)param;
    vtkKWWizardWorkflow *curWF = wfDW->m_curWizWidg->GetWizardWorkflow();
    std::cout<<wfDW->m_curStepID<<" == "<<curWF->GetCurrentStep()->GetId() + 1<<std::endl;
    if(wfDW->m_curStepID > curWF->GetCurrentStep()->GetId())
    {
        std::cout<<"Back pressed"<<std::endl;
        wfDW->m_curStepID = curWF->GetCurrentStep()->GetId();
    }
}

void vtkWFDynamicWizard::nextTransitionCallback(vtkObject* obj, unsigned long,void*, void*)
{
    std::cout<<"nextTransitionCallback"<<std::endl;
}

void vtkWFDynamicWizard::backTransitionCallback(vtkObject* obj, unsigned long,void*, void*)
{
    std::cout<<"backTransitionCallback"<<std::endl;
}
