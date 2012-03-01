// $Id: ireTask.cpp,v 1.2 2006/06/03 00:38:56 kapoor Exp $
#include "ireTask.h"
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstInteractive/ireFramework.h>

CMN_IMPLEMENT_SERVICES(ireTask);

ireTask::ireTask(const std::string & name):
    mtsTaskContinuous(name, 16) {
    CMN_LOG_CLASS_INIT_VERBOSE << "ireFramework::Instance" << std::endl;
    ireFramework::Instance();
}

ireTask::~ireTask() {
    CMN_LOG_CLASS_INIT_VERBOSE << "ireFramework::FinalizeShell" << std::endl;
    ireFramework::FinalizeShell();
}

void ireTask::Run(void) {
    static long ticks = 0;
    try {
        CMN_LOG_CLASS_INIT_VERBOSE << "ire Run called: " << ticks << std::endl;
        osaSleep(0.1);
        ticks++;
        ireFramework::LaunchIREShell(const_cast<char*>(StartupString.c_str()), false);
        //this->Kill(); // else the thread would restart
    }
    catch (...) {
        CMN_LOG_CLASS_INIT_ERROR << "*** ERROR:  could not launch IRE shell ***" << std::endl;
    }
}

void ireTask::Startup(void) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup called" << std::endl;
}

void ireTask::Cleanup(void) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Cleanup called" << std::endl;
}

void ireTask::Configure(const std::string & filename) {
}

void *ireTask::RunInternal(void* argument)
{
  this->Run();
  return 0;
}

void ireTask::Create(void *data) {
  CMN_LOG_CLASS_INIT_VERBOSE << "IRE Create called" << std::endl
                             << "Creating thread for task " << Name << std::endl;
  // Lock the StateChange mutex and unlock it when the thread starts running (in RunInternal)
  //StateChange.Lock();
  State = mtsComponentState::INITIALIZING;
  Thread.Create<ireTask, void*>(this, &ireTask::RunInternal, data);
}

bool ireTask::IsActive() const {
    return (ireFramework::IsInitialized());
}

