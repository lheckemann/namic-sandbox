/*========================================================================

Module:  $RCSfile: vtkUltrasoundVolumeReconstructor.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada All rights reserved.
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

#include <iostream>
#include <sstream>
#include <limits>
#include <string>

#define NOMINMAX
//#define REMOVE_ALPHA_CHANNEL
#define DEBUG_IMAGES //Write tagger output to HDD
//#define DEBUG_MATRICES //Prints tagger matrices to stdout
 
//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkBMPWriter.h"
#include "vtkMatrix4x4.h"


#include "vtkObjectFactory.h"
#include "vtkUltrasoundVolumeReconstructor.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#ifdef USE_TRACKER_DEVICE
#include "vtkNDITracker.h"
#else
#include "vtkTrackerSimulator.h"
#endif

#ifdef USE_ULTRASOUND_DEVICE
#include "vtkV4L2VideoSource.h"
#else
#include "vtkVideoSourceSimulator.h"
#endif

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625
#define US_IMAGE_FAN_RATIO 0.6 //Amount of ultrasound images's height that
                               //is used by the ultrasound fan

using namespace std;

static inline void vtkSleep(double duration);

vtkCxxRevisionMacro(vtkUltrasoundVolumeReconstructor, "$Revision$");
vtkStandardNewMacro(vtkUltrasoundVolumeReconstructor);

//----------------------------------------------------------------------------
vtkUltrasoundVolumeReconstructor::vtkUltrasoundVolumeReconstructor()
{
#ifdef USE_ULTRASOUND_DEVICE
  this->NbFrames = 50 + 100; //# of Frames to capture + additional 100 frames which are skipped
#else
  this->NbFrames = 50;
#endif

  this->FrameRate = 30;
  this->ScanDepth = 70; //Unit: mm

  this->CalibrationFileName = NULL;

// Temporal change to test simulation mode working in windows
#ifdef USE_ULTRASOUND_DEVICE
  char* devicename = "/dev/video0";
  this->SetVideoDevice(devicename);
  this->SetVideoChannel(3); //S-Video at Hauppauge Impact VCB Modell 558
  this->SetVideoMode(1); //NTSC
#endif


  this->calibReader = vtkUltrasoundCalibFileReader::New();

  this->Verbose = false;
  
#ifdef USE_ULTRASOUND_DEVICE
  this->sonixGrabber = vtkV4L2VideoSource::New();
#else
  this->sonixGrabber = vtkVideoSourceSimulator::New();
#endif //USE_ULTRASOUND_DEVICE 
  
  this->tagger = vtkTaggedImageFilter::New();

#ifdef USE_TRACKER_DEVICE
  this->tracker = vtkNDITracker::New();
#else
  this->tracker = vtkTrackerSimulator::New();
#endif //USE_TRACKER_DEVICE

}

//----------------------------------------------------------------------------
vtkUltrasoundVolumeReconstructor::~vtkUltrasoundVolumeReconstructor()
{

  this->tracker->Delete();
  this->sonixGrabber->ReleaseSystemResources();
  this->sonixGrabber->Delete();
  this->tagger->Delete();
  this->calibReader->Delete();
  this->SetCalibrationFileName(NULL);
}
//----------------------------------------------------------------------------
void vtkUltrasoundVolumeReconstructor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeReconstructor::ConfigurePipeline()
{
  this->calibReader->SetFileName(this->CalibrationFileName);
  this->calibReader->ReadCalibFile();

#ifdef USE_ULTRASOUND_DEVICE
  this->sonixGrabber->SetVideoDevice(this->GetVideoDevice());
  this->sonixGrabber->SetVideoChannel(this->GetVideoChannel());
  this->sonixGrabber->SetVideoMode(this->GetVideoMode());
#endif

  // set up the video source (ultrasound machine)  
  this->sonixGrabber->SetFrameRate(this->FrameRate);  
  this->sonixGrabber->SetFrameBufferSize(this->NbFrames);
  
  double *imageOrigin = this->calibReader->GetImageOrigin();
  this->sonixGrabber->SetDataOrigin(imageOrigin);
  double *imageSpacing = this->calibReader->GetImageSpacing();
  this->sonixGrabber->SetDataSpacing(imageSpacing);
  
  int *imSize = this->calibReader->GetImageSize();
  this->sonixGrabber->SetFrameSize(imSize[0], imSize[1], 1);
  //
  // Setting up the synchronization filter
  this->tagger->SetVideoSource(sonixGrabber);
  
  // set up the tracker if necessary
  bool error = this->StartTracker();    
  
  this->tagger->Initialize();

  return error;
}
//
//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeReconstructor::StartTracker()
{
  if(this->tracker->Probe() != 1)
    {
    cerr << "ERROR: Tracking system not found" << endl;
    return false;
    }

  // make sure the tracking buffer is large enough for the number of the image sequence requested
  vtkTrackerTool *tool = this->tracker->GetTool(0);
  tool->GetBuffer()->SetBufferSize((int) (this->NbFrames * CERTUS_UPDATE_RATE / this->FrameRate * FUDGE_FACTOR + 0.5) ); 
  this->tracker->StartTracking();
  this->tagger->SetTrackerTool(tool);
  this->tagger->SetCalibrationMatrix(this->calibReader->GetCalibrationMatrix());

  return true;
}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeReconstructor::ReconstructVolume(vtkImageData * Volume)
{
  cout << "Hardware Initialization: " << std::flush;
  cout << '\a' << std::flush;
  for(int i = 0; i < 11; i++)
    {
#ifdef USE_TRACKER_DEVICE
      vtkSleep(1);
#else
      vtkSleep(0.2);
#endif
      cout << 10 - i << " " << std::flush;
    }
  cout << endl;

  cout << "Start Recording" << endl;
  cout << '\a' << std::flush;
  vtkSleep(0.2);
  cout << '\a' << std::flush; 

  this->sonixGrabber->Record();  //Start recording frame from the video
   
  for (int i = 0; i < (int) (this->NbFrames / this->FrameRate + 0.5) * 2; i++)
    {
    cout << "*" << std::flush;

#ifdef USE_TRACKER_DEVICE || USE_ULTRASOUND_DEVICE
    cout << '\a' << std::flush;//Only beep when we use no simulator
#endif

    vtkSleep(0.5);//Wait half a second for the next beep
    }
  cout << endl;

  this->sonixGrabber->Stop();//Stop recording

  this->tracker->StopTracking();//Stop tracking

  cout << "Stopped Recording" << endl;
  cout << '\a' << std::flush;
  vtkSleep(0.2);
  cout << '\a' << std::flush;

  // set up the panoramic reconstruction class
  vtk3DPanoramicVolumeReconstructor *panoramaReconstructor = vtk3DPanoramicVolumeReconstructor::New();
  panoramaReconstructor->CompoundingOn();
  panoramaReconstructor->SetInterpolationModeToLinear();
  //panoramaReconstructor->GetOutput()->SetScalarTypeToUnsignedChar();

  // Determine the extent of the volume that needs to be reconstructed by 
  // iterating throught all the acquired frames
  double clipRectangle[4];
  this->calibReader->GetClipRectangle(clipRectangle);
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

  if(nbFramesGrabbed == 0)
    {
    cerr << "ERROR: Grabbed no frames" << endl;
    return false;        
    }
  else
    {
    cout << "Recording successful" << endl;
    if(Verbose)
      {
      cout << "  Recorded synchronized transforms and ultrasound images for " << ((int) this->NbFrames / this->FrameRate + 0.5) << "s" << endl;
      cout << "  Grabbed Frames: " << nbFramesGrabbed << endl;
      }

#ifdef DEBUG_MATRICES
    cout << "  Tracker matrices:" 
         << this->tagger->GetTrackerTool()->GetBuffer()->GetNumberOfItems() <<endl;
    cout << "  Tracker buffer size: " 
         <<  this->tagger->GetTrackerTool()->GetBuffer()->GetBufferSize() << endl;    
#endif
    } 

  this->sonixGrabber->Rewind();

#ifdef USE_TRACKER_DEVICE
  vtkMatrix4x4 * tempMatrix = vtkMatrix4x4::New();
#endif

  for(int i=0; i < nbFramesGrabbed; i++)
    {
    // get those transforms... and compute the bounding box
    this->tagger->Update();

    // determine the bounding box occupied by the reconstructed volume
    for(int j=0; j < 4; j++)
      {

#ifdef USE_TRACKER_DEVICE
        //Adjust tracker matrix to ultrasound scan depth
      tempMatrix->DeepCopy(this->tagger->GetTransform()->GetMatrix());
      AdjustMatrix(*tempMatrix);
      tempMatrix->MultiplyPoint(imCorners[j],transformedPt);

#else

      this->tagger->GetTransform()->MultiplyPoint(imCorners[j],transformedPt);

#endif

      minX = min(transformedPt[0], minX);
      minY = min(transformedPt[1], minY);
      minZ = min(transformedPt[2], minZ);
      maxX = max(transformedPt[0], maxX);
      maxY = max(transformedPt[1], maxY);
      maxZ = max(transformedPt[2], maxZ);
      }
    this->sonixGrabber->Seek(1);
    }

#ifdef USE_TRACKER_DEVICE
  tempMatrix->Delete();
#endif

#ifdef DEBUG_MATRICES
  cout << "X|Y|Z - Min: " << minX << "|" << minY <<"|"<< minZ
       << "- Max: " << maxX <<"|" << maxY <<"|" << maxZ << endl;
#endif

  double spacing[3] = {1,1,1};
  int volumeExtent[6] = { 0, (int)( (maxX - minX) / spacing[0] ), 
                          0, (int)( (maxY - minY) / spacing[1] ), 
                          0, (int)( (maxZ - minZ) / spacing[2] )};

  panoramaReconstructor->SetOutputExtent(volumeExtent);
  panoramaReconstructor->SetOutputSpacing(spacing);
  panoramaReconstructor->SetOutputOrigin(minX, minY, minZ);

  //---------------------------------------------------------------------------
  //Rewind and add recorded Slices to the PanoramaReconstructor

  panoramaReconstructor->SetSlice(tagger->GetOutput());
  panoramaReconstructor->GetOutput()->Update();
  this->sonixGrabber->Rewind();

#ifdef USE_ULTRASOUND_DEVICE
  this->sonixGrabber->Seek(100);//The first 100 frames are black therefore skip them
  if(Verbose)
    {
    cout << "Skip the first 100 frames" << endl;
    }
#endif

  this->tagger->Update();

  vtkMatrix4x4 *sliceAxes = vtkMatrix4x4::New();
  this->tagger->GetTransform()->GetMatrix(sliceAxes);
  panoramaReconstructor->SetSliceAxes(sliceAxes);

#ifdef DEBUG_IMAGES
  vtkBMPWriter *writer = vtkBMPWriter::New();
  char filename[256];
#endif

  //Set the correct amount of the output volume's scalar components
  panoramaReconstructor->GetOutput()->SetNumberOfScalarComponents(tagger->GetOutput()->GetNumberOfScalarComponents()); 
  panoramaReconstructor->GetOutput()->AllocateScalars();
   
  cout << "Start Volume Reconstruction" << endl;

  for(int i=0; i < nbFramesGrabbed; i++)
    {
    
    if( i != 0)
      {
      cout << "*" <<std::flush;
      }

    if (i % 50 == 0 && i != 0)
      {      
      cout << "\n"<< std::flush;
      }

    this->tagger->Update();
    this->tagger->GetTransform()->GetMatrix(sliceAxes); //Get trackingmatrix of current frame

#ifdef USE_TRACKER_DEVICE
    AdjustMatrix(*sliceAxes);   // Adjust tracker matrix to ultrasound scan depth
#endif
    fprintf(stderr,"1");
    panoramaReconstructor->SetSliceAxes(sliceAxes); //Set current trackingmatrix
    fprintf(stderr,"2");

#ifdef DEBUG_MATRICES
    cout << "Tracker matrix:\n";
    sliceAxes->Print(cout);
#endif
    fprintf(stderr,"3");
    panoramaReconstructor->InsertSlice(); //Add current slice to the reconstructor
    this->sonixGrabber->Seek(1); //Advance to the next frame

#ifdef DEBUG_IMAGES

    writer->SetInput(this->tagger->GetOutput());  
    sprintf(filename,"./Output/output%03d.bmp",i);
    writer->SetFileName(filename);
    writer->Update();

#endif

    }

  cout << endl;

  //----- Reconstruction done -----

  if(panoramaReconstructor->GetPixelCount() > 0)
    {
      cout << "Reconstruction successfully completed" << endl;
      if(Verbose)
        {
        cout << "  Inserted " << panoramaReconstructor->GetPixelCount() << " pixels into the output volume" << endl;
        }
    }
  else
    {
      cout << "Reconstruction failed" << endl;
      return false;
    }

  panoramaReconstructor->FillHolesInOutput();
  
  //---------------------------------------------------------------------------
  // Prepare reconstructed 3D volume for transfer

#ifdef REMOVE_ALPHA_CHANNEL
  // To remove the alpha channel of the reconstructed volume
  vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
  extract->SetInput(panoramaReconstructor->GetOutput());
  extract->SetComponents(0);
  vtkImageData * extractOutput = extract->GetOutput();
  
#else
  vtkImageData * extractOutput = panoramaReconstructor->GetOutput();
  
#endif

  //--- Adjust Properties of Volume ---
  
  //Dimensions
  int dimensions[3];   
  extractOutput->GetDimensions(dimensions);
  Volume->SetDimensions(dimensions);

  //Spacing
  double ouputSpacing[3];
  extractOutput->GetSpacing(ouputSpacing);
  Volume->SetSpacing(ouputSpacing);
  
  //SetScalarType
  Volume->SetScalarType(extractOutput->GetScalarType());
  Volume->AllocateScalars();

  unsigned char * pBuff = (unsigned char *) Volume->GetScalarPointer();
  unsigned char * pExtract = (unsigned char *) extractOutput->GetScalarPointer();  

  //Fill transfer buffer
  for(int i = 0 ; i < dimensions[2] ; ++i)
    {
    for(int j = 0 ; j < dimensions[1] ; ++j)
      {
      for(int k = 0 ; k < dimensions[0] ; ++k)
        {
        *pBuff = *pExtract;
        pBuff++;
        pExtract++;
        }
      }
    }   

#ifdef REMOVE_ALPHA_CHANNEL

  extract->Delete();

#endif

  sliceAxes->Delete();
  panoramaReconstructor->Delete();


  return true;
}

//Adjust tracker matrix to ultrasound scan depth
void vtkUltrasoundVolumeReconstructor::AdjustMatrix(vtkMatrix4x4& matrix)
{  
  double scaleFactor = (this->sonixGrabber->GetFrameSize())[1] / this->ScanDepth * US_IMAGE_FAN_RATIO;
  
  matrix.Element[0][3] = matrix.Element[0][3] * scaleFactor;//x
  matrix.Element[1][3] = matrix.Element[1][3] * scaleFactor;//y
  matrix.Element[2][3] = matrix.Element[2][3] * scaleFactor;//z

}

static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi) || (__APPLE__)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
}
