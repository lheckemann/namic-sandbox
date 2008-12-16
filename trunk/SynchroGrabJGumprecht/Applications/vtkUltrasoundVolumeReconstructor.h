/*=========================================================================

Module:  $RCSfile: vtkUltrasoundVolumeReconstructor.h,v $
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

// .NAME vtkUltrasoundVolumeReconstructor - manage the vtk pipeline of SynchroGrab.
// .SECTION Description
// vtkUltrasoundVolumeReconstructor is responsible for the creation and configuration
// of the pipeline used by the application called SynchroGrab.
// Once, the pipeline is properly configured, it also manage the data collection.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef __vtkUltrasoundVolumeReconstructor_h
#define __vtkUltrasoundVolumeReconstructor_h

#include "SynchroGrabConfigure.h"

#include "vtkObject.h"
#include "vtkImageData.h"

class vtkUltrasoundCalibFileReader;
class vtkTaggedImageFilter;

#ifdef USE_ULTRASOUND_DEVICE
class vtkV4L2VideoSource;
#else
class vtkVideoSourceSimulator;
#endif

#ifdef USE_TRACKER_DEVICE
class vtkNDITracker;
#else
class vtkTrackerSimulator;
#endif

class vtkMatrix4x4;


class vtkUltrasoundVolumeReconstructor : public vtkObject
{
public:
  static vtkUltrasoundVolumeReconstructor *New();
  vtkTypeRevisionMacro(vtkUltrasoundVolumeReconstructor, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(VolumeReconstructionEnabled, bool);
  vtkGetMacro(VolumeReconstructionEnabled, bool);

  vtkSetStringMacro(CalibrationFileName);
  vtkGetStringMacro(CalibrationFileName);

  vtkSetMacro(NbFrames, int);
  vtkGetMacro(NbFrames, int);

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

  bool ReconstructVolume(vtkImageData * ImageBuffer);
  bool StartTracker();
  bool ConfigurePipeline();

protected:
  vtkUltrasoundVolumeReconstructor();
  ~vtkUltrasoundVolumeReconstructor();

  int NbFrames; //Number of frames to grab

  char *CalibrationFileName;

  char *VideoDevice; // e.g. /dev/video
  int   VideoChannel; // e.g. 3 at Hauppauge Impact VCB Modell 558
  int   VideoMode; //NTSC == 1 , PAL == 2
  double FrameRate;  

  bool VolumeReconstructionEnabled;

  double ScanDepth;
  
  vtkUltrasoundCalibFileReader *calibReader;
  
#ifdef USE_TRACKER_DEVICE
  vtkNDITracker *tracker;  
#else
  vtkTrackerSimulator *tracker;  
#endif

  vtkTaggedImageFilter *tagger;

#ifdef USE_ULTRASOUND_DEVICE
  vtkV4L2VideoSource *sonixGrabber;  
#else
  vtkVideoSourceSimulator *sonixGrabber;
#endif

  void AdjustMatrix(vtkMatrix4x4& matrix);

private:

  vtkUltrasoundVolumeReconstructor(const vtkUltrasoundVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkUltrasoundVolumeReconstructor&);  // Not implemented.
};
#endif
