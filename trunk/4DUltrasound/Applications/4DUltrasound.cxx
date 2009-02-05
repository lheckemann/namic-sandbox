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
#include "vtkDataCollector.h"
#include "vtkDataProcessor.h"
#include "vtkDataSender.h"
#include "vtkImageData.h"
#include "vtkTimerLog.h"

using namespace std;

void printSplashScreen();
void printUsage();
bool parseCommandLineArguments(int argc, char **argv, vtkDataCollector *collector, vtkDataProcessor* processor, vtkDataSender *sender);
void goodByeScreen();

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

  double startTime = vtkTimerLog::GetUniversalTime();
  collector->SetStartUpTime(startTime);
  processor->SetStartUpTime(startTime);
  sender->SetStartUpTime(startTime);

  printSplashScreen();

  //Read command line arguments
  bool successParsingCommandLine = parseCommandLineArguments(argc, argv, collector, processor, sender);
  if(!successParsingCommandLine)
    {
    cerr << "ERROR: Could parse commandline arguments" << endl << endl;
    goodByeScreen();
    }
  else
    {
    logStream.precision(0);
    logStream.setf(ios::fixed,ios::floatfield);
    logStream.open(logFile, ios::out);
    collector->SetLogStream(logStream);
    processor->SetLogStream(logStream);
    sender->SetLogStream(logStream);
          
    cout << "--- Started ---" << endl << endl;

    //redirect vtk errors to a file
    vtkFileOutputWindow *errOut = vtkFileOutputWindow::New();
    errOut->SetFileName("vtkError.txt");
    vtkOutputWindow::SetInstance(errOut);

    //Configure collector
    if(collector->Initialize() != 0)
      {
        cerr << "ERROR: Could not initialize DataCollector" <<endl;
        printUsage();
        goodByeScreen();
      }
    else
      {
      // Connect to OpenIGTLink Server
      if(sender->ConnectToServer() != 0)
        {
        terminate += 1;
        }

      //Start Send Thread
      if(terminate == 0)
        {
        if(sender->StartSending() != 0)
          {
          terminate += 2;
          }
        }

      //Start Send Thread
      if(terminate == 0)
        {
        if(processor->StartProcessing(sender) != 0)
          {
          terminate += 4;
          }
        }

      //Colleced video frames and tracking matrices and forward them to the sender
      if(terminate == 0)
        {
        if(collector->StartCollecting(processor) != 0)
          {
          terminate += 8;
          }
        }

      //Wait for user input to terminate 4D-Ultrasound
      goodByeScreen();

      //Stop Collecting
      collector->StopCollecting();
      collector->Delete();

      //Stop Processing
      processor->StopProcessing();
      processor->Delete();

      //Stop Sending
      sender->StopSending();
      sender->Delete();

      //Close Sever Connection
      sender->CloseServerConnection();

      cout << endl;
      if(terminate == 0)
        {
        cout << "--- 4D Ultrasound finished ---" << endl;
        }
      else
        {
        cout << "--- 4D Ultrasound finished with ERRORS ---" << endl
             << "---           ERRORCODE: "<< terminate <<"             ---" << endl;
        }
      }
      logStream.close();
    }
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
         << "                                        (MANDATORY)"<<endl
         << "--reconstruct-volume or -rv:            Enable volume reconstruction" << endl
         << "--oigtl-server xxx or -os xxx:          Specify OpenIGTLink server"<<endl
         << "                                        (default: 'localhost')" << endl
         << "--oigtl-port xxx or -op xxx:            Specify OpenIGTLink port of"<<endl
         << "                                        server (default: 18944)" <<endl
         << "--nb-frames xxx or -n xxx:              Specify the number of frames"<<endl
         << "                                        that will be collected"<<endl
         << "                                        (default: 50)" << endl
         << "--frames-per-second xxx or -fps xxx:    Number of frames per second for"<<endl
         << "                                        the ultrasound data collection"<<endl
         << "                                        (default: 10)" << endl
         << "--video-source xxx or -vs xxx:          Set video source "<<endl
         << "                                        (default: '/dev/video0')" << endl
         << "--video-source-channel xxx or -vsc xxx: Set video source channel"<<endl
         << "                                        (default: 3)" << endl
         << "--video-mode xxx or -vm xxx:            Set video mode; "<<endl
         << "                                        Options: NTSC, PAL "<<endl
         << "                                        (default: NTSC)" << endl
         << "--scan-depth xxx or -sd xxx:            Set depth of ultrasound "<<endl
         << "                                        scan in Millimeter"<<endl
         << "                                        (default: 70mm)" <<endl
         << "--verbose or -v:                        Print more information" <<endl
         << endl
         << "--------------------------------------------------------------------------------"
         << endl  << endl;
}

//------------------------------------------------------------------------------
//
// Parse the command line options.
//
bool parseCommandLineArguments(int argc, char **argv, vtkDataCollector *collector, vtkDataProcessor *processor, vtkDataSender *sender)
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
            calibrationFileSpecified = true;
            }
          }
        //Enable volume reconstruction
        else if(currentArg == "--volume-reconstruction" || currentArg == "-vr")
          {
          processor->EnableVolumeReconstruction(true);
          }
        //OpenIGTLink Server
        else if(currentArg == "--oigtl-server" || currentArg == "-os")
          {
          sender->SetOIGTLServer(argv[++i]);
          }
        //OpenIGTLink Port at Server
        else if(currentArg == "--oigtl-port" || currentArg == "-op")
          {
          if( i < argc - 1)
            {
            sender->SetServerPort(atoi(argv[++i]));
            }
          }
        //Number of frames to grab
        else if(currentArg == "--nb-frames" || currentArg == "-n")
          {
          if( i < argc - 1)
            {
              int nbFrames = atoi(argv[++i]);
            if(nbFrames <= 0)
              {
              cout << "ERROR: Number of frames must be greater than 0 and not: " << nbFrames << endl;
              return false;
              }
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
            collector->SetScanDepth(scanDepth);
            }
          }
        else if(currentArg == "--verbose" || currentArg == "-v")
          {
          collector->SetVerbose(true);
          sender->SetVerbose(true);
          processor->SetVerbose(true);
          }
        else
           {
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
