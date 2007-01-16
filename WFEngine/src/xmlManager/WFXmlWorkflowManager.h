#ifndef WFXMLWORKFLOWMANAGER_H_
#define WFXMLWORKFLOWMANAGER_H_

#include "WFXmlManager.h"
#include "WFStepObject.h"
using namespace WFEngine::nmWFXmlManager;

namespace WFEngine
{
    
 namespace nmWFXmlWorkflowManager
 {
     class WFEngine::nmWFStepObject::WFStepObject;
     
     class WFXmlWorkflowManager : public WFXmlManager
     {
     public:
         static WFXmlWorkflowManager* New();
         
         bool IsLoaded();
                  
     protected:
         WFXmlWorkflowManager();
         virtual ~WFXmlWorkflowManager();
         
         WFEngine::nmWFStepObject::WFStepObject *getNextWorkstepDescription(WFEngine::nmWFStepObject::WFStepObject *curWFStep);
         int loadWorkflowDescription(std::string &wfXmlFile);
         std::string getWorkflowName();
         std::string getWorkflowDescription();
         DOMElement *getParentWFElement();
         DOMElement *getMetadataWFElement();
         
         DOMElement *getElementFromID(std::string &id);
         
         void destroy();
     private:
         bool m_isLoaded;
         
         std::string specification;
         std::string decomposition;
         std::string schema;
         std::string processControlElements;
         std::string metaData;
         
         DOMElement *m_parentWFElement;
         
         
     };
 }

}
 
#endif /*WFXMLWORKFLOWMANAGER_H_*/
