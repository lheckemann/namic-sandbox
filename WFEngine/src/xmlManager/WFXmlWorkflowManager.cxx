#include "WFXmlWorkflowManager.h"

using namespace WFEngine::nmWFXmlWorkflowManager;

XERCES_CPP_NAMESPACE_USE;

WFXmlWorkflowManager::WFXmlWorkflowManager() : WFXmlManager()
{
    this->specification = "specification";
    this->decomposition = "decomposition";
    this->schema = "schema";
}

WFXmlWorkflowManager::~WFXmlWorkflowManager()
{
}

WFXmlWorkflowManager *WFXmlWorkflowManager::New()
{
    return new WFXmlWorkflowManager();
}

int WFXmlWorkflowManager::loadWorkflowDescription(std::string &wfXmlFile)
{
    if(this->loadXmlFile(wfXmlFile))
    {
        this->m_isLoaded = true;
        return 1;
    }
    else return 0;
}

bool WFXmlWorkflowManager::IsLoaded()
{
    return this->m_isLoaded;
}

std::string WFXmlWorkflowManager::getWorkflowName()
{
    if(this->IsLoaded())
    {
        DOMNodeList *decompositionList = this->getAllChildesByName(specification, decomposition);
        if(decompositionList->getLength() > 0)
        {
            DOMElement *firstDecompElem = (DOMElement*)decompositionList->item(0);
            return XMLString::transcode(firstDecompElem->getAttribute(XMLString::transcode("id")));
        }            
    }
    return "";
}

void WFXmlWorkflowManager::destroy()
{
    delete(this);
}
