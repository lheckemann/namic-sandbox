/*=========================================================================

Module:    $RCSfile: vtkDataSender.cxx,v $
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
#undef REMOVE_ALPHA_CHANNEL
 
//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"

#include "vtkObjectFactory.h"
#include "vtkDataSender.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkVideoSourceSimulator.h"

#include "igtlImageMessage.h"
#include "igtlMath.h"
#include "igtlOSUtil.h"

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625

using namespace std;

vtkCxxRevisionMacro(vtkDataSender, "$Revision$");
vtkStandardNewMacro(vtkDataSender);

//----------------------------------------------------------------------------
vtkDataSender::vtkDataSender()
{
  this->ServerPort = 18944;
  this->OIGTLServer = NULL; 
  this->SetOIGTLServer("localhost");
  this->SendRate = 30 * 1.5;

  this->socket = NULL;
  this->socket = igtl::ClientSocket::New();

  this->Verbose = false;
  
  this->Connected = false;
  this->Sending = false;
  
  this->PlayerThreader = vtkMultiThreader::New();;
  this->PlayerThreadId = -1;
  
  this->newDataBufferSize = 100;
  this->newDataBufferIndex = -1;
  
  frameProperties.Set = false;
  frameProperties.SubVolumeOffset[0] = 0;
  frameProperties.SubVolumeOffset[1] = 0;
  frameProperties.SubVolumeOffset[2] = 0;
}

//----------------------------------------------------------------------------
vtkDataSender::~vtkDataSender()
{
  this->SetOIGTLServer(NULL);
  this->PlayerThreader->Delete();
  
}


//----------------------------------------------------------------------------
void vtkDataSender::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
int vtkDataSender::ConnectToServer()
{
  // Opening an OpenIGTLink socket
  int e = this->socket->ConnectToServer(OIGTLServer, ServerPort);

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
  
  double startTime = vtkTimerLog::GetUniversalTime();
  double rate = self->GetSendRate();
  int counter = 0;
  int currentIndex = -1;
  igtl::ImageMessage::Pointer imageMessage = igtl::ImageMessage::New();
  int errors = 0;
  
  do
    {
    if(!self->IsNewDataBufferEmpty())
      {//New data available
      
      //JG 09/01/21
      //cout << "New Data in Send Buffer arrived" <<endl;
      
      currentIndex = self->GetHeadOfNewDataBuffer();
      
      //Prepare new data
      imageMessage = self->PrepareImageMessage(currentIndex);
      
      //Send new data
      errors += self->SendMessage(imageMessage);
      
      //Delete sended data (Remove Index from new data buffer + free maps from data
      errors += self->DeleteData(currentIndex);
      
      if(errors <= -10)
        {
        cerr << "ERROR: Too many errors occured while sending" << endl;
        break;
        }    
      }
    
    //Increase counter for sleep time            
    counter++;
    
    Warten ist unnoetig, falls Daten vorhanden
    
    } 
  while(vtkThreadSleep(data, startTime + counter/rate));
  
  return NULL;
}
//----------------------------------------------------------------------------
bool vtkDataSender::IsNewDataBufferEmpty()
{
  return this->newDataBuffer.empty();
}

bool vtkDataSender::IsNewDataBufferFull()
{
  if(this->newDataBuffer.size() >= this->newDataBufferSize)
    {           
    return true;
    }
  else
    {
    return false;
    } 
}

//----------------------------------------------------------------------------
int vtkDataSender::GetHeadOfNewDataBuffer()
{ 
  int head = this->newDataBuffer.front();
  this->newDataBuffer.pop();
  return head;
}

//----------------------------------------------------------------------------
int vtkDataSender::IncrementBufferIndex(int increment)
{
  this->newDataBufferIndex = (this->newDataBufferIndex + increment) % this->newDataBufferSize;
        
  return this->newDataBufferIndex;
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
    cerr << "ERROR: Data collector already collects data" << endl;
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
      cout << "Successfully started to send" << endl;
      }
    return 0;
    }
  else
    {
    cerr << "ERROR: Could not start sender thread" << endl;
    return -1;
    }  
}

//------------------------------------------------------------
int vtkDataSender::StopSending()
{
  //Stop send thread  
  if (this->Sending)
    {    
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Sending = false;
    
    if(Verbose)
      {
      cout << "Stop sending" << endl;
      }
    }
  
  return 0;
        
}

//------------------------------------------------------------
igtl::ImageMessage::Pointer vtkDataSender::PrepareImageMessage(int index)
{ 
  vtkImageData * frame = this->newImageMap[index];
  
  if(frame == NULL)
    {
    cerr << "ERROR: no image to prepare at index: " << index << endl;
    return NULL;
    }
  
  vtkMatrix4x4 * matrix = this->newTrackerMatrixMap[index];
  
  if(matrix == NULL)
    {
    cerr << "ERROR: no matrix to prepare at index: " << index << endl;
    return NULL;
    }
  
  if(!frameProperties.Set)//Only neccessary once
    {
    //Get property of frame
    frame->GetDimensions(frameProperties.Size);
    frame->GetSpacing(frameProperties.Spacing);               
    frameProperties.ScalarType = frame->GetScalarType();    
    frameProperties.SubVolumeSize[0] = frameProperties.Size[0];
    frameProperties.SubVolumeSize[1] = frameProperties.Size[1];
    frameProperties.SubVolumeSize[2] = frameProperties.Size[2];
    frameProperties.Set = true;     
    }
  
  //------------------------------------------------------------
  // Create a new IMAGE type message
  igtl::ImageMessage::Pointer imageMessage = igtl::ImageMessage::New();
   
  imageMessage->SetDimensions(frameProperties.Size);
  imageMessage->SetSpacing((float) frameProperties.Spacing[0],(float) frameProperties.Spacing[1], (float) frameProperties.Spacing[2]);
  imageMessage->SetScalarType(frameProperties.ScalarType);
  imageMessage->SetDeviceName("4D Ultrasound Data Sender");
  imageMessage->SetSubVolume(frameProperties.SubVolumeSize, frameProperties.SubVolumeOffset);
  imageMessage->AllocateScalars();
  
  if(Verbose)
    {
    cout << "Size of image frame to send:" << frameProperties.Size[0] << " | " << frameProperties.Size[1] << " | " << frameProperties.Size[2] << endl;
    }

  //Copy image to output buffer
  unsigned char * pImageMessage = (unsigned char*) imageMessage->GetScalarPointer();
  unsigned char * pFrame = (unsigned char*) frame->GetScalarPointer();

  for(int i = 0 ; i < frameProperties.Size[0] * frameProperties.Size[1] * frameProperties.Size[2] ; i++ )
    {
    *pImageMessage = (unsigned char) *pFrame;
    pFrame++; pImageMessage++;
    }
  
  igtl::Matrix4x4 igtlMatrix;
  
  //Copy matrix to output buffer
  for(int i = 0; i < 4; ++i)
    {
    for(int j = 0; j < 4; ++j)
      {
      igtlMatrix[i][j] = matrix->Element[i][j];      
      }
    } 
  
  if(Verbose)
    {
    cout << "OpenIGTLink image message matrix" << endl;
    igtl::PrintMatrix(igtlMatrix); 
    }
  
  imageMessage->SetMatrix(igtlMatrix);
  
  imageMessage->Pack();// Pack (serialize)

  return imageMessage;
    
}

//------------------------------------------------------------
int vtkDataSender::SendMessage(igtl::ImageMessage::Pointer message)
{    
    //Send message    
    int ret = this->socket->Send(message->GetPackPointer(), message->GetPackSize());
    if (ret == 0)    
      {
      std::cerr << "ERROR: Connection to OpenIGTLink Server lost while sending!\n";
      return -1;
      }
    else
      {
      if(Verbose)
        {
        cout << "Message successfully send to OpenIGTLink Server" << endl;
        }
      }     
  return 0;
}

//------------------------------------------------------------
int vtkDataSender::NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix)
{
  //Check if new data buffer is full
  if(this->IsNewDataBufferFull())
    {
    cerr << "ERROR: Senders new data buffer is full" << endl;
    return -1;
    }  
          
  int index = this->IncrementBufferIndex(1);
        
  //Add new data to Buffer    
  this->AddFrameToFrameMap(index, frame);//Imageframe  
  this->AddMatrixToMatrixMap(index, trackerMatrix);//TrackerMatrix
          
  this->newDataBuffer.push(index);//Add Index to new data buffer
  
  return 0;     
}

//------------------------------------------------------------
int vtkDataSender::DeleteData(int index)
{
  if(this->newImageMap[index] == NULL)
    {
    cerr << "ERROR: new image map has no image for index: " << index << endl;
    return -1;
    }
  
  if(this->newTrackerMatrixMap[index] == NULL)
    {
    cerr << "ERROR: new tracker matrix map has no matrix for index: " << index << endl;
    return -1;
    }
    
  this->newImageMap.erase(index);
  this->newTrackerMatrixMap.erase(index);
  
  return 0;
}

//------------------------------------------------------------
int vtkDataSender::AddFrameToFrameMap(int index, vtkImageData* frame)
{
  if(this->newImageMap[index] != NULL)
    {
    cerr << "ERROR: new image map already has an image for index: " << index << endl;
    return -1;
    }
    
  this->newImageMap[index] = frame;
  return 0;
}

//------------------------------------------------------------
int vtkDataSender::AddMatrixToMatrixMap(int index, vtkMatrix4x4* matrix)
{
  if(this->newTrackerMatrixMap[index] != NULL)
    {
    cerr << "ERROR: new tracker matrix map already has a matrix for index: " << index << endl;
    return -1;
    }
    
  this->newTrackerMatrixMap[index] = matrix;
  return 0;
}

//------------------------------------------------------------
// Function to get image data
int vtkDataSender::vtkFillImageMessage(vtkImageData *ImageBuffer, igtl::ImageMessage::Pointer& msg)
{
  int size[3];      // image dimension
  double spacing[3]; // spacing (mm/pixel)
  int svsize[3];    // sub-volume size
  int svoffset[3];  // sub-volume offset
  int scalarType;   // scalar type

  //If we reconstructing a 3DVolume everything is already set for us
  ImageBuffer->GetDimensions(size);
  ImageBuffer->GetSpacing(spacing);
  //spacing[2] *= 1;

  //For a fast Computer enable this
  //spacing[0] *= 3;
  //spacing[1] *= 3;
  //spacing[2] *= 3;
    
  svsize[0]   = size[0];       
  svsize[1]   = size[1];       
  svsize[2]   = size[2];           
  svoffset[0] = svoffset[1] = svoffset[2] = 0;           
  scalarType = ImageBuffer->GetScalarType();    
  
  //------------------------------------------------------------
  // Create a new IMAGE type message    
  msg->SetDimensions(size);
  msg->SetSpacing((float) spacing[0],(float) spacing[1], (float) spacing[2]);
  msg->SetScalarType(scalarType);
  msg->SetDeviceName("SynchroGrab");
  msg->SetSubVolume(svsize, svoffset);
  msg->AllocateScalars();
  
  if(Verbose)
    {
    cerr <<    "3D Volume Size:" << size[0] << " " << size[1] << " " << size[2] << endl;
    }

  unsigned char * p_msg = (unsigned char*) msg->GetScalarPointer();
  unsigned char * p_ibuffer = (unsigned char*) ImageBuffer->GetScalarPointer();

  for(int i=0 ; i < size[0] * size[1] * size[2] ; i++ )
    {
    *p_msg = (unsigned char) * p_ibuffer;
    p_ibuffer++; p_msg++;
    }

  return 1;
}

//------------------------------------------------------------
// Function to generate random matrix.
void vtkDataSender::vtkGetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];  
  
 #ifdef USE_TRACKER_DEVICE
 //NDI tracker matrix looks like
  //  0  0 -1 90
  //  0  1  0  0
  //  1  0  0  0
  //  0  0  0  1  
  matrix[0][0] =   0.0;  matrix[1][0] =  0.0;  matrix[2][0] =  1.0; matrix[3][0] = 0.0;
  matrix[0][1] =   0.0;  matrix[1][1] =  1.0;  matrix[2][1] =  0.0; matrix[3][1] = 0.0;
  matrix[0][2] =  -1.0;  matrix[1][2] =  0.0;  matrix[2][2] =  0.0; matrix[3][2] = 0.0;
  matrix[0][3] =  90.0;  matrix[1][3] =  0.0;  matrix[2][3] =  0.0; matrix[3][3] = 1.0;
#else
  //Tracker simulator matrix looks like
  // -1  0  0  0
  //  0  0  1  0
  //  0  1  0  0
  //  0  0  0  1
  matrix[0][0] =  -1.0;  matrix[1][0] =  0.0;  matrix[2][0] =  0.0; matrix[3][0] = 0.0;
  matrix[0][1] =   0.0;  matrix[1][1] =  0.0;  matrix[2][1] =  1.0; matrix[3][1] = 0.0;
  matrix[0][2] =   0.0;  matrix[1][2] =  1.0;  matrix[2][2] =  0.0; matrix[3][2] = 0.0;
  matrix[0][3] =   0.0;  matrix[1][3] =  0.0;  matrix[2][3] =  0.0; matrix[3][3] = 1.0;
#endif

  if(Verbose)
    {
    cout << "OpenIGTLink image message matrix" << endl;
    igtl::PrintMatrix(matrix); 
    }
  
}
