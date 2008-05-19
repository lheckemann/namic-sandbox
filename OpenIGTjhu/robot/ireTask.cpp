// $Id: ireTask.cpp,v 1.2 2006/06/03 00:38:56 kapoor Exp $
#include "ireTask.h"
#include <cisstOSAbstraction/osaTime.h>
#include <cisstInteractive/ireFramework.h>

CMN_IMPLEMENT_SERVICES(ireTask);

ireTask::ireTask(const char *name):
    rtsTask(name, 0, false, 16) {
        CMN_LOG(1) << "ireFramework::Instance" << std::endl;
        ireFramework::Instance();
    }

ireTask::~ireTask() {
    CMN_LOG(1) << "ireFramework::FinalizeShell" << std::endl;
    ireFramework::FinalizeShell();
}

void ireTask::Run(void) {
    static long ticks = 0;
    try {
        CMN_LOG(1) << "ire Run called :" << ticks << std::endl;
        osaTime::Sleep(100);
        ticks++;
        ireFramework::LaunchIREShell(const_cast<char*>(StartupString.c_str()), false);
        this->EndTask = true; // else the thread would restart
#if 0
        IreLaunch IRE;
        osaThread IreThread;
        IreThread.Create<IreLaunch, char *>(&IRE, &IreLaunch::Run, "from Sine6LibPython import *; objreg = cmnObjectRegister_Instance(); sineobj = objreg.FindObject(\"sineTask\")");
#endif
    }
    catch (...) {
        std::cout << "*** ERROR:  could not launch IRE shell ***" << std::endl;
    }
}

void ireTask::Startup(void) {
    CMN_LOG(1) << "ireTask Startup called" << std::endl;
    ireFramework::InitShell();
}

void ireTask::Cleanup(void) {
    CMN_LOG(1) << "ireTask Cleanup called" << std::endl;
}

void ireTask::Configure(const char *filename) {
}

bool ireTask::IsActive() const {
    return (ireFramework::IsInitialized());
}

