#include "WFWorkflowManager.h"

namespace WFEngine
{

namespace nmWFWorkflowManager
{

using namespace WFEngine::nmWFXmlWorkflowManager;
    
WFWorkflowManager::WFWorkflowManager()
{
    this->m_isLoaded = false;
}

WFWorkflowManager::~WFWorkflowManager()
{
}

WFWorkflowManager *WFWorkflowManager::New()
{
    return new WFWorkflowManager;
}

int WFWorkflowManager::LoadWorkflowFile(std::string wfDescFile)
{
 int retVal = 1;
 this->m_WFXmlWorkflowManager = WFXmlWorkflowManager::New();
 retVal = this->m_WFXmlWorkflowManager->loadWorkflowDescription(wfDescFile);
 if(retVal)
 {
   this->m_isLoaded = true;
 }
 return retVal;
}

int WFWorkflowManager::SetWorkflowFile(std::string wfDescFile)
{
 if(this->LoadWorkflowFile(wfDescFile))
 {
  this->m_wfDescFile = wfDescFile;
  this->m_isLoaded = true;
 }
 return m_isLoaded;
}

std::string WFWorkflowManager::GetWorkflowFile()
{
 return this->m_wfDescFile;
}

std::string WFWorkflowManager::GetWorkflowName()
{
    return this->m_WFXmlWorkflowManager->getWorkflowName();
}

void WFWorkflowManager::Close()
{
    if(IsLoaded())
    {
        m_WFXmlWorkflowManager->destroy();
        this->m_WFXmlWorkflowManager = NULL;
        this->m_wfDescFile = "";
        this->m_isLoaded = false;   
    }
}

bool WFWorkflowManager::IsLoaded()
{
    return m_isLoaded;
}

}//namespace

}//namespace
