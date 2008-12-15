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
#include "vtkUltrasoundVolumeReconstructor.h"
#include "vtkUltrasoundVolumeSender.h"
#include "vtkImageData.h"

using namespace std;

void printSplashScreen()
{

  cout << endl
       <<"************************************************" << endl
       <<"*                                              *" << endl
       <<"* SynchroGrab - Tracked 3D Ultrasound Imaging  *" << endl
       <<"*                                              *" << endl
       <<"* Date: Dec 2008                               *" << endl
       <<"*                                              *" << endl
       <<"************************************************" << endl;

}

void goodByeScreen()
{
 
 cout << endl;
 cout << "Terminate SynchroGrab" << endl
      << endl
      << "Bye Bye..." << endl <<endl << endl;

}

void printUsage()
{   
  printSplashScreen();
    
    cout << endl
         << "--------------------------------------------------------------------------------"
         << endl;

    // print instructions on how to use this program.
    cout << "DESCRIPTION : " << endl;
    cout << "This software does tracked 3D Ultrasound imaging. It captures frames from an"<< endl
         << "ultrasound device and tracking information from a tracker device." << endl
         << "The images and the tracking information are combined to a 3D volume image."<<endl
         << "The reconstructed volume is send to an OpenIGTLink server of choice," <<endl
         << " e.g. 3DSlicer at the end." << endl;
    cout << endl
         << "--------------------------------------------------------------------------------"
         << endl;
    cout << "OPTIONS: " << endl
         << "--calibration-file xxxx or -c xxxx: To specify the calibration file" << endl 
         << "--reconstruct-volume or -rv: Do volume reconstruction" << endl
         << "--oigtl-server or -os: Specify OpenIGTLink server (default: 'localhost')" << endl
         << "--oigtl-port or -op: Specify OpenIGTLink port of server" <<endl
         << "--nb-frames xxxx or -n xxxx: to specify the number of frames that " << endl 
         << "                            will be collected (default: 50)" << endl
         << "--fps: Number of frames per second for the ultrasound data collection" << endl
         << "      (default: 10)" << endl
         << "--video-source or -vs: Set video source (default: '/dev/video0')" << endl
         << "--video-source-channel or -vsc: Set video source channel (default: 3)" << endl
         << "--video-mode or -vm: Set video mode; Options: NTSC, PAL (default: NTSC)" << endl
         << endl
         << "--------------------------------------------------------------------------------"
         << endl  << endl; 
}


//
// Parse the command line options.
//
bool parseCommandLineArguments(int argc, char **argv, vtkUltrasoundVolumeReconstructor *reconstructor, vtkUltrasoundVolumeSender *sender)
{
    // needs to be supplied
    bool calibrationFileSpecified = false;

    for(int i=1; i < argc; i++)
        {
        string currentArg(argv[i]);
        if( currentArg == "--calibration-file" || currentArg == "-c")
          {
          if( i < argc - 1)
            {
            reconstructor->SetCalibrationFileName(argv[++i]);
            calibrationFileSpecified = true;
            }
          }
        else if(currentArg == "--reconstruct-volume" || currentArg == "-rv")
          {
          reconstructor->SetVolumeReconstructionEnabled(true);
          }
        else if(currentArg == "--oigtl-server" || currentArg == "-os")
          {
          sender->SetOIGTLServer(argv[++i]);
          }           
        else if(currentArg == "--oigtl-port" || currentArg == "-op")
          {
          if( i < argc - 1)
            {
            sender->SetServerPort(atoi(argv[++i]));
            }
          }
        else if(currentArg == "--nb-frames" || currentArg == "-n") 
          {
          if( i < argc - 1)
            {
            reconstructor->SetNbFrames(atoi(argv[++i]));
            }
          }
        else if(currentArg == "--frames-per-second" || currentArg == "-fps") 
          {
          if( i < argc - 1)
            {
            reconstructor->SetFrameRate(atof(argv[++i]));
            }
          }
       else if(currentArg == "--video-source" || currentArg == "-vs")
          {
          reconstructor->SetVideoDevice(argv[++i]);
          }
       else if(currentArg == "--video-source-channel" || currentArg == "-vsc")
          {
      reconstructor->SetVideoChannel(atoi(argv[++i]));
          }
       else if(currentArg == "--video-mode" || currentArg == "-vm")
          {
          string videoMode (argv[++i]);
          if(videoMode == "NTSC")
            {
            reconstructor->SetVideoMode(1);            
            }
          else if(videoMode == "PAL")
            {
            reconstructor->SetVideoMode(2);            
            }
          else
            {
            printSplashScreen();
            cout << "ERROR: False video mode: " << videoMode  << endl
                 << "       Available video modes: NTSC and PAL" << endl;
            goodByeScreen();
            return false;
            }
          }
       else 
          {
          printUsage();
          cout << "ERROR: Unrecognized command: '" << argv[i] <<"'" << endl;         
          goodByeScreen();
          return false;
          }
      }
    return calibrationFileSpecified;
}

/******************************************************************************
 * 
 * MAIN
 * 
 ******************************************************************************/
int main(int argc, char **argv)
{ 
  
    vtkUltrasoundVolumeReconstructor *reconstructor = vtkUltrasoundVolumeReconstructor::New();
    vtkUltrasoundVolumeSender *sender = vtkUltrasoundVolumeSender::New();
    
    //Read command line arguments
    bool successParsingCommandLine = parseCommandLineArguments(argc,argv,reconstructor, sender);
    if(!successParsingCommandLine)
        return -1;
    
    printSplashScreen();
    cout << "--- Started ---" << endl << endl;

    
    //     redirect vtk errors to a file
    vtkFileOutputWindow *errOut = vtkFileOutputWindow::New();
    errOut->SetFileName("vtkError.txt");
    vtkOutputWindow::SetInstance(errOut);
    
    //Configure Reconstructor
    if(reconstructor->ConfigurePipeline()){
        cerr << "ERROR: Could not configure UltrasoundVolumeReconstructor" << endl;
        return -1;
    };
    
    vtkImageData *ImageBuffer = vtkImageData::New();

    // Volume Reconstruction
    if(reconstructor->GetVolumeReconstructionEnabled())
      {
      if(!reconstructor->ReconstructVolume(ImageBuffer))
        {
        goodByeScreen();
        return -1;
        }
      
      // Connect to OpenIGTLink Server
      if(!sender->ConnectToServer())
        {
        goodByeScreen();
        return -1;
        }

      //Send volume
      if(!sender->SendImages(ImageBuffer, 1))
        {
        goodByeScreen();
        return -1;
        }

      //Close Sever Connection
      if(!sender->CloseServerConnection())
        {
        goodByeScreen();
        return -1;
        }
      }


    //Free Memory
    ImageBuffer->Delete();
    reconstructor->Delete();
    sender->Delete();

    cout << endl;
    cout << "--- Synchgrograb finished ---" << endl;
    goodByeScreen();

}
