#include "WFXmlWorkflowManager.h"
#include "WFStepObject.h"

#include <iostream>

using namespace WFEngine::nmWFXmlWorkflowManager;
using namespace WFEngine::nmWFStepObject;

XERCES_CPP_NAMESPACE_USE;

WFXmlWorkflowManager::WFXmlWorkflowManager() : WFXmlManager()
{
    this->specification = "specification";
    this->decomposition = "decomposition";
    this->schema = "schema";
    this->processControlElements = "processControlElements";
    this->metaData = "metaData";
    
    this->m_parentWFElement = NULL;
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
    this->m_parentWFElement = NULL;
    
    if(this->loadXmlFile(wfXmlFile))
    {
        this->m_isLoaded = true;
        this->m_parentWFElement = this->getParentWFElement();
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
    std::cout<<"getWorkflowName from Node: ";
    if(this->m_parentWFElement)
    {
        std::cout<<XMLString::transcode(this->m_parentWFElement->getTagName())<<std::endl;
        return XMLString::transcode(m_parentWFElement->getAttribute(XMLString::transcode("id")));
    }            
    std::cout<<"not found!"<<std::endl;
    return "";
}

std::string WFXmlWorkflowManager::getWorkflowDescription()
{
    std::cout<<"getWorkflowDescription from Node: ";
    DOMElement *metaDataElem = this->getMetadataWFElement();
    if(metaDataElem)
    {
        std::cout<<XMLString::transcode(metaDataElem->getTagName())<<std::endl;
        std::string description = "description";
        DOMNodeList *metaEntries = this->getAllChildesByName(metaDataElem, description);
        if(metaEntries->getLength() > 0)
        {
            return XMLString::transcode(((DOMElement*)metaEntries->item(0))->getTextContent());
        }
    }            
    std::cout<<"not found!"<<std::endl;
    return "";
}

void WFXmlWorkflowManager::destroy()
{
    this->m_isLoaded = false;
    delete(this);
}

WFStepObject *WFXmlWorkflowManager::getNextWorkstepDescription(WFStepObject *curWFStep)
{
    std::string nextID;
    if(curWFStep)
    {
        nextID = curWFStep->GetNextStepID();
    }
    else
    {
        nextID = "";
    }
    
    DOMElement *curTaskElement = this->getElementFromID(nextID);
    DOMElement *curProcessElement;
    if(curTaskElement)
    {
        WFStepObject *curWS = WFStepObject::New();

        std::string ID = XMLString::transcode(curTaskElement->getAttribute(XMLString::transcode("id")));
        curWS->SetID(ID);
        
        std::string desc = this->getWorkflowDescription();
        curWS->SetDescription(desc);
        
        std::string name = "name";
        curProcessElement = this->getFirstChildByName(curTaskElement, name);
        if(curProcessElement)
        {
            std::string newName = XMLString::transcode(curProcessElement->getTextContent());
            curWS->SetName(newName);
            curProcessElement = NULL;
        }
        
        
        std::string flowsInto = "flowsInto";
        std::string nextElementRef = "nextElementRef";
        curProcessElement = this->getFirstChildByName(curTaskElement, flowsInto);
        if(curProcessElement)
        {
            DOMNodeList *nextStepIDsElements = this->getAllChildesByName(curProcessElement, nextElementRef);
            for(int i = 0; i < nextStepIDsElements->getLength(); i++)
            {
                DOMElement *curElem = (DOMElement*)(nextStepIDsElements->item(i));
                std::string nextStepID = XMLString::transcode(curElem->getAttribute(XMLString::transcode("id")));
                curWS->AddNextStepID(nextStepID);
            }    
        }
        

//        curWS->SetVariableMapping();
        return curWS;
    }
    else
        return NULL;
}

DOMElement *WFXmlWorkflowManager::getParentWFElement()
{
    if(this->IsLoaded())
    {
        DOMNodeList *decompositionList = this->getAllChildesByName(specification, decomposition);
        if(decompositionList->getLength() > 0)
        {
            return (DOMElement*)decompositionList->item(0);
        }
    }
    return NULL;
}

DOMElement *WFXmlWorkflowManager::getMetadataWFElement()
{
    if(this->IsLoaded())
    {
        DOMNodeList *metaDataList = this->getAllChildesByName(specification, metaData);
        if(metaDataList->getLength() > 0)
        {
            return (DOMElement*)metaDataList->item(0);
        }
    }
    return NULL;
}

DOMElement *WFXmlWorkflowManager::getElementFromID(std::string &id)
{
    DOMElement *rootWFElement = getParentWFElement();
    if(!rootWFElement)
    {
        return NULL;
    }
    
    DOMElement *processWFElement = this->getFirstChildByName(rootWFElement, this->processControlElements);
    if(!processWFElement)
        return NULL;
    
    std::string task = "task";
    DOMNodeList *taskNodes = this->getAllChildesByName(processWFElement, task);
    if(taskNodes == NULL || taskNodes->getLength() == 0)
    {
        return NULL;
    }
    if(id == "")
    {
        return (DOMElement*)(taskNodes->item(0));
    }
    else
    {
        for(int i = 0; i < taskNodes->getLength(); i++)
        {
            DOMElement *curElem = (DOMElement*)taskNodes->item(i);
            if(strcmp(XMLString::transcode(curElem->getAttribute(XMLString::transcode("id"))),id.c_str()) == 0)
            {
                std::cout<<id<<" task found"<<std::endl;
                return curElem;
            }
        }
    }
    std::cout<<id<<" task not found"<<std::endl;
    return NULL;
}
