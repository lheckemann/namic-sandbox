#include "WFXmlWorkflowManager.h"

using namespace WFEngine::nmWFXmlConfigManager;

XERCES_CPP_NAMESPACE_USE;

WFXmlWorkflowManager::WFXmlWorkflowManager()
{
}

WFXmlWorkflowManager::~WFXmlWorkflowManager()
{
}

WFXmlWorkflowManager *WFXmlWorkflowManager::New()
{
    return new WFXmlWorkflowManager();
}

void WFXmlWorkflowManager::loadWorkflowDescription(std::string &wfXmlFile)
{
    if(this->loadXmlFile(wfXmlFile))
        this->m_isLoaded = true;
}

bool WFXmlWorkflowManager::IsLoaded()
{
    return this->m_isLoaded;
}
