#ifndef WFXMLWORKFLOWMANAGER_H_
#define WFXMLWORKFLOWMANAGER_H_

#include "WFXmlManager.h"

using namespace WFEngine::nmWFXmlManager;

namespace WFEngine
{
    
 namespace nmWFXmlConfigManager
 {
     class WFXmlWorkflowManager : public WFXmlManager
     {
     public:
         static WFXmlWorkflowManager* New(); 
         void getNextWorkstepDescription();
         void loadWorkflowDescription(std::string &wfXmlFile);
         bool IsLoaded();
                  
     protected:
         WFXmlWorkflowManager();
         virtual ~WFXmlWorkflowManager();
         
         int loadXmlFile(std::string &xmlFileName);        
     private:
         bool m_isLoaded;
     };
 }

}
 
#endif /*WFXMLWORKFLOWMANAGER_H_*/
