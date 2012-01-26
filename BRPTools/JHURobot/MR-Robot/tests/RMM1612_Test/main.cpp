/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#define IREDEF 1

// system includes
#include <sys/io.h>
// include what is needed from cisst
#include <cisstCommon.h>
#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstInteractive/ireFramework.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>

//GSF
#include <devRMM1612.h>

class IreLaunch {
    public:
        // Constructor.  Call ireFramework::InitShell() so that we can use
        // ireFramework::IsInitialized() to determine when the IRE has been
        // exited (and ireFramework::FinalizeShell() was called).
        IreLaunch() { ireFramework::Instance(); }
        ~IreLaunch() {}
        void *Run(char *startup) {
            try {
                ireFramework::LaunchIREShell(startup, false);
            }
            catch (...) {
                std::cout << "*** ERROR:  could not launch IRE shell ***" << std::endl;
            }
            ireFramework::FinalizeShell();
            return this;
        }
};


// main function
int main(int argc, char** argv) {
    iopl(3);


    // add cout for all log
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("cmnXMLPath", CMN_LOG_LOD_RUN_ERROR);
    cmnClassRegister::SetLoD("devRMM1612", CMN_LOG_LOD_VERY_VERBOSE);

    //GSF
    CMN_LOG_INIT_VERBOSE << "*** Creating RMM1612 device ***" << std::endl;
    std::string RMMDev = "RMM1612";
    mtsDevice *device = new devRMM1612(RMMDev.c_str());
   
    std::string RMMConfigFile = "./RMM1612_TestXMLConfig/RMM1612_Test.xml";
    CMN_LOG_INIT_VERBOSE << "*** Configure using config file " << RMMConfigFile << " ***" << std::endl;
    device->Configure(RMMConfigFile.c_str());
    
    // GSF
    mtsProvidedInterface* prov = device->GetProvidedInterface("MainInterface");
    if (prov)
       mtsCommandWriteBase* SetOutputAll = prov->GetCommandWrite("SetOutputAll");

    CMN_LOG_INIT_VERBOSE << "*** Registering RMM device ***" << std::endl;
    // It is easiest to add the device to the Task Manager and then access it
    // from Python via the Task Manager.
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddDevice(device);
    cmnObjectRegister::Register("TaskManager", taskManager);

#if IREDEF
    CMN_LOG_INIT_VERBOSE << "*** Launching IRE shell (C++ Thread) ***" << std::endl;
    IreLaunch IRE;
    osaThread IreThread;
    IreThread.Create<IreLaunch, char *>(&IRE, &IreLaunch::Run,
              "from cisstMultiTaskPython import *;"
              "TaskManager = cmnObjectRegister.FindObject('TaskManager');"
              "DAC = TaskManager.GetDevice('RMM1612').GetProvidedInterface('MainInterface');"
              "DAC.UpdateFromC();"
              "parm = DAC.SetOutputAll.GetArgumentPrototype();"
              "print 'call DAC.SetOutputAll(parm)'");

    // Wait for IRE to initialize itself
    while (ireFramework::IsStarting())
        osaSleep(0.5 * cmn_s);  // Wait 0.5 seconds

    // wait for IRE to exit
    while (ireFramework::IsActive())
        osaSleep(0.5 * cmn_s);  // Wait 0.5 seconds

    IreThread.Wait();
#endif

    CMN_LOG_INIT_VERBOSE << "*** Clean up and exit ***" << std::endl;
    return 0;
}
