/*=========================================================================
Module:  $RCSfile: SynchroGrab.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Author:  Jan Gumprecht, Nobuhiko Hata, Harvard Medical School

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 * Neither the name of Queen's University nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Neither the name of Harvard Medical School nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

//
// This command line application let you reconstruct 3D ultrasound volume
// based on the images collected with a tracked probe.
//
// The application first connects to a sonix RP machine during an image
// acquisition session.  Then, it also connects to a Certus which is used to
// track the position and orientation of the ultrasound probe.  The tion
// information (relation between the ultrasound image space and the dynamic reference
// object tracked by the Certus) have to be specified in a configuration file.
// Once a predefined number of frames have been recorded, the application
// creates the 3D volume and exists.
//

#include "vtkFileOutputWindow.h"
#include "vtkImageData.h"
#include "vtkTimerLog.h"

#include "vtkDataCollector.h"
#include "vtkDataProcessor.h"
#include "vtkDataSender.h"
#include "vtkInstrumentTracker.h"
#include "vtkNDITracker.h"

using namespace std;

void printSplashScreen();
void printUsage();
bool parseCommandLineArguments(int argc, char **argv,
                               vtkDataCollector *collector,
                               vtkDataProcessor* processor,
                               vtkDataSender *sender,
                               vtkInstrumentTracker *instrumentTracker);
void goodByeScreen();
void goodByeInput();
static inline void vtkSleep(double duration);

/******************************************************************************
 *
 * MAIN
 *
 ******************************************************************************/
int main(int argc, char **argv)
{
  int terminate = 0;
  ofstream logStream;
  char* logFile = "./logFile.txt";

  vtkDataCollector *collector = vtkDataCollector::New();
  vtkDataProcessor *processor = vtkDataProcessor::New();
  vtkDataSender *sender = vtkDataSender::New();
  
  vtkInstrumentTracker *instrumentTracker =   vtkInstrumentTracker::New();

  vtkNDITracker * tracker = vtkNDITracker::New();
  
  //Fix StartUp time
  double startTime = vtkTimerLog::GetUniversalTime();
  collector->SetStartUpTime(startTime);
  processor->SetStartUpTime(startTime);
  sender->SetStartUpTime(startTime);
  instrumentTracker->SetStartUpTime(startTime);

  printSplashScreen();

  //Read command line arguments
  bool successParsingCommandLine = parseCommandLineArguments(argc, argv, collector, processor, sender, instrumentTracker);
  if(!successParsingCommandLine)
    {
    cerr << "ERROR: Could parse commandline arguments" << endl << endl;
    goodByeScreen();
    goodByeInput();
    }
  else
    {
    //Log Stream Preparation
    logStream.precision(0);
    logStream.setf(ios::fixed,ios::floatfield);
    logStream.open(logFile, ios::out);
    collector->SetLogStream(logStream);
    processor->SetLogStream(logStream);
    sender->SetLogStream(logStream);
    instrumentTracker->SetLogStream(logStream);

    cout << "--- Started ---" << endl << endl;

    //redirect vtk errors to a file
    vtkFileOutputWindow *errOut = vtkFileOutputWindow::New();
    errOut->SetFileName("vtkError.txt");
    vtkOutputWindow::SetInstance(errOut);

    cout << "Hardware Initialization: " << std::flush;
    cout << '\a' << std::flush;
    for(int i = 0; i < 11; i++)
      {
      vtkSleep(0.2);
      
      cout << 10 - i << " " << std::flush;
      }
    cout << endl;

    //cout << "Start Recording" << endl;
    cout << '\a' << std::flush;
    vtkSleep(0.2);
    cout << '\a' << std::flush;
    
    if(instrumentTracker->GetTrackingEnabled() || collector->GetTrackerDeviceEnabled())
      {
      
      if(tracker->Probe() != 1)
        {
        cerr << "ERROR: Tracking system not found" << endl;
        terminate += 1;
        }
      else 
        {
        tracker->StartTracking();
        }
      }
    
    //Configure collector
    if(terminate == 0)
      {
      if(collector->GetTrackerDeviceEnabled())
        {
        if(collector->Initialize(tracker) != 0)
          {
            cerr << "ERROR: Could not initialize DataCollector" << endl;
            terminate += 2;
          }
        }
      else
        {
        if(collector->Initialize(NULL) != 0)
          {
            cerr << "ERROR: Could not initialize DataCollector" << endl;
            terminate += 2;
          }
        }
      }
      
    // Connect Sender to OpenIGTLink Server
    if(terminate == 0)
      {
      if(sender->ConnectToServer() != 0)
        {
        terminate += 4;
        }
      }

    //Start Send Thread
    if(terminate == 0)
      {
      if(sender->StartSending() != 0)
        {
        terminate += 8;
        }
      }

    //Start Send Thread
    if(terminate == 0)
      {
      if(processor->StartProcessing(sender) != 0)
        {
        terminate += 16;
        }
      }
    
    //Colleced video frames and tracking matrices and forward them to the sender
    if(terminate == 0)
      {
      if(collector->StartCollecting(processor) != 0)
        {
        terminate += 32;
        }
      }
    
    goodByeScreen();

    //Colleced instrument tracking matrices and forward them to the OpenIGTLink server
    if(terminate == 0)
      {
      if(instrumentTracker->GetTrackingEnabled())
        {
        if(-1 != instrumentTracker->ConnectToServer())
          {
          if(-1 == instrumentTracker->StartTracking(tracker))
            {
            terminate += 64;
            }
          }
        else
          {
          terminate += 64;
          }
        }
      else if(instrumentTracker->GetSimulationEnabled())
        {
        if(-1 != instrumentTracker->ConnectToServer())
          {
          if(-1 == instrumentTracker->StartTracking(NULL))
            {
            terminate += 64;
            }
          }
        else
          {
          terminate += 64;
          }
        }
      }

    //Wait for user input to terminate 4D-Ultrasound
    if(terminate != 0)
      {
      cerr << "ERROR occured while starting 4DUltrasound | Error-Code: " << terminate << endl;
      }
    
    goodByeInput();

    instrumentTracker->StopTracking();
    instrumentTracker->CloseServerConnection();

    //Stop Collecting
    collector->StopCollecting();

    //Stop Processing
    processor->StopProcessing();

    //Stop Sending
    sender->StopSending();
    //Close Sever Connection
    sender->CloseServerConnection();      

    if(instrumentTracker->GetTrackingEnabled() || collector->GetTrackerDeviceEnabled())
      {
      tracker->StopTracking();
      }
    
    
    cout << endl;
    if(terminate == 0)
      {
      cout << "--- 4D Ultrasound finished ---" << endl << endl;
      }
    else
      {
      cout << "--- 4D Ultrasound finished with ERRORS ---" << endl
           << "---           ERRORCODE: "<< terminate <<"             ---" << endl << endl;
      }
    logStream.close();
    errOut->Delete();
    }
  instrumentTracker->Delete();
  collector->Delete();
  processor->Delete();
  sender->Delete();
  tracker->Delete();
  
}//End Main

//------------------------------------------------------------------------------
void printSplashScreen()
{

  cout << endl
       <<"*******************************************************************" << endl
       <<"*                                                                 *" << endl
       <<"* 4D Ultrasound - Real-Time Tracked 3D Ultrasound Imaging         *" << endl
       <<"*                                                                 *" << endl
       <<"* Author: Jan Gumprecht                                           *" << endl
       <<"*         Brigham and Women's Hospital                            *" << endl
       <<"* Date:   Jan 2009                                                *" << endl
       <<"*                                                                 *" << endl
       <<"*******************************************************************" << endl;

}


//------------------------------------------------------------------------------
void printUsage()
{
    cout << endl
         << "--------------------------------------------------------------------------------"
         << endl;

    // print instructions on how to use this program.
    cout << "DESCRIPTION " << endl
         << "-----------"<<endl;
    cout << "This software does tracked 3D Ultrasound imaging. It captures frames from an"<< endl
         << "ultrasound device and tracking information from a tracker device." << endl
         << "The images and the tracking information are combined to a 3D volume image."<<endl
         << "The reconstructed volume is send to an OpenIGTLink server of choice," <<endl
         << " e.g. 3DSlicer at the end." << endl;
    cout << endl
         << "--------------------------------------------------------------------------------"
         << endl;
    cout << "OPTIONS " << endl
         << "-------"<<endl
         << "--calibration-file xxx or -c xxx:       Specify the calibration file" << endl
         << "                                        (MANDATORY)"<< endl
         << "--reconstruct-volume or -rv:            Enable volume reconstruction" << endl
         << "--track-ultrasound or -tu:              Enable ultrasound tracking"<< endl
         << "--track-instrument or -ti:              Enable instrument tracking" << endl
         << "--simulate-instrument or -si:           Simulate instrument"<< endl
         << "--oigtl-server xxx or -os xxx:          Specify OpenIGTLink server"<< endl
         << "                                        (default: 'localhost')" << endl
         << "--oigtl-port xxx or -op xxx:            Specify OpenIGTLink port of"<< endl
         << "                                        server (default: 18944)" << endl
         << "--frames-per-second xxx or -fps xxx:    Number of frames per second for"<< endl
         << "                                        the ultrasound data collection"<< endl
         << "                                        (default: 10)" << endl
         << "--video-source xxx or -vs xxx:          Set video source "<< endl
         << "                                        (default: '/dev/video0')"<< endl
         << "--video-source-channel xxx or -vsc xxx: Set video source channel"<< endl
         << "                                        (default: 3)" << endl
         << "--video-mode xxx or -vm xxx:            Set video mode; "<< endl
         << "                                        Options: NTSC, PAL "<< endl
         << "                                        (default: NTSC)" << endl
         << "--scan-depth xxx or -sd xxx:            Set depth of ultrasound "<< endl
         << "                                        scan in Millimeter"<< endl
         << "                                        (default: 70mm)" << endl
         << "--tracker-offset xxx or -to xxx:        Set offset of tracker to " << endl
         << "                                        ultrasound probe in Millimeter" << endl
         << "                                        (default: 35mm)" << endl
         << "--maximum-volumesize xxx or -maxvs xxx: Maximum volume size (in Byte) " << endl
         << "                                        the system can handle" << endl
         << "--verbose or -v:                        Print more information" << endl
         << endl
         << "--------------------------------------------------------------------------------"
         << endl << endl;
}

//------------------------------------------------------------------------------
//
// Parse the command line options.
//
bool parseCommandLineArguments(int argc, char **argv,
                               vtkDataCollector *collector,
                               vtkDataProcessor *processor,
                               vtkDataSender *sender,
                               vtkInstrumentTracker *instrumentTracker)
{
    // needs to be supplied
    bool calibrationFileSpecified = false;

    for(int i=1; i < argc; i++)
        {
        string currentArg(argv[i]);
        //Calibration file
        
        if( currentArg == "--calibration-file" || currentArg == "-c")
          {
          if( i < argc - 1)
            {
            collector->SetCalibrationFileName(argv[++i]);
            processor->SetCalibrationFileName(argv[i]);
            instrumentTracker->SetCalibrationFileName(argv[i]);
            calibrationFileSpecified = true;
            }
          }
        //Enable volume reconstruction
        else if(currentArg == "--reconstruct-volume" || currentArg == "-rv")
          {
          processor->EnableVolumeReconstruction(true);
          }
        //Track operting instrument
        else if(currentArg == "--track-instrument" || currentArg == "-ti")
          {
          if(!instrumentTracker->GetSimulationEnabled())
            {
            instrumentTracker->SetTrackingEnabled(true);
            }
          else
            {
            cout << "ERROR: Instrument can either be simulated or tracked NOT BOTH" << endl;
            return false;
            }
          }
        else if(currentArg == "--simulate-instrument" || currentArg == "-si")
          {
          if(!instrumentTracker->GetTrackingEnabled())
            {
            instrumentTracker->SetSimulationEnabled(true);
            }
          else
            {
            cout << "ERROR: Instrument can either be simulated or tracked NOT BOTH" << endl;
            return false;
            }
          }
        //Track ultrasound
        else if(currentArg == "--track-ultrasound" || currentArg == "-tu")
          {
          collector->EnableTrackerTool();
          }
        //OpenIGTLink Server
        else if(currentArg == "--oigtl-server" || currentArg == "-os")
          {
          sender->SetOIGTLServer(argv[++i]);
          instrumentTracker->SetOIGTLServer(argv[i]);
          }
        //OpenIGTLink Port at Server
        else if(currentArg == "--oigtl-port" || currentArg == "-op")
          {
          if( i < argc - 1)
            {
            sender->SetServerPort(atoi(argv[++i]));
            instrumentTracker->SetServerPort(atoi(argv[i]));
            }
          }
        // Framerate of Ultrasound device
        else if(currentArg == "--frames-per-second" || currentArg == "-fps")
          {
          if( i < argc - 1)
            {
              double fps = atof(argv[++i]);

            if(fps <= 0)
              {
              cout << "ERROR: Framerate must be greater than 0 and not: " << fps << endl;
              return false;
              }
            collector->SetFrameRate(fps);
            processor->SetProcessPeriod(1/(fps * 1.5));
            sender   ->SetSendPeriod(1/(fps * 1.5));
            }
          }
        //Video Source
        else if(currentArg == "--video-source" || currentArg == "-vs")
           {
           collector->SetVideoDevice(argv[++i]);
           }
        //Video Source channel
        else if(currentArg == "--video-source-channel" || currentArg == "-vsc")
           {
           collector->SetVideoChannel(atoi(argv[++i]));
           }
        //Video Mode
        else if(currentArg == "--video-mode" || currentArg == "-vm")
           {
           string videoMode (argv[++i]);

           if(videoMode == "NTSC")
             {
             collector->SetVideoMode(1);
             }
           else if(videoMode == "PAL")
             {
             collector->SetVideoMode(2);
             }
           else
             {
             cout << "ERROR: False video mode: " << videoMode  << endl
                  << "       Available video modes: NTSC and PAL" << endl;
             return false;
             }
           }
        else if(currentArg == "--scan-depth" || currentArg == "-sd")
          {
          if( i < argc - 1)
            {
            double scanDepth  = atof(argv[++i]);

            if(scanDepth <= 0)
              {
              cout << "ERROR: Scan Depth must be greater than 0 and not: " << scanDepth << endl;
              return false;
              }
            collector->SetUltrasoundScanDepth(scanDepth);
            }
          }
        else if(currentArg == "--tracker-offset" || currentArg == "-to")
          {
          collector->SetTrackerOffset(atoi(argv[++i]));
          }
        else if(currentArg == "--maximum-volumesize" || currentArg == "-maxvs")
          {
          double volumeSize = atoi(argv[++i]);
          if(volumeSize <= 0)
            {
            cout << "ERROR: Specified volume size ( "<< volumeSize <<" ) is too small." << endl
                 << "       Volume size must be bigger than 0 " << endl;
            return false;
            }
          else
            {
            collector->SetMaximumVolumeSize(volumeSize);
            processor->SetMaximumVolumeSize(volumeSize);
            }
          }
        else if(currentArg == "--delay-factor" || currentArg == "-df")
          {
          processor->SetDelayFactor(atof(argv[++i]));
          }
        else if(currentArg == "--verbose" || currentArg == "-v")
          {
          collector->SetVerbose(true);
          sender->SetVerbose(true);
          processor->SetVerbose(true);
          instrumentTracker->SetVerbose(true);
          }       
        else
           {
           printUsage();
           cout << "ERROR: Unrecognized command: '" << argv[i] <<"'" << endl;
           return false;
           }
      }
    return calibrationFileSpecified;
}

//------------------------------------------------------------------------------
void goodByeScreen()
{

 cout << endl;
 cout << "Press 't' and hit 'ENTER' to terminate 4D Ultrasound"<<endl;
}

void goodByeInput()
{
 string input;
 while(cin >> input)
   {
   if(input == "t")
     {
       break;
     }
   }

  cout << endl;
  cout << "Terminate 4D Ultrasound" << endl
       << endl
       << "Bye Bye..." << endl <<endl << endl;

}

/******************************************************************************
 *  static inline void vtkSleep(double duration)
 *
 *  Platform-independent sleep function
 *  Set the current thread to sleep for a certain amount of time
 *
 *  @Param: double duration - Time to sleep in ms
 *
 * ****************************************************************************/
static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi) || defined(__APPLE__)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
}
