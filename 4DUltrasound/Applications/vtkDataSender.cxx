/*=========================================================================

Module:    $RCSfile: vtkDataSender.cxx,v $
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
#include <assert.h>


#define NOMINMAX
#undef REMOVE_ALPHA_CHANNEL
//#define DEBUG_IMAGES //Write output to HDD

//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkBMPWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"

#include "vtkObjectFactory.h"
#include "vtkDataSender.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkVideoSourceSimulator.h"
#include "vtkUltrasoundCalibFileReader.h"

#include "igtlImageMessage.h"
#include "igtlMath.h"
#include "igtlOSUtil.h"
#include "igtlClientSocket.h"

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625

using namespace std;

vtkCxxRevisionMacro(vtkDataSender, "$Revision$");
vtkStandardNewMacro(vtkDataSender);

//----------------------------------------------------------------------------
vtkDataSender::vtkDataSender()
{
  this->Initialized                   = false;
  this->CalibrationFileName           = NULL;
  this->calibReader                   = vtkUltrasoundCalibFileReader::New();

  this->ServerPort                    = 18944;
  this->OIGTLServer                   = NULL;
  this->SetOIGTLServer("localhost");
  this->SendPeriod                    = 1.0 /(30.0 * 1.5);

  this->socket                        = NULL;
  this->socket                        = igtl::ClientSocket::New();

  this->Verbose                       = false;

  this->Connected                     = false;
  this->Sending                       = false;

  this->PlayerThreader                = vtkMultiThreader::New();;
  this->PlayerThreadId                = -1;
  this->IndexLockedByDataSender       = -1;
  this->IndexLockedByDataProcessor    = -1;
  this->IndexLock                     = vtkMutexLock::New();

  this->SendDataBufferSize            = 10;
  this->SendDataBufferIndex           = -1;

  this->frameProperties.Set           = false;
  this->frameProperties.SubVolumeOffset[0] = 0;
  this->frameProperties.SubVolumeOffset[1] = 0;
  this->frameProperties.SubVolumeOffset[2] = 0;

  this->StartUpTime                   = vtkTimerLog::GetUniversalTime();
  this->LogStream.ostream::rdbuf(cerr.rdbuf());

  this->lastFrameRateUpdate           = 0;
  this->UpDateCounter                 = 0;

  this->TransformationFactorMmToPixel = 10;

  this->Statistics.fpsCounter         = 0;

  this->Statistics.meanFPS            = 0;
  this->Statistics.maxFPS             = -1;
  this->Statistics.minFPS             = 30;

  this->Statistics.volumeCounter      = 0;

  this->Statistics.meanVolumeSize[0]  = 0;
  this->Statistics.meanVolumeSize[1]  = 0;
  this->Statistics.meanVolumeSize[2]  = 0;

  this->Statistics.maxVolumeSize[0]   = 0;
  this->Statistics.maxVolumeSize[1]   = 0;
  this->Statistics.maxVolumeSize[2]   = 0;

  this->Statistics.minVolumeSize[0]   = 0;
  this->Statistics.minVolumeSize[1]   = 0;
  this->Statistics.minVolumeSize[2]   = 0;

  this->ReconstructionEnabled         = false;

  this->FrameCounter                  = 0;

  this->ImageSpacing[0]               = 1;
  this->ImageSpacing[1]               = 1;
  this->ImageSpacing[2]               = 1;
}

//----------------------------------------------------------------------------
vtkDataSender::~vtkDataSender()
{
  int index;

  this->SetOIGTLServer(NULL);
  if(this->PlayerThreader)
    {
    this->PlayerThreader->Delete();
    this->PlayerThreader = NULL;
    }

  this->StopSending();
  this->CloseServerConnection();

  if(this->calibReader)
    {
    this->calibReader->Delete();
    this->calibReader = NULL;
    }

  //Delete all buffer objects
  while(!this->IsSendDataBufferEmpty())
    {
    index = this->GetHeadOfNewDataBuffer();
    this->UnlockData(index, DATASENDER);
    this->UnlockData(index, DATAPROCESSOR);
    this->TryToDeleteData(index);
    }
  if(this->IndexLock)
    {
    this->IndexLock->Delete();
    this->IndexLock = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkDataSender::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkDataSender::Initialize()
{

  if(this->Initialized)
    {
    return 0;
    }

  if(this->calibReader != NULL)
   {
   this->calibReader->SetFileName(this->CalibrationFileName);
   if(-1 == this->calibReader->ReadCalibFile())
     {
     #ifdef ERRORCOLLECTOR
     this->GetLogStream() << this->GetUpTime() << " |S-ERROR: Can not read calibration file => Stop initialization" << endl;
     #endif
     return -1;
     }

   this->SetOIGTLServer(this->calibReader->GetOpenIGTLinkServer());

   this->SetServerPort(this->calibReader->GetOpenIGTLinkServerPortUltrasound ());

   this->calibReader->GetImageSpacing(this->ImageSpacing);

   }
  else
    {
    #ifdef ERRORCOLLECTOR
      this->LogStream << this->GetUpTime() << " |S-ERROR: Calibration file reader not specified" << endl;
    #endif
    return -1;
    }

  this->Initialized = true;
  return 0;
}


//----------------------------------------------------------------------------
int vtkDataSender::ConnectToServer()
{
  // Opening an OpenIGTLink socket
  int e = this->socket->ConnectToServer(this->GetOIGTLServer(), this->ServerPort);

  if(e != 0)
    {
    cout << " ERROR: Failed to connect the OpenIGTLink socket to the server ("<< OIGTLServer <<":"<<  ServerPort << ") Error code : " << e << endl;
    return -1;
    }
  else if(Verbose)
    {
    cout << "Successful connected to the OpenIGTLink server ("<< OIGTLServer <<":"<<  ServerPort << ")" << endl;
    }

  this->Connected = true;

  return 0;
}

//----------------------------------------------------------------------------
int vtkDataSender::CloseServerConnection()
{

  this->socket->CloseSocket();

  if(Verbose && this->Connected)
    {
    cout << "Connection to the OpenIGTLink server ("<< OIGTLServer <<":"<<  ServerPort << ") closed" << endl;
    }

  this->Connected = false;

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
 *  static void *vtkDataSenderThread(vtkMultiThreader::ThreadInfo *data)
 *
 *  This function runs in an alternate thread to asyncronously send data
 *
 *  @Author:Jan Gumprecht
 *  @Date:  20.Januar 2009
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data
 *
 * ****************************************************************************/
static void *vtkDataSenderThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkDataSender *self = (vtkDataSender *)(data->UserData);

  double sendPeriod = self->GetSendPeriod();
  int currentIndex = -1;
  igtl::ImageMessage::Pointer imageMessage;
  int errors = 0;
  bool dataAvailable = false;
  double sectionTime;
  double loopTime;

  do
    {
    if(dataAvailable || !self->IsSendDataBufferEmpty())
      {//New data available
      loopTime = self->GetUpTime();
      currentIndex = self->GetHeadOfNewDataBuffer();

      #ifdef TIMINGSENDER
      self->GetLogStream() << self->GetUpTime() <<" |*********************************************" << endl;
      self->GetLogStream() << self->GetUpTime() <<" |S-INFO: Sender Found new Data at Index: " << currentIndex
                           << " | L:" << self->GetUpTime() - loopTime << endl
                           << "         | BufferSize: " << self->GetCurrentBufferSize() <<endl;
      #endif

      //Prepare new data
      sectionTime = self->GetUpTime();
      if(-1 != self->PrepareImageMessage(currentIndex, imageMessage))
        {
        #ifdef TIMINGSENDER
        self->GetLogStream() << self->GetUpTime() <<" |S-INFO: Message Prepared" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
        #endif

        //Send new data
        sectionTime = self->GetUpTime();
        errors += self->SendMessage(imageMessage);

        #ifdef TIMINGSENDER
        self->GetLogStream() << self->GetUpTime() << " |S-INFO: Message Send"
                                                  << " | S: " << self->GetUpTime() - sectionTime
                                                  << " | L:" << self->GetUpTime() - loopTime
                                                  << " | FPS: " << 1 / (self->GetUpTime() - loopTime) << endl;
        #endif

        }

      //Delete sended data (Remove Index from new data buffer + free maps from data
      errors += self->UnlockData(currentIndex, DATASENDER);

      #ifdef  DEBUGSENDER
      self->GetLogStream() <<  self->GetUpTime() << " |S-INFO: Try to delete data at "<< currentIndex << endl;
      #endif
      errors += self->TryToDeleteData(currentIndex);

      if(errors <= - ERRORTOLERANCE)
        {
        #ifdef  ERRORSENDER
          self->GetLogStream() << self->GetUpTime() <<" |S-ERROR: Stopped sending too many errors occured" << endl;
        #endif
        self->StopSending();
        }
      }

    dataAvailable = !self->IsSendDataBufferEmpty();

    if(dataAvailable)
      {
      #ifdef  DEBUGSENDER
        self->GetLogStream() << self->GetUpTime() <<" |S-INFO: Data available sender won't sleep" << endl;
      #endif
      }
    else
      {
      #ifdef  DEBUGSENDER
        self->GetLogStream() << self->GetUpTime() <<" |S-INFO: NO Data available sender sleeps now" << endl;
      #endif
      }
    }
  while(vtkThreadSleep(data, vtkTimerLog::GetUniversalTime() + sendPeriod, dataAvailable));

  return NULL;
}
//----------------------------------------------------------------------------
bool vtkDataSender::IsSendDataBufferEmpty()
{
  return this->sendDataQueue.empty();
}

//----------------------------------------------------------------------------
bool vtkDataSender::IsSendDataBufferFull()
{
  if(this->sendDataQueue.size() + 1 >= this->SendDataBufferSize)
    {
    #ifdef  DEBUGSENDER
    this->GetLogStream() << this->GetUpTime() <<" |S-INFO: Send Buffer full: " << this->sendDataQueue.size() + 1 << " of "<< this->SendDataBufferSize << " used."<< endl;
    #endif
    return true;
    }
  else
    {
    #ifdef  DEBUGSENDER
    this->GetLogStream() << this->GetUpTime() <<" |S-INFO: Send Buffer NOT full: " << this->sendDataQueue.size() + 1 << " of "<< this->SendDataBufferSize << " used."<< endl;
    #endif
    return false;
    }
}

//----------------------------------------------------------------------------
int vtkDataSender::GetHeadOfNewDataBuffer()
{
  int head = this->sendDataQueue.front();
  this->sendDataQueue.pop();
  return head;
}

//----------------------------------------------------------------------------
int vtkDataSender::IncrementBufferIndex(int increment)
{
  this->SendDataBufferIndex = (this->SendDataBufferIndex + increment) % this->SendDataBufferSize;

  return this->SendDataBufferIndex;
}

//----------------------------------------------------------------------------
int vtkDataSender::StartSending()
{
   if (!this->Connected)
    {
    if(!this->ConnectToServer())
      {
      return 0;
      }
    }

  if (this->Sending)
    {
    #ifdef ERRORSENDER
    this->LogStream <<  this->GetUpTime() << " |S-ERROR: Data collector already collects data" << endl;
    #endif
    return -1;
    }

  //Start send thread
  this->PlayerThreadId =
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkDataSenderThread, this);

  //Check if thread successfully started
  if(this->PlayerThreadId != -1)
    {
    this->Sending = true;
    if(Verbose)
      {
      cout << "Start sending data" << endl;
      }
    return 0;
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Could not start sender thread" << endl;
        #endif
    return -1;
    }
}

//------------------------------------------------------------
int vtkDataSender::StopSending()
{
  //Stop send thread
  if (this->Sending)
    {
    #ifdef DEBUGSENDER
      this->LogStream <<  this->GetUpTime() << " |S-INFO: Stop Sender Thread" << endl;
    #endif
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Sending = false;

    if(this->Verbose)
      {
      cout << "Stop sending" << endl;
      }
    }

  #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Data Sender stopped sending" << endl;
  #endif

  return 0;

}

//------------------------------------------------------------
int vtkDataSender::PrepareImageMessage(int index,
                                       igtl::ImageMessage::Pointer& imageMessage)
{

  if(this->IsIndexAvailable(index) || index < 0 || index >= this->SendDataBufferSize)
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: No data to prepare at index: " << index << endl;
    #endif
    return -1;
    }

  #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Acquire lock for index: " << index << endl;
  #endif

  if(-1 == this->LockIndex(index, DATASENDER))
    {
    int i = 0;
    do
      {
      if(i++ > 100)
        {
        #ifdef  ERRORSENDER
          this->LogStream <<  this->GetUpTime() << " |S-ERROR: Cannot acquire lock for index: " << index << " | TimeWaited: " << i << " ms" <<endl;
        #endif
        }
      vtkSleep(0.01);
      }
    while(-1 == this->LockIndex(index, DATASENDER));
    }

  //Get Data

  vtkImageData * frame = this->sendDataBuffer[index].ImageData;
  vtkMatrix4x4 * matrix = this->sendDataBuffer[index].Matrix;

  #ifdef DEBUG_IMAGES
    vtkBMPWriter *writer = vtkBMPWriter::New();
    char filename[256];
    writer->SetInput(frame);
    sprintf(filename,"./Output/DataSenderFrame%03d.bmp",this->FrameCounter);
    writer->SetFileName(filename);
    writer->Update();
    if(writer)
      {
      writer->Delete();
      writer = NULL;
      }
  #endif //DEBUG_IMAGES

//  if(!this->frameProperties.Set)//Only neccessary once
//    {
    //Get property of frame
    frame->GetDimensions(this->frameProperties.Size);
//    if(this->ReconstructionEnabled)
//      {
//      frame->GetSpacing(this->frameProperties.Spacing);
//      }

    #ifdef HIGH_DEFINITION
    if(this->ReconstructionEnabled)
      {
      this->frameProperties.Spacing[0] = 1 / this->TransformationFactorMmToPixel;
      this->frameProperties.Spacing[1] = 1 / this->TransformationFactorMmToPixel;
      this->frameProperties.Spacing[2] = 1 / this->TransformationFactorMmToPixel;
      }
//    this->frameProperties.Origin[0] = (float) frame->GetOrigin()[0] / this->TransformationFactorMmToPixel;
//    this->frameProperties.Origin[1] = (float) frame->GetOrigin()[1] / this->TransformationFactorMmToPixel;
//    this->frameProperties.Origin[2] = (float) frame->GetOrigin()[2] / this->TransformationFactorMmToPixel;
    this->frameProperties.Origin[0] = (float) frame->GetOrigin()[0];
    this->frameProperties.Origin[1] = (float) frame->GetOrigin()[1];
    this->frameProperties.Origin[2] = (float) frame->GetOrigin()[2];
    #else
    if(this->ReconstructionEnabled)
      {
//      this->frameProperties.Spacing[0] = 1;
//      this->frameProperties.Spacing[1] = 1;
//      this->frameProperties.Spacing[2] = 1;

      this->frameProperties.Spacing[0] = this->ImageSpacing[0];
      this->frameProperties.Spacing[1] = this->ImageSpacing[1];
      this->frameProperties.Spacing[2] = this->ImageSpacing[2];
      }
    this->frameProperties.Origin[0] = (float) frame->GetOrigin()[0];
    this->frameProperties.Origin[1] = (float) frame->GetOrigin()[1];
    this->frameProperties.Origin[2] = (float) frame->GetOrigin()[2];
    #endif

    this->frameProperties.ScalarType = frame->GetScalarType();
    this->frameProperties.SubVolumeSize[0] = this->frameProperties.Size[0];
    this->frameProperties.SubVolumeSize[1] = this->frameProperties.Size[1];
    this->frameProperties.SubVolumeSize[2] = this->frameProperties.Size[2];

    this->frameProperties.Set = true;
//    }

  //----------------------------------------------------------------------------
  // Create a new IMAGE type message
  imageMessage = igtl::ImageMessage::New();

//  imageMessage->SetOrigin(this->frameProperties.Origin[0], this->frameProperties.Origin[1], this->frameProperties.Origin[2]);
  imageMessage->SetDimensions(this->frameProperties.Size);
  imageMessage->SetSpacing((float) this->frameProperties.Spacing[0],(float) this->frameProperties.Spacing[1], (float) this->frameProperties.Spacing[2]);
  imageMessage->SetScalarType(this->frameProperties.ScalarType);
  imageMessage->SetDeviceName("4D US Volume");
  imageMessage->SetSubVolume(this->frameProperties.SubVolumeSize, this->frameProperties.SubVolumeOffset);
  imageMessage->AllocateScalars();

  //Copy image to output buffer
  unsigned char * pImageMessage = (unsigned char*) imageMessage->GetScalarPointer();
  unsigned char * pFrame = (unsigned char*) frame->GetScalarPointer();

  int scalarComponents = frame->GetNumberOfScalarComponents();

  #ifdef  DEBUGSENDER
    int counter = 0;
    double copyStart = this->GetUpTime();
  #endif

  #ifdef  DEBUGSENDER
    counter = this->frameProperties.Size[0] * this->frameProperties.Size[1] * this->frameProperties.Size[2]; //scalarcomponents
  #endif
//  memcpy(pImageMessage, pFrame, this->frameProperties.Size[0] * this->frameProperties.Size[1] * this->frameProperties.Size[2] * scalarComponents);
  memcpy(pImageMessage, pFrame, this->frameProperties.Size[0] * this->frameProperties.Size[1] * this->frameProperties.Size[2]);

//  for(int i = 0 ; i < this->frameProperties.Size[0] * this->frameProperties.Size[1] * this->frameProperties.Size[2] ; i++ )
//    {
//    *pImageMessage = (unsigned char) *pFrame;
//    ++pFrame; ++pImageMessage;
//    #ifdef  DEBUGSENDER
//      ++counter;
//    #endif
//    }

    igtl::Matrix4x4 igtlMatrix;

    //Copy matrix to output buffer
    #ifdef HIGH_DEFINITION
    igtlMatrix[0][0] = matrix->Element[0][0];
    igtlMatrix[0][1] = matrix->Element[0][1];
    igtlMatrix[0][2] = matrix->Element[0][2];
    igtlMatrix[0][3] = matrix->Element[0][3] / this->TransformationFactorMmToPixel;

    igtlMatrix[1][0] = matrix->Element[1][0];
    igtlMatrix[1][1] = matrix->Element[1][1];
    igtlMatrix[1][2] = matrix->Element[1][2];
    igtlMatrix[1][3] = matrix->Element[1][3] / this->TransformationFactorMmToPixel;

    igtlMatrix[2][0] = matrix->Element[2][0];
    igtlMatrix[2][1] = matrix->Element[2][1];
    igtlMatrix[2][2] = matrix->Element[2][2];
    igtlMatrix[2][3] = matrix->Element[2][3] / this->TransformationFactorMmToPixel;

    igtlMatrix[3][0] = matrix->Element[3][0];
    igtlMatrix[3][1] = matrix->Element[3][1];
    igtlMatrix[3][2] = matrix->Element[3][2];
    igtlMatrix[3][3] = matrix->Element[3][3];
    #else

    igtlMatrix[0][0] = matrix->Element[0][0];
    igtlMatrix[0][1] = matrix->Element[0][1];
    igtlMatrix[0][2] = matrix->Element[0][2];
    igtlMatrix[0][3] = matrix->Element[0][3];

    igtlMatrix[1][0] = matrix->Element[1][0];
    igtlMatrix[1][1] = matrix->Element[1][1];
    igtlMatrix[1][2] = matrix->Element[1][2];
    igtlMatrix[1][3] = matrix->Element[1][3];

    igtlMatrix[2][0] = matrix->Element[2][0];
    igtlMatrix[2][1] = matrix->Element[2][1];
    igtlMatrix[2][2] = matrix->Element[2][2];
    igtlMatrix[2][3] = matrix->Element[2][3];

    igtlMatrix[3][0] = matrix->Element[3][0];
    igtlMatrix[3][1] = matrix->Element[3][1];
    igtlMatrix[3][2] = matrix->Element[3][2];
    igtlMatrix[3][3] = matrix->Element[3][3];
    #endif

    this->ReleaseLock(DATASENDER);

  //  for(int i = 0; i < 4; ++i)
  //    {
  //    for(int j = 0; j < 4; ++j)
  //      {
  //      igtlMatrix[i][j] = matrix->Element[i][j];
  //      }
  //    }

//  #ifdef  DEBUGSENDER
//    this->LogStream <<  this->GetUpTime() << " |S-INFO: OpenIGTLink image message matrix" << endl;
//    matrix->Print(this->LogStream);
//  #endif

    imageMessage->SetMatrix(igtlMatrix);

  #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Size of image frame to send:"
                    << this->frameProperties.Size[0] << " | "
                    << this->frameProperties.Size[1] << " | "
                    << this->frameProperties.Size[2] << endl
                    << "           | Spacing: "
                    << this->frameProperties.Spacing[0] << " | "
                    << this->frameProperties.Spacing[1] << " | "
                    << this->frameProperties.Spacing[2] << endl
                    << "           | Origin: "
                    << this->frameProperties.Origin[0]<< "|"
                    << this->frameProperties.Origin[1]<< "|"
                    << this->frameProperties.Origin[2]<< endl
                    << "           | Copied Pixels: "<< counter << " | Copy time: " << this->GetUpTime() - copyStart <<endl
                    << "           | Index: " << index << " | "  <<endl;
  #endif
  imageMessage->Pack();// Pack (serialize)

    //Statistics----------------------------------------------------------------
    this->Statistics.volumeCounter++;
    this->Statistics.meanVolumeSize[0] += this->frameProperties.Size[0];
    this->Statistics.meanVolumeSize[1] += this->frameProperties.Size[1];
    this->Statistics.meanVolumeSize[2] += this->frameProperties.Size[2];

    double volumeSize = this->frameProperties.Size[0] * this->frameProperties.Size[1] * this->frameProperties.Size[2];

    if(volumeSize > this->Statistics.maxVolumeSize[0] * this->Statistics.maxVolumeSize[1] * this->Statistics.maxVolumeSize[2])
      {
      this->Statistics.maxVolumeSize[0] = this->frameProperties.Size[0];
      this->Statistics.maxVolumeSize[1] = this->frameProperties.Size[1];
      this->Statistics.maxVolumeSize[2] = this->frameProperties.Size[2];
      }

    if(volumeSize < this->Statistics.minVolumeSize[0] * this->Statistics.minVolumeSize[1] * this->Statistics.minVolumeSize[2])
      {
      this->Statistics.minVolumeSize[0] = this->frameProperties.Size[0];
      this->Statistics.minVolumeSize[1] = this->frameProperties.Size[1];
      this->Statistics.minVolumeSize[2] = this->frameProperties.Size[2];
      }

    #ifdef  DEBUGSENDER
      this->LogStream <<  this->GetUpTime() << " |S-INFO: Statistics" << endl
                                   << "           |        Mean Volume Dimensions: " << this->Statistics.meanVolumeSize[0] / this->Statistics.volumeCounter << " | " << this->Statistics.meanVolumeSize[1] / this->Statistics.volumeCounter << " | " << this->Statistics.meanVolumeSize[2] / this->Statistics.volumeCounter << endl
                                   << "           |        Max Volume Dimensions: " << this->Statistics.maxVolumeSize[0] << " | " << this->Statistics.maxVolumeSize[1] << " | " << this->Statistics.maxVolumeSize[2] << endl
                                   << "           |        Min Volume Dimensions: " << this->Statistics.minVolumeSize[0] << " | " << this->Statistics.minVolumeSize[1] << " | " << this->Statistics.minVolumeSize[2] << endl;
    #endif

  return 0;

}

//------------------------------------------------------------
int vtkDataSender::SendMessage(igtl::ImageMessage::Pointer& message)
{
    //Send message
    double sendTime = this->GetUpTime();
    int ret = this->socket->Send(message->GetPackPointer(), message->GetPackSize());
    if (ret == 0)
      {
      cerr << "ERROR: Connection to OpenIGTLink Server lost while sending!" <<endl;
      this->StopSending();
      return -1;
      }
    else
      {
      #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Message successfully send to OpenIGTLink Server "<< endl
                        << "         | Send time: " << this->GetUpTime() - sendTime << "| "  << endl;
      #endif
      if(Verbose)
        {
        this->UpdateFrameRate(this->GetUpTime());
        }
      }

  this->FrameCounter++;

  return 0;
}

//------------------------------------------------------------
int vtkDataSender::NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix)
{
  //Check if new data buffer is full
  if(this->IsSendDataBufferFull())
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Senders new data buffer is full" << endl;
    #endif
    return -1;
    }

  int index = this->IncrementBufferIndex(1);

  //Add new data to Buffer
  if(-1 == this->AddDatatoBuffer(index, frame, trackerMatrix))
    {
    this->IncrementBufferIndex(-1);
    return -1;
    }


  return index;
}

//------------------------------------------------------------
int vtkDataSender::AddDatatoBuffer(int index, vtkImageData* imageData, vtkMatrix4x4* matrix)
{
  if(!this->IsIndexAvailable(index))
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Send Data Buffer already has data at index: " << index << endl
                                     << "        |        Available Indices are: ";
      this->PrintAvailableIndices();
      this->LogStream << endl;
    #endif
    return -1;
    }

  //Create and fill new send data object
  struct SenderDataStruct newSendData;

  newSendData.ImageData = imageData;
  newSendData.Matrix = matrix;
  newSendData.SenderLock = 1;
  newSendData.ProcessorLock = 1;

  //Add Object to buffer
  this->sendDataBuffer[index] = newSendData;
  this->sendDataQueue.push(index);//Add Index to new data buffer

  return 0;
}

//------------------------------------------------------------
int vtkDataSender::TryToDeleteData(int index)
{
  if(this->IsIndexAvailable(index) || index < 0 || index >= this->SendDataBufferSize)
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Not data to delete in Send Data Buffer at index: " << index << endl;
    #endif
    return -1;
    }

  if(this->sendDataBuffer[index].SenderLock <= 0 && this->sendDataBuffer[index].ProcessorLock <= 0)
    {

    //Delete Image Frame
    if(this->sendDataBuffer[index].ImageData)
      {
      this->sendDataBuffer[index].ImageData->Delete();
      this->sendDataBuffer[index].ImageData = NULL;
      }
    else
      {
      #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Not image to delete in Send Data Buffer at index: " << index << endl;
      #endif
      }

    //Delete Matrix
    if(NULL != this->sendDataBuffer[index].Matrix)
      {
      this->sendDataBuffer[index].Matrix->Delete();
      this->sendDataBuffer[index].Matrix = NULL;
      }
    else
      {
      #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Not matrix to delete in Send Data Buffer at index: " << index << endl;
      #endif
      }

    //Free Index
    if(1 != this->sendDataBuffer.erase(index))
      {
      assert(true);
      }

    #ifdef  DEBUGSENDER
      this->LogStream <<  this->GetUpTime() << " |S-INFO: Deleted data in Send Data Buffer at index: " << index << endl
                                     << "        |        Available Indices are: ";
      this->PrintAvailableIndices();
      this->LogStream << endl;
    #endif
    }
  else
    {
    #ifdef  DEBUGSENDER
      this->LogStream <<  this->GetUpTime() << " |S-WARNING: Could not delete data at index:" << index << " locked by: ";

      if( this->sendDataBuffer[index].SenderLock > 0)
        {
        this->LogStream << "DataSender ";
        }

      if(this->sendDataBuffer[index].ProcessorLock > 0)
        {
        this->LogStream << "DataProcessor";
        }

      this->LogStream << endl;
    #endif
    }

  return 0;
}

//------------------------------------------------------------
vtkImageData* vtkDataSender::GetVolume(int index)
{
if(this->IsIndexAvailable(index) || index < 0 || index >= this->SendDataBufferSize)
   {
   #ifdef  ERRORSENDER
     this->LogStream <<  this->GetUpTime() << " |S-ERROR: Send Data Buffer has no data at index: " << index << endl;
   #endif
   return NULL;
   }

  vtkImageData * volume = this->sendDataBuffer[index].ImageData;

  return volume;
}

//------------------------------------------------------------
int vtkDataSender::UnlockData(int index, int lock)
{
  if(this->IsIndexAvailable(index) || index < 0 || index >= this->SendDataBufferSize)
    {
    #ifdef  ERRORSENDER
    this->LogStream <<  this->GetUpTime() << " |S-ERROR: Send Data Buffer has no data at index: " << index << endl;
    #endif
    return -1;
    }

  if(lock == DATASENDER)
    {
    this->sendDataBuffer[index].SenderLock--;
    #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Data Sender released data lock" << endl;
    #endif
    }
  else if(lock == DATAPROCESSOR)
    {
    this->sendDataBuffer[index].ProcessorLock--;
    #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Data Processor released data lock" << endl;
    #endif
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Try to release unknown lock" << endl;
    #endif

    return -1;
    }

  return 0;
}

/******************************************************************************
 * double vtkDataSender::GetUpTime()
 *
 *  Returns elapsed Time since program start
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 * ****************************************************************************/
double vtkDataSender::GetUpTime()
{
  return vtkTimerLog::GetUniversalTime() - this->GetStartUpTime();
}

/******************************************************************************
 * bool vtkDataSender::IsIndexAvailable(int index)
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
bool vtkDataSender::IsIndexAvailable(int index)
{
  if(this->sendDataBuffer.find(index) == this->sendDataBuffer.end() && index < this->SendDataBufferSize && index >= 0)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/******************************************************************************
 * void vtkDataSender::SetLogStream(ofstream &LogStream)
 *
 *  Redirects Logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 * ****************************************************************************/
void vtkDataSender::SetLogStream(ofstream &LogStream)
{
  this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
  this->LogStream.precision(6);
  this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
 * ofstream& vtkDataSender::GetLogStream()
 *
 *  Returns logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Return: Logstream
 *
 * ****************************************************************************/
ofstream& vtkDataSender::GetLogStream()
{
  return this->LogStream;
}

/******************************************************************************
 * void UpdateFrameRate(double sendTime)
 *
 *  Calculates and prints current frame rate to console
 *
 *  @Author:Jan Gumprecht
 *  @Date:  5.February 2009
 *
 *  @Param: double sentTime - send time of last message
 *
 * ****************************************************************************/
void vtkDataSender::UpdateFrameRate(double sendTime)
{
  this->UpDateCounter++;

  if(this->lastFrameRateUpdate + 1 <= this->GetUpTime())
    {
    if(this->lastFrameRateUpdate != 0)
      {
      float fps = this->UpDateCounter / (sendTime - this->lastFrameRateUpdate);
      cout << "\b\b\b\b\b" << std::flush;
      cout << setw(5) << setprecision(2) << fps;
      cout << "\a" <<std::flush;

      this->UpDateCounter = 0;
      this->Statistics.fpsCounter++;
      this->Statistics.meanFPS += fps;
      if(fps > this->Statistics.maxFPS)
        {
        this->Statistics.maxFPS = fps;
        }
      if(fps < this->Statistics.minFPS)
        {
        this->Statistics.minFPS = fps;
        }
      #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Current frame rate " << fps << endl
                                    << "           |        Mean FPS: " << this->Statistics.meanFPS / this->Statistics.fpsCounter << " Max FPS: "<< this->Statistics.maxFPS << " Min FPS: "<< this->Statistics.minFPS << endl;

      #endif
      }
    else
      {
      cout << "Frame rate:       fps\b\b\b\b" <<std::flush;
      }
    this->lastFrameRateUpdate = sendTime;
    }
}

/******************************************************************************
 * int vtkDataSender::LockIndex(int index, int requester)
 *
 *  Lock index "index"
 *
 *  @Author:Jan Gumprecht
 *  @Date:  11.February 2009
 *
 *  @Param: int index - index to lock
 *  @Param: int requester - Thread who wants to lock the index
 *
 *  @Param: 0 on success
 *         -1 on failure
 *
 * ****************************************************************************/
int vtkDataSender::LockIndex(int index, int requester)
{
  int retVal = 0;
  this->IndexLock->Lock();
  if(requester == DATASENDER)
    {
    if(index == this->IndexLockedByDataProcessor)
       {
       retVal = -1;
       }
    else
      {
      #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Sender locked index:" << index << endl;
      #endif
      this->IndexLockedByDataSender = index;
      }
    }
  else if(requester == DATAPROCESSOR)
    {
    if(index == this->IndexLockedByDataSender)
       {
       retVal = -1;
       }
    else
      {
      #ifdef DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Processor locked index:" << index << endl;
      #endif
      this->IndexLockedByDataProcessor = index;
      }
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Unknown requester("<< requester <<") tries to lock index:" << index << endl;
    #endif
    retVal = -1;
    }
  this->IndexLock->Unlock();

  return retVal;
}

/******************************************************************************
 * int vtkDataSender::ReleaseLock(int requester)
 *
 *  Release Lock
 *
 *  @Author:Jan Gumprecht
 *  @Date:  11.February 2009
 *
 *  @Param: int index - index to lock
 *  @Param: int requester - Thread who wants to lock the index
 *
 *  @Param: 0 on success
 *         -1 on failure
 *
 * ****************************************************************************/
int vtkDataSender::ReleaseLock(int requester)
{
  int retVal = 0;
  this->IndexLock->Lock();
  if(requester == DATASENDER)
    {
    #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Sender released index:" << this->IndexLockedByDataSender << endl;
    #endif
    this->IndexLockedByDataSender = -1;
    }
  else if (requester == DATAPROCESSOR)
    {
    #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Processor released index:" << this->IndexLockedByDataProcessor << endl;
    #endif
    this->IndexLockedByDataProcessor = -1;
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Unknown requester("<< requester <<") tries to release a lock "<< endl;
    #endif
    retVal = -1;
    }
  this->IndexLock->Unlock();

  return retVal;
}

/******************************************************************************
 * void vtkDataSender::PrintAvailableIndices()
 *
 *  @Author:Jan Gumprecht
 *  @Date:  29.July 2009
 *
 *
 * ****************************************************************************/
void vtkDataSender::PrintAvailableIndices()
{
  for (int i = 0 ; i < this->GetSendDataBufferSize() ; ++i)
    {
    if(this->IsIndexAvailable(i))
      {
      this->LogStream << i;
      if(i < this->GetSendDataBufferSize() -1 )
        {
        this->LogStream << ", ";
        }
      else
        {
        this->LogStream << " ";
        }
      }
    }
}
