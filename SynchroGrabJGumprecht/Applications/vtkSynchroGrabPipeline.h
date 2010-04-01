/*=========================================================================

Module:  $RCSfile: vtkSynchroGrabPipeline.h,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Author: Nobuhiko Hata, Harvard Medical School
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

// .NAME vtkSynchroGrabPipeline - manage the vtk pipeline of SynchroGrab.
// .SECTION Description
// vtkSynchroGrabPipeline is responsible for the creation and configuration
// of the pipeline used by the application called SynchroGrab.
// Once, the pipeline is properly configured, it also manage the data collection.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef __vtkSynchroGrabPipeline_h
#define __vtkSynchroGrabPipeline_h

#include "SynchroGrabConfigure.h"

#include "vtkObject.h"
#include "vtkImageData.h"

#include "igtlClientSocket.h"
#include "igtlImageMessage.h"

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


class vtkSynchroGrabPipeline : public vtkObject
{
public:
  static vtkSynchroGrabPipeline *New();
  vtkTypeRevisionMacro(vtkSynchroGrabPipeline, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(NbFrames, int);
  vtkGetMacro(NbFrames, int);

  vtkSetStringMacro(SonixAddr);
  vtkGetStringMacro(SonixAddr);
  
  vtkSetStringMacro(VideoDevice);
  vtkGetStringMacro(VideoDevice);

  vtkSetStringMacro(CalibrationFileName);
  vtkGetStringMacro(CalibrationFileName);

  vtkSetStringMacro(OIGTLServer);
  vtkGetStringMacro(OIGTLServer);

  vtkSetMacro(TransfertImages, bool);
  vtkGetMacro(TransfertImages, bool);

  vtkSetMacro(VolumeReconstructionEnabled, bool);
  vtkGetMacro(VolumeReconstructionEnabled, bool);

  vtkSetMacro(ServerPort, int);
  vtkGetMacro(ServerPort, int);

  vtkSetMacro(UseTrackerTransforms, bool);
  vtkGetMacro(UseTrackerTransforms, bool);

  vtkSetMacro(FrameRate, double);
  vtkGetMacro(FrameRate, double);


  bool ReconstructVolume();
  bool ConfigurePipeline();
  bool ConnectToServer();
  bool CloseServerConnection();
  bool SendImages();
  void vtkGetRandomTestMatrix(igtl::Matrix4x4& matrix); 
  int vtkGetTestImage(igtl::ImageMessage::Pointer& msg);
  void FillImage();
  bool StartTracker();


protected:
  vtkSynchroGrabPipeline();
  ~vtkSynchroGrabPipeline();

  int ServerPort;
  int NbFrames;

  char *SonixAddr;
  char *CalibrationFileName;
  char *OIGTLServer;
  char *VideoDevice;

  bool TransfertImages;
  bool VolumeReconstructionEnabled;
  bool UseTrackerTransforms;

  double FrameRate;  
  
  //Buffers the 3D volume before it transfered via OpenIGTLink
  vtkImageData *transfer_buffer;  


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

  
  igtl::ClientSocket::Pointer socket;
  
 private:
  vtkSynchroGrabPipeline(const vtkSynchroGrabPipeline&);  // Not implemented.
  void operator=(const vtkSynchroGrabPipeline&);  // Not implemented.
};
#endif