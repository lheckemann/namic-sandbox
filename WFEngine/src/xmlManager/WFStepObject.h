#ifndef WFSTEPOBJECT_H_
#define WFSTEPOBJECT_H_

#include <string>
#include <map>
#include <vector>

namespace WFEngine
{
    namespace nmWFStepObject
    {
        class WFStepObject
        {
        public:
            static WFStepObject* New();
            
            void SetID(std::string &ID);
            void SetName(std::string &name);
            void AddNextStepID(std::string &nextStepID);
            void SetVariableMapping(std::string &from, std::string &to);
            void SetDescription(std::string &stepDesc);
            
            std::string GetNextStepID();
            std::string GetID();
            std::string GetName();
            std::string GetDescription();
                                    
        protected:
            WFStepObject();
            virtual ~WFStepObject();
        private:
            std::string m_ID;
            std::string m_name;
            std::vector<std::string> m_nextSteps;
            std::map<std::string, std::string> m_varMapping;
            std::string m_desc;
        };
    }
}

#endif /*WFSTEPOBJECT_H_*/
