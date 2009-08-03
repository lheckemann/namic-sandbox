/*========================================================================

Module:  $RCSfile: vtkDataProcessor.cxx,v $
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
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
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <assert.h>

#define NOMINMAX
//#define REMOVE_ALPHA_CHANNEL

//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkBMPWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"

#include "vtkObjectFactory.h"
#include "vtkDataProcessor.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkUltrasoundCalibFileReader.h"
#include "vtk3DPanoramicVolumeReconstructor.h"

#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

#include "vtkDataSender.h"

#define US_IMAGE_FAN_RATIO 0.6 //Amount of ultrasound images's height that
                               //is used by the ultrasound fan
#define DEFAULT_MAXIMUM_VOLUME_SIZE -1
#define DEFAULT_DELAY_FACTOR 50

using namespace std;

static inline void vtkSleep(double duration);

vtkCxxRevisionMacro(vtkDataProcessor, "$Revision$");
vtkStandardNewMacro(vtkDataProcessor);

/******************************************************************************
 * vtkDataProcessor()
 *
 *  Constructor
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 * ****************************************************************************/
vtkDataProcessor::vtkDataProcessor()
{
  this->Verbose = false;
  this->VolumeReconstructionEnabled = false;
  this->Processing = false;
  this->ProcessPeriod = 1 / 30;
  this->UltraSoundTrackingEnabled = false;
  this->MaximumVolumeSize[0] = DEFAULT_MAXIMUM_VOLUME_SIZE;
  this->MaximumVolumeSize[1] = DEFAULT_MAXIMUM_VOLUME_SIZE;
  this->MaximumVolumeSize[2] = DEFAULT_MAXIMUM_VOLUME_SIZE;
  this->DelayFactor = DEFAULT_DELAY_FACTOR;

  this->dataBufferSize = 3;
  this->dataBufferIndex = -1;

  this->clipRectangle[0] = 0;
  this->clipRectangle[1] = 0;
  this->clipRectangle[2] = 0;
  this->clipRectangle[3] = 0;

  this->PlayerThreader = vtkMultiThreader::New();
  this->PlayerThreadId = -1;
  this->DataBufferLock = vtkMutexLock::New();
  this->DataBufferIndexQueueLock = vtkMutexLock::New();

  this->CalibrationFileName = NULL;
  this->calibReader = vtkUltrasoundCalibFileReader::New();

  this->DataSender = NULL;

  this->StartUpTime = vtkTimerLog::GetUniversalTime();
  this->LogStream.ostream::rdbuf(cerr.rdbuf());
  this->oldVolume = NULL;

  this->DynamicVolumeSize = false;
  this->VolumeInitialized = false;
}

/******************************************************************************
 * ~vtkDataProcessor()
 *
 *  Destructor
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 * ****************************************************************************/
vtkDataProcessor::~vtkDataProcessor()
{
  this->StopProcessing();
  if(VolumeReconstructionEnabled)
    {
    //Delete all buffer objects
    while(!this->IsDataBufferEmpty())
      {
      this->DeleteData(this->GetHeadOfDataBuffer());
      }
    }

  if(this->Reconstructor)
    {
    this->Reconstructor->Delete();
    }

  if(this->DataBufferLock)
    {
  this->DataBufferLock->Delete();
    }

  if(this->DataBufferIndexQueueLock)
    {
    this->DataBufferIndexQueueLock->Delete();
    }

  if(this->PlayerThreader)
    {
    this->PlayerThreader->Delete();
    }
  if(this->calibReader)
    {
    this->calibReader->Delete();
    }

  this->SetCalibrationFileName(NULL);
}
/******************************************************************************
 *  PrintSelf(ostream& os, vtkIndent indent)
 *
 *  Print information about the instance
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 * ****************************************************************************/
void vtkDataProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
  //Not implemented yet
  this->Superclass::PrintSelf(os,indent);
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
 *  @Date:  27.January 2009
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data *
 *  @Param: double time - Time until which to sleep
 *  @Param: bool checkActiveFlagImmediately - avoid waiting if enabled
 *
 * ****************************************************************************/
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time, bool checkActiveFlagImmediately)
{
  if(checkActiveFlagImmediately)
    {
    // check to see if we are being told to quit
      data->ActiveFlagLock->Lock();
      int activeFlag = *(data->ActiveFlag);
      data->ActiveFlagLock->Unlock();

      if (activeFlag == 0)
        {//We are told to quit
        return 0;
        }

      //Go on processing
      return 1;
    }

  // loop either until the time has arrived or until the thread is ended
  for (int i = 0;; i++)
    {
      double remaining = time - vtkTimerLog::GetUniversalTime();

      // check to see if we have reached the specified time
      if (remaining <= 0)
        {
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
 *  static void *vtkDataProcessorThread(vtkMultiThreader::ThreadInfo *data)
 *
 *  This function runs in an alternate thread to asyncronously collect data
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.Januar 2009
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data
 *
 * ****************************************************************************/
static void *vtkDataProcessorThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkDataProcessor *self = (vtkDataProcessor *)(data->UserData);
  float processPeriod = self->GetProcessPeriod();
  int lastDataSenderIndex = -2;
  int currentIndex = -1;
  bool dataAvailable = false;
  int errors = 0;
  string input;
  double sectionTime;
  double loopTime;

  #ifdef  DEBUGPROCESSOR
    self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Processor Thread started" << endl;
  #endif

  do
    {
    #ifdef DEBUGPROCESSOR
    //self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Processor Thread looks for new data" << endl;
    #endif

    if(!self->GetDataSender()->GetSending())
      {
      #ifdef DEBUGPROCESSOR
      self->GetLogStream() <<  self->GetUpTime() << " |P-WARNING: Data sender stopped sending -> stop processing" << endl;
      #endif
      self->StopProcessing();
      dataAvailable = true; //To check immediately if we received a termination signal
      }
    else
      {
      if(self->GetDataSender()->IsSendDataBufferFull())
        {
        #ifdef DEBUGPROCESSOR
          self->GetLogStream() <<  self->GetUpTime() << " |P-WARNING: Data sender is too slow -> wait" << endl;
        #endif
        }
      else
        {
        if(dataAvailable || !self->IsDataBufferEmpty())
          {//New data to process available
          loopTime = self->GetUpTime();
          currentIndex = self->GetHeadOfDataBuffer();

          #ifdef  TIMINGPROCESSOR
            self->GetLogStream() <<  self->GetUpTime() << " |-------------------------------------" << endl;
            self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Processor Thread found new data with index:" << currentIndex
                                 << " | L:" << self->GetUpTime() - loopTime << endl
                                 << "        | BufferSize: " << self->GetBufferSize() << endl;

          #endif
          //If frame is too old skip it
          if(!self->IsDataExpired(currentIndex))
            {
            //Check and Update Volume=============================================
            sectionTime = self->GetUpTime();
            if(-1 != self->CheckandUpdateVolume(currentIndex, lastDataSenderIndex))//Check if volume must be expanded
              {
              #ifdef  TIMINGPROCESSOR
                self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Check and Update DONE" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
              #endif

              //Reconstruct Volume================================================
              sectionTime = self->GetUpTime();
              if(-1 != self->ReconstructVolume(currentIndex))
                {
                #ifdef  TIMINGPROCESSOR
                  self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Volume Reconstruction DONE" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
                #endif

                //Forward Data====================================================
                sectionTime = self->GetUpTime();
                lastDataSenderIndex = self->ForwardData(self->GetReconstructor()->GetOutput());
                #ifdef  TIMINGPROCESSOR
                  self->GetLogStream() << self->GetUpTime() << " |P-INFO: Volume forwarding DONE" << endl
                                                   << "          | Last data sender index: " << lastDataSenderIndex
                                                            << " | S: " << self->GetUpTime() - sectionTime
                                                            << " | L:" << self->GetUpTime() - loopTime
                                                            << " | FPS: " << 1 / (self->GetUpTime() - loopTime) << endl;
                #endif
                if(lastDataSenderIndex < 0)
                  {
                  errors++;
                  }
                }
              else
                {
                #ifdef  DEBUGPROCESSOR
                  self->GetLogStream() <<  self->GetUpTime() << " |P-WARNING: Volume Reconstruction failed" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
                  cerr <<  self->GetUpTime() << " |P-WARNING: Volume Reconstruction failed" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
                #endif
  //              self->ResetOldVolume(lastDataSenderIndex);
                errors++;
                }
              }
  //          else
  //            {
  //            lastDataSenderIndex = -2;
  //            }
            }
          else
            {
            #ifdef  DEBUGPROCESSOR
               self->GetLogStream() <<  self->GetUpTime() << " |P-WARNING: Data too old for reconstruction (Index: "<< currentIndex << " )" << endl;
               cout <<  self->GetUpTime() << " |P-WARNING: Data too old for reconstruction (Index: "<< currentIndex << " )" << endl;
            #endif
            }
            self->DeleteData(currentIndex);
          }
        dataAvailable = !self->IsDataBufferEmpty();

        if(errors > 50)
          {
          #ifdef ERRORPROCESSOR
          self->GetLogStream() <<  self->GetUpTime() << " |P-ERROR: too many errors ( " << errors << " ) occured terminate data processor" << endl;
          #endif
          self->ResetOldVolume(lastDataSenderIndex); //Delete unnecessary data
  //        self->ResetOldVolume(-1); //Delete copy of last reconstructed volume if necessary
          self->StopProcessing();
          self->GetDataSender()->StopSending();
          }

        #ifdef  DEBUGPROCESSOR
          if(dataAvailable)
            {
            self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Data Processor wont't sleep" << " | " << self->GetUpTime()<< endl;
            }
          else
            {
            self->GetLogStream() <<  self->GetUpTime() << " |P-INFO: Data Processor sleeps now" << " | " << self->GetUpTime()<< endl;
            }
        #endif
        }
      }//Check if sender stopped sending
    }
  while(vtkThreadSleep(data, vtkTimerLog::GetUniversalTime() + processPeriod, dataAvailable));

  return NULL;
}

/******************************************************************************
 *  int vtkDataProcessor::StartProcessing(vtkDataSender * sender)
 *
 *  Starts process thread
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.Januar 2009
 *
 *  @Param: vtkDataSender * sender: DataSender to which proccessed data is
 *                                  forwarded
 *
 *  @Return: 0 on success, 1 on warning, -1 on error
 *
 * ****************************************************************************/
int vtkDataProcessor::StartProcessing(vtkDataSender * sender)
{
  if(this->Processing)
    {
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-WARNING: Data Processor already processes data" << endl;
    #endif
    return 1;
    }

  if(sender != NULL)
    {
    this->DataSender = sender;
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: No data sender provided. Data processing not possible" << endl;
    #endif
    return -1;
    }

  if(!this->VolumeReconstructionEnabled)
    {//Nothing to start
    this->Processing = true;
    return 0;
    }

  //Start process thread
  this->PlayerThreadId =
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkDataProcessorThread, this);

  //Check if thread successfully started
  if(this->PlayerThreadId != -1)
    {
    this->Processing = true;
    if(Verbose)
      {
      cout << "Start processing data" << endl;
      }
    return 0;
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Could not start process thread" << endl;
    #endif
    return -1;
    }
}

/******************************************************************************
 *  int vtkDataProcessor::StopProcessing()
 *
 *  Stops the process thread if running
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.Januar 2009
 *
 *  @Return: Return 0 on sucess, 1 for warning
 *
 * ****************************************************************************/
int vtkDataProcessor::StopProcessing()
{
  if(NULL != this->DataSender)
    {
    this->DataSender->StopSending();
    }

  if(this->Processing)
    {//Stop thread
        this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Processing = false;

    if(this->Verbose)
      {
      cout << "Stop processing" <<endl;
      }
    }
  else
    {
//    #ifdef  DEBUGPROCESSOR
//      this->LogStream << this->GetUpTime()  << " |P-WARNING: Try to stop processor thread although not running" << endl;
//    #endif
    return 1;
    }

  #ifdef  DEBUGPROCESSOR
    this->LogStream << this->GetUpTime()  << " |P-INFO: Data processor stopped processing" << endl;
  #endif

  return 0;
}

/******************************************************************************
 * int vtkDataProcessor::EnableVolumeReconstruction(bool flag)
 *
 *  Turn volume reconstruction ON or OFF
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 *  @Param: bool flag - to set the state of volume reconstruction
 *
 *  @Return: 0 on success
 *
 * ****************************************************************************/
int vtkDataProcessor::EnableVolumeReconstruction(bool flag)
{
  if(flag && !this->VolumeReconstructionEnabled)
    {//Reconstruction was not enabled and shall be enabled now

    // set up the panoramic reconstruction class
    this->Reconstructor = vtk3DPanoramicVolumeReconstructor::New();
//    this->Reconstructor->CompoundingOn();
    this->Reconstructor->CompoundingOff();
    this->Reconstructor->SetInterpolationModeToLinear();

    this->calibReader->SetFileName(this->CalibrationFileName);
    if(-1 == this->calibReader->ReadCalibFile())
       {
       #ifdef ERRORPROCESSOR
       this->GetLogStream() << this->GetUpTime() << " |P-ERROR: Can not read calibration file => Volume reconstruction not possible" << endl;
       #endif
       return -1;
       }
    this->calibReader->GetClipRectangle(this->clipRectangle);
    this->Reconstructor->SetClipRectangle(this->clipRectangle);

    if(this->MaximumVolumeSize[0] == DEFAULT_MAXIMUM_VOLUME_SIZE)
      {
      this->MaximumVolumeSize[0] = this->calibReader->GetMaximumVolumeSize()[0];
      this->MaximumVolumeSize[1] = this->calibReader->GetMaximumVolumeSize()[1];
      this->MaximumVolumeSize[2] = this->calibReader->GetMaximumVolumeSize()[2];
      }

    this->ProcessPeriod = 1 / this->calibReader->GetFramesPerSecond();

    if(this->DelayFactor == DEFAULT_DELAY_FACTOR)
      {
      this->DelayFactor = this->calibReader->GetDelayFactor();
      }

    this->ReconstructorLifeTime = 500;
    }

  if(!flag && this->VolumeReconstructionEnabled)
    {//Turn of volume reconstruction
    if(this->Reconstructor)
      {
      this->Reconstructor->Delete();
      this->Reconstructor = NULL;
      }
    }

  this->VolumeReconstructionEnabled = flag;

  #ifdef  DEBUGPROCESSOR
    //this->LogStream << this->GetUpTime()  << " |P-INFO: Reconstruction started" << endl;
  #endif

  return 0;
}

/******************************************************************************
 * int vtkDataProcessor::CheckandUpdateVolume(int index, int dataSenderIndex)
 *
 *  Check if new frame at index "index" fits into existing volume. If expand
 *  volume appropriately
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 *  @Param: int index - Buffer index of new frame and tracker matrix
 *  @Param: int dataSenderIndex: - Flag to check if this is the first time we
 *                                 reconstruct a volume
 *
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::CheckandUpdateVolume(int index, int dataSenderIndex)
{
  int retVal = 0;
  bool expansionPossible = true;

//  if(!this->DynamicVolumeSize && this->VolumeInitialized && dataSenderIndex == -2)
//    {
//    #ifdef  WARNINGPROCESSOR
//      this->LogStream << this->GetUpTime()  << " |P-WARNING: Data sender index reset" <<endl;
//    #endif
//    dataSenderIndex = -2;
//    }

  if(!this->VolumeReconstructionEnabled)
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to check and update a volume without reconstruction enabled" <<endl;
    #endif
    return -1;
    }

  if(this->IsIndexAvailable(index)  || index < 0 || index >= this->dataBufferSize)
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to check and update a volume with empty index: " << index <<endl;
    #endif
    return -1;
    }


  //Read New Data---------------------------------------------------------------
  this->DataBufferLock->Lock();
  vtkImageData * newFrame = (this->dataBuffer[index]).Frame;
  vtkMatrix4x4 * newTrackerMatrix = this->dataBuffer[index].Matrix;

  vtkFloatingPointType newOrigin[3] = {this->dataBuffer[index].Origin[0],
                                       this->dataBuffer[index].Origin[1],
                                       this->dataBuffer[index].Origin[2]};

  double spacing[3] = {this->dataBuffer[index].Spacing[0],
                       this->dataBuffer[index].Spacing[1],
                       this->dataBuffer[index].Spacing[2]};

  int newExtent[6] = {this->dataBuffer[index].VolumeExtent[0],
                      this->dataBuffer[index].VolumeExtent[1],
                      this->dataBuffer[index].VolumeExtent[2],
                      this->dataBuffer[index].VolumeExtent[3],
                      this->dataBuffer[index].VolumeExtent[4],
                      this->dataBuffer[index].VolumeExtent[5]};

  #ifdef  DEBUGPROCESSOR
    this->LogStream << this->GetUpTime()  << " |P-INFO: Check and update a volume" <<endl;
    this->LogStream << this->GetUpTime()  << " |P-INFO: Properties New Frame:" << endl
                    << "          | Origin: " << newOrigin[0] << "| " << newOrigin[1] << " |" << newOrigin[2] <<endl
                    << "          | Dimensions: " << newExtent[1] + 1<< "| "<< newExtent[3] + 1<<" | "<< newExtent[5] + 1<< endl
                    << "          | TimeStamp: " << this->dataBuffer[index].TimeStamp <<endl
                    << "          | Matrix:" <<endl;
//    newTrackerMatrix->Print(this->LogStream );
  #endif
  this->DataBufferLock->Unlock();

  //Check if new extent larger than old extent or if origin changed------------
  vtkFloatingPointType oldOrigin[3];
  bool originChanged = false;
  int oldExtent[6];
  bool extentChanged = false;

  if(dataSenderIndex != -2)
    { // -2 == no volume was reconstructed so far

    //Update Origin and Extent
    this->Reconstructor->GetOutputOrigin(oldOrigin);
    this->Reconstructor->GetOutputExtent(oldExtent);
    for(int i = 0; i < 3; ++i)
      {
      if(newOrigin[i] < oldOrigin[i])
        {
        originChanged = true;
        newExtent[2 * i + 1] = (int) (max(newOrigin[i] + newExtent[2 * i + 1], oldOrigin[i] + oldExtent[2 * i + 1]) - newOrigin[i]);
        }
      else
        {
        newExtent[2 * i + 1] = (int) (max(newOrigin[i] + newExtent[2 * i + 1], oldOrigin[i] + oldExtent[2 * i + 1]) - oldOrigin[i]);
        newOrigin[i] = oldOrigin[i];
        if(newExtent[2 * i + 1] < oldExtent[2 * i + 1])
          {
          newExtent[2 * i + 1] = oldExtent[2 * i + 1];
          }
        else if(newExtent[2 * i + 1] > oldExtent[2 * i + 1])
          {
          extentChanged = true;
          }
        }
      }

    #ifdef  DEBUGPROCESSOR
    this->LogStream << this->GetUpTime()  << " |P-INFO: Old Volume: Origin: "<< oldOrigin[0]<<"| "<< oldOrigin[1]<<"| "<<  oldOrigin[2]<< endl
                            << "          |             Extent:  "<< oldExtent[0]<<"-"<<oldExtent[1] <<" | "<< oldExtent[2]<<"-"<< oldExtent[3]
                                                       <<" | "<< oldExtent[4]<<"-"<< oldExtent[5]<<" "<<endl
                            << "          | New Volume: Origin: "<< newOrigin[0]<<"| "<<newOrigin[1] <<"| "<< newOrigin[2]<<"" << endl
                            << "          |             Extent:  "<< newExtent[0]<<"-"<<newExtent[1] <<" | "<< newExtent[2]<<"-"<< newExtent[3]
                                                       <<" | "<< newExtent[4]<<"-"<< newExtent[5]<<" "<<endl;
    #endif
    }

    //Check Size of new volume
    double volumeSize =   (newExtent[1] - newExtent[0] + 1)
                        * (newExtent[3] - newExtent[2] + 1)
                        * (newExtent[5] - newExtent[4] + 1);

//    //Check if volume reconstruction is possible
//    if((dataSenderIndex > this->DataSender->GetSendDataBufferSize() || dataSenderIndex < 0) && this->oldVolume == NULL)
//  //  if(this->oldVolume == NULL)
//      {
//      #ifdef  DEBUGPROCESSOR
//        this->LogStream << this->GetUpTime()  << " |P-WARNING: Data sender index ( " << dataSenderIndex <<" ) invalid no expansion possible: " <<endl;
//      #endif
//      expansionPossible = false;
//      }

//  if((extentChanged || originChanged) && !expansionPossible)
  if(volumeSize > this->GetMaximumVolumeSize() || volumeSize <= 0 || ((extentChanged || originChanged) && this->oldVolume == NULL))
    {
    retVal = -1;

    #ifdef  ERRORPROCESSOR
    if(volumeSize > this->GetMaximumVolumeSize())
      {
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Expansion necessary but not possible since boundary box of inserted frame ("<<volumeSize<< ") is bigger than maxi vol size:" << this->GetMaximumVolumeSize() <<endl;
      cout << this->GetUpTime() << " |P-ERROR: Expansion necessary but not possible since boundary box of inserted frame ("<<volumeSize<< ") is bigger than maxi vol size:" << this->GetMaximumVolumeSize() <<endl;
      }

    if( volumeSize <= 0)
      {
    this->LogStream << this->GetUpTime()  << " |P-ERROR: Ceck and Update aborted since volumeSize could not be computed "<<endl;
          cout << this->GetUpTime() << " |P-ERROR: Ceck and Update aborted since volumeSize could not be computed " <<endl;
      }
    if((extentChanged || originChanged) && this->oldVolume == NULL)
      {
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Expansion necessary but not possible since no oldVolume exists" <<endl;
      cout << this->GetUpTime() << " |P-ERROR: Expansion necessary but not possible since no oldVolume exists" << endl;
      }
     #endif
    }
  else
    {
//    if(volumeSize <= this->GetMaximumVolumeSize())//include in 716
//      {
      //Check if volume properties have changed
      if(dataSenderIndex == -2 || (originChanged || extentChanged))
        {
        #ifdef  DEBUGPROCESSOR
          if(dataSenderIndex == -2)
            {
            this->LogStream << this->GetUpTime()  << " |P-INFO: Create initial volume" << " | "  << endl;
            }
          else
            {
            this->LogStream << this->GetUpTime()  << " |P-INFO: Update Existing volume" << " | "  << endl;
            }
          #endif

        //Create new volume-------------------------------------------------------
        this->Reconstructor->SetOutputExtent(newExtent);
        this->Reconstructor->SetOutputSpacing(spacing);
        this->Reconstructor->SetOutputOrigin(newOrigin);

        this->Reconstructor->SetSlice(newFrame);
        this->Reconstructor->GetOutput()->Update();

        this->Reconstructor->SetSliceAxes(newTrackerMatrix);
        this->Reconstructor->GetOutput()->SetNumberOfScalarComponents(newFrame->GetNumberOfScalarComponents());
        this->Reconstructor->GetOutput()->Update();
    //      this->Reconstructor->GetOutput()->SetNumberOfScalarComponents(2);
        this->Reconstructor->GetOutput()->AllocateScalars();
        this->Reconstructor->ClearOutput();
    //      this->DecreaseLifeTimeOfReconstructor(1);


        if((originChanged || extentChanged) && this->VolumeInitialized && this->oldVolume != NULL)
        //check if old volume available vs. this->volume initialized
          {
          #ifdef  DEBUGPROCESSOR
            this->LogStream << this->GetUpTime()  << " |P-INFO: Expand volume | DataSenderIndex: "<< dataSenderIndex << endl;
          #endif

          #ifdef  DEBUGPROCESSOR
            this->LogStream << this->GetUpTime()  << " |P-INFO: Acquire lock for DataSenderIndex: "<< dataSenderIndex << endl;
          #endif

          //Lock data
          if(-1 == this->DataSender->LockIndex(dataSenderIndex, DATAPROCESSOR) && dataSenderIndex != -1)
            {
            int i = 0;
            do
              {
              if(++i > 100000)
                {
                #ifdef  ERRORPROCESSOR
                  this->LogStream << this->GetUpTime()  << " |P-ERROR: Cannot acquire lock for DataSenderIndex: " << dataSenderIndex << " | TimeWaited: " << i << " ms" <<endl;
                #endif
                }
              vtkSleep(0.01);
              } //Wait for Lock to be available
            while(-1 == this->DataSender->LockIndex(dataSenderIndex, DATAPROCESSOR));
            }

//          if(this->oldVolume == NULL) //include in line 766
//            {
//            if(this->DynamicVolumeSize)
//              {
//              #ifdef  ERRORPROCESSOR
//                this->LogStream << this->GetUpTime()  << " |P-ERROR: No oldVolume available => can not expand volume" << endl;
//              #endif
//              retVal = -1;
//              }
//            }

          //Copy Old Volume at correct position into new volume
//          else
            if(this->MergeVolumes(Reconstructor->GetOutput(),newOrigin,newExtent,
                                     this->oldVolume, oldOrigin,oldExtent,
                                     this->Reconstructor->GetOutput()->GetNumberOfScalarComponents())
                  == -1)
            {
            #ifdef  ERRORPROCESSOR
              this->LogStream << this->GetUpTime()  << " |P-ERROR: Expand failed" << endl;
              cout << this->GetUpTime()  << " |P-ERROR: Expand failed" << endl;
            #endif
            retVal = -1;
            }
          if(dataSenderIndex != -1)
            {
            this->DataSender->ReleaseLock(DATAPROCESSOR);
            }
          }//Merge
        this->VolumeInitialized = true;
        }
//      }// Check size of new volume
//    else
//      {
//      #ifdef  ERRORPROCESSOR
//      this->LogStream << this->GetUpTime()  << " |P-ERROR: Updated volume ist too big: "<< volumeSize << "| Max Volume Size: " << this->GetMaximumVolumeSize()  << endl;
//      cout << this->GetUpTime()  << " |P-ERROR: Updated volume ist too big: "<< volumeSize << "| Max Volume Size: " << this->GetMaximumVolumeSize()  << endl;
//      #endif
//      retVal = -1;
//      }
    }
  this->ResetOldVolume(dataSenderIndex);

  return retVal;
}

/******************************************************************************
 *  int vtkDataProcessor::ReconstructVolume(int index)
 *
 *  Add frame and matrix at index to reconstructed volume
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 *  @Param: int index - Buffer index of frame and matrix
 *
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::ReconstructVolume(int index)
{
  if(!this->VolumeReconstructionEnabled)
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to reconstruct a volume without reconstruction enabled" <<endl;
    #endif
    return -1;
    }

  if(this->IsIndexAvailable(index) || index < 0 || index >= this->dataBufferSize)
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Try to reconstruct volume with empty index: " << index <<endl;
    #endif
    return -1;
    }

    this->DataBufferLock->Lock();
    this->Reconstructor->SetSliceAxes(this->dataBuffer[index].Matrix); //Set current trackingmatrix
    this->Reconstructor->SetSlice(this->dataBuffer[index].Frame);
    this->DataBufferLock->Unlock();

    #ifdef  DEBUGPROCESSOR
      double reconstructionTime = this->GetUpTime();
      this->LogStream << this->GetUpTime()  << " |P-INFO: Start Reconstruction" << endl;
    #endif

    this->Reconstructor->InsertSlice(); //Add current slice to the reconstructor

  //Reconstruction done -> Check for errors
  if( this->Reconstructor->GetPixelCount() > 0)
    {
    #ifdef  DEBUGPROCESSOR
          this->LogStream << this->GetUpTime()  << " |P-INFO: Reconstruction successfully completed" << " | " << endl;
          this->LogStream << "         | Inserted " <<     this->Reconstructor->GetPixelCount() << " pixels into the output volume" <<endl
                          << "         | Reconstruction time: "<< this->GetUpTime() - reconstructionTime << endl;
    #endif
    }
  else
    {
    #ifdef  ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Reconstruction failed" << " | " << endl;
    #endif
    return -1;
    }

//  this->Reconstructor->GetOutput()->Update();

  #ifdef  DEBUGPROCESSOR
    reconstructionTime = this->GetUpTime();
  #endif
  //this->Reconstructor->FillHolesInOutput();
  #ifdef  DEBUGPROCESSOR
    this->LogStream << "         | FillHolesInOutput took: " << this->GetUpTime() - reconstructionTime <<endl;
  #endif

  return 0;
}

/******************************************************************************
 *  int vtkDataProcessor::ForwardData()
 *
 *  Prepare and forward reconstructed volume to DataSender
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Return: Success: Index of DataSender's newDataBuffer were forwarded volume
 *                    is stored
 *           Failure: -1
 *
 * ****************************************************************************/
int vtkDataProcessor::ForwardData(vtkImageData * image)
{
//Postprocess reconstructed volume
#ifdef REMOVE_ALPHA_CHANNEL
  // To remove the alpha channel of the reconstructed volume
  vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
  extract->SetInput(image);
  extract->SetComponents(0);

  vtkImageData * reconstructedVolume = extract->GetOutput();

  if(extract)
    {
    extract->Delete();
    extract = NULL;
    }

#else
  vtkImageData * reconstructedVolume = image;

#endif
  //Generate and fill volume which will be forwarded----------------------------
  vtkImageData* volumeToForward = vtkImageData::New();

  #ifdef DEBUGPROCESSOR
    double copyTime = this->GetUpTime();
    int counter = 0;
  #endif

  //Copy volume
  this->DuplicateImage(reconstructedVolume, volumeToForward);

  //Create and Fill OpenIGTLink Matrix for forwarding---------------------------
  vtkMatrix4x4 * matrix = vtkMatrix4x4::New();
  this->GetVolumeMatrix(matrix);

  vtkMatrix4x4 * adjustMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4 * oldMatrix = vtkMatrix4x4::New();

  //Transform coordinate System-------------------------------------------------
  oldMatrix->DeepCopy(matrix);

  adjustMatrix->Identity();
  adjustMatrix->Element[0][0] = 0;
  adjustMatrix->Element[1][1] = 0;
  adjustMatrix->Element[2][2] = 0;

  adjustMatrix->Element[2][0] = -1;
  adjustMatrix->Element[0][1] =  1;
  adjustMatrix->Element[1][2] = -1;

  vtkMatrix4x4::Multiply4x4(oldMatrix, adjustMatrix, matrix);

  if(adjustMatrix)
    {
    adjustMatrix->Delete();
    adjustMatrix = NULL;
    }
  if(oldMatrix)
    {
    oldMatrix->Delete();
    oldMatrix = NULL;
    }

  double xOrigin = reconstructedVolume->GetOrigin()[0];
  double yOrigin = reconstructedVolume->GetOrigin()[1];
  double zOrigin = reconstructedVolume->GetOrigin()[2];

  reconstructedVolume->SetOrigin(yOrigin, -zOrigin, -xOrigin);

  //Adjust matrix to OpenIGTLink offset-----------------------------------------
  double xLength = (reconstructedVolume->GetDimensions()[0] - 1) / 2;
  double yLength = (reconstructedVolume->GetDimensions()[1] - 1) / 2;
  double zLength = (reconstructedVolume->GetDimensions()[2] - 1) / 2;

  double xOpenIGTLinkOffset = matrix->Element[0][0] * xLength + matrix->Element[0][1] * yLength + matrix->Element[0][2] * zLength;
  double yOpenIGTLinkOffset = matrix->Element[1][0] * xLength + matrix->Element[1][1] * yLength + matrix->Element[1][2] * zLength;
  double zOpenIGTLinkOffset = matrix->Element[2][0] * xLength + matrix->Element[2][1] * yLength + matrix->Element[2][2] * zLength;

  matrix->Element[0][3] = reconstructedVolume->GetOrigin()[0] + xOpenIGTLinkOffset;
  matrix->Element[1][3] = reconstructedVolume->GetOrigin()[1] + yOpenIGTLinkOffset;
  matrix->Element[2][3] = reconstructedVolume->GetOrigin()[2] + zOpenIGTLinkOffset;

  //Forward data to sender------------------------------------------------------
  int retval = this->DataSender->NewData(volumeToForward, matrix);

  //Save tmp reference to volume for volume expansion
  this->oldVolume = volumeToForward;

  if(retval != -1 && retval <= this->DataSender->GetSendDataBufferSize())
    {
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Volume forwarded to data sender " << " | Copytime: " << this->GetUpTime() -  copyTime << endl
                      << "         | Pixels: "<< counter << " | Data Sender Index: " << retval << endl
                      << "         | Volume Dimensions: "<< volumeToForward->GetDimensions()[0] << " | "<< volumeToForward->GetDimensions()[1] << " | "<< volumeToForward->GetDimensions()[2] << " | " << endl
                      << "         | SenderIndex: "<< retval << " | "  << endl;
    #endif
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Could not forward volume to data sender" << " | " << endl;
    #endif
    retval = -1;
    }

  return retval;
}

/******************************************************************************
 * int vtkDataProcessor::AddNewDataToBuffer(int index, struct DataStruct dataStruct)
 *
 *  Adds a new data to the index(key) - data(value) map
 *
 *  @Author:Jan Gumprecht
 *  @Date:  27.January 2009
 *
 *  @Param: int index - Add new data to buffer with this key
 *  @Param: struct DataStruct pDataStruct - New data to add
 *
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::AddNewDataToBuffer(int index, struct DataStruct dataStruct)
{

  this->DataBufferLock->Lock();
  if(index < 0 || index >= this->dataBufferSize)
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: tried to add data to data buffer at invalid index: " << index << endl;
    #endif
    this->DataBufferLock->Unlock();
    return -1;
    }

  if(this->dataBuffer.find(index) != this->dataBuffer.end())
      {
      #ifdef ERRORPROCESSOR
        this->LogStream << this->GetUpTime()  << " |P-ERROR: data buffer already has data at index: " << index << endl;
      #endif
      this->DataBufferLock->Unlock();
      return -1;
      }

  this->dataBuffer[index] = dataStruct;
  this->DataBufferLock->Unlock();

  this->DataBufferIndexQueueLock->Lock();
  this->dataBufferIndexQueue.push(index);//Add Index to new data buffer queue
  this->DataBufferIndexQueueLock->Unlock();
  #ifdef  DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: New Data added at index "<< index << endl;
  #endif

  return 0;
}

/******************************************************************************
 * int vtkDataProcessor::NewData(struct DataStruct dataStruct)
 *
 *  New data arrived -> process it
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: struct DataStruct pDataStruct - Data to add to the buffer
 *
 *  @Return: 0 if new data is successfully stored, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::NewData(struct DataStruct dataStruct)
{
  //Check if data processing started
  if(!this->Processing)
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Data Processor is not started; Cannot accept data" << endl;
    #endif
    return -1;
    }

  //Check if volume reconstruction enabled
  if(!this->VolumeReconstructionEnabled)
    {//No, forward data to data sender
    return this->DataSender->NewData(dataStruct.Frame, dataStruct.Matrix);
    }

  //Check if new data buffer is full
  if(this->IsDataBufferFull())
    {
    #ifdef TIMINGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-WARNING: Processors new data buffer is full" << endl;
    #endif
    return -1;
    }

  //Get new index
  int index = this->IncrementBufferIndex(1);

  //Add new data to Buffer
  if(-1 == this->AddNewDataToBuffer(index, dataStruct))
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Processor cannot add new data to buffer" << endl;
    #endif
    this->IncrementBufferIndex(-1);
    return -1;
    }

  return 0;
}

/******************************************************************************
 *  bool vtkDataProcessor::IsDataBufferEmpty()
 *
 *  Checks if new data buffer is empty
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Return: True, if empty, otherwise false
 *
 * ****************************************************************************/
bool vtkDataProcessor::IsDataBufferEmpty()
{
  this->DataBufferIndexQueueLock->Lock();
  bool retVal = this->dataBufferIndexQueue.empty();
  this->DataBufferIndexQueueLock->Unlock();

  return retVal;
}

/******************************************************************************
 *  bool vtkDataProcessor::IsDataBufferFull()
 *
 *  Checks if new data buffer is full
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Return: True, if full, otherwise false
 *
 * ****************************************************************************/
bool vtkDataProcessor::IsDataBufferFull()
{
  this->DataBufferIndexQueueLock->Lock();
  if(this->dataBufferIndexQueue.size() >= this->dataBufferSize)
    {
    this->DataBufferIndexQueueLock->Unlock();
    return true;
    }
  else
    {
    this->DataBufferIndexQueueLock->Unlock();
    return false;
    }
}

/******************************************************************************
 *  void vtkDataProcessor::GetVolumeMatrix(igtl::Matrix4x4& matrix)
 *
 *  Fills given transformation matrix with correct data
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: Matrix4x4& matrix - Matrix to fill
 *
 * ****************************************************************************/
void vtkDataProcessor::GetVolumeMatrix(vtkMatrix4x4* matrix)
{
  float position[3];
  float orientation[4];

  if(this->UltraSoundTrackingEnabled)
    {
    //NDI tracker matrix looks like
    //  1  0  0  0
    //  0  1  0  0
    //  0  0  1  0
    //  0  0  0  1

    matrix->Element[0][0] =   1.0;  matrix->Element[0][1] =  0.0;  matrix->Element[0][2] =  0.0; matrix->Element[0][3] = 0.0;
    matrix->Element[1][0] =   0.0;  matrix->Element[1][1] =  1.0;  matrix->Element[1][2] =  0.0; matrix->Element[1][3] = 0.0;
    matrix->Element[2][0] =   0.0;  matrix->Element[2][1] =  0.0;  matrix->Element[2][2] =  1.0; matrix->Element[2][3] = 0.0;
    matrix->Element[3][0] =   0.0;  matrix->Element[3][1] =  0.0;  matrix->Element[3][2] =  0.0; matrix->Element[3][3] = 1.0;

    }
  else
    {
    //Tracker simulator matrix looks like
    // -1  0  0  0
    //  0  0  1  0
    //  0  1  0  0
    //  0  0  0  1
    matrix->Element[0][0] =  -1.0;  matrix->Element[0][1] =  0.0;  matrix->Element[0][2] =  0.0; matrix->Element[0][3] = 0.0;
    matrix->Element[1][0] =   0.0;  matrix->Element[1][1] =  0.0;  matrix->Element[1][2] =  1.0; matrix->Element[1][3] = 0.0;
    matrix->Element[2][0] =   0.0;  matrix->Element[2][1] =  1.0;  matrix->Element[2][2] =  0.0; matrix->Element[2][3] = 0.0;
    matrix->Element[3][0] =   0.0;  matrix->Element[3][1] =  0.0;  matrix->Element[3][2] =  0.0; matrix->Element[3][3] = 1.0;

    }

#ifdef DEBUGPROCESSOR
//    this->LogStream << this->GetUpTime()  << " |P-INFO: OpenIGTLink image message matrix" << endl;
//    matrix->Print(this->LogStream);
#endif
}

/******************************************************************************
 *  int vtkDataProcessor::MergeVolumes(vtkImageData* newVolume,
 *                        vtkFloatingPointType* originNewVolume,
 *                        int* extentNewVolume,
 *                        vtkImageData* oldVolume,
 *                        vtkFloatingPointType* originOldVolume,
 *                        int* extentOldVolume)
 *
 *  Merges old Volume in new volume at correct position
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: vtkImageData* newVolume - New volume into which old volume is merged
 *  @Param: vtkFloatingPointType* originNewVolume - Origin of new volume
 *  @Param: int* extentNewVolume - Extent of new volume
 *  @Param: vtkImageData* oldVolume - Old volume which is merged in new volume
 *  @Param: vtkFloatingPointType* originOldVolume - Origin of old volume
 *  @Param: int* extentOldVolume - Old volume's extent
 *
 *  @Return: 0 on success, -1 otherwise
 *
 * ****************************************************************************/
int vtkDataProcessor::MergeVolumes(vtkImageData* newVolume,
                                   vtkFloatingPointType* originNewVolume,
                                   int* extentNewVolume,
                                   vtkImageData* oldVolume,
                                   vtkFloatingPointType* originOldVolume,
                                   int* extentOldVolume,
                                   int ScalarComponents)
{
  if(oldVolume == NULL)
    {
    return -1;
    }

  int x, y, z;

  int counter = 0;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;

  //Calc x, y, z start and end coordinates of old volume in new volume
  int xStart = (int) originNewVolume[0] + ((int) originOldVolume[0] - (int) originNewVolume[0]) * ScalarComponents;
  int xEnd   = xStart + extentOldVolume[1] * ScalarComponents;
  int yStart = (int) originOldVolume[1];
  int yEnd   = yStart + extentOldVolume[3];
  int zStart = (int) originOldVolume[2];
  int zEnd   = zStart + extentOldVolume[5];

  #ifdef DEBUGPROCESSOR
    this->LogStream << this->GetUpTime()  << " |P-INFO: Merge volumes " << endl
                                 << "          |        Dimension new volume: "<< newVolume->GetDimensions()[0] << "| " << newVolume->GetDimensions()[1] << " | " << newVolume->GetDimensions()[2]<< endl;


//      this->LogStream << this->GetUpTime()  << " |P-INFO: Print Infos about volumes " << endl;
//      vtkIndent indent;
//      this->LogStream << this->GetUpTime()  << " |P-INFO: Old Volume " << endl;
//      oldVolume->PrintSelf(this->LogStream, indent);
//      this->LogStream << this->GetUpTime()  << " |P-INFO: New Volume " << endl;
//      newVolume->PrintSelf(this->LogStream, indent);
  #endif

  // Get increments to march through data
  oldVolume->GetContinuousIncrements(extentNewVolume, inIncX, inIncY, inIncZ);
  newVolume->GetContinuousIncrements(extentNewVolume, outIncX, outIncY, outIncZ);

  int rowLength = (extentNewVolume[1] - extentNewVolume[0] + 1) * ScalarComponents + outIncY;
  int oldRowLength = extentOldVolume[1] - extentOldVolume[0] + 1;
  int frameSize = (extentNewVolume[3] - extentNewVolume[2] + 1) * rowLength + outIncZ;

  int spaceBeforeXStart = xStart - (int) originNewVolume[0];
  int spaceAfterXEnd = (int) originNewVolume[0] + extentNewVolume[1] * ScalarComponents - xEnd;

  int spaceBeforeYStart = (yStart - (int) originNewVolume[1] + outIncY) * rowLength;
  int spaceAfterYEnd = ((int) originNewVolume[1] + extentNewVolume[3] * ScalarComponents - yEnd) * rowLength;

  int spaceBeforeZStart = (zStart - (int) originNewVolume[2]) * frameSize;

  //Get Data pointer
  unsigned char* pDataNewVolume = (unsigned char *) newVolume->GetScalarPointer();
  unsigned char* pDataOldVolume = (unsigned char *) oldVolume->GetScalarPointer();

  unsigned char buf;

  try
   {
    //Copy Data
    pDataNewVolume += spaceBeforeZStart;
    for(z = zStart; z <= zEnd; ++z)
      {
      pDataNewVolume += spaceBeforeYStart;
      for(y = yStart; y <= yEnd; ++y)
        {
          pDataNewVolume += spaceBeforeXStart;

          memcpy(pDataNewVolume, pDataOldVolume, oldRowLength);
          pDataOldVolume += oldRowLength;
          pDataNewVolume += oldRowLength;
          counter += oldRowLength;

          pDataNewVolume += spaceAfterXEnd;

          pDataNewVolume += outIncY;
          pDataOldVolume += inIncY;
        }
      pDataNewVolume += spaceAfterYEnd;

      pDataNewVolume += outIncZ;
      pDataNewVolume += inIncZ;
      }

    }//End try
  catch (...)
    {
    throw;
    }

  if(counter != 0)
    {
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Copied " << counter << " Pixel in expanded volume" << endl
                                     << "        |        Volume has "<< (extentOldVolume[1] - extentOldVolume[0] + 1) * (extentOldVolume[3] - extentOldVolume[2] + 1) * (extentOldVolume[5] - extentOldVolume[4] +1)<< "Pixels" << endl;
    #endif
    return 0;
    }
  else
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: No pixels in expanded volume copied" << endl;
    #endif
    return -1;
    }
}

/******************************************************************************
 *  int vtkDataProcessor::DeleteData(int index)
 *
 *  Delete Data at at index "index"
 *
 *  @Author:Jan Gumprecht
 *  @Date:  14.February 2009
 *
 *  @Param: int index - Delete data for this index
 *
 * ****************************************************************************/
int vtkDataProcessor::DeleteData(int index)
{
  if(this->IsIndexAvailable(index) || index < 0 || index >= this->dataBufferSize)
    {
    #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Cannot delete data; Index (" << index << ") empty"<< endl;
    #endif
    return -1;
    }


  this->DataBufferLock->Lock();
  if(this->VolumeReconstructionEnabled)
    {//If not enabled memory is freed at the data sender
    if(this->dataBuffer[index].Frame)
      {
      this->dataBuffer[index].Frame->Delete();
      this->dataBuffer[index].Frame = NULL;
      }
    else
      {
      #ifdef ERRORPROCESSOR
        this->LogStream << this->GetUpTime()  << " |P-ERROR: Valid Index (" << index << ") has no frame object"<< endl;
      #endif
      }

    if(this->dataBuffer[index].Matrix)
      {
      this->dataBuffer[index].Matrix->Delete();
      this->dataBuffer[index].Matrix = NULL;
      }
    else
      {
      #ifdef ERRORPROCESSOR
        this->LogStream << this->GetUpTime()  << " |P-ERROR: Valid Index (" << index << ") has no matrix object"<< endl;
      #endif
      }

    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Deleted data at Index: " << index << endl;
    #endif
    }

  this->dataBuffer.erase(index);
  this->DataBufferLock->Unlock();

  this->DataBufferIndexQueueLock->Lock();
  this->dataBufferIndexQueue.pop();
  this->DataBufferIndexQueueLock->Unlock();

  return 0;
}

/******************************************************************************
 * int vtkDataProcessor::GetHeadOfDataBuffer()
 *
 *  Return index of the head of new data buffer queue
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: int index - Delete data for this index
 *
 * ****************************************************************************/
int vtkDataProcessor::GetHeadOfDataBuffer()
{
  this->DataBufferIndexQueueLock->Lock();
  int head = this->dataBufferIndexQueue.front();
  this->DataBufferIndexQueueLock->Unlock();

  return head;
}

/******************************************************************************
 * int vtkDataProcessor::IncrementBufferIndex(int increment)
 *
 *  Icrement buffer and return new current buffer index
 *
 *  @Author:Jan Gumprecht
 *  @Date:  31.January 2009
 *
 *  @Param: int increment - postions to increment
 *
 * ****************************************************************************/
int vtkDataProcessor::IncrementBufferIndex(int increment)
{
  this->dataBufferIndex = (this->dataBufferIndex + increment) % this->dataBufferSize;

  return this->dataBufferIndex;
}

/******************************************************************************
 * double vtkDataProcessor::GetUpTime()
 *
 *  Returns elapsed Time since program start
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 * ****************************************************************************/
double vtkDataProcessor::GetUpTime()
{
  return vtkTimerLog::GetUniversalTime() - this->GetStartUpTime();
}

/******************************************************************************
 * bool vtkDataProcessor::IsIndexAvailable(int index)
 *
 *  Checks if given index is available
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 *  @Return: True, if index is available
 *           False, if index is used
 *
 * ****************************************************************************/
bool vtkDataProcessor::IsIndexAvailable(int index)
{
  this->DataBufferLock->Lock();
  if(this->dataBuffer.find(index) == this->dataBuffer.end() && index <= this->dataBufferSize && index >= 0)
    {
    this->DataBufferLock->Unlock();
    return true;
    }
  else
    {
    this->DataBufferLock->Unlock();
    return false;
    }
}

/******************************************************************************
 * void vtkDataProcessor::SetLogStream(ofstream &LogStream)
 *
 *  Redirects Logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 * ****************************************************************************/
void vtkDataProcessor::SetLogStream(ofstream &LogStream)
{
  this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
  this->LogStream.precision(6);
  this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
 * ofstream& vtkDataProcessor::GetLogStream()
 *
 *  Returns logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Return: Logstream
 *
 * ****************************************************************************/
ofstream& vtkDataProcessor::GetLogStream()
{
        return this->LogStream;
}

/******************************************************************************
 * void vtkDataProcessor::ResetOldVolume()
 *
 *  Resets the last reconstructed volume
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 * ****************************************************************************/
void vtkDataProcessor::ResetOldVolume(int dataSenderIndex)
{
  if(dataSenderIndex != -1 )
    {
    this->oldVolume = NULL;
    this->DataSender->UnlockData(dataSenderIndex, DATAPROCESSOR);

    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Try to delete data in sender buffer at Index: " << dataSenderIndex << endl;
    #endif

    this->DataSender->TryToDeleteData(dataSenderIndex);
    }
  else
    {
    if(this->oldVolume)
      {
      #ifdef ERRORPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-ERROR: Data sender index ( " << dataSenderIndex << " ) invalid -> Delete Old Volume: "<< endl;
      #endif
      this->oldVolume->Delete();
      this->oldVolume = NULL;

      }
    else
      {
      #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Data sender index ( " << dataSenderIndex << " ) invalid and no volume to delete: "<< endl;
      #endif
      }
    }
}

/******************************************************************************
 * void vtkDataProcessor::DuplicateFrame(vtkImageData * original, vtkImageData * duplicate)
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
void vtkDataProcessor::DuplicateImage(vtkImageData * original, vtkImageData * duplicate)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;
  int rowLength;
  int extent[6];

  //Duplicate Properties
  original->GetExtent(extent);
  duplicate->SetExtent(extent);

  duplicate->SetSpacing(original->GetSpacing());
  duplicate->SetScalarType(original->GetScalarType());
  duplicate->SetOrigin(original->GetOrigin());
  duplicate->SetNumberOfScalarComponents(original->GetNumberOfScalarComponents());

  //Reserve memory
  duplicate->AllocateScalars();

  // find the region to loop over
  rowLength = (extent[1] - extent[0]+1)*original->GetNumberOfScalarComponents();
  maxY = extent[3] - extent[2];
  maxZ = extent[5] - extent[4];

  // Get increments to march through data
  original->GetContinuousIncrements(extent, inIncX, inIncY, inIncZ);
  duplicate->GetContinuousIncrements(extent, outIncX, outIncY, outIncZ);

  unsigned char * outPtr = (unsigned char*) duplicate->GetScalarPointer();
  unsigned char * inPtr = (unsigned char*) original->GetScalarPointer();


  if(inIncY == outIncY && inIncZ == outIncZ)
    {
    long long bytesToCopy = ((rowLength + outIncY) * (maxY + 1) + outIncZ) * (maxZ + 1);
    memcpy(outPtr, inPtr, bytesToCopy);
    }
  else
    {

    //Copy image data
    for (idxZ = 0; idxZ <= maxZ; idxZ++)
      {
      for (idxY = 0; idxY <= maxY; idxY++)
        {
        for (idxR = 0; idxR < rowLength; idxR++)
          {
          // Pixel operation
          *outPtr = *inPtr;
          outPtr++;
          inPtr++;
          }
        outPtr += outIncY;
        inPtr += inIncY;
        }
      outPtr += outIncZ;
      inPtr += inIncZ;
      }
    }
}

/******************************************************************************
 * double vtkDataProcessor::GetMaximumVolumeSize()
 *
 * Returns the maximum volume size the system can handle
 *
 *  @Author:Jan Gumprecht
 *  @Date:  13.February 2009
 *
 *  @Return:  Maximum volume size the system can handle
 *
 * ****************************************************************************/
double vtkDataProcessor::GetMaximumVolumeSize()
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

//    #ifdef DEBUGPROCESSOR
//      this->LogStream << this->GetUpTime()  << " |P-INFO: Maximum Volume Size is "<< retVal << endl;
//    #endif

  return retVal;
}

/******************************************************************************
 * bool vtkDataProcessor::IsDataExpired(int index)
 *
 * Checks if data at index "index" is too old to process
 *
 *  @Author:Jan Gumprecht
 *  @Date:  13.February 2009
 *
 *  @Param: int index - check data at this index
 *
 *  @Return:  true  - if data is too old
 *            false - if data is not too old
 *
 * ****************************************************************************/
bool vtkDataProcessor::IsDataExpired(int index)
{
  bool retVal;

  this->DataBufferLock->Lock();

  if(this->GetUpTime() - this->dataBuffer[index].TimeStamp
      > 0.01 * this->DelayFactor)
    {
    #ifdef DEBUGPROCESSOR
      this->LogStream << this->GetUpTime()  << " |P-INFO: Data is expired; Recording time: "<< this->dataBuffer[index].TimeStamp<<"; Maximum Delay: "<< this->GetProcessPeriod() * this->DelayFactor<< endl;
    #endif
    retVal =  true;
    }
  else
    {
    retVal =  false;
    }

  this->DataBufferLock->Unlock();

  return retVal;
}

/******************************************************************************
 * void vtkDataProcessor::DecreaseLifeTimeOfReconstructor(int decrease)
 *
 *  Decrease lifetime of reconstructor. If reconstructor is dead create a new
 *  reconstructor
 *
 *  @Author:Jan Gumprecht
 *  @Date:  13.February 2009
 *
 *  @Param: int decrease - decrese lifetime of reconstructor about this amount
 *
 * ****************************************************************************/
void vtkDataProcessor::DecreaseLifeTimeOfReconstructor(int decrease)
{
  this->ReconstructorLifeTime -= decrease;

  if(this->ReconstructorLifeTime <= 0)
    {
    this->EnableVolumeReconstruction(false);
    this->EnableVolumeReconstruction(true);
    }
}
