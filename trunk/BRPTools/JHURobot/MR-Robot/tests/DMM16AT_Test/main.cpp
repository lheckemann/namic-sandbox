/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#define IREDEF 1

// system includes
#include <sys/io.h>
// include what is needed from cisst
#include <cisstCommon.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstInteractive/ireFramework.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>

//GSF
#include <devDMM16AT.h>

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
    cmnLogger::SetMaskClass("cmnXMLPath", CMN_LOG_ALLOW_ERRORS);
    cmnLogger::SetMaskClass("devDMM16AT", CMN_LOG_ALLOW_ALL);

    //GSF
    CMN_LOG_INIT_VERBOSE << "*** Creating DMM16AT device ***" << std::endl;
    std::string DMMDev = "DMM16AT";
    mtsComponent *device = new devDMM16AT(DMMDev.c_str());
   
    std::string DMMConfigFile = "./DMM16AT_TestXMLConfig/DMM16AT_Test.xml";
    CMN_LOG_INIT_VERBOSE << "*** Configure using config file " << DMMConfigFile << " ***" << std::endl;
    device->Configure(DMMConfigFile.c_str());

    //GSF
    mtsInterfaceProvided* prov = device->GetInterfaceProvided("MainInterface");
    mtsCommandRead* GetInputAll = prov->GetCommandRead("GetInputAll");

    CMN_LOG_INIT_VERBOSE << "*** Registering DMM16AT device1 ***" << std::endl;
    // It is easiest to add the device to the Task Manager and then access it
    // from Python via the Task Manager.
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    CMN_LOG_INIT_VERBOSE << "*** Registering DMM16AT device2 ***" << std::endl;
    taskManager->AddComponent(device);
    CMN_LOG_INIT_VERBOSE << "*** Registering DMM16AT device3 ***" << std::endl;
    cmnObjectRegister::Register("TaskManager", taskManager);

#if IREDEF
    CMN_LOG_INIT_VERBOSE << "*** Launching IRE shell (C++ Thread) ***" << std::endl;
    IreLaunch IRE;
    osaThread IreThread;
    IreThread.Create<IreLaunch, char *>(&IRE, &IreLaunch::Run,
              "from cisstMultiTaskPython import *;"
              "TaskManager = cmnObjectRegister.FindObject('TaskManager');"
              "ADC = TaskManager.GetDevice('DMM16AT').GetInterfaceProvided('MainInterface');"
              "ADC.UpdateFromC();"
              "print 'call ADC.GetInputAll()'");

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
