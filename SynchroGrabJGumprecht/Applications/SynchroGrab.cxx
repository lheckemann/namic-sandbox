/*========================================================================= 
Module:  $RCSfile: SynchroGrab.cxx,v $ 
Author:  Jonathan Boisvert, Queens School Of Computing
Author: Jan Gumprecht, Nobuhiko Hata, Harvard Medical sChool
 
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
#include "vtkMultiThreader.h"
#include "vtkSynchroGrabPipeline.h"
//#include "vtkMINCImageReader.h"
#include "vtkImageData.h"
//#include "vtkImageReader2.h"
#include "vtkDICOMImageReader.h"

using namespace std;

void printUsage()
{
    // print instructions on how to use this program.
    cout << "Description : " << endl;
    cout << "This humble program connects to a sonix RP machine runnig the standard" << endl <<
        "clinical software (sonix RP) and to a Certus which is used to track the " << endl << 
        "position and orientation of the ultrasound probe.  The calibration " << endl << 
        " information (relation between the ultrasound image space and the" << endl <<
        "dynamic reference object tracked by the Certus) have to be supplied by" << endl << 
        "configuration file.  Once a predefined number of frames have been " << endl << 
        "recorded, the application creates the 3D volume and exists." << endl << endl;

    cout << "Options: " << endl
         <<  "--calibration-file xxxx or -c xxxx: to specify the calibration file" << endl 
         <<  "--output-file xxxx or -o xxxx: to specify the ouput file" << endl
         <<  "                               (default value: outputVol.vtk)" << endl
         << "--sonix-addr xxxx or -a xxxx: to specify the ip adress of the sonix " << endl
         << "                              RP machine (default value : 127.0.0.1)" << endl
         << "--nb-frames xxxx or -n xxxx: to specify the number of frames that " << endl 
         << "                            will be collected (default: 150)" << endl
         << "--fps: Number of frames per second for the ultrasound data collection" << endl
         << "      (default 10)" << endl;
}


//
// Parse the command line options.
//
bool parseCommandLineArguments(int argc, char **argv, vtkSynchroGrabPipeline *pipeline)
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
                pipeline->SetCalibrationFileName(argv[++i]);
                calibrationFileSpecified = true;
                }
            }
        else if(currentArg == "--oigtl-transfert-images")
            {
            pipeline->SetTransfertImages(true);
            pipeline->SetVolumeReconstructionEnabled(false);
            }
        else if(currentArg == "--oigtl-transfert-volumes")
            {
            pipeline->SetVolumeReconstructionEnabled(true);
            pipeline->SetTransfertImages(false);
            }
        else if(currentArg == "--oigtl-port")
            {
            if( i < argc - 1)
                pipeline->SetServerPort(atoi(argv[++i]));
            }
        else if(currentArg == "--use-tracker-transforms")
            {
            pipeline->SetUseTrackerTransforms(true);
            }        
        else if(currentArg == "--sonix-addr" || currentArg == "-a") 
            {
            if( i < argc - 1)
                pipeline->SetSonixAddr(argv[++i]);
            }
        else if(currentArg == "--nb-frames" || currentArg == "-n") 
            {
            if( i < argc - 1)
                pipeline->SetNbFrames(atoi(argv[++i]));
            }
        else if(currentArg == "--fps") 
            {
            if( i < argc - 1)
                pipeline->SetFrameRate(atof(argv[++i]));
            }
       else if(currentArg == "--reconstruct-volume")
            {
            pipeline->SetVolumeReconstructionEnabled(true);
            pipeline->SetTransfertImages(true);
            }           
        else 
            {
            printUsage();
            cout << "Unrecognized command: " << argv[i] << endl;
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


    vtkSynchroGrabPipeline *pipeline = vtkSynchroGrabPipeline::New();

    //NH
    //12/2/08
    // the following line is unkowon leak detected vtk
    // removed temporary to avoid seg fault
    //bool successParsingCommandLine = parseCommandLineArguments(argc,argv,pipeline);
    //if(!successParsingCommandLine)
    //    return -1;
    
    //line remal end. NH 12/2/08
    
    
    //     redirect vtk errors to a file
    vtkFileOutputWindow *errOut = vtkFileOutputWindow::New();
    errOut->SetFileName("vtkError.txt");
    vtkOutputWindow::SetInstance(errOut);
    
    pipeline->ConfigurePipeline();
    // Volume Reconstruction
if(pipeline->GetVolumeReconstructionEnabled())
 {if(!pipeline->ReconstructVolume())return -1;}
    // Transfer Images
    if(pipeline->GetTransfertImages())
      {
if(!pipeline->ConnectToServer())return -1;
if(!pipeline->SendImages())return -1;
if(!pipeline->CloseServerConnection())return -1;
      }
    pipeline->Delete();
}
