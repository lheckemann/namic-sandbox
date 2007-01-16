#include "WFDirectInterface.h"
//#include "WFWorkflowManager.h"

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "WFStepObject.h"

using namespace WFEngine;
using namespace WFEngine::nmWFStepObject;

WFDirectInterface::WFDirectInterface()
{
    this->m_isLoaded = false;
}

WFDirectInterface::~WFDirectInterface()
{
}

WFDirectInterface *WFDirectInterface::New()
{
    return new WFDirectInterface;
}

std::vector<WFDirectInterface::workflowDesc*> WFDirectInterface::GetAllKnownWorkflows()
{
    using namespace WFEngine::nmWFWorkflowManager;
    
    std::cout<<"GetAllKnownWorkflows"<<std::endl;
    
    std::vector<std::map<std::string, std::string> > knownWFs;
    std::vector<WFDirectInterface::workflowDesc*> myKnownWFs;
    WFDirectInterface::workflowDesc *myWFDesc;    
    
    knownWFs = this->Superclass::m_wfeOpts->GetKnownWorkflows();
    
    std::vector< std::map<std::string,std::string> >::iterator iter;
    for(iter = knownWFs.begin(); iter != knownWFs.end(); iter++)
    {
      std::map<std::string, std::string> attrMap = (*iter);
      
      myWFDesc = new WFDirectInterface::workflowDesc;
      myWFDesc->fileName = attrMap["fileName"];
      
      WFWorkflowManager *wfXmlWFMgr = WFWorkflowManager::New();
      wfXmlWFMgr->SetWorkflowFile(attrMap["fileName"]);
      
      myWFDesc->workflowName = wfXmlWFMgr->GetWorkflowName();
      myWFDesc->createTime = this->getModifiedDateFromFile(attrMap["fileName"]);
//      int i = 1000000000;
//      myWFDesc.createTime = i;
      std::cout<<"test"<<std::endl;
      myKnownWFs.push_back(myWFDesc); 
    }
    
    return myKnownWFs;
}

int WFDirectInterface::InitializeWFEngine()
{
    return this->Superclass::InitializeWFEngine("");
}

void WFDirectInterface::SaveState()
{
    this->Superclass::SaveState();
}

int WFDirectInterface::getModifiedDateFromFile(std::string fileName)
{
    struct stat curStat;
    stat(fileName.c_str(), &curStat);
    return curStat.st_mtime;
}

int WFDirectInterface::loadWorkflowFromFile(std::string fileName)
{
    using namespace WFEngine::nmWFWorkflowManager;
    
    this->m_wfMgr = WFWorkflowManager::New();
    
    if(m_wfMgr->SetWorkflowFile(fileName))
        m_isLoaded = true;
    
    return m_isLoaded;    
}

void WFDirectInterface::CloseWorkflowManager()
{
    if(IsLoaded())
        this->m_wfMgr->Close();
}

int WFDirectInterface::IsLoaded()
{
    return this->m_isLoaded;
}

WFStepObject *WFDirectInterface::getNextWorkStep()
{
    return this->m_wfMgr->GetNextWFStep();
}

WFStepObject *WFDirectInterface::getBackWorkStep()
{
    this->m_wfMgr->GetPreviousWFStep();
}

