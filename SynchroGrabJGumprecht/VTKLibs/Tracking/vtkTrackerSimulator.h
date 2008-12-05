/*=========================================================================

  Module:    $RCSfile: vtkTrackerSimulator.h,v $
  Creator:   David Gobbi <dgobbi@cs.queensu.ca>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2008/06/18 21:59:29 $
  Version:   $Revision: 1.3 $


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

// .NAME vtkTrackerSimulator - VTK interface for the NDI Optotrak Certus
// .SECTION Description
// The vtkTrackerSimulator class provides an interface to the Optotrak
// Certus (Northern Digital Inc., Waterloo, Canada), utilizing the PCI
// interface card.
// .SECTION see also
// vtkNDITracker vtkTrackerTool


#ifndef __vtkTrackerSimulator_h
#define __vtkTrackerSimulator_h

#define NEW_TRACKER_SIMULATOR
//#undef NEW_TRACKER_SIMULATOR

#include "vtkTrackingWin32Header.h"
#include "vtkTracker.h"
#include "igtlMath.h"

#define MATRICES_PER_SECOND 20

class vtkMultiThreader;

// the number of tools this class can handle
#define VTK_TRACKER_NTOOLS 1

class VTK_TRACKING_EXPORT vtkTrackerSimulator : public vtkTracker
{
public:

  static vtkTrackerSimulator *New();
  vtkTypeMacro(vtkTrackerSimulator,vtkTracker);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  // Description:
  // Probe to check whether there is an attached Certus system that
  // is able to track.  After Probe is called, you can call
  // GetVersion() to get information about the attached Certus system.
  int Probe();

  // Description:
  // Get the a string (perhaps a long one) describing the type and version
  // of the device.
  vtkGetStringMacro(Version);

  // Description:
  // Get an update from the tracking system and push the new transforms
  // to the tools.  This should only be used within vtkTracker.cxx.
  void InternalUpdate();  
  void Update();

  void StartTracking();
  void StopTracking();

protected:
  vtkTrackerSimulator();
  ~vtkTrackerSimulator();

  // Description:
  // Set the version information.
  vtkSetStringMacro(Version);

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state into full tracking mode.  The device will
  // only be reset if communication cannot be established without
  // a reset.
  int InternalStartTracking();

  // Description:
  // Stop the tracking system and bring it back to its ground state:
  // Initialized, not tracking, at 9600 Baud.
  int InternalStopTracking();

  // Description:
  // Methods for detecting which ports have tools in them, and
  // auto-enabling those tools.
  int EnableToolPorts();
  int DisableToolPorts();

  char *Version;

  int NumberOfRigidBodies;

  vtkMatrix4x4 *SendMatrix;
  
  //Counts the amount of matrixes which have already been generated
  int matrixCounter;  
  
  int IsDeviceTracking;

  int PortEnabled[VTK_TRACKER_NTOOLS];

  void GenerateTrackerMatrix(vtkMatrix4x4& matrix, int zPosition);
  
  //Multithreader to run a thread for matrix recording
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;

private:
  vtkTrackerSimulator(const vtkTrackerSimulator&);
  void operator=(const vtkTrackerSimulator&);  
};

#endif





