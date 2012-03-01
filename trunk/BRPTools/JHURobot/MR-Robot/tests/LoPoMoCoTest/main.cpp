/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */


//#define REMOVEdevLoPoMoCo
#if defined (REMOVEdevLoPoMoCo)
#warning Removed devLoPoMoCo
#endif

#define IREDEF 1

// system includes
#include <sys/io.h>
// include what is needed from cisst
#include <cisstCommon.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstInteractive/ireFramework.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>

//GSF
#include <sawLoPoMoCo/mtsLoPoMoCo.h>

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
#if !defined (REMOVEdevLoPoMoCo)
    iopl(3);

    // add cout for all log
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("cmnXMLPath", CMN_LOG_ALLOW_ERRORS);
    cmnLogger::SetMaskClass("mtsLoPoMoCo", CMN_LOG_ALLOW_ALL);

    ///////////////  GSF ADDED FOR SINGLE BOARD
    //GSF
    CMN_LOG_INIT_VERBOSE << "*** Creating LoPoMoCo device ***" << std::endl;
    std::string LoPoMoCoDev = "LoPoMoCoTestBoard";
    mtsComponent *device = new mtsLoPoMoCo(LoPoMoCoDev.c_str(), 1); // 1 LoPoMoCo board
   
    std::string LoPoMoCoConfigFile = "./LoPoMoCoTestXMLConfig/LoPoMoCo_Test.xml";
    CMN_LOG_INIT_VERBOSE << "*** Configure using config file " << LoPoMoCoConfigFile << " ***" << std::endl;
    device->Configure(LoPoMoCoConfigFile.c_str());

    //////////////
    //LoPoMoCo
    mtsInterfaceProvided* prov = device->GetInterfaceProvided("WriteInterface");
    if (prov) {
        mtsCommandVoid* LatchEncoders = prov->GetCommandVoid("LatchEncoders");
        mtsCommandVoid* StartMotorCurrentConv = prov->GetCommandVoid("StartMotorCurrentConv");
        mtsCommandVoid* StartPotFeedbackConv = prov->GetCommandVoid("StartPotFeedbackConv");

        mtsCommandRead* GetPositions = prov->GetCommandRead("GetPositions");
        mtsCommandRead* GetVelocities = prov->GetCommandRead("GetVelocities");
        mtsCommandRead* GetMotorCurrents = prov->GetCommandRead("GetMotorCurrents");
        mtsCommandRead* GetPotFeedbacks = prov->GetCommandRead("GetPotFeedbacks");

        mtsCommandWriteBase* SetMotorVoltages = prov->GetCommandWrite("SetMotorVoltages");
        mtsCommandWriteBase* SetCurrentLimits = prov->GetCommandWrite("SetCurrentLimits");
        mtsCommandWriteBase* Enable = prov->GetCommandWrite("Enable");
        mtsCommandWriteBase* Disable = prov->GetCommandWrite("Disable");
        mtsCommandWriteBase* ResetEncoders = prov->GetCommandWrite("ResetEncoders");
        mtsCommandWriteBase* SetPositions = prov->GetCommandWrite("SetPositions");

        mtsCommandVoid* LoadMotorVoltages = prov->GetCommandVoid("LoadMotorVoltages");
        mtsCommandVoid* LoadCurrentLimits = prov->GetCommandVoid("LoadCurrentLimits");
        mtsCommandVoid* LoadMotorVoltagesCurrentLimits = prov->GetCommandVoid("LoadMotorVoltagesCurrentLimits");

        mtsCommandQualifiedRead* FrequencyToRPS = prov->GetCommandQualifiedRead("FrequencyToRPS");
        mtsCommandQualifiedRead* ADCToMotorCurrents = prov->GetCommandQualifiedRead("ADCToMotorCurrents");
        mtsCommandQualifiedRead* ADCToPotFeedbacks = prov->GetCommandQualifiedRead("ADCToPotFeedbacks");
        mtsCommandQualifiedRead* MotorVoltagesToDAC = prov->GetCommandQualifiedRead("MotorVoltagesToDAC");
        mtsCommandQualifiedRead* CurrentLimitsToDAC = prov->GetCommandQualifiedRead("CurrentLimitsToDAC");

        CMN_LOG_INIT_DEBUG
            << *LatchEncoders << std::endl
            << *StartMotorCurrentConv << std::endl
            << *StartPotFeedbackConv << std::endl
            << *GetPositions << std::endl
            << *GetVelocities << std::endl
            << *GetMotorCurrents << std::endl
            << *GetPotFeedbacks << std::endl
            << *SetMotorVoltages << std::endl
            << *SetCurrentLimits << std::endl
            << *Enable << std::endl
            << *Disable << std::endl
            << *ResetEncoders << std::endl
            << *SetPositions << std::endl
            << *LoadMotorVoltages << std::endl
            << *LoadCurrentLimits << std::endl
            << *LoadMotorVoltagesCurrentLimits << std::endl
            << *FrequencyToRPS << std::endl
            << *ADCToMotorCurrents << std::endl
            << *ADCToPotFeedbacks << std::endl
            << *MotorVoltagesToDAC << std::endl
            << *CurrentLimitsToDAC << std::endl;
    }


    CMN_LOG_INIT_VERBOSE << "*** Registering LoPoMoCo device ***" << std::endl;
    // It is easiest to add the device to the Task Manager and then access it
    // from Python via the Task Manager.
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddComponent(device);
    cmnObjectRegister::Register("TaskManager", taskManager);

#if IREDEF
    CMN_LOG_INIT_VERBOSE << "*** Launching IRE shell (C++ Thread) ***" << std::endl;
    IreLaunch IRE;
    osaThread IreThread;
    IreThread.Create<IreLaunch, char *>(&IRE, &IreLaunch::Run,
              "from cisstMultiTaskPython import *;"
              "TaskManager = cmnObjectRegister.FindObject('TaskManager');"
              "LoPoMoCo = TaskManager.GetDevice('LoPoMoCo').GetProvidedInterface('WriteInterface');"
              "LoPoMoCo.UpdateFromC();"
              "print 'call LoPoMoCo.Command(...)';"
              "print '(parameter from LoPoMoCo.Command.GetArgumentPrototype()'");

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
#endif
}
