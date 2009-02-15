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
#define DEBUG_MATRICES //Prints tagger matrices to stdout

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

/******************************************************************************
 * vtkDataCollector::vtkDataCollector()
 *
 *  @Author:Jan Gumprecht
 *  @Date:  02.February 2009
 *
 * ****************************************************************************/
vtkDataCollector::vtkDataCollector()
{
  this->FrameRate = 30;
  this->ScanDepth = 70; //Unit: mm

  this->FrameBufferSize = 100;
  this->MaximumVolumeSize = -1;

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
  
  this->TrackerOffset = 30; //mm

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
  
  if(!this->TrackerDeviceEnabled)
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
    this->calibReader->ReadCalibFile();
    this->calibReader->GetClipRectangle(this->clipRectangle);
    }
  else
    {
    #ifdef ERRORCOLLECTOR
      this->LogStream << this->GetUpTime() << " |C-ERROR: Calibration file reader not specified" << endl;
    #endif
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
//        self->GetLogStream() << self->GetUpTime() << " |C-INFO Tracker matrix:" << endl;
//        dataStruct.Matrix->Print(self->GetLogStream());
        #endif
        
        //Data Processing and error checking------------------------------------
        if(self->IsTrackerDeviceEnabled())
          {
          if(-1 == self->ProcessMatrix(&dataStruct))
            {//Tracker matrix is unusable
            skip = true;
            dataStruct.Matrix->Delete();
            }
          }
          
        if(!skip)
          {
          //Get new frame-------------------------------------------------------
          dataStruct.Frame = vtkImageData::New();
          self->DuplicateFrame(self->GetTagger()->GetOutput(), dataStruct.Frame);
          dataStruct.Frame->SetNumberOfScalarComponents(1);
    
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
          }
        else
          {
          #ifdef DEBUGCOLLECTOR
            self->GetLogStream() << self->GetUpTime() << " |C-WARNING: Tracker sends unusable matrices" << endl;
          #endif
          }//Check if Processor has buffer space available
        skip = false;
        }
      }//Check if Processor stopped processing

    frame++;
    }
  while(vtkThreadSleep(data, startTime + frame/rate));

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

    if(Verbose)
      {
      cout << "Stop collecting" << endl;
      }
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
    #ifdef DEBUGCOLLECTOR
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
  vtkMatrix4x4 * adjustMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4 * oldMatrix = vtkMatrix4x4::New();
  
  //Adjust Obliqueness----------------------------------------------------------
  oldMatrix->DeepCopy(pDataStruct->Matrix);
  
  adjustMatrix->Identity();  
  adjustMatrix->Element[0][0] = 0;
  adjustMatrix->Element[1][1] = 0;
  adjustMatrix->Element[2][2] = 0;
  
  adjustMatrix->Element[0][0] = 0.93358;
  adjustMatrix->Element[1][0] = 0.35836;
  adjustMatrix->Element[0][1] = -0.358368;
  adjustMatrix->Element[1][1] = 0.933580;
  adjustMatrix->Element[2][2] = 1;

  vtkMatrix4x4::Multiply4x4(oldMatrix, adjustMatrix, pDataStruct->Matrix);
  
//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime() << " |C-INFO: Process Matrix Obliqueness adjusted:"<< endl;
//    pDataStruct->Matrix->Print(this->LogStream);
//  #endif

  //Transform coordinate System-------------------------------------------------
  oldMatrix->DeepCopy(pDataStruct->Matrix);
  
  adjustMatrix->Identity();  
  adjustMatrix->Element[0][0] = 0;
  adjustMatrix->Element[1][1] = 0;
  adjustMatrix->Element[2][2] = 0;
  
  adjustMatrix->Element[0][0] =  1;
  adjustMatrix->Element[2][1] = -1;
  adjustMatrix->Element[1][2] = -1;

  vtkMatrix4x4::Multiply4x4(oldMatrix, adjustMatrix, pDataStruct->Matrix);
  adjustMatrix->Delete();
  oldMatrix->Delete();
  
//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime() << " |C-INFO: Process Matrix coordinate system transformed:"<< endl;
//    pDataStruct->Matrix->Print(this->LogStream);
//  #endif
  
  //Apply Offset----------------------------------------------------------------
  double xOffset = -1 * this->calibReader->GetImageSize()[0] / 2;
  double yOffset = -1 * (this->calibReader->GetImageSize()[1] + this->TrackerOffset);
  double zOffset = -10;
  
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
  
//  #ifdef DEBUGCOLLECTOR
//    this->LogStream << this->GetUpTime() << " |C-INFO: Process Matrix offset applied:"<< endl;
//    pDataStruct->Matrix->Print(this->LogStream);
//  #endif

  //Reshape matrix according to ultrasound scan depth---------------------------
//  double scaleFactor = 1;//(this->VideoSource->GetFrameSize())[1] / this->ScanDepth * US_IMAGE_FAN_RATIO;
//
//  pDataStruct->Matrix->Element[0][3] = pDataStruct->Matrix->Element[0][3] * scaleFactor;//x
//  pDataStruct->Matrix->Element[1][3] = pDataStruct->Matrix->Element[1][3] * scaleFactor;//y
//  pDataStruct->Matrix->Element[2][3] = pDataStruct->Matrix->Element[2][3] * scaleFactor;//z

  if(-1 == this->CalculateVolumeProperties(pDataStruct))
    {
    #ifdef DEBUGCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: Can not calculate volume properties"<< endl;
    #endif
    return -1;
    }
  
  if(pDataStruct->VolumeExtent[1] * pDataStruct->VolumeExtent[3] * pDataStruct->VolumeExtent[5] 
        > this->GetMaximumVolumeSize())
    {
    #ifdef DEBUGCOLLECTOR
    this->LogStream << this->GetUpTime() << " |C-ERROR: Volume of new frame is too big"<< endl;
    #endif
    return -1;
    }
    
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
  
  double xmin = this->clipRectangle[0], ymin = this->clipRectangle[1],
         xmax = this->clipRectangle[2], ymax = this->clipRectangle[3];

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
  
  vtkFloatingPointType newOrigin[3] = {minX, minY, minZ};
  
  pDataStruct->Origin[0] = minX;
  pDataStruct->Origin[1] = minY;
  pDataStruct->Origin[2] = minZ;
  
  int newExtent[6] = { 0, (int)((maxX - minX) / pDataStruct->Spacing[0]),
                       0, (int)((maxY - minY) / pDataStruct->Spacing[1]),
                       0, (int)((maxZ - minZ) / pDataStruct->Spacing[2])};

  pDataStruct->VolumeExtent[0] = newExtent[0];
  pDataStruct->VolumeExtent[1] = newExtent[1];
  pDataStruct->VolumeExtent[2] = newExtent[2];
  pDataStruct->VolumeExtent[3] = newExtent[3];
  pDataStruct->VolumeExtent[4] = newExtent[4];
  pDataStruct->VolumeExtent[5] = newExtent[5];
  
  #ifdef DEBUGCOLLECTOR
  this->LogStream << this->GetUpTime() << " |C-INFO: Volume Properties of new Frame: Origin: " << pDataStruct->Origin[0]<<"| "<< pDataStruct->Origin[1] <<"| "<<  pDataStruct->Origin[2]<< endl
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
  
  if(-1 == this->MaximumVolumeSize)
    {//Nothing was specified
    double xMin = this->clipRectangle[0], yMin = this->clipRectangle[1],
           xMax = this->clipRectangle[2], yMax = this->clipRectangle[3];
    double width = xMax - xMin;
    double height = yMax - yMin;
    double depth = height;
    
    retVal = width * height * depth;
    }
  else
    {
    retVal = this->MaximumVolumeSize;
    }
  
//  #ifdef DEBUGCOLLECTOR
//    this->GetLogStream() << this->GetUpTime() << " |C-INFO: Maximum volume size: "<< retVal << endl;
//  #endif
  
}
