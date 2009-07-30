/*========================================================================

Module:  $RCSfile: vtkDataCollector.cxx,v $
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

//#define REMOVE_ALPHA_CHANNEL
#define DEBUG_IMAGES //Write tagger output to HDD
#define DEBUG_MATRICES //Prints tagger matrices to stdout
#define SHRINK

//#include <windows.h>

#include "vtkDataCollector.h"
#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkBMPWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"
#include "vtkImageData.h"
#include "vtkImageShrink3D.h"

#include "vtkObjectFactory.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#include "vtkNDITracker.h"
#include "vtkTrackerSimulator.h"

#ifdef USE_ULTRASOUND_DEVICE
#include "vtkV4L2VideoSource.h"
#else
#include "vtkVideoSourceSimulator.h"
#endif

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625

#define DEFAULT_FPS 30
#define DEFAULT_VIDEO_DEVICE_NAME /dev/video
#define DEFAULT_VIDEO_CHANNEL 3 //S-Video at Hauppauge Impact VCB Modell 558
#define DEFAULT_VIDEO_MODE 1 //NTSC
#define DEFAULT_ULTRASOUND_SCANDEPTH 70 //mm
#define DEFAULT_MAXIMUM_VOLUME_SIZE -1

using namespace std;

vtkCxxRevisionMacro(vtkDataCollector, "$Revision$");
vtkStandardNewMacro(vtkDataCollector);

/******************************************************************************
 * vtkDataCollector::vtkDataCollector()
 *
 *  @Author:Jan Gumprecht
 *  @Date:  02.February 2009
 *
 * ****************************************************************************/
vtkDataCollector::vtkDataCollector()
{
  this->Verbose = false;
  this->FrameRate = DEFAULT_FPS;
  //this->UltrasoundScanDepth = DEFAULT_ULTRASOUND_SCANDEPTH; //Unit: mm

  this->FrameBufferSize = 100;
  this->MaximumVolumeSize[0] = DEFAULT_MAXIMUM_VOLUME_SIZE;
  this->MaximumVolumeSize[1] = DEFAULT_MAXIMUM_VOLUME_SIZE;
  this->MaximumVolumeSize[2] = DEFAULT_MAXIMUM_VOLUME_SIZE;

  this->CalibrationFileName = NULL;
  this->calibReader = vtkUltrasoundCalibFileReader::New();

  #ifdef USE_ULTRASOUND_DEVICE
    char* devicename = "/dev/video";
    this->SetVideoDevice(devicename);
    this->SetVideoChannel(DEFAULT_VIDEO_CHANNEL);
    this->SetVideoMode(DEFAULT_VIDEO_MODE); //NTSC
    this->VideoSource = vtkV4L2VideoSource::New();
  #else
   this->VideoSource = vtkVideoSourceSimulator::New();
  #endif //USE_ULTRASOUND_DEVICE

//#ifdef USE_ULTRASOUND_DEVICE
//  char* devicename = "/dev/video";
//  this->SetVideoDevice(devicename);
//  this->SetVideoChannel(3); //S-Video at Hauppauge Impact VCB Modell 558
//  this->SetVideoMode(1); //NTSC
//#endif

#ifdef USE_ULTRASOUND_DEVICE
  this->VideoSource = vtkV4L2VideoSource::New();
#else
  this->VideoSource = vtkVideoSourceSimulator::New();
#endif //USE_ULTRASOUND_DEVICE

  this->Tagger = vtkTaggedImageFilter::New();

  this->TrackerDeviceEnabled = false;
  this->trackerSimulator = vtkTrackerSimulator::New();
  this->NDItracker = NULL;

  this->DataProcessor = NULL;

  this->PlayerThreader = vtkMultiThreader::New();;
  this->PlayerThreadId = -1;

  this->Collecting = false;
  this->Initialized = false;

  this->StartUpTime = vtkTimerLog::GetUniversalTime();
  this->LogStream.ostream::rdbuf(cerr.rdbuf());

  this->TrackerOffset[0] = 0; //mm
  this->TrackerOffset[1] = -10; //mm
  this->TrackerOffset[2] = 30; //mm

  this->ImageMargin[0] = 0;
  this->ImageMargin[1] = 0;
  this->ImageMargin[2] = 0;
  this->ImageMargin[3] = 0;

  this->SystemOffset[0] = 0;
  this->SystemOffset[1] = 0;
  this->SystemOffset[2] = 300;

  this->TransformationFactorMmToPixel = 1;

  this->ObliquenessAdjustmentMatrix = vtkMatrix4x4::New();
  this->ObliquenessAdjustmentMatrix->Identity();

  this->CoordinateTransformationMatrix = vtkMatrix4x4::New();
  this->CoordinateTransformationMatrix->Identity();

  this->FixedVolumeProperties.Frame = NULL;
  this->FixedVolumeProperties.Matrix = NULL;
  this->FixedVolumeProperties.Spacing[0] = 1;
  this->FixedVolumeProperties.Spacing[1] = 1;
  this->FixedVolumeProperties.Spacing[2] = 1;
  this->FixedVolumeProperties.Origin[0] = 0;
  this->FixedVolumeProperties.Origin[1] = 0;
  this->FixedVolumeProperties.Origin[2] = 0;
  this->FixedVolumeProperties.TimeStamp = this->StartUpTime;
  this->FixedVolumeProperties.VolumeExtent[0] = -1;
  this->FixedVolumeProperties.VolumeExtent[1] = -1;
  this->FixedVolumeProperties.VolumeExtent[2] = -1;
  this->FixedVolumeProperties.VolumeExtent[3] = -1;
  this->FixedVolumeProperties.VolumeExtent[4] = -1;
  this->FixedVolumeProperties.VolumeExtent[5] = -1;

  this->DynamicVolumeSize = false;
  this->VolumeInitialized = false;
}

/******************************************************************************
 * vtkDataCollector::~vtkDataCollector()
 *
 *  @Author:Jan Gumprecht
 *  @Date:  02.February 2009
 *
 * ****************************************************************************/
vtkDataCollector::~vtkDataCollector()
{
  this->StopCollecting();

  if(!this->TrackerDeviceEnabled && this->trackerSimulator != NULL)
    {
    this->trackerSimulator->Delete();
    }

  this->VideoSource->ReleaseSystemResources();
  this->VideoSource->Delete();
  this->Tagger->Delete();
  this->calibReader->Delete();
  this->SetCalibrationFileName(NULL);
  this->PlayerThreader->Delete();
}

/******************************************************************************
 * void vtkDataCollector::PrintSelf(ostream& os, vtkIndent indent)
 *
 *  @Author:Jan Gumprecht
 *  @Date:  02.February 2009
 *
 * ****************************************************************************/
void vtkDataCollector::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

/******************************************************************************
 * int vtkDataCollector::Initialize(vtkNDITracker* tracker)
 *
 *  @Author:Jan Gumprecht
 *  @Date:  02.February 2009
 *
 * ****************************************************************************/
int vtkDataCollector::Initialize(vtkNDITracker* tracker)
{
  if(this->Initialized)
    {
    return 0;
    }

  if(this->TrackerDeviceEnabled && tracker == NULL)
    {
    #ifdef ERRORCOLLECTOR
      this->GetLogStream() << this->GetUpTime() << " |C-ERROR: Can not initialize Data Collector no tracker provided" << endl;
    #endif
    return -1;
    }

  if(this->calibReader != NULL)
    {
    this->calibReader->SetFileName(this->CalibrationFileName);
    if(-1 == this->calibReader->ReadCalibFile())
      {
      #ifdef ERRORCOLLECTOR
      this->GetLogStream() << this->GetUpTime() << " |C-ERROR: Can not read calibration file => Stop initialization" << endl;
      #endif
      return -1;
      }
    this->calibReader->GetClipRectangle(this->clipRectangle);
    this->calibReader->GetImageMargin(this->ImageMargin);
    this->calibReader->GetShrinkFactor(this->ShrinkFactor);
    this->calibReader->GetSystemOffset(this->SystemOffset);
    if(this->MaximumVolumeSize[0] == DEFAULT_MAXIMUM_VOLUME_SIZE)
      {
      this->MaximumVolumeSize[0] = this->calibReader->GetMaximumVolumeSize()[0];
      this->MaximumVolumeSize[1] = this->calibReader->GetMaximumVolumeSize()[1];
      this->MaximumVolumeSize[2] = this->calibReader->GetMaximumVolumeSize()[2];
      }
    this->TransformationFactorMmToPixel =  this->calibReader->GetTransformationFactorMmToPixel();

    this->calibReader->GetTrackerOffset(this->TrackerOffset);

    this->ObliquenessAdjustmentMatrix = this->calibReader->GetObliquenessAdjustmentMatrix();
    this->CoordinateTransformationMatrix = this->calibReader->GetCoordinateTransformationMatrix();

    //Video Source Settings
    this->SetVideoDevice(this->calibReader->GetVideoSource());
    this->SetVideoChannel(this->calibReader->GetVideoChannel());
    this->SetVideoMode(this->calibReader->GetVideoMode());

    //Ultrasound Settings
    this->SetFrameRate(this->calibReader->GetFramesPerSecond());

    }
  else
    {
    #ifdef ERRORCOLLECTOR
      this->LogStream << this->GetUpTime() << " |C-ERROR: Calibration file reader not specified" << endl;
    #endif
    return -1;
    }

  int *imSize = this->calibReader->GetImageSize();
  this->VideoSource->SetFrameSize(imSize[0], imSize[1], 1);

#ifdef USE_ULTRASOUND_DEVICE
  this->VideoSource->SetVideoDevice(this->GetVideoDevice());
  this->VideoSource->SetVideoChannel(this->GetVideoChannel());
  this->VideoSource->SetVideoMode(this->GetVideoMode());
#endif

  // set up the video source (ultrasound machine)
  this->VideoSource->SetFrameRate(this->FrameRate);
  this->VideoSource->SetFrameBufferSize(this->FrameBufferSize);

  double imageOrigin[3] = {0 ,0, 0};//Images are not shifted
  this->VideoSource->SetDataOrigin(imageOrigin);

  double *imageSpacing = this->calibReader->GetImageSpacing();
  this->VideoSource->SetDataSpacing(imageSpacing);

  //
  // Setting up the synchronization filter
  this->Tagger->SetVideoSource(this->VideoSource);

  if(this->TrackerDeviceEnabled)
    {

    this->NDItracker = tracker;
    this->tool = this->NDItracker->GetTool(0);
    this->tool->GetBuffer()->SetBufferSize((int) (this->FrameBufferSize * CERTUS_UPDATE_RATE / this->FrameRate * FUDGE_FACTOR + 0.5) );
    }
  else
    {
    this->tool = this->trackerSimulator->GetTool(0);
    this->tool->GetBuffer()->SetBufferSize((int) (this->FrameBufferSize * CERTUS_UPDATE_RATE / this->FrameRate * FUDGE_FACTOR + 0.5) );
    this->trackerSimulator->StartTracking();
    }


  this->Tagger->SetTrackerTool(this->tool);
  this->Tagger->SetCalibrationMatrix(this->calibReader->GetCalibrationMatrix());

  this->Tagger->Initialize();
  #ifdef USE_ULTRASOUND_DEVICE
  this->VideoSource->Record();
  this->VideoSource->Stop();
  #endif
  this->Initialized = true;
  return 0;
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

/******************************************************************************
 *  static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time)
 *
 *  Sleep until the specified absolute time has arrived.
 *  You must pass a handle to the current thread.
 *  If '0' is returned, then the thread was aborted before or during the wait. *
 *
 *  @Author:Jan Gumprecht
 *  @Date:  22.December 2008
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data
 *
 *  @Param: double time - Time until which to sleep
 *
 * ****************************************************************************/
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time)
{
  // loop either until the time has arrived or until the thread is ended
  for (int i = 0;; i++)
    {
      double remaining = time - vtkTimerLog::GetUniversalTime();

      // check to see if we have reached the specified time
      if (remaining <= 0)
        {
        if (i == 0)
          {
          vtkGenericWarningMacro("Dropped a video frame.");
          }
        return 1;
      }
      // check the ActiveFlag at least every 0.1 seconds
      if (remaining > 0.1)
        {
        remaining = 0.1;
        }

      // check to see if we are being told to quit
      data->ActiveFlagLock->Lock();
      int activeFlag = *(data->ActiveFlag);
      data->ActiveFlagLock->Unlock();

      if (activeFlag == 0)
        {
        break;
        }

      vtkSleep(remaining);
    }

  return 0;
}

/******************************************************************************
 *  static void *vtkDataCollectorThread(vtkMultiThreader::ThreadInfo *data)
 *
 *  This function runs in an alternate thread to asyncronously collect data
 *
 *  @Author:Jan Gumprecht
 *  @Date:  19.Januar 2009
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data
 *
 * ****************************************************************************/
static void *vtkDataCollectorThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkDataCollector *self = (vtkDataCollector *)(data->UserData);

  double startTime = vtkTimerLog::GetUniversalTime();
  double rate = self->GetFrameRate();
  int frame = 0;
  int extent[6];
  bool skip = false;
  vtkImageShrink3D* mask = vtkImageShrink3D::New();
  mask->AveragingOn();

#ifdef DEBUG_IMAGES
  vtkBMPWriter *writer = vtkBMPWriter::New();
  char filename[256];
#endif

  do
    {
    //Check if data processor still works---------------------------------------
    if(!self->GetDataProcessor()->GetProcessing())
      {
      #ifdef DEBUGCOLLECTOR
        self->GetLogStream() << self->GetUpTime() << " |C-WARNING: Data processor stopped processing -> Stop collecting" << endl;
      #endif
      self->StopCollecting();

      cout << "====================================================" << endl << endl
           << "--- 4D Ultrasound stopped working ---" << endl << endl
           << "Press 't' and hit 'ENTER' to terminate 4D Ultrasound"<< endl;
      }
    else
      {//Check if data buffer of data processor already is full-----------------
      if(!self->GetDataProcessor()->IsDataBufferFull())
        {
        struct DataStruct dataStruct;

        dataStruct.TimeStamp = self->GetUpTime();

        //Grab new frame--------------------------------------------------------
        self->GetVideoSource()->Grab();

        //Get Tracking Matrix for new frame-------------------------------------
        dataStruct.Matrix = vtkMatrix4x4::New();
        self->GetTagger()->Update();
        dataStruct.Matrix->DeepCopy(self->GetTagger()->GetTransform()->GetMatrix());

        //Set Spacing of new frame----------------------------------------------
        self->GetTagger()->GetOutput()->GetSpacing(dataStruct.Spacing);


        #ifdef DEBUG_MATRICES
        self->GetLogStream() << self->GetUpTime() << " |C-INFO Tracker matrix:" << endl;
        dataStruct.Matrix->Print(self->GetLogStream());
        #endif


        //Data Processing and error checking------------------------------------
        if(self->IsTrackerDeviceEnabled())
          {
          if(-1 == self->ProcessMatrix(&dataStruct))
            {//Tracker matrix is unusable
            skip = true;
            }
          }

        if(!skip && -1 == self->CalculateVolumeProperties(&dataStruct))
          {
          #ifdef DEBUGCOLLECTOR
          self->GetLogStream() << self->GetUpTime() << " |C-ERROR: Can not calculate volume properties"<< endl;
          #endif
          skip = true;
          }

        if(!skip && dataStruct.VolumeExtent[1] * dataStruct.VolumeExtent[3] * dataStruct.VolumeExtent[5]
                    > self->GetMaximumVolumeSize())
          {
          #ifdef DEBUGCOLLECTOR
          self->GetLogStream() << self->GetUpTime() << " |C-ERROR: Volume of new frame is too big"<< endl;
          #endif
          skip = true;
          }

        if(!skip)
          {
          //Get new frame-------------------------------------------------------
          dataStruct.Frame = vtkImageData::New();

//          self->DuplicateFrame(self->GetTagger()->GetOutput(), dataStruct.Frame);
          self->ExtractImage(self->GetTagger()->GetOutput(), dataStruct.Frame);
          #ifdef SHRINK

          mask->SetInput(dataStruct.Frame);
          mask->SetShrinkFactors(self->GetShrinkFactor()[0], self->GetShrinkFactor()[1], self->GetShrinkFactor()[2]);
          mask->Update();

          dataStruct.Frame->Delete();
          dataStruct.Frame = vtkImageData::New();

          self->DuplicateFrame(mask->GetOutput(), dataStruct.Frame);
          dataStruct.Frame->SetSpacing(dataStruct.Spacing);
          #endif

          #ifdef DEBUG_IMAGES
          writer->SetInput(dataStruct.Frame);
          sprintf(filename,"./Output/output%03d.bmp",frame);
          writer->SetFileName(filename);
          writer->Update();
          #endif //DEBUG_IMAGES

          //Forward frame + matrix to data sender-------------------------------
          if(-1 == self->GetDataProcessor()->NewData(dataStruct))
            {
            #ifdef DEBUGCOLLECTOR
            self->GetLogStream() << self->GetUpTime() << " |C-WARNING: Data Collector can not forward data to Data Processor" << endl;
            #endif
            dataStruct.Matrix->Delete();
            dataStruct.Frame->Delete();
            }
          else
            {
            #ifdef DEBUGCOLLECTOR
            self->GetLogStream() << self->GetUpTime() << " |C-INFO: Data sent to processor" << endl;
            #endif
            }
          }
        else
          {
          #ifdef DEBUGCOLLECTOR
            self->GetLogStream() << self->GetUpTime() << " |C-WARNING: Tracker sends unusable matrices" << endl;
          #endif
          dataStruct.Matrix->Delete();
          }
        skip = false;
        }//Check if Processor has buffer space available
      }//Check if Processor stopped processing

    frame++;
    }
  while(vtkThreadSleep(data, startTime + frame/rate));

  mask->Delete();

return NULL;
}

/******************************************************************************
 * int vtkDataCollector::StartCollecting(vtkDataProcessor * processor)
 *
 *  @Author:Jan Gumprecht
 *  @Date:  02.February 2009
 *
 * ****************************************************************************/
int vtkDataCollector::StartCollecting(vtkDataProcessor * processor)
{
  if (!this->Initialized)
    {
    #ifdef ERRORCOLLECTOR
      this->LogStream << this->GetUpTime() << " |C-ERROR: Can not start tracking; Data collector not initialized";
    #endif
    return -1;

    }

  if (!this->Collecting)
    {
    this->Collecting = true;
    }
  else
    {
    #ifdef ERRORCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: Data collector already collects data";
    #endif
    return -1;
    }

  if(processor != NULL)
    {
    this->DataProcessor = processor;
    this->DataProcessor->SetUltraSoundTrackingEnabled(this->TrackerDeviceEnabled);
    }
  else
    {
    #ifdef ERRORCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: No data processor provided. Data collection not possible" << endl;
    #endif
    return -1;
    }

  //Start Thread
  this->PlayerThreadId =
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkDataCollectorThread, this);

  if(this->PlayerThreadId != -1)
    {
    if(Verbose)
      {
      cout << "Start collecting data" << endl;
      }
    return 0;
    }
  else
    {
    #ifdef ERRORCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: Collector thread could not be started" << endl;
    #endif
    return -1;
    }
}
/******************************************************************************
 * int vtkDataCollector::StopCollecting()
 *
 *  @Author:Jan Gumprecht
 *  @Date:  02.February 2009
 *
 * ****************************************************************************/
int vtkDataCollector::StopCollecting()
{
  if(NULL != this->DataProcessor)
    {
    this->DataProcessor->StopProcessing();
    }

  if (this->Collecting)
    {
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Collecting = false;

    if(this->TrackerDeviceEnabled)
      {
//      this->NDItracker->StopTracking();
      }
    else
      {
      this->trackerSimulator->StopTracking();
      }

    if(this->Verbose)
      {
      cout << "Stop collecting" << endl;
      }

    #ifdef DEBUGCOLLECTOR
      this->LogStream << this->GetUpTime() << " |C-INFO: Data collector stopped collecting"<< endl;
    #endif

    }

  return 0;
}

/******************************************************************************
 * int vtkDataCollector::ProcessMatrix(struct DataStruct *pDataStruct)
 *
 *  @Author:Jan Gumprecht
 *  @Date:  14.February 2009
 *
 * ****************************************************************************/
int vtkDataCollector::ProcessMatrix(struct DataStruct *pDataStruct)
{
  if(this->IsMatrixEmpty(pDataStruct->Matrix) || this->IsIdentityMatrix(pDataStruct->Matrix))
    {
    #ifdef INFOCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: Tracker received no data"<< endl;
    #endif
    return -1;
    }

//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime() << " |C-INFO: Process Matrix Original Matrix:"<< endl;
//    pDataStruct->Matrix->Print(this->LogStream);
//  #endif

  //----------------------------------------------------------------------------
  //Calibrate tracker Matrix----------------------------------------------------
  vtkMatrix4x4 * oldMatrix = vtkMatrix4x4::New();
  double stretchFactor = 4.0 / 5.0 * 9.0 / 10.0 * 1.08;

  //Adjust Obliqueness----------------------------------------------------------
  oldMatrix->DeepCopy(pDataStruct->Matrix);
  vtkMatrix4x4::Multiply4x4(oldMatrix, this->ObliquenessAdjustmentMatrix, pDataStruct->Matrix);

//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime() << " |C-INFO: Process Matrix Obliqueness adjusted:"<< endl;
//    pDataStruct->Matrix->Print(this->LogStream);
//  #endif

 //Transform coordinate System-------------------------------------------------
  oldMatrix->DeepCopy(pDataStruct->Matrix);
  vtkMatrix4x4::Multiply4x4(oldMatrix, this->CoordinateTransformationMatrix, pDataStruct->Matrix);

//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime() << " |C-INFO: Process Matrix coordinate system transformed:"<< endl;
//    pDataStruct->Matrix->Print(this->LogStream);
//  #endif

  //Strech coordinate to make them fit to slicer coordinates i.g. 1 unit == 1 mm in slicer
  pDataStruct->Matrix->Element[0][3] *= stretchFactor;
  pDataStruct->Matrix->Element[1][3] *= stretchFactor;
  pDataStruct->Matrix->Element[2][3] *= stretchFactor;

  //Apply Offset----------------------------------------------------------------
  double xOffset = -1 * (this->clipRectangle[2] + 1)  / 2;
  double yOffset = -1 * (this->clipRectangle[3] + 1  + this->TrackerOffset[2]);
  double zOffset = this->TrackerOffset[1];

  double xAxis[3] = {pDataStruct->Matrix->Element[0][0], pDataStruct->Matrix->Element[1][0], pDataStruct->Matrix->Element[2][0]};
  double yAxis[3] = {pDataStruct->Matrix->Element[0][1], pDataStruct->Matrix->Element[1][1], pDataStruct->Matrix->Element[2][1]};
  double zAxis[3] = {pDataStruct->Matrix->Element[0][2], pDataStruct->Matrix->Element[1][2], pDataStruct->Matrix->Element[2][2]};

  //Shift Frame according to offset
  //X-Offset
  pDataStruct->Matrix->Element[0][3] += (xAxis[0] * xOffset);
  pDataStruct->Matrix->Element[1][3] += (xAxis[1] * xOffset);
  pDataStruct->Matrix->Element[2][3] += (xAxis[2] * xOffset);

  //Y-Offset
  pDataStruct->Matrix->Element[0][3] += (yAxis[0] * yOffset);
  pDataStruct->Matrix->Element[1][3] += (yAxis[1] * yOffset);
  pDataStruct->Matrix->Element[2][3] += (yAxis[2] * yOffset);

  //Z-Offset
  pDataStruct->Matrix->Element[0][3] += (zAxis[0] * zOffset);
  pDataStruct->Matrix->Element[1][3] += (zAxis[1] * zOffset);
  pDataStruct->Matrix->Element[2][3] += (zAxis[2] * zOffset);

  #ifdef HIGH_DEFINITION
  pDataStruct->Matrix->Element[0][3] *= this->TransformationFactorMmToPixel;
  pDataStruct->Matrix->Element[1][3] *= this->TransformationFactorMmToPixel;
  pDataStruct->Matrix->Element[2][3] *= this->TransformationFactorMmToPixel;
  #endif

  pDataStruct->Matrix->Element[0][3] += (this->GetSystemOffset()[0] * stretchFactor);
  pDataStruct->Matrix->Element[1][3] += (this->GetSystemOffset()[1] * stretchFactor);
  pDataStruct->Matrix->Element[2][3] += (this->GetSystemOffset()[2] * stretchFactor);

//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime() << " |C-INFO: Process Matrix offset applied:"<< endl;
//    pDataStruct->Matrix->Print(this->LogStream);
//  #endif

  oldMatrix->Delete();

  return 0;
}

/******************************************************************************
 * double vtkDataCollector::GetUpTime()
 *
 *  Returns elapsed Time since program start
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 * ****************************************************************************/
double vtkDataCollector::GetUpTime()
{
  return vtkTimerLog::GetUniversalTime() - this->GetStartUpTime();
}

/******************************************************************************
 * void vtkDataCollector::SetLogStream(ofstream &LogStream)
 *
 *  Redirects Logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 * ****************************************************************************/
void vtkDataCollector::SetLogStream(ofstream &LogStream)
{
  this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
  this->LogStream.precision(6);
  this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
 * ofstream& vtkDataCollector::GetLogStream()
 *
 *  Returns logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Return: Logstream
 *
 * ****************************************************************************/
ofstream& vtkDataCollector::GetLogStream()
{
        return this->LogStream;
}

/******************************************************************************
 * void vtkDataCollector::ExtractImage(vtkImageData * original, vtkImageData * duplicate)
 *
 *  Duplicates Image inData to Image outData
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Param: vtkImageData * original - Original
 *  @Param: vtkImageData * copy - Copy
 *
 * ****************************************************************************/
int vtkDataCollector::ExtractImage(vtkImageData * original, vtkImageData * extract)
{
  if( original == NULL || extract == NULL)
    {
    #ifdef ERRORCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: Extract Image called with empty image object(s)"<< endl;
    #endif
    return -1;
    }

  if(original->GetNumberOfScalarComponents() != 1)
    {
    #ifdef ERRORCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: Original image has more than 1 scalar component: " << original->GetNumberOfScalarComponents()<< endl;
    #endif
    return -1;
    }

//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime()  << " |C-INFO: Extract Image " << endl;
//  #endif

  int x, y, z;
  int ScalarComponents = 1;

  int topMargin    = this->calibReader->GetImageMargin()[0];
  int bottomMargin = this->calibReader->GetImageMargin()[1];
  int leftMargin   = this->calibReader->GetImageMargin()[2];
  int rightMargin  = this->calibReader->GetImageMargin()[3];

  int width = (int)(this->calibReader->GetImageSize()[0] - leftMargin - rightMargin);
  int height = (int)(this->calibReader->GetImageSize()[1] - topMargin - bottomMargin);

  //Create extracted image
  extract->SetExtent(0, width -1, 0, height - 1, 0, 0);
  extract->SetSpacing(original->GetSpacing());
  extract->SetScalarType(original->GetScalarType());
  extract->SetOrigin(original->GetOrigin());
  extract->SetNumberOfScalarComponents(original->GetNumberOfScalarComponents());
  extract->AllocateScalars(); //Reserve memory

  int counter = 0;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;

  //Calc x, y, z start and end coordinates of old volume in new volume
  int yStart = topMargin;
  int yEnd   = original->GetExtent()[3] - bottomMargin;


  // Get increments to march through data
  original->GetContinuousIncrements(original->GetExtent(), inIncX, inIncY, inIncZ);
  extract->GetContinuousIncrements(extract->GetExtent(), outIncX, outIncY, outIncZ);

  int rowLength = width * ScalarComponents + outIncY;
  int oldRowLength = original->GetExtent()[1] - original->GetExtent()[0] + 1;

  int spaceBeforeXStart = leftMargin * ScalarComponents;
  int spaceAfterXEnd = rightMargin * ScalarComponents;

  int spaceBeforeYStart = (topMargin + outIncY) * oldRowLength;

  //Get Data pointer
  unsigned char* pDataOriginal = (unsigned char *) original->GetScalarPointer();
  unsigned char* pDataExtract = (unsigned char *) extract->GetScalarPointer();

  try
    {
    //Copy Data
    pDataOriginal += spaceBeforeYStart;
    for(y = yStart; y <= yEnd; ++y)
      {
      pDataOriginal += spaceBeforeXStart;

      memcpy(pDataExtract, pDataOriginal, rowLength);
      pDataExtract += rowLength;
      pDataOriginal += rowLength;
      counter += rowLength;

      pDataOriginal += spaceAfterXEnd;

      pDataExtract += outIncY;
      pDataOriginal += inIncY;
      }
    }//End try
  catch (...)
    {
    throw;
    }

  if(counter != 0)
    {
//    #ifdef DEBUGPROCESSOR
//      this->LogStream << this->GetUpTime()  << " |C-INFO: extracted " << counter << " Pixel from original" << endl;
//    #endif
    return 0;
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |C-ERROR: No pixels extracted " << endl;
    #endif
    return -1;
    }
}

/******************************************************************************
 * bool vtkDataCollector::IsTrackerDeviceEnabled()
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Return: True  - If tracker device enabled
 *           False - If tracker device disabled
 *
 * ****************************************************************************/
bool vtkDataCollector::IsTrackerDeviceEnabled()
{
  return this->TrackerDeviceEnabled;
}

/******************************************************************************
 * int vtkDataCollector::EnableTrackerTool()
 *
 * Enable tracker device
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Return:  0 - on success
 *           -1 - on failure
 *
 * ****************************************************************************/
int vtkDataCollector::EnableTrackerTool()
{
  if(this->Initialized)
    {
    #ifdef ERRORCOLLECTOR
      this->LogStream << this->GetUpTime() << " |C-ERROR: Collector already initializes cannot start tracker device" << endl;
    #endif
    return -1;
    }
  this->TrackerDeviceEnabled = true;
  this->trackerSimulator->Delete();

  return 0;
}

/******************************************************************************
 * bool vtkDataCollector::IsIdentityMatrix(vtkMatrix4x4 * matrix)
 *
 * Check if matrix "matrix" is identity matrix
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Param: vtkMatrix4x4 * matrix
 *
 *  @Return:  true if identity matrix
 *            false if not identity matrix
 *
 * ****************************************************************************/
bool vtkDataCollector::IsIdentityMatrix(vtkMatrix4x4 * matrix)
{
  if(   matrix->Element[0][0] == 1
     && matrix->Element[0][1] == 0
     && matrix->Element[0][2] == 0
     && matrix->Element[0][3] == 0

     && matrix->Element[1][0] == 0
     && matrix->Element[1][1] == 1
     && matrix->Element[1][2] == 0
     && matrix->Element[1][3] == 0

     && matrix->Element[2][0] == 0
     && matrix->Element[2][1] == 0
     && matrix->Element[2][2] == 1
     && matrix->Element[2][3] == 0

     && matrix->Element[3][0] == 0
     && matrix->Element[3][1] == 0
     && matrix->Element[3][2] == 0
     && matrix->Element[3][3] == 1
     )
    {
    return true;
    }
  else
    {
    return false;
    }
}

/******************************************************************************
 * bool vtkDataCollector::IsMatrixEmpty(vtkMatrix4x4 * matrix)
 *
 * Check if matrix "matrix" is identity matrix
 *
 *  @Author:Jan Gumprecht
 *  @Date:  13.February 2009
 *
 *  @Param: vtkMatrix4x4 * matrix
 *
 *  @Return:  true if matrix is empty
 *            false if not matrix is not empty
 *
 * ****************************************************************************/
bool vtkDataCollector::IsMatrixEmpty(vtkMatrix4x4 * matrix)
{
  if(   matrix->Element[0][0] == 0
     && matrix->Element[0][1] == 0
     && matrix->Element[0][2] == 0
     && matrix->Element[0][3] == 0

     && matrix->Element[1][0] == 0
     && matrix->Element[1][1] == 0
     && matrix->Element[1][2] == 0
     && matrix->Element[1][3] == 0

     && matrix->Element[2][0] == 0
     && matrix->Element[2][1] == 0
     && matrix->Element[2][2] == 0
     && matrix->Element[2][3] == 0

     && matrix->Element[3][0] == 0
     && matrix->Element[3][1] == 0
     && matrix->Element[3][2] == 0
     && matrix->Element[3][3] == 0
     )
    {
    return true;
    }
  else
    {
    return false;
    }
}

/******************************************************************************
 * int vtkDataCollector::CalculateVolumeProperties(struct DataStruct dataStruct)
 *
 * Calculates the normal of a plane. The plane was parallel to the x and y axis
 * but is transformed according to given transformation matrix
 *
 *  @Author:Jan Gumprecht
 *  @Date:  13.February 2009
 *
 *  @Param: struct DataStruct dataStruct
 *
 *  @Return:  0 on success
 *           -1 on failure or if volume is too big
 *
 * ****************************************************************************/
int vtkDataCollector::CalculateVolumeProperties(struct DataStruct* pDataStruct)
{
  if(!this->Initialized)
    {
    #ifdef ERRORCOLLECTOR
      this->LogStream << this->GetUpTime() << " |C-ERROR: Cannot calculate volume properties; Data Collector not initialized" << endl;
    #endif
    return -1;
    }

//  if(!this->DynamicVolumeSize && this->VolumeInitialized)
//    {
//    pDataStruct->Origin[0] = this->FixedVolumeProperties.Origin[0];
//    pDataStruct->Origin[1] = this->FixedVolumeProperties.Origin[1];
//    pDataStruct->Origin[2] = this->FixedVolumeProperties.Origin[2];
//
//    pDataStruct->VolumeExtent[0] = this->FixedVolumeProperties.VolumeExtent[0];
//    pDataStruct->VolumeExtent[1] = this->FixedVolumeProperties.VolumeExtent[1];
//    pDataStruct->VolumeExtent[2] = this->FixedVolumeProperties.VolumeExtent[2];
//    pDataStruct->VolumeExtent[3] = this->FixedVolumeProperties.VolumeExtent[3];
//    pDataStruct->VolumeExtent[4] = this->FixedVolumeProperties.VolumeExtent[4];
//    pDataStruct->VolumeExtent[5] = this->FixedVolumeProperties.VolumeExtent[5];
//    return 0;
//    }

  double maxX, maxY, maxZ;
  double minX, minY, minZ;
  double xOrigin, yOrigin, zOrigin;
  double xmin, xmax, ymin, ymax;

  xmin = this->clipRectangle[0], ymin = this->clipRectangle[1],
  xmax = this->clipRectangle[2], ymax = this->clipRectangle[3];

  double imCorners[4][4]= {
    { xmin, ymin, 0, 1},
    { xmin, ymax, 0, 1},
    { xmax, ymin, 0, 1},
    { xmax, ymax, 0, 1} };

  double transformedPt[4];

  maxX = maxY = maxZ = -1 * numeric_limits<double>::max();

  minX = minY = minZ = numeric_limits<double>::max();

  // Determine dimensions of reconstructed volume
  for(int j=0; j < 4; j++)
    {
    pDataStruct->Matrix->MultiplyPoint(imCorners[j],transformedPt);
    minX = min(transformedPt[0], minX);
    minY = min(transformedPt[1], minY);
    minZ = min(transformedPt[2], minZ);
    maxX = max(transformedPt[0], maxX);
    maxY = max(transformedPt[1], maxY);
    maxZ = max(transformedPt[2], maxZ);
    }

  minX = floor(minX);
  minY = floor(minY);
  minZ = floor(minZ);

  maxX = floor(maxX);
  maxY = floor(maxY);
  maxZ = floor(maxZ);

  xOrigin = minX;
  yOrigin = minY;
  zOrigin = minZ;

//  if(!this->DynamicVolumeSize)
//    {
//    xOrigin = xOrigin + (int)((maxX - minX)) - this->MaximumVolumeSize[0] / 2;
//    yOrigin = yOrigin + (int)((maxY - minY)) - this->MaximumVolumeSize[1] / 2;
//    zOrigin = zOrigin + (int)((maxZ - minZ)) - this->MaximumVolumeSize[2] / 2;
//
//    minX = 0;
//    minY = 0;
//    minZ = 0;
//
//    maxX = this->MaximumVolumeSize[0] - 1;
//    maxY = this->MaximumVolumeSize[1] - 1;
//    maxZ = this->MaximumVolumeSize[2] - 1;
//
//    this->FixedVolumeProperties.Origin[0] = xOrigin;
//    this->FixedVolumeProperties.Origin[1] = yOrigin;
//    this->FixedVolumeProperties.Origin[2] = zOrigin;
//
//    this->FixedVolumeProperties.VolumeExtent[0] = 0;
//    this->FixedVolumeProperties.VolumeExtent[1] = (int) maxX;
//    this->FixedVolumeProperties.VolumeExtent[2] = 0;
//    this->FixedVolumeProperties.VolumeExtent[3] = (int) maxY;
//    this->FixedVolumeProperties.VolumeExtent[4] = 0;
//    this->FixedVolumeProperties.VolumeExtent[5] = (int) maxZ;
//
//    this->VolumeInitialized = true;
//    }

  pDataStruct->Origin[0] = xOrigin;
  pDataStruct->Origin[1] = yOrigin;
  pDataStruct->Origin[2] = zOrigin;

  pDataStruct->VolumeExtent[0] = 0;
  pDataStruct->VolumeExtent[1] = (int)((maxX - minX));
  pDataStruct->VolumeExtent[2] = 0;
  pDataStruct->VolumeExtent[3] = (int)((maxY - minY));
  pDataStruct->VolumeExtent[4] = 0;
  pDataStruct->VolumeExtent[5] = (int)((maxZ - minZ));

  #ifdef DEBUGCOLLECTOR
  this->LogStream << this->GetUpTime() << " |C-INFO: Volume Properties of new Frame: " << endl;
  if(this->DynamicVolumeSize)
    {
               this->LogStream << "         |        Clip Rectangle: "<< xmin << "-" << xmax << " | " << ymin << "-" << ymax << endl;
    }
               this->LogStream << "         |        Origin: " << pDataStruct->Origin[0]<<"| "<< pDataStruct->Origin[1] <<"| "<<  pDataStruct->Origin[2]<< endl
                               << "         |        Extent:  "<< pDataStruct->VolumeExtent[0]<<"-"<<pDataStruct->VolumeExtent[1] <<" | "<< pDataStruct->VolumeExtent[2]<<"-"<< pDataStruct->VolumeExtent[3] <<" | "<< pDataStruct->VolumeExtent[4]<<"-"<< pDataStruct->VolumeExtent[5]<<" "<<endl;
  #endif

  return 0;
}

/******************************************************************************
 * int vtkDataCollector::Calculate(struct DataStruct dataStruct, double* normal)
 *
 * Calculates the normal of a plane. The plane was parallel to the x and y axis
 * but is transformed according to given transformation matrix
 *
 *  @Author:Jan Gumprecht
 *  @Date:  13.February 2009
 *
 *  @Param: struct DataStruct dataStruct
 *  @Param: double* normal
 *
 *  @Return:  0 on success
 *           -1 on failure
 *
 * ****************************************************************************/
int vtkDataCollector::CalculateNormal(struct DataStruct dataStruct, double* normal)
{
  if(dataStruct.Matrix == NULL)
    {
    #ifdef ERRORCOLLECTOR
      this->LogStream << this->GetUpTime() << " |C-ERROR: Cannot calculate normal no data provided" << endl;
    #endif
    return -1;
    }

  double imCorners[4][4]= {
    { 0, 0, 0, 1},
    { 0, 1, 0, 1},
    { 1, 0, 0, 1},
    { 1, 1, 0, 1} };

  double transformedPt[4];
  double minXminY[3], maxXminY[3], minXmaxY[3], maxXmaxY[3];

  //Calculate 2 vectors within the plane---------------------------------------
  dataStruct.Matrix->MultiplyPoint(imCorners[0],transformedPt);
  minXminY[0] = transformedPt[0];
  minXminY[1] = transformedPt[1];
  minXminY[2] = transformedPt[2];

  dataStruct.Matrix->MultiplyPoint(imCorners[1],transformedPt);
  maxXminY[0] = transformedPt[0];
  maxXminY[1] = transformedPt[1];
  maxXminY[2] = transformedPt[2];

  dataStruct.Matrix->MultiplyPoint(imCorners[2],transformedPt);
  minXmaxY[0] = transformedPt[0];
  minXmaxY[1] = transformedPt[1];
  minXmaxY[2] = transformedPt[2];

  double vector1[3];
  vector1[0] = minXmaxY[0] - minXminY[0];
  vector1[1] = minXmaxY[1] - minXminY[1];
  vector1[2] = minXmaxY[2] - minXminY[2];

  double vector2[3];
  vector2[0] = maxXminY[0] - minXminY[0];
  vector2[1] = maxXminY[1] - minXminY[1];
  vector2[2] = maxXminY[2] - minXminY[2];

  //Calculate normal------------------------------------------------------------
  normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
  normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
  normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];

  //Normalize-------------------------------------------------------------------
  double length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
  normal[0] /= length;
  normal[1] /= length;
  normal[2] /= length;

  #ifdef DEBUGCOLLECTOR
    this->GetLogStream() << this->GetUpTime() << " |C-INFO: Normal Vector is: "<< normal[0] << "|" << normal[1] << "|" << normal[2] << endl;
  #endif

return 0;
}

/******************************************************************************
 * double vtkDataCollector::GetMaximumVolumeSize()
 *
 * Returns the maximum volume size the system can handle
 *
 *  @Author:Jan Gumprecht
 *  @Date:  13.February 2009
 *
 *  @Return:  Maximum volume size the system can handle
 *
 * ****************************************************************************/
double vtkDataCollector::GetMaximumVolumeSize()
{
  double retVal;

  if(DEFAULT_MAXIMUM_VOLUME_SIZE == this->MaximumVolumeSize[0])
    {//Nothing was specified
    double xMin = this->clipRectangle[0], yMin = this->clipRectangle[1],
           xMax = this->clipRectangle[2], yMax = this->clipRectangle[3];
    double width = xMax - xMin;
    double height = yMax - yMin;
    double depth = width;

    retVal = width * height * depth * depth;
    }
  else
    {
    retVal = this->MaximumVolumeSize[0] * this->MaximumVolumeSize[1] * this->MaximumVolumeSize[2];
    }

//  #ifdef DEBUGCOLLECTOR
//    this->GetLogStream() << this->GetUpTime() << " |C-INFO: Maximum volume size: "<< retVal << endl;
//  #endif

}

/******************************************************************************
 * int vtkDataCollector::DuplicateFrame(vtkImageData * original, vtkImageData * duplicate)
 *
 *  Duplicates Image inData to Image outData
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Param: vtkImageData * original - Original
 *  @Param: vtkImageData * copy - Copy
 *
 * ****************************************************************************/
int vtkDataCollector::DuplicateFrame(vtkImageData * original, vtkImageData * duplicate)
{
  if(this->DataProcessor != NULL)
    {
    this->DataProcessor->DuplicateImage(original, duplicate);
    return 0;
    }
  else
    {
    return -1;
    }
}

/******************************************************************************
 * int vtkDataCollector::GrabOneImage()
 *
 *  Grabs one image and stores it on the HDD at the specified directory
 *  and with the specified name
 *
 *  @Author:Jan Gumprecht
 *  @Date:  30.July 2009
 *
 * ****************************************************************************/
int vtkDataCollector::GrabOneImage()
{
  vtkBMPWriter *writer = vtkBMPWriter::New();
  char filename[256];

  for (int i = 0; i < 10; ++i)
    {
    this->GetVideoSource()->Grab();
    vtkSleep(0.2);
    }
  this->GetVideoSource()->FastForward();

  writer->SetInput(this->GetTagger()->GetOutput());
  sprintf(filename,this->GetFileName());
  writer->SetFileName(filename);
  writer->Update();

  writer->Delete();

  return 0;
}
