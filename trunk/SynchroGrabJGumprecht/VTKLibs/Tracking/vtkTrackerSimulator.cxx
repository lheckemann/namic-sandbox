/*=========================================================================

Module:    $RCSfile: vtkTrackerSimulator.cxx,v $
Author:  Jan Gumprecht, Harvard Medical School

Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA
All rights reserved.

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

#define MAX_ERROR_STRING_LENGTH 10000
#define DH_STATUS_ENABLED 1

#include <limits.h>
#include <float.h>
#include <math.h>
#include <cassert>
#include <ctype.h>

// NDI header files require this on Windows
#if defined(_WIN32) && !defined(__WINDOWS_H)
#define __WINDOWS_H
#include "Windows.h"
#endif

//// requires NDIoapi/ndlib
//#include "ndtypes.h"
//#include "ndpack.h"
//#include "ndopto.h"

// requires Atamai ndicapi
//#include "ndicapi_math.h"

#include "vtkTimerLog.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTrackerSimulator.h"
#include "vtkTrackerTool.h"
#include "vtkFrameToTimeConverter.h"
#include "vtkObjectFactory.h"

#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"

//----------------------------------------------------------------------------
vtkTrackerSimulator* vtkTrackerSimulator::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTrackerSimulator");
  if(ret)
    {
    return (vtkTrackerSimulator*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTrackerSimulator;
}

//----------------------------------------------------------------------------
vtkTrackerSimulator::vtkTrackerSimulator()
{
  this->Version = NULL;
  this->SendMatrix = vtkMatrix4x4::New();
  this->IsDeviceTracking = 0;

  this->matrixCounter;

  this->NumberOfRigidBodies = 0;
  this->SetNumberOfTools(VTK_TRACKER_NTOOLS);

  for (int i = 0; i < VTK_TRACKER_NTOOLS; i++)
    {
//    this->PortHandle[i] = 0;
    this->PortEnabled[i] = 0;
    }
  
#ifdef NEW_TRACKER_SIMULATOR
 
  this->PlayerThreader = vtkMultiThreader::New();
  this->PlayerThreadId = -1;
 
#endif
  
}

//----------------------------------------------------------------------------
vtkTrackerSimulator::~vtkTrackerSimulator() 
{
  if (this->Tracking)
    {
    this->StopTracking();
    }
  this->SendMatrix->Delete();
  if (this->Version)
    {
    delete [] this->Version;
    }
#ifdef NEW_TRACKER_SIMULATOR    
  this->PlayerThreader->Delete();
#endif
}
  
//----------------------------------------------------------------------------
void vtkTrackerSimulator::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkTracker::PrintSelf(os,indent);

  os << indent << "SendMatrix: " << this->SendMatrix << "\n";
  this->SendMatrix->PrintSelf(os,indent.GetNextIndent());
  os << indent << "NumberOfRigidBodies: " << this->NumberOfRigidBodies << "\n";
//  os << indent << "NumberOfMarkers: " << this->NumberOfMarkers << "\n";
}
  
//----------------------------------------------------------------------------
static char vtkCertusErrorString[MAX_ERROR_STRING_LENGTH + 1];

#if VTK_CERTUS_DEBUG_STATEMENTS
#define vtkCertusDebugMacro(t) \
{ \
  cerr << "vtkTrackerSimulator.cxx:" << __LINE__ << " " t << "\n"; \
}
#else
#define vtkCertusDebugMacro(t)
#endif

//----------------------------------------------------------------------------
int vtkTrackerSimulator::Probe()
{
  return 1;
}

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

//----------------------------------------------------------------------------
// Sleep until the specified absolute time has arrived.
// You must pass a handle to the current thread.  
// If '0' is returned, then the thread was aborted before or during the wait.
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
          vtkGenericWarningMacro("Dropped a tracker matrix.");
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

//----------------------------------------------------------------------------
// this function runs in an alternate thread to asyncronously generate matrices
static void *vtkTrackerSimulatorRecordThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkTrackerSimulator *self = (vtkTrackerSimulator *)(data->UserData);
  
  double startTime = vtkTimerLog::GetUniversalTime();
  double matrix = 1;

  do
    {
      self->InternalUpdate();
      matrix++;
    }    
  while (vtkThreadSleep(data, startTime + matrix/MATRICES_PER_SECOND));

  return NULL;
}

//----------------------------------------------------------------------------
void vtkTrackerSimulator::StartTracking()
{
  
  this->Tracking = this->InternalStartTracking();
  
}

//----------------------------------------------------------------------------
int vtkTrackerSimulator::InternalStartTracking()
{
  if (this->IsDeviceTracking)
    {
    return 1;
    }

  // Attempt to initialize the Certus
  // and enable all the tools
  if (!this->EnableToolPorts())
    {
    return 0;
    }

  this->IsDeviceTracking = 1;

#ifdef NEW_TRACKER_SIMULATOR

  this->PlayerThreadId =  
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkTrackerSimulatorRecordThread,this);
            
#else
  
  this->InternalUpdate();

#endif
  
  this->vtkTracker::Update();

  return 1;
}

//----------------------------------------------------------------------------
void vtkTrackerSimulator::StopTracking()
{
  this->InternalStopTracking();
  this->Tracking = 0;
}

//----------------------------------------------------------------------------
int vtkTrackerSimulator::InternalStopTracking()
{
  if(this->IsDeviceTracking)
    {
    this->IsDeviceTracking = 0;

#ifdef NEW_TRACKER_SIMULATOR

    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    
#endif    
      
    }
  

  return 1;
}

//----------------------------------------------------------------------------
void vtkTrackerSimulator::Update()
{
  if (this->Tracking)
    {
    this->InternalUpdate();
    }

  this->vtkTracker::Update();
}

//----------------------------------------------------------------------------
void vtkTrackerSimulator::InternalUpdate()
{
  int missing[VTK_TRACKER_NTOOLS];
  double transform[VTK_TRACKER_NTOOLS][8];

  if (!this->IsDeviceTracking)
    {
    vtkWarningMacro( << "called Update() when Certus was not tracking");
    return;
    }

  // initialize transformations to identity
  for (int tool = 0; tool < VTK_TRACKER_NTOOLS; tool++)
    {
    missing[tool] = 1; 

    transform[tool][0] = 1.0;
    transform[tool][1] = transform[tool][2] = transform[tool][3] = 0.0;
    transform[tool][4] = transform[tool][5] = transform[tool][6] = 0.0;
    transform[tool][7] = 0.0;
    }

#ifdef NEW_TRACKER_SIMULATOR
  double timestamp = vtkTimerLog::GetUniversalTime(); //New not original
#endif

  for (int tool = 0; tool < VTK_TRACKER_NTOOLS; tool++) 
    {
      
    int flags = 0;
#ifdef NEW_TRACKER_SIMULATOR

      this->SendMatrix->Identity();

      GenerateTrackerMatrix(*this->SendMatrix, this->matrixCounter);

      // send the matrix and flags to the tool's vtkTrackerBuffer
      this->ToolUpdate(tool, this->SendMatrix, flags, timestamp);
      
#else

    int limit = 101;

    for (int time = 1; time < limit; ++time)
      { 

      this->SendMatrix->Identity();

      GenerateTrackerMatrix(*this->SendMatrix, time);
      
      // send the matrix and flags to the tool's vtkTrackerBuffer
      this->ToolUpdate(tool, this->SendMatrix, flags, index);
      
      }
      
#endif

    }
    
    ++this->matrixCounter;

}

//----------------------------------------------------------------------------
// Enable all tool ports that have tools plugged into them.
// The reference port is enabled with NDI_STATIC.
int vtkTrackerSimulator::EnableToolPorts()
{
  int toolCounter = 0;

  // reset our information about the tool ports
  for (toolCounter = 0; toolCounter < VTK_TRACKER_NTOOLS; toolCounter++)
    {
    this->PortEnabled[toolCounter] = 0;
    }
    
    this->NumberOfRigidBodies = 0; 
    
    int port = 0;

   //JG   
   char deviceName[128 + 1];
   sprintf(deviceName, "vtkTrackerSimulator");
  // add rigid bodies
  for (toolCounter = 0; toolCounter < VTK_TRACKER_NTOOLS; toolCounter++)
    {
      //JG Start
      this->PortEnabled[toolCounter] = DH_STATUS_ENABLED;
      this->Tools[toolCounter]->SetToolManufacturer(deviceName);
      this->Tools[toolCounter]->SetToolPartNumber(deviceName);
      //JG End
      
    if (this->PortEnabled[toolCounter])
      {
        this->NumberOfRigidBodies++;

      }
    }

  return 1;
}

//----------------------------------------------------------------------------
// Disable all enabled tool ports.
int vtkTrackerSimulator::DisableToolPorts()
{
   // disable the enabled ports
  for (int toolCounter = 0; toolCounter < VTK_TRACKER_NTOOLS; toolCounter++)
    {
    this->PortEnabled[toolCounter] = 0;
    }
  return 1;
}

void vtkTrackerSimulator::GenerateTrackerMatrix(vtkMatrix4x4& matrix, int index)
{
  float position[3];
  float orientation[4];

  matrix.Identity();
  // random orientation
  float theta = index/100*3.14;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  
  
  matrix[0][3] = 0;
  matrix[1][3] = 0;
  matrix[2][3] = index / 2.0;

  // matrix.Identity();
  //  matrix.SetElement(0, 2, orientation[1]);
  //matrix.SetElement(1, 2, orientation[2]);
  //matrix.SetElement(2, 2, orientation[3]);

}


