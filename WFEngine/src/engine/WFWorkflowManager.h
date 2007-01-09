#ifndef WFWORKFLOWMANAGER_H_
#define WFWORKFLOWMANAGER_H_

#include "WFXmlWorkflowManager.h"
#include "WFStepObject.h"

#include <string>

namespace WFEngine
{
    namespace nmWFWorkflowManager
    {
        class WFWorkflowManager
        {
        public:
            static WFWorkflowManager *New();
            
            int SetWorkflowFile(std::string wfDescFile);
            std::string GetWorkflowFile();
            
            std::string GetWorkflowName();
            
            void Close();
            
            bool IsLoaded();
        protected:
            WFWorkflowManager();
            virtual ~WFWorkflowManager();
            
            int LoadWorkflowFile(std::string wfDescFile);
            
        private:
            bool m_isLoaded;
            std::string m_wfDescFile;
            WFEngine::nmWFXmlWorkflowManager::WFXmlWorkflowManager *m_WFXmlWorkflowManager;
            
        };
    }
}

#endif /*WFWORKFLOWMANAGER_H_*/
