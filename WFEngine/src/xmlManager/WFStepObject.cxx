#include "WFStepObject.h"

using namespace WFEngine::nmWFStepObject;

WFStepObject::WFStepObject()
{
    this->m_ID = "";
    this->m_name = "";
    this->m_desc = "";
}

WFStepObject::~WFStepObject()
{
    
}

WFStepObject *WFStepObject::New()
{
    return new WFStepObject();
}

void WFStepObject::SetID(std::string &ID)
{
    this->m_ID = ID;
}

void WFStepObject::SetName(std::string &name)
{
    this->m_name = name;
}

void WFStepObject::AddNextStepID(std::string &nextStepID)
{
    this->m_nextSteps.push_back(nextStepID);
}

void WFStepObject::SetVariableMapping(std::string &from, std::string &to)
{
    this->m_varMapping.insert(std::make_pair(from,to));
}

std::string WFStepObject::GetID()
{
    return this->m_ID;
}

std::string WFStepObject::GetName()
{
    return this->m_name;
}

std::string WFStepObject::GetNextStepID()
{
    if(this->m_nextSteps.size() > 0)
        return this->m_nextSteps[0];
    else
        return "";
            
}

std::string WFStepObject::GetDescription()
{
    return this->m_desc;
}

void WFStepObject::SetDescription(std::string &stepDesc)
{
    this->m_desc = stepDesc;
}
