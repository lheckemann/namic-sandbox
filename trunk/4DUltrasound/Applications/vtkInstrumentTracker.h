/*=========================================================================

Module:  $RCSfile: vtkInstrumentTracker.h $
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School

Copyright (c) 2009, Brigham and Women's Hospital, Boston, MA
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

// .NAME vtkInstrumentTracker - send image to Slicer3
// .SECTION Description
// vtkInstrumentTracker is responsible for the creation and configuration
// of socket connection with Slicer3, and as requested, sending images via OpenIGTLink.
// .SECTION Usage
// See 4DUltrasound.cxx


#ifndef VTKINSTRUMENTTRACKER_H_
#define VTKINSTRUMENTTRACKER_H_

#include "SynchroGrabConfigure.h"

#include "vtkObject.h"

#include "igtlClientSocket.h"
#include "igtlTransformMessage.h"

class vtkTaggedImageFilter;
class vtkMultiThreader;
class vtkMatrix4x4;
class vtkTrackerTool;
class vtkUltrasoundCalibFileReader;

#ifdef USE_TRACKER_DEVICE
class vtkNDITracker;
#else
class vtkTrackerSimulator;
#endif

class vtkInstrumentTracker : public vtkObject
{
public:
  static vtkInstrumentTracker *New();
  vtkTypeRevisionMacro(vtkInstrumentTracker, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(Enabled, bool);
  vtkGetMacro(Enabled, bool);

  vtkSetMacro(Verbose, bool);
  vtkGetMacro(Verbose, bool);

  vtkSetMacro(StartUpTime, double);
  vtkGetMacro(StartUpTime, double);

  vtkSetStringMacro(CalibrationFileName);
  vtkGetStringMacro(CalibrationFileName);

  vtkSetStringMacro(OIGTLServer);
  vtkGetStringMacro(OIGTLServer);

  vtkSetMacro(ServerPort, int);
  vtkGetMacro(ServerPort, int);

  vtkSetMacro(TrackingRate, double);
  vtkGetMacro(TrackingRate, double);

  vtkSetMacro(UltraSoundScanDepth, double);
  vtkGetMacro(UltraSoundScanDepth, double);
  
  vtkSetMacro(TrackerTool, vtkTrackerTool*);
  vtkGetMacro(TrackerTool, vtkTrackerTool*);

  #ifdef USE_TRACKER_DEVICE
  vtkGetMacro(tracker, vtkNDITracker*);
  #else
  vtkGetMacro(tracker, vtkTrackerSimulator*);
# endif

  void SetLogStream(ofstream &LogStream);
  ofstream& GetLogStream();

  int ConnectToServer();
  int CloseServerConnection();

  int StartTracking();
  int StopTracking();

  int StartTracker();
  void AdjustMatrix(vtkMatrix4x4& matrix);

  int SendMessage(igtl::TransformMessage::Pointer& message);

  double GetUpTime();

  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

protected:
  vtkInstrumentTracker();
  ~vtkInstrumentTracker();

  bool Enabled;
  bool Verbose;
  double StartUpTime;
  ofstream LogStream;

  bool Connected;
  bool Tracking;

  double TrackingRate;
  int FrameBufferSize;
  double UltraSoundScanDepth;

  int ServerPort;
  char *OIGTLServer;
  igtl::ClientSocket::Pointer socket;
  char* CalibrationFileName;

  vtkUltrasoundCalibFileReader *calibReader;

  #ifdef USE_TRACKER_DEVICE
  vtkNDITracker *tracker;
  #else
  vtkTrackerSimulator *tracker;
  #endif

  vtkTrackerTool *TrackerTool;

  //Multithreader to run a thread of collecting and sending data
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;

};

#endif /* VTKINSTRUMENTTRACKER_H_ */
