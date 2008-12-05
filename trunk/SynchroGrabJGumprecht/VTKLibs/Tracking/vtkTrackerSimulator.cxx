/*=========================================================================

  Module:    $RCSfile: vtkTrackerSimulator.cxx,v $
  Creator:   David Gobbi <dgobbi@cs.queensu.ca>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2008/06/18 21:59:29 $
  Version:   $Revision: 1.5 $

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
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

// turn this on to print lots of debug information
//#define VTK_CERTUS_DEBUG_STATEMENTS 0
// turn this on to turn of multithreading
//#define VTK_CERTUS_NO_THREADING 0

//----------------------------------------------------------------------------
// map values 0, 1, 2 to the proper Certus VLED state constant 
//static VLEDState vtkNDICertusMapVLEDState[] = {
//  VLEDST_OFF, VLEDST_ON, VLEDST_BLINK };

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
//  this->NumberOfMarkers = 0;
  this->NumberOfRigidBodies = 0;
  this->SetNumberOfTools(VTK_CERTUS_NTOOLS);

  for (int i = 0; i < VTK_CERTUS_NTOOLS; i++)
    {
//    this->PortHandle[i] = 0;
    this->PortEnabled[i] = 0;
    }

  // for accurate timing
  this->Timer = vtkFrameToTimeConverter::New();
  this->Timer->SetNominalFrequency(60.0);
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
  if (this->Timer)
    {
    this->Timer->Delete();
    }
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

//#define vtkPrintCertusErrorMacro() \
//{ \
//  if (OptotrakGetErrorString(vtkCertusErrorString, \
//                             MAX_ERROR_STRING_LENGTH+1) == 0) \
//    { \
//    vtkErrorMacro(<< vtkCertusErrorString); \
//    } \
//} 

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

//----------------------------------------------------------------------------
void vtkTrackerSimulator::StartTracking()
{
//#if VTK_CERTUS_NO_THREADING
  this->Tracking = this->InternalStartTracking();
//#else
//  this->vtkTracker::StartTracking();
//#endif
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

  // For accurate timing
  this->Timer->Initialize();

  this->IsDeviceTracking = 1;
  
  this->InternalUpdate();
  
  this->vtkTracker::Update();

  return 1;
}

//----------------------------------------------------------------------------
void vtkTrackerSimulator::StopTracking()
{
//#if VTK_sCERTUS_NO_THREADING
  this->InternalStopTracking();
  this->Tracking = 0;
//#else
//  this->vtkTracker::StopTracking();
//#endif
}

//----------------------------------------------------------------------------
int vtkTrackerSimulator::InternalStopTracking()
{
  this->IsDeviceTracking = 0;

  return 1;
}

//----------------------------------------------------------------------------
void vtkTrackerSimulator::Update()
{
//#if VTK_CERTUS_NO_THREADING
  if (this->Tracking)
    {
    this->InternalUpdate();
    }
//#endif

  this->vtkTracker::Update();
}

//----------------------------------------------------------------------------
void vtkTrackerSimulator::InternalUpdate()
{
//  int tool;
  int missing[VTK_CERTUS_NTOOLS];
  long statusFlags[VTK_CERTUS_NTOOLS];
  double transform[VTK_CERTUS_NTOOLS][8];
  double *referenceTransform = 0;

  if (!this->IsDeviceTracking)
    {
    vtkWarningMacro( << "called Update() when Certus was not tracking");
    return;
    }

  // initialize transformations to identity
  for (int tool = 0; tool < VTK_CERTUS_NTOOLS; tool++)
    {
    missing[tool] = 1; 

    transform[tool][0] = 1.0;
    transform[tool][1] = transform[tool][2] = transform[tool][3] = 0.0;
    transform[tool][4] = transform[tool][5] = transform[tool][6] = 0.0;
    transform[tool][7] = 0.0;
    }

  double timestamp = vtkTimerLog::GetUniversalTime(); //New not original

  for (int tool = 0; tool < VTK_CERTUS_NTOOLS; tool++) 
    {
    // convert status flags from Optotrak format to vtkTracker format
    int flags = 0;

    int limit = 101;

    for (int index = 1; index < limit; ++index)
      { 
        
      this->SendMatrix->Identity();

      GetRandomTestMatrix(*this->SendMatrix, index);

      // send the matrix and flags to the tool's vtkTrackerBuffer
//      this->ToolUpdate(tool, this->SendMatrix, flags, timestamp);
      this->ToolUpdate(tool, this->SendMatrix, flags, index);
      }
    }
}

//----------------------------------------------------------------------------
// Enable all tool ports that have tools plugged into them.
// The reference port is enabled with NDI_STATIC.
int vtkTrackerSimulator::EnableToolPorts()
{
  int toolCounter = 0;

  // reset our information about the tool ports
  for (toolCounter = 0; toolCounter < VTK_CERTUS_NTOOLS; toolCounter++)
    {
    this->PortEnabled[toolCounter] = 0;
    }
    
    this->NumberOfRigidBodies = 0; 
    
    int port = 0;

   //JG   
   char deviceName[128 + 1];
   sprintf(deviceName, "vtkTrackerSimulator");
  // add rigid bodies
  for (toolCounter = 0; toolCounter < VTK_CERTUS_NTOOLS; toolCounter++)
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
  for (int toolCounter = 0; toolCounter < VTK_CERTUS_NTOOLS; toolCounter++)
    {
    this->PortEnabled[toolCounter] = 0;
    }
  return 1;
}

void vtkTrackerSimulator::GetRandomTestMatrix(vtkMatrix4x4& matrix, int index)
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
//  static float theta = 0.0;
//  orientation[0]=0.0;
//  orientation[1]=0.6666666666*cos(theta);
//  orientation[2]=0.577350269189626;
//  orientation[3]=0.6666666666*sin(theta);
//  theta = theta + 0.1;
//
//  igtl::Matrix4x4 igtlMatrix;

//  for(int i = 0; i < 4; ++i)
//    {
//    for(int j = 0; j < 4; ++j)
//      {
//      igtlMatrix[i][j] = matrix[i][j]; 
//      }
//    }

  //igtl::Matrix4x4 matrix;
//  igtl::QuaternionToMatrix(orientation, igtlMatrix);

//  igtl::PrintMatrix(igtlMatrix);
  
//  for(int i = 0; i < 4; ++i)
//    {
//    for(int j = 0; j < 4; ++j)
//      {
//      matrix[i][j] = igtlMatrix[i][j];  
//      }
//    }
//  matrix[0][3] = position[0];
//  matrix[1][3] = position[1];
//  matrix[2][3] = position[2];

  matrix.Identity();
  
  matrix.SetElement(0, 3, 1);
  matrix.SetElement(1, 3, 1);
  matrix.SetElement(2, 3, index);

}


