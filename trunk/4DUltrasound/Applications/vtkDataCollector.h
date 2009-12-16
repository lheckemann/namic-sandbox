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

#include "4DUltrasoundConfigure.h"

#include "vtkObject.h"
#include "vtkDataProcessor.h"

class vtkUltrasoundCalibFileReader;
class vtkTaggedImageFilter;
class vtkMultiThreader;

class vtkV4L2VideoSource;
class vtkVideoSourceSimulator;

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
  //vtkSetMacro(UltrasoundScanDepth, double);
  //vtkGetMacro(UltrasoundScanDepth, double);

  vtkSetMacro(FrameBufferSize, int);
  vtkGetMacro(FrameBufferSize, int);

  vtkSetMacro(StartUpTime, double);
  vtkGetMacro(StartUpTime, double);

  vtkSetVector3Macro(TrackerOffset, double);
  vtkGetVector3Macro(TrackerOffset, double);

  vtkGetVector3Macro(ShrinkFactor, int);

//#ifdef USE_ULTRASOUND_DEVICE
  vtkGetMacro(V4L2VideoSource, vtkV4L2VideoSource *);
//#else
  vtkGetMacro(VideoSourceSimulator, vtkVideoSourceSimulator *);
//#endif

  vtkGetMacro(Tagger, vtkTaggedImageFilter *);

  vtkGetMacro(DataProcessor, vtkDataProcessor *);

  vtkSetVector3Macro(MaximumVolumeSize, double);
  double GetMaximumVolumeSize();

  void SetLogStream(ofstream &LogStream);
  ofstream& GetLogStream();

  vtkSetVector3Macro(SystemOffset, double);
  vtkGetVector3Macro(SystemOffset, double);

  vtkSetMacro(DynamicVolumeSize, bool);
  vtkGetMacro(DynamicVolumeSize, bool);

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  vtkGetMacro(FrameGrabbingEnabled, bool);

  vtkSetMacro(EnableFrameGrabbing, bool);
  vtkGetMacro(EnableFrameGrabbing, bool);

  vtkSetMacro(CollectCalibrationData, bool);
  vtkGetMacro(CollectCalibrationData, bool);

  vtkGetMacro(OrientationAtCalibrationMatrix, vtkMatrix4x4 *);

  vtkGetMacro(OldCoordinates, double *);

  vtkGetMacro(PositionCorrectionFactor, double)

  int Initialize(vtkNDITracker* tracker);
  int StartCollecting(vtkDataProcessor * processor);
  int StopCollecting();
  int CalibrateMatrix(struct DataStruct* pDataStruct);
  double GetUpTime();
  int ExtractImage(vtkImageData * original, vtkImageData * duplicate);
  int DuplicateFrame(vtkImageData * original, vtkImageData * duplicate);
  bool IsTrackerDeviceEnabled();
  int EnableTrackerTool();
  bool IsIdentityMatrix(vtkMatrix4x4 * matrix);
  bool IsMatrixEmpty(vtkMatrix4x4 * matrix);
  int CalculateVolumeProperties(struct DataStruct* pDataStruct);
  int GrabOneImage();

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
  double TrackerOffset[3];//Offset of tracker to ultrasound probe; Unit: mm
  double MaximumVolumeSize[3];
  double SystemOffset[3];
  struct DataStruct FixedVolumeProperties;
  bool DynamicVolumeSize;
  bool VolumeInitialized;
  bool FrameGrabbingEnabled;
  bool EnableFrameGrabbing;
  bool CollectCalibrationData;

  vtkMatrix4x4 *TrackerCalibrationMatrix;
  vtkMatrix4x4 *OrientationAtCalibrationMatrix;
  double OldCoordinates[3];
  bool CoordinateJump(vtkMatrix4x4 * matrix);

  int ImageMargin[4];
  //double UltrasoundScanDepth;
  int ShrinkFactor[3];
  double TransformationFactorMmToPixel;
  double PositionCorrectionFactor;

  bool TrackerDeviceEnabled;
  vtkTrackerTool *tool;
  vtkNDITracker *NDItracker;
  vtkTrackerSimulator *trackerSimulator;

  vtkTaggedImageFilter *Tagger;

  vtkV4L2VideoSource *V4L2VideoSource;
  vtkVideoSourceSimulator *VideoSourceSimulator;

  vtkDataProcessor* DataProcessor;

  //Multithreader to run a thread of collecting and sending data
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;

  bool Collecting;
  bool Initialized;
  char* FileName;

  int StartV4L2VideoSource();

private:

  vtkDataCollector(const vtkDataCollector&);  // Not implemented.
  void operator=(const vtkDataCollector&);  // Not implemented.
};
#endif
