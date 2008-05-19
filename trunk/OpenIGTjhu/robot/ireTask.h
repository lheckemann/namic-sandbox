// $Id: ireTask.h,v 1.2 2006/06/03 00:38:56 kapoor Exp $

#ifndef _ireTask_h
#define _ireTask_h

#include <cisstRealTime/rtsTask.h>
#include <string>

class ireTask: public rtsTask {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    std::string StartupString;
    // Constructor.  Call ireFrameWork::InitShell() so that we can use
    // ireFramework::IsInitialized() to determine when the IRE has been
    // exited (and ireFramework::FinalizeShell() was called).
    ireTask(const char *name);
    ~ireTask();

    virtual void Run(void);
    virtual void Cleanup(void);
    virtual void Startup(void);
    virtual void Configure(const char *filename);

    bool IsActive(void) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ireTask);

#endif // _ireTask_h
