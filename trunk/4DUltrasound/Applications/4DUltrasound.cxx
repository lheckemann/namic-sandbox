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

//ERROR DEFINITION

#define ERROR_NO_TRACKER_FOUND 1
#define ERROR_DATA_COLLECTOR_NOT_INITIALIZED 2
#define ERROR_DATA_SENDER_NOT_INITIALIZED 3
#define ERROR_NO_CONNECTION_TO_OPENIGTLINKSERVER 4
#define ERROR_DATA_SENDER_DOES_NOT_WORK 5
#define ERROR_DATA_PROCESSOR_DOES_NOT_WORK 6
#define ERROR_DATA_COLLECTOR_DOES_NOT_WORK 7
#define ERROR_INSTRUMENT_TRACKING_DOES_NOT_WORK 8


using namespace std;

void printSplashScreen();
void printUsage();
int parseCommandLineArguments(int argc, char **argv,
vtkDataCollector *collector,
vtkDataProcessor* processor,
vtkDataSender *sender,
vtkInstrumentTracker *instrumentTracker);
void goodByeScreen();
void goodByeInput();
void printErrorCodes(int terminate);
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
  bool grabOneImage = false;

  //Log Stream Preparation for error logging----------------------------------
  logStream.precision(0);
  logStream.setf(ios::fixed,ios::floatfield);
  logStream.open(logFile, ios::out);

    //redirect vtk errors to a file
    vtkFileOutputWindow *errOut = vtkFileOutputWindow::New();
    errOut->SetFileName("vtkError.txt");
    vtkOutputWindow::SetInstance(errOut);

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

  collector->SetLogStream(logStream);
  processor->SetLogStream(logStream);
  sender->SetLogStream(logStream);
  instrumentTracker->SetLogStream(logStream);

  printSplashScreen();

  //Read command line arguments
  int successParsingCommandLine = parseCommandLineArguments(argc, argv, collector, processor, sender, instrumentTracker);
  if(-1 == successParsingCommandLine)
   {
   cerr << "ERROR: Could not parse commandline arguments" << endl << endl;
   goodByeScreen();
   goodByeInput();
   }
  else
    {

    //Enable Volume Reconstruction
    processor->EnableVolumeReconstruction(true);
    sender->SetReconstructionEnabled(true);

    //Print start process for the user------------------------------------------
    cout << "--- Started ---" << endl << endl;

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
      if(tracker->Probe() != 1)//Test tracker
        {
        cerr << "ERROR: Tracking system not found" << endl;
        terminate += ERROR_NO_TRACKER_FOUND;
        }
      else
        {
        tracker->StartTracking();
        }
      }

  //Initialize collector
  if(terminate == 0)
    {
    if(collector->GetTrackerDeviceEnabled())
     {
     if(collector->Initialize(tracker) != 0)
       {
       cerr << "ERROR: Could not initialize DataCollector" << endl;
       terminate += ERROR_DATA_COLLECTOR_NOT_INITIALIZED;
       }
     }
    else
      {
      if(collector->Initialize(NULL) != 0)
        {
        cerr << "ERROR: Could not initialize DataCollector" << endl;
        terminate += ERROR_DATA_COLLECTOR_NOT_INITIALIZED;
        }
      }
    }

  //If requested by CLP grab one frame from the frame grabber and terminate
  if(1 == successParsingCommandLine)
    {
    collector->GrabOneImage();
    cout << "Image successfully grabbed to " << collector->GetFileName() << endl;
    }
  else
    {
      //Configure collector
      if(terminate == 0)
        {
        if(sender->Initialize() != 0)
          {
          cerr << "ERROR: Could not initialize DataSender" << endl;
          terminate += ERROR_DATA_SENDER_NOT_INITIALIZED;
          }
        }

       // Connect Sender to OpenIGTLink Server
       if(terminate == 0)
         {
         if(sender->ConnectToServer() != 0)
           {
           terminate += ERROR_NO_CONNECTION_TO_OPENIGTLINKSERVER;
           }
         }

       //Start Send Thread
       if(terminate == 0)
         {
         if(sender->StartSending() != 0)
           {
           terminate += ERROR_DATA_SENDER_DOES_NOT_WORK;
           }
         }

       //Start Send Thread
       if(terminate == 0)
         {
         if(processor->StartProcessing(sender) != 0)
           {
           terminate += ERROR_DATA_PROCESSOR_DOES_NOT_WORK;
           }
         }

       //Collect video frames and tracking matrices and forward them to the sender
       if(terminate == 0)
         {
         if(collector->StartCollecting(processor) != 0)
           {
           terminate += ERROR_DATA_COLLECTOR_DOES_NOT_WORK;
           }
         }

  //     goodByeScreen();

       //Collected instrument tracking matrices and forward them to the OpenIGTLink server
       if(terminate == 0)
         {
         if(instrumentTracker->GetTrackingEnabled())
           {
           if(-1 != instrumentTracker->ConnectToServer())
             {
             if(-1 == instrumentTracker->StartTracking(tracker))
               {
               terminate += ERROR_INSTRUMENT_TRACKING_DOES_NOT_WORK;
               }
             }
           else
             {
             terminate += ERROR_INSTRUMENT_TRACKING_DOES_NOT_WORK;
             }
           }
           else if(instrumentTracker->GetSimulationEnabled())
           {
           if(-1 != instrumentTracker->ConnectToServer())
             {
             if(-1 == instrumentTracker->StartTracking(NULL))
               {
               terminate += ERROR_INSTRUMENT_TRACKING_DOES_NOT_WORK;
               }
             }
           else
             {
             terminate += ERROR_INSTRUMENT_TRACKING_DOES_NOT_WORK;
             }
           }
         }
       }//GrabOneImage

    //Wait for user input to terminate 4D-Ultrasound
    if(terminate != 0)
      {
      cout << '\a' << std::flush;
        vtkSleep(0.2);
      cout << '\a' << std::flush;
      vtkSleep(0.2);
      cout << '\a' << std::flush;

      cerr << "ERROR occurred while starting 4DUltrasound" << endl
      << endl;
      printErrorCodes(terminate);
      goodByeScreen();
      }
    else
      {
      if(!collector->GetCollectCalibrationData())
        {
        goodByeScreen();
        }
      goodByeInput();
      }

    //Stop Collecting
    collector->StopCollecting();

    //Stop Processing
//    processor->StopProcessing();

    //Stop Sending
//    sender->StopSending();

    //Close Sever Connection
    sender->CloseServerConnection();      

    if(instrumentTracker->GetTrackingEnabled() && instrumentTracker->GetTracking())
      {
      goodByeInput();
      }

    instrumentTracker->StopTracking();
    instrumentTracker->CloseServerConnection();
    
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
    }

  if(instrumentTracker)
    {
    instrumentTracker->Delete();
    instrumentTracker = NULL;
    }
  if(collector)
    {
    collector->Delete();
    collector = NULL;
    }
  if(processor)
    {
    processor->Delete();
    processor = NULL;
    }
  if(sender)
    {
    sender->Delete();
    sender = NULL;
    }
  if(tracker)
    {
    tracker->Delete();
    tracker = NULL;
    }

  logStream.close();

  if(errOut)
    {
    errOut->Delete();
    errOut = NULL;
    }
}//End Main

/******************************************************************************
 *  void printSplashScreen()
 *
 *  Print the splash screen
 *
 *  @author: Jan Gumprecht
 *  @date:   20. Januar 2009
 *
 * ****************************************************************************/
void printSplashScreen()
{

  cout << endl
       <<"*******************************************************************" << endl
       <<"*                                                                 *" << endl
       <<"* 4D Ultrasound - Real-Time Tracked 3D Ultrasound Imaging         *" << endl
       <<"*                                                                 *" << endl
       <<"* Author: Jan Gumprecht                                           *" << endl
       <<"*         Brigham and Women's Hospital                            *" << endl
       <<"* Date:   Aug 2009                                                *" << endl
       <<"*                                                                 *" << endl
       <<"*******************************************************************" << endl;

}

/******************************************************************************
 *  void printUsage()
 *
 *  Print Description of the software as well as its command line parameters
 *
 *  @author: Jan Gumprecht
 *  @date:   20. Januar 2009
 *
 * ****************************************************************************/
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
         << "e.g. 3DSlicer at the end. Additionally it is possible to track a" << endl
         << "surgical instrument." << endl;
    cout << endl
         << "--------------------------------------------------------------------------------"
         << endl;
    cout << "OPTIONS " << endl
         << "-------"<<endl
         << "--calibration-file xxx or -c xxx:       Specify the calibration file" << endl
         << "                                        (MANDATORY)"<< endl
//         << "--reconstruct-volume or -rv:            Enable volume reconstruction" << endl
//         << "--dynamic-volumesize or -dvs:           Enable dynamic volume size for" << endl
//         << "                                        reconstructed volume"<< endl
         << "--grab-ultrasound-frames or -gf:        Enables the grabbing of ultrasound"<< endl
         << "                                        frames from the capture board"<< endl
         << "--track-ultrasound or -tu:              Enable ultrasound tracking"<< endl
         << "--track-instrument or -ti:              Enable instrument tracking" << endl
         << "--simulate-instrument or -si:           Simulate instrument"<< endl
         << "--grab-one-frame xxx or -gof xxx:       Grabs one ultrasound frame and "<< endl
         << "                                        saves it as the specified file" << endl
         << "--get-calibration-data  or -gcd:        Collects data necessary for the "<< endl
         << "                                        calibration" << endl
//         << "--oigtl-server xxx or -os xxx:          Specify OpenIGTLink server"<< endl
//         << "                                        (default: 'localhost')" << endl
//         << "--oigtl-port xxx or -op xxx:            Specify OpenIGTLink port of"<< endl
//         << "                                        server (default: 18944)" << endl
//         << "--frames-per-second xxx or -fps xxx:    Number of frames per second for"<< endl
//         << "                                        the ultrasound data collection"<< endl
//         << "                                        (default: 10)" << endl
//         << "--video-source xxx or -vs xxx:          Set video source "<< endl
//         << "                                        (default: '/dev/video0')"<< endl
//         << "--video-source-channel xxx or -vsc xxx: Set video source channel"<< endl
//         << "                                        (default: 3)" << endl
//         << "--video-mode xxx or -vm xxx:            Set video mode; "<< endl
//         << "                                        Options: NTSC, PAL "<< endl
//         << "                                        (default: NTSC)" << endl
//         << "--scan-depth xxx or -sd xxx:            Set depth of ultrasound "<< endl
//         << "                                        scan in Millimeter"<< endl
//         << "                                        (default: 70mm)" << endl
         << "--verbose or -v:                        Print more information" << endl
         << endl
         << "--------------------------------------------------------------------------------"
         << endl << endl;
}

/******************************************************************************
 *  int parseCommandLineArguments(int argc, char **argv,
 *                             vtkDataCollector *collector,
 *                             vtkDataProcessor *processor,
 *                             vtkDataSender *sender,
 *                             vtkInstrumentTracker *instrumentTracker)
 *
 *  Parse the command line parameters (CLPs) and check, if possible, for errors
 *
 *  @author: Jan Gumprecht
 *  @date:   20. Januar 2009
 *
 *  @param: int argc - Number of CLPs
 *  @param: char **argv - CLPs
 *  @param: vtkDataCollector *collector - Instance of the "Data Collector"
 *  @param: vtkDataProcessor *processor - Instance of the "Data Processor"
 *  @param: vtkDataSender *sender - Instance of the "Data Sender"
 *  @param: vtkInstrumentTracker *instrumentTracker - Instance of the
 *                                                    "Instrument Tracker"
 *
 * ****************************************************************************/
int parseCommandLineArguments(int argc, char **argv,
                               vtkDataCollector *collector,
                               vtkDataProcessor *processor,
                               vtkDataSender *sender,
                               vtkInstrumentTracker *instrumentTracker)
{
    // needs to be supplied
    int retVal = -1;
    
    int i = 1;
    
    string firstArg(argv[i]);
    
    //Calibration file must be the first argument
    if( firstArg == "--calibration-file" || firstArg == "-c")
       {
       if( i < argc - 1)
         {
         collector->SetCalibrationFileName(argv[++i]);
         processor->SetCalibrationFileName(argv[i]);
         sender->SetCalibrationFileName(argv[i]);
         instrumentTracker->SetCalibrationFileName(argv[i]);
         retVal = 0;
         }
       }
    else
      {
      printUsage();
      cerr << "ERROR: The first parameter must be the calibration file" << endl;
      return -1;
      }

    for(i = 3; i < argc; i++)
        {
        string currentArg(argv[i]);
       
//        //Enable volume reconstruction
//        if(currentArg == "--reconstruct-volume" || currentArg == "-rv")
//          {
//          processor->EnableVolumeReconstruction(true);
//          sender->SetReconstructionEnabled(true);
//          }
//        else if(currentArg == "--dynamic-volumesize" || currentArg == "-dvs")
//          {
//          if(processor->GetVolumeReconstructionEnabled())
//            {
//            collector->SetDynamicVolumeSize(true);
//            processor->SetDynamicVolumeSize(true);
//            }
//          else
//            {
//            printUsage();
//            cerr << "ERROR: Volume reconstruction must be enabled before dynamic volume size option is avaliable" << endl;
//            return -1;
//            }
//          }

        if(currentArg == "--grab-ultrasound-frames" || currentArg == "-gf")
          {
          collector->SetEnableFrameGrabbing(true);
          }
        //Track surgical instrument
        else if(currentArg == "--track-instrument" || currentArg == "-ti")
          {
          if(!instrumentTracker->GetSimulationEnabled())
            {
            instrumentTracker->SetTrackingEnabled(true);
            }
          else
            {
            printUsage();
            cout << "ERROR: Instrument can either be simulated or tracked NOT BOTH" << endl;
            return -1;
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
            printUsage();
            cout << "ERROR: Instrument can either be simulated or tracked NOT BOTH" << endl;
            return -1;
            }
          }
        //Track ultrasound
        else if(currentArg == "--track-ultrasound" || currentArg == "-tu")
          {
          collector->EnableTrackerTool();
          }
        else if(currentArg == "--get-calibration-data" || currentArg == "-gcd")
          {
          collector->SetCollectCalibrationData(true);
          processor->SetCollectCalibrationData(true);
          collector->SetEnableFrameGrabbing(true);
          collector->EnableTrackerTool();
          }

//        //OpenIGTLink Server
//        else if(currentArg == "--oigtl-server" || currentArg == "-os")
//          {
//          if( i < argc - 1)
//             {
//             sender->SetOIGTLServer(argv[++i]);
//             instrumentTracker->SetOIGTLServer(argv[i]);
//             }
//          else
//            {
//            printUsage();
//            cout << "ERROR: OpenIGTLink Server not specified" << endl;
//            return -1;
//            }
//          }
//        //OpenIGTLink Port at Server
//        else if(currentArg == "--oigtl-port" || currentArg == "-op")
//          {
//          if( i < argc - 1)
//            {
//            sender->SetServerPort(atoi(argv[++i]));
//            instrumentTracker->SetServerPort(atoi(argv[i]));
//            }
//          else
//            {
//            printUsage();
//            cout << "ERROR: OpenIGTLink Port not specified" << endl;
//            return -1;
//            }
//          }
//        // Framerate of Ultrasound device
//        else if(currentArg == "--frames-per-second" || currentArg == "-fps")
//          {
//          if( i < argc - 1)
//            {
//              double fps = atof(argv[++i]);
//
//            if(fps <= 0)
//              {
//              cout << "ERROR: Framerate must be greater than 0 and not: " << fps << endl;
//              return -1;
//              }
//            collector->SetFrameRate(fps);
//            processor->SetProcessPeriod(1/(fps * 1.5));
//            sender   ->SetSendPeriod(1/(fps * 1.5));
//            }
//          else
//            {
//            printUsage();
//            cout << "ERROR: Frames per second not specified" << endl;
//            return -1;
//            }
//          }
//        //Video Source
//        else if(currentArg == "--video-source" || currentArg == "-vs")
//           {
//           if( i < argc - 1)
//             {
//             collector->SetVideoDevice(argv[++i]);
//             }
//           else
//             {
//             printUsage();
//             cout << "ERROR: Video source not specified" << endl;
//             return -1;
//             }
//           }
//        //Video Source channel
//        else if(currentArg == "--video-source-channel" || currentArg == "-vsc")
//           {
//           if( i < argc - 1)
//             {
//             collector->SetVideoChannel(atoi(argv[++i]));
//             }
//           else
//             {
//             printUsage();
//             cout << "ERROR: Video source channel not specified" << endl;
//             return -1;
//             }
//           }
        //Video Mode
//        else if(currentArg == "--video-mode" || currentArg == "-vm")
//           {
//           if( i < argc - 1)
//             {
//             string videoMode (argv[++i]);
//
//             if(videoMode == "NTSC")
//               {
//               collector->SetVideoMode(1);
//               }
//             else if(videoMode == "PAL")
//               {
//               collector->SetVideoMode(2);
//               }
//             else
//               {
//               cout << "ERROR: False video mode: " << videoMode  << endl
//                    << "       Available video modes: NTSC and PAL" << endl;
//               return -1;
//               }
//             }
//           else
//             {
//             printUsage();
//             cout << "ERROR: Video mode not specified" << endl;
//             return -1;
//             }
//           }
//        else if(currentArg == "--scan-depth" || currentArg == "-sd")
//          {
//          if( i < argc - 1)
//            {
//            double scanDepth  = atof(argv[++i]);
//
//            if(scanDepth <= 0)
//              {
//              cout << "ERROR: Scan Depth must be greater than 0 and not: " << scanDepth << endl;
//              return -1;
//              }
//            collector->SetUltrasoundScanDepth(scanDepth);
//            }
//          else
//            {
//            printUsage();
//            cout << "ERROR: Scand depth not specified" << endl;
//            return -1;
//            }
//          }
//        else if(currentArg == "--delay-factor" || currentArg == "-df")
//          {
//          if( i < argc - 1)
//            {
//            processor->SetDelayFactor(atof(argv[++i]));
//            }
//          else
//            {
//            printUsage();
//            cout << "ERROR: Delay factor size not specified" << endl;
//            return -1;
//            }
//          }
//        else if(currentArg == "--system-offset" || currentArg == "-so")
//          {
//          if( i < argc - 3)
//            {
//            double systemOffset[3] = {atof(argv[++i]), atof(argv[++i]), atof(argv[++i])};
//            collector->SetSystemOffset(systemOffset[0], systemOffset[1], systemOffset[2]);
//            instrumentTracker->SetSystemOffset(systemOffset[0], systemOffset[1], systemOffset[2]);
//            }
//          else
//            {
//            printUsage();
//            cout << "ERROR: System offset not specified" << endl;
//            return -1;
//            }
//          }
        else if(currentArg == "--grab-one-frame" || currentArg == "-gof")
          {
          if( i < argc - 1)
            {
            collector->SetFileName(argv[++i]);
            collector->SetEnableFrameGrabbing(true);
            return 1;
            }
          else
            {
            printUsage();
            cout << "ERROR: No Filename for the grabbed image specified" << endl;
            return -1;
            }
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
           return -1;
           }
      }
    return retVal;
}

/******************************************************************************
 *  void goodByeScreen()
 *
 *  Print request to terminate the software
 *
 *  @author: Jan Gumprecht
 *  @date:   20. Januar 2009
 *
 * ****************************************************************************/
void goodByeScreen()
{

 cout << endl;
 cout << "Press 't' and hit 'ENTER' to terminate 4D Ultrasound"<<endl;
}

/******************************************************************************
 *  void goodByeInput()
 *
 *  Wait for termination signal and print good bye screen
 *
 *  @author: Jan Gumprecht
 *  @date:   20. Januar 2009
 *
 * ****************************************************************************/
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
 *  void printErrorCodes(int terminate)
 *
 *  Prints Error message for the specified code
 *
 *  @author: Jan Gumprecht
 *  @date:   27. July 2009
 *
 *  @Param: terminate - errorcode of the message to print
 *
 * ****************************************************************************/
void printErrorCodes(int terminate)
{

  switch(terminate)
    {
    case ERROR_NO_TRACKER_FOUND: cerr <<"No tracking system found" <<endl
                                      <<"(It needs two tries to recognize the tracker "<< endl
                                      <<"after the tracking hardware was connected to the computer)" << endl;
         break;
    case ERROR_DATA_COLLECTOR_NOT_INITIALIZED: cerr << "Can not initialize data collection" << endl;
         break;
    case ERROR_DATA_SENDER_NOT_INITIALIZED: cerr << "Can not initialize data forwarding to visualization software" << endl;
             break;
    case ERROR_NO_CONNECTION_TO_OPENIGTLINKSERVER: cerr << "Can not connect to OpenIGTLink Server" << endl;
         break;
    case ERROR_DATA_SENDER_DOES_NOT_WORK: cerr << "Can not start to send data to visualization software" << endl;
         break;
    case ERROR_DATA_PROCESSOR_DOES_NOT_WORK: cerr << "Can not start to process data, i.e. to do volume reconstruction" << endl;
         break;
    case ERROR_DATA_COLLECTOR_DOES_NOT_WORK: cerr << "Can not start to collect data" << endl;
         break;
    case ERROR_INSTRUMENT_TRACKING_DOES_NOT_WORK: cerr << "Can not track instrument" << endl;
         break;
    default: cerr << "Wrong Error Code. Code " << terminate << " does not exist" << endl;
    }
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
