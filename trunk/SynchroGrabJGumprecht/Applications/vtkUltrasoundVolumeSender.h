/*=========================================================================

Module:  $RCSfile: vtkUltrasoundVolumeSender.h,v $
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

// .NAME vtkUltrasoundVolumeSender - send image to Slicer3 
// .SECTION Description
// vtkUltrasoundVolumeSender is responsible for the creation and configuration
// of socket connection with Slicer3, and as requested, sending images via OpenIGTLink.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef __vtkUltrasoundVolumeSender_h
#define __vtkUltrasoundVolumeSender_h

#include "SynchroGrabConfigure.h"

#include "vtkObject.h"
#include "vtkImageData.h"

#include "igtlClientSocket.h"
#include "igtlImageMessage.h"


class vtkUltrasoundVolumeSender : public vtkObject
{
public:
  static vtkUltrasoundVolumeSender *New();
  vtkTypeRevisionMacro(vtkUltrasoundVolumeSender, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(OIGTLServer);
  vtkGetStringMacro(OIGTLServer);

  vtkSetMacro(ServerPort, int);
  vtkGetMacro(ServerPort, int);

  bool ConnectToServer();
  bool CloseServerConnection();
  bool SendImages(vtkImageData * ImageBuffer, int repetitions);
  void vtkGetRandomTestMatrix(igtl::Matrix4x4& matrix); 
  int vtkFillImageMessage(vtkImageData * ImageBuffer, igtl::ImageMessage::Pointer& msg);


protected:
  vtkUltrasoundVolumeSender();
  ~vtkUltrasoundVolumeSender();

  int ServerPort;
  char *OIGTLServer;

  igtl::ClientSocket::Pointer socket;
  
 private:
  vtkUltrasoundVolumeSender(const vtkUltrasoundVolumeSender&);  // Not implemented.
  void operator=(const vtkUltrasoundVolumeSender&);  // Not implemented.
};
#endif
