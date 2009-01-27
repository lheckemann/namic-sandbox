/*=========================================================================

Module:  $RCSfile: vtkDataSender.h,v $
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

// .NAME vtkDataSender - send image to Slicer3 
// .SECTION Description
// vtkDataSender is responsible for the creation and configuration
// of socket connection with Slicer3, and as requested, sending images via OpenIGTLink.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef __vtkDataSender_h
#define __vtkDataSender_h

#include <queue>
#include <map>

#include "SynchroGrabConfigure.h"

#include "vtkObject.h"
#include "vtkImageData.h"

#include "igtlClientSocket.h"
#include "igtlImageMessage.h"

class vtkMultiThreader;
class vtMatrix4x4;

struct FramePoperties {
        bool Set;
        int ScalarType;
        int Size[3];
        double Spacing[3];      
        int SubVolumeSize[3];
        int SubVolumeOffset[3]; 
};

class vtkDataSender : public vtkObject
{
public:
  static vtkDataSender *New();
  vtkTypeRevisionMacro(vtkDataSender, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(Verbose, bool);
  vtkGetMacro(Verbose, bool);

  vtkSetStringMacro(OIGTLServer);
  vtkGetStringMacro(OIGTLServer);

  vtkSetMacro(ServerPort, int);
  vtkGetMacro(ServerPort, int);
  
  vtkSetMacro(SendRate, double);
  vtkGetMacro(SendRate, double);
  
  int GetHeadOfNewDataBuffer();
  bool IsNewDataBufferEmpty();
  bool IsNewDataBufferFull();
  int IncrementBufferIndex(int increment);

  int ConnectToServer();
  int CloseServerConnection();
  
  int StartSending();
  int StopSending();
  
  int NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix);
  
  void vtkGetRandomTestMatrix(igtl::Matrix4x4& matrix); 
  int vtkFillImageMessage(vtkImageData * ImageBuffer, igtl::ImageMessage::Pointer& msg);
  
  igtl::ImageMessage::Pointer PrepareImageMessage(int index);
  int SendMessage(igtl::ImageMessage::Pointer message);
  int DeleteData(int index);

protected:
  vtkDataSender();
  ~vtkDataSender();

  bool Verbose;

  int ServerPort;
  char *OIGTLServer;
  double SendRate;

  igtl::ClientSocket::Pointer socket;
  
  bool Sending;
  bool Connected;
  
  //Multithreader to run a thread of collecting and sending data
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;
  
  std::queue<int> newDataBuffer; //Stores index of incoming objects 
  int newDataBufferSize; //Maximum amount of items that can be stored at the same time
  int newDataBufferIndex; //Object which is currently/ was last processed
  
  std::map<int, vtkImageData*> newImageMap;
  std::map<int, vtkMatrix4x4*> newTrackerMatrixMap;  
  
  int AddFrameToFrameMap(int index, vtkImageData* frame);
  int AddMatrixToMatrixMap(int index, vtkMatrix4x4* matrix);
  
  struct FramePoperties frameProperties;
  
 private:
  vtkDataSender(const vtkDataSender&);  // Not implemented.
  void operator=(const vtkDataSender&);  // Not implemented.
};
#endif
