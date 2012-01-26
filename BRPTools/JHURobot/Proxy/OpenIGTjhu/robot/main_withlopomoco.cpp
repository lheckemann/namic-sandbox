/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/* ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab: */
/* -*- Mode: C++; tab-width:4 -*- */
//$ Id: $
#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstRealTime.h>
#include <cisstDeviceInterface.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ctfMainUIThread.h"
#include "ctfControlThread.h"
//#include "daVinciControlThread.h"
//#include "TwoSnake5BarControlThread.h"

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

int checkfile(const char *fname)
{
    struct stat filebuf;
    if (stat(fname, &filebuf) < 0) {
        CMN_LOG(1) << "MAIN: Could not configure MAIN task, configuration -" << fname << "- file missing!" <<std::endl;
        return -3;
    }
    return 10;
}

int main(int argc, char** argv)
{
    const double UIperiod = 20; // in milliseconds
    const double ServoPeriod = 1; // in milliseconds

    // ----------------------------------------------------------------------
    // Set the Levels of Detail for the log
    // ----------------------------------------------------------------------
    // Set the logger LoD to high
    cmnLogger::SetLoD(10);
    // Add cout with only errors
    cmnLogger::GetMultiplexer()->AddChannel(cout, 10);
    // Change default log level to 10
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(10);
    // Set LoD high for some classes
    cmnClassRegister::SetLoD("cmnXMLPath", 1);
    cmnClassRegister::SetLoD("ddiLoPoMoCo", 10);
    cmnClassRegister::SetLoD("ctfControlThread", 1);
    cmnClassRegister::SetLoD("ctfMainUIThread", 5);

    // ----------------------------------------------------------------------
    // Test if this programs (user) has the right permissions
    // ----------------------------------------------------------------------
    if (iopl(3) < 0) {
        CMN_LOG(1) << "MAIN: Couldn't get port permissions" << endl;
        return 2;
    }
   
    if (argc != 2) {
        CMN_LOG(1) << "controlThreadFLTK: incorrect usage" << std::endl;
        CMN_LOG(1) << "usage: " << argv[1] << " xml-config-filename" << std::endl;
        return -4;
    }
    if (checkfile(argv[1]) < 0) {
        return -3;
    }

    char currentWorkingDir[256];
    if (getcwd(currentWorkingDir, 255)<0) {
        CMN_LOG(1) << "MAIN: Couldn't get current working directory" << std::endl;
    }

    std::string inputArgument(argv[1]), relativeFilePath, fileName;
    CMN_LOG(1) << "Current working directory: " << currentWorkingDir << std::endl;
    CMN_LOG(1) << "Input Argument: " << inputArgument << std::endl;
    parseInputArgument(inputArgument, relativeFilePath, fileName);
    CMN_LOG(1) << "Main File Name: " << fileName << std::endl;
    CMN_LOG(1) << "Relative Path: " << relativeFilePath << std::endl;

    // ----------------------------------------------------------------------
    // Create the task manager and the tasks
    // ----------------------------------------------------------------------
    cmnXMLPath xmlConfig;

    //from old code
    char filename[] = "./MRRobot_ControlThreadXMLConfig/MRRobot_Main.xml";
    struct stat filebuf;
    if (stat(filename, &filebuf) < 0) {
        CMN_LOG(1) << "MAIN: Could not configure MAIN task, configuration "
            << filename << " file missing!"
            <<std::endl;
        return 3;
    }
    xmlConfig.SetInputSource(filename);

    //xmlConfig.SetInputSource(argv[1]);

    char path[40], context[40];
    std::string FLTKConfigFile, ServoConfigFile, LoPoMoCoConfigFile;
    std::string FLTKConfigDev, ServoConfigDev, LoPoMoCoConfigDev;
    
    CMN_LOG(3) << "Configuring MAIN" << std::endl;
    
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "FLTKConfig/@FileName", FLTKConfigFile);
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "FLTKConfig/@DevName", FLTKConfigDev);
    FLTKConfigFile.insert(0, relativeFilePath);
    CMN_LOG(3) << FLTKConfigDev << " " << FLTKConfigFile << std::endl;
    if (checkfile(FLTKConfigFile.c_str()) < 0) return -3;
    
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "ServoConfig/@FileName", ServoConfigFile);
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "ServoConfig/@DevName", ServoConfigDev);
    ServoConfigFile.insert(0, relativeFilePath);
    CMN_LOG(3) << ServoConfigDev << " " << ServoConfigFile << std::endl;
    if (checkfile(ServoConfigFile.c_str()) < 0) return -3;
    
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "LoPoMoCoConfig/@FileName", LoPoMoCoConfigFile);
    xmlConfig.GetXMLValue("/Config/Device[@Name=\"MAIN\"]", "LoPoMoCoConfig/@DevName", LoPoMoCoConfigDev);
    LoPoMoCoConfigFile.insert(0, relativeFilePath);
    CMN_LOG(3) << LoPoMoCoConfigDev << " " << LoPoMoCoConfigFile << std::endl;
    if (checkfile(LoPoMoCoConfigFile.c_str()) < 0) return -3;


/*
    xmlConfig.SetInputSource(LoPoMoCoConfigFile.c_str());
    std::string LoPoMoCoSubConfigFile[4];
    for (int i = 0; i < 4; i++) {
        sprintf(path, "ConfigFile[%d]/@Name", i+1);
        sprintf(context, "/Config/Device[@Name=\"%s\"]", LoPoMoCoConfigDev.c_str());
        xmlConfig.GetXMLValue(context, path, LoPoMoCoSubConfigFile[i]);
        CMN_LOG(3) << LoPoMoCoSubConfigFile[i].substr(0, LoPoMoCoSubConfigFile[i].find('.')) << std::endl;
    }
*/

    // ----------------------------------------------------------------------
    // Create the task manager and the tasks
    // ----------------------------------------------------------------------
//tmp
CMN_LOG(3) << "rtsTaskManager *taskManager = rtsTaskManager::GetInstance();" << std::endl;
    rtsTaskManager *taskManager = rtsTaskManager::GetInstance();

    /*
    ddiDeviceInterface *loPoMoCo = 0;
    if (strcmp(ServoConfigDev.c_str(), "SVML") == 0) {
        CMN_LOG(3) << "MAIN: Creating Two LoPoMoCo" << std::endl;
        loPoMoCo = new ddiTwoLoPoMoCo(LoPoMoCoConfigDev.c_str(),
                LoPoMoCoSubConfigFile[0].substr(0, LoPoMoCoSubConfigFile[0].find('.')).c_str(),
                LoPoMoCoSubConfigFile[1].substr(0, LoPoMoCoSubConfigFile[1].find('.')).c_str(),
                relativeFilePath.c_str());
    } else if ((strcmp(ServoConfigDev.c_str(), "SSBR") == 0) || (strcmp(ServoConfigDev.c_str(), "SSBL") == 0)) {
        CMN_LOG(3) << "MAIN: Creating Three LoPoMoCo" << std::endl;
        loPoMoCo = new ddiThreeLoPoMoCo(LoPoMoCoConfigDev.c_str(),
                LoPoMoCoSubConfigFile[0].substr(0, LoPoMoCoSubConfigFile[0].find('.')).c_str(),
                LoPoMoCoSubConfigFile[1].substr(0, LoPoMoCoSubConfigFile[1].find('.')).c_str(),
                LoPoMoCoSubConfigFile[2].substr(0, LoPoMoCoSubConfigFile[2].find('.')).c_str(),
                relativeFilePath.c_str());
    } else {
        CMN_LOG(3) << "MAIN: Creating Four LoPoMoCo" << std::endl;
        loPoMoCo = new ddiFourLoPoMoCo(LoPoMoCoConfigDev.c_str(),
                LoPoMoCoSubConfigFile[0].substr(0, LoPoMoCoSubConfigFile[0].find('.')).c_str(),
                LoPoMoCoSubConfigFile[1].substr(0, LoPoMoCoSubConfigFile[1].find('.')).c_str(),
                LoPoMoCoSubConfigFile[2].substr(0, LoPoMoCoSubConfigFile[2].find('.')).c_str(),
                LoPoMoCoSubConfigFile[3].substr(0, LoPoMoCoSubConfigFile[3].find('.')).c_str(),
                relativeFilePath.c_str());
    }
*/

//tmp
CMN_LOG(3) << "ctfMainUIThread *mainUIThread = new ctfMainUIThread(FLTKConfigDev.c_str(), UIperiod * 1000 * 1000);" << std::endl;
    ctfMainUIThread *mainUIThread = new ctfMainUIThread(FLTKConfigDev.c_str(), UIperiod * 1000 * 1000);

//tmp   
CMN_LOG(3) << "rtsTask *controlThread = 0;" << std::endl;
    rtsTask *controlThread = 0;

    //if (strcmp(ServoConfigDev.c_str(), "SVML") == 0) {
    //    CMN_LOG(3) << "MAIN: Creating Current Controller" << std::endl;
    //    controlThread = new daVinciControlThread(ServoConfigDev.c_str(), ServoPeriod * 1000 * 1000);
    //} else if ((strcmp(ServoConfigDev.c_str(), "SSBR") == 0) || (strcmp(ServoConfigDev.c_str(), "SSBL") == 0)) {
    //    CMN_LOG(3) << "MAIN: Creating Voltage Controller for snakes" << std::endl;
    //    controlThread = new TwoSnake5BarControlThread(ServoConfigDev.c_str(), ServoPeriod * 1000 * 1000);
    //} else {
        CMN_LOG(3) << "MAIN: Creating Voltage Controller" << std::endl;
        controlThread = new ctfControlThread(ServoConfigDev.c_str(), ServoPeriod * 1000 * 1000);
    //}
    
    
    ///////////////  GSF ADDED FOR SINGLE BOARD
    CMN_LOG(1) << "MAIN: Creating LoPoMoCo device" << std::endl;
    ddiDeviceInterface *loPoMoCo = 0;
    loPoMoCo = new ddiLoPoMoCo(LoPoMoCoConfigDev.c_str());
   
    CMN_LOG(3) << "controlThreadFLTK: All objects created." << endl;

    cmnObjectRegister::Register("servo", controlThread);

    // Add the tasks to the task manager
    taskManager->AddTask(mainUIThread);
    taskManager->AddTask(controlThread);
    taskManager->AddDevice(loPoMoCo, LoPoMoCoConfigFile.c_str());
    taskManager->AddDevice(controlThread);
    CMN_LOG(3) << "controlThreadFLTK: All tasks added to the task manager." << endl;
    // Add a link between owner (BSVO) and new reader (FLTK)
    taskManager->AssociateDevice(ServoConfigDev.c_str(), LoPoMoCoConfigDev.c_str(), ServoConfigFile.c_str());
    taskManager->AssociateDevice(FLTKConfigDev.c_str(), ServoConfigDev.c_str(), FLTKConfigFile.c_str());
    CMN_LOG(3) << "controlThreadFLTK: All objects associated correctly." << endl;

    // Start all periodic tasks here
    mainUIThread->Create();
    controlThread->Create();
    mainUIThread->Start();
    controlThread->Start();
    CMN_LOG(3) << "controlThreadFLTK: All tasks started." << endl;
    osaTime::Sleep(500);
    // The FLTK event loop will manage the GUI events
    while (1) {
        if (mainUIThread->GetExitFlag()) break;
        osaTime::Sleep((long int)(UIperiod * 1));
    }
    CMN_LOG(3) << "controlThreadFLTK: FLTK loop ended." << std::endl;
    osaTime::Sleep((long int)(UIperiod * 2));
    mainUIThread->Kill();
    controlThread->Kill();
    osaTime::Sleep((long int)(UIperiod * 2));

    while (!mainUIThread->IsTerminated()) osaTime::Sleep((long int)(UIperiod));
    while (!controlThread->IsTerminated()) osaTime::Sleep((long int)(UIperiod));
    CMN_LOG(3) << "controlThreadFLTK: Quitting." << std::endl;

    //delete mainUIThread;
    //delete controlThread;
    return 0;
}

// $ Log: $
//
//
