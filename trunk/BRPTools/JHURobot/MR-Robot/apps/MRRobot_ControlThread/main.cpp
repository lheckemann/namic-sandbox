
/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/* ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab: */
/* -*- Mode: C++; tab-width:4 -*- */
//$ Id: $

#include <cisstCommon.h>
#include <cisstCommon/cmnXMLPath.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>
#include <sawLoPoMoCo/mtsLoPoMoCo.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ctfMainUIThread.h"
#include "ctfControlThread.h"
#include "devDMM16AT.h"
#include "devRMM1612.h"

//#define REMOVEdevLoPoMoCo
#if defined (REMOVEdevLoPoMoCo)
#warning Removed devLoPoMoCo
#endif

using namespace std;

void parseInputArgument(std::string &inputArgument, std::string &relativeFilePath, std::string &fileName)
{
    unsigned int posSeperator = inputArgument.find('/', 0);
    while (posSeperator != inputArgument.npos) {
        relativeFilePath = inputArgument.substr(0, posSeperator);
        fileName = inputArgument.substr(posSeperator+1, inputArgument.size());
        posSeperator = inputArgument.find('/', posSeperator+1); // find the next '/'
    }
    relativeFilePath += "/"; // append a '/' at the end
}

int checkfile(const std::string & fname)
{
    struct stat filebuf;
    if (stat(fname.c_str(), &filebuf) < 0) {
        CMN_LOG_INIT_ERROR << "MAIN: Could not configure MAIN task, configuration -" << fname << "- file missing!" <<std::endl;
        return -3;
    }
    return 10;
}

int main(int argc, char** argv)
//int main(int argc)
{
    const double UIperiod = 20 * cmn_ms;
    const double ServoPeriod = 1 * cmn_ms;

    // ----------------------------------------------------------------------
    // Set the Levels of Detail for the log
    // ----------------------------------------------------------------------
    // Set the logger LoD to high
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    // Add cout with only errors
    cmnLogger::AddChannel(cout, CMN_LOG_ALLOW_ALL);
    // Change default log level to 10
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(CMN_LOG_ALLOW_ALL);
    // Set LoD high for some classes
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("cmnXMLPath", CMN_LOG_ALLOW_ERRORS);
    //cmnLogger::SetMaskClass("mtsLoPoMoCo", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("ctfControlThread", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("ctfMainUIThread", CMN_LOG_ALLOW_ALL);

    // ----------------------------------------------------------------------
    // Test if this programs (user) has the right permissions
    // ----------------------------------------------------------------------
    if (iopl(3) < 0) {
        CMN_LOG_INIT_ERROR << "MAIN: Couldn't get port permissions" << endl;
        return 2;
    }
   
    /*
    if (argc != 2) {
        CMN_LOG_INIT_ERROR << "controlThreadFLTK: incorrect usage" << std::endl;
        CMN_LOG_INIT_ERROR << "usage: " << argv[1] << " xml-config-filename" << std::endl;
        return -4;
    }
    if (checkfile(argv[1]) < 0) {
        return -3;
    }
*/

    char currentWorkingDir[256];
    if (getcwd(currentWorkingDir, 255)<0) {
        CMN_LOG_INIT_ERROR << "MAIN: Couldn't get current working directory" << std::endl;
    }

    //HARDCODE MAIN CONFIG FILE HERE
    //std::string inputArgument(argv[1]), relativeFilePath, fileName;
    char filename[] = "./MRRobot_ControlThreadXMLConfig/MRRobot_Main.xml";
    //char filename[] = "./MRRobot_ControlThread/XMLConfig/MRRobot_Main.xml";
    std::string inputArgument(filename), relativeFilePath, fileName;


    CMN_LOG_INIT_VERBOSE << "Current working directory: " << currentWorkingDir << std::endl;
    CMN_LOG_INIT_VERBOSE << "Input Argument: " << inputArgument << std::endl;
    parseInputArgument(inputArgument, relativeFilePath, fileName);
    CMN_LOG_INIT_VERBOSE << "Main File Name: " << fileName << std::endl;
    CMN_LOG_INIT_VERBOSE << "Relative Path: " << relativeFilePath << std::endl;

    // ----------------------------------------------------------------------
    // Create the task manager and the tasks
    // ----------------------------------------------------------------------
    cmnXMLPath xmlConfig;

    //xmlConfig.SetInputSource(argv[1]);
    xmlConfig.SetInputSource(filename);

    char path[40], context[40];
    std::string FLTKConfigFile, ServoConfigFile;
    std::string FLTKConfigDev, ServoConfigDev;
    
    CMN_LOG_INIT_VERBOSE << "Configuring MAIN" << std::endl;
    
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "FLTKConfig/@FileName", FLTKConfigFile);
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "FLTKConfig/@DevName", FLTKConfigDev);
    FLTKConfigFile.insert(0, relativeFilePath);
    CMN_LOG_INIT_VERBOSE << FLTKConfigDev << " " << FLTKConfigFile << std::endl;
    if (checkfile(FLTKConfigFile) < 0) return -3;
    
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "ServoConfig/@FileName", ServoConfigFile);
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "ServoConfig/@DevName", ServoConfigDev);
    ServoConfigFile.insert(0, relativeFilePath);
    CMN_LOG_INIT_VERBOSE << ServoConfigDev << " " << ServoConfigFile << std::endl;
    if (checkfile(ServoConfigFile) < 0) return -3;

#if 0
    // PKAZ: I think the following config file is obsolete.  It was originally intended to configure
    //       the CompositeMRRobotController, but the latest version of that class no longer uses it
    //       (instead, it hard-coded the config files for the LoPoMoCo, ADC, and DAC devices).
    //       We are no longer using a composite device, so those three individual config files are
    //       passed directly to each device (see below).
    std::string LoPoMoCoConfigFile, LoPoMoCoConfigDev;
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "LoPoMoCoConfig/@FileName", LoPoMoCoConfigFile);
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "LoPoMoCoConfig/@DevName", LoPoMoCoConfigDev);
    LoPoMoCoConfigFile.insert(0, relativeFilePath);
    CMN_LOG_INIT_VERBOSE << LoPoMoCoConfigDev << " " << LoPoMoCoConfigFile << std::endl;
    if (checkfile(LoPoMoCoConfigFile) < 0) return -3;
#endif

    // ----------------------------------------------------------------------
    // Create the task manager and the tasks
    // ----------------------------------------------------------------------

    mtsTaskManager *taskManager = mtsTaskManager::GetInstance();

    CMN_LOG_INIT_VERBOSE << "MAIN: Creating Main UI Thread " << FLTKConfigDev << std::endl;
    ctfMainUIThread *mainUIThread = new ctfMainUIThread(FLTKConfigDev, UIperiod);
    mainUIThread->Configure(FLTKConfigFile);

    mtsTaskPeriodic *controlThread = 0;
    CMN_LOG_INIT_VERBOSE << "MAIN: Creating Voltage Controller " << ServoConfigDev << std::endl;
    controlThread = new ctfControlThread(ServoConfigDev, ServoPeriod);
    controlThread->Configure(ServoConfigFile);

    // PKAZ: note below that the XML config filenames are hard-coded. Probably, these should be moved
    //       to MRRobot_Main.xml
    
#if !defined (REMOVEdevLoPoMoCo)
    CMN_LOG_INIT_VERBOSE << "MAIN: Creating LoPoMoCo device" << std::endl;

    mtsComponent *loPoMoCo = new mtsLoPoMoCo("LoPoMoCo", 1); // Composite device was "MRRobot_Hardware"
    loPoMoCo->Configure("./LoPoMoCoTestXMLConfig/LoPoMoCo_Test.xml");
#endif

    CMN_LOG_INIT_VERBOSE << "MAIN: Creating ADC board (DMM16AT) device" << std::endl;
    mtsComponent *ADC_board = new devDMM16AT("DMM16AT");
    ADC_board->Configure("./DMM16AT_TestXMLConfig/DMM16AT_Test.xml");
 
    CMN_LOG_INIT_VERBOSE << "MAIN: Creating DAC board (RMM1612) device" << std::endl;
    // Specify false in constructor so that dscud5 initialization isn't done twice
    mtsComponent *DAC_board = new devRMM1612("RMM1612", false);
    DAC_board->Configure("./RMM1612_TestXMLConfig/RMM1612_Test.xml");
   
    CMN_LOG_INIT_VERBOSE << "controlThreadFLTK: All objects created and configured." << endl;

    cmnObjectRegister::Register("TaskManager", taskManager);
    //cmnObjectRegister::Register("servo", controlThread);

    // Add the tasks to the task manager
    taskManager->AddComponent(mainUIThread);
    taskManager->AddComponent(controlThread);
#if !defined (REMOVEdevLoPoMoCo)
    taskManager->AddComponent(loPoMoCo);
#endif
    taskManager->AddComponent(ADC_board);
    taskManager->AddComponent(DAC_board);
    CMN_LOG_INIT_VERBOSE << "controlThreadFLTK: All tasks added to the task manager." << endl;

    // Connect Controller (servo) to the hardware (LoPoMoCo, ADC, DAC)
    taskManager->Connect(ServoConfigDev, "LoPoMoCo", "LoPoMoCo", "WriteInterface");
    taskManager->Connect(ServoConfigDev, "ADC", "DMM16AT", "MainInterface");
    taskManager->Connect(ServoConfigDev, "DAC", "RMM1612", "MainInterface");

    // Connect UI Thread to Controller (servo)
    taskManager->Connect(FLTKConfigDev, "Controller", ServoConfigDev, "MainInterface");

    CMN_LOG_INIT_VERBOSE << "controlThreadFLTK: All object interfaces connected." << endl;

    // Start all tasks here
    taskManager->CreateAll();
    taskManager->StartAll();
    CMN_LOG_INIT_VERBOSE << "controlThreadFLTK: All tasks started." << endl;

    osaSleep(0.5);
    // The FLTK event loop will manage the GUI events
    while (1) {
        if (mainUIThread->GetExitFlag()) break;
        osaSleep(UIperiod);
    }
    CMN_LOG_INIT_VERBOSE << "controlThreadFLTK: FLTK loop ended." << std::endl;
    osaSleep(UIperiod * 2);
    mainUIThread->Kill();
    controlThread->Kill();
    osaSleep(UIperiod * 2);

    while (!mainUIThread->IsTerminated()) osaSleep(UIperiod);
    while (!controlThread->IsTerminated()) osaSleep(UIperiod);
    CMN_LOG_INIT_VERBOSE << "controlThreadFLTK: Quitting." << std::endl;

    //delete mainUIThread;
    //delete controlThread;
    return 0;
}
