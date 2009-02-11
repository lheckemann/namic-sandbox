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
//#define DEBUG_IMAGES //Write tagger output to HDD
//#define DEBUG_MATRICES //Prints tagger matrices to stdout
#define NEWCOLLECTOR

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

#include "vtkObjectFactory.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#include "vtkDataProcessor.h"

#include "vtkNDITracker.h"
#include "vtkTrackerSimulator.h"

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

vtkCxxRevisionMacro(vtkDataCollector, "$Revision$");
vtkStandardNewMacro(vtkDataCollector);

//----------------------------------------------------------------------------
vtkDataCollector::vtkDataCollector()
{
  this->FrameRate = 30;
  this->ScanDepth = 70; //Unit: mm

  this->FrameBufferSize = 100;

  this->CalibrationFileName = NULL;
  this->calibReader = vtkUltrasoundCalibFileReader::New();

#ifdef USE_ULTRASOUND_DEVICE
  char* devicename = "/dev/video";
  this->SetVideoDevice(devicename);
  this->SetVideoChannel(3); //S-Video at Hauppauge Impact VCB Modell 558
  this->SetVideoMode(1); //NTSC
#endif


  this->Verbose = false;

#ifdef USE_ULTRASOUND_DEVICE
  this->VideoSource = vtkV4L2VideoSource::New();
#else
  this->VideoSource = vtkVideoSourceSimulator::New();
#endif //USE_ULTRASOUND_DEVICE

  this->Tagger = vtkTaggedImageFilter::New();

  this->trackerDeviceEnabled = false;  
  this->trackerSimulator = vtkTrackerSimulator::New();
  this->NDItracker = NULL;
  
  this->DataProcessor = NULL;

  this->PlayerThreader = vtkMultiThreader::New();;
  this->PlayerThreadId = -1;

  this->Collecting = false;
  this->Initialized = false;

  this->StartUpTime = vtkTimerLog::GetUniversalTime();
  this->LogStream.ostream::rdbuf(cerr.rdbuf());

}

//----------------------------------------------------------------------------
vtkDataCollector::~vtkDataCollector()
{
  this->StopCollecting();
  
  if(this->trackerDeviceEnabled)
    {
    this->NDItracker->Delete();
    }
  else
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
//----------------------------------------------------------------------------
void vtkDataCollector::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkDataCollector::Initialize()
{
  if(this->Initialized)
    {
    return 0;
    }

  if(this->calibReader != NULL)
    {
    this->calibReader->SetFileName(this->CalibrationFileName);
    this->calibReader->ReadCalibFile();
    }
  else
    {
    this->LogStream << "C-ERROR: Calibration file reader not specified" << endl;
    return -1;
    }

#ifdef USE_ULTRASOUND_DEVICE
  this->VideoSource->SetVideoDevice(this->GetVideoDevice());
  this->VideoSource->SetVideoChannel(this->GetVideoChannel());
  this->VideoSource->SetVideoMode(this->GetVideoMode());
#endif

  // set up the video source (ultrasound machine)
  this->VideoSource->SetFrameRate(this->FrameRate);
  this->VideoSource->SetFrameBufferSize(this->FrameBufferSize);

  double *imageOrigin = this->calibReader->GetImageOrigin();
  this->VideoSource->SetDataOrigin(imageOrigin);
  double *imageSpacing = this->calibReader->GetImageSpacing();
  this->VideoSource->SetDataSpacing(imageSpacing);

  int *imSize = this->calibReader->GetImageSize();
  this->VideoSource->SetFrameSize(imSize[0], imSize[1], 1);
  //
  // Setting up the synchronization filter
  this->Tagger->SetVideoSource(this->VideoSource);

  // set up the tracker if necessary
  if(-1 == this->StartTracker())
    {
    return -1;
    }
  else
    {
    this->Tagger->Initialize();
    #ifdef USE_ULTRASOUND_DEVICE
    this->VideoSource->Record();
    this->VideoSource->Stop();
    #endif
    this->Initialized = true;
    return 0;
    }

}
//
//----------------------------------------------------------------------------
int vtkDataCollector::StartTracker()
{

  vtkTrackerTool *tool;

  if(this->trackerDeviceEnabled)
    {
    if(this->NDItracker->Probe() != 1)
      {
      this->LogStream << "C-ERROR: Tracking system not found" << endl;
      return -1;
      }
    tool = this->NDItracker->GetTool(0);
    }
  else
    {
    tool = this->trackerSimulator->GetTool(0);
    }

  // make sure the tracking buffer is large enough for the number of the image sequence requested

  tool->GetBuffer()->SetBufferSize((int) (this->FrameBufferSize * CERTUS_UPDATE_RATE / this->FrameRate * FUDGE_FACTOR + 0.5) );
  
  if(this->trackerDeviceEnabled)
    {
    this->NDItracker->StartTracking();
    }
  else
    {
    this->trackerSimulator->StartTracking();
    }
  this->Tagger->SetTrackerTool(tool);
  this->Tagger->SetCalibrationMatrix(this->calibReader->GetCalibrationMatrix());

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


#ifdef DEBUG_IMAGES
  vtkBMPWriter *writer = vtkBMPWriter::New();
  char filename[256];
#endif

  do
    {
    //JG 09/01/21
    //this->LogStream << "New frame to process" <<endl;
    if(!self->GetDataProcessor()->IsNewDataBufferFull())
      {
      //Grab new frame
      self->GetVideoSource()->Grab();

      //Get Tracking Matrix for new frame
      vtkMatrix4x4 *trackerMatrix = vtkMatrix4x4::New();
      self->GetTagger()->Update();
      trackerMatrix->DeepCopy(self->GetTagger()->GetTransform()->GetMatrix());

      #ifdef DEBUG_MATRICES
      self->GetLogStream() << self->GetUpTime() << " |C-INFO Tracker matrix:" << endl;
      trackerMatrix->Print(self->GetLogStream());
      #endif

      if(self->IsTrackerDeviceEnabled())
        {
        self->AdjustMatrix(*trackerMatrix);// Adjust tracker matrix to ultrasound scan depth
        }

      #ifdef NEWCOLLECTOR
      vtkImageData* newFrame = vtkImageData::New();
      self->DuplicateFrame(self->GetTagger()->GetOutput(), newFrame);
      #endif

      #ifdef DEBUG_IMAGES

      #ifdef NEWCOLLECTOR
      writer->SetInput(newFrame);
      #else
      writer->SetInput(self->GetTagger()->GetOutput());
      #endif

      sprintf(filename,"./Output/output%03d.bmp",frame);
      writer->SetFileName(filename);
      writer->Update();
      #endif //DEBUG_IMAGES

      //Send frame + matrix
      #ifdef NEWCOLLECTOR
      if(self->GetDataProcessor()->NewData(newFrame, trackerMatrix) == -1)
      #else
      if(self->GetDataProcessor()->NewData(self->GetTagger()->GetOutput(), trackerMatrix) == -1)
      #endif
        {
        #ifdef DEBUGCOLLECTOR
        self->GetLogStream() << self->GetUpTime() << " |C-WARNING: Data Collector can not forward data to Data Processor" << endl;
        #endif
        }
      }

    frame++;
    }
  while(vtkThreadSleep(data, startTime + frame/rate));

return NULL;
}

//----------------------------------------------------------------------------
int vtkDataCollector::StartCollecting(vtkDataProcessor * processor)
{
  if (!this->Initialized)
    {
    if(!this->Initialize())
      {
        #ifdef ERRORCOLLECTOR
        this->LogStream << this->GetUpTime() << " |C-ERROR: Could not initialize DataCollector" << endl;
        #endif
        return -1;
      }
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
    this->DataProcessor->SetUltraSoundTrackingEnabled(this->trackerDeviceEnabled);
    }
  else
    {
    #ifdef ERRORCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: No data processor provided. Data collection not possible" << endl;
    #endif
    return -1;
    }

  cout << "Hardware Initialization: " << std::flush;
  cout << '\a' << std::flush;
  for(int i = 0; i < 11; i++)
    {
    if(this->trackerDeviceEnabled)
      {
      vtkSleep(1);
      }
    else
      {
      vtkSleep(0.2);
      }
      cout << 10 - i << " " << std::flush;
    }
  cout << endl;

  //cout << "Start Recording" << endl;
  cout << '\a' << std::flush;
  vtkSleep(0.2);
  cout << '\a' << std::flush;

  //Start Thread
  this->PlayerThreadId =
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkDataCollectorThread, this);

  if(this->PlayerThreadId != -1)
    {
    if(Verbose)
      {
      cout << "Start to collect data" << endl;
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
//-------------------------------------------------------------------
int vtkDataCollector::StopCollecting()
{
  if (this->Collecting)
    {
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Collecting = false;
    
    if(this->trackerDeviceEnabled)
      {
      this->NDItracker->StopTracking();
      }
    else
      {
      this->trackerSimulator->StopTracking();
      }

    if(Verbose)
      {
      cout << "Stop collecting" << endl;
      }
    }

  return 0;
}

//-------------------------------------------------------------------
//Adjust tracker matrix to ultrasound scan depth
void vtkDataCollector::AdjustMatrix(vtkMatrix4x4& matrix)
{
  double scaleFactor = (this->VideoSource->GetFrameSize())[1] / this->ScanDepth * US_IMAGE_FAN_RATIO;

  matrix.Element[0][3] = matrix.Element[0][3] * scaleFactor;//x
  matrix.Element[1][3] = matrix.Element[1][3] * scaleFactor;//y
  matrix.Element[2][3] = matrix.Element[2][3] * scaleFactor;//z

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
 * void vtkDataCollector::DuplicateFrame(vtkImageData * original, vtkImageData * duplicate)
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
  return this->trackerDeviceEnabled;
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
  this->trackerDeviceEnabled = true;
  this->NDItracker = vtkNDITracker::New();
  this->trackerSimulator->Delete();
  
  return 0;
}
