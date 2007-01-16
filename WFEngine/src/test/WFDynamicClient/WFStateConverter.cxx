#include "WFStateConverter.h"
#include "vtkKWStateMachineState.h"
#include "vtkKWWizardStep.h"


#include <iostream>

using namespace WFEngine::nmWFStepObject;

WFStateConverter::WFStateConverter()
{
    this->m_isLoaded = false;
}

WFStateConverter::WFStateConverter(WFStepObject *wfStep)
{
    this->m_isLoaded = false;
    this->ConvertWFStep(wfStep);
}


WFStateConverter::~WFStateConverter()
{
}

void WFStateConverter::ConvertWFStep(WFStepObject *wfStep)
{
    this->m_KWState = this->createKWState(wfStep);
    this->m_KWWizardStep = this->createKWWizardStep(wfStep);
    
    this->m_isLoaded = true;
}

vtkKWStateMachineState *WFStateConverter::createKWState(WFStepObject *wfStep)
{
    vtkKWStateMachineState *curState = vtkKWStateMachineState::New();
    
    curState->SetName(wfStep->GetName().c_str());
    curState->SetDescription(wfStep->GetDescription().c_str());
    
    return curState;
}

vtkKWWizardStep *WFStateConverter::createKWWizardStep(WFStepObject *wfStep)
{
    vtkKWWizardStep *curWS = vtkKWWizardStep::New();
    
    curWS->SetName(wfStep->GetName().c_str());
    curWS->SetDescription(wfStep->GetDescription().c_str());
    
    return curWS;
}

vtkKWStateMachineState *WFStateConverter::GetKWState()
{
    if(IsLoaded())
        return this->m_KWState;
    else
        std::cout<<"WFStateConverter not loaded!"<<std::endl;            
}

vtkKWWizardStep *WFStateConverter::GetKWWizardStep()
{
    if(IsLoaded())
        return this->m_KWWizardStep;
    else
        std::cout<<"WFStateConverter not loaded!"<<std::endl;            
}

bool WFStateConverter::IsLoaded()
{
    return this->m_isLoaded;
}

//vtkKWStateMachineState *WFStateConverter::YAWL2KWState(WFStepObject *wfState)
//{
//    vtkKWStateMachineState *kwState = vtkKWStateMachineState::New();
//    
//    kwState->SetName(wfState->GetName);
//    kwState->SetDescription()
//}
//
//WFStepObject *WFStateConverter::KWState2YAWL(vtkKWStateMachineState *kwState)
//{
//    
//}



