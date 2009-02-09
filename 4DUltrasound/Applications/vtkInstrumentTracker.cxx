/*========================================================================

Module:  $RCSfile: vtkDataProcessor.cxx,v $
Authors: Jan Gumprecht Harvard Medical School
Copyright (c) 2009, Brigham and Women's Hospital, Boston, MA
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
#include <string>

#include "vtkInstrumentTracker.h"

#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"
#include "vtkObjectFactory.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkUltrasoundCalibFileReader.h"

#include "igtlMath.h"
#include "igtlOSUtil.h"

#ifdef USE_TRACKER_DEVICE
#include "vtkNDITracker.h"
#else
#include "vtkTrackerSimulator.h"
#endif

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625
#define US_IMAGE_FAN_RATIO 0.6 //Amount of ultrasound images's height that
                               //is used by the ultrasound fan

using namespace std;

vtkCxxRevisionMacro(vtkInstrumentTracker, "$Revision$");
vtkStandardNewMacro(vtkInstrumentTracker);

/******************************************************************************
 * vtkDataProcessor()
 *
 *  Constructor
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 * ****************************************************************************/
vtkInstrumentTracker::vtkInstrumentTracker()
{
  this->Enabled = false; //If false Instrument Tracker does nothing

  this->ServerPort = 18945;
  this->OIGTLServer = NULL;
  this->SetOIGTLServer("localhost");
  this->TrackingRate = 30;
  this->FrameBufferSize = 100;
  this->UltraSoundScanDepth = 70; //Unit: mm

  this->socket = NULL;
  this->socket = igtl::ClientSocket::New();

  this->Verbose = false;
  this->CalibrationFileName = NULL;

  this->Connected = false;
  this->Tracking = false;

  #ifdef USE_TRACKER_DEVICE
  this->tracker = vtkNDITracker::New();
  #else
  this->tracker = vtkTrackerSimulator::New();
  #endif //USE_TRACKER_DEVICE

  this->TrackerTool = this->tracker->GetTool(0);

  this->PlayerThreader = vtkMultiThreader::New();;
  this->PlayerThreadId = -1;

  this->StartUpTime = vtkTimerLog::GetUniversalTime();
  this->LogStream.ostream::rdbuf(cerr.rdbuf());
}

/******************************************************************************
 * vtkInstrumentTracker::~vtkInstrumentTracker()
 *
 *  Destructor
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 * ****************************************************************************/
vtkInstrumentTracker::~vtkInstrumentTracker()
{
  this->tracker->Delete();
  this->PlayerThreader->Delete();

  if(this->Tracking)
    {
    this->StopTracking();
    }

  if(this->Connected)
    {
    this->CloseServerConnection();
    }

}

/******************************************************************************
 * void vtkInstrumentTracker::PrintSelf(ostream& os, vtkIndent indent)
 *
 *  Print information about the instance
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 * ****************************************************************************/
void vtkInstrumentTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

/******************************************************************************
 * int vtkInstrumentTracker::ConnectToServer()
 *
 *  Connect to OpenIGTLink Server
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 *  @Return:  0 on success
 *           -1 on failure
 *
 * ****************************************************************************/
int vtkInstrumentTracker::ConnectToServer()
{
  if(!this->Enabled)
    {
    #ifdef ERROR_INST_TRACKER
    this->LogStream << this->GetUpTime() << " |I-ERROR: Instrument tracker not enabled can not connect to server" << endl;
    #endif
    return -1;
    }

  // Opening an OpenIGTLink socket
  int e = this->socket->ConnectToServer(OIGTLServer, ServerPort);

  if(e != 0)
    {
    cout << " ERROR: Failed to connect the OpenIGTLink socket to the server ("<< OIGTLServer <<":"<<  ServerPort << ") Error code : " << e << endl;
    return -1;
    }

  if(Verbose)
    {
    cout << "Successful connected to the OpenIGTLink server ("<< OIGTLServer <<":"<<  ServerPort << ")" << endl;
    }

  this->Connected = true;

  return 0;
}

/******************************************************************************
 * int vtkInstrumentTracker::CloseServerConnection()
 *
 *  Close Connection to OpenIGTLink Server
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 *  @Return:  0
 *
 * ****************************************************************************/
int vtkInstrumentTracker::CloseServerConnection()
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
        if (i == 0)
          {
          vtkGenericWarningMacro("Dropped a instrument tracking matrix frame.");
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
 *  static void *vtkInstrumentTrackerThread(vtkMultiThreader::ThreadInfo *data)
 *
 *  This function runs in an alternate thread to asyncronously collect data
 *
 *  @Author:Jan Gumprecht
 *  @Date:  19.Januar 2009
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data
 *
 * ****************************************************************************/
static void *vtkInstrumentTrackerThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkInstrumentTracker *self = (vtkInstrumentTracker *)(data->UserData);

  double startTime = vtkTimerLog::GetUniversalTime();
  double rate = self->GetTrackingRate();
  int frame = 0;
  double sectionTime;
  double loopTime;
  int errors = 0;

  vtkMatrix4x4 *trackerMatrix = vtkMatrix4x4::New();

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName("Instrument Tracker");

  do
    {
    //Grab new frame
    loopTime = self->GetUpTime();
    #ifdef TIMING_INST_TRACKER
    self->GetLogStream() << self->GetUpTime() << " |---------------------------------------" << endl;
    self->GetLogStream() << self->GetUpTime() << " |S-INFO: Sender Found new Data at Index: " << currentIndex
                                              << " | L:" << self->GetUpTime() - loopTime << endl;
    #endif

    igtl::Matrix4x4 igtlMatrix;

    #ifdef USE_TRACKER_DEVICE
    //Get Tracking Matrix for new frame
    this->TrackerTool->GetBuffer()->GetFlagsAndMatrixFromTime(this->ToolMatrix, transformTime);
    self->AdjustMatrix(*trackerMatrix);// Adjust tracker matrix to ultrasound scan depth

    //Copy matrix
    igtlMatrix[0][0] = trackerMatrix->Element[0][0];
    igtlMatrix[0][1] = trackerMatrix->Element[0][1];
    igtlMatrix[0][2] = trackerMatrix->Element[0][2];
    igtlMatrix[0][3] = trackerMatrix->Element[0][3];

    igtlMatrix[1][0] = trackerMatrix->Element[1][0];
    igtlMatrix[1][1] = trackerMatrix->Element[1][1];
    igtlMatrix[1][2] = trackerMatrix->Element[1][2];
    igtlMatrix[1][3] = trackerMatrix->Element[1][3];

    igtlMatrix[2][0] = trackerMatrix->Element[2][0];
    igtlMatrix[2][1] = trackerMatrix->Element[2][1];
    igtlMatrix[2][2] = trackerMatrix->Element[2][2];
    igtlMatrix[2][3] = trackerMatrix->Element[2][3];

    igtlMatrix[3][0] = trackerMatrix->Element[3][0];
    igtlMatrix[3][1] = trackerMatrix->Element[3][1];
    igtlMatrix[3][2] = trackerMatrix->Element[3][2];
    igtlMatrix[3][3] = trackerMatrix->Element[3][3];

    #else
    self->GetRandomTestMatrix(igtlMatrix);
    #endif

    #ifdef DEBUG_MATRICES
    self->GetLogStream() << self->GetUpTime() << " |I-INFO Tracker matrix:" << endl;
    igtl::PrintMatrix(igtlMatrix);
    #endif

    transMsg->SetMatrix(igtlMatrix);
    transMsg->Pack();

    errors += self->SendMessage(transMsg);

      #ifdef TIMING_INST_TRACKER
      self->GetLogStream() << self->GetUpTime() << " |I-INFO: Message Send"
                                                << " | S: " << self->GetUpTime() - sectionTime
                                                << " | L:" << self->GetUpTime() - loopTime
                                                << " | FPS: " << 1 / (self->GetUpTime() - loopTime) << endl;
      #endif

      if(errors <= - ERRORTOLERANCE)
        {
        #ifdef  ERROR_INST_TRACKER
          self->GetLogStream() << self->GetUpTime() <<" |I-ERROR: Stopped sending too many errors occured" << endl;
        #endif
        break;
        }

    frame++;
    }
  while(vtkThreadSleep(data, startTime + frame/rate));

return NULL;
}

//----------------------------------------------------------------------------
//StartTracking
int vtkInstrumentTracker::StartTracking()
{
  if(!this->Enabled)
    {
    #ifdef ERROR_INST_TRACKER
    this->LogStream << this->GetUpTime() << " |I-ERROR: Instrument tracker not enabled can not start tracking" << endl;
    #endif
    return -1;
    }

  if(!this->Tracking)
    {
    this->Tracking = true;
    }
  else
    {
    #ifdef ERROR_INST_TRACKER
    this->LogStream << this->GetUpTime() << " |I-ERROR: Instrument tracker already collects data" << endl;
    #endif
    return false;
    }

  if(this->tracker->Probe() != 1)
   {
   #ifdef ERROR_INST_TRACKER
   this->LogStream << "I-ERROR: Tracking system not found" << endl;
   #endif
   return -1;
   }

  if(this->calibReader != NULL)
    {
    this->calibReader->SetFileName(this->CalibrationFileName);
    this->calibReader->ReadCalibFile();
    }
  else
    {
    #ifdef ERROR_INST_TRACKER
    this->LogStream << "I-ERROR: Calibration file reader not specified" << endl;
    #endif
    return -1;
    }

  this->TrackerTool->GetBuffer()->SetBufferSize((int) (this->FrameBufferSize * CERTUS_UPDATE_RATE / this->TrackingRate * FUDGE_FACTOR + 0.5) );
  this->TrackerTool->SetCalibrationMatrix(this->calibReader->GetCalibrationMatrix());

  this->tracker->StartTracking();

 //Start Thread
   this->PlayerThreadId =
             this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
             &vtkInstrumentTrackerThread, this);

   if(this->PlayerThreadId == -1)
      {
      #ifdef ERROR_INST_TRACKER
      this->LogStream << this->GetUpTime() << " |I-ERROR: Collector thread could not be started" << endl;
      #endif
      return -1;
      }

  #ifdef DEBUG_INST_TRACKER
  this->LogStream << "I-INFO: Instrument Tracker Thread started" << endl;
  #endif

 return 0;

}
//----------------------------------------------------------------------------
//StopTracking
int vtkInstrumentTracker::StopTracking()
{
  if (this->Tracking)
    {
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Tracking = false;

    #ifdef DEBUG_INST_TRACKER
    this->LogStream << "I-INFO: Instrument Tracker Thread stop" << endl;
    #endif
    }

  return 0;
}

//------------------------------------------------------------
int vtkInstrumentTracker::SendMessage(igtl::TransformMessage::Pointer& message)
{
    //Send message
    if (socket->Send(message->GetPackPointer(), message->GetPackSize()))
      {
      cerr << "ERROR: Connection to OpenIGTLink Server lost while sending!" <<endl;
      return -1;
      }
    else
      {
      #ifdef  DEBUG_INST_TRACKER
        this->LogStream <<  this->GetUpTime() << " |I-INFO: Message successfully send to OpenIGTLink Server "<< endl
      #endif
      }
  return 0;
}

//----------------------------------------------------------------------------
//Adjust tracker matrix to ultrasound scan depth
void vtkInstrumentTracker::AdjustMatrix(vtkMatrix4x4& matrix)
{
  double scaleFactor = (  this->calibReader->GetImageSize()[1]
                        / this->UltraSoundScanDepth * US_IMAGE_FAN_RATIO);

  matrix.Element[0][3] = matrix.Element[0][3] * scaleFactor;//x
  matrix.Element[1][3] = matrix.Element[1][3] * scaleFactor;//y
  matrix.Element[2][3] = matrix.Element[2][3] * scaleFactor;//z

}
/******************************************************************************
 * double vtkInstrumentTracker::GetUpTime()
 *
 *  Returns elapsed Time since program start
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 * ****************************************************************************/
double vtkInstrumentTracker::GetUpTime()
{
  return vtkTimerLog::GetUniversalTime() - this->GetStartUpTime();
}

/******************************************************************************
 * void vtkInstrumentTracker::SetLogStream(ofstream &LogStream)
 *
 *  Redirects Logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 * ****************************************************************************/
void vtkInstrumentTracker::SetLogStream(ofstream &LogStream)
{
  this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
  this->LogStream.precision(6);
  this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
 * ofstream& vtkInstrumentTracker::GetLogStream()
 *
 *  Returns logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  8.February 2009
 *
 *  @Return: Logstream
 *
 * ****************************************************************************/
ofstream& vtkInstrumentTracker::GetLogStream()
{
        return this->LogStream;
}
//----------------------------------------------------------------------------
//Generate Matrix
void vtkInstrumentTracker::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];

//  igtl::PrintMatrix(matrix);
}
