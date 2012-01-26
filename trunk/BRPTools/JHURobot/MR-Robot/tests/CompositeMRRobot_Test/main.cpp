/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

//************************************************************************************
// PKAZ: This used to be a test program for the Composite Robot device, which was
//       a composite of LoPoMoCo, ADC (DMM16AT), and DAC (RMM1612). Now, it is just
//       a test of all the individual devices.
//       Anyway, it seems overkill to have separate test programs for each device -- 
//       the DMM16AT_Test, RMM1612_Test, and LoPoMoCoTest could be eliminated since
//       all functionality is combined here.
//************************************************************************************

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
#include <devDMM16AT.h>
#include <devRMM1612.h>
#include <cisstDevices/devLoPoMoCo.h>

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
    cmnClassRegister::SetLoD("devDMM16AT", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devRMM1612", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devLoPoMoCo", CMN_LOG_LOD_VERY_VERBOSE);

    // PKAZ: Previously, this file created a CompositeDevice, which is not supported
    //       in cisstMultiTask.  I think it ignored the CompositeMRRobot_Test.xml config
    //       file anyway, and used hard-coded XML file names.

    mtsProvidedInterface* prov;

    //ADC
    CMN_LOG_INIT_VERBOSE << "*** Creating DMM16AT device ***" << std::endl;
    std::string DMMDev = "DMM16AT";
    mtsDevice *ADC_Device = new devDMM16AT(DMMDev.c_str());
    std::string DMMConfigFile = "./DMM16AT_TestXMLConfig/DMM16AT_Test.xml";
    CMN_LOG_INIT_VERBOSE << "*** Configure using config file " << DMMConfigFile << " ***" << std::endl;
    ADC_Device->Configure(DMMConfigFile.c_str());
    prov = ADC_Device->GetProvidedInterface("MainInterface");
    if (prov) {
       mtsCommandReadBase* GetInputAll = prov->GetCommandRead("GetInputAll");
       CMN_LOG_INIT_DEBUG << *GetInputAll << std::endl;
    }
   
    //DAC
    CMN_LOG_INIT_VERBOSE << "*** Creating RMM1612 device ***" << std::endl;
    std::string RMMDev = "RMM1612";
    mtsDevice *DAC_Device = new devRMM1612(RMMDev.c_str());
    std::string RMMConfigFile = "./RMM1612_TestXMLConfig/RMM1612_Test.xml";
    CMN_LOG_INIT_VERBOSE << "*** Configure using config file " << RMMConfigFile << " ***" << std::endl;
    DAC_Device->Configure(RMMConfigFile.c_str());
    prov = DAC_Device->GetProvidedInterface("MainInterface");
    if (prov) {
       mtsCommandWriteBase* SetOutputAll = prov->GetCommandWrite("SetOutputAll");
       CMN_LOG_INIT_DEBUG << *SetOutputAll << std::endl;
    }

    //LoPoMoCo
    CMN_LOG_INIT_VERBOSE << "*** Creating devLoPoMoCo device ***" << std::endl;
    std::string LoPoMoCoDev = "LoPoMoCo";
    mtsDevice *LoPoMoCo_Device = new devLoPoMoCo(LoPoMoCoDev.c_str(), 1); // 1 LoPoMoCo board
    std::string LoPoMoCoConfigFile = "./LoPoMoCoTestXMLConfig/LoPoMoCo_Test.xml";
    CMN_LOG_INIT_VERBOSE << "*** Configure using config file " << LoPoMoCoConfigFile << " ***" << std::endl;
    LoPoMoCo_Device->Configure(LoPoMoCoConfigFile.c_str());
    prov = LoPoMoCo_Device->GetProvidedInterface("WriteInterface");
    if (prov) {
        mtsCommandVoidBase* LatchEncoders = prov->GetCommandVoid("LatchEncoders");
        mtsCommandVoidBase* StartMotorCurrentConv = prov->GetCommandVoid("StartMotorCurrentConv");
        mtsCommandVoidBase* StartPotFeedbackConv = prov->GetCommandVoid("StartPotFeedbackConv");

        mtsCommandReadBase* GetPositions = prov->GetCommandRead("GetPositions");
        mtsCommandReadBase* GetVelocities = prov->GetCommandRead("GetVelocities");
        mtsCommandReadBase* GetMotorCurrents = prov->GetCommandRead("GetMotorCurrents");
        mtsCommandReadBase* GetPotFeedbacks = prov->GetCommandRead("GetPotFeedbacks");

        mtsCommandWriteBase* SetMotorVoltages = prov->GetCommandWrite("SetMotorVoltages");
        mtsCommandWriteBase* SetCurrentLimits = prov->GetCommandWrite("SetCurrentLimits");
        mtsCommandWriteBase* Enable = prov->GetCommandWrite("Enable");
        mtsCommandWriteBase* Disable = prov->GetCommandWrite("Disable");
        mtsCommandWriteBase* ResetEncoders = prov->GetCommandWrite("ResetEncoders");
        mtsCommandWriteBase* SetPositions = prov->GetCommandWrite("SetPositions");

        mtsCommandVoidBase* LoadMotorVoltages = prov->GetCommandVoid("LoadMotorVoltages");
        mtsCommandVoidBase* LoadCurrentLimits = prov->GetCommandVoid("LoadCurrentLimits");
        mtsCommandVoidBase* LoadMotorVoltagesCurrentLimits = prov->GetCommandVoid("LoadMotorVoltagesCurrentLimits");

        mtsCommandQualifiedReadBase* FrequencyToRPS = prov->GetCommandQualifiedRead("FrequencyToRPS");
        mtsCommandQualifiedReadBase* ADCToMotorCurrents = prov->GetCommandQualifiedRead("ADCToMotorCurrents");
        mtsCommandQualifiedReadBase* ADCToPotFeedbacks = prov->GetCommandQualifiedRead("ADCToPotFeedbacks");
        mtsCommandQualifiedReadBase* MotorVoltagesToDAC = prov->GetCommandQualifiedRead("MotorVoltagesToDAC");
        mtsCommandQualifiedReadBase* CurrentLimitsToDAC = prov->GetCommandQualifiedRead("CurrentLimitsToDAC");

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

    CMN_LOG_INIT_VERBOSE << "*** Registering all devices ***" << std::endl;
    // It is easiest to add the devices to the Task Manager and then access them
    // from Python via the Task Manager.
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddDevice(ADC_Device);
    taskManager->AddDevice(DAC_Device);
    taskManager->AddDevice(LoPoMoCo_Device);
    cmnObjectRegister::Register("TaskManager", taskManager);

#if IREDEF
    CMN_LOG_INIT_VERBOSE << "*** Launching IRE shell (C++ Thread) ***" << std::endl;
    IreLaunch IRE;
    osaThread IreThread;
    IreThread.Create<IreLaunch, char *>(&IRE, &IreLaunch::Run,
              "from cisstMultiTaskPython import *;"
              "TaskManager = cmnObjectRegister.FindObject('TaskManager');"
              "ADC = TaskManager.GetDevice('DMM16AT').GetProvidedInterface('MainInterface');"
              "ADC.UpdateFromC();"
              "DAC = TaskManager.GetDevice('RMM1612').GetProvidedInterface('MainInterface');"
              "DAC.UpdateFromC();"
              "LoPoMoCo = TaskManager.GetDevice('LoPoMoCo').GetProvidedInterface('WriteInterface');"
              "LoPoMoCo.UpdateFromC();"
              "print 'Use ADC, DAC, and LoPoMoCo devices'");

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
