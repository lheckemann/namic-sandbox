#ifndef WFDIRECTINTERFACE_H_
#define WFDIRECTINTERFACE_H_

#define WFDIRECTINTERFACE

#include <string>
#include <vector>
#include "WFStepObject.h"
#include "WFXmlWorkflowManager.h"
#include "WFBaseEngine.h"

class WFDirectInterface : public WFEngine::WFBaseEngine
{
public:
    static WFDirectInterface *New();
    
    int InitializeWFEngine();
    
    typedef struct 
    {
        std::string fileName;
        std::string workflowName;
        int createTime; 
    } workflowDesc;
    
    std::vector<WFDirectInterface::workflowDesc*> GetAllKnownWorkflows();
    
    int loadWorkflowFromFile(std::string fileName);
    
    int IsLoaded();
    
    void SaveState();
    void CloseWorkflowManager();
protected:
    WFDirectInterface();
    virtual ~WFDirectInterface();
    
    int getModifiedDateFromFile(std::string fileName);
    typedef WFEngine::WFBaseEngine Superclass;
private:
    WFEngine::WFBaseEngine *m_wfBE;
    WFEngine::nmWFWorkflowManager::WFWorkflowManager *m_wfMgr;
    
    bool m_isLoaded;
};

#endif /*WFDIRECTINTERFACE_H_*/
