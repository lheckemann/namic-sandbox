/*=========================================================================

Module:    $RCSfile: vtkUltrasoundVolumeSender.cxx,v $
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

#include <iostream>
#include <sstream>
#include <limits>
#include <string>

#define NOMINMAX
#undef REMOVE_ALPHA_CHANNEL
 
//#include <windows.h>

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"

#include "vtkObjectFactory.h"
#include "vtkUltrasoundVolumeSender.h"

#include "vtkTaggedImageFilter.h"
#include "vtkTransform.h"
#include "vtkVideoSourceSimulator.h"

#include "igtlImageMessage.h"
#include "igtlMath.h"
#include "igtlOSUtil.h"

#define FUDGE_FACTOR 1.6
#define CERTUS_UPDATE_RATE 625

using namespace std;

vtkCxxRevisionMacro(vtkUltrasoundVolumeSender, "$Revision$");
vtkStandardNewMacro(vtkUltrasoundVolumeSender);

//----------------------------------------------------------------------------
vtkUltrasoundVolumeSender::vtkUltrasoundVolumeSender()
{
  this->ServerPort = 18944;
  this->OIGTLServer = NULL; 
  this->SetOIGTLServer("localhost");

  this->socket = NULL;
  this->socket = igtl::ClientSocket::New();

  this->Verbose = false;
}

//----------------------------------------------------------------------------
vtkUltrasoundVolumeSender::~vtkUltrasoundVolumeSender()
{

  this->SetOIGTLServer(NULL);
}


//----------------------------------------------------------------------------
void vtkUltrasoundVolumeSender::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeSender::ConnectToServer()
{
  // Opening an OpenIGTLink socket
  int e = this->socket->ConnectToServer(OIGTLServer, ServerPort);

  if(e != 0)
    {
    cout << " ERROR: Failed to connect the OpenIGTLink socket to the server ("<< OIGTLServer <<":"<<  ServerPort << ") Error code : " << e << endl;
    return false;
    }
  else if(Verbose)
    {
    cout << "Successful connection to the OpenIGTLink server ("<< OIGTLServer <<":"<<  ServerPort << ")" << endl;
    }  

  return true;
}

//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeSender::CloseServerConnection()
{
  this->socket->CloseSocket();
  return true;
}



//----------------------------------------------------------------------------
bool vtkUltrasoundVolumeSender::SendImages(vtkImageData * ImageBuffer, int repetitions)
{

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < repetitions; i ++)
    { 

    //------------------------------------------------------------
    // Get Image data
    igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
    
    vtkFillImageMessage(ImageBuffer, imgMsg);

    //------------------------------------------------------------
    // Get random orientation matrix and set it.
    igtl::Matrix4x4 matrix;
    vtkGetRandomTestMatrix(matrix);
    imgMsg->SetMatrix(matrix);

    //------------------------------------------------------------
    // Pack (serialize) and send
    imgMsg->Pack();
    
    int ret = this->socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
    if (ret == 0)
      {
      std::cerr << "ERROR: Connection Lost to OpenIGTLink Server while sending!\n";
      return false;
      }
    else
      {
      cout << "3D Volume successfully send to OpenIGTLink Server" << endl;
      }     
    
      //int sleeptime = (int) (1000.0 / this->FrameRate); 
      //igtl::Sleep(sleeptime); // wait    
      igtl::Sleep(33); // wait          
    }    
  
  return true;

}

//------------------------------------------------------------
// Function to get image data
int vtkUltrasoundVolumeSender::vtkFillImageMessage(vtkImageData *ImageBuffer, igtl::ImageMessage::Pointer& msg)
{
  int size[3];      // image dimension
  double spacing[3]; // spacing (mm/pixel)
  int svsize[3];    // sub-volume size
  int svoffset[3];  // sub-volume offset
  int scalarType;   // scalar type

  //If we reconstructing a 3DVolume everything is already set for us
    ImageBuffer->GetDimensions(size);
    ImageBuffer->GetSpacing(spacing);
    //spacing[2] *= 1;

    //For a fast Computer enable this
    //spacing[0] *= 3;
    //spacing[1] *= 3;
    //spacing[2] *= 3;
    
    svsize[0]   = size[0];       
    svsize[1]   = size[1];       
    svsize[2]   = size[2];           
    svoffset[0] = svoffset[1] = svoffset[2] = 0;           
    scalarType = ImageBuffer->GetScalarType();    
  
  //------------------------------------------------------------
  // Create a new IMAGE type message    
  msg->SetDimensions(size);
  msg->SetSpacing((float) spacing[0],(float) spacing[1], (float) spacing[2]);
  msg->SetScalarType(scalarType);
  msg->SetDeviceName("SynchroGrab");
  msg->SetSubVolume(svsize, svoffset);
  msg->AllocateScalars();
  
  if(Verbose)
    {
    cerr <<    "3D Volume Size:" << size[0] << " " << size[1] << " " << size[2] << endl;
    }

  unsigned char * p_msg = (unsigned char*) msg->GetScalarPointer();
  unsigned char * p_ibuffer = (unsigned char*) ImageBuffer->GetScalarPointer();

  for(int i=0 ; i < size[0] * size[1] * size[2] ; i++ )
    {
    *p_msg = (unsigned char) * p_ibuffer;
    p_ibuffer++; p_msg++;
    }

  return 1;
}

//------------------------------------------------------------
// Function to generate random matrix.
void vtkUltrasoundVolumeSender::vtkGetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];  
  
 #ifdef USE_TRACKER_DEVICE
 //NDI tracker matrix looks like
  //  0  0 -1  0
  // -1  0  0  0
  //  0  1  0  0
  //  0  0  0  1
  matrix[0][0] =   0.0;  matrix[1][0] =  0.0;  matrix[2][0] =  1.0; matrix[3][0] = 0.0;
  matrix[0][1] =   0.0;  matrix[1][1] =  1.0;  matrix[2][1] =  0.0; matrix[3][1] = 0.0;
  matrix[0][2] =  -1.0;  matrix[1][2] =  0.0;  matrix[2][2] =  0.0; matrix[3][2] = 0.0;
  matrix[0][3] =  90.0;  matrix[1][3] =  0.0;  matrix[2][3] =  0.0; matrix[3][3] = 1.0;
#else
  //Tracker simulator matrix looks like
  //  1  0  0  0
  //  0  1  0  0
  //  0  0  1  0
  //  0  0  0  1
  matrix[0][0] =  -1.0;  matrix[1][0] =  0.0;  matrix[2][0] =  0.0; matrix[3][0] = 0.0;
  matrix[0][1] =   0.0;  matrix[1][1] =  0.0;  matrix[2][1] =  1.0; matrix[3][1] = 0.0;
  matrix[0][2] =   0.0;  matrix[1][2] =  1.0;  matrix[2][2] =  0.0; matrix[3][2] = 0.0;
  matrix[0][3] =   0.0;  matrix[1][3] =  0.0;  matrix[2][3] =  0.0; matrix[3][3] = 1.0;
#endif

  if(Verbose)
    {
    cout << "OpenIGTLink image message matrix" << endl;
    igtl::PrintMatrix(matrix); 
    }
  
}
