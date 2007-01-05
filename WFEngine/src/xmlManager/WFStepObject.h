#ifndef WFSTEPOBJECT_H_
#define WFSTEPOBJECT_H_

namespace WFEngine
{
    namespace nmWFStepObject
    {
        class WFStepObject
        {
        public:
            static WFStepObject* New();
        protected:
            WFStepObject();
            virtual ~WFStepObject();
        };
    }
}

#endif /*WFSTEPOBJECT_H_*/
