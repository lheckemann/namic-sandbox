/*=========================================================================

Module:  $RCSfile: vtkDataCollector.h,v $
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School

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

// .NAME vtkDataCollector - manage the vtk pipeline of SynchroGrab.
// .SECTION Description
// vtkDataCollector is responsible for the creation and configuration
// of the pipeline used by the application called SynchroGrab.
// Once, the pipeline is properly configured, it also manage the data collection.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef __vtkDataCollector_h
#define __vtkDataCollector_h

#include "SynchroGrabConfigure.h"

#include "vtkObject.h"

class vtkUltrasoundCalibFileReader;
class vtkTaggedImageFilter;
class vtkDataProcessor;
class vtkMultiThreader;

#ifdef USE_ULTRASOUND_DEVICE
class vtkV4L2VideoSource;
#else
class vtkVideoSourceSimulator;
#endif

class vtkNDITracker;
class vtkTrackerSimulator;
class vtkTrackerTool;

class vtkMatrix4x4;
class vtkImageData;

class vtkDataCollector : public vtkObject
{
public:
  static vtkDataCollector *New();
  vtkTypeRevisionMacro(vtkDataCollector, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(Verbose, bool);
  vtkGetMacro(Verbose, bool);

  vtkSetStringMacro(CalibrationFileName);
  vtkGetStringMacro(CalibrationFileName);
  
  vtkSetMacro(TrackerDeviceEnabled, bool);
  vtkGetMacro(TrackerDeviceEnabled, bool);

  vtkSetStringMacro(VideoDevice);
  vtkGetStringMacro(VideoDevice);

  vtkSetMacro(VideoChannel, int);
  vtkGetMacro(VideoChannel, int);

  vtkSetMacro(VideoMode, int);
  vtkGetMacro(VideoMode, int);

  vtkSetMacro(FrameRate, double);
  vtkGetMacro(FrameRate, double);

  //Depth of the Ultrasound scan in Millimeter
  vtkSetMacro(ScanDepth, double);
  vtkGetMacro(ScanDepth, double);

  vtkSetMacro(FrameBufferSize, int);
  vtkGetMacro(FrameBufferSize, int);

  vtkSetMacro(StartUpTime, double);
  vtkGetMacro(StartUpTime, double);
  
  vtkSetMacro(TrackerOffset, double);
  vtkGetMacro(TrackerOffset, double);

#ifdef USE_ULTRASOUND_DEVICE
  vtkGetMacro(VideoSource, vtkV4L2VideoSource *);
#else
  vtkGetMacro(VideoSource, vtkVideoSourceSimulator *);
#endif

  vtkGetMacro(Tagger, vtkTaggedImageFilter *);

  vtkGetMacro(DataProcessor, vtkDataProcessor *);

  vtkSetMacro(MaximumVolumeSize, double);
  double GetMaximumVolumeSize();
  
  void SetLogStream(ofstream &LogStream);
  ofstream& GetLogStream();

  int Initialize(vtkNDITracker* tracker);
  int StartCollecting(vtkDataProcessor * processor);
  int StopCollecting();
  int ProcessMatrix(struct DataStruct* pDataStruct);
  double GetUpTime();
  int DuplicateFrame(vtkImageData * original, vtkImageData * duplicate);
  bool IsTrackerDeviceEnabled();
  int EnableTrackerTool();
  bool IsIdentityMatrix(vtkMatrix4x4 * matrix);
  bool IsMatrixEmpty(vtkMatrix4x4 * matrix);
  int CalculateNormal(struct DataStruct * pDataStruct, double* normal);
  int CalculateVolumeProperties(struct DataStruct * pDataStruct);

protected:
  vtkDataCollector();
  ~vtkDataCollector();

  bool Verbose;
  double StartUpTime;
  ofstream LogStream;

  char *CalibrationFileName;
  vtkUltrasoundCalibFileReader *calibReader;
  double clipRectangle[4];
  
  char *VideoDevice; // e.g. /dev/video
  int   VideoChannel; // e.g. 3 at Hauppauge Impact VCB Modell 558
  int   VideoMode; //NTSC == 1 , PAL == 2
  double FrameRate;
  int FrameBufferSize;
  double TrackerOffset;//Offset of tracker to ultrasound probe; Unit: mm
  double MaximumVolumeSize;

  double ScanDepth;

  bool TrackerDeviceEnabled;
  vtkTrackerTool *tool;
  vtkNDITracker *NDItracker;
  vtkTrackerSimulator *trackerSimulator;

  vtkTaggedImageFilter *Tagger;

#ifdef USE_ULTRASOUND_DEVICE
  vtkV4L2VideoSource *VideoSource;
#else
  vtkVideoSourceSimulator *VideoSource;
#endif

  vtkDataProcessor* DataProcessor;

  //Multithreader to run a thread of collecting and sending data
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;

  bool Collecting;
  bool Initialized;

private:

  vtkDataCollector(const vtkDataCollector&);  // Not implemented.
  void operator=(const vtkDataCollector&);  // Not implemented.
};
#endif
