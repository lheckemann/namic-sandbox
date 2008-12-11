/*=========================================================================

Module:    $RCSfile: vtkUltrasoundVolumeReconstructor.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
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

#include <iostream>
#include <sstream>
#include <limits>
#include <string>

#define NOMINMAX
#undef REMOVE_ALPHA_CHANNEL
 
//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkBMPWriter.h"


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

using namespace std;

static inline void vtkSleep(double duration);

vtkCxxRevisionMacro(vtkUltrasoundVolumeReconstructor, "$Revision$");
vtkStandardNewMacro(vtkUltrasoundVolumeReconstructor);

//----------------------------------------------------------------------------
vtkUltrasoundVolumeReconstructor::vtkUltrasoundVolumeReconstructor()
{
  this->NbFrames = 50 + 100; //# of Frames to capture + additional 100 frames which are skipped
  this->FrameRate = 30;

  this->CalibrationFileName = NULL;
  this->SetVideoDevice("/dev/video0");

  this->VolumeReconstructionEnabled = true;
  this->UseTrackerTransforms = true;

  this->calibReader = vtkUltrasoundCalibFileReader::New();
  
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

// McGumbel
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
  bool error;
  if( this->UseTrackerTransforms){
    
    error = this->StartTracker();
    
    
    //NH
    //12/2/08
    //Debug line
    //    cerr << "DEBUG: this->StartTracker() returns:" << error << endl;
  }
  
  this->tagger->Initialize();

  return error;
}
//
//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeReconstructor::StartTracker()
{
  if(this->tracker->Probe() != 1)
    {
    cerr << "Tracking system not found" << endl;
    return false;
    }

  // make sure the tracking buffer is large enough for the number of the image sequence requested
  vtkTrackerTool *tool = this->tracker->GetTool(0);
  tool->GetBuffer()->SetBufferSize((int) (this->NbFrames * CERTUS_UPDATE_RATE / this->FrameRate * FUDGE_FACTOR + 0.5) ); 
  this->tracker->StartTracking();
  this->tagger->SetTrackerTool(tool);  
  this->tagger->SetCalibrationMatrix(this->calibReader->GetCalibrationMatrix());
}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeReconstructor::ReconstructVolume(vtkImageData * ImageBuffer)
 {
   cout << "Initialization:" << std::flush;
   cout << '\a' << std::flush;
   for(int i = 0; i < 11; i++)
     {
     vtkSleep(1);
     cout << 10 - i << " " << std::flush;
     }
   cout << endl;

   cout << "Start Scanning" << endl;
   cout << '\a' << std::flush;
   vtkSleep(0.2);
   cout << '\a' << std::flush; 

   this->sonixGrabber->Record();  //Start recording frame from the video
   
   for (int i = 0; i < (int) (this->NbFrames / this->FrameRate + 0.5) * 2; i++)
        { 
    cout <<"*" << '\a' << std::flush;
        vtkSleep(0.5);
        }
   cout << endl;
   // igtl:sleep((int) (this->NbFrames / this->FrameRate + 0.5));// wait for the images (delay in seconds)


   this->sonixGrabber->Stop();//Stop recording


   this->tracker->StopTracking();//Stop tracking

   cout << "Stop Scanning" << endl;
   cout << '\a' << std::flush;
   vtkSleep(0.2);
   cout << '\a' << std::flush;

   cout << "Scanning successful" << endl;

   cout << "  Recorded synchronized transforms and ultrasound images for " << ((int) this->NbFrames / this->FrameRate + 0.5) << "s" << endl;

   // set up the panoramic reconstruction class
   vtk3DPanoramicVolumeReconstructor *panoramaReconstructor = vtk3DPanoramicVolumeReconstructor::New();
   panoramaReconstructor->CompoundingOn();
   panoramaReconstructor->SetInterpolationModeToLinear();
   //  panoramaReconstructor->GetOutput()->SetScalarTypeToUnsignedChar(); //Causes a segmentation fault

   //NH
   //12/2/08
   //cerr << "vtk3DPanoramicVolumeReconsturctor instantiated" << endl;


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

   //NH
   // 12/02/08
   //   cerr << "all clipping number set" << endl;
   cerr << "  Grabbed Frames: " << nbFramesGrabbed << endl;

   this->sonixGrabber->Rewind();

   for(int i=0; i < nbFramesGrabbed; i++)
     {
     // get those transforms... and compute the bounding box
     this->tagger->Update();

     // determine the bounding box occupied by the reconstructed volume
     for(int j=0; j < 4; j++)
       {
       this->tagger->GetTransform()->MultiplyPoint(imCorners[j],transformedPt);
       minX = min(transformedPt[0], minX);
       minY = min(transformedPt[1], minY);
       minZ = min(transformedPt[2], minZ);
       maxX = max(transformedPt[0], maxX);
       maxY = max(transformedPt[1], maxY);
       maxZ = max(transformedPt[2], maxZ);
       }
     this->sonixGrabber->Seek(1);

     }

   double spacing[3] = {1,1,1};
   int volumeExtent[6] = { 0, (int)( (maxX - minX) / spacing[0] ), 
         0, (int)( (maxY - minY) / spacing[1] ), 
         0, (int)( (maxZ - minZ) / spacing[2] ) };

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
   cout << "Skip the first 100 frames" << endl;
 #endif

   this->tagger->Update();

   vtkMatrix4x4 *sliceAxes = vtkMatrix4x4::New();
   this->tagger->GetTransform()->GetMatrix(sliceAxes);
   panoramaReconstructor->SetSliceAxes(sliceAxes);

   //NH
   vtkBMPWriter *writer = vtkBMPWriter::New();
   char filename[256];

   cout << "Start Volume Reconstruction" << endl;

   for(int i=0; i < nbFramesGrabbed; i++)
     {
       cout << "*" << std::flush;

        if (i % 80 == 0)
         {
         cout << "\n"<< std::flush;
         }

     this->tagger->Update();
     this->tagger->GetTransform()->GetMatrix(sliceAxes); //Get trackingmatrix of current frame
     panoramaReconstructor->SetSliceAxes(sliceAxes); //Set current trackingmatrix
     //cout << "Tracker matrix:\n";
     //sliceAxes->Print(cout);
     panoramaReconstructor->InsertSlice(); //Add current slice to the reconstructor
     this->sonixGrabber->Seek(1); //Advance to the next frame

    writer->SetInput(this->tagger->GetOutput());  
    sprintf(filename,"./Output/output%03d.bmp",i);
    writer->SetFileName(filename);
    writer->Update();

    }

  //----- Reconstruction done -----

   if(panoramaReconstructor->GetPixelCount() > 0)
     {
       cout << "Reconstruction successfully completed" << endl;
       cout << "  Inserted " << panoramaReconstructor->GetPixelCount() << " pixels into the output volume" << endl;
     }
   else
     {
       cout << "Reconstruction failed" << endl;
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

  //--- Adjust Properties of ImageBuffer ---
  
  //Dimensions
  int dimensions[3];   
  extractOutput->GetDimensions(dimensions);
  ImageBuffer->SetDimensions(dimensions);
  
  //Spacing
  double ouputSpacing[3];
  extractOutput->GetSpacing(ouputSpacing);
  ImageBuffer->SetSpacing(ouputSpacing);
  
  //SetScalarType
  ImageBuffer->SetScalarType(extractOutput->GetScalarType());
  ImageBuffer->AllocateScalars();

  unsigned char * pBuff = (unsigned char *) ImageBuffer->GetScalarPointer();
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

  panoramaReconstructor->Delete();


  return true;
}


static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
}
