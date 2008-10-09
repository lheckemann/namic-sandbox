

// to do : specify the copyright info
// to do : specify licensing info

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

#include <iostream>
#include <sstream>
#include <limits>
#include <string>

#define NOMINMAX 
#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkFileOutputWindow.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkMultiThreader.h"
#include "vtkSonixVideoSource.h"
#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkNDICertusTracker.h"

#include "utx_imaging_modes.h"
#include "ulterius_def.h"

#include "igtlclientsocket.h"
#include "igtlImageMessage.h"
#include "igtlMath.h"

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625

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
// Parse the command line options and put them on a customs struct.
//
struct CommandLineArguments 
{ 
    int nbFrames;
    float fps;
    string outputFile;
    string sonixAddr;
    string calibrationFile;
    string oigtlServer;
    int oigtlPort;
    bool oigtlTransfertImages;
    bool oigtlTransfertVolumes;
    bool useTrackerTransforms;
};

bool parseCommandLineArguments(int argc, char **argv, CommandLineArguments &args)
{
    // setting default values
    args.nbFrames=150;
    args.outputFile = "outputVol.vtk";
    args.sonixAddr = "127.0.0.1";
    args.oigtlServer = "127.0.0.1";
    args.oigtlPort = 18944;
    args.fps = 10;
    args.oigtlTransfertImages=false;
    args.oigtlTransfertVolumes=false;
    args.useTrackerTransforms=false;

    bool calibrationFileSpecified = false;

    for(int i=1; i < argc; i++)
        {
        string currentArg(argv[i]);
        if( currentArg == "--calibration-file" || currentArg == "-c")
            {
            if( i < argc - 1)
                {
                args.calibrationFile = argv[++i];
                calibrationFileSpecified = true;
                }
            }
        else if(currentArg == "--oigtl-transfert-images")
            {
            args.oigtlTransfertImages = true;
            args.oigtlTransfertVolumes = false;
            }
        else if(currentArg == "--oigtl-transfert-volumes")
            {
            args.oigtlTransfertVolumes = true;
            args.oigtlTransfertImages = false;
            }
        else if(currentArg == "--oigtl-port")
            {
            if( i < argc - 1)
                args.oigtlPort = atoi(argv[++i]);
            }
        else if(currentArg == "--use-tracker-transforms")
            {
            args.useTrackerTransforms = true;
            }
        else if(currentArg == "--output-file" || currentArg == "-o")
            {
            if( i < argc - 1)
                args.outputFile = argv[++i];
            }
        else if(currentArg == "--sonix-addr" || currentArg == "-a") 
            {
            if( i < argc - 1)
                args.sonixAddr = argv[++i];
            }
        else if(currentArg == "--nb-frames" || currentArg == "-n") 
            {
            if( i < argc - 1)
                args.nbFrames = atoi(argv[++i]);
            }
        else if(currentArg == "--fps") 
            {
            if( i < argc - 1)
                args.fps = atof(argv[++i]);
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

int main(int argc, char **argv)
{
    CommandLineArguments args;
    bool successParsingCommandLine = parseCommandLineArguments(argc,argv,args);
    if(!successParsingCommandLine)
        return -1;

    // redirect vtk errors to a file
    vtkFileOutputWindow *errOut = vtkFileOutputWindow::New();
    errOut->SetFileName("vtkError.txt");
    vtkOutputWindow::SetInstance(errOut);

    // limit the number threads for debugging
    vtkMultiThreader::SetGlobalMaximumNumberOfThreads(1);

    // read the calibration file
    vtkUltrasoundCalibFileReader *calibReader = vtkUltrasoundCalibFileReader::New();
    calibReader->SetFileName(args.calibrationFile.c_str());
    calibReader->ReadCalibFile();

    // set up the video source (ultrasound machine)
    vtkSonixVideoSource *sonixGrabber = vtkSonixVideoSource::New();
    sonixGrabber->SetSonixIP((char*)args.sonixAddr.c_str());
    sonixGrabber->SetFrameRate(args.fps);
    sonixGrabber->SetImagingMode(BMode);
    sonixGrabber->SetAcquisitionDataType(udtBPost);
    sonixGrabber->SetFrameBufferSize(args.nbFrames);

    double *imageOrigin = calibReader->GetImageOrigin();
    sonixGrabber->SetDataOrigin(imageOrigin);
    double *imageSpacing = calibReader->GetImageSpacing();
    sonixGrabber->SetDataSpacing(imageSpacing);

    int *imSize = calibReader->GetImageSize();
    sonixGrabber->SetFrameSize(imSize[0], imSize[1], 1);

    // Setting up the synchronization filter
    vtkTaggedImageFilter *tagger = vtkTaggedImageFilter::New();
    tagger->SetVideoSource(sonixGrabber);

    // set up the tracker if necessary
    vtkNDICertusTracker *tracker;
    if( args.useTrackerTransforms)
        {
        tracker = vtkNDICertusTracker::New();
        if(tracker->Probe() != 1)
            {
            cerr << "Tracking system not found" << endl;
            return -1;
            }
        // make sure the tracking buffer is large enough for the number of the image sequence requested
        vtkTrackerTool *tool = tracker->GetTool(0);
        tool->GetBuffer()->SetBufferSize( args.nbFrames * CERTUS_UPDATE_RATE / args.fps * FUDGE_FACTOR ); 
        tracker->StartTracking();

        tagger->SetTrackerTool(tool);
        tagger->SetCalibrationMatrix(calibReader->GetCalibrationMatrix());
        }

    tagger->Initialize();

    if(args.oigtlTransfertImages)
        {
        
        // Opening an OpenIGTLink socket
        igtl::ClientSocket::Pointer socket = NULL;
        socket = igtl::ClientSocket::New();
        int e = socket->ConnectToServer(args.oigtlServer.c_str(),args.oigtlPort);

        if(e != 0)
            {
            cout << "Failed to connect the OpenIGTLink socket to the server.  Error code : " << e << endl;
            return -1;
            }
        else
            cout << "Successful connection to the OpenIGTLink server" << endl;

        // Start the video source and configure an image frame
        sonixGrabber->Record();
        sonixGrabber->Update();

        igtl::ImageMessage::Pointer imMessage = igtl::ImageMessage::New();
        imMessage->SetDeviceName("SonixRP");
        vtkImageData *image = sonixGrabber->GetOutput();
        int *dim = image->GetDimensions();
        double *spacing = image->GetSpacing();

        cout << "Dimensions " << dim[0] << " " << dim[1] << " " << endl;
        cout << "Spacing " << spacing[0] << " " << spacing[1] << " " << spacing[2] << endl;
        cout << "Data dimension : " << image->GetDataDimension() << endl;
        cout << "Number of scalar components : " << image->GetNumberOfScalarComponents() << endl;
        cout << "Scalar Type : " << image->GetScalarType() << " ( " << image->GetScalarTypeAsString() << ")" << endl;

        imMessage->SetDimensions(dim[0], dim[1], 1);
        imMessage->SetSpacing(spacing[0], spacing[1], spacing[2]);
        imMessage->SetScalarType(image->GetScalarType());
        imMessage->AllocateScalars();

        int svd[3];
        int svoff[3];
        imMessage->GetSubVolume(svd, svoff);

        cout << "SubVolume dimensions : " << svd[0] << " " << svd[1] << " " << svd[2] << endl;
        cout << "SubVolume offsets : " << svoff[0] << " " << svoff[1] << " " << svoff[2] << endl;

        // Setup a transform matrix for the OpenIGTLink message
        igtl::Matrix4x4 igtlMatrix = {{1.0, 0.0, 0.0, 0.0},{0.0, 1.0, 0.0, 0.0},{0.0, 0.0, 1.0, 0.0},{0.0, 0.0, 0.0, 1.0}};
        vtkMatrix4x4 *vtkMat = vtkMatrix4x4::New();

        for( int i=0; i < args.nbFrames; i++)
            {
            Sleep(1/args.fps*1000);
            tagger->Update();

            tagger->GetTransform()->GetMatrix(vtkMat);
            for(int i=0;i<4;i++)
                for(int j=0;j<4;j++)
                    igtlMatrix[i][j]=(*vtkMat)[i][j];
            imMessage->SetMatrix(igtlMatrix);

            unsigned char* imgPtr = (unsigned char*)tagger->GetOutput()->GetScalarPointer();
            int imSize = imMessage->GetImageSize();

            // copy the image raw data 
            memcpy(imMessage->GetScalarPointer(), imgPtr, imSize);
            imMessage->Pack();
            
            // sending this frame to the server
            std::cout << "PackSize:  " << imMessage->GetPackSize() << std::endl;
            std::cout << "BodyMode:  " << imMessage->GetBodyType() << std::endl;

            int ret = socket->Send(imMessage->GetPackPointer(), imMessage->GetPackSize());
            if (ret == 0)
                {
                std::cerr << "Error : Connection Lost!\n";
                return -1;
                }
            else 
                cout << "image frame send successfully" << endl;
            }

        // closing the connection 
        socket->CloseSocket();
        }
    else
        {
        sonixGrabber->Record();  //start recording frame from the video

        // wait for the images (delay in milliseconds)
        Sleep(args.nbFrames / args.fps * 1000);

        sonixGrabber->Stop();

        tracker->StopTracking();

        cout << "Recorded synchronized transforms and ultrasound images for " << args.nbFrames / args.fps * 1000 << "ms" << endl;

        // set up the panoramic reconstruction class
        vtk3DPanoramicVolumeReconstructor *panoramaReconstructor = vtk3DPanoramicVolumeReconstructor::New();

        panoramaReconstructor->CompoundingOn();
        panoramaReconstructor->SetInterpolationModeToLinear();
        panoramaReconstructor->GetOutput()->SetScalarTypeToUnsignedChar();

        // Determine the extent of the volume that needs to be reconstructed by 
        // iterating throught all the acquired frames
        double clipRectangle[4];
        calibReader->GetClipRectangle(clipRectangle);
        panoramaReconstructor->SetClipRectangle(clipRectangle);
        double xmin = clipRectangle[0], ymin = clipRectangle[1], 
               xmax = clipRectangle[2], ymax = clipRectangle[3];

        double imCorners[4][4]= { 
            { xmin, ymin, 0, 1}, 
            { xmin, ymax, 0, 1},
            { xmax, ymin, 0, 1},
            { xmax, ymax, 0, 1} };

        double transformedPt[4];

        double maxX, maxY, maxZ;
        maxX = maxY = maxZ = -1 * numeric_limits<double>::max();
        double minX, minY, minZ;
        minX = minY = minZ = numeric_limits<double>::max();

        int nbFramesGrabbed = sonixGrabber->GetFrameCount();

        sonixGrabber->Rewind();
        for(int i=0; i < nbFramesGrabbed; i++)
            {
            // get those transforms... and computer the bounding box
            tagger->Update();

            // determine the bounding box occupied by the reconstructed volume
            for(int j=0; j < 4; j++)
                {
                tagger->GetTransform()->MultiplyPoint(imCorners[j],transformedPt);
                minX = min(transformedPt[0], minX);
                minY = min(transformedPt[1], minY);
                minZ = min(transformedPt[2], minZ);
                maxX = max(transformedPt[0], maxX);
                maxY = max(transformedPt[1], maxY);
                maxZ = max(transformedPt[2], maxZ);
                }
            sonixGrabber->Seek(1);
            }

        double spacing[3] = {0.5,0.5,0.5};
        int volumeExtent[6] = { 0, (int)( (maxX - minX) / spacing[0] ), 
                                0, (int)( (maxY - minY) / spacing[1] ), 
                                0, (int)( (maxZ - minZ) / spacing[2] ) };

        panoramaReconstructor->SetOutputExtent(volumeExtent);
        panoramaReconstructor->SetOutputSpacing(spacing);
        panoramaReconstructor->SetOutputOrigin(minX, minY, minZ);

        // rewind and add the slices the panoramaReconstructor
        panoramaReconstructor->SetSlice(tagger->GetOutput());
        panoramaReconstructor->GetOutput()->Update();
        sonixGrabber->Rewind();
        tagger->Update();

        vtkMatrix4x4 *sliceAxes = vtkMatrix4x4::New();
        tagger->GetTransform()->GetMatrix(sliceAxes);
        panoramaReconstructor->SetSliceAxes(sliceAxes);

        for(int i=0; i < nbFramesGrabbed; i++)
            {
            tagger->Update();
            tagger->GetTransform()->GetMatrix(sliceAxes);
            panoramaReconstructor->SetSliceAxes(sliceAxes);
            panoramaReconstructor->InsertSlice();
            sonixGrabber->Seek(1);
            }

        cout << "Inserted " << panoramaReconstructor->GetPixelCount() << " pixels into the output volume" << endl;

        panoramaReconstructor->FillHolesInOutput();

        // To remove the alpha channel of the reconstructed volume
        vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
        extract->SetInput(panoramaReconstructor->GetOutput());
        extract->SetComponents(0);

        // write it to the specified file.
        vtkDataSetWriter *writer3D = vtkDataSetWriter::New();
        writer3D->SetFileTypeToBinary();
        writer3D->SetInput(extract->GetOutput());
        writer3D->SetFileName(args.outputFile.c_str());
        writer3D->Write();

        writer3D->Delete();
        extract->Delete();
        panoramaReconstructor->Delete();
    }

    sonixGrabber->ReleaseSystemResources();
    sonixGrabber->Delete();
    calibReader->Delete();
    errOut->Delete();
}
