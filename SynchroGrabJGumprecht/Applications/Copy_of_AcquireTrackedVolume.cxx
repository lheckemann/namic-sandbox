

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

// for error redirection
#include "vtkFileOutputWindow.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkNDICertusTracker.h"

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
         << "-fps: Number of frames per second for the ultrasound data collection" << endl
         << "      (default 30)" << endl;
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
};

bool parseCommandLineArguments(int argc, char **argv, CommandLineArguments &args)
{
    // setting default values
    args.nbFrames=150;
    args.outputFile = "outputVol.vtk";
    args.sonixAddr = "127.0.0.1";
    args.fps = 30;

    bool calibrationFileSpecified = false;

    for(int i=1; i < argc; i++)
        {
        string currentArg(argv[i]);
        if( currentArg == "--calibration-file" || currentArg == "-c")
            {
            if( i < argc - 1)
                args.calibrationFile = argv[++i];
        calibrationFileSpecified = true;
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
  errOut->SetFileName("VTKERROR.txt");
  vtkOutputWindow::SetInstance(errOut);

  // limit the number threads for debugging
  vtkMultiThreader::SetGlobalMaximumNumberOfThreads(1);

    // read the calibration file (do it before the acquisition to avoid wasting 
    // an acquisition because of a missing (or erroneous) calibration file.
    vtkUltrasoundCalibFileReader *calibReader = vtkUltrasoundCalibFileReader::New();
    calibReader->SetFileName(args.calibrationFile.c_str());
    calibReader->ReadCalibFile();

    // TODO: it seems that there is very little error handling done in the calibration reading class
    // we should probably make this more robust before distributing the code

    // set up the video source (ultrasound machine)
    vtkSonixVideoSource *sonixGrabber = vtkSonixVideoSource::New();
    sonixGrabber->SetSonixIP((char*)args.sonixAddr.c_str());
    sonixGrabber->SetFrameRate(args.fps);
    // TODO: use enum types constants or use an enum type instead of litterals (to improve code clarity).
    sonixGrabber->SetImagingMode(0);
    sonixGrabber->SetAcquisitionDataType(0x00000004);
    sonixGrabber->SetFrameBufferSize(args.nbFrames);

// temp
//    sonixGrabber->SetDataOrigin(calibReader->GetImageOrigin());
//    sonixGrabber->SetDataSpacing(calibReader->GetImageSpacing());
// temp

    int *imSize = calibReader->GetImageSize();
    sonixGrabber->SetFrameSize(imSize[0], imSize[1], 1);

    // set up the tracker
    vtkNDICertusTracker *tracker = vtkNDICertusTracker::New();
    if(tracker->Probe() != 1)
        {
        cerr << "Tracking system not found" << endl;
        return -1;
        }

    // make sure the tracking buffer is large enough for the number of the image sequence requested
    vtkTrackerTool *tool = tracker->GetTool(0);
    tool->GetBuffer()->SetBufferSize( args.nbFrames * CERTUS_UPDATE_RATE / args.fps * FUDGE_FACTOR ); 

    tracker->StartTracking();

    // IsMissing does not seam to work at the moment, so I added to && falses to skip it
    // TODO : debug the IsMissing failure to indicate anything else than true s
    if(tracker->GetTool(0)->IsMissing() && false) 
        {
        cerr << "No tool found in the tracking system field of view" << endl;
        return -1;
        }

    // TODO: control the size of the TrackerBuffer associate with the tool
    // TODO: make the tool selection configurable
    //vtkTrackerTool *tool = tracker->GetTool(0);


    // Setting up the synchronization filter
    vtkTaggedImageFilter *tagger = vtkTaggedImageFilter::New();
    tagger->SetVideoSource(sonixGrabber);
    tagger->SetTrackerTool(tool);

 // temp
//    tagger->SetCalibrationMatrix(calibReader->GetCalibrationMatrix());
 // temp

    // We don't have any temporal tion done yet... 
    // TODO : add a time lag option in the command line argumentss
    tagger->SetTemporalLag(0);

    tagger->Initialize();

    sonixGrabber->Record();  //start recording frame from the video

    // wait for the images (delay in milliseconds)
    Sleep(args.nbFrames / args.fps * 1000);

    sonixGrabber->Stop();
    tracker->StopTracking();

    // temp
    cout << "Recorded synchronized transforms and frames for " << args.nbFrames / args.fps * 1000 << "ms" << endl;
    // temp

    // set up the panoramic reconstruction class
  vtk3DPanoramicVolumeReconstructor *panoramaReconstructor = vtk3DPanoramicVolumeReconstructor::New();

// temp
//    panoramaReconstructor->SetOptimization(2);
  panoramaReconstructor->SetOptimization(1);
// temp

    panoramaReconstructor->CompoundingOn();
    panoramaReconstructor->SetInterpolationModeToLinear();
  panoramaReconstructor->GetOutput()->SetScalarTypeToUnsignedChar();

    // TODO : convert this fonction call to use GetTransform instead from the TaggedImageFilter
    panoramaReconstructor->SetTrackerTool(tagger->GetTrackerTool());

    // Determine the extent of the volume that needs to be reconstructed by 
    // iterating throught all the acquired frames

  // TODO: provide level of configurability for the clip rectangle.
  double clipRectangle[4];

// temp
//  calibReader->GetClipRectangle(clipRectangle);
//    panoramaReconstructor->SetClipRectangle(clipRectangle);
//  double xmin = clipRectangle[0], ymin = clipRectangle[1], 
//        xmax = clipRectangle[2], ymax = clipRectangle[3];
  double xmin=0, ymin=0, xmax=639, ymax=399;
// temp

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

    // TODO (maybe) provide some configurability for this parameter
    // temp
  // double spacing[3] = {0.5,0.5,0.5};
  double spacing[3] = {1.0, 1.0, 1.0};
  // temp
    int volumeExtent[6] = { 0, (int)( (maxX - minX) / spacing[0] ), 
                            0, (int)( (maxY - minY) / spacing[1] ), 
                            0, (int)( (maxZ - minZ) / spacing[2] ) };
    panoramaReconstructor->SetOutputExtent(volumeExtent);
    panoramaReconstructor->SetOutputSpacing(spacing);
  // temp
    panoramaReconstructor->SetOutputOrigin(minX, minY, minZ);
  // temp

  // temp
    // Note/TODO : This should probably be done by default in vtk3DPanoramicVolumeReconstructor
    //vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    //panoramaReconstructor->SetSliceAxes(matrix);
  // temp

  //
    // rewind and add the slices the panoramaReconstructor
  //

  // Use a cast filter since the panorama reconstruction class 
  // wants to have the same data in input and output
  //vtkImageCast *castFilter = vtkImageCast::New();
  //castFilter->SetInput(tagger->GetOutput());
  // castFilter->SetOutputScalarTypeToDouble();
    //panoramaReconstructor->SetSlice(castFilter->GetOutput());

  panoramaReconstructor->SetSlice(tagger->GetOutput());
    sonixGrabber->Rewind();
    sonixGrabber->GetOutput()->Update();
    
  // no sure why but this line crashes the program
  // temp
  //panoramaReconstructor->GetOutput()->Update();
  // temp

  // temp 
  // writing sonix rp images to a png (to make sure the image are gettting through the pipelin)
  vtkJPEGWriter *imWriter = vtkJPEGWriter::New();
  imWriter->SetInput(tagger->GetOutput());
  // temp

    for(int i=0; i < nbFramesGrabbed; i++)
        {
    // temp
//    tagger->Print( vtkstd::cout );
//    tagger->GetOutput()->Print( vtkstd::cout );
      tagger->GetOutput()->SetOrigin(calibReader->GetImageOrigin());
      tagger->GetOutput()->SetSpacing(calibReader->GetImageSpacing());
    // temp

    // temp
    ostringstream stringStream;
    stringStream << "testIm" << i << ".jpg";
    imWriter->SetFileName(stringStream.str().c_str());
    imWriter->Write();
    // temp


        panoramaReconstructor->InsertSlice();
        sonixGrabber->Seek(1);
        }

    // temp
    cout << "inserted " << panoramaReconstructor->GetPixelCount() << " pixels into the output volume" << endl;
    cout << "channels: " << panoramaReconstructor->GetOutput()->GetNumberOfScalarComponents() << endl;
    // temp

/*
print "inserted", frames, "frames into output volume"
print "inserted", freehand.GetPixelCount(), "pixels into output volume"
print "channels:", freehand.GetOutput().GetNumberOfScalarComponents()

*/

    // fill small holes in the reconstructed volume.

// temp : tend to crash the program with more than 10 frames are collected.
    panoramaReconstructor->FillHolesInOutput();

    // get rid of the alpha channel
    vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
    extract->SetInput(panoramaReconstructor->GetOutput());
    extract->SetComponents(0);

    // write it to the specified file.
    vtkDataSetWriter *writer3D = vtkDataSetWriter::New();
    writer3D->SetFileTypeToBinary();
    writer3D->SetInput(extract->GetOutput());
    writer3D->SetFileName(args.outputFile.c_str());
    writer3D->Write();

    // temp
    cout << args.calibrationFile << endl;
    cout << args.nbFrames << endl;
    cout << args.outputFile << endl;
    cout << args.sonixAddr << endl;
    cout << args.fps << endl;
    // temp


    // TODO: make sure the delete operations matches the calls to xxx::New()

    calibReader->Delete();

    sonixGrabber->ReleaseSystemResources();
    sonixGrabber->Delete();
}
