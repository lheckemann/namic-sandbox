#ifndef WFXMLWORKFLOWMANAGER_H_
#define WFXMLWORKFLOWMANAGER_H_

#include "WFXmlManager.h"

using namespace WFEngine::nmWFXmlManager;

namespace WFEngine
{
    
 namespace nmWFXmlWorkflowManager
 {
     class WFXmlWorkflowManager : public WFXmlManager
     {
     public:
         static WFXmlWorkflowManager* New(); 
         void getNextWorkstepDescription();
         int loadWorkflowDescription(std::string &wfXmlFile);
         bool IsLoaded();
         
         std::string getWorkflowName();
         void destroy();
     protected:
         WFXmlWorkflowManager();
         virtual ~WFXmlWorkflowManager();
                 
     private:
         bool m_isLoaded;
         
         std::string specification;
         std::string decomposition;
         std::string schema;
         
     };
 }

}
 
#endif /*WFXMLWORKFLOWMANAGER_H_*/
